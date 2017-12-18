#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include "gpio_api.h"    //gpio
#include "device.h"      
#include "wait_api.h"   //delay
#include "serial_api.h" //uart
#include "serial_ex_api.h"

gpio_t      gpio_led;
gpio_t      gpio_led2;
gpio_t      gpio_tx;
gpio_t      gpio_rx;
serial_t    sobj;

void led_test_r(void);
void led_test_g(void);
void uart_send_test(void);
void uart_receive_test(void);
void uart_send_string(serial_t *sobj, char *pstr);

#define LED_R      PC_5
#define LED_G      PC_4
#define UART_TX     PA_4
#define UART_RX     PA_0
#define GPIO_HIGH          1
#define GPIO_LOW           0

volatile char rc=0;

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;
TaskHandle_t task3_handler;
TaskHandle_t task4_handler;

extern void wlan_netowrk(void);
extern void console_init(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */



void uart_send_string(serial_t *sobj, char *pstr)
{
    unsigned int i=0;

    while (*(pstr+i) != 0) {
        serial_putc(sobj, *(pstr+i));
        i++;
    }
}



int main(void)
{
  
   /*
	console_init();
#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	wlan_network();
#endif
  */
  
        /**************LED GPIO INIT***********/
        gpio_init(&gpio_led,LED_R);
        gpio_dir(&gpio_led, PIN_OUTPUT);
        gpio_mode(&gpio_led, PullNone); 
         /**************LED_GPIO INIT***********/
        gpio_init(&gpio_led2,LED_G);
        gpio_dir(&gpio_led2, PIN_OUTPUT);
        gpio_mode(&gpio_led2, PullNone);
        
        
           /******** UART TX GPIOA.7*******/
        gpio_init(&gpio_tx, UART_TX);
        gpio_dir(&gpio_tx, PIN_OUTPUT);
        gpio_mode(&gpio_tx, PullUp);
        gpio_write(&gpio_tx, GPIO_HIGH);
        
        
          /******* UART RX GPIOA.6*********/
        gpio_init(&gpio_rx,UART_RX);
        gpio_dir(&gpio_rx, PIN_INPUT);
        gpio_mode(&gpio_rx, PullUp);
        
        /**************UART INIT***********/
        serial_init(&sobj,UART_TX,UART_RX);
        serial_baud(&sobj,38400);
        serial_format(&sobj, 8, ParityNone, 1);
     
xTaskCreate( led_test_r, (const char *)"led_test_r", 1024, ( void * ) NULL, tskIDLE_PRIORITY + 1, task1_handler);   //LED RED


    //3 3)Enable Schedule, Start Kernel
#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	RtlConsolTaskRom(NULL);
#endif

}


void led_test_r(void)
{
  int i=0;
  for (;;)
 {
  gpio_write(&gpio_led, 1);
  vTaskDelay(500);
  gpio_write(&gpio_led, 0);
  vTaskDelay(500);
   i++;
    if(i==3)
   { 
     xTaskCreate( led_test_g, (const char *)"led_test_g", 1024, ( void * ) NULL, tskIDLE_PRIORITY + 1, task2_handler); //LED GREEN
     xTaskCreate( uart_send_test, (const char *)"uart_send_test", 1024, ( void * ) NULL, 1, task3_handler );    //UART send data
     xTaskCreate( uart_receive_test, (const char *)"uart_receive_test", 1024, ( void * ) NULL, 1, task4_handler );    //UART receive data
     vTaskDelete(NULL);
   }
 }
}

void led_test_g(void)
{
  for (;;)
 {
  gpio_write(&gpio_led2, 1);
  vTaskDelay(500);
  gpio_write(&gpio_led2, 0);
  vTaskDelay(500);
 }
}
 /*******UART SEND TX DATA********/
void uart_send_test(void)
{
  for (;;)
 {
   uart_send_string(&sobj, "Hello World!!\r\n");
   vTaskDelay(1000);
 }
}

 /*******UART RECEIVE RX DATA********/
void uart_receive_test(void)
{
  
    char rc;
    while(1)
    {
     rc=serial_getc(&sobj);
     serial_putc(&sobj, rc);
    }
}
