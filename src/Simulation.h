/*
 * =============================================================================
 * Simulation.h
 * =============================================================================
 *
 * Public interface for the synchronous SIR state-update engine.
 * Handles network-based viral transmission, mutation, and interventions.
 *
 * =============================================================================
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Model.h"

/* ── Core Simulation ─────────────────────────────────────────────────────── */

/*
 * Advances the population by one discrete time step synchronously.
 * - I -> R: Infected recover with probability gamma.
 * - S -> I: Susceptibles are infected by I-neighbors with probability beta.
 * - Mutation: New infections spawn a mutated strain with probability teta.
 * * Returns 1 on success, 0 on memory allocation failure, -1 on invalid args.
 */
int update_state(Graph* graph, tree* virus_tree, double teta);

/* Returns 1 if at least one individual is currently infected (I), 0 otherwise. */
int is_epidemic_active(Graph* graph);


/* ── Metrics & Utilities ─────────────────────────────────────────────────── */

/*
 * Tallies current states. Returns a heap-allocated array indexed by state: [S, I, R].
 * Caller is responsible for calling free() on the returned pointer.
 */
int* count_individuals(Graph* graph);

/* Returns the cumulative number of individuals ever infected by variant_id. */
int get_total_infected(tree* virus_tree, int variant_id);

/* Mutates transmission/recovery rates by a random step in [-0.1, 0.1], clamped to [0,1]. */
double generate_new(double x);


/* ── Interventions & Runners ─────────────────────────────────────────────── */

/* Simulates social distancing by permanently severing network edges at the given rate. */
int apply_quarantine(Graph* graph, double rate);

/* Instantly transitions Susceptible individuals to Recovered (immune) based on rate. */
int apply_vaccination(Graph* graph, double rate);

/* Runs simulation to T_MAX, applying a quarantine intervention at step T_quar. Logs to CSV. */
int Run_simulation_quarantine_export_individuals(Graph* graph, tree* virus_tree, double teta, char* filename_individuals, int T_MAX, int T_quar, double rate);

/* Runs simulation to T_MAX, applying a vaccination intervention at step T_vac. Logs to CSV. */
int Run_simulation_vaccination_export_individuals(Graph* graph, tree* virus_tree, double teta, char* filename_individuals, int T_MAX, int T_vac, double rate);

#endif