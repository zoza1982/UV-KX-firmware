/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>     // NULL

#include "dp32g030/gpio.h"
#include "dp32g030/spi.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "driver/st7565.h"
#include "driver/system.h"
#include "misc.h"

uint8_t gStatusLine[LCD_WIDTH];
uint8_t gFrameBuffer[FRAME_LINES][LCD_WIDTH];

static void DrawLine(uint8_t column, uint8_t line, const uint8_t * lineBuffer, unsigned size_defVal)
{   
    // void unused parameter
    (void)column;
    (void)line;
    (void)lineBuffer;
    (void)size_defVal;
}

void ST7565_DrawLine(const unsigned int Column, const unsigned int Line, const uint8_t *pBitmap, const unsigned int Size)
{
    // void unused parameter
    (void)Column;
    (void)Line;
    (void)pBitmap;
    (void)Size;
}


#ifdef ENABLE_FEAT_F4HWN
    // Optimization
    //
    // ST7565_BlitScreen(0) = ST7565_BlitStatusLine()
    // ST7565_BlitScreen(1..7) = ST7565_BlitLine()
    // ST7565_BlitScreen(8) = ST7565_BlitFullScreen()
    //

    static void ST7565_BlitScreen(uint8_t line)
    {
        // void unused parameter
        (void)line;
    }

    void ST7565_BlitFullScreen(void)
    {
        
    }

    void ST7565_BlitLine(unsigned line)
    {
        // void unused parameter
        (void)line;
    }

    void ST7565_BlitStatusLine(void)
    {
        
    }
#else
    void ST7565_BlitFullScreen(void)
    {
        
    }

    void ST7565_BlitLine(unsigned line)
    {
        // void unused parameter
        (void)line;
    }

    void ST7565_BlitStatusLine(void)
    {   // the top small text line on the display
        
    }
#endif

void ST7565_FillScreen(uint8_t value)
{
    // void unused parameter
    (void)value;
}



#ifdef ENABLE_FEAT_F4HWN
    static void ST7565_Cmd(uint8_t i)
    {
        // void unused parameter
        (void)i;
    }

    #if defined(ENABLE_FEAT_F4HWN_CTR) || defined(ENABLE_FEAT_F4HWN_INV)
    void ST7565_ContrastAndInv(void)
    {
        
    }
    #endif

    int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    #if !defined(ENABLE_SPECTRUM) || !defined(ENABLE_FMRADIO)
    void ST7565_Gauge(uint8_t line, uint8_t min, uint8_t max, uint8_t value)
    {
        // void unused parameter
        (void)line;
        (void)min;
        (void)max;
        (void)value;
    }
    #endif
#endif
    
void ST7565_Init(void)
{
    
}

#ifdef ENABLE_FEAT_F4HWN_SLEEP
    void ST7565_ShutDown(void)
    {
        
    }
#endif

void ST7565_FixInterfGlitch(void)
{

}

void ST7565_HardwareReset(void)
{
    
}

void ST7565_SelectColumnAndLine(uint8_t Column, uint8_t Line)
{
    // void unused parameter
    (void)Column;
    (void)Line;
}

void ST7565_WriteByte(uint8_t Value)
{
    // void unused parameter
    (void)Value;
}
