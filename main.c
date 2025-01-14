 /*
* main.c
* Author : IHA
*
* Example main file including LoRaWAN setup
* Just for inspiration :)
*/

//library filer
#include <ATMEGA_FreeRTOS.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <semphr.h>
#include <task.h>
#include <serial.h>
#include <time.h>

//include drivers
#include "hih8120.h"
#include "serial.h"
#include "stdio_driver.h"
#include "lora_driver.h"

//header filer for de task vi opretter
#include "TerrariumFiles/temp_sensor.h"
#include "TerrariumFiles/loRaWanHandler.h"


//definere vores min og maks ( vha. FreeRTOS)
#define TEMP_TASK_PRIORITY (configMAX_PRIORITIES - 3)


//task setup
TaskHandle_t tempSensorHandle = NULL;
TaskHandle_t loRaWanHandle = NULL;


int main() {
	
	 // Here I make room for two downlink messages in the message buffer
	MessageBufferHandle_t downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	
	 // The parameter is the USART port the RN2483 module is connected to - 
	 // in this case USART1 - here no message buffer for down-link messages are defined
	 
	//lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);
    //xTaskCreate(loraWan_TaskReset,"task reset", configMINIMAL_STACK_SIZE,NULL,8,NULL);
	
	
	
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);
	// Initialise the trace-driver to be used together with the R2R-Network
	trace_init();

	// vvvvvvvvvvvvvvvvv BELOW IS LoRaWAN initialisation vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Initialise the HAL layer and use 5 for LED driver priority
	   //hal_create(5);
	
	// Create LoRaWAN task and start it up with priority 3
	  lora_handler_create(3);
	

		// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	// for at printe ud skal dette her med ( header filen "stdio_driver.h" skal includes
       stdio_initialise(ser_USART0);
 
 
     // Initialise the LoRaWAN driver without down-link buffer
        //lora_driver_initialise(1, NULL);
     //setup for loRaWAN
	 lora_driver_initialise(ser_USART1,NULL);
	 //lora_handler_task(3);



	//vi starter med at s�tte alle variablerne til 0
	//terrariumdata.humidity = 0;
	//terrariumdata.temperature = 0;
	
		
	//opretter de Task vi skal lave ( vha. FreeRTOS)
	xTaskCreate(tempSensorTask, "Temperature measurement", configMINIMAL_STACK_SIZE, NULL, TEMP_TASK_PRIORITY, &tempSensorHandle);
	xTaskCreate(lora_handler_task, "Led", configMINIMAL_STACK_SIZE, NULL,4, &loRaWanHandle);
	
	//xTaskCreate()


	// Innitalisere semaphoren
	//semaphore = xSemaphoreCreateMutex();
	
	// initialalisere temp/fugt m�leren
	if(HIH8120_OK !=  hih8120_initialise()) {
		printf("Failed to initialize temperature sensor\n");
		return 1;
	}
	
// der m� ikke k�res kode, n�r scheduleren er eksiveret
	vTaskStartScheduler();

	while (1)
	{}

}


