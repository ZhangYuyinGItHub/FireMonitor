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

void Delay(__IO u32 nCount);

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

  while (1)
  {
		LED1( ON );			  // 亮
		Delay(0x6FFFEF);
		LED1( OFF );		  // 灭

		LED2( ON );
		Delay(0x6FFFEF);
		LED2( OFF );

		LED3( ON );
		Delay(0x6FFFEF);
		LED3( OFF );      
  }
}

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


/******************* (C) COPYRIGHT 2011 野火嵌入式开发工作室 *****END OF FILE****/
