#include "user_task.h"
#include "led.h"

#include  "FreeRTOS.h"
#include  "task.h"

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 
void user_task1(void *pvP)
{
	  while (1)
		{
			LED1( ON );			  // ÁÁ
			Delay(0x6FFFEF);
			LED1( OFF );		  // Ãð

      vTaskDelay(4000);			
		}
}

void user_task2(void *pvP)
{
	  while (1)
		{
			LED2( ON );
			Delay(0x6FFF0);
			LED2( OFF );    
      Delay(0x6FFF0);			
		}
}
