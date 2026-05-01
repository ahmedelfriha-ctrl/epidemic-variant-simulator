/*
 * Simulation.c
 * ------------
 * Synchronous SIR state-update engine with viral mutation.
 * See Simulation.h for interface documentation.
 */

#include "Simulation.h"


/* ── Mutation helper ──────────────────────────────────────────────────────── */

/* Perturbs x by a uniform random value in [-0.1, 0.1], then clamps to [0,1].
 * Used to generate slightly altered beta/gamma values for mutant variants. */
double generate_new(double x) {
    double max_variation = 0.1;
    double change = ((rand_double() * 2) - 1) * max_variation;

    if      (x + change > 1) return 1.0;
    else if (x + change < 0) return 0.0;
    else                     return x + change;
}


/* ── Core update ──────────────────────────────────────────────────────────── */

int update_state(Graph* graph, tree* virus_tree, double teta) {

    if (graph == NULL || virus_tree == NULL || teta < 0.0 || teta > 1.0)
        return -1;

    /* Buffer the next generation's states and variant assignments.
     * Working on copies ensures all transitions are based on the state at
     * time t, not partially-updated state at t+1 (synchronous semantics). */
    State* new_states   = malloc(graph->n * sizeof(State));
    if (new_states == NULL) return 0;

    int*   new_variants = malloc(graph->n * sizeof(int));
    if (new_variants == NULL) { free(new_states); return 0; }


    for (int i = 0; i < graph->n; i++) {

        /* ── I → R  (recovery) ──────────────────────────────────────────── */
        if (graph->states[i] == I) {

            if (rand_double() < get_gamma(graph, virus_tree, i)) {
                new_states[i]   = R;
                new_variants[i] = -1;
                virus_tree->variants[graph->variants[i]]->current_infected--;
            } else {
                new_states[i]   = I;
                new_variants[i] = get_variant_id(graph, virus_tree, i);
            }
        }

        /* ── S → I  (infection) ─────────────────────────────────────────── */
        else if (graph->states[i] == S) {

            neighbour* current = graph->adj_list[i];

            /* Scan neighbors; stop at the first one that successfully infects.
             * This models one exposure event per susceptible per time step. */
            while (current != NULL) {

                if (graph->states[current->id_neighbour] == I &&
                    rand_double() < get_beta(graph, virus_tree, current->id_neighbour)) {

                    new_states[i] = I;

                    /* ── Mutation branch ──────────────────────────────────
                     * Condition: mutation roll succeeds AND the variant tree
                     * still has capacity. max_size = n is a safe bound since
                     * at most n transmission events can occur per simulation. */
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

                        new_variant->current_infected++;
                        new_variant->total_infected++;
                        virus_tree->current_size++;          /* must increment before next check */
                        virus_tree->variants[new_id] = new_variant;
                        new_variants[i] = new_id;

                    } else {
                        /* ── No mutation: inherit the transmitting variant ─ */
                        int vid = get_variant_id(graph, virus_tree, current->id_neighbour);
                        new_variants[i] = vid;
                        virus_tree->variants[vid]->current_infected++;
                        virus_tree->variants[vid]->total_infected++;
                    }

                    break; /* infection confirmed; skip remaining neighbors */
                }

                current = current->next;
            }

            /* No infectious neighbor triggered an infection this step. */
            if (current == NULL) {
                new_states[i]   = S;
                new_variants[i] = -1;
            }
        }

        /* ── R → R  (permanent immunity) ───────────────────────────────── */
        else {
            new_states[i]   = R;
            new_variants[i] = -1;
        }
    }

    /* Commit: swap in the new buffers and release the old ones. */
    free(graph->states);
    free(graph->variants);
    graph->states   = new_states;
    graph->variants = new_variants;

    return 1;
}


/* ── Termination check ────────────────────────────────────────────────────── */

int is_epidemic_active(Graph* graph) {
    for (int i = 0; i < graph->n; i++)
        if (graph->states[i] == I) return 1;
    return 0;
}


/* ── Main loop ────────────────────────────────────────────────────────────── */

int Run_simulation(Graph* graph, tree* virus_tree, double teta) {
    int T_final = 0;

    while (is_epidemic_active(graph)) {
        update_state(graph, virus_tree, teta);
        T_final++;
    }

    return T_final;
}