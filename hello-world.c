#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h> 
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);

static struct etimer timer;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

	etimer_set(&timer, CLOCK_CONF_SECOND/4); 
	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(sht11_sensor);

	while(1){
		
		PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER); 

		float light_sensor_value = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
		float I = light_sensor_value/100000;
		float light_lx = 0.625*1e6*I*1000;

		int temp_sensor_value = sht11_sensor.value(SHT11_SENSOR_TEMP);
		int temperature =  0.01*temp_sensor_value - 39.6 ;

		printf("Light Sensor %d\n",light_lx);
		printf("Temp Sensor %d\n", temperature);

		if(temperature > 28 ){
		printf("Alarm");
		leds_on(LEDS_ALL);
		}
	
		leds_off(LEDS_ALL);
		etimer_reset(&timer);
	}

  

  PROCESS_END();
}
