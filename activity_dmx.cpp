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
  Serial.println ("DMX initialized");
  DMX::WriteAllValue(0, 1, 512);
  DMX::Write(1, 9);
  DMX::Write(2, 0);
  DMX::Write(3, 0);
  DMX::Write(4, 255);
  DMX::Write(5, 255);
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
  memmove (&_history[0], &_history[1], (sizeof(_history) - sizeof(_history[0])));
  _history[(sizeof(_history)/sizeof(_history[0])) - 1] = value;
  #endif

   
}

void activity_dmx_show (void)
{
  
  #if DMX_ENABLE
  if (color != 0)
  {
    _strip.setBrightness(255);
    
    
    
    uint32_t rgbw = _strip.ColorHSV(color *255, 255, 255);
    //Serial.print ("DMX ");
    //Serial.print (color);
    //Serial.print (" ");
    //Serial.println (rgbw, HEX);
    DMX::Write(1, 9);
    DMX::Write(2, 0);
    DMX::Write(3, 0);
    DMX::Write(4, 255);
    DMX::Write(5, rgbw & 0xff );
    DMX::Write(6, (rgbw >> 8) & 0xff);
    DMX::Write(7, (rgbw >> 16) & 0xff);
    DMX::Write(8, 0 );

    DMX::Write(11, 9);
    DMX::Write(12, 0);
    DMX::Write(13, 0);
    DMX::Write(14, 255);
    DMX::Write(15, rgbw & 0xff );
    DMX::Write(16, (rgbw >> 8) & 0xff);
    DMX::Write(17, (rgbw >> 16) & 0xff);
    DMX::Write(18, 0xff );

    DMX::Write(21, 9);
    DMX::Write(22, 0);
    DMX::Write(23, 0);
    DMX::Write(24, 255);
    DMX::Write(25, rgbw & 0xff );
    DMX::Write(26, 0);
    DMX::Write(27, 0);
    DMX::Write(28, 0);

    DMX::Write(31, 9);
    DMX::Write(32, 0);
    DMX::Write(33, 0);
    DMX::Write(34, 255);
    DMX::Write(35, rgbw & 0xff );
    DMX::Write(36, 0);
    DMX::Write(37, 0);
    DMX::Write(38, rgbw & 0xff);

    DMX::Write(41, 9);
    DMX::Write(42, 0);
    DMX::Write(43, 0);
    DMX::Write(44, 255);
    DMX::Write(45, 0);
    DMX::Write(46, rgbw & 0xff);
    DMX::Write(47, 0);
    DMX::Write(48, 0);

    DMX::Write(51, 9);
    DMX::Write(52, 0);
    DMX::Write(53, 0);
    DMX::Write(54, 255);
    DMX::Write(55, 0);
    DMX::Write(56, rgbw & 0xff);
    DMX::Write(57, 0);
    DMX::Write(58, rgbw & 0xff);

    DMX::Write(61, 9);
    DMX::Write(62, 0);
    DMX::Write(63, 0);
    DMX::Write(64, 255);
    DMX::Write(65, rgbw & 0xff );
    DMX::Write(66, (rgbw >> 8) & 0xff);
    DMX::Write(67, (rgbw >> 16) & 0xff);
    DMX::Write(68, 96 );

    DMX::Write(71, 9);
    DMX::Write(72, 0);
    DMX::Write(73, 0);
    DMX::Write(74, 255);
    DMX::Write(75, 0);
    DMX::Write(76, 0);
    DMX::Write(77, rgbw & 0xff);
    DMX::Write(78, 0);

    DMX::Write(81, 9);
    DMX::Write(82, 0);
    DMX::Write(83, 0);
    DMX::Write(84, 255);
    DMX::Write(85, 0);
    DMX::Write(86, 0);
    DMX::Write(87, rgbw & 0xff);
    DMX::Write(88, rgbw & 0xff);

    DMX::Write(91, 9);
    DMX::Write(92, 0);
    DMX::Write(93, 0);
    DMX::Write(94, 255);
    DMX::Write(95, 0);
    DMX::Write(96, 0);
    DMX::Write(97, 0);
    DMX::Write(98, rgbw & 0xff);

      DMX::Write(101, 0xff );
      DMX::Write(102, rgbw & 0xff );
      DMX::Write(103, (rgbw >> 8) & 0xff);
      DMX::Write(104, (rgbw >> 16) & 0xff);
      DMX::Write(105, 0 );
      DMX::Write(106, 0 );
      DMX::Write(107, 0 );
      DMX::Write(108, 0 );
      

      DMX::Write(111, 0xff );
      DMX::Write(112, rgbw & 0xff );
      DMX::Write(113, (rgbw >> 8) & 0xff);
      DMX::Write(114, (rgbw >> 16) & 0xff);
      DMX::Write(115, 0xff );
      DMX::Write(116, 0 );
      DMX::Write(117, 0 );
      DMX::Write(118, 0 );

  }
  #endif
}

void activity_dmx_clear ()
{
  #if DMX_ENABLE
  DMX::WriteAllValue(0, 1, 512);
  lastvalue = 0;
  color = 0;
  memset (&_history[0], 0, sizeof(_history));
   DMX::Write(1, 201);
    DMX::Write(2, 110);
    DMX::Write(3, 16);
    DMX::Write(4, 32);
    DMX::Write(5, 0 );
    DMX::Write(6, 0);
    DMX::Write(7, 0);
    DMX::Write(8, 0 );
  #endif
}
