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
#include <FS.h>
#include <SPIFFS.h>

#include "flower_music.h"
#include "flower_sensor.h"
#include "MidiFlowerSequencer.h"

#define JSON_MAX_SIZE 1024

#define JSON_BPM_TAG "bpm"
#define JSON_SCALE_TAG "scale"

#define JSON_ROOT_TAG "root"
#define JSON_LOOP_TAG "loop"
#define JSON_LEVEL_TAG "level"
#define JSON_OCTAVE_START "notemin"
#define JSON_CHANNEL1_MUL "mul1"
#define JSON_CHANNEL2_MUL "mul2"
#define JSON_CHANNEL3_MUL "mul3"
#define JSON_CHANNEL4_MUL "mul4"
#define JSON_CHANNEL1_SIZE "siz1"
#define JSON_CHANNEL2_SIZE "siz2"
#define JSON_CHANNEL3_SIZE "siz3"
#define JSON_CHANNEL4_SIZE "siz4"
#define JSON_CHANNEL1_FILL "fill1"
#define JSON_CHANNEL2_FILL "fill2"
#define JSON_CHANNEL3_FILL "fill3"
#define JSON_CHANNEL4_FILL "fill4"

#define JSON_CHANNEL_MUL "mul"

#define FORMAT_SPIFFS_IF_FAILED true

static  StaticJsonDocument<1024> jdoc;
static  const char* config_path = "/config.jso";

extern CMidiFlowerSequencer sequencer;  

static void config_set_json (void)
{
    
    jdoc[JSON_BPM_TAG] = flower_music_get_basebpm ();
    jdoc[JSON_SCALE_TAG] = flower_music_get_scale ();
    jdoc[JSON_ROOT_TAG] = flower_music_get_current_root ();
    jdoc[JSON_LOOP_TAG] = flower_music_get_loop ();
    jdoc[JSON_LEVEL_TAG] = flower_sensor_get_auto_threshold ();
    
    jdoc[JSON_OCTAVE_START] = flower_music_get_note_min ();

    jdoc[JSON_CHANNEL1_MUL] = sequencer.get_track_mulbpm(0);
    jdoc[JSON_CHANNEL2_MUL] = sequencer.get_track_mulbpm(1);
    jdoc[JSON_CHANNEL3_MUL] = sequencer.get_track_mulbpm(2);
    jdoc[JSON_CHANNEL4_MUL] = sequencer.get_track_mulbpm(3);

    jdoc[JSON_CHANNEL1_FILL] = sequencer.get_track_ratio(0);
    jdoc[JSON_CHANNEL2_FILL] = sequencer.get_track_ratio(1);
    jdoc[JSON_CHANNEL3_FILL] = sequencer.get_track_ratio(2);
    jdoc[JSON_CHANNEL4_FILL] = sequencer.get_track_ratio(3);

    jdoc[JSON_CHANNEL1_SIZE] = sequencer.get_track_size (0);
    jdoc[JSON_CHANNEL2_SIZE] = sequencer.get_track_size (1);
    jdoc[JSON_CHANNEL3_SIZE] = sequencer.get_track_size (2);
    jdoc[JSON_CHANNEL4_SIZE] = sequencer.get_track_size (3);
    
    
    
}

static int get_json_int (const char* tag)
{
    int v = jdoc[tag];
    Serial.printf ("tag=%s =%d\n", tag, v);
    return v;
}
static void config_get_json (void)
{
       
    int v = jdoc[JSON_BPM_TAG];
    Serial.printf ("value json %d\n", v);
    Serial.printf ("value json %d\n", get_json_int (JSON_BPM_TAG));
    
    flower_music_set_basebpm (get_json_int (JSON_BPM_TAG));
    flower_music_set_scale (get_json_int (JSON_SCALE_TAG));
    flower_music_set_root (get_json_int (JSON_ROOT_TAG));
    flower_music_set_note_min (get_json_int (JSON_OCTAVE_START));
    flower_music_set_loop (get_json_int (JSON_LOOP_TAG));
    flower_sensor_set_auto_threshold (get_json_int (JSON_LEVEL_TAG));

    sequencer.set_track_mulbpm(0, get_json_int (JSON_CHANNEL1_MUL));
    sequencer.set_track_mulbpm(1, get_json_int (JSON_CHANNEL2_MUL));
    sequencer.set_track_mulbpm(2, get_json_int (JSON_CHANNEL3_MUL));
    sequencer.set_track_mulbpm(3, get_json_int (JSON_CHANNEL4_MUL));

    sequencer.set_track_ratio(0, get_json_int (JSON_CHANNEL1_FILL));
    sequencer.set_track_ratio(1, get_json_int (JSON_CHANNEL2_FILL));
    sequencer.set_track_ratio(2, get_json_int (JSON_CHANNEL3_FILL));
    sequencer.set_track_ratio(3, get_json_int (JSON_CHANNEL4_FILL));

    sequencer.set_track_size (0, get_json_int (JSON_CHANNEL1_SIZE));
    sequencer.set_track_size (1, get_json_int (JSON_CHANNEL2_SIZE));
    sequencer.set_track_size (2, get_json_int (JSON_CHANNEL3_SIZE));
    sequencer.set_track_size (3, get_json_int (JSON_CHANNEL4_SIZE));
    
}

void config_save (void)
{
    jdoc.clear ();
    config_set_json ();

  

    // Open file for writing
    
    File file = SPIFFS.open(config_path, FILE_WRITE);
    if (!file) {
        Serial.println(F("Failed to create file"));
        return;
    }

    // save config json
    if (serializeJson(jdoc, file) == 0) {
        Serial.println(F("Failed to write to file"));
    }
    serializeJson (jdoc, Serial);
    // Close the file
    file.close();
    
}

void config_load (void)
{
    File file = SPIFFS.open(config_path);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.


    // Deserialize the JSON document
    //jdoc.clear ();
    Serial.printf ("deserialize\n");
    DeserializationError error = deserializeJson(jdoc, file);
    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        file.close ();
        return;
    }  

    Serial.printf ("Read config\n");
    config_get_json ();
    // Close the file (Curiously, File's destructor doesn't close the file)
    file.close();
}

void config_init (void)
{
    // Initialize SD library
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
   }
}