#include "contiki.h"
#include <stdio.h> 
#include "dev/sht11-sensor.h"
#include <stdlib.h>
#include <math.h>
/*---------------------------------------------------------------------------*/
PROCESS(fifo_process, "FIFO process");
AUTOSTART_PROCESSES(&fifo_process);

PROCESS_THREAD(fifo_process, ev, data)
{
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(sht11_sensor);

	#define Buffer_ELEMENTS 12
	#define Buffer_SIZE (Buffer_ELEMENTS + 1)
	int Head, Tail;
	int Buffer[Buffer_SIZE];

void InitBuffer(void)
{
    Head = Tail = 0;
}

int BufferPush(int data)
{
    if(Head == (( Tail - 1 + Buffer_SIZE) % Buffer_SIZE))
    {
        return -1; 
    }

    Buffer[Head] = data;

    Head = (Head + 1) % Buffer_SIZE;

    return 0; 
}

int BufferPop(int *old)
{
    if(Head == Tail)
    {
        return -1; 
    }

    *old = Buffer[Tail];

    Tail = (Tail + 1) % Buffer_SIZE;

    return 0; 
}


float standard_deviation(int arr[])
{
float sum = 0.0, mean, standDev = 0.0;
float stddev;
int i;
for(i=0; i<10; ++i)
{
sum += arr[i];
}
mean = sum/10;
for(i=0; i<10; ++i)
{standDev += pow(arr[i] - mean, 2);}
stddev = sqrtf(standDev/10);
return stddev;
}



	InitBuffer();

	while(1){
		int i;
		int j;
		int l;
		float stddev;
		int stds;
		int temp_sensor_value = sht11_sensor.value(SHT11_SENSOR_TEMP);
		int temperature =  0.01*temp_sensor_value - 39.6 ;
		
		BufferPush(temperature);
		
		if(Head == (( Tail - 1 + Buffer_SIZE) % Buffer_SIZE))
    		{
		 printf("\nBuffer Full");
       		 break; 
    		}

		for(i=0; i <Buffer_SIZE; i++){

		printf("\nBuffer Elements %d", Buffer[i]);

		stddev = standard_deviation(Buffer);
		stds = (int)stddev;
		int k = (stddev - stds) * 10000;
		printf("\nStandard Deviation = %d.%d", stds , k);

		if(stds < 30){ //low activity
			// No interesting activity occured
			for(j=0; j< Buffer_SIZE ; j+=4){
				for(l=j; l<=4; l++){
				printf("\nEvery four numbers %d",Buffer[l]);
			}
			}
			
		}
		else if( stds > 30 && stds < 200){ //High activity
				for(j=0; j< Buffer_SIZE ; j+=2){
				for(l=j; l<=2; l++){
				printf("\nEvery two numbers %d",Buffer[l]);
			}
			}
		}
		else if( stds > 200) { // Extremely high activity
			// No Aggregation
		}
		}
		
		
	}
	
	
	
 	PROCESS_END();
}

