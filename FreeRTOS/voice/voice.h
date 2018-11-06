#ifndef _VOICE_H_
#define _VOICE_H_

#include "stm32f10x.h"
#include "user_task.h"

#define DMA_SIZE             (360) /*include 2 vocie frame*/
#define VOICE_BUFFER_SIZE    1024

typedef struct
{
    uint8_t    buf_flag;
    uint8_t buf0[VOICE_BUFFER_SIZE];
    uint8_t buf1[VOICE_BUFFER_SIZE];
} st_Voice_buf;

/*=================================================
 *             voice queue config
 ==================================================*/
#define  M_BLOCK_SIZE    (106)//DMA_SIZE//320
#define  M_BLOCK_NUM     128//16

typedef struct
{
    uint32_t    size;
    uint32_t    send_idx;       //head
    uint32_t    store_idx;       //tail
    uint8_t     *p_buf;
} st_voice_queue;


void voice_init(void);
void voice_in_queue(uint8_t *buffer);
void voice_init_Queue(void);
uint8_t voice_out_queue(void);
uint8_t is_voice_queue_empty(void);

void voice_msg_handle(stg_task_msg *pMsg);

#endif
