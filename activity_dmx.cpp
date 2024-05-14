/*
 MIT License

Copyright (c) 2024 S Godin (Climate Change Lab)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "board.h"
#include "activity_dmx.h"

#if DMX_ENABLE
#include "dmx.h"
#endif

#if DMX_ENABLE
static Adafruit_NeoPixel _strip (NB_NEOPIXEL, PIN_DISABLED_NEOPIXEL, NEO_RGBW );
static uint16_t _history[NB_NEOPIXEL*2];
#endif

void activity_dmx_begin (void)
{
  
  #if DMX_ENABLE
  DMX::Initialize(output);
  DMX::WriteAll(0, 1, 512);
  DMX::Write(1, 0);
  DMX::Write(2, 255);
  DMX::Write(3, 0);
  DMX::Write(4, 0);
  #endif
}

static uint16_t color = 0;
static uint32_t lastvalue = 0;
void activity_dmx_event (uint32_t value)
{
  /*
  if (value > lastvalue && color < 32)
    color++;
  if (value < lastvalue && color > 1)   
    color--;
  activity_add_history (value & 0xff);
  */
  lastvalue = value;
  
  
    
     
}

void activity_dmx_process (void)
{
  if (lastvalue != 0)
  {
    if (color < lastvalue)
      color++;
    else if (color > lastvalue && color > 1)
      color--;
   
  }  
}
void activity_dmx_add_history (uint8_t value)
{
  #if DMX_ENABLE
  memmove (&_history[0], &_history[1], (sizeof(_history) - 1));
  _history[sizeof(_history) - 1] = value;
  #endif

   
}

void activity_dmx_show (void)
{
  
  #if DMX_ENABLE
  uint32_t rgbw = _strip.ColorHSV(color * 512, 255, 255);
  //DMX::Write(1, rgbw & 0xff );
  //DMX::Write(2, (rgbw << 8) & 0xff);
  //DMX::Write(3, (rgbw << 16) & 0xff);
  //DMX::Write(4, (rgbw << 24) & 0xff);
  DMX::Write(1, 0xff );
  DMX::Write(2, 0xff );
  DMX::Write(3, 0 );
  DMX::Write(4, 0 );
  DMX::Write(5, 0xFF );
  DMX::Write(6, 0xFF );
  #endif
}

void activity_dmx_clear ()
{
  #if DMX_ENABLE
  lastvalue = 0;
  color = 0;
  memset (&_history[0], 0, sizeof(_history));
  #endif
}
