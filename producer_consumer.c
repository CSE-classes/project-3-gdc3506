
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
	
	while((current = fgetc(fp)) != EOF) {
		pthread_mutex_lock(&mutex);

		// buffer full? then wait
		while(count == N) {
			pthread_cond_wait(&empty, &mutex);
		}

		//insert item into buffer
		queue[tail] = current_char;
		// wrap for circular queue
		tail = (tail + 1) % N;
		count++;

		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);
	}
}

void *consumer(void *arg) {

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