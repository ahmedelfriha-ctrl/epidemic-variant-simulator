#ifndef GRAPH_H
#define GRAPH_H

#include "Types.h"


Graph* create_graph(int n); // initialize a graph.

void destroy_graph(Graph* graph);

int add_edge(Graph* graph , int id_individual , int id_neighbour); // add an edge between two individuals.

Graph* load_graph(char* filename); // loads a graph from a file.

int is_neighbour_I(Graph* graph, int id_individual);//Checks the adjacency list of a node for any infected neighbors.

int update_state(Graph* graph , double beta , double gamma); //  update the individuals' states.



#endif