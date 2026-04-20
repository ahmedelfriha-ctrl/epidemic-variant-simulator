#include "Simulation.h"

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

int is_epidemic_active(Graph* graph)
{
    for (int i=0 ; i< graph->n ; i++)
    {
        if (graph->states[i]==I) 
        {
            return 1; // Short-circuit: found one, no need to check others
        } 
    }
    return 0; // Checked everyone, no infected found
}

int Run_simulation(Graph* graph , double beta , double gamma)
{
    int T_final=0;

    // Continue as long as there is at least one infected person

    while (is_epidemic_active(graph))
    {
        update_state(graph,beta,gamma);
        T_final++;
    }

    return T_final;
    
}
