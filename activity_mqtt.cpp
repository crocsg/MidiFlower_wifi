#include <Arduino.h>
#include <WiFi.h>
#include "ESP32MQTTClient.h"
#include "board.h"

#if BOARD_ENABLE_MQTT
ESP32MQTTClient mqttcli;
#endif

#define DATA_CHUNCK_SIZE 8

void mqtt_task(void*pvParameters);

static uint32_t last_mqtt_message = 0;
static TaskHandle_t mqtt_task_handle = NULL;

QueueHandle_t       _mqtt_data_queue;
SemaphoreHandle_t   _mqtt_data_sem;

typedef struct _mqtt_data_t
{
    uint32_t min;
    uint32_t max;
    uint32_t avg;
    uint32_t stddev; // as fixed point
} mqtt_data_t;

void mqtt_init (void)
{
    #if BOARD_ENABLE_MQTT
    // queue protection semaphore
    _mqtt_data_sem = xSemaphoreCreateMutex();

    _mqtt_data_queue = xQueueCreate(MQTT_QUEUE_ELM_NBR, sizeof(mqtt_data_t));


    xTaskCreatePinnedToCore(mqtt_task, "mqtt_send_task", 32*1024, NULL, 2, &mqtt_task_handle, MQTT_CORE);
    #endif
}

static int cnt = 0;
static uint8_t mqtt_connected = 0;
void mqtt_loop (void)
{
    #if BOARD_ENABLE_MQTT
    static mqtt_data_t data;
    int idx;

    if (xQueueReceive (_mqtt_data_queue, &data, pdMS_TO_TICKS(1000)) == pdTRUE)
    {
        char buf[256];
        snprintf (buf, sizeof(buf), "{'min': 0x%x, 'max': 0x%x, 'avg': 0x%x, 'stddev': 0x%x}", 
            data.min, data.max, data.avg, data.stddev);
        buf[sizeof(buf)-1] = 0;
        mqttcli.publish ("/test/midiflower", buf, strlen(buf), false); 

    }

    /*
    if (millis() - last_mqtt_message > 250 && mqtt_connected)
    {
        static char buf[128];
        sprintf (buf, "{'count': %d, 'time': %d}", cnt++, millis());
        mqttcli.publish ("/test/midiflower", buf, 1, false); 
        //Serial.println ("mqtt publish");   
        last_mqtt_message = millis();
    }
    */
    #endif
}

void mqtt_task(void* /*pvParameters*/)
{
  #if BOARD_ENABLE_MQTT
    mqttcli.setURI("mqtt://marsu.sgodin.com:1883");
    mqttcli.enableLastWillMessage("/test/esp/lwt", "I am going offline");
    mqttcli.setKeepAlive(30);
    mqttcli.loopStart();
    last_mqtt_message = millis () + 5000;
    //Serial.println ("mqtt started");

    while (1)
    {
        //vTaskDelay(pdMS_TO_TICKS(50));
        mqtt_loop();
        //Serial.printf ("mqtt task loop\n");
    }
    
    #endif
}

void onMqttConnect(esp_mqtt_client_handle_t client)
{
    //Serial.println("MQTT Connected");
    mqtt_connected = 1;
}

void handleMQTT(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
  auto *event = static_cast<esp_mqtt_event_handle_t>(event_data);
  #if BOARD_ENABLE_MQTT
  mqttcli.onEventCallback(event);
  #endif
}