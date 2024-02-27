#include <FreeRTOS.h>
#include <task.h>
 
#define SERIAL_PORT Serial

/* Dimensions of the buffer that the task being created will use as its stack.*/
#define STACK_SIZE 200
 
void setup() 
{
  SERIAL_PORT.begin(115200);

  xTaskCreate(task1, "task1", STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &xHandle);
  xTaskCreate(task2, "task2", STACK_SIZE, ( void * ) 1, tskIDLE_PRIORITY, &xHandle);
}

void task1(void *pvParameters)
{
  (void) pvParameters;
  while (1)
  {
    vTaskDelay(5000)
  }
}

void task2(void *pvParameters)
{
  (void) pvParameters;
  while (1)
  {
    vTaskDelay(10000)
    if (digitalRead(LED_BUILTIN) == HIGH) {
      if (xSemaphoreTake(mutex, 0 == pdTRUE)) {
        SERIAL_PORT.println("LED OFF!");
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(BLINK_OFF_TIME);
        xSemaphoreGive(mutex);
      }
    }
  }
}

void loop() { 
}
