/*
 * Model.h
 * -------
 * for graph construction, I/O, and virus-tree management.
 */

#ifndef MODEL_H
#define MODEL_H

#include "Types.h"

/* ── Graph lifecycle ──────────────────────────────────────────────────────── */

/* Allocates and zero-initialises a graph of n individuals (all states = S). */
Graph*   create_graph(int n);

void     destroy_graph(Graph* graph);

/* Adds an undirected edge between two individuals.
 * Returns 1 on success, -1 if either id is out of bounds or allocation fails. */
int      add_edge(Graph* graph, int id_individual, int id_neighbour);

/* Parses a simulation scenario from a text file.
 * File format:
 *   Line 1 : <n> <beta> <gamma>
 *   Lines 2…n+1 : <id> <state>   (state ∈ {S, I, R})
 *   Remaining : <u> <v>  (one undirected edge per line)
 *
 * Allocates and returns the Graph; also initialises *virus_tree with the
 * root variant derived from the file's beta/gamma values. */
Graph*   load_epidemic(char* filename, tree** virus_tree);

/* ── Virus-tree lifecycle ─────────────────────────────────────────────────── */

/* Creates the variant tree with capacity n and a single root variant
 * (id=0, parent=-1) using the supplied beta and gamma. */
tree*    create_virus(int n, double beta, double gamma);

void     destroy_virus(tree* virus_tree);

/* Allocates a new variant node; does NOT insert it into the tree. */
variant* create_variant(int id, int parent_id, double beta, double gamma);

/* ── Per-individual accessors (require individual to be in state I) ────────── */

double   get_beta      (Graph* graph, tree* virus_tree, int individual_id);
double   get_gamma     (Graph* graph, tree* virus_tree, int individual_id);
int      get_variant_id(Graph* graph, tree* virus_tree, int individual_id);

#endif