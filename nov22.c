#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <errno.h>
//#include <sys/wait.h>
#include "random437.h"
#include <math.h>

//*************************************************//
#define MAXWAITPEOPLE 800
//#define MAXTIME 1140 //1900 or 1140 mins
#define MAXTIME 550

//Structure to organize all data points

struct {
	int MAXPERCAR;
	int CARNUM;
	int dq;
	int LASTCAR;
    int WAITING;
} logi;

//https://www.youtube.com/watch?v=ZxBn89Yx8M8&ab_channel=GodfredTech
//Utilize military time for ease of data handling
struct {
	int mins;
} TIME;

pthread_mutex_t shared_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER; //COND 2 is initialized wrong for use in iterator function it needs to be added or decremented

void defaults(int cars, int cap) {
	logi.CARNUM = cars;
	logi.MAXPERCAR= cap;
	logi.dq=0;
	TIME.mins=540; // 9 am in mins
	logi.LASTCAR=1;
    logi.WAITING = 0;
}

void poissondefs() {

	if(logi.dq< MAXWAITPEOPLE) {
		if(TIME.mins>= 540 && TIME.mins<660) {
			logi.dq+=poissonRandom(25);
		} else if(TIME.mins>=660 && TIME.mins<840) {
			logi.dq+=poissonRandom(45);
		} else if(TIME.mins>=840 && TIME.mins<960) {
			logi.dq+=poissonRandom(35);
		} else if(TIME.mins>=960 && TIME.mins< MAXTIME) {
			logi.dq+=poissonRandom(25);
		} else {

			printf("MAX QUEUE\n");
		}
	}
}

void* clockiterator() {
	while(TIME.mins<MAXTIME) {
		pthread_mutex_lock(&shared_mutex);
		
		poissondefs();
		printf("queue is:%d \n", logi.dq);
		pthread_cond_broadcast(&cond);
		printf("broadcast\n");
		pthread_cond_wait(&cond2, &shared_mutex);
		printf("wait\n\n");
		TIME.mins++;// = m;
		pthread_mutex_unlock(&shared_mutex);
	}

    pthread_cond_broadcast(&cond);
	printf("We are CLOSED!\n");
}

// export data to csv file


void* riding() {
    //printf("in car thread\n");
    while(TIME.mins<MAXTIME){
        pthread_mutex_lock(&shared_mutex);
		logi.dq-=logi.MAXPERCAR;
	    // if this is the last car do this \/\/\/\/
        if (logi.LASTCAR==logi.CARNUM){
            pthread_cond_signal(&cond2);
            logi.LASTCAR = 1;
            printf("last car go\n");
        }
        else{
            logi.LASTCAR++;
            printf("car go\n");
        }
        pthread_cond_wait(&cond, &shared_mutex); //mutex lock too???!?!
        pthread_mutex_unlock(&shared_mutex);   
    }
}

void* ridehandler() {
	pthread_t ite[logi.CARNUM + 1];
	//parallel thread creation from the slides

	// Thread for iterator goes HERE
	pthread_create(&ite[0], NULL, clockiterator,NULL);
	usleep(1000);
	// Threads for car goes HERE
	for(int x=1; x <= logi.CARNUM; x++) {
		pthread_create(&ite[x], NULL, riding,NULL);
	}
	for(int y=0; y <= logi.CARNUM; y++) {
		pthread_join(ite[y], NULL);
	}

    return NULL;
}

int waitingLine()
{

}

int rejects(int inLine)
{
    int reject = 0;
    if(inLine > MAXWAITPEOPLE)
    {
        reject = inLine - MAXWAITPEOPLE;
    }
    if (inLine <= MAXWAITPEOPLE)
    {
        return;
    }

    return reject;
}

void* threadhandler() {
    int line;
	// are these thread creations even necessarry or should i just call ridehandler and q handler
	//pthread_mutex_init(&shared_mutex, NULL);
	//pthread_cond_init(&cond,NULL);
	//pthread_cond_init(&cond2,NULL);

	ridehandler();
    rejects(line);
}

int main(int argc, char** argv)
{
	//use command line grabber from PA02
	int N=0;
	int M=0;
	int opt=0;
	while ((opt = getopt(argc, argv, "N:M:")) != -1) {
		switch (opt) {
		case 'N':
			N = atoi(optarg);
			break;
		case 'M':
			M = atoi(optarg);
			break;
		default:
			printf("ERROR");
			break;
		}
	}
	defaults(N,M);
	threadhandler();
	return 0;
}
