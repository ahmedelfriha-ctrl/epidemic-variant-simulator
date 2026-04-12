#ifndef GRAPH_H
#define GRAPH_H

#include "Types.h"


Graph* create_graph(int n); // initialize a graph.

void destroy_graph(Graph* graph);

void add_edge(Graph* graph , int id_individual , int id_neighbour); // add an edge between two individuals.

Graph* load_graph(char* filename); // loads a graph from a file.

void update_state(Graph* graph , double beta , double gamma); //  update the individuals' states.



#endif