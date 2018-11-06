/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    adpcm.c
  * @brief   adpcm for voice.
    * @details
  * @author  yuyin_zhang
  * @date    05-11-2018
  * @version v0.2
  ******************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

#include    "adpcm.h"
#include    "stdint.h"
#include    "FreeRTOS.h"
#include    "usart1.h"


//uint8_t EncodeBuffer[ENCODE_FRAME_SIZE];
//adpcm_state gAdpcmEncState = {0};
//uint32_t  gAdpcmBitCount = 0;
//adpcm_IMA_state gAdpcmState = {0};


const int indexTable[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

const int stepsizeTable[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void decode_adpcm_frame(unsigned char nibble, adpcm_state *state,
                        unsigned short *outdata)
{
    int step_index = state->index;
    int value = state->valprev;
    int step = stepsizeTable[step_index];
    int diff;

    diff = step;
    if ((nibble & 4) != 0)
    {
        diff += (int)(step << 3);
    }
    if ((nibble & 2) != 0)
    {
        diff += (int)(step << 2);
    }
    if ((nibble & 1) != 0)
    {
        diff += (int)(step << 1);
    }
    diff >>= 3;

    if (nibble & 8)
    {
        value -= diff;
        if (value < -32768)
        {
            value = 32768;
        }
    }
    else
    {
        value += diff;
        if (value > 32767)
        {
            value = 32767;
        }
    }
    state->valprev = value;

    /* copy to stream */
    if (outdata != NULL)
    {
        *(outdata + state->write_out_index) = (unsigned short)value;
        state->write_out_index ++;
    }
    if (value > state->peak_level)
    {
        state->peak_level = value;
    }

    /* update step_index */
    step_index += indexTable[nibble];
    /* clamp step_index */
    if (step_index < 0)
    {
        step_index = 0;
    }
    else if (step_index > 88)
    {
        step_index = 88;
    }
    state->index = step_index;
}

void decode_adpcm_16to3(unsigned char *indata, unsigned int num_bytes, unsigned short *outdata,
                        adpcm_state *state)
{

    unsigned int i, j, size;
    unsigned int  packed;
    unsigned char data;

    i = 0;
    while (i < num_bytes)
    {
        packed = 0;
        size = 0;
        for (j = 0; j < 3 && i < num_bytes; ++j, ++i)
        {
            packed |= indata[i] << (24 - size);
            size += 8;
        }
        size /= 3;
        for (j = 0; j < size; ++j)
        {
            data = ((packed >> 28) & 0x0E) | 0x01;
            packed <<= 3;
            decode_adpcm_frame(data, state, outdata);
        }
    }
//  unsigned int group = 0, pack_int = 0, index = 0;
//  unsigned short pack_short = 0;
//  unsigned char data = 0;
//  int j = 0;

//  for (index = 0;index < (num_bytes/3*3); index += 3)
//  {
//
//      pack_int = (unsigned int)((*(indata + index)<<24)|(*(indata + index + 1)<<16)|(*(indata + index + 2)<<8));
//      for ( j = 0; j < 8; j++)
//      {
//          data = ((pack_int >> 28) & 0x0E) | 0x01;
//          pack_int <<= 3;
//          decode_adpcm_frame(data, state, outdata);
//      }
//  }

//  group = (num_bytes%3) *8 /3;
//  if (num_bytes%3 == 1)
//      pack_short = (unsigned int)((*(indata + index)<<8));
//  else if (num_bytes%3 == 2)
//      pack_short = (unsigned int)((*(indata + index)<<8)|(*(indata + index + 1)));
//  for (j = 0; j<(group); j++)
//  {
//      data = ((pack_short >> 12) & 0x0E) | 0x01;
//      pack_short <<= 3;
//      decode_adpcm_frame(data, state, outdata);
//  }
}

void adpcm_encoder(adpcm_IMA_state *state)
{
    int i, j;
    int mask;
    int storeIma;
    int32_t predictedDiff;
    int32_t tempStepSize;
    int16_t *ptr = state->inp;
    int32_t predictedSample = (int32_t)state->predictedSample;
    int index = state->index;
    int stepSize = stepsizeTable[index];
    uint8_t *optr = state->out;
    uint16_t outBuf = 0;
    int size = state->imaSize;
    int outMsb = 16 - size;
    int imaAnd = state->imaAnd;
    int imaOr = state->imaOr;
    int shift = 4 - size;


    for (i = 0; i < state->len; i++)
    {
        int inp = *ptr++;
        int32_t diff = inp - predictedSample;
        int sign = diff;
        uint8_t newIma = 0;
        if (sign < 0)
        {
            diff = -diff;

        }
        mask = 4;
        tempStepSize = stepSize << 3;
        diff <<= 3;
        for (j = 0; j < 3; j++)
        {
            if (diff > tempStepSize)
            {
                newIma |= mask;
                diff -= tempStepSize;
            }
            tempStepSize >>= 1;
            mask >>= 1;
        }
        storeIma = newIma;

        if (sign < 0)
        {
            storeIma |= 8;
        }
        storeIma = storeIma >> shift;

        outBuf |= (storeIma << outMsb);
        if (outMsb < 8)
        {
            *optr++ = (uint8_t)(outBuf >> 8);
            outMsb += 8;
            outBuf = (uint16_t)(outBuf << 8);

        }
        outMsb  -= size;
        newIma = newIma & imaAnd;
        newIma |= imaOr;

        predictedDiff = (newIma * (int32_t)stepSize) + (stepSize >> 1);
        predictedDiff >>= 2;
        predictedSample += (sign < 0) ? - predictedDiff : predictedDiff;
        if (predictedSample > 32767)
        {
            predictedSample = 32767;
        }
        if (predictedSample < - 32768)
        {
            predictedSample = 32768;
        }

        index += indexTable[newIma];
        if (index < 0)
        {
            index = 0;
        }
        else if (index > 88)
        {
            index = 88;
        }
        stepSize = stepsizeTable[index];
    }
    * optr++ = (uint8_t)(outBuf >> 8);
    state->index = index;
    state->predictedSample = predictedSample;

}


