/*
 * =============================================================================
 * Question1.c
 * =============================================================================
 *
 * Baseline simulation runner.
 * Executes three standard epidemic scenarios (R0 = 1, High R0, Low R0) 
 * Outputs population state over time to CSV files for analysis.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include "Simulation.h"

int main() {
    /* Seed the random number generator for stochastic transitions. */
    srand((unsigned int)time(NULL));

    tree* virus_tree;
    Graph* graph;
    int final_time; 

    /* ── Scenario 1: R0 = 1 ──────────────────────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/R0_1.txt", &virus_tree);
    if (graph != NULL) {
        /* Run for max 100 steps*/
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/R0_1_individuals.csv", 100);
        printf("R0_1 complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Scenario 2: High R0 ─────────────────────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/R0_high.txt", &virus_tree);
    if (graph != NULL) {
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/R0_high_individuals.csv", 100);
        printf("R0_High complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Scenario 3: Low R0 ──────────────────────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/R0_low.txt", &virus_tree);
    if (graph != NULL) {
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/R0_low_individuals.csv", 100);
        printf("R0_Low complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }
    
    return 0;
}
