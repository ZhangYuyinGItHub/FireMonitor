/**
************************************************************************************************************
*               Copyright(c) 2014-2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     adpcm_encode.h
* @brief
* @author   Susan_Zhang
* @date     2015-10
* @version  v0.1
*************************************************************************************************************
*/

#ifndef __ADPCM_ENCODE_H__
#define __ADPCM_ENCODE_H__

#include    "stdint.h"
//#include    "board.h"

#define ENCODE_FRAME_SIZE       20//每次压缩后的数据字节数//(FRAME_SIZE_AFTER_ENC*3/4)//80,bytes , 64bps --> 48bps
#define VOICE_SAMPLE_NUM        53//每20个bytes的字节中包含53组采样点压缩后的3bit，即53*3bits.//(MSBC_2_FRAME_SIZE/2)//320, 16bit voice data


//typedef struct
//{
//    short   valprev;    /* Previous output value */
//    char    index;      /* Index into stepsize table */
//} adpcm_state;

typedef  struct
{
    int     valprev;    /* Previous output value */
    int     index;      /* Index into stepsize table */
    short   peak_level;
    int     write_out_index;
    int     write_in_index;
} adpcm_state;

typedef struct
{
    int16_t *inp;
    int index;
    int imaSize;
    uint8_t *out;
    int imaAnd;
    int imaOr ;
    int32_t predictedSample;
    int len;
} adpcm_IMA_state;



void decode_adpcm_16to3(unsigned char *indata, unsigned int num_bytes, unsigned short *outdata,
                        adpcm_state *state);
void adpcm_encoder(adpcm_IMA_state *state);

//extern uint8_t EncodeBuffer[ENCODE_FRAME_SIZE];
//extern adpcm_state gAdpcmEncState;


#endif /* __CVSD_ENCODE_H__ */
