/*
 * Simulation.h
 * ------------
 * for the synchronous SIR state-update engine.
 *
 * The simulation follows a discrete-time, synchronous (cellular-automaton)
 * update rule: all state transitions for time step t are computed from the
 * state at t-1 and committed simultaneously, avoiding order-of-update bias.
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Model.h"

/* Advances the population by one time step.
 *
 * Transition rules:
 *   I → R  with probability gamma  (recovery)
 *   S → I  with probability beta   (infection by at least one I neighbor)
 *   R → R  always                  (permanent immunity)
 *
 * When S→I occurs, the new case inherits the transmitting neighbor's variant.
 * With probability teta (and provided the variant tree has capacity), the
 * virus mutates: a child variant is created with beta and gamma slightly
 * perturbed by generate_new().
 *
 * Returns  1 on success,
 *          0 if a memory allocation fails (simulation state is unchanged) or arguments are invalid. */
int update_state(Graph* graph, tree* virus_tree, double teta);

/* Returns 1 if at least one individual is currently in state I, 0 otherwise.
 * Used as the main loop termination condition. */
int is_epidemic_active(Graph* graph);

/* Runs the simulation to completion (no I nodes remain).
 * Returns the total number of time steps elapsed. */
int Run_simulation(Graph* graph, tree* virus_tree, double teta);

/* Generates a mutated parameter value from a parent value x ∈ [0,1].
 * Applies a bounded random walk with max step ±0.1, clamped to [0,1]. */
double generate_new(double x);

#endif