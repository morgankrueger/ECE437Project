#include <stdio.h>
#include <stdlib>
#include <unistd.h>
#include <getopt.h>
#include "random437.h"
#define MAXWAITPEOPLE 800
#define MAXTIME 19 //1900 is 7pm in military time


//Structire to organize all data points

struct{
    int MAXPERCAR;
    int CARNUM;
}logi;

//https://www.youtube.com/watch?v=ZxBn89Yx8M8&ab_channel=GodfredTech
//Utilize military time for ease of data handling
struct{
    int hours; 
    int minutes;
    int seconds;
} TIME;

void defaults(int cars, int cap){
    logi.CARNUM= cars;
    logi.MAXPERCAR= cap;
    TIME.hrs=9;
    TIME.mins=0;
    TIME.secs=0;
}

int poissondefs(){
    int temp=0;
    
    if(TIME.hours<11){
        temp=poissonRandom(25);
    }else if(TIME.hrs>=11 && TIME.hrs<14){
        temp=poissonRandom(45);
    }else if(TIME.hrs>=14 && TIME.hrs<16){
         temp=poissonRandom(35);
    }else if(TIME.hrs>=16){
         temp=poissonRandom(25);
    }

    return temp;
}
    
void clockiterator{
    h= TIME.hrs;
    m=TIME.mins;
    s=TIME.secs;
while(h<MAXTIME){
   if(m==59){
    h++;
    m=0;
    }else{
        if(s==59){
            m++;
            s=0;
        }else{
            s++}
        }
        
    
}
    TIME.hrs=h;
    TIME.mins=m;
    TIME.secs=s;
    //how to get possiondefs the time as the clock iterates through
}



// export data to csv file

void threadhandler(){
    
}




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
		defaults(N,M);
		threadhandler();
		return 0;
	}
