/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���  ��main.c
 * ����    �����ص�LED1��LED2��LED3���Թ̶���Ƶ��������˸��Ƶ�ʿɵ�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.0.0
 *
 * ����    ��fire  QQ: 313303034 
 * ����    ��firestm32.blog.chinaunix.net
**********************************************************************************/	

#include "stm32f10x.h"
#include "led.h"

void Delay(__IO u32 nCount);

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{
	/* ����ϵͳʱ��Ϊ72M */      
  SystemInit();	
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();

  while (1)
  {
		LED1( ON );			  // ��
		Delay(0x6FFFEF);
		LED1( OFF );		  // ��

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


/******************* (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� *****END OF FILE****/
