#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "board.h"
#include "activity.h"

#ifdef PIN_NEOPIXEL

Adafruit_NeoPixel strip (NB_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB );


static uint16_t _history[NB_NEOPIXEL];
#endif

void activity_begin (void)
{
  #ifdef PIN_NEOPIXEL
  strip.begin();
  for (int i = 0; i < NB_NEOPIXEL; i++) {
      strip.setPixelColor(i, 0, 0, 255);
  }
  strip.show();
  #endif
}

static uint16_t color = 0;
static uint32_t lastvalue = 0;
void activity_event (uint32_t value)
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

void activity_process (void)
{
  if (lastvalue != 0)
  {
    if (color < lastvalue)
      color++;
    else if (color > lastvalue && color > 1)
      color--;
   
  }  
}
void activity_add_history (uint8_t value)
{
  #ifdef PIN_NEOPIXEL
  memcpy (&_history[0], &_history[1], sizeof(_history) - 1);
  _history[sizeof(_history) - 1] = value;
  #endif

   
}

void activity_show (void)
{
  #ifdef PIN_NEOPIXEL
  for (int i = 0; i < NB_NEOPIXEL; i++)
  {
    
    strip.setPixelColor (i, color == 0 ? 0L : strip.ColorHSV(color * 512, 255, 255));
  }
  strip.show ();
  #endif
}

void activity_clear ()
{
  #ifdef PIN_NEOPIXEL
  lastvalue = 0;
  color = 0;
  memset (&_history[0], 0, sizeof(_history));
  #endif
}
