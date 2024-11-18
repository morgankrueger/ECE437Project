#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "random437.h"
#define MAXWAITPEOPLE 800
#define MAXTIME 19 //1900 is 7pm in military time


Structure to organize all data points

typedef struct{
    int MAXPERCAR;
    int CARNUM;
}logi;

//https://www.youtube.com/watch?v=ZxBn89Yx8M8&ab_channel=GodfredTech
//Utilize military time for ease of data handling
typedef struct{
    int hours; 
    int minutes;
} CLOCK;

void defaults(int cars, int cap){
	logi.CARNUM= cars;
    logi.MAXPERCAR= cap;
    CLOCK.hours=9;
    CLOCK. minutes=0;
    
    
}

int arrivalRate(int minutes)
{
	int rate = 0;
	if(minutes <= 120)
	{
		minutes = 25;
	}
	else if (minutes > 120 && minutes <= 240)
	{
		minutes = 45;
	}
	else if (minutes > 240 && minutes <= 360)
	{
		minutes = 35;
	}
	else 
	{
		minutes = 25;
	}
	rate = poissonRandom(minutes);

	printf("The rate of arrival is %d", rate);

	return rate;
}



// export data to csv file






int main(int argc, char** argv)
{
	//use command line grabber from PA02
	int N=0;
	int M=0;
	int opt=0;
	while ((opt = getopt(argc, argv, "F:S:")) != -1) {
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
		}}
		
		return 0;
	}
