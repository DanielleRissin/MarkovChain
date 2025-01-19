// //
// // Created by rissin on 2025-01-13.
// //
// #include <string.h> // For strlen(), strcmp(), strcpy()
// #include "markov_chain.h"
// #define ZERO 0
// #define ONE 1
// #define TWO 2
// #define THREE 3
// #define TEN 10
// #define MINUSONE -1
//
// #define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
//
// #define EMPTY -1
// #define BOARD_SIZE 100
// #define MAX_GENERATION_LENGTH 60
//
// #define DICE_MAX 6
// #define NUM_OF_TRANSITIONS 20
// #define MSGERRORARGC "Usage: the number of argc isnt 2\n"
// /**
// * represents the transitions by ladders and snakes in the game
// * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
// */
// const int transitions[][2] = {
//     {13, 4},
//     {85, 17},
//     {95, 67},
//     {97, 58},
//     {66, 89},
//     {87, 31},
//     {57, 83},
//     {91, 25},
//     {28, 50},
//     {35, 11},
//     {8, 30},
//     {41, 62},
//     {81, 43},
//     {69, 32},
//     {20, 39},
//     {33, 70},
//     {79, 99},
//     {23, 76},
//     {15, 47},
//     {61, 14}
// };
//
// /**
// * struct represents a Cell in the game board
// */
// typedef struct Cell {
//     int number; // Cell number 1-100
//     int ladder_to; // ladder_to represents the jump of the ladder in
//     // case there is one from this square
//     int snake_to; // snake_to represents the jump of the
//     // snake in case there is one from this square
//     //both ladder_to and snake_to should be -1 if the Cell doesn't have them
// } Cell;
//
// /** Error handler **/
//
//
// static int handle_error(char *error_msg, MarkovChain **database) {
//     printf("%s", error_msg);
//     if (database != NULL) {
//         free_markov_chain(database);
//     }
//     return EXIT_FAILURE;
// }
//
// static int create_board(Cell *cells[BOARD_SIZE]) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         cells[i] = malloc(sizeof(Cell));
//         if (cells[i] == NULL) {
//             for (int j = 0; j < i; j++) {
//                 free(cells[j]);
//             }
//             handle_error(ALLOCATION_ERROR_MASSAGE, NULL);
//             return EXIT_FAILURE;
//         }
//         *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
//     }
//
//     for (int i = 0; i < NUM_OF_TRANSITIONS; i++) {
//         int from = transitions[i][0];
//         int to = transitions[i][1];
//         if (from < to) {
//             cells[from - 1]->ladder_to = to;
//         } else {
//             cells[from - 1]->snake_to = to;
//         }
//     }
//     return EXIT_SUCCESS;
// }
//
// /**
// * fills database
// * @param markov_chain
// * @return EXIT_SUCCESS or EXIT_FAILURE
// */
// static int fill_database(MarkovChain *markov_chain) {
//     Cell *cells[BOARD_SIZE];
//     if (create_board(cells) == EXIT_FAILURE) {
//         return EXIT_FAILURE;
//     }
//     MarkovNode *from_node = NULL, *to_node = NULL;
//     size_t index_to;
//     for (size_t i = 0; i < BOARD_SIZE; i++) {
//         add_to_database(markov_chain, cells[i]);
//     }
//     for (size_t i = 0; i < BOARD_SIZE; i++) {
//         from_node = get_node_from_database(markov_chain, cells[i])->data;
//         if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY) {
//             index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
//             to_node = get_node_from_database(markov_chain, cells[index_to])
//                     ->data;
//             add_node_to_counter_list(from_node, to_node, markov_chain);
//         } else {
//             for (int j = 1; j <= DICE_MAX; j++) {
//                 index_to = ((Cell *) (from_node->data))->number + j - 1;
//                 if (index_to >= BOARD_SIZE) {
//                     break;
//                 }
//                 to_node = get_node_from_database(markov_chain, cells[index_to])
//                         ->data;
//                 add_node_to_counter_list(from_node, to_node, markov_chain);
//             }
//         }
//     }
//     // free temp arr
//     for (size_t i = 0; i < BOARD_SIZE; i++) {
//         free(cells[i]);
//     }
//     return EXIT_SUCCESS;
// }
//
// ///**
// // * a pointer to a function that gets a pointer of Cell data type
// // * and returns a newly allocated copy of it
// // * returns a generic pointer.
// // */
// //static Cell *copy_func_cell (Cell *data)
// //{
// // char *word = malloc (sizeof (char) * (strlen (data) + 1));
// // if (word == NULL)
// // {
// // return NULL;
// // }
// // strcpy (word, data);
// // return word;
// //
// //}
// /**
// * this function takes care of printing a cell printing needed for a cell
// * @param cell
// */
// static void print_func_path(void *data) {
//     Cell *cell = (Cell *) data;
//     if (cell->number == BOARD_SIZE) {
//         printf("[%d]", cell->number);
//     } else if (cell->ladder_to != MINUSONE) {
//         printf("[%d]-ladder to %d -> ", cell->number, cell->ladder_to);
//     } else if (cell->snake_to != MINUSONE) {
//         printf("[%d]-snake to %d -> ", cell->number, cell->snake_to);
//     } else {
//         printf("[%d] -> ", cell->number);
//     }
// }
//
// /**
// * this function gets a pointer of cell and returns a newly allocated copy
// * of it. NULL if else
// */
// static void *copy_func_cell(void *data) {
//     Cell *cell = (Cell *) data;
//     Cell *temp = malloc(sizeof(Cell));
//     if (temp == NULL) {
//         return NULL;
//     }
//     temp->number = cell->number;
//     temp->snake_to = cell->snake_to;
//     temp->ladder_to = cell->ladder_to;
//     return temp;
// }
//
// /**
// * pointer to a func that gets 2 pointers of cell data type
// * (same one) and compare between them
// * returns: - a positive value if the first is bigger
// * - a negative value if the second is big
// * - 0 if equal
// * /
//
// // returns: - a positive value if the first is bigger
// // - a negative value if the second is bigger
// * - 0 if equal
// */
// static int comp_func_cell(void *data1, void *data2) {
//     Cell *cell1 = (Cell *) data1;
//     Cell *cell2 = (Cell *) data2;
//     return (cell1->number) - (cell2->number);
// }
//
// /**
// * this function frees all the char data storred
// * @param data pointer to the char data string.
// */
// static void free_data_cell(void *data) {
//     Cell *cell = (Cell *) data;
//     free(cell);
// }
//
// /**
// *
// * @param word the word we want to check if
// * has a "."
// * in the end.
// * @return
// */
// static bool is_last_cell(void *data) {
//     Cell *cell = (Cell *) data;
//     if (cell->number == BOARD_SIZE) {
//         return true;
//     }
//     return false;
// }
//
// /**
//
// * THIS FUNCTION CERATS A MARKOV CHAIN AND RESETS IT AS NEEDED.
// * @return
// */
// static MarkovChain *create_markove_chain() {
//     MarkovChain *markov_chain_main = malloc(sizeof(MarkovChain));
//     if (markov_chain_main == NULL) {
//         printf("%s", ALLOCATION_ERROR_MASSAGE);
//         return NULL;
//     }
//     LinkedList *linked_list_main = malloc(sizeof(LinkedList));
//     if (linked_list_main == NULL) {
//         printf("%s", ALLOCATION_ERROR_MASSAGE);
//         free(markov_chain_main);
//         return NULL;
//     }
//     markov_chain_main->database = linked_list_main;
//     markov_chain_main->database->first = NULL;
//     markov_chain_main->database->last = NULL;
//     markov_chain_main->database->size = 0;
//     //things i added in EX3.B
//     markov_chain_main->copy_func = (CopyFunc) copy_func_cell;
//     markov_chain_main->comp_func = (CompFunc) comp_func_cell;
//     markov_chain_main->free_data = (FreeData) free_data_cell;
//     markov_chain_main->print_func = (PrintFunc) print_func_path;
//     markov_chain_main->is_last = (IsLast) is_last_cell;
//     return markov_chain_main;
// }
//
// /**
// * this function creates the wnated paths
// * @param markov_chain_main
// * @param num_of_path
// */
// void creat_path(MarkovChain *markov_chain, int num_of_path) //generate path
// {
//     for (int i = 1; i < num_of_path + 1; i++) {
//         printf("Random Walk %d: ", i);
//         MarkovNode *first = markov_chain->database->first->data;
//         generate_random_sequence(markov_chain, first, MAX_GENERATION_LENGTH);
//         printf("%s", "\n");
//     }
// }
//
// /**
// * @param argc num of arguments
// * @param argv 1) Seed
// * 2) Number of sentences to generate
// * @return EXIT_SUCCESS or EXIT_FAILURE
// */
//
// int main(int argc, char *argv[]) {
//     if (argc != THREE) {
//         printf("%s",MSGERRORARGC);
//         return EXIT_FAILURE;
//     }
//     long seed = strtol(argv[ONE], NULL, TEN);
//     srand(seed);
//     long path_num = strtol(argv[TWO], NULL, TEN);
//
//     MarkovChain *markov_chain_main = create_markove_chain();
//     if (markov_chain_main == NULL) {
//         printf(ALLOCATION_ERROR_MASSAGE);
//         return EXIT_FAILURE;
//     }
//     int ans = fill_database(markov_chain_main);
//     if (ans == EXIT_FAILURE) {
//         free(markov_chain_main->database);
//         free(markov_chain_main);
//         printf(ALLOCATION_ERROR_MASSAGE);
//         return EXIT_FAILURE;
//     }
//     creat_path(markov_chain_main, (int) path_num); //generate path
//     free_markov_chain(&markov_chain_main);
//     return EXIT_SUCCESS;
// }
