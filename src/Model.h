/*
 * =============================================================================
 * Model.h
 * =============================================================================
 *
 * Public interface for graph construction, file I/O, and variant tracking.
 *
 * =============================================================================
 */

#ifndef MODEL_H
#define MODEL_H

#include "Types.h"

/* ── Graph lifecycle ─────────────────────────────────────────────────────── */

/* Allocates and initializes a graph of n individuals (default state S). */
Graph* create_graph(int n);

/* Frees the graph structure, arrays, and all adjacency list nodes. */
void destroy_graph(Graph* graph);

/* Inserts an undirected edge between two individuals. 
 * Returns 1 on success, -1 if IDs are invalid or memory allocation fails. */
int add_edge(Graph* graph, int id_individual, int id_neighbour);


/* ── File I/O ────────────────────────────────────────────────────────────── */

/* * Parses a simulation scenario from a text file.
 * Allocates and returns the Graph. Populates *virus_tree with the root variant.
 * Expected file format:
 * Line 1      : <n> <beta> <gamma>
 * Lines 2...n : <id> <state> (S, I, or R)
 * Remaining   : <u> <v> (undirected edges)
 */
Graph* load_epidemic(char* filename, tree** virus_tree);


/* ── Virus-tree lifecycle ────────────────────────────────────────────────── */

/* Initializes the variant tracker with capacity n and seeds the root strain. */
tree* create_virus(int n, double beta, double gamma);

/* Frees the variant tree and all stored strains. */
void destroy_virus(tree* virus_tree);

/* Allocates a new variant node (does NOT automatically insert it into the tree). */
variant* create_variant(int id, int parent_id, double beta, double gamma);


/* ── Per-individual accessors ────────────────────────────────────────────── */

/* Retrieves parameters for the strain currently infecting the given individual.
 * WARNING: Behavior is undefined if the individual is not currently in state I. */
double get_beta      (Graph* graph, tree* virus_tree, int individual_id);
double get_gamma     (Graph* graph, tree* virus_tree, int individual_id);
int    get_variant_id(Graph* graph, tree* virus_tree, int individual_id);

#endif