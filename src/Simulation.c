/*
 * Simulation.c
 * ------------
 * Synchronous SIR state-update engine with viral mutation and interventions.
 * See Simulation.h for interface documentation.
 */

#include "Simulation.h"


/* ── Mutation helper ──────────────────────────────────────────────────────── */

/* Perturbs value x by a uniform random amount in [-0.1, 0.1], clamped to [0,1].
 * Used to slightly mutate transmission (beta) and recovery (gamma) rates. */
double generate_new(double x) {
    double max_variation = 0.1;
    double change = ((rand_double() * 2) - 1) * max_variation;

    if      (x + change > 1.0) return 1.0;
    else if (x + change < 0.0) return 0.0;
    else                       return x + change;
}


/* ── Core update ──────────────────────────────────────────────────────────── */

/* Computes the next time step for all individuals synchronously.
 * Uses double-buffering to prevent cascading updates within the same tick. */
int update_state(Graph* graph, tree* virus_tree, double teta) {

    if (graph == NULL || virus_tree == NULL || teta < 0.0 || teta > 1.0)
        return -1;

    State* new_states   = malloc(graph->n * sizeof(State));
    if (new_states == NULL) return 0;

    int* new_variants = malloc(graph->n * sizeof(int));
    if (new_variants == NULL) { free(new_states); return 0; }

    for (int i = 0; i < graph->n; i++) {

        /* ── I → R (Recovery Check) ─────────────────────────────────────── */
        if (graph->states[i] == I) {
            if (rand_double() < get_gamma(graph, virus_tree, i)) {
                new_states[i]   = R;
                new_variants[i] = -1;
            } else {
                new_states[i]   = I;
                new_variants[i] = get_variant_id(graph, virus_tree, i);
            }
        }

        /* ── S → I (Infection Check) ────────────────────────────────────── */
        else if (graph->states[i] == S) {
            neighbour* current = graph->adj_list[i];

            /* Scan neighbors; models one exposure event per susceptible per step. */
            while (current != NULL) {
                if (graph->states[current->id_neighbour] == I &&
                    rand_double() < get_beta(graph, virus_tree, current->id_neighbour)) {

                    new_states[i] = I;

                    /* ── Mutation Branch ────────────────────────────────── */
                    if (rand_double() < teta &&
                        virus_tree->current_size < virus_tree->max_size) {

                        int new_id = virus_tree->current_size;
                        variant* new_variant = create_variant(
                            new_id,
                            get_variant_id(graph, virus_tree, current->id_neighbour),
                            generate_new(get_beta  (graph, virus_tree, current->id_neighbour)),
                            generate_new(get_gamma (graph, virus_tree, current->id_neighbour))
                        );
                        
                        if (new_variant == NULL) {
                            free(new_states);
                            free(new_variants);
                            return 0;
                        }

                        new_variant->total_infected++;
                        virus_tree->current_size++;
                        virus_tree->variants[new_id] = new_variant;
                        new_variants[i] = new_id;

                    } else {
                        /* ── Standard Infection (Inherit Strain) ────────── */
                        int vid = get_variant_id(graph, virus_tree, current->id_neighbour);
                        new_variants[i] = vid;
                        virus_tree->variants[vid]->total_infected++;
                    }
                    break; /* Infection confirmed; skip remaining neighbors */
                }
                current = current->next;
            }

            /* No exposure resulted in infection this step. */
            if (current == NULL) {
                new_states[i]   = S;
                new_variants[i] = -1;
            }
        }

        /* ── R → R (Maintain Permanent Immunity) ────────────────────────── */
        else {
            new_states[i]   = R;
            new_variants[i] = -1;
        }
    }

    /* Commit buffers */
    free(graph->states);
    free(graph->variants);
    graph->states   = new_states;
    graph->variants = new_variants;

    return 1;
}


/* ── Simulation utilities ─────────────────────────────────────────────────── */

/* Returns 1 if at least one individual is infectious, 0 otherwise. */
int is_epidemic_active(Graph* graph) {
    for (int i = 0; i < graph->n; i++)
        if (graph->states[i] == I) return 1;
    return 0;
}

/* Tallies population states. Returns a 3-element array: [S, I, R].
 * Note: Caller is responsible for freeing the returned array. */
int* count_individuals(Graph* graph) {
    int* SIR = calloc(3, sizeof(int));
    if (SIR == NULL) return NULL;

    for (int i = 0; i < graph->n; i++) {
        if      (graph->states[i] == I) SIR[I]++;
        else if (graph->states[i] == S) SIR[S]++;
        else                            SIR[R]++;
    }
    return SIR;
}

/* Returns the cumulative number of infections caused by a specific variant. */
int get_total_infected(tree* virus_tree, int variant_id) {
    return virus_tree->variants[variant_id]->total_infected;
}


/* ── Simulation runners ───────────────────────────────────────────────────── */

/* Runs the SIR simulation until eradication or T_MAX, logging S/I/R counts to CSV. */
int Run_simulation_export_individuals(Graph* graph, tree* virus_tree, double teta, char* filename_individuals, int T_MAX) {
    int T = 0;
    FILE* file_indiv = fopen(filename_individuals, "w");
    if (file_indiv == NULL) {
        printf("unable to open %s\n", filename_individuals);
        return -1;
    }

    fprintf(file_indiv, "Time,Susceptible,Infected,Recovered\n");

    int* SIR = count_individuals(graph);
    if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
    fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
    free(SIR);

    while (is_epidemic_active(graph) && T <= T_MAX) {
        update_state(graph, virus_tree, teta);
        T++;

        SIR = count_individuals(graph);
        if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
        fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
        free(SIR);
    }

    fclose(file_indiv);
    return T;
}

/* Runs the simulation silently, exporting only the final variant data to CSV. */
int Run_simulation_export_variants(Graph* graph, tree* virus_tree, double teta, char* filename_variants, int T_MAX) {
    int T = 0;

    while (is_epidemic_active(graph) && T <= T_MAX) {
        update_state(graph, virus_tree, teta);
        T++;
    }

    FILE* file_variants = fopen(filename_variants, "w");
    if (file_variants == NULL) {
        printf("unable to open %s\n", filename_variants);
        return -1;
    }

    fprintf(file_variants, "variant,beta,gamma,total_infected\n");
    for (int i = 0; i < virus_tree->current_size; i++) {
        fprintf(file_variants, "%d,%lf,%lf,%d\n",
                i,
                virus_tree->variants[i]->beta,
                virus_tree->variants[i]->gamma,
                get_total_infected(virus_tree, i));
    }

    fclose(file_variants);
    return T;
}


/* ── Interventions ────────────────────────────────────────────────────────── */

/* Simulates social distancing by randomly severing network edges based on rate. */
int apply_quarantine(Graph* graph, double rate) {
    if (graph == NULL) return -1;

    neighbour** current;
    neighbour* tmp;
    for (int i = 0; i < graph->n; i++) {
        current = &graph->adj_list[i];
        while (*current != NULL) {
            if (rand_double() < rate) {
                tmp = *current;
                *current = (*current)->next;
                free(tmp);
            } else {
                current = &(*current)->next;
            }
        }
    }
    return 1;
}

/* Simulates a vaccination campaign by instantly giving immunity to S individuals. */
int apply_vaccination(Graph* graph, double rate) {
    for (int i = 0; i < graph->n; i++) {
        if (graph->states[i] == S && rand_double() < rate) {
            graph->states[i] = R;
        }
    }
    return 1;
}


/* Runs simulation with a one-time vaccination campaign triggered at T_vac. */
int Run_simulation_vaccination_export_individuals(Graph* graph, tree* virus_tree, double teta, char* filename_individuals, int T_MAX, int T_vac, double rate) {
    int T = 0;
    FILE* file_indiv = fopen(filename_individuals, "w");
    if (file_indiv == NULL) {
        printf("unable to open %s\n", filename_individuals);
        return -1;
    }

    fprintf(file_indiv, "Time,Susceptible,Infected,Recovered\n");

    int* SIR = count_individuals(graph);
    if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
    fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
    free(SIR);

    while (is_epidemic_active(graph) && T <= T_MAX) {
        if (T == T_vac) {
            apply_vaccination(graph, rate);
        }
        
        update_state(graph, virus_tree, teta);
        T++;

        SIR = count_individuals(graph);
        if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
        fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
        free(SIR);
    }

    fclose(file_indiv);
    return T;
}

/* Runs simulation with a one-time quarantine (edge removal) triggered at T_quar. */
int Run_simulation_quarantine_export_individuals(Graph* graph, tree* virus_tree, double teta, char* filename_individuals, int T_MAX, int T_quar, double rate) {
    int T = 0;
    FILE* file_indiv = fopen(filename_individuals, "w");
    if (file_indiv == NULL) {
        printf("unable to open %s\n", filename_individuals);
        return -1;
    }

    fprintf(file_indiv, "Time,Susceptible,Infected,Recovered\n");

    int* SIR = count_individuals(graph);
    if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
    fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
    free(SIR);

    while (is_epidemic_active(graph) && T <= T_MAX) {
        if (T == T_quar) {
            apply_quarantine(graph, rate);
        }
        
        update_state(graph, virus_tree, teta);
        T++;

        SIR = count_individuals(graph);
        if (SIR == NULL) { printf("error while loading SIR\n"); return -1; }
        fprintf(file_indiv, "%d,%d,%d,%d\n", T, SIR[S], SIR[I], SIR[R]);
        free(SIR);
    }

    fclose(file_indiv);
    return T;
}