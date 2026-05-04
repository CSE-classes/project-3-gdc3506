
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int n1;
char *s1;
FILE *fp;

//buffer size of 5 chars
#define N 5
char queue[N];
int head = 0;
int tail = 0;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
	// open file and get string
	if((fp=fopen("message.txt", "r"))==NULL){
		printf("ERROR: can't open message.txt!\n");
		return NULL;
	}

	char current_char;
	
	while((current_char = fgetc(fp)) != EOF) {
		pthread_mutex_lock(&mutex);

		// buffer full? then wait for consumer to consume
		while(count == N) {
			pthread_cond_wait(&empty, &mutex);
		}

		//insert char into buffer
		queue[tail] = current_char;
		// wrap for circular queue
		tail = (tail + 1) % N;
		count++;

		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);
	}
	//end of file reached! tell consumer this

	pthread_mutex_lock(&mutex);
	// if full, wait for consumer to consume something to make space
	while(count == N) {
		pthread_cond_wait(&empty, &mutex);
	}
	queue[tail] = '\0'; //add end marker
	count++;
	pthread_cond_signal(&full);
	pthread_mutex_unlock(&mutex);

	fclose(fp);
	return(NULL);
}

void *consumer(void *arg) {
	while(1) {
		pthread_mutex_lock(&mutex);

		// wait until producer produces something
		while(count == 0) {
			pthread_cond_wait(&full, &mutex);
		}

		//extract top char from queue
		char current_char = queue[head];
		// circular queue, so wrap if needed
		head = (head + 1) % N;
		count--;

		if(current_char == '\0') { // reached end?
			pthread_mutex_unlock(&mutex);
			break; //done reading
		}

		printf("%c", current_char);

		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
	}
	return(NULL);
}

//create producer and consumer threads
int main(int argc, char *argv[])
{
	pthread_t producer_t, consumer_t;

	pthread_create(&producer_t, NULL, producer, NULL);
	pthread_create(&consumer_t, NULL, consumer, NULL);

	pthread_join(producer_t, NULL);
	pthread_join(consumer_t, NULL);

	pthread_exit(0);
}