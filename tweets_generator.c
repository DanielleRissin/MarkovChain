//
// Created by rissin on 2025-01-14.
//
#include <stdlib.h>
#include <stdio.h>
#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>

#define MAXCHARSIZE 1001
#define MAXTWEETSIZE 20
#define FALSE 0
#define TRUE 1
#define MINUSONE -1
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define TEN 10
#define MSGERRORFILE "Error: this file isn't valid\n"
#define MSGERRORARGC "Usage: the number of argc isn't 3 or 4\n"

int words_until_now = 0; // Counter for words processed

/**
 * Checks if the given word ends with a '.'
 */
static bool is_dot_in_end_of_word(void *word) {
    char *temp_word = (char *)word;
    if (temp_word[strlen(temp_word) - 1] == '.') {
        return true;
    }
    return false;
}

/**
 * Processes one line from the input file and adds it to the Markov Chain.
 */
static int input_one_line(char *str, MarkovChain *markov_chain, Node *token_node_prev, int max_num_words) {
    char *temp = strtok(str, " \n\r");
    while (temp != NULL && (words_until_now < max_num_words || max_num_words == -1)) {
        Node *token_node = get_node_from_database(markov_chain, temp);
        if (token_node == NULL) {
            Node *new_node = add_to_database(markov_chain, temp);
            if (new_node == NULL) {
                printf("%s", ALLOCATION_ERROR_MASSAGE);
                free_markov_chain(&markov_chain);
                return FALSE;
            }
            token_node = new_node;
        }
        if (token_node_prev != NULL && !markov_chain->is_last(token_node_prev->data->data)) {
            add_node_to_counter_list(token_node_prev->data, token_node->data, markov_chain);
        }
        token_node_prev = token_node;
        temp = strtok(NULL, " \n\r");
        words_until_now++;
    }
    return TRUE;
}

/**
 * Reads data from the file and fills the Markov Chain.
 */
static int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    char str[MAXCHARSIZE];
    while (fgets(str, MAXCHARSIZE, fp) != NULL) {
        int ans = input_one_line(str, markov_chain, NULL, words_to_read);
        if (ans == FALSE) {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * Validates command-line arguments and opens the input file.
 */
static FILE *validate(int argc, char *file_path) {
    if (argc != FOUR && argc != FIVE) {
        printf("%s", MSGERRORARGC);
        return NULL;
    }
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("%s", MSGERRORFILE);
        return NULL;
    }
    return fp;
}

/**
 * Generates and prints the required number of tweets.
 */
static void tweets_gen(MarkovChain *markov_chain, int num_of_tweets) {
    for (int i = 1; i <= num_of_tweets; i++) {
        printf("Tweet %d: ", i);
        MarkovNode *temp = get_first_random_node(markov_chain);
        generate_random_sequence(markov_chain, temp, MAXTWEETSIZE);
        printf("\n");
    }
}

/**
 * Creates a deep copy of a string.
 */
static void *copy_func_char(void *data) {
    char *new_data = (char *)data;
    char *word = malloc(sizeof(char) * (strlen(new_data) + 1));
    if (word == NULL) {
        return NULL;
    }
    strcpy(word, new_data);
    return word;
}

/**
 * Frees a string.
 */
static void free_data_char(void *data) {
    char *temp_data = (char *)data;
    free(temp_data);
}

/**
 * Prints a string and appends a space if it is not the last word.
 */
static void print_func_char(void *data) {
    char *temp_data = (char *)data;
    printf("%s", temp_data);
    if (!is_dot_in_end_of_word(temp_data)) {
        printf(" ");
    }
}

/**
 * Creates and initializes a Markov Chain.
 */
static MarkovChain *create_markove_chain() {
    MarkovChain *markov_chain_main = malloc(sizeof(MarkovChain));
    LinkedList *linked_list_main = malloc(sizeof(LinkedList));
    if (linked_list_main == NULL || markov_chain_main == NULL) {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        free(linked_list_main);
        free(markov_chain_main);
        return NULL;
    }
    markov_chain_main->database = linked_list_main;
    markov_chain_main->database->first = NULL;
    markov_chain_main->database->last = NULL;
    markov_chain_main->database->size = 0;

    markov_chain_main->copy_func = (CopyFunc)copy_func_char;
    markov_chain_main->comp_func = (CompFunc)strcmp;
    markov_chain_main->free_data = (FreeData)free_data_char;
    markov_chain_main->print_func = (PrintFunc)print_func_char;
    markov_chain_main->is_last = (IsLast)is_dot_in_end_of_word;
    return markov_chain_main;
}

/**
 * Main function.
 */
int main(int argc, char *argv[]) {
    FILE *fp = validate(argc, argv[THREE]);
    if (fp == NULL) {
        return EXIT_FAILURE;
    }

    long seed = strtol(argv[ONE], NULL, TEN);
    srand(seed);

    long num_of_tweets = strtol(argv[TWO], NULL, TEN);
    long num_of_words = MINUSONE;
    if (argc == FIVE) {
        num_of_words = strtol(argv[FOUR], NULL, TEN);
    }

    MarkovChain *markov_chain_main = create_markove_chain();
    if (markov_chain_main == NULL) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    int ans = fill_database(fp, (int)num_of_words, markov_chain_main);
    if (ans == FALSE) {
        fclose(fp);
        free(markov_chain_main->database);
        free(markov_chain_main);
        return EXIT_FAILURE;
    }

    tweets_gen(markov_chain_main, (int)num_of_tweets);
    free_markov_chain(&markov_chain_main);
    fclose(fp);
    return EXIT_SUCCESS;
}
