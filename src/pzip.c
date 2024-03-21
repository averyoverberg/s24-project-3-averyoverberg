#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"


/**
*callback_function() - callback pthread function
*Inputs:
*@args: 
*/
void *callback_function(void *args){
	// Cast arg to the appropriate type that contains the data my thread needs
	struct ThreadData *thread_data = (struct ThreadData *)args;

	//setting local variables for easy access
	int start_index = thread_data->start_index;
	int end_index = thread_data->end_index;
	char *input_chars = thread_data->input_chars;
	int *char_frequency = thread_data->char_frequency;
	pthread_mutex_t *mutex = thread_data->mutex;
	pthread_barrier_t *barrier = thread_data->barrier;

	int local_char_freq[26]={0};

	//iterating through portion of input
	for (int i = start_index; i<end_index; i++){
		if (input_chars[i] >= 'a' && input_chars[i]<= 'z'){
			local_char_freq[input_chars[i] - 'a']++;

		}
	}

	//lock mutex
	pthread_mutex_lock(mutex);

	for (int i = 0; i<26; i++){
		char_frequency[i] += local_char_freq[i];
	}

	//unlock mutex
	pthread_mutex_unlock(mutex);

	// Wait at the barrier for all threads to synchronize
    pthread_barrier_wait(barrier);

	pthread_exit(NULL);
}

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characters in the input file
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
          int *char_frequency)
{
    // Calculate portion size & remainder
    int portion_size = input_chars_size / n_threads;
    int remainder = input_chars_size % n_threads;

    // Initialize start and end index, rc, and barrier
    int start_index = 0;
    int end_index = 0;
    int rc;
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, n_threads);

    // Initialize mutex
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Array of thread IDs
    pthread_t thread_ids[n_threads];

    // Array of ThreadData structures
    struct ThreadData thread_data[n_threads];

    for (int i = 0; i < n_threads; i++) {
        end_index = start_index + portion_size - 1;
        if (i < remainder) {
            end_index++;
        }

        thread_data[i].start_index = start_index;
        thread_data[i].end_index = end_index;
        thread_data[i].input_chars = input_chars;
        thread_data[i].char_frequency = char_frequency;
        thread_data[i].mutex = &mutex;
		thread_data[i].barrier = &barrier;

        rc = pthread_create(&thread_ids[i], NULL, callback_function, (void *)&thread_data[i]);
        //if create fails
		if (rc) {
    		// deleting threads that have already been created
			for (int j = 0; j < i; j++) {
				if (pthread_join(thread_ids[j], NULL)) {
					pthread_cancel(thread_ids[j]);
				}
			}
			pthread_mutex_destroy(&mutex);
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
			}

        start_index = end_index + 1; // Update start index for the next thread
    }

    // Join threads
    for (int i = 0; i < n_threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

	exit(0);
}

