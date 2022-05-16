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

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <uri/UriBraces.h>

#include "webserver_config.h"
#include "flower_music.h"
#include "wifiap.h"
#include "util.h"

#define HTTP_PORT 80
#define DNS_PORT 53

WebServer server(HTTP_PORT);
DNSServer dnsServer;

// get the sequencer from flower_music
extern CMidiFlowerSequencer sequencer;  


void handle_OnHome();
void handle_Onsetroot();
void handle_Onsettempo();
void handle_Onsetscale();
void handle_Onsetmul1();
void handle_Onsetmul2();
void handle_Onsetmul3();
void handle_Onsetmul4();
void handle_NotFound();
String SendHTML(void);

const char *midinotes[] =
    {
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "E#",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B",
};

const uint16_t miditempo[] =
    {
        40,
        60,
        75,
        90,
        96,
        120,
        130,
        135,
        140,
        145,
        160,
        175,

};

const uint16_t notemul[] =
{
        0,1,2,4

};

void webserver_config_init(void)
{
    server.on("/", handle_OnHome);                           // home page
    server.on(UriBraces("/setroot={}"), handle_Onsetroot);   // set root page
    server.on(UriBraces("/settempo={}"), handle_Onsettempo); // set root page
    server.on(UriBraces("/setscale={}"), handle_Onsetscale); // set scale page
    server.on(UriBraces("/setmul1={}"), handle_Onsetmul1);   // set time multiplier
    server.on(UriBraces("/setmul2={}"), handle_Onsetmul2);   // set time multiplier
    server.on(UriBraces("/setmul3={}"), handle_Onsetmul3);   // set time multiplier
    server.on(UriBraces("/setmul4={}"), handle_Onsetmul4);   // set time multiplier

    server.onNotFound(handle_NotFound);                      // handle 404 error
    server.begin();

    // for captive portal
    dnsServer.start(DNS_PORT, "*", wifiap_get_local_ip());
}

void webserver_handle_event(void)
{
    dnsServer.processNextRequest();
    server.handleClient();
}

void handle_OnHome()
{
    Serial.printf("Web server Home page request\n");
    server.send(200, "text/html", SendHTML());
}

void handle_Onsetroot()
{
    String root = server.pathArg(0);
    int newroot = atoi(root.c_str());
    flower_music_set_root(newroot);
    flower_music_reset ();

    server.send(200, "text/html", SendHTML());
}

void handle_Onsetscale()
{
    String scales = server.pathArg(0);
    int newscale = atoi(scales.c_str());
    flower_music_set_scale(newscale);
    flower_music_reset ();

    server.send(200, "text/html", SendHTML());
}
void handle_Onsettempo()
{
    String bpms = server.pathArg(0);
    int bpm = atoi(bpms.c_str());
    flower_music_set_basebpm(bpm);
    
    

    server.send(200, "text/html", SendHTML());
}

void setMul(uint8_t track, uint32_t mul)
{
    std::vector<CSequence *> sequences = sequencer.get_tracks();
    if (track < sequences.size ())
    {
        //flower_music_reset ();
        sequences[track]->setBpmMultiplier (mul);
    }
}
void handle_Onsetmul1()
{
    String muls = server.pathArg(0);
    uint32_t mul = atoi(muls.c_str());
    Serial.printf ("set mul 1 =%lu\n", mul);
    setMul (0, mul);
    
    server.send(200, "text/html", SendHTML());
}

void handle_Onsetmul2()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    Serial.printf ("set mul 2 =%lu\n", mul);
    setMul (1, mul);
    
    server.send(200, "text/html", SendHTML());
}

void handle_Onsetmul3()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    Serial.printf ("set mul 3 =%lu\n", mul);

    setMul (2, mul);
    
    server.send(200, "text/html", SendHTML());
}

void handle_Onsetmul4()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    Serial.printf ("set mul 4 =%lu\n", mul);
    setMul (3, mul);
    
    server.send(200, "text/html", SendHTML());
}

void handle_NotFound()
{
    server.send(404, "text/plain", "Hapyness Not found !!! :-(");
}

String SendHTML(void)
{
    Serial.printf ("page request\n");

    String ptr = "<!DOCTYPE html> <html>\n";
    ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr += "<title>MIDI Flower Control</title>\n";
    ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr += ".button {display: block;width: 120px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 20px;margin: 0px auto 35px;cursor: pointer;border-radius: 8px;}\n";
    ptr += ".button-on {background-color: #3498db;}\n";
    ptr += ".button-on:active {background-color: #2980b9;}\n";
    ptr += ".button-off {background-color: #34d098;}\n";
    ptr += ".button-off:active {background-color: #2c3e50;}\n";
    ptr += "p {font-size: 14px;color: #888;margin-bottom: 12px;}\n";
    ptr += "</style>\n";
    ptr += "</head>\n";
    ptr += "<body>\n";
    ptr += "<h1>MIDI Flower Control</h1>\n";

    ptr += "<h1>Octaves</h1>\n";

    ptr += "<h1>Gamme</h1>\n";
    int scalenbr = flower_music_get_scale_name_nbr();
    char **scalenames = flower_music_get_scale_name();
    int cur_scale = flower_music_get_scale ();

    for (uint8_t n = 0; n < scalenbr; n++)
    {
        Serial.printf ("Scale name %s\n", scalenames[n]);
        if (n == cur_scale)
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += scalenames[n];
            ptr += "</a>\n";                 
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/setscale=";
            ptr += std::to_string(n).c_str();
            ptr += "\">Utiliser ";
            ptr += scalenames[n];
            ptr += "</a>\n";
        }
    }
    ptr += "<h1>BPM</h1>\n";
    uint16_t bpm = flower_music_get_basebpm();
    for (uint8_t n = 0; n < ARRAYLEN(miditempo); n++)
    {
        if (bpm == miditempo[n])
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += std::to_string(bpm).c_str();
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/settempo=";
            ptr += std::to_string(miditempo[n]).c_str();
            ptr += "\">Utiliser ";
            ptr += std::to_string(miditempo[n]).c_str();
            ptr += "</a>\n";
        }
    }

    ptr += "<h1>Midi Canal 1</h1>\n";
    uint32_t multi =sequencer.get_tracks()[0]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = std::to_string (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/setmul1=";
            ptr += std::to_string(notemul[n]).c_str();
            ptr += "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }
    ptr += "<h1>Midi Canal 2</h1>\n";
    multi =sequencer.get_tracks()[1]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = std::to_string (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/setmul2=";
            ptr += std::to_string(notemul[n]).c_str();
            ptr += "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }

    ptr += "<h1>Midi Canal 3</h1>\n";
    multi =sequencer.get_tracks()[2]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = std::to_string (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/setmul3=";
            ptr += std::to_string(notemul[n]).c_str();
            ptr += "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }
    ptr += "<h1>Midi Canal 4</h1>\n";
    multi =sequencer.get_tracks()[3]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = std::to_string(notemul[n]).c_str();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-on\" href=\"/setmul4=";
            ptr += std::to_string(notemul[n]).c_str();
            ptr += "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }
    ptr += "<h1>Note Fondamentale</h1>\n";

    int root = flower_music_get_root();
    for (uint8_t n = 0; n < ARRAYLEN(midinotes); n++)
    {

        if (n == root)
        {
            // ptr +="<p>";
            // ptr +="</p>";
            ptr += "<a class=\"button button-off\" href=\"/\">";
            ptr += midinotes[n];
            ptr += "</a>\n";
        }
        else
        {
            // ptr +="<p>";
            // ptr +="</p>";
            ptr += "<a class=\"button button-on\" href=\"/setroot=";
            ptr += std::to_string(n).c_str();
            ptr += "\">Utiliser ";
            ptr += midinotes[n];
            ptr += "</a>\n";
        }
    }

#if 0
   if(led1stat)
  {}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}
#endif
    ptr += "</body>\n";
    ptr += "</html>\n";
    return ptr;
}