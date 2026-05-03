/*
 * =============================================================================
 * Types.h
 * =============================================================================
 *
 * Central type definitions for the network-based SIR epidemic simulation.
 * Isolates shared structs and enums to prevent circular dependencies.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdio.h>

/*
 * Epidemiological states for the SIR model.
 * S: Susceptible (healthy, can be infected).
 * I: Infectious (currently infected, can transmit).
 * R: Recovered (permanently immune or vaccinated).
 */
typedef enum { S, I, R } State;

/*
 * A single viral strain (node in the mutation lineage).
 * * variant_id     : Unique ID (and index in the tree array).
 * parent_id      : ID of the parent strain (-1 for root/original).
 * total_infected : Cumulative infections caused by this variant.
 * beta           : Transmission probability [0.0, 1.0].
 * gamma          : Recovery probability [0.0, 1.0].
 */
typedef struct variant {
    int    variant_id;
    int    parent_id;
    int    total_infected;
    double beta;
    double gamma;
} variant;

/*
 * Flat array storing all viral variants for O(1) lookup.
 * * max_size     : Capacity (bounded by population size n).
 * current_size : Total number of variants created so far.
 * variants     : Array of variant pointers.
 */
typedef struct tree {
    int       max_size;
    int       current_size;
    variant** variants;
} tree;

/*
 * Adjacency list node representing a network edge (contact).
 */
typedef struct neighbour {
    int               id_neighbour;
    struct neighbour* next;
} neighbour;

/*
 * Undirected contact network representing the population.
 * * n        : Total number of individuals.
 * states   : Array of current epidemiological states (S/I/R).
 * variants : Array of currently infecting variant IDs (-1 if not infected).
 * adj_list : Array of adjacency list heads for each individual.
 */
typedef struct Graph {
    int         n;
    State* states;
    int* variants;
    neighbour** adj_list;
} Graph;

/*
 * Utility: Generates a uniform random double in [0.0, 1.0].
 */
static inline double rand_double(void) {
    return (double)rand() / (double)RAND_MAX;
}

#endif