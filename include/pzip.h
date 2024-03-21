#ifndef PZIP_H
#define PZIP_H

#include <stdint.h>
#include <pthread.h>

/**
 * The struct that holds all the thread data
 * 
 */
struct ThreadData{
    int start_index;
    int end_index;
    char *input_chars;
    int *char_frequency;
	pthread_mutex_t *mutex;
	pthread_barrier_t *barrier; 
};

/**
 * The struct that holds consecutive character-occurence pairs.
 */
struct zipped_char {
	char character;
	uint8_t occurence;
};
/**
*callback_function() - callback pthread function
*Inputs:
*@args:    
*/
void *callback_function(void *args);
/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */
void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency);


#endif /* PZIP_H */
