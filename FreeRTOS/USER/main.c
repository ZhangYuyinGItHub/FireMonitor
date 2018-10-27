/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：板载的LED1、LED2、LED3、以固定的频率轮流闪烁（频率可调）。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.0.0
 *
 * 作者    ：fire  QQ: 313303034 
 * 博客    ：firestm32.blog.chinaunix.net
**********************************************************************************/	

#include "stm32f10x.h"
#include "led.h"

#include  "FreeRTOS.h"
#include  "task.h"

#define USER_TASK1_PRIO   2
#define USER_TASK1_STK    128
TaskHandle_t user_task1_handler;
void user_task1(void *pvP);

#define USER_TASK2_PRIO   1
#define USER_TASK2_STK    128
TaskHandle_t user_task2_handler;
void user_task2(void *pvP);

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{
	/* 配置系统时钟为72M */      
  SystemInit();	
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	xTaskCreate( (TaskFunction_t   )user_task1, 
	             (const char*      )"user_task1",
							 (uint16_t         )USER_TASK1_STK,
							 (void*            )NULL,
							 (UBaseType_t      )USER_TASK1_PRIO,
							 (TaskHandle_t*    )&user_task1_handler);
							 
		xTaskCreate( (TaskFunction_t   )user_task2, 
	             (const char*      )"user_task2",
							 (uint16_t         )USER_TASK2_STK,
							 (void*            )NULL,
							 (UBaseType_t      )USER_TASK2_PRIO,
							 (TaskHandle_t*    )&user_task2_handler);

    vTaskStartScheduler();
  
}




/******************* (C) COPYRIGHT 2011 野火嵌入式开发工作室 *****END OF FILE****/
