#ifndef _USER_TASK_H_
#define _USER_TASK_H_

#include "stm32f10x.h"

#define TASK_STK_SIZE       1024
#define UART1_QUEUE_SIZE    0x20

typedef struct
{
    uint16_t     msg_type;
    uint16_t     msg_value;
    void        *p_buf;

} stg_task_msg;

typedef enum
{
    MSG_TYPE_UART1 = 0,
	  MSG_TYPE_DMA_UART1,
    MSG_TYPE_MAX_NUM,
} enum_msg_type;

extern void os_task_init(void);

#endif
