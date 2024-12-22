/*
 MIT License

Copyright (c) 2022 S Godin (Climate Change Lab)
Copyright (c) 2016 electricityforprogress

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


This project is based on https://github.com/electricityforprogress/MIDIsprout great
work about biodata sonification
*/

#include <Arduino.h>
#include "board.h"

#include "midinote.h"
#include "flower_music.h"
#include "sequence.h"
#include "flower_sensor.h"
#include "MidiFlowerSequencer.h"
#include "webserver_config.h"
#include "wifiap.h"
#include "config.h"
#include "activity.h"
#include "activity_dmx.h"
#include "activity_mqtt.h"




#define NBNOTE_FOR_BETTER_MEASURE   15

static uint32_t chipId = 0;


// get the sequencer from flower_music
extern CMidiFlowerSequencer sequencer;  


void flowersensor_measure (uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical);
void flowersensor_measure_light (uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical);


// microcontroleur initialisation

void setup()
{
  // initialize led output
  pinMode(LED, OUTPUT);
	digitalWrite(LED, LED_ON); // set led ON
	
  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  // start Serial if you want to debug
  Serial.begin(115200);                 //initialize Serial for debug

  config_init ();

  sequencer.Init (chipId);

   // start NEOPIXEL
  #if NEO_PIXEL_ENABLE
  #ifdef PIN_NEOPIXEL
    activity_begin ();
  #endif 
  #endif

  
  
  
  // start wifi Access Point
  wifiap_init (chipId);
  
  // start web server
  webserver_config_init ();
  
  // start music sequencer
  flower_music_init ();
  
  #if DMX_ENABLE
    activity_dmx_begin ();
  #endif

  // start flower sensor
  flower_sensor_init(FLOWER_SENSOR_PIN);
  flower_sensor_set_analyse_short(1);

  // define a function to get measures
  flower_sensor_set_callback (flowersensor_measure);
  flower_sensor_set_callback_light (flowersensor_measure_light);
  
  // load config
  config_load ();
  
  // enable sequencer
  sequencer.setLock(false);
}

// 
static uint8_t cnt = 0;
static uint32_t last_activity = 0;
static uint32_t last_report = 0;
static uint8_t mqtt_connected = 0;

void loop()
{
  
  digitalWrite(LED, ((++cnt) & 0x01) == 0 ? LED_ON : LED_OFF); // set led ON
  
  // good music is good rythm. You must call this very often
  // a least one time by 10 millisec
  sequencer.Loop ();
  
  
  // build flower measure
  // must be called often
  flower_sensor_build_mes();

  // do some control stuff
  ControlMusic ();

  // if we have some music notes, change configuration for better measure
 if (flower_music_get_loop() ==0)
 {
    flower_sensor_set_analyse_short(2);
 }
 else if (sequencer.get_track_nbnote(0) > NBNOTE_FOR_BETTER_MEASURE )
  {
    flower_sensor_set_analyse_short(0);
  }

  #if NEOPIXEL_ENABLE
    #ifdef PIN_NEOPIXEL
    if (millis () - last_activity > 25)
    {
      activity_process ();
      activity_show ();

      if (wifi_isStationStarted())
      {
        if (! mqtt_connected)
        {
          mqtt_connected = 1;
          mqtt_init ();
        }
        
      }

      last_activity = millis ();
    }
    #endif
  #endif
  #if DMX_ENABLE
  if (millis () - last_activity > 25)
  {
    activity_dmx_process ();
    activity_dmx_show ();
    last_activity = millis ();
  }
  #endif
  
  if (millis () - last_report > 5 * 60000)
  {
    //Serial.print("Setup: Executing on core ");
    //Serial.println(xPortGetCoreID());
    //Serial.print("portTICK_PERIOD_MS ");
    //Serial.println(portTICK_PERIOD_MS);
    last_report = millis();
  }
}

static uint32_t lastaverag = 0;
static uint16_t color = 0;

// Flower sensor measure callback. receive flower measures 
// min    min value
// max    max value
// averg  average
// delta  max - min
// stdevi standard deviation
// stdevical standard deviation * threshold
void flowersensor_measure (uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical)
{
    // give all the measure to flower music generation code
    BuildNoteFromMeasure (millis(), min, max, averg, delta, stdevi, stdevical);  
}

// Flower sensor measure callback. receive flower measures 
// min    min value
// max    max value
// averg  average
// delta  max - min
// stdevi standard deviation
// stdevical standard deviation * threshold
void flowersensor_measure_light (uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical)
{
    
    #if NEOPIXEL_ENABLE
      #ifdef PIN_NEOPIXEL
      activity_event (delta%64);
      #endif 
    #endif

    #if DMX_ENABLE
      activity_dmx_event (delta%255);  
    #endif

    #if REPORT_DATA_SERIAL
    static char buf[256];
    snprintf (buf, sizeof(buf), "min:%d,max:%d,averg:%d,delta:%d,stdevi:%f\n", min, max, averg, delta, stdevi);
    buf[sizeof(buf)-1] = 0;
    Serial.println(buf);
    #endif
}
