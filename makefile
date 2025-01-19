OBJ=markov_chain.c markov_chain.h linked_list.c linked_list.h
tweets: $(OBJ) tweets_generator.c
	gcc -Wall -Wextra -Wvla $^ -o tweets_generator
snake:$(OBJ) snakes_and_ladders.c
	gcc -Wall -Wextra -Wvla $^ -o snakes_and_ladders