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
#include "board.h"
#include "wifiap.h"

#define HTTP_PORT 80

static char WIFI_SSID[63] = "";
static char WIFI_PASSWORD[63] = "12345678"; // i suggest to use a better password

static char WIFI_STA_SSID[128] = "maisonliffre";
static char WIFI_STA_PWD[64] = "02101967";

IPAddress local_IP(192, 168, 163, 1);
IPAddress gateway_IP(192, 168, 163, 1);
IPAddress subnet_IP(255, 255, 255, 0);

IPAddress station_IP(0, 0, 0, 0);
IPAddress station_gw_IP(0, 0, 0, 0);
IPAddress station_subnet_mask(255, 255, 255, 0);

static uint8_t _station_started = 0;
static TaskHandle_t wifi_task_handle = NULL;
static uint32_t wifi_id = 0;

void WiFiEvent(WiFiEvent_t event) {
  //Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:               Serial.println("WiFi interface ready"); break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:           Serial.println("Completed scan for access points"); break;
    case ARDUINO_EVENT_WIFI_STA_START:           Serial.println("WiFi client started"); break;
    case ARDUINO_EVENT_WIFI_STA_STOP:            Serial.println("WiFi clients stopped"); break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:       Serial.println("Connected to access point"); break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:    Serial.println("Disconnected from WiFi access point"); break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE: Serial.println("Authentication mode of access point has changed"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      //Serial.print("Obtained IP address: ");
      //Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:        Serial.println("Lost IP address and IP address is reset to 0"); break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:          Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_FAILED:           Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:          Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode"); break;
    case ARDUINO_EVENT_WPS_ER_PIN:              Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode"); break;
    case ARDUINO_EVENT_WIFI_AP_START:           Serial.println("WiFi access point started"); break;
    case ARDUINO_EVENT_WIFI_AP_STOP:            Serial.println("WiFi access point  stopped"); break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:    Serial.println("Client connected"); break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED: Serial.println("Client disconnected"); break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:   Serial.println("Assigned IP address to client"); break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:  Serial.println("Received probe request"); break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:         Serial.println("AP IPv6 is preferred"); break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:        Serial.println("STA IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_GOT_IP6:             Serial.println("Ethernet IPv6 is preferred"); break;
    case ARDUINO_EVENT_ETH_START:               Serial.println("Ethernet started"); break;
    case ARDUINO_EVENT_ETH_STOP:                Serial.println("Ethernet stopped"); break;
    case ARDUINO_EVENT_ETH_CONNECTED:           Serial.println("Ethernet connected"); break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:        Serial.println("Ethernet disconnected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:              Serial.println("Obtained IP address"); break;
    default:                                    break;
  }
}

void onWiFiEvent(WiFiEvent_t event)
{
    //Serial.printf ("wifi event 0x%x 0x%x\n", (int) event, WIFI_EVENT_STA_CONNECTED);
    switch (event)
    {
    case ARDUINO_EVENT_ETH_CONNECTED:
    case WIFI_EVENT_STA_CONNECTED:
        _station_started = 1;
        //Serial.println("ESP32 Connected to WiFi Network");
        break;
    case WIFI_EVENT_AP_START:
        //Serial.println("ESP32 soft AP started");
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        //Serial.println("Station connected to ESP32 soft AP");
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        //Serial.println("Station disconnected from ESP32 soft AP");
        break;
    default:
        break;
    }
    WiFiEvent (event);
}
void wifiap_task (void* /*pparam*/)
{
    snprintf(WIFI_SSID, sizeof(WIFI_SSID), "MidiFlower2_%08lx", wifi_id); // build WIFI access point name
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.onEvent(onWiFiEvent);

#if BOARD_ENABLE_WIFI_STA
    WiFi.mode(WIFI_MODE_APSTA); // Access point + station
    Serial.println("Wifi AP+STA enabled");
#else
    WiFi.mode(WIFI_AP); // AccessPoint only
#endif

    WiFi.softAPConfig(local_IP, gateway_IP, subnet_IP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PWD);

#if BOARD_ENABLE_WIFI_STA
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
#endif

    delay(250); // a little time to let AP start
    _station_started = 1;
    Serial.printf("wifi: %s %s\r\n", WIFI_SSID, WIFI_PASSWORD);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));

    }
    
}

void wifiap_init(uint32_t chipId)
{
    wifi_id = chipId;
    xTaskCreatePinnedToCore(wifiap_task, "wifi_task", 16*1024, NULL, 1, &wifi_task_handle, WIFI_CORE);
}

IPAddress &wifiap_get_local_ip(void)
{

    return local_IP;
}

uint8_t wifi_isStationStarted (void)
{
    return _station_started;
}