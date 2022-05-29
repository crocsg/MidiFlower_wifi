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
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <uri/UriBraces.h>

#include "webserver_config.h"
#include "flower_music.h"
#include "wifiap.h"
#include "util.h"
#include "config.h"

#define HTTP_PORT 80
#define DNS_PORT 53

WebServer server(HTTP_PORT);
DNSServer dnsServer;


#define MIDI_OCTAVE_NOTE    12
#define MAX_OCTAVE          5
#define MIN_OCTAVE_NOTE     24

// get the sequencer from flower_music
extern CMidiFlowerSequencer sequencer;  


static void handle_OnHome();
static void handle_Onsetroot();
static void handle_Onsettempo();
static void handle_Onsetscale();
static void handle_Onsetmul1();
static void handle_Onsetmul2();
static void handle_Onsetmul3();
static void handle_Onsetmul4();
static void handle_OnSetsize();
static void handle_OnSetRatio();
static void handle_NotFound();
static void handle_OntestScale();
static void handle_OnPlay();
static void handle_OnConfigChannel();
static void handle_OnsetMin();

static String HomePage(void);
static String TestPage (void);
static String ChannelPage (void);

static const char *midioctave[] =
{
    "C1",
    "C2",
    "C3",
};

static const char *midinotes[] =
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

static const uint16_t seqsize[] = 
{
    8,
    16,
    32,
    64,
    128
};

static const uint16_t seqratio[] = 
{
    25,
    50,
    75,
    100,
    
};

static const uint16_t miditempo[] =
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

static const uint16_t notemul[] =
{
        0,1,2,4

};

void webserver_config_init(void)
{
    server.on("/", handle_OnHome);                           // home page
    server.on(UriBraces("/setroot={}"), handle_Onsetroot);   // set root page
    server.on(UriBraces("/settempo={}"), handle_Onsettempo); // set root page
    server.on(UriBraces("/setscale={}"), handle_Onsetscale); // set scale page
    server.on(UriBraces("/setmin={}"), handle_OnsetMin);    // set Note min

    server.on(UriBraces("/setmul1={}"), handle_Onsetmul1);   // set time multiplier
    server.on(UriBraces("/setmul2={}"), handle_Onsetmul2);   // set time multiplier
    server.on(UriBraces("/setmul3={}"), handle_Onsetmul3);   // set time multiplier
    server.on(UriBraces("/setmul4={}"), handle_Onsetmul4);   // set time multiplier

    server.on("/chann", handle_OnConfigChannel);   // display config channel page
    server.on(UriBraces("/setsize={}/{}"), handle_OnSetsize);   // sequence size
    server.on(UriBraces("/setratio={}/{}"), handle_OnSetRatio);   // sequence size



    server.on("/testscale", handle_OntestScale);   // display testScale page

    
    server.on(UriBraces("/play={}"), handle_OnPlay);   // display testScale page

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

static void handle_OnHome()
{
    sequencer.setLock(false);
    //Serial.printf("Web server Home page request\n");
    server.send(200, "text/html", HomePage());
}

static void handle_OntestScale()
{
    sequencer.setLock(true);
    //Serial.printf("Web server Test scale page request\n");
    server.send(200, "text/html", TestPage());
}

static void handle_OnConfigChannel()
{
    sequencer.setLock(false);
    //Serial.printf("Web server Config channel page request\n");
    server.send(200, "text/html", ChannelPage());
}

static void handle_OnPlay()
{
    String snote = server.pathArg(0);
    uint8_t note = (uint8_t) atoi(snote.c_str());
    
    note = note + flower_music_get_note_min(); // note derived from configurated octave
    //flower_music_reset ();
    sequencer.midiSerial(144, 1, note, 100);

    server.send(200, "text/html", TestPage());
}


static void handle_Onsetroot()
{
    String root = server.pathArg(0);
    int newroot = atoi(root.c_str());
    flower_music_set_root(newroot);
    //flower_music_reset ();
    config_save ();
    server.send(200, "text/html", HomePage());
}

static void handle_Onsetscale()
{
    String scales = server.pathArg(0);
    int newscale = atoi(scales.c_str());
    flower_music_set_scale(newscale);
    config_save ();

    server.send(200, "text/html", HomePage());
}

static void handle_OnsetMin()
{
    String mins = server.pathArg(0);
    int newmin = atoi(mins.c_str());
    flower_music_set_note_min((newmin * MIDI_OCTAVE_NOTE) + MIN_OCTAVE_NOTE);
    flower_music_set_note_max(flower_music_get_note_min () + (MAX_OCTAVE * MIDI_OCTAVE_NOTE));

    config_save ();

    server.send(200, "text/html", HomePage());
}

static void handle_Onsettempo()
{
    String bpms = server.pathArg(0);
    int bpm = atoi(bpms.c_str());
    flower_music_set_basebpm(bpm);
    config_save ();
    

    server.send(200, "text/html", HomePage());
}

static void setMul(uint8_t track, uint32_t mul)
{
    std::vector<CSequence *> sequences = sequencer.get_tracks();
    if (sequences.size () > track)
    {
        //flower_music_reset ();
        sequences[track]->setBpmMultiplier (mul);
        config_save ();
    }
    
}

static void setSize(uint8_t track, uint32_t size)
{
    std::vector<CSequence *> sequences = sequencer.get_tracks();
    if (sequences.size () > track)
    {
        //Serial.printf ("set size chan %d =%lu\n", track, size);
        sequences[track]->setSize (size);
        //Serial.printf ("set size chan %d =%lu\n", track, sequences[track]->size());
        config_save ();
    }
    
}

static void setRatio(uint8_t track, uint32_t ratio)
{
    std::vector<CSequence *> sequences = sequencer.get_tracks();
    if (sequences.size () > track)
    {
        //Serial.printf ("Track %d ratio %d\n", track, ratio);
        sequences[track]->setRatio (ratio);
        //Serial.printf ("Track %d ratio %d\n", track, sequences[track]->getRatio ());        
    }
    config_save ();
}

static void handle_Onsetmul1()
{
    String muls = server.pathArg(0);
    uint32_t mul = atoi(muls.c_str());
    //Serial.printf ("set mul 1 =%lu\n", mul);
    setMul (0, mul);
    
    server.send(200, "text/html", ChannelPage());
}

static void handle_Onsetmul2()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    //Serial.printf ("set mul 2 =%lu\n", mul);
    setMul (1, mul);
    
    server.send(200, "text/html", ChannelPage());
}

static void handle_Onsetmul3()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    //Serial.printf ("set mul 3 =%lu\n", mul);

    setMul (2, mul);
    
    server.send(200, "text/html", ChannelPage());
}

static void handle_Onsetmul4()
{
    String muls = server.pathArg(0);
    int mul = atoi(muls.c_str());
    
    //Serial.printf ("set mul 4 =%lu\n", mul);
    setMul (3, mul);
    
    server.send(200, "text/html", ChannelPage());
}

static void handle_OnSetsize()
{
    String chans = server.pathArg(0);
    String sizes = server.pathArg(1);
    uint8_t chan = atoi(chans.c_str());
    uint32_t size = atoi(sizes.c_str());
        
    setSize (chan, size);
    
    server.send(200, "text/html", ChannelPage());
}

static void handle_OnSetRatio()
{
    String chans = server.pathArg(0);
    String ratios = server.pathArg(1);
    uint8_t chan = atoi(chans.c_str());
    uint32_t ratio = atoi(ratios.c_str());
    
    
    setRatio (chan, ratio);
    

    server.send(200, "text/html", ChannelPage());
}
void handle_NotFound()
{
    server.send(404, "text/plain", "Hapyness Not found !!! :-(");
}

static String GetHTMLHeader (void)
{
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr += "<title>MIDI Flower Control</title>\n";
    ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr += ".button {display: block;width: 120px;background-color: #3498db;border: none;color: white;padding: 9px 15px;text-decoration: none;font-size: 20px;margin: 0px auto 35px;cursor: pointer;border-radius: 8px;}\n";
    ptr += ".button-on {background-color: #3498db;}\n";
    ptr += ".button-on:active {background-color: #2980b9;}\n";
    ptr += ".button-off {background-color: #34387b;}\n";
    ptr += ".button-off:active {background-color: #2c3e50;}\n";
    ptr += ".button-link {background-color: #349834;border-radius: 24px;}\n";
    ptr += "p {font-size: 14px;color: #888;margin-bottom: 12px;text-decoration: underline}\n";
    ptr += "</style>\n";
    ptr += "</head>\n";
    ptr += "<body>\n";

    return ptr;
}

static String GetNavigationMenu ()
{
    String ptr = "";
    ptr += "<a class=\"button button-link\" href=\"/testscale\">Scale Test</a>";
    ptr += "<a class=\"button button-link\" href=\"/chann\">Channel Config</a>";

    return ptr;
}

static String HomePage(void)
{
    Serial.printf ("page request\n");

    String ptr = GetHTMLHeader ();
    ptr += "<h1>MIDI Flower Control</h1>\n";

    ptr += "<h1 id=\"scale\">Scale</h1>\n";
    int scalenbr = flower_music_get_scale_name_nbr();
    const char **scalenames = flower_music_get_scale_name();
    int cur_scale = flower_music_get_scale ();

    for (uint8_t n = 0; n < scalenbr; n++)
    {
        Serial.printf ("Scale name %s\n", scalenames[n]);
        if (n == cur_scale)
        {
            ptr += "<a class=\"button button-on\" href=\"#scale\">";
            ptr += scalenames[n];
            ptr += "</a>\n";                 
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setscale=";
            ptr += String(n);
            ptr += "\">";
            ptr += scalenames[n];
            ptr += "</a>\n";
        }
    }
    ptr += "<h1 id=\"bpm\">BPM</h1>\n";
    uint16_t bpm = flower_music_get_basebpm();
    for (uint8_t n = 0; n < ARRAYLEN(miditempo); n++)
    {
        if (bpm == miditempo[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#bpm\">";
            ptr += String(bpm);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/settempo=";
            ptr += String(miditempo[n]);
            ptr += "\">";
            ptr += String(miditempo[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h1 id=\"nmin\">Note Min</h1>\n";
    int vmin = ((flower_music_get_note_min () - MIN_OCTAVE_NOTE) / MIDI_OCTAVE_NOTE) % ARRAYLEN(midioctave);
     for (uint8_t n = 0; n < ARRAYLEN(midioctave); n++)
    {
        if (vmin == n)
        {
            ptr += "<a class=\"button button-on\" href=\"#nmin\">";
            ptr += midioctave[n];
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setmin=";
            ptr += String(n);
            ptr += "\">";
            ptr += midioctave[n];
            ptr += "</a>\n";
        }
    }

    ptr += "<h1 id=\"tone\">Tone</h1>\n";

    int root = flower_music_get_root();
    for (uint8_t n = 0; n < ARRAYLEN(midinotes); n++)
    {

        if (n == root)
        {
            // ptr +="<p>";
            // ptr +="</p>";
            ptr += "<a class=\"button button-on\" href=\"#tone\">";
            ptr += midinotes[n];
            ptr += "</a>\n";
        }
        else
        {
            // ptr +="<p>";
            // ptr +="</p>";
            ptr += "<a class=\"button button-off\" href=\"/setroot=";
            ptr += String(n);
            ptr += "\">";
            ptr += midinotes[n];
            ptr += "</a>\n";
        }
    }

    // menu
    ptr +="<hr/>";
    ptr += GetNavigationMenu ();

    ptr += "</body>\n";
    ptr += "</html>\n";
    return ptr;
}

static String ChannelPage (void)
{
    String ptr = GetHTMLHeader ();

    ptr += "<h1>Channels</h1>\n";
       
     ptr += "<h1>Midi Channel 1</h1>\n";
     ptr += "<h2 id=\"tp1\">Tempo</h2>\n";
    uint32_t multi =sequencer.get_tracks()[0]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = String (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-on\" href=\"#tp1\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setmul1=";
            ptr += String(notemul[n]);
            ptr += n == 0 ? "\">" : "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"sl1\">Sequence length</h2>\n";
    uint32_t size =sequencer.get_tracks()[0]->size ();
    for (uint8_t n = 0; n < ARRAYLEN(seqsize); n++)
    {
            
         if (size == seqsize[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#sl1\">";
            ptr += String(size);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setsize=0/";
            ptr += String(seqsize[n]);
            ptr += "\">";
            ptr += String(seqsize[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"ra1\">Fill Ratio</h2>\n";
    uint32_t ratio =sequencer.get_tracks()[0]->getRatio();
    for (uint8_t n = 0; n < ARRAYLEN(seqratio); n++)
    {
            
         if (ratio == seqratio[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#ra1\">";
            ptr += String(ratio);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setratio=0/";
            ptr += String(seqratio[n]);
            ptr += "\">";
            ptr += String(seqratio[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h1>Midi Channel 2</h1>\n";
    ptr += "<h2 id=\"tp2\">Tempo</h2>\n";
    multi =sequencer.get_tracks()[1]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = String (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-on\" href=\"#tp2\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setmul2=";
            ptr += String(notemul[n]);
            
            ptr += n == 0 ? "\">" : "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"sl2\">Sequence length</h2>\n";
    size =sequencer.get_tracks()[1]->size ();
    for (uint8_t n = 0; n < ARRAYLEN(seqsize); n++)
    {
            
         if (size == seqsize[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#sl2\">";
            ptr += String(size);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setsize=1/";
            ptr += String(seqsize[n]);
            ptr += "\">";
            ptr += String(seqsize[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"ra2\">Fill Ratio</h2>\n";
    ratio =sequencer.get_tracks()[1]->getRatio();
    for (uint8_t n = 0; n < ARRAYLEN(seqratio); n++)
    {
            
         if (ratio == seqratio[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#ra2\">";
            ptr += String(ratio);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setratio=1/";
            ptr += String(seqratio[n]);
            ptr += "\">";
            ptr += String(seqratio[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h1>Midi Channel 3</h1>\n";
    ptr += "<h2 id=\"tp3\">Tempo</h2>\n";
    multi =sequencer.get_tracks()[2]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = String (notemul[n]).c_str ();

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-on\" href=\"#tp3\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setmul3=";
            ptr += String(notemul[n]);
            ptr += n == 0 ? "\">" : "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"sl3\">Sequence length</h2>\n";
    size =sequencer.get_tracks()[2]->size ();
    for (uint8_t n = 0; n < ARRAYLEN(seqsize); n++)
    {
            
         if (size == seqsize[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#sl3\">";
            ptr += String(size);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setsize=2/";
            ptr += String(seqsize[n]);
            ptr += "\">";
            ptr += String(seqsize[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"ra3\">Fill Ratio</h2>\n";
    ratio =sequencer.get_tracks()[2]->getRatio();
    for (uint8_t n = 0; n < ARRAYLEN(seqratio); n++)
    {
            
         if (ratio == seqratio[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#ra3\">";
            ptr += String(ratio);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setratio=2/";
            ptr += String(seqratio[n]);
            ptr += "\">";
            ptr += String(seqratio[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h1>Midi Channel 4</h1>\n";
    ptr += "<h2 id=\"tp4\">Tempo</h2>\n";
    multi =sequencer.get_tracks()[3]->getBpmMultiplier ();
    for (uint8_t n = 0; n < ARRAYLEN(notemul); n++)
    {
        String mul = "Off";
        if (n > 0)
            mul = String(notemul[n]);

        if (notemul[n] == multi)
        {
            ptr += "<a class=\"button button-on\" href=\"#tp4\">";
            ptr += mul;
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setmul4=";
            ptr += String(notemul[n]);
            ptr += n == 0 ? "\">" : "\">X ";
            ptr += mul;
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"sl4\">Sequence length</h2>\n";
    size =sequencer.get_tracks()[3]->size ();
    for (uint8_t n = 0; n < ARRAYLEN(seqsize); n++)
    {
            
         if (size == seqsize[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#sl4\">";
            ptr += String(size);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setsize=3/";
            ptr += String(seqsize[n]);
            ptr += "\">";
            ptr += String(seqsize[n]);
            ptr += "</a>\n";
        }
    }

    ptr += "<h2 id=\"ra4\">Fill Ratio</h2>\n";
    ratio =sequencer.get_tracks()[3]->getRatio();
    for (uint8_t n = 0; n < ARRAYLEN(seqratio); n++)
    {
            
         if (ratio == seqratio[n])
        {
            ptr += "<a class=\"button button-on\" href=\"#ra4\">";
            ptr += String(ratio);
            ptr += "</a>\n";
        }
        else
        {
            ptr += "<a class=\"button button-off\" href=\"/setratio=3/";
            ptr += String(seqratio[n]);
            ptr += "\">";
            ptr += String(seqratio[n]);
            ptr += "</a>\n";
        }
    }
    ptr +="<hr/>";

    // back to home page
    ptr += "<a class=\"button button-link\" href=\"/\">Home</a>";
    ptr += GetNavigationMenu ();

    ptr += "</body>\n";
    ptr += "</html>\n";

    return ptr;
}

String TestPage (void)
{
    String ptr = GetHTMLHeader ();
  

    ptr += "<h1>Test Scale</h1>\n";
    int scalenbr = flower_music_get_scale_name_nbr();
    const char **scalenames = flower_music_get_scale_name();
    int cur_scale = flower_music_get_scale ();
    uint8_t *cur_scaleptr = flower_music_get_current_scale ();
    int root = flower_music_get_current_root ();
    ptr += "<h2>Current Scale</h2>";
    ptr += "<h2>";
    ptr += scalenames[cur_scale];
    ptr += "</h2>";

    uint16_t note = 0xFFFF;
    for (uint8_t i = 0; i < ARRAYLEN(midinotes); i++)
    {
        if (note != cur_scaleptr[i])
        {
            note = cur_scaleptr[i];
            ptr += "<a class=\"button button-on\" href=\"/play=";
            ptr += String(note + root);
            ptr += "\">";
            ptr += midinotes[(note + root) % ARRAYLEN(midinotes)];
            ptr += "</a>\n";
        }
    }

    // back to home page
    ptr +="<hr/>";
    ptr += "<a class=\"button button-link\" href=\"/\">Home</a>";
    ptr += GetNavigationMenu ();

    ptr += "</body>\n";
    ptr += "</html>\n";

    return ptr;
}