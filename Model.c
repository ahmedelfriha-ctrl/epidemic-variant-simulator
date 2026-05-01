/*
 * Model.c
 * -------
 * Implementation of graph construction, file I/O, and virus-tree management.
 * See Model.h for the public interface documentation.
 */

#include "Model.h"


/* ── Graph lifecycle ──────────────────────────────────────────────────────── */

Graph* create_graph(int n) {

    Graph* graph = malloc(sizeof(Graph));
    if (graph == NULL) return NULL;

    graph->n = n;

    /* All individuals start susceptible (S = 0 from the enum). */
    graph->states = calloc(n, sizeof(State));
    if (graph->states == NULL) {
        free(graph);
        return NULL;
    }

    graph->variants = malloc(n * sizeof(int));
    if (graph->variants == NULL) {
        free(graph->states);
        free(graph);
        return NULL;
    }
    /* -1 means "not currently infected by any variant". */
    for (int i = 0; i < n; i++)
        graph->variants[i] = -1;

    /* calloc zeroes the pointers, so every list head starts as NULL. */
    graph->adj_list = calloc(n, sizeof(neighbour*));
    if (graph->adj_list == NULL) {
        free(graph->variants);
        free(graph->states);
        free(graph);
        return NULL;
    }

    return graph;
}

void destroy_graph(Graph* graph) {
    if (graph == NULL) return;

    free(graph->states);
    free(graph->variants);

    for (int i = 0; i < graph->n; i++) {
        neighbour* tmp;
        while (graph->adj_list[i] != NULL) {
            tmp = graph->adj_list[i]->next;
            free(graph->adj_list[i]);
            graph->adj_list[i] = tmp;
        }
    }
    free(graph->adj_list);
    free(graph);
}

int add_edge(Graph* graph, int id_individual, int id_neighbour) {

    if (id_individual < 0 || id_individual >= graph->n ||
        id_neighbour  < 0 || id_neighbour  >= graph->n)
        return -1;

    /* Prepend to id_individual's list. */
    neighbour* new_neighbour_1 = malloc(sizeof(neighbour));
    if (new_neighbour_1 == NULL) return -1;

    new_neighbour_1->id_neighbour = id_neighbour;
    new_neighbour_1->next         = graph->adj_list[id_individual];
    graph->adj_list[id_individual] = new_neighbour_1;

    /* Prepend to id_neighbour's list (undirected edge). */
    neighbour* new_neighbour_2 = malloc(sizeof(neighbour));
    if (new_neighbour_2 == NULL) {
        free(new_neighbour_1);
        return -1;
    }

    new_neighbour_2->id_neighbour = id_individual;
    new_neighbour_2->next         = graph->adj_list[id_neighbour];
    graph->adj_list[id_neighbour] = new_neighbour_2;

    return 1;
}


/* ── Virus-tree lifecycle ─────────────────────────────────────────────────── */

variant* create_variant(int id, int parent_id, double beta, double gamma) {

    variant* new_variant = malloc(sizeof(variant));
    if (new_variant == NULL) return NULL;

    new_variant->variant_id       = id;
    new_variant->parent_id        = parent_id;
    new_variant->current_infected = 0;
    new_variant->total_infected   = 0;
    new_variant->beta             = beta;
    new_variant->gamma            = gamma;

    return new_variant;
}

tree* create_virus(int n, double beta, double gamma) {

    if (n <= 0) return NULL;

    tree* virus_tree = malloc(sizeof(tree));
    if (virus_tree == NULL) return NULL;

    virus_tree->variants = calloc(n, sizeof(variant*));
    if (virus_tree->variants == NULL) {
        free(virus_tree);
        return NULL;
    }

    /* Seed the tree with the original (root) variant. */
    variant* initial_variant = create_variant(0, -1, beta, gamma);
    if (initial_variant == NULL) {
        free(virus_tree->variants);
        free(virus_tree);
        return NULL;
    }

    /* max_size = n is a biologically justified upper bound: a mutation
     * requires a transmission event, so there can be at most n mutations
     * across the entire simulation in a network of n individuals. */
    virus_tree->max_size     = n;
    virus_tree->current_size = 1;
    virus_tree->variants[0]  = initial_variant;

    return virus_tree;
}

void destroy_virus(tree* virus_tree) {
    if (virus_tree == NULL) return;

    for (int i = 0; i < virus_tree->current_size; i++)
        free(virus_tree->variants[i]);

    free(virus_tree->variants);
    free(virus_tree);
}


/* ── File I/O ─────────────────────────────────────────────────────────────── */

Graph* load_epidemic(char* filename, tree** virus_tree) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;

    int n;
    fscanf(file, "%d", &n);

    Graph* graph = create_graph(n);
    if (graph == NULL) { fclose(file); return NULL; }

    double beta, gamma;
    fscanf(file, "%lf %lf", &beta, &gamma);

    *virus_tree = create_virus(n, beta, gamma);
    if (*virus_tree == NULL) { fclose(file); return NULL; }

    int  u;
    char v;
    int  s;

    for (int i = 0; i < n; i++) {
        fscanf(file, "%d %c", &u, &v);

        if      (v == 'S') s = S;
        else if (v == 'I') s = I;
        else               s = R;

        graph->states[u] = s;

        if (s == I) {
            /* Attribute the initially infected nodes to the root variant. */
            graph->variants[u] = 0;
            (*virus_tree)->variants[0]->current_infected++;
            (*virus_tree)->variants[0]->total_infected++;
        } else {
            graph->variants[u] = -1;
        }
    }

    /* Read edges until EOF. */
    int w;
    while (fscanf(file, "%d %d", &u, &w) == 2)
        add_edge(graph, u, w);

    fclose(file);
    return graph;
}


/* ── Per-individual accessors ─────────────────────────────────────────────── */

/* These resolve the two-level indirection:
 *   individual → variant_id (graph->variants[])
 *   variant_id → variant*  (virus_tree->variants[])
 * Only call them when the individual is in state I. */

double get_beta(Graph* graph, tree* virus_tree, int individual_id) {
    return virus_tree->variants[graph->variants[individual_id]]->beta;
}

double get_gamma(Graph* graph, tree* virus_tree, int individual_id) {
    return virus_tree->variants[graph->variants[individual_id]]->gamma;
}

int get_variant_id(Graph* graph, tree* virus_tree, int individual_id) {
    return virus_tree->variants[graph->variants[individual_id]]->variant_id;
}