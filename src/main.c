#include <stdio.h>

#include "Simulation.h"
#include<time.h>

int main()
{

    srand((unsigned int)time(NULL));

    tree* virus_tree=NULL;
    Graph* graph=load_epidemic("data/sample_graph.txt",&virus_tree);

    int final_time = Run_simulation(graph, virus_tree, 0.05, "data/individuals.csv", "data/variants.csv", "data/parameters_teta_0.05.csv");
    printf("Simulation complete! The final time is: %d steps.\n", final_time);

    destroy_graph(graph);
    destroy_virus(virus_tree);
    
    return 0;
}