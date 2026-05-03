/*
 * =============================================================================
 * Question4.c
 * =============================================================================
 *
 * Viral mutation tracking simulation runner.
 * Executes an epidemic simulation on a Small-World network with a high 
 * mutation rate (teta = 0.3). Exports the final viral lineage data 
 * (transmission rates, recovery rates, and total cases per variant) 
 * to CSV rather than tracking daily population SIR counts.
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

    tree* virus_tree = NULL;
    Graph* graph      = NULL;
    int    final_time;

    /* ── Mutation Tracking: Small-World Network ──────────────────────────── */
    graph = load_epidemic("data/small_world.txt", &virus_tree);
    if (graph != NULL) {
        
        /* Run for max 100 steps with a high mutation rate (teta = 0.3).
         * Uses the variant-export function to log evolutionary data. */
        final_time = Run_simulation_export_variants( 
            graph, virus_tree, 0.3,
            "data/variants.csv",                       
            100
        );
        
        printf("Mutation tracking simulation complete! Final time: %d steps.\n", final_time);
        
        destroy_graph(graph);
        destroy_virus(virus_tree);
    }

    return 0;
}