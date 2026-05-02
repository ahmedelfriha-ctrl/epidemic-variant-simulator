/*
 * Types.h
 * -------
 * Central type definitions for the network-based SIR epidemic simulation.
 * All shared structs, enums, and utilities live here to avoid circular
 * dependencies between Graph.h and Simulation.h.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdio.h>

/* ── Epidemiological States ─────────────────────────────────────────────────
 * S : Susceptible  — healthy, can be infected by an infectious neighbor.
 * I : Infectious   — currently infected and capable of transmitting.
 * R : Recovered    — permanently immune (also used to model vaccinated nodes).
 */
typedef enum { S, I, R } State;


/* ── Virus Variant ──────────────────────────────────────────────────────────
 * Represents a single viral strain in the mutation tree.
 *
 * variant_id       : unique identifier (also its index in tree.variants[]).
 * parent_id        : id of the strain this one mutated from; -1 for the root.
 * current_infected : number of individuals *currently* carrying this variant.
 * total_infected   : cumulative count since the variant first appeared.
 * beta             : transmission probability per contact per time step.
 * gamma            : recovery probability per time step.
 */
typedef struct variant {
    int    variant_id;
    int    parent_id;
    int    current_infected;
    int    total_infected;
    double beta;
    double gamma;
} variant;

/* ── Variant Tree ───────────────────────────────────────────────────────────
 * Flat array that stores all variants ever observed during the simulation.
 * Indexed by variant_id, so lookup is O(1).
 *
 * max_size     : capacity of the variants[] array (set to n, the population
 *                size). This is a safe upper bound because each mutation
 *                requires a transmission event, and there can be at most n
 *                simultaneous transmissions in a network of n nodes.
 * current_size : next free slot; also equals the total number of variants
 *                created so far.
 */
typedef struct tree {
    int       max_size;
    int       current_size;
    variant** variants;
} tree;


/* ── Adjacency-List Node ────────────────────────────────────────────────────
 * One entry in the linked-list of neighbors for a given individual.
 */
typedef struct neighbour {
    int              id_neighbour;
    struct neighbour* next;
} neighbour;

/* ── Population Graph ───────────────────────────────────────────────────────
 * Undirected graph where each node is an individual and each edge is a
 * contact through which the virus can spread.
 *
 * n        : total number of individuals.
 * states   : current epidemiological state of every individual.
 * variants : variant_id currently infecting each individual; -1 if not I.
 * adj_list : adjacency list — adj_list[i] is the head of i's neighbor list.
 */
typedef struct Graph {
    int        n;
    State*     states;
    int*       variants;
    neighbour** adj_list;
} Graph;


/* ── Utility: uniform random double in [0, 1) ───────────────────────────── */
static inline double rand_double(void) {
    return (double)rand() / (double)RAND_MAX;
}

#endif