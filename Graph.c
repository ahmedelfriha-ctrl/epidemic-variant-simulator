#include "Graph.h"

Graph* create_graph(int n){

    Graph* graph=malloc(sizeof(Graph));
    if(graph==NULL) return NULL;

    graph->n=n;

    graph->states=calloc(n,sizeof(State));
    if(graph->states==NULL) {
        free(graph);
        return NULL;
    }

    graph->adj_list=calloc(n,sizeof(neighbour*));
    if (graph->adj_list==NULL)
    {
        free(graph->states);
        free(graph);
        return NULL;
    }
    

    return graph;

};

void destroy_graph(Graph* graph){

    free(graph->states);

    for (int i=0 ; i<graph->n ; i++){
        neighbour* tmp;
        while (graph->adj_list[i]!=NULL)
        {   

            tmp=graph->adj_list[i]->next;
            free(graph->adj_list[i]);
            graph->adj_list[i]=tmp;
        }
    }
    free(graph->adj_list);

    free(graph);
    }


