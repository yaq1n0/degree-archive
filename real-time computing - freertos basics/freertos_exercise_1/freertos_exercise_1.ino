#include <FreeRTOS.h>
#include <task.h>
 
#define SERIAL_PORT Serial
#define BLINK_ON_TIME   2000
#define BLINK_OFF_TIME  2000

/* Dimensions of the buffer that the task being created will use as its stack.
  NOTE:  This is the number of words the stack will hold, not the number of
  bytes.  For example, if each stack item is 32-bits, and this is set to 100,
  then 400 bytes (100 * 32-bits) will be allocated. */
#define STACK_SIZE 200

/* Structure that will hold the Task Control Block of the task being created. */
StaticTask_t xTaskBuffer;
 
/* Buffer that the task being created will use as its stack.  Note this is
  an array of StackType_t variables.   . */
StackType_t xStack[ STACK_SIZE ];
 
 
void setup() 
{
  
  SERIAL_PORT.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  xTaskCreateStatic(led, "led", STACK_SIZE, NULL, configMAX_PRIORITIES - 1, xStack, &xTaskBuffer);
}

void led(void *pvParameters)
{
  (void) pvParameters;
  while (1)
  {
     SERIAL_PORT.println("LED ON!");
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(BLINK_ON_TIME);


    SERIAL_PORT.println("LED OFF!");
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(BLINK_OFF_TIME);
   }
}

 

void loop() {
 
}
