#include "user_task.h"
#include "led.h"

#include  "FreeRTOS.h"
#include  "task.h"

#include "usart1.h"

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 
void user_task1(void *pvP)
{
	  while (1)
		{
			printf("task1 is start\r\n");
			LED1( ON );			  // ��
			Delay(0x6FFFEF);
			LED1( OFF );		  // ��

      vTaskDelay(4000);			
		}
}

void user_task2(void *pvP)
{
	  while (1)
		{
			printf("task2 is start\r\n");
			LED2( ON );
			Delay(0x6FFF0);
			LED2( OFF );    
      Delay(0x6FFF0);			
		}
}
