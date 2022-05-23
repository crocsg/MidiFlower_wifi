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

#include "flower_music.h"
#include "flower_sensor.h"
#include "sequence.h"
#include "util.h"

uint32_t last_sample_check = millis ();


// definition des tempos
#define BPM60 ((60 * 1000) / 60)
#define BPM90 ((60 * 1000) / 90)
#define BPM120 ((60 * 1000) / 120)
#define BPM160 ((60 * 1000) / 160)
#define BPM BPM120

#define BASE_BPM 120

// nombre maxi de notes midi simultanees dans le sequencer
// max midi notes tracked in sequencer
#define MAX_MIDI_NOTES 128

// sequenceur midi
// midi sequencer
CMidiFlowerSequencer sequencer = CMidiFlowerSequencer(MAX_MIDI_NOTES);

// definition des "pistes" midi du tempo associé et du taux de remplissage max
// define tracks with associated BPM and fill ratio
CSequence midi_track_1 = CSequence(60, BASE_BPM, 1, 75);
CSequence midi_track_2 = CSequence(120,BASE_BPM, 1, 75);
CSequence midi_track_3 = CSequence(120, BASE_BPM, 4, 75);
CSequence midi_track_4 = CSequence(120, BASE_BPM, 1, 35);

//******************************
// define scale for music style
uint8_t scaleMajor[FLOWER_MUSIC_SCALE_SIZE]  =       {0,0 ,2,2, 4,4, 5,5, 7,7, 9, 11};        
uint8_t scaleMinor[FLOWER_MUSIC_SCALE_SIZE]  =       {0,0, 2,2, 3,3, 5,5, 7,7, 8, 10};

uint8_t scaleMinorHarmo[FLOWER_MUSIC_SCALE_SIZE] =   {0,0, 2,2, 3,3, 5,5, 7,7, 8, 11};
uint8_t scaleMinorMelo[FLOWER_MUSIC_SCALE_SIZE] =    {0,0, 2,2, 3,3, 5,5, 7,7, 9, 11};
uint8_t scalePentaMajor[FLOWER_MUSIC_SCALE_SIZE] =   {0,0, 2,2, 4,4, 7,7,7, 9,9,9};
uint8_t scalePentaMinor[FLOWER_MUSIC_SCALE_SIZE] =   {0,0, 3,3, 5,5, 7,7,7, 10,10,10};
uint8_t scaleBlues[FLOWER_MUSIC_SCALE_SIZE] =        {0,0, 3,3, 5,5, 6,6, 7,7, 10,10};

uint8_t scaleChrom[FLOWER_MUSIC_SCALE_SIZE] =        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};


const char* scalename[] = 
{
  "Major",
  "Minor",
  "Minor Harmonic",
  "Minor Melodic",
  "Pentatonic Major",
  "Pentatonic Minor",
  "Blues"
  "Chromatic"
};

uint8_t *scaleSelect = scaleMajor; //initialize scaling
uint8_t current_scale = 0;
int root = 4;       //initialize for root
uint8_t noteMin = 24;   //C1  - keyboard note minimum
uint8_t noteMax = 84;   //C6  - keyboard note maximum

uint32_t basebpm   = BASE_BPM;


void flower_music_init (void)
{
  // register midi track on sequencer
  sequencer.register_track(&midi_track_1);
  sequencer.register_track(&midi_track_2);
  sequencer.register_track(&midi_track_3);
  sequencer.register_track(&midi_track_4);
  
}

// Flower sensor measure callback. compute a note and pass it to midi sequencer
// min    min value
// max    max value
// averg  average
// delta  max - min
// stdevi standard deviation
// stdevical standard deviation * threshold
void BuildNoteFromMeasure (uint32_t currentmillis, uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical)
{
    int dur = 200+(map(delta%127,1,127,100,2000));      //length of note derived from delta
    int ramp = 3 + (dur%80) ; //control slide rate, min 25 (or 3 ;)
     

    //set scaling, root key, note
    int note = map(averg%127,0,127,noteMin,noteMax);    //derive note from average measure
    //note = scaleNote(note, scaleSelect, root);          //scale the note (force the note in selected scale)
    
    setNote(currentmillis, note, 100, dur);                            // add the note in some sequencer channels
}




int scaleNote(int note, uint8_t scale[], int root) 
{
  
  int scaled = note % FLOWER_MUSIC_SCALE_SIZE;
  int octave = note / FLOWER_MUSIC_SCALE_SIZE;
  scaled = scaleSelect[scaled];
  scaled = (scaled + (FLOWER_MUSIC_SCALE_SIZE * octave)) + root; //apply octave and root
  
  return scaled;
}

void setNote(uint32_t currentMillis, int value, int velocity, long duration)
{
    //Serial.printf ("add note time =%lu\n", currentMillis);
  // if first track is not 33% full add the note to it
  if (sequencer.get_track_nbnote(0) < sequencer.get_track_size (0) / 3)
    sequencer.addNote(0, currentMillis, value, velocity, duration, 1);
  // if second track is not full at 33% add the note to it
  else if (sequencer.get_track_nbnote(1) < sequencer.get_track_size (1) / 3)
    sequencer.addNote(1, currentMillis, value, velocity, duration, 2);
  else
  {
    // find a track and add the note
    uint16_t seq = currentMillis % sequencer.get_nbtracks ();
    
    sequencer.addNote(seq, currentMillis, value, velocity, duration, seq + 1);
  }
}

void ControlMusic (void)
{
  // fade off music when the is no measure
  uint32_t currentMillis =  millis ();  // get time in ms

  if (currentMillis - last_sample_check >= 5000)
  {
    uint32_t last_samples = flower_sensor_get_last_sample_time_ms ();   // get last mesure time
    std::vector<CSequence *>& psequences = sequencer.get_tracks ();     // get tracks

    if (currentMillis - last_samples > 15000)
    {
        //no measure for 15s 
        
        psequences[2]->mute (80);   // track3 volume at 80%
        psequences[3]->clear ();    // clear track 4
    }
    if (currentMillis - last_samples > 30000)
    {
        //no measure for 30s 
        psequences[0]->mute (75);   // track1 volume at 75%
        psequences[1]->clear ();    // clear track
        psequences[2]->clear ();    // clear track
        psequences[3]->clear ();    // clear track
    }
    if (currentMillis - last_samples > 60000)
    {
        //no measure for 60s 
        psequences[0]->clear ();    // clear track
    }

    last_sample_check = currentMillis;
  }
}

int flower_music_get_root (void)
{
  return (root);
}

void    flower_music_set_root (int n)
{
  root = n;
}
void    flower_music_reset ()
{
  std::vector<CSequence *>& psequences = sequencer.get_tracks ();     // get tracks
  for (auto it = psequences.begin(); it != psequences.end (); ++it)   // walk on tracks
    (*it)->clear ();                                                  // clear track
  last_sample_check = millis ();  

}

uint32_t     flower_music_get_basebpm (void)
{
  return basebpm;
}
void    flower_music_set_basebpm (uint32_t bpm)
{
  basebpm = bpm;

  std::vector<CSequence *>& psequences = sequencer.get_tracks ();     // get tracks
  for (auto it = psequences.begin(); it != psequences.end (); ++it)
  {
    (*it)->setBpm (bpm);
  }
}


uint8_t flower_music_get_scale (void)
{
  return current_scale;
}



void flower_music_set_scale (uint8_t scale)
{
  current_scale = scale;
 
  switch (scale)
  {
    case 0:
      scaleSelect = scaleMajor;
      break;
    case 1:
      scaleSelect = scaleMinor;
      break;      
    case 2:
      scaleSelect = scaleMinorHarmo;
      break;      
    case 3:
      scaleSelect = scaleMinorMelo;
      break;            
    case 4:
      scaleSelect = scalePentaMajor;
      break;     
    case 5:
      scaleSelect = scalePentaMinor;
      break;       
    case 6:
      scaleSelect = scaleBlues;
      break;         
    case 7:
      scaleSelect = scaleChrom;
      break;        
    default:
      scaleSelect = scaleMajor;
      break;           

  }
}

const char** flower_music_get_scale_name (void)
{
  return scalename;
}

uint8_t flower_music_get_scale_name_nbr (void)
{
  return ARRAYLEN(scalename);
}

uint8_t* flower_music_get_current_scale (void)
{
  return scaleSelect;
}

int flower_music_get_current_root (void)
{
  return (root);
}