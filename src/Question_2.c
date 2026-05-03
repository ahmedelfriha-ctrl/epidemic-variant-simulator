/*
 * =============================================================================
 * Question2.c
 * =============================================================================
 *
 * Network topology simulation runner.
 * Executes the epidemic simulation across four distinct graph structures:
 * Small-World, Random Geometric, Scale-Free, and Connected Caveman.
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

    /* ── Topology 1: Small-World Network ─────────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        /* Run for max 100 steps*/
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/small_world_individuals.csv", 100);
        printf("Small-world complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Topology 2: Random Geometric Graph ──────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/random_geometry.txt", &virus_tree);
    if (graph != NULL) {
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/random_geometry_individuals.csv", 100);
        printf("Random geometry complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    /* ── Topology 3: Scale-Free Network ──────────────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/scale_free.txt", &virus_tree);
    if (graph != NULL) {
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/scale_free_individuals.csv", 100);
        printf("Scale-free complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }
    
    /* ── Topology 4: Connected Caveman Network ───────────────────────────── */
    virus_tree = NULL;
    graph = load_epidemic("data/caveman_connection.txt", &virus_tree);
    if (graph != NULL) {
        final_time = Run_simulation_export_individuals(graph, virus_tree, 0.05, "data/caveman_connection_individuals.csv", 100);
        printf("Caveman connection complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }
    
    return 0;
}