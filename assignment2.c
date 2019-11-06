#include "contiki.h"
#include <stdio.h> 
#include "dev/sht11-sensor.h"
#include <stdlib.h>
/*---------------------------------------------------------------------------*/
PROCESS(fifo_process, "FIFO process");
AUTOSTART_PROCESSES(&fifo_process);


int Buffer_Size = 4;
int *CircBuffer;
int Head = -1;
int Tail = -1;
int isfull_flag = 0;

PROCESS_THREAD(fifo_process, ev, data)
{
	PROCESS_BEGIN();


	void pushtoBuffer(int data){

	if((Tail == Buffer_Size -1 && Head == 0)||(Tail + 1 == Head)){
		printf("\n Buffer is Full");
	}
	if(Tail == Buffer_Size -1){
		Tail = 0;
	}
	else {
		Tail++;
		CircBuffer[Tail] = data;
		printf("\n Write to buffer : %d" , CircBuffer[Tail]);
		if(Head == -1){
			Head = 0;		
		}
	}
	}

	void popfromBuffer(){
		
		if(Head == -1){
			printf("Circular queue is empty");
		}
		data = CircBuffer[Head];
		printf("\n Item popping= %d",data);
		CircBuffer[Head] = 0;
		if(Head == Tail){
			Head = Tail = -1;		
		}
		else{
			if(Head == Buffer_Size-1){
			Head = 0;
			}
			else
			{		
				Head++;
			}
			printf("\nDeleted Item = %d",data);
		
		}
	}

	void printBuffer(){
	int i;
	printf("\n Circular Buffer Elements \n");
		for( i=0; i<Buffer_Size; i++){
			printf("%d\t", CircBuffer[i]);
		}
	
	}

	
	
	pushtoBuffer(30);
	printf(" Head = %d Tail = %d" , Head, Tail);
	pushtoBuffer(40);
	printf(" Head = %d Tail = %d" , Head, Tail);
	popfromBuffer();
	printf(" Head = %d Tail = %d" , Head, Tail);
	pushtoBuffer(60);
	printf(" Head = %d Tail = %d" , Head, Tail);
	printBuffer();

 	PROCESS_END();
}


