#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h> 
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include "powertrace.h"
#include "net/rime.h"



#define COOJA_PORT 1234
/*---------------------------------------------------------------------------*/

static process_event_t notification_event_ready;
PROCESS(fire_detector_process, "Fire Detector process");
PROCESS(notification_process, "Notification Process");
AUTOSTART_PROCESSES(&fire_detector_process , &notification_process);

	int count=0;
	double temparray[10];
	double lightarray[10];
	double tempsum = 0.0;
	double lightsum = 0.0;
	double mean_temperature;
	double mean_light;
	int intmeantemp ;
	int meantt ;
	int tempthershold;
	int lighthreshold;


/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
         from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(fire_detector_process, ev, data)
{
	static struct etimer timer;
 	PROCESS_BEGIN();

	unicast_open(&uc, 146, &unicast_callbacks);


	notification_event_ready = process_alloc_event();
	etimer_set(&timer, CLOCK_CONF_SECOND); 
	SENSORS_ACTIVATE(light_sensor);
	SENSORS_ACTIVATE(sht11_sensor);
	int n = 1; 
	powertrace_start(CLOCK_SECOND * n);

	while(1){
		
		PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER); 
		rimeaddr_t addr;

		double light_sensor_value = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
		double I = light_sensor_value/100000;
		double light_lx = 0.625*1e6*I*1000;

		int intlight = (int)light_lx;
		int ll = (int)(light_lx - intlight);

		int temp_sensor_value = sht11_sensor.value(SHT11_SENSOR_TEMP);
		int temperature =  0.01*temp_sensor_value - 39.6 ;


		int inttemp = (int)temperature;
		int tt = (int)(temperature - inttemp);


 		


		
		if (count >= 10)
		{
		mean_temperature = tempsum/10.0;
		mean_light = lightsum/10.0;


		int intmeanlight = (int)mean_light;
		int meanll = (int)(mean_light - intmeanlight);
	
		intmeantemp = (int)mean_temperature;
		meantt = (int)(mean_temperature - intmeantemp);

		if ((intmeantemp > 20 && intmeantemp < 30) && (intmeanlight > 300 && intmeanlight < 600))
		{
			tempthershold = 37;
			lighthreshold = 700;
		}
		else
		{
			tempthershold = 47;
			lighthreshold = 1200;
		}
	
		
		
		printf("Mean Temp %d.%d\n",intmeantemp , meantt);
		count = 0;
		lightsum = 0;
		tempsum = 0;
		}
		else{
		tempsum = tempsum + temperature;
		lightsum = lightsum + light_lx;
		count = count + 1;
		printf("Mean Temp %d.%d\n",intmeantemp , meantt);
		printf("Light Sensor %d.%d\n",intlight,ll);
		printf("Temp Sensor %d.%d\n", inttemp , tt);

		if(inttemp >= tempthershold && intlight >= lighthreshold ){

		process_post(&notification_process, notification_event_ready, &temperature);
		

		}
	
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
			leds_on(LEDS_ALL);
			packetbuf_copyfrom("Alarm", 6);
  		  	addr.u8[0] = 1;
   		 	addr.u8[1] = 0;
   			 if(!rimeaddr_cmp(&addr, &rimeaddr_node_addr)) {
     			 unicast_send(&uc, &addr);
			printf("Fire Alarm Triggered");
   			 }
			}
			leds_off(LEDS_ALL);
			PROCESS_END();
		}
