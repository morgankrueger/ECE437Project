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
#include <stdbool.h>
#define PRINTFILE true
//Generating CSV file 
//instructions from https://dev.to/arepp23/how-to-write-to-a-csv-file-in-c-1l5b 


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
	float totalriders; // need to fix total riders logic
	float totalppl;
	float rejectratio;
	int worstline;
	float avg;
	float avgwait;// total number of people in line in the queue (dq)/ total number of ppl(total ppl)
	float arrivals_per_minute;
	float rejected_per_minute;
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
	logi.arrivals_per_minute = 0;
	logi.rejected_per_minute = 0;
}

float rejects(float inLine)
{
    float reject = 0;
    
    reject = inLine - MAXWAITPEOPLE;
    logi.totalppl += reject;
    
	logi.totalriders -= reject;
    logi.dq = MAXWAITPEOPLE;
    return reject;
}


void poissondefs() {

		if(TIME.mins>= 540 && TIME.mins<660) {
			logi.arrivals_per_minute = poissonRandom(25);
			logi.dq+=logi.arrivals_per_minute;
			logi.totalriders+=logi.arrivals_per_minute;// total riders need to follow reject stuff to make sure things are being accounted for right
			logi.totalppl+=logi.arrivals_per_minute; //total ppl needs to follow reject redundancy, maybe?
		    if(logi.dq>MAXWAITPEOPLE){
                logi.rejected_per_minute = rejects(logi.dq);
				logi.REJECTED += logi.rejected_per_minute;
            }   
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            }
			logi.avgwait += (logi.dq / logi.totalppl);
		} else if(TIME.mins>=660 && TIME.mins<840) {
			logi.arrivals_per_minute = poissonRandom(45);
			logi.dq+=logi.arrivals_per_minute;
			logi.totalriders+=logi.arrivals_per_minute;
			logi.totalppl+=logi.arrivals_per_minute;
            if(logi.dq>MAXWAITPEOPLE){
                logi.rejected_per_minute = rejects(logi.dq);
				logi.REJECTED += logi.rejected_per_minute;
            }
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            }
			logi.avgwait += (logi.dq / logi.totalppl);
		} else if(TIME.mins>=840 && TIME.mins<960) {
			logi.arrivals_per_minute = poissonRandom (35);
			logi.dq+=logi.arrivals_per_minute;
			logi.totalriders+=logi.arrivals_per_minute;
            logi.totalppl+=logi.arrivals_per_minute;
            if(logi.dq>MAXWAITPEOPLE){
                logi.rejected_per_minute = rejects(logi.dq);
				logi.REJECTED += logi.rejected_per_minute;
            }
            if(logi.dq> logi.worstline){
	           logi.worstline=logi.dq;
            } 
			logi.avgwait += (logi.dq / logi.totalppl);          
		} else if(TIME.mins>=960 && TIME.mins< MAXTIME) {
			logi.arrivals_per_minute = poissonRandom(25);
			logi.dq+=logi.arrivals_per_minute;
			logi.totalriders+=logi.arrivals_per_minute ;
            logi.totalppl+=logi.arrivals_per_minute;
            if(logi.dq > logi.worstline){
	           logi.worstline=logi.dq;
            }
	        if(logi.dq>MAXWAITPEOPLE){
                logi.rejected_per_minute = rejects(logi.dq);
				logi.REJECTED += logi.rejected_per_minute;
            }
		}
}

void printing(FILE *fpt)
{
	fprintf(fpt,"%d, %.1f, %.1f, %.f\n", (TIME.mins -540), logi.arrivals_per_minute, logi.dq, logi.rejected_per_minute);
}

void* clockiterator() {
	char buffer[25];
    sprintf(buffer, "%dcars%dseats.csv", logi.CARNUM, logi.MAXPERCAR);
    char *filename = buffer;
	FILE *fpt;
	fpt = fopen(filename, "w+");
	if(fpt == NULL)
	{
		perror("Could not open file");
	}
	fprintf(fpt, "TimeMins, TotalArrivalPerMinute, WaitingQueue, Rejected\n");

	while(TIME.mins<MAXTIME) {
		pthread_mutex_lock(&shared_mutex);
		
		poissondefs();
		printf("queue is:%f at time:%d\n", logi.dq, (TIME.mins - 540));
		pthread_cond_broadcast(&cond);
		printf("broadcast\n");
		pthread_cond_wait(&cond2, &shared_mutex);
		printf("wait\n\n");
		printing(fpt);
		TIME.mins++;// = m;
		pthread_mutex_unlock(&shared_mutex);
	}
	fclose(fpt);
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
	
	logi.rejectratio = logi.REJECTED/logi.totalppl;
	printf("Total Arrival, Total Rejected, Reject Ratio,  Average Wait,   Max Waiting Line\n");
	printf("%.1f,\t %.1f,\t %f,\t %f,\t %d\n", logi.totalppl,logi.REJECTED, logi.rejectratio, logi.avgwait,logi.worstline);

	return 0;
}