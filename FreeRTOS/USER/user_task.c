#include "user_task.h"
#include "led.h"
#include "usart1.h"

#include  "FreeRTOS.h"
#include  "task.h"
#include "queue.h"

/*============================================================================*
 *                              Micro
 *============================================================================*/
#define USER_TASK1_PRIO   3
#define USER_TASK1_STK    TASK_STK_SIZE
TaskHandle_t user_task1_handler;


#define USER_TASK2_PRIO   2
#define USER_TASK2_STK    TASK_STK_SIZE
TaskHandle_t user_task2_handler;


#define UART1_REV_PRIO    4
#define UART1_REV_STK    TASK_STK_SIZE
TaskHandle_t uart1_task_handler;


/*============================================================================*
 *                              Local Variables
 *============================================================================*/
xQueueHandle xQueueRx;

/*============================================================================*
 *                              Local Functions
 *============================================================================*/

void user_task1(void *pvP);
void user_task2(void *pvP);
void usart1_handle_task(void *pvP);

void os_task_init(void)
{
    taskENTER_CRITICAL();
    xQueueRx = xQueueCreate(UART1_QUEUE_SIZE, sizeof(u8));


    if (xQueueRx)
    {
        printf("Uart Queue Create Success!\r\n");
    }
    else
    {
        printf("Err: Uart Queue Create failed!\r\n");
    }

    xTaskCreate((TaskFunction_t)user_task1,
                (const char *)"user_task1",
                (uint16_t)USER_TASK1_STK,
                (void *)NULL,
                (UBaseType_t)USER_TASK1_PRIO,
                (TaskHandle_t *)&user_task1_handler);

    xTaskCreate((TaskFunction_t)user_task2,
                (const char *)"user_task2",
                (uint16_t)USER_TASK2_STK,
                (void *)NULL,
                (UBaseType_t)USER_TASK2_PRIO,
                (TaskHandle_t *)&user_task2_handler);

    xTaskCreate((TaskFunction_t)usart1_handle_task,
                (const char *)"usart1_handle_task",
                (uint16_t)UART1_REV_STK,
                (void *)NULL,
                (UBaseType_t)UART1_REV_PRIO,
                (TaskHandle_t *)&uart1_task_handler);

    taskEXIT_CRITICAL();

}
void Delay(__IO u32 nCount)
{
    for (; nCount != 0; nCount--);
}

void usart1_handle_task(void *pvP)
{

    u8 rev = 0;
    printf("uart task is init\r\n");
    while (1)
    {
        if ((xQueueRx != 0) && (xQueueReceive(xQueueRx, &rev, portMAX_DELAY)))
        {
            printf("0x%x,\r\n", rev);
        }
        else
        {
            printf("no rev data \r\n");
        }
    }
}

void user_task1(void *pvP)
{
    printf("task1 is init\r\n");
    while (1)
    {
        LED1(ON);              // ÁÁ
        Delay(0x6FFFEF);
        LED1(OFF);         // Ãð

        vTaskDelay(4000);
    }
}

void user_task2(void *pvP)
{
    printf("task2 is init\r\n");
    while (1)
    {
        //printf("task2 is start\r\n");
        LED2(ON);
        Delay(0x6FFF0);
        LED2(OFF);
        Delay(0x6FFF0);
    }
}
