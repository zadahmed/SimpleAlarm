#include "contiki.h"
#include <stdio.h> 
#include "dev/light-sensor.h"
#include <stdlib.h>
#include <math.h>

/*---------------------------------------------------------------------------*/
PROCESS(fifo_process, "FIFO process");
AUTOSTART_PROCESSES(&fifo_process);

PROCESS_THREAD(fifo_process, ev, data)
{
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(light_sensor);

	#define Buffer_ELEMENTS 12
	#define Buffer_SIZE (Buffer_ELEMENTS + 1)
	int Head, Tail;
	float Buffer[Buffer_ELEMENTS];
	float mean;
	float meanfour;
	float AggBuffer[Buffer_SIZE];
	int aggValue = 0;

void InitBuffer(void)
{
    Head = Tail = 0;
}

void BufferPush(float data)
{
  if(Head == Buffer_ELEMENTS)
    {
       Head = 0;

    }

    Buffer[Head] = data;

    Head = (Head + 1);
}

void AggBufferPush(float data)
{
  if(Head == Buffer_ELEMENTS)
    {
       Head = 0;

    }

    AggBuffer[Head] = data;

    Head = (Head + 1);
}

// void BufferPop()
// {
//   if(Tail == Buffer_SIZE)
//     {
//        Tail = 0;

//     }

//     //Buffer[Tail] = data;

//     Tail = (Tail + 1);
// }


float standard_deviation(float arr[])
{
	float sum = 0.0, standDev = 0.0;
	float stddev;
	int i;
	for(i=0; i<12; i++)
	{
	sum += arr[i];
	}
	mean = sum/12;

	for(i=0; i<12; ++i)
	{standDev += pow(arr[i] - mean, 2);}
	stddev = sqrtf(standDev/12);
	return stddev;
}

	float sum1 = 0;
	InitBuffer();

	while(1){
		int i;
		int j;
		int m;
    	float zi[Buffer_SIZE];
		int l;
		float stddev;
		int stds;

		//Get Light
		double light_sensor_value = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
		double I = light_sensor_value/100000;
		double light_lx = 0.625*1e6*I*1000;

		int intlight = (int)light_lx;
		int ll = (int)(light_lx - intlight);
		printf("\nLight = %d.%d", intlight , ll);
		BufferPush(light_lx);  //Push Light

		
		for(i=0; i <Buffer_ELEMENTS; i++){
			int BufferElement = (int)Buffer[i];
			int BuffDiff = (Buffer[i] - BufferElement) * 10000;
			printf("\nBuffer Elements %d.%d", BufferElement,BuffDiff);

		}
		
		//printf("\n summmmmmm %d\n", (int)sum1);
		if(Head == Buffer_ELEMENTS){
			stddev = standard_deviation(Buffer);
			stds = (int)stddev;
			int k = (stddev - stds) * 10000;
			printf("\nStandard Deviation = %d.%d", stds , k);



			int cnt = 0;
			if (stds < 25){
				aggValue = 4;
				
			for(j=0; j <= 8 ; j += 4){
					// printf("\nJ is %d",j);
					if(j==0){
						sum1 = 0;
						for(l=0; l<4; l++){
						sum1 += Buffer[l];

						// printf("\nL is %d", l);
					}
					sum1 = sum1/4;
					int sum1int = (int)sum1;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);

					}
					 else if(j==4){
						 sum1 = 0;
						for(l=4; l<8; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/4;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);
					}
					else  if(j==8){
						 sum1 = 0;
						for(l=8; l<12; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					sum1 = sum1/4;
					int sum1int = (int)sum1;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);
					}
					else if(j==12){
						printf('\nBuffer ended');
					}
	 				
			}

			printf("X = [");
			int ctr1;
			for ( ctr1 = 0; ctr1<aggValue; ctr1++){
				int AggBufferElement = (int)AggBuffer[ctr1];
				int AggBuffDiff = (AggBuffer[ctr1] - AggBufferElement) * 10000;
				printf("\nAgg Buffer Elements %d.%d", AggBufferElement,AggBuffDiff);
				printf( ",");
			}
			printf("]");
			
			}

			if(stds > 25 && stds < 200)
			{

				for(j=0; j <= 10 ; j += 2){
				aggValue = 2;
				if(j==0){
						sum1 = 0;
						for(l=0; l<2; l++){
						sum1 += Buffer[l];

						// printf("\nL is %d", l);
					}
					sum1 = sum1/2;
					int sum1int = (int)sum1;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);

					}
					 else if(j==2){
						 sum1 = 0;
						for(l=2; l<4; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/2;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);
					}
					 else if(j==4){
						 sum1 = 0;
						for(l=4; l<6; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/2;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);
					}

					 else if(j==6){
						 sum1 = 0;
						for(l=6; l<8; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/2;
					int sum1diff = (sum1 - sum1int) * 10000;
					// printf("\n summ %d.%d\n", sum1int , sum1diff);
					AggBufferPush(sum1);
					}

					 else if(j==8){
						 sum1 = 0;
						for(l=8; l<10; l++){
						sum1 += Buffer[l];
						// printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/2;
					int sum1diff = (sum1 - sum1int) * 10000;
					AggBufferPush(sum1);
					}

					 else if(j==10){
						 sum1 = 0;
						for(l=10; l<12; l++){
						sum1 += Buffer[l];
						printf("\nL is %d", l);
					}
					int sum1int = (int)sum1;
					sum1 = sum1/2;
					int sum1diff = (sum1 - sum1int) * 10000;
					AggBufferPush(sum1);
					}


					printf("X = [");
			}
			
			int ctr1;
			for ( ctr1 = 0; ctr1<6; ctr1++){
				int AggBufferElement = (int)AggBuffer[ctr1];
				int AggBuffDiff = (AggBuffer[ctr1] - AggBufferElement) * 10000;
				printf("\nAgg Buffer Elements %d.%d", AggBufferElement,AggBuffDiff);
				printf( ",");
			}
			printf("]");

		}
		
		}
	}

	
 	PROCESS_END();
}


