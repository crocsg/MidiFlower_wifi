/*
 MIT License

Copyright (c) 2022 S Godin (Climate Change Lab)

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
#include "wifiserver.h"
#include <WiFi.h>

#define HTTP_PORT   80


static char WIFI_SSID[63]= ""; 
static char WIFI_PASSWORD[63] = "12345678";    // i suggest to use a better password

IPAddress local_IP(192,168,163,1);
IPAddress gateway_IP(192,168,163,1);
IPAddress subnet_IP(255,255,255,0);





void wifiap_init (uint32_t chipId)
{
    snprintf(WIFI_SSID, sizeof(WIFI_SSID), "MidiFlowerDMX_%08lx", chipId); // build WIFI access point name
    WiFi.disconnect();   
    WiFi.mode(WIFI_OFF); 
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway_IP, subnet_IP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    
    delay(250);   // a little time to let AP start
    Serial.printf ("wifi: %s %s\r\n", WIFI_SSID, WIFI_PASSWORD);
}

IPAddress& wifiap_get_local_ip (void)
{
    return local_IP;
}
