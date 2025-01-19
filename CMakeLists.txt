cmake_minimum_required(VERSION 3.28)
project(GenericMarkovChain C)

set(CMAKE_C_STANDARD 11)

add_executable(GenericMarkovChain
        markov_chain.c
        markov_chain.h
        snakes_and_ladders.c
        linked_list.c
        tweets_generator.c)
