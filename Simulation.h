#ifndef SIMULATION_H
#define SIMULATION_H

#include "Graph.h"

int is_neighbour_I(Graph* graph, int id_individual);//Checks the adjacency list of a node for any infected neighbors.

int update_state(Graph* graph , double beta , double gamma); //  update the individuals' states.

int is_epidemic_active(Graph* graph);//Checks if there is at least one infected individual remaining.

int Run_simulation(Graph* graph , double beta , double gamma); //Executes the simulation until the infection is entirely cleared (I=0).

#endif