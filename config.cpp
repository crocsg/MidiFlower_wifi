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

#include "config.h"
#include <ArduinoJson.h>

#include "flower_music.h"
#include "MidiFlowerSequencer.h"

#define JSON_MAX_SIZE 1024

#define JSON_BPM_TAG "bpm"
#define JSON_SCALE_TAG "scale"
#define JSON_ROOT_TAG "root"
#define JSON_CHANNEL_MUL "mul"


StaticJsonDocument<1024> jdoc;

extern CMidiFlowerSequencer sequencer;  

static void config_set_json (void)
{
    
    jdoc[JSON_BPM_TAG] = flower_music_get_basebpm ();
    jdoc[JSON_SCALE_TAG] = flower_music_get_scale ();
    jdoc[JSON_ROOT_TAG] = flower_music_get_current_root ();
    
    JsonArray array = jdoc.to<JsonArray>();

    for (uint8_t c = 0; c < sequencer.get_nbtracks (); c++)
    {
        JsonObject channels = jdoc.createNestedObject();
        channels[JSON_CHANNEL_MUL] = sequencer.get_track_mulbpm (c);
        array.add (channels); 
    }
    
}

void config_save (void)
{
    String content = "json debile";
    //jdoc.clear ();
    config_set_json ();
    serializeJson(jdoc, content);
    //Serial.printf("JSON content: %s\r\n", content);
    //serializeJsonPretty(jdoc, Serial);
}