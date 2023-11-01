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



#define SAMPLESIZE  32

#define BOARD_LOLIN32           0
#define BOARD_VELLEMAN_ESP32    1
#define BOARD_ESP32_DEVKIT    	2
#define BOARD_DOIT_DEVKIT       4
#define BOARD_CUSTOM			      5

// change the define accordingly to your configuration
//#define BOARD_CONFIG    BOARD_LOLIN32
#define BOARD_CONFIG    BOARD_VELLEMAN_ESP32
//#define BOARD_CONFIG    BOARD_ESP32_DEVKIT
//#define BOARD_CONFIG    BOARD_DOIT_DEVKIT
//#define BOARD_CONFIG    BOARD_CUSTOM

#if (BOARD_CONFIG == BOARD_LOLIN32)
#define LED                        5     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  LOW
#define LED_OFF HIGH

#define FLOWER_SENSOR_PIN          2     // galvanometer input (flower sensor)

#define MIDI_BLE_OUTPUT             1     // output on Bluetooth Midi
#define MIDI_SERIAL_OUTPUT          0     // output on Serial Midi

#define PIN_TX_MIDI                -1     // Midi serial TX pin
#define PIN_RX_MIDI                -1

#endif

#if (BOARD_CONFIG == BOARD_VELLEMAN_ESP32)
#define LED                         5     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  HIGH
#define LED_OFF LOW

#define FLOWER_SENSOR_PIN          12     // galvanometer input (flower sensor)

#define MIDI_BLE_OUTPUT             1     // output on Bluetooth Midi
#define MIDI_SERIAL_OUTPUT          1    // output on Serial Midi

#define PIN_TX_MIDI                25
#define PIN_RX_MIDI                26

#define PIN_NEOPIXEL               33
#define NB_NEOPIXEL                16



#endif

#if (BOARD_CONFIG == BOARD_ESP32_DEVKIT)
#define LED                         2     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  HIGH
#define LED_OFF LOW

#define FLOWER_SENSOR_PIN          12     // galvanometer input (flower sensor)

#define MIDI_BLE_OUTPUT             1     // output on Bluetooth Midi
#define MIDI_SERIAL_OUTPUT          0     // output on Serial Midi

#define PIN_TX_MIDI                -1     // Midi serial TX pin
#define PIN_RX_MIDI                -1

#endif

#if (BOARD_CONFIG == BOARD_DOIT_DEVKIT)
#define LED                         2     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  HIGH
#define LED_OFF LOW
#define FLOWER_SENSOR_PIN          27     // galvanometer input (flower sensor)

#define MIDI_BLE_OUTPUT             1     // output on Bluetooth Midi
#define MIDI_SERIAL_OUTPUT          0     // output on Serial Midi

#define PIN_TX_MIDI                25
#define PIN_RX_MIDI                26

#define PIN_NEOPIXEL               12
#define NB_NEOPIXEL                16

#endif

#if (BOARD_CONFIG == BOARD_CUSTOM)
#define LED                         2     // ESP32 Onboard LED (depends on ESP32 board)
#define LED_ON  HIGH
#define LED_OFF LOW


#define FLOWER_SENSOR_PIN          27     // galvanometer input (flower sensor)
#define MIDI_BLE_OUTPUT             1     // output on Bluetooth Midi
#define MIDI_SERIAL_OUTPUT          0     // output on Serial Midi

#define PIN_TX_MIDI                25
#define PIN_RX_MIDI                26




#endif



#endif // __BOARD_H
