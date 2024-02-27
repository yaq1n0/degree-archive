#include <FreeRTOS.h>
#include <task.h>
#include "semphr.h"
 
#define SERIAL_PORT Serial
#define BLINK_ON_TIME   2000
#define BLINK_OFF_TIME  2000

/* Dimensions of the buffer that the task being created will use as its stack.*/
#define STACK_SIZE 200

/* Structure that will hold the Task control block of the tasks being created. */
StaticTask_t xTaskBuffer_A;
StaticTask_t xTaskBuffer_B;

/* Buffer that the task being created will use as its stack.   */
StackType_t xStack_A[ STACK_SIZE ];
StackType_t xStack_B[ STACK_SIZE ];

static SemaphoreHandle_t mutex;
 
void setup() 
{
  
  SERIAL_PORT.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  mutex = xSemaphoreCreateMutex();

  xTaskCreateStatic(led_ON, "led_ON", STACK_SIZE, NULL, 1, xStack_A, &xTaskBuffer_A);
  xTaskCreateStatic(led_OFF, "led_OFF", STACK_SIZE, NULL, 1, xStack_B, &xTaskBuffer_B);
}

void led_ON(void *pvParameters)
{
  (void) pvParameters;
  while (1)
  {
    if (digitalRead(LED_BUILTIN) == LOW) {
      if (xSemaphoreTake(mutex, 0 == pdTRUE)) {
            SERIAL_PORT.println("LED ON!");
            digitalWrite(LED_BUILTIN, HIGH);
            vTaskDelay(BLINK_ON_TIME);
            xSemaphoreGive(mutex);
    }
   }
  }
}

void led_OFF(void *pvParameters)
{
  (void) pvParameters;
  while (1)
  {
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
