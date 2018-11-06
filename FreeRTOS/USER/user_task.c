#include "user_task.h"
#include "led.h"
#include "usart1.h"

#include  "FreeRTOS.h"
#include  "task.h"
#include "queue.h"

#include "voice.h"

/*============================================================================*
 *                              Micro
 *============================================================================*/
#define USER_TASK1_PRIO   3
#define USER_TASK1_STK    (TASK_STK_SIZE)
TaskHandle_t user_task1_handler;


#define USER_TASK2_PRIO   2
#define USER_TASK2_STK    TASK_STK_SIZE
TaskHandle_t user_task2_handler;


#define UART1_REV_PRIO    4
#define UART1_REV_STK    (TASK_STK_SIZE*2)
TaskHandle_t uart1_task_handler;


/*============================================================================*
 *                              Local Variables
 *============================================================================*/
xQueueHandle xQueueRx;
extern st_Voice_buf voice_buf;
/*============================================================================*
 *                              Local Functions
 *============================================================================*/

void user_task1(void *pvP);
void user_task2(void *pvP);
void user_msg_handle_task(void *pvP);

void os_task_init(void)
{
    taskENTER_CRITICAL();


    xQueueRx = xQueueCreate(UART1_QUEUE_SIZE, sizeof(stg_task_msg));


    if (xQueueRx)
    {
        //printf("Uart Queue Create Success!\r\n");
    }
    else
    {
        //printf("Err: Uart Queue Create failed!\r\n");
    }

    xTaskCreate((TaskFunction_t)user_task1,
                (const char *)"user_task1",
                (uint16_t)USER_TASK1_STK,
                (void *)NULL,
                (UBaseType_t)USER_TASK1_PRIO,
                (TaskHandle_t *)&user_task1_handler);

//    xTaskCreate((TaskFunction_t)user_task2,
//                (const char *)"user_task2",
//                (uint16_t)USER_TASK2_STK,
//                (void *)NULL,
//                (UBaseType_t)USER_TASK2_PRIO,
//                (TaskHandle_t *)&user_task2_handler);

    xTaskCreate((TaskFunction_t)user_msg_handle_task,
                (const char *)"user_msg_handle_task",
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

/**
  * @brief : user_msg_handle_task, used for user msg handle.
  * @param : void *pvP, task input param form task create.
  * @retval : none
  */
void user_msg_handle_task(void *pvP)
{
    stg_task_msg msg;
    while (1)
    {
        if ((xQueueRx != 0) && (xQueueReceive(xQueueRx, &msg, portMAX_DELAY)))
        {
            if (msg.msg_type == MSG_TYPE_DMA_UART1)
            {
                voice_msg_handle(&msg);
            }
        }
        //printf("user_msg_handle_task\r\n");
    }
}

void user_task1(void *pvP)
{
    while (1)
    {
        if (!is_voice_queue_empty())
        {
            //printf("voice out data \r\n");
            LED1(ON);          // ¡¡
            voice_out_queue();
        }
        else
        {
            //printf("voice no data \r\n");
            LED1(OFF);          // ¡¡
        }
    }
}

void user_task2(void *pvP)
{
//    printf("task2 is init\r\n");
    while (1)
    {
        //printf("task2 is start\r\n");
        LED2(ON);
        Delay(0x6FFF0);
        LED2(OFF);
        Delay(0x6FFF0);
    }
}
