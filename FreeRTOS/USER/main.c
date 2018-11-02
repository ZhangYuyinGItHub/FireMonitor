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
#include "usart1.h"

#include  "FreeRTOS.h"
#include  "task.h"
#include  "user_task.h"

#include  "voice.h"


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



    /*����ģ���ʼ��*/
    USART1_Config();
    //UART1_NVIC_Configuration();
    USART2_Config();

    voice_init();

    os_task_init();

    vTaskStartScheduler();

}




/******************* (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� *****END OF FILE****/
