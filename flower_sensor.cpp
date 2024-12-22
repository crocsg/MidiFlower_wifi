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
#include <cmath>
#include <cstdint>


#include "flower_sensor.h"
#include "board.h"

#define DESIRED_EVENT 24
#define SAMPLESIZE    32
#define MIN_DELTA     750

static flower_sensor_callback_mes _setmes = NULL;
static flower_sensor_callback_mes _setlight = NULL;

static uint8_t      _samplesize = SAMPLESIZE / 2; //set sample array size
static uint8_t      state;
volatile uint32_t   _microseconds = 0; //sampling timer
volatile uint8_t    _sindex = 0;
volatile uint32_t   _samples[SAMPLESIZE];
volatile uint32_t   _last_samples = 0;

static float threshold = 1;  //change threshold multiplier
static float threshMin =  0.0001; //scaling threshold min
static float threshMax = 15; //scaling threshold max
static uint8_t auto_threshold = 1;
static uint32_t  threshold_last_millis = 0;
static uint32_t  threshold_evt = 0;



//*******************************


  void flower_sensor_interrupt ()
  {
    if(_sindex < _samplesize)
    {
        uint32_t now = micros();
        _samples[_sindex] = now - _microseconds;
        _microseconds = now;
        _sindex += 1;
    }

    //digitalWrite(LED, ((state) & 0x01) == 0 ? HIGH : LOW);
    _last_samples = millis ();  // last measure time
    state++;
}


void flower_sensor_init (int interruptPin)
{

  _last_samples = millis ();
  pinMode (interruptPin, INPUT);
  attachInterrupt(interruptPin, flower_sensor_interrupt, RISING);  //begin sampling data from interrupt

}

uint8_t flower_sensor_data_available (void)
{
  return (_sindex >= _samplesize ? 1 : 0);
}

void flower_sensor_set_callback (flower_sensor_callback_mes clbk)
{
  _setmes = clbk;
}

void flower_sensor_set_callback_light (flower_sensor_callback_mes clbk)
{
  _setlight = clbk;
}

void flower_sensor_set_analyse_short (uint8_t s)
{
  if (s != 0)
      _samplesize = SAMPLESIZE / s;
  else
    _samplesize = SAMPLESIZE;
}

float map2float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void flower_sensor_update_threshold (void)
{

  if (auto_threshold == 1)
  {
  uint32_t currentMillis = millis ();
  if (currentMillis - threshold_last_millis > 5000)
  {

      if (threshold_evt < DESIRED_EVENT)
      {
        if (threshold > threshMin)
          threshold /= 1.4;
      }
      else
      {
        if (threshold < threshMax)
          threshold *= 1.4;
      }
      threshold_last_millis = currentMillis;
      threshold_evt = 0;

    }
  }
  
  else
  {
    int val = analogRead (PIN_ADC_THESHOLD);
    threshold = map2float (val, 0, 4095, threshMin, threshMax);
    //Serial.printf ("threshold %d %f\r\n", val, threshold);
  }
}

void flower_sensor_build_mes (void)
{
  if (flower_sensor_data_available ())
      flower_sensor_analyzeSample (); // build data

  flower_sensor_update_threshold (); // update measure threshold
}


uint8_t flower_sensor_get_auto_threshold (void)
{
  return auto_threshold;
}

void flower_sensor_set_auto_threshold (uint8_t s)
{
  auto_threshold = s;
}



uint32_t flower_sensor_get_last_sample_time_ms (void)
{
  return _last_samples;
}

void flower_sensor_analyzeSample(void)
{
  uint32_t averg = 0;
  uint32_t maxim = 0;
  uint32_t minim = 10000000;
  float stdevi = 0;
  uint32_t delta = 0;
  uint8_t change = 0;


  
   //digitalWrite(LED, ((state) & 0x01) == 0 ? LED_ON : LED_OFF);
  state++;
  if (_sindex >= _samplesize) { //array is full
    unsigned long sampanalysis[SAMPLESIZE];
    #if 0
    for (uint8_t i=0; i < _samplesize; i++){
      //skip first element in the array
      sampanalysis[i] = _samples[i];  //load analysis table (due to volitle)
      //manual calculation
      if(sampanalysis[i] > maxim) { maxim = sampanalysis[i]; }
      if(sampanalysis[i] < minim) { minim = sampanalysis[i]; }
      averg += sampanalysis[i];

    }
    averg = averg / (_samplesize);
    for (uint8_t i = 0; i < _samplesize; i++)
    {
      stdevi += (sampanalysis[i] - averg) * (sampanalysis[i] - averg) ;  //prep stdevi
    }

    // compute std deviation
    stdevi = stdevi / (_samplesize);
    if (stdevi < 1) 
    { 
      stdevi = 1.0; //min stdevi of 1
    } 

    stdevi = sqrt(stdevi); //calculate stdevi
    #else
    for (uint8_t i=0; i < _samplesize; i++){
      //skip first element in the array
      sampanalysis[i] = _samples[i];  //load analysis table (due to volitle)
      //manual calculation
      if(sampanalysis[i] > maxim) { maxim = sampanalysis[i]; }
      if(sampanalysis[i] < minim) { minim = sampanalysis[i]; }
      averg += sampanalysis[i];
      stdevi += sampanalysis[i] * sampanalysis[i];
    }
    averg = averg / (_samplesize);
    //for (uint8_t i = 0; i < _samplesize; i++)
    //{
    //  stdevi += (sampanalysis[i] - averg) * (sampanalysis[i] - averg) ;  //prep stdevi
    //}
    double tmpstd = (stdevi / _samplesize) - (averg * averg);
    if (tmpstd > 0.0)
      stdevi = sqrt(tmpstd); //calculate stdevu

    else stdevi = 1;
    
    // compute std deviation
    //stdevi = stdevi / (_samplesize);
    if (stdevi < 1) 
    { 
      stdevi = 1.0; //min stdevi of 1
    } 
    
    #endif

    delta = maxim - minim;

    
    //Serial.printf ("analyze %d %f %ld %f\r\n", _sindex, threshold, delta, (stdevi * threshold));
    //**********perform change detection
    if (delta > (stdevi * threshold) && delta > MIN_DELTA)
    {
      change = 1;
      threshold_evt ++;
    }

    if (_setlight)
    {
      _setlight (minim, maxim, averg, delta, stdevi, stdevi * threshold);
    }
    if (change && _setmes)
    {
      //Serial.printf("%ld %ld %ld %ld %f %f\r\n", minim, maxim, averg, delta, stdevi, stdevi * threshold); 
      _setmes (minim, maxim, averg, delta, stdevi, stdevi * threshold);
    }
    //reset array for next sample
     _sindex = 0;

  }
}
