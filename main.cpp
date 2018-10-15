#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>


#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N
 


int forks[] = {1,1,1,1,1}; // 1 if free 0 if taken
int phil[N] = { 0, 1, 2, 3, 4 }; // philosophers
pthread_mutex_t mutex[5]; // mutex 
pthread_cond_t cond_var[5]; // conditionals

// take up chopsticks
void take_fork(int phnum){ 
	int next= (phnum+1)%5;
	//lock left fork
	pthread_mutex_lock(&mutex[phnum]);
	printf("Philosoper %d needs left fork. \n",phnum);
	// wait for left fork
	while(forks[phnum] !=1) {
			pthread_cond_wait(&mutex[phnum], &cond_var[phnum]);
	}
	// left fork now taken
	forks[phnum]=0;
	// signal
	pthread_cond_signal(&cond_var[phnum]); 
	// unlock
	pthread_mutex_unlock(&mutex[phnum]); 
	//lock right fork
	pthread_mutex_lock(&mutex[next]);
	printf("Philosopher %d picked up left fork and needs right. \n", phnum);
    while(forks[next]!=1){ // right fork unavailable
    	pthread_cond_wait(&mutex[next], &cond_var[next]);
   	}
   	forks[next] = 0; // take right fork 
   	pthread_cond_signal(&cond_var[next]); //signal data change
   	pthread_mutex_unlock(&mutex[next]); // unlock

   	printf("Philosopher %d picked up its right fork and has both forks. \n", phnum);
	
}

// put down chopsticks
void put_fork(int phnum){


	printf("Philosopher %d is done eating \n",phnum);

 	int next= (phnum+1)%5;
	//left fork
	pthread_mutex_lock(&mutex[phnum]);//lock left fork
	//printf("Philosopher %d is done locking left \n",phnum);

	//printf("Philosopher %d is returning left fork. \n", phnum);
	forks[phnum]=1; // put fork back
	pthread_cond_signal(&cond_var[phnum]);  // signal
	pthread_mutex_unlock(&mutex[phnum]);  // unlock

	//right fork
	pthread_mutex_lock(&mutex[next]); //lock right fork
	//printf("Philosopher %d is returning left fork. \n", phnum);
	forks[next]=1; // put fork back
 	pthread_cond_signal(&cond_var[next]); //signal data change
 	
 	pthread_mutex_unlock(&mutex[next]); // unlock

 	printf("Philosopher %d has no forks. \n", phnum);

}

void* philospher(void* num)
{
 
    while (1) {
 
        int id = *(int*)num;
 
		int time = rand() % 3 +1;

		printf("Philosopher %d is thinking for %d seconds \n",id,time);
		
		sleep(time);


		take_fork(id);

		//eating time!

		printf("Philosopher %d is eating for %d seconds \n",id,time);
		sleep(time);

		put_fork(id);

		
 
    }
}

int main()
{
 	
 	srand(time(NULL));
 	
 	int i;
    
    pthread_t threads[5];
 
    for (i = 0; i < N; i++){
 
		pthread_mutex_init(&mutex[i],NULL);
		pthread_cond_init(&cond_var[i],NULL);
	}

    for (i = 0; i < N; i++) {
 
       	
       	printf("Creating Philosopher %d \n", i );

        pthread_create(&threads[i], NULL,philospher, &phil[i]);
 
    }
 
    for (i = 0; i < N; i++)
 
        pthread_join(threads[i], NULL);

    return 0;
}





 
