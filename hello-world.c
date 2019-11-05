#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h> 
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"


/*---------------------------------------------------------------------------*/

static process_event_t notification_event_ready;
PROCESS(fire_detector_process, "Fire Detector process");
PROCESS(notification_process, "Notification Process");
AUTOSTART_PROCESSES(&fire_detector_process , &notification_process);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(fire_detector_process, ev, data)
{
	static struct etimer timer;
 	PROCESS_BEGIN();
	notification_event_ready = process_alloc_event();
	etimer_set(&timer, CLOCK_CONF_SECOND/4); 
	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(sht11_sensor);

	while(1){
		
		PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER); 

		double light_sensor_value = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
		double I = light_sensor_value/100000;
		double light_lx = 0.625*1e6*I*1000;

		int temp_sensor_value = sht11_sensor.value(SHT11_SENSOR_TEMP);
		int temperature =  0.01*temp_sensor_value - 39.6 ;

		printf("Light Sensor %d\n",light_lx);
		printf("Temp Sensor %d\n", temperature);

		if(temperature >= 47  ){

		process_post(&notification_process, notification_event_ready, &temperature);
		

		}
	
		
		etimer_reset(&timer);
	}

 	PROCESS_END();
}

		PROCESS_THREAD(notification_process , ev , data){

			PROCESS_BEGIN();
			while(1)
			{
			PROCESS_WAIT_EVENT_UNTIL(ev == notification_event_ready);
			printf("Fire Detected , Temperature - %d\n", data);
			leds_on(LEDS_ALL);
			}
			leds_off(LEDS_ALL);
			PROCESS_END();
		}
