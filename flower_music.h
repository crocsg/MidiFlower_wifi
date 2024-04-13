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


#ifndef __FLOWER_MUSIC_H
#define __FLOWER_MUSIC_H

#include "MidiFlowerSequencer.h"
#include "sequence.h"

#define FLOWER_MUSIC_SCALE_SIZE 12

void            BuildNoteFromMeasure (uint32_t currentmillis, uint32_t min, uint32_t max, uint32_t averg, uint32_t delta, float stdevi, float stdevical);
uint16_t        scaleNote(uint16_t note, uint8_t scale[], uint16_t root);
void            setNote(uint32_t currentMillis, int value, int velocity, int duration, int ramp);
void            ControlMusic (void);
void            flower_music_init (void);
int             flower_music_get_root (void);
void            flower_music_set_root (int root);
uint8_t         flower_music_get_loop(void);
uint32_t        flower_music_get_basebpm (void);
void            flower_music_set_basebpm (uint32_t bpm);
void            flower_music_reset (void);
uint8_t         flower_music_get_scale (void);
void            flower_music_set_scale (uint8_t scale);
const char**    flower_music_get_scale_name (void);
uint8_t         flower_music_get_scale_name_nbr (void);
uint8_t*        flower_music_get_current_scale (void);
int             flower_music_get_current_root (void);
void            flower_music_set_note_max (int maxv);
void            flower_music_set_note_min (int minv);
int             flower_music_get_note_min (void);
int             flower_music_get_note_max (void);

#endif
