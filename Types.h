#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdio.h>

typedef enum{S , I , R} State;

//------------ Graph declaration ---------------

typedef struct neighbour
{
    int id_neighbour; // the index of the neighbour in States
    struct neighbour* next;
}neighbour ;


typedef struct Graph
{
    int n; // The number of individuals in the graph
    State* states; // Table containing the states of the individuals in the graph
    neighbour** adj_list; // list of pointers for the neighbours
}Graph;


//------------ Returns a random double between 0 and 1---------------
static inline double rand_double(void) {
    return (double)rand() / (double)RAND_MAX;
}

#endif