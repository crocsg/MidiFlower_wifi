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


#ifndef __FLOWER_SENSOR
#define __FLOWER_SENSOR



typedef void(* flower_sensor_callback_mes) (uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical);

void flower_sensor_init (int pin);
void flower_sensor_set_callback (flower_sensor_callback_mes clbk);
void flower_sensor_set_callback_light (flower_sensor_callback_mes clbk);
void flower_sensor_analyzeSample(void);
uint8_t flower_sensor_data_available (void);
void flower_sensor_set_analyse_short (uint8_t s);
void flower_sensor_update_threshold (void);
uint32_t flower_sensor_get_last_sample_time_ms (void);
void flower_sensor_build_mes ();
uint8_t flower_sensor_get_auto_threshold (void);
void flower_sensor_set_auto_threshold (uint8_t s);
#endif // __FLOWER_SENSOR
