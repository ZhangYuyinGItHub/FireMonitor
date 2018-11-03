
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "voice.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usart1.h"
#include "string.h"
#include "user_task.h"

/*============================================================================*
 *                              Micro
 *============================================================================*/


/*============================================================================*
 *                              External Variables
 *============================================================================*/
 
extern QueueHandle_t xQueueRx;

/*============================================================================*
 *                              Local Variables
 *============================================================================*/

st_Voice_buf voice_buf = {0};

static uint8_t voice_date_buf[M_BLOCK_SIZE * M_BLOCK_NUM] = {0};
st_voice_queue  g_voice_data;

const uint8_t voice_data[] = {0};
/*============================================================================*
 *                              Local Functions
 *============================================================================*/
void usart1_dma_nvic_config(void);
void usart1_dma_config(void);

/**
 * @brief init voice queue
 * @param none
 * @return none
 */
void voice_init_Queue(void)
{
    g_voice_data.size = M_BLOCK_NUM;
    g_voice_data.send_idx   = 0;
    g_voice_data.store_idx  = 0;
    g_voice_data.pkt_sn = 0;

    g_voice_data.flag  = 1;
    g_voice_data.send_flag = 0;
    g_voice_data.p_buf = (uint8_t *)voice_date_buf;
}
/**
 * @brief in voice queue
 * @param buffer - buffer data to be stored
 * @return none
 */
void voice_in_queue(uint8_t *buffer)
{
    uint32_t tail;
    tail = (g_voice_data.store_idx + 1) % g_voice_data.size;
    if (tail == g_voice_data.send_idx)
    {
        //voice queue is full.
    }
    else
    {
        /*copy from buffer to voice queue*/
        memcpy(g_voice_data.p_buf + g_voice_data.store_idx * M_BLOCK_SIZE,
               (uint8_t *) buffer,
               M_BLOCK_SIZE);
    }
    g_voice_data.store_idx = tail;
}
/**
 * @brief out voice queue
 * @param none
 * @return none
 */
uint8_t voice_out_queue(void)
{
    if (g_voice_data.store_idx == g_voice_data.send_idx)
    {
        //voice queue is empty.
        return 1;
    }
    else
    {

        uart_send_data(USART2, g_voice_data.p_buf + g_voice_data.send_idx * M_BLOCK_SIZE, M_BLOCK_SIZE);
        //调整队列
        g_voice_data.send_idx = (g_voice_data.send_idx + 1) % g_voice_data.size;

        return 0;
    }
}

/**
 * @brief check if voice queue is full
 * @param none
 * @return true or false
 */
uint8_t is_voice_queue_full(void)
{
    if ((g_voice_data.store_idx + 1) % g_voice_data.size == g_voice_data.send_idx)
    {
        //queue is full
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief check if voice queue is empty
 * @param none
 * @return true or false
 */
uint8_t is_voice_queue_empty(void)
{
    if (g_voice_data.store_idx == g_voice_data.send_idx)
    {
        //queue is empty
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief voice module init
 * @param none
 * @return none
 */
void voice_init(void)
{
    voice_init_Queue();

    usart1_dma_config();
    usart1_dma_nvic_config();

    voice_buf.buf_flag = 0;
}

void usart1_dma_config(void)
{

    DMA_InitTypeDef DMA_InitStructure;

    /* DMA1 Channel6 (triggered by USART1 Rx event) Config */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA1 Channel5 (triggered by USART1 Rx event) Config */
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;// DMA 源地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)voice_buf.buf0;// DMA 目的地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;//pripheral act in source role
    DMA_InitStructure.DMA_BufferSize         = DMA_SIZE ;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; //源地址不需要增长
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;//目的地址需要增长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //搬运字节传输
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;//放置字节传输
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;// 是否需要循环传输，这里不需要循环传输
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;//通道的优先级
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; // 是否是内存到内存的模式，这里disable

    DMA_Init(DMA1_Channel5, &DMA_InitStructure);//

    DMA_Cmd(DMA1_Channel5, ENABLE);//使能DMA通道

    USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE); //使能串口DMA发送

    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE); //??DMA?????????

}

void usart1_dma_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles usart1 exception.
  * @param  None
  * @retval : None
  */
void USART1_IRQHandler(void)
{
    u8 value;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        value = USART_ReceiveData(USART1);
        value = value;
    }
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);


}
/*
 * 函数名：DMA1_Channel5_IRQHandler
 * 描述  ：USART1 串口DMA中断接收
 * 输入  ：void
 * 输出  : void
 */
void DMA1_Channel5_IRQHandler(void)
{

    if (DMA_GetITStatus(DMA1_IT_TC5))
    {
        DMA_ClearITPendingBit(DMA1_IT_TC5); //清除中断标志位

        if (xQueueRx != 0)
        {
            stg_task_msg msg;
            msg.msg_type = MSG_TYPE_DMA_UART1;
            msg.msg_value = voice_buf.buf_flag;
            if (pdPASS == xQueueSendFromISR(xQueueRx, &msg, 0))
            {
                //printf("uart send msg success\r\n");
            }
            else
            {
                //printf("uart send msg failed\r\n");
            }
        }

        DMA_Cmd(DMA1_Channel5, DISABLE);//Disable DMA通道
        DMA_SetCurrDataCounter(DMA1_Channel5, (uint32_t)DMA_SIZE);
        if (voice_buf.buf_flag)
        {
            voice_buf.buf_flag = 0;
            DMA1_Channel5->CMAR = (uint32_t)voice_buf.buf0;
        }
        else
        {
            voice_buf.buf_flag = 1;
            DMA1_Channel5->CMAR = (uint32_t)voice_buf.buf1;
        }

        DMA_Cmd(DMA1_Channel5, ENABLE);//Enable DMA通道

    }
}

/**
  * @brief  voice_msg_handle , handle voice msg.
  * @param  stg_task_msg* pMsg, voice msg struct
  * @retval : None
  */
void voice_msg_handle(stg_task_msg* pMsg)
{
	  if (pMsg->msg_type == MSG_TYPE_DMA_UART1)
		{
				if (pMsg->msg_value == 1)
				{
						voice_in_queue(voice_buf.buf1);
				}
				else
				{
						voice_in_queue(voice_buf.buf0);
				}
	  }
}
