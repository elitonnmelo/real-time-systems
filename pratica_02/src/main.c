#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


#define TRIGGER_PIN 19
#define ECHO_PIN 28
TaskHandle_t kill_led_task1;
TaskHandle_t led_task2;

static unsigned long int idle_tick_counter = 0;
void vApplicationIdleHook(void)
{
	unsigned long int tick = xTaskGetTickCount();
	while (xTaskGetTickCount() == tick);
	idle_tick_counter++;
}

void taskCPUUsage(void *pvParameters)
{
	unsigned long int idle_tick_last, ticks;
	idle_tick_last = idle_tick_counter = 0;
	for (;;) {
		/* wait for 3 seconds */
		vTaskDelay(3000/portTICK_PERIOD_MS);

		/* calculate quantity of idle ticks per second */
		if (idle_tick_counter > idle_tick_last)
			ticks = idle_tick_counter - idle_tick_last;
		else
			ticks = 0xFFFFFFFF - idle_tick_last + idle_tick_counter;
		ticks /= 4;

		/* print idle ticks per second */
		printf("%ld idle ticks per second (out of %ld)\n", ticks, configTICK_RATE_HZ);

		/* calc and print CPU usage */
		ticks = (configTICK_RATE_HZ - ticks) * 100 / configTICK_RATE_HZ;
		printf("CPU usage: %d%%\n", ticks);

		/* update idle ticks */
		idle_tick_last = idle_tick_counter ;
	}
}

// void taskUseCPU(void *pvParameters)
// {
// 	unsigned int i, j;
// 	for (;;) {
// 		for (i = 0, j = 0; i < 10000; i++){
// 			j *= i + 12.34;
// 		}
// 		vTaskDelay(100/portTICK_PERIOD_MS);
// 	}
// }

void kill_led_task(void *GPIO){   
    const uint LED_PIN = GPIO;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for (int i = 0; i < 3; i++) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void getDistance() {

    // gpio_init(PICO_DEFAULT_LED_PIN);
    // gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    while(1) {
        // gpio_put(PICO_DEFAULT_LED_PIN, 1);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // gpio_put(PICO_DEFAULT_LED_PIN, 0);

        gpio_put(TRIGGER_PIN, 1);
        sleep_ms(5);
        gpio_put(TRIGGER_PIN, 0);
        
        uint32_t signaloff, signalon;
        
        while (gpio_get(ECHO_PIN) == 0) {
            signaloff = time_us_32();
        }
        while (gpio_get(ECHO_PIN) == 1) {
            signalon = time_us_32();    
        }
        
        uint32_t timepassed = signalon - signaloff;
        float distance = (timepassed * 0.0343) / 2;
        if (distance < 10) {
            xTaskCreate(kill_led_task, (const char *) "Kill_Led_Task", 256, (void *) 25 , 1, &kill_led_task1);
            vTaskDelete(led_task2);
            //vTaskDelete(kill_led_task1);
            break;
            
        }
        
       // printf("Distancia do Objeto: %.2f\n", distance);
    }
    vTaskDelete(NULL);
}

void led_task(void *GPIO)
{   
    const uint LED_PIN = GPIO;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

int main() {

    stdio_init_all();
    
    xTaskCreate(led_task, (const char *) "LED_Task2", 256, (void *) 15 , 1, &led_task2);
    xTaskCreate(getDistance, (const char *) "getDistance", 256, NULL, 1, NULL);

    // /* create task to show CPU usage */
	 xTaskCreate(taskCPUUsage, (signed char *)"Task CPU Usage", configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);

	// /* create task to use CPU */
	 //xTaskCreate(taskUseCPU, (signed char *)"TaskUseCPU", configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);

    vTaskStartScheduler();
    while(1){};
}
