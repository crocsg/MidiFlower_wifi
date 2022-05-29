/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 S Godin (Climate Change Lab)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */




#ifndef __BOARD_H
#define __BOARD_H

#include <Arduino.h>

#define SAMPLESIZE  32

#define BOARD_LOLIN32           0
#define BOARD_VELLEMAN_ESP32    1

// change the define accordingly to your configuration
//#define BOARD_CONFIG    BOARD_LOLIN32
#define BOARD_CONFIG    BOARD_LOLIN32

#if (BOARD_CONFIG == BOARD_VELLEMAN_ESP32)
#define LED                        5     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  LOW
#define LED_OFF HIGH

#define FLOWER_SENSOR_PIN          2     // galvanometer input (flower sensor)

#endif

#if (BOARD_CONFIG == BOARD_VELLEMAN_ESP32)
#define LED                         5     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  HIGH
#define LED_OFF LOW

#define FLOWER_SENSOR_PIN          12     // galvanometer input (flower sensor)
#endif



#endif // __BOARD_H
