#include "contiki.h"
#include "contiki-conf.h"
#include <stdio.h> 
#include "ringbuf.h"
#include "dev/sht11-sensor.h"

/*---------------------------------------------------------------------------*/
PROCESS(fifo_process, "FIFO process");
AUTOSTART_PROCESSES(&fifo_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(fifo_process, ev, data)
{
	PROCESS_BEGIN();


	typedef struct {
	    uint8_t * const buffer;
	    int head;
	    int tail;
	    const int maxlen;
	} circ_bbuf_t;


	int circ_bbuf_push(circ_bbuf_t *c, uint8_t data)
	{
	    int next;

	    next = c->head + 1;  // next is where head will point to after this write.
	    if (next >= c->maxlen)
		next = 0;

	    if (next == c->tail)  // if the head + 1 == tail, circular buffer is full
		return -1;

	    c->buffer[c->head] = data;  // Load data and then move
	    c->head = next;             // head to next data offset.
	    return 0;  // return success to indicate successful push.
	}

	int circ_bbuf_pop(circ_bbuf_t *c, uint8_t *data)
	{
	    int next;

	    if (c->head == c->tail)  // if the head == tail, we don't have any data
		return -1;

	    next = c->tail + 1;  // next is where tail will point to after this read.
	    if(next >= c->maxlen)
		next = 0;

	    *data = c->buffer[c->tail];  // Read data and then move
	    c->tail = next;              // tail to next offset.
	    return 0;  // return success to indicate successful push.
	}

	             
	  
	CIRC_BBUF_DEF(my_circ_buf, 32);

 	//struct ringbuf *buffer;
	//uint8_t *data;

  	//ringbuf_init(buffer, data , 64);
	//SENSORS_ACTIVATE(sht11_sensor);

	//while(1){
		
	//	int temp_sensor_value = sht11_sensor.value(SHT11_SENSOR_TEMP);
	//	int temperature =  0.01*temp_sensor_value - 39.6 ;
	//	uint8_t y = temperature;
	//	printf("Temperature is = %d\n", y);
	//	ringbuf_put(buffer , y);

	//	int buffersize = ringbuf_size(buffer);
	//	int getbuffer = ringbuf_get(buffer);
	//	printf("Buffer Size is %d\n",buffersize);
	//	printf("Buffer Element is %d\n",getbuffer);

	//}


 	PROCESS_END();
}

	
