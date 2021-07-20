#include <pthread.h> 
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define SIGMA 0.2
#define MEAN 0.5
#define MAX_INTERVALS 1000

pthread_mutex_t lock;
long size_problem;
int intervals;
long thread_count;
long * hist;

unsigned int seed;

double rand_double() {
    return ( (double)(rand_r(&seed)+ 1. )/( (double)(RAND_MAX) + 1. ));
}

double normalRandom() {
   double v1=rand_double();
   double v2=rand_double();
   return (cos(2*3.14*v2)*sqrt(-2.*log(v1)))*SIGMA+MEAN;
}

void * threadRandom(void * rank){
    long i;
    long my_size_problem = size_problem/thread_count;

    long * my_hist = malloc(sizeof(long)*intervals);
   
    for(i = 0; i < my_size_problem; i++){
        double value = normalRandom();
        int position = (int)(value*size_problem/(size_problem/intervals));
        
        if(position >= 0 && position < intervals)
            ++my_hist[position];
    }
    
    pthread_mutex_lock(&lock);
    
    for(i = 0; i < intervals; i++)
        hist[i]+=my_hist[i];
    
    pthread_mutex_unlock(&lock);

    free(my_hist);

    return NULL;
}

void write_vector_in_file(long * vector, int n){
	FILE * file;
	
	char file_name[50] = "data/size_";
    char size[30];
    sprintf(size, "%ld_threads_", size_problem);
    strcat(file_name, size);
    
    char threads[10];
    sprintf(threads, "%ld.csv", thread_count);
    strcat(file_name, threads);

	file = fopen(file_name,"wt");
    
	if(file != NULL){
		for(size_t i=0; i<n;i++){
			if(fprintf(file, "%ld;%ld \n",i,vector[i]) == EOF)
                printf("Fail! It's not possible write in file.");
        }
		fclose(file);
	}else{
		printf("Fail! It's not possible open file: %s.\n",file_name);
		exit(1);
	}
}



int main(int argc, char **argv){
    long thread;
    struct timeval start, stop;

    size_problem = atoi(argv[1])*10000;
    
    thread_count = atoi(argv[2]);

    if(size_problem < MAX_INTERVALS){
        printf("Enter a value greater than: %d\n", MAX_INTERVALS);
        return 0;
    }
    
    seed = time(NULL);
    
    intervals = rand_r(&seed) % MAX_INTERVALS;
    hist = malloc(sizeof(long)*intervals);
	
    pthread_t * thread_handles = malloc(thread_count*sizeof(pthread_t));
    
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 0; 
    }
    
    gettimeofday(&start, 0);
    
    for(thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, threadRandom, (void*) thread);
    
    for(thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    
    gettimeofday(&stop, 0);
    printf("%d", (int)(((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec))*100));

    pthread_mutex_destroy(&lock);
    
    write_vector_in_file(hist,intervals);
    
    free(thread_handles);
    free(hist);
    return 0;
}