/*
 * =============================================================================
 * Question3.c
 * =============================================================================
 *
 * Intervention strategies simulation runner.
 * Evaluates the impact of different epidemic countermeasures (vaccination
 * and quarantine) triggered at a specific time step (T=7) on a Small-World 
 * network. Compares outcomes across different intervention adoption rates.
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

    /* ── Baseline: Small-World (No Intervention) ─────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Run for max 100 steps with teta = 0.05 */
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/small_world_individuals.csv", 100);
        printf("Baseline Small-world complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Intervention 1: Vaccination (High Rate: 90%) ────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Triggered at T=7, Rate=0.90 */
        final_time = Run_simulation_vaccination_export_individuals(graph, virus_tree, 0.05, "data/vaccination_rate_0.9_individuals.csv", 100, 7, 0.9);
        printf("Vaccination (0.9) simulation complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }
    
    /* ── Intervention 2: Vaccination (Moderate Rate: 50%) ────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Triggered at T=7, Rate=0.50 */
        final_time = Run_simulation_vaccination_export_individuals(graph, virus_tree, 0.05, "data/vaccination_rate_0.5_individuals.csv", 100, 7, 0.5);
        printf("Vaccination (0.5) simulation complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }
    
    /* ── Intervention 3: Quarantine (Moderate Rate: 50%) ─────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Triggered at T=7, Rate=0.50 */
        final_time = Run_simulation_quarantine_export_individuals(graph, virus_tree, 0.05, "data/quarantine_rate_0.5_individuals.csv", 100, 7, 0.5);
        printf("Quarantine (0.5) simulation complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Intervention 4: Quarantine (High Rate: 90%) ─────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Triggered at T=7, Rate=0.90 */
        final_time = Run_simulation_quarantine_export_individuals(graph, virus_tree, 0.05, "data/quarantine_rate_0.9_individuals.csv", 100, 7, 0.9);
        printf("Quarantine (0.9) simulation complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    return 0;
}