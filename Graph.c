#include "Graph.h"


Graph* create_graph(int n){

    Graph* graph = malloc(sizeof(Graph));
    if(graph == NULL) return NULL;

    graph->n = n;

    // Allocate state array, initialized to 0 (S)
    graph->states = calloc(n, sizeof(State));
    if(graph->states == NULL) {
        free(graph);
        return NULL;
    }

    // Allocate adjacency list array (array of pointers)
    graph->adj_list = calloc(n, sizeof(neighbour*));
    if (graph->adj_list == NULL)
    {
        free(graph->states);
        free(graph);
        return NULL;
    }
    
    return graph;
}

void destroy_graph(Graph* graph){
    if(graph == NULL) return;

    free(graph->states);

    // Traverse and free each linked list in the adjacency array
    for (int i = 0 ; i < graph->n ; i++){
        neighbour* tmp;
        while (graph->adj_list[i] != NULL)
        {   
            tmp = graph->adj_list[i]->next;
            free(graph->adj_list[i]);
            graph->adj_list[i] = tmp;
        }
    }
    
    free(graph->adj_list);
    free(graph);
}


int add_edge(Graph* graph, int id_individual, int id_neighbour) 
{   
    // Boundary validation to prevent segmentation faults
    if (id_individual < 0 || id_individual >= graph->n || 
        id_neighbour < 0 || id_neighbour >= graph->n) {
        return -1;
    }

    // Add id_neighbour to id_individual's adjacency list
    neighbour* new_neighbour_1 = malloc(sizeof(neighbour));
    if (new_neighbour_1 == NULL) return -1; 

    new_neighbour_1->id_neighbour = id_neighbour;
    new_neighbour_1->next = graph->adj_list[id_individual];
    graph->adj_list[id_individual] = new_neighbour_1;

    // Add id_individual to id_neighbour's adjacency list
    neighbour* new_neighbour_2 = malloc(sizeof(neighbour));
    if (new_neighbour_2 == NULL) {
        free(new_neighbour_1);
        return -1; 
    }

    new_neighbour_2->id_neighbour = id_individual;
    new_neighbour_2->next = graph->adj_list[id_neighbour];
    graph->adj_list[id_neighbour] = new_neighbour_2;

    return 1;
}


Graph* load_graph(char* filename) 
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;

    int n;
    // Extract total node count
    if (fscanf(file, "%d", &n) != 1) {
        fclose(file);
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (graph == NULL) {
        fclose(file);
        return NULL;
    }
    
    int u, v;

    // Parse edge pairs until EOF or formatting breaks
    while (fscanf(file, "%d %d", &u, &v) == 2)
    {
        add_edge(graph, u, v);
    }

    fclose(file);
    return graph;
}


int is_neighbour_I(Graph* graph, int id_individual)
{
    neighbour* current = graph->adj_list[id_individual];

    // Traverse the adjacency list with short-circuit evaluation
    while(current != NULL)
    {
        if(graph->states[current->id_neighbour] == I){
            return 1; 
        }
        current = current->next;
    }
    
    return 0;
}


int update_state(Graph* graph, double beta, double gamma)
{
    // Parameter validation
    if(graph == NULL || beta > 1.0 || beta < 0.0 || gamma < 0.0 || gamma > 1.0) return -1;

    // Allocate buffer for synchronous state transitions
    State* new_states = malloc(graph->n * sizeof(State));
    if(new_states == NULL) return 0;

    double rand_val;

    for (int i = 0 ; i < graph->n ; i++)
    {
        rand_val = rand_double();

        if (graph->states[i] == I)
        {
            // Evaluate I -> R transition
            if(rand_val < gamma){
                new_states[i] = R;
            }
            else{
                new_states[i] = I;
            }
        }
        else if(graph->states[i] == S)
        {
            // Evaluate S -> I transition (requires exposure to infected neighbor)
            if(is_neighbour_I(graph, i))
            {
                if(rand_val < beta){
                    new_states[i] = I;
                }
                else{
                    new_states[i] = S;
                }
            }
            else
            {
                new_states[i] = S;
            }
        }
        else if(graph->states[i] == R)
        {
            // R is an absorbing state; immunity is permanent
            new_states[i] = R;
        }
    }

    // Commit new states and release the old buffer (Pointer Swap)
    free(graph->states);
    graph->states = new_states;

    return 1;
}