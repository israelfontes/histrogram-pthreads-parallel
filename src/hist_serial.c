#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define SIGMA 0.2
#define MEAN 0.5
#define MAX_INTERVALS 1000

unsigned int seed;
long size_problem;

double rand_double() {
    return ( (double)rand_r(&seed) + 1. )/( (double)(RAND_MAX) + 1. );
}

double normalRandom() {
   double v1=rand_double();
   double v2=rand_double();
   return (cos(2*3.14*v2)*sqrt(-2.*log(v1)))*SIGMA+MEAN;
}


void write_vector_in_file(long * vector, int n){
	FILE * file;
	
	char file_name[30] = "data/size_";
    char size[30];
    sprintf(size, "%ld_serial.csv", size_problem);
    strcat(file_name, size);

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
    size_problem = atoi(argv[1])*10000;
    
    struct timeval start, stop;

    if(size_problem < MAX_INTERVALS){
        printf("Enter a value greater than: %d\n", MAX_INTERVALS);
        return 0;
    }

    seed = time(NULL);

    int intervals = rand_r(&seed) % MAX_INTERVALS;
    long * hist = malloc(sizeof(long)*intervals);
    
    gettimeofday(&start, 0);

    for(long i = 0; i < size_problem; i++){
        double value = normalRandom();
        int position = (int)(value*size_problem/(size_problem/intervals));
        
        if(position >= 0 && position < intervals)
            ++hist[position];
    }
    
    gettimeofday(&stop, 0);
    printf("%d", (int)(((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec))*100));

    write_vector_in_file(hist,intervals);
    
    free(hist);
    return 0;
}