#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

int count = 0;
TaskHandle_t led_task1;
TaskHandle_t led_task2;

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
        count++;
        printf("count: %d\n", count);
    }
}
void task_delete()
{   
    while(1){
        if (count >= 6){
            vTaskDelete(&led_task1);
           // vTaskDelete(NULL);

        }
        if  (count >= 10){
            vTaskDelete(&led_task2);
            vTaskDelete(NULL);
            //count = 0;D

        }
    }
    
}
int main()
{
    stdio_init_all();
    xTaskCreate(led_task, (const char *) "LED_Task", 256, (void *) 13, 1, &led_task1);
    xTaskCreate(led_task, (const char *) "LED_Task2", 256, (void *) 15 , 1, &led_task2);
    xTaskCreate(task_delete, (const char *) "Task_Delete", 256, NULL, 1, &led_task2);


    vTaskStartScheduler();

    while(1){};
}