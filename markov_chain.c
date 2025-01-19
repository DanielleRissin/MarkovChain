#include <stdlib.h>
#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>
#include <stdio.h>

#define ZERO 0
#define ONE 1

void insert_data(MarkovNode *temp_node, void *temp_data);

/**
* Get random num ber between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not inc
* luding)
* @return Random number
*/
int get_random_number(int max_number) {
    return rand() % max_number;
}

/**
 * @param list the list we want the node from
 * @param i the node number wanted
 * @return the wanted node.
 */

Node *wanted_node_in_ll(LinkedList *l_list, int i) {
    Node *p = l_list->first;
    for (int j = 0; j < i; j++) {
        p = p->next;
    }
    return p;
}

/**
* the function sums up all the sum of frequnceys.
* @param p_list
* @param size_of_list
* @return
*/
int sum_frequency(NextNodeCounter *p_list, int size_of_list) //
{
    int sum = 0;
    //size of list is important because we need a way to run on all args in list
    // and this is the way to do it
    for (int i = 0; i < size_of_list; i++) {
        sum += p_list[i].frequency;
    }
    return sum;
}

/**
* this function finds the correct index for the random word wanted
* @param rand_num
* @param p_list
* @param size_of_list
* @return
*/
int find_idx(int rand_num, NextNodeCounter *p_list, int size_of_list) {
    rand_num++;
    int sum = 0;
    //size of list is important because we need a way to run on all args in list
    // and this is the way to do it
    for (int i = 0; i < size_of_list; i++) {
        sum += p_list[i].frequency;
        if (sum >= rand_num) {
            return i;
        }
    }
    return sum;
}

/**
* this function alloc the counter list for the given node. if aloocation
* dost not succed - returns null.
* @param first_node
* @return
*/
NextNodeCounter *counter_list_allocation(MarkovNode *first_node) {
    if (first_node->counter_list == NULL) // in case no counter list in
    {
        return malloc(sizeof (NextNodeCounter));
    }
    return realloc(first_node->counter_list, sizeof (NextNodeCounter)
                                             * (first_node->counter_list_size + 1));
}

/**
* this function resets node by givven data
* @param temp_node
* @param temp_data
*/
void insert_data(MarkovNode *temp_node, void *temp_data) {
    temp_node->data = temp_data;
    temp_node->counter_list_size = 0;
    temp_node->counter_list = NULL;
}

MarkovNode *get_first_random_node(MarkovChain *markov_chain) {
    int i = get_random_number(markov_chain->database->size);
    bool ans = true;
    Node *curr_node = NULL;
    while (ans != false) {
        curr_node = wanted_node_in_ll(markov_chain->database, i);
        ans = markov_chain->is_last(curr_node->data->data);
        if (ans != false) {
            i = get_random_number(markov_chain->database->size);
        }
    }
    return curr_node->data;
}

/**
* Choose randomly the next state, depend on it's occurrence frequency.
* @param state_struct_ptr MarkovNode to choose from
* @return MarkovNode of the chosen state
*/

MarkovNode *get_next_random_node(MarkovNode *state_struct_ptr) {
    int max_num = sum_frequency(state_struct_ptr
                                ->counter_list, state_struct_ptr->counter_list_size);
    int rand_num = get_random_number(max_num);
    int i = find_idx(rand_num, state_struct_ptr
                     ->counter_list, state_struct_ptr->counter_list_size);
    return state_struct_ptr->counter_list[i].markov_node;
}

//what happens if we have one word only in the chain??c400 dos not happen
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length) {
    //we stop if we have max_length word or if we have other limiting tnai
    if (first_node == NULL) {
        first_node = get_first_random_node(markov_chain);
    }
    markov_chain->print_func(first_node->data);

    bool is_last_ans = false;
    int i_list = 1;
    while (i_list < max_length && is_last_ans == false) {
        MarkovNode * temp_node = get_next_random_node(first_node);
        markov_chain->print_func(temp_node->data);
        i_list += 1;
        is_last_ans = markov_chain->is_last(temp_node->data);
        first_node = temp_node;
    }
}

void free_markov_chain(MarkovChain **ptr_chain) {
    if (*ptr_chain == NULL) {
        return;
    }
    Node *node = (*ptr_chain)->database->first;
    while (node != NULL) {
        //4 free
        Node *next = node->next;
        (*ptr_chain)->free_data(node->data->data); //markovenode data
        free(node->data->counter_list);
        free(node->data); //markovnode
        free(node);
        node = next;
    }

    free((*ptr_chain)->database);
    free(*ptr_chain);
    *ptr_chain = NULL;
}

bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
                              *second_node, MarkovChain *markov_chain) {
    if (first_node == NULL || second_node == NULL) {
        return false;
    }
    for (int i = 0; i < first_node->counter_list_size; i++) {
        if (markov_chain->comp_func(first_node->
                                    counter_list[i].markov_node->data, second_node->data) == ZERO) {
            first_node->counter_list[i].frequency += 1;
            return true;
        }
    }
    NextNodeCounter * temp = counter_list_allocation(first_node);
    if (temp == NULL) {
        return false;
    }
    //update new arg in counter list
    first_node->counter_list = temp;
    first_node->counter_list[first_node->counter_list_size].markov_node
            = second_node;
    first_node->counter_list[first_node->counter_list_size].frequency = 1;
    first_node->counter_list_size += 1;
    return true;
}

Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr) {
    Node *temp_first = markov_chain->database->first;
    for (int i = 0; i < markov_chain->database->size; i++) {
        if (markov_chain->comp_func(temp_first->data->data, data_ptr) == ZERO) {
            return temp_first;
        }
        temp_first = temp_first->next;
    }
    return NULL;
}

Node *add_to_database(MarkovChain *markov_chain, void *data_ptr) {
    Node *temp_first = markov_chain->database->first;
    for (int i = 0; i < markov_chain->database->size; i++) {
        if (markov_chain->comp_func(temp_first->data->data, data_ptr) == ZERO) {
            return temp_first;
        }
        temp_first = temp_first->next;
    }
    MarkovNode * temp_node = malloc(sizeof (MarkovNode));
    if (temp_node == NULL) {
        return NULL;
    }
    void *temp_data = markov_chain->copy_func(data_ptr);
    if (temp_data == NULL) {
        free(temp_node);
        return NULL;
    }
    insert_data(temp_node, temp_data);
    int ans = add(markov_chain->database, temp_node);
    if (ans == ONE) {
        free(temp_node->data);
        free(temp_node);
        return NULL;
    }
    return markov_chain->database->last;
}
