// add -lm flag to compile

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <errno.h>
//#include <sys/wait.h>
#include "random437.h"
#include <math.h>

//Generating CSV file 
//instructions from https://dev.to/arepp23/how-to-write-to-a-csv-file-in-c-1l5b 
FILE *fpt;

//*************************************************//
#define MAXWAITPEOPLE 800
#define MAXTIME 1140 //1900 or 1140 mins
//#define MAXTIME 550

//Structure to organize all data points

struct {
	int MAXPERCAR;
	int CARNUM;
	float dq;
	int LASTCAR;
    int WAITING;
	float REJECTED;
	int totalriders; // need to fix total riders logic
	float totalppl;
	float rejectratio;
	int worstline;
	float avg;
	float avgwait;// total number of people in line in the queue (dq)/ total number of ppl(total ppl)
} logi;

//https://www.youtube.com/watch?v=ZxBn89Yx8M8&ab_channel=GodfredTech
//Utilize military time for ease of data handling
struct {
	int mins;
	int wait;
} TIME;

pthread_mutex_t shared_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER; 

void defaults(int cars, int cap) {
	logi.CARNUM = cars;
	logi.MAXPERCAR = cap;
	logi.dq = 0;
	TIME.mins = 540; // 9 am in mins
	TIME.wait=0;
	logi.LASTCAR = 1;
	logi.REJECTED = 0;
	logi.totalriders =0;
    logi.totalppl = 0;
    logi.rejectratio = 0;
    logi.worstline = 0;
	logi.avgwait = 0;
	logi.avg = 0;
}

int rejects(int inLine)
{
    int reject = 0;
    
    reject = inLine - MAXWAITPEOPLE;
    logi.totalppl += reject;
    
	logi.totalriders -= reject;
    logi.dq = MAXWAITPEOPLE;
    return reject;
}


void poissondefs() {
    

		if(TIME.mins>= 540 && TIME.mins<660) {
			logi.dq+=poissonRandom(25);
			logi.totalriders+=poissonRandom(25);// total riders need to follow reject stuff to make sure things are being accounted for right
			logi.totalppl+=poissonRandom(25); //total ppl needs to follow reject redundancy, maybe?
		    if(logi.dq>MAXWAITPEOPLE){
                logi.REJECTED += rejects(logi.dq);
            }   
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            }
			logi.avgwait += (logi.dq / logi.totalppl);
		} else if(TIME.mins>=660 && TIME.mins<840) {
			logi.dq+=poissonRandom(45);
			logi.totalriders+=poissonRandom(45);
			logi.totalppl+=poissonRandom(45);
            if(logi.dq>MAXWAITPEOPLE){
               logi.REJECTED += rejects(logi.dq);
            }
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            }
			logi.avgwait += (logi.dq / logi.totalppl);
		} else if(TIME.mins>=840 && TIME.mins<960) {
			logi.dq+=poissonRandom(35);
			logi.totalriders+=poissonRandom(35);
            logi.totalppl+=poissonRandom(35);
            if(logi.dq>MAXWAITPEOPLE){
                logi.REJECTED += rejects(logi.dq);
            }
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            } 
			logi.avgwait += (logi.dq / logi.totalppl);          
		} else if(TIME.mins>=960 && TIME.mins< MAXTIME) {
			logi.dq+=poissonRandom(25);
			logi.totalriders+=poissonRandom(25);
            logi.totalppl+=poissonRandom(25);
            if(logi.dq > logi.worstline){
	           logi.worstline=logi.dq;
            }
	        if(logi.dq>MAXWAITPEOPLE){
                logi.REJECTED += rejects(logi.dq);
            }
            logi.avgwait = (logi.dq / logi.totalppl);
		}
}

void* clockiterator() {
	while(TIME.mins<MAXTIME) {
		pthread_mutex_lock(&shared_mutex);
		
		poissondefs();
		printf("queue is:%f \n", logi.dq);
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



void* riding() {
    //printf("in car thread\n");
    while(TIME.mins<MAXTIME){
        pthread_mutex_lock(&shared_mutex);
		
		if(logi.dq >= logi.MAXPERCAR)
		{
			logi.dq-=logi.MAXPERCAR;
		}
		else
		{
			logi.dq = 0;
		}
		
	    // if this is the last car do this 
        if (logi.LASTCAR==logi.CARNUM){
            pthread_cond_signal(&cond2);
            logi.LASTCAR = 1;
            printf("last car go\n");
        }
        else{
            logi.LASTCAR++;
            printf("car go\n");
        }
        pthread_cond_wait(&cond, &shared_mutex); // total wait here?
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


void* threadhandler() {
    int line;

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

	fpt = fopen("PA05.csv", "w+");
	if(fpt == NULL)
	{
		perror("Could not open file");
		return 1;
	}

	logi.rejectratio = logi.REJECTED/logi.totalppl;

	logi.avg = logi.avgwait / (MAXTIME - TIME.mins);
	printf("File created");//Test to ensure file is created

	printf("%d", logi.REJECTED);
    
	fprintf(fpt, "N, M, Total Arrival, Total Go Away, Rejection Ratio, Average Wait Time(mins), Max Waiting Line\n");

	fprintf(fpt,"%d, %d, %.1f, %.1f, %f, %f, %d\n", N, M, logi.totalppl,logi.REJECTED, logi.rejectratio, logi.avg,logi.worstline);
	fclose(fpt);

	return 0;
}