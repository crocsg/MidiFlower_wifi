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

#include "MidiFlowerSequencer.h"
#include "webserver_config.h"
#include "flower_music.h"

#define MIN_TIME_MS_BETWEEN_NOTE   (20)

void CMidiFlowerSequencer::Loop(void)
{
    uint32_t currentMillis = millis(); // manage time

    if (currentMillis - m_sequence_time >= MIN_TIME_MS_BETWEEN_NOTE)
    {
        //Serial.printf ("check notenbr=%d seq=%d\n", m_ptracks[m_sequence_index]->getnbnotes(), m_sequence_index);
        MIDImessage note;
        m_sequence_time = currentMillis;
        if ((m_sequence_index > 0 && m_ptracks[m_sequence_index]->getnbnotes() > m_ptracks[m_sequence_index]->size() / 8) ||
            m_sequence_index == 0)
        {
            if (m_ptracks[m_sequence_index]->play(currentMillis, &note) == 1)
            {
                //Serial.printf ("play seq=%d milli=%lu\n", m_sequence_index, currentMillis);
                note.value = scaleNote(note.value, flower_music_get_current_scale(), flower_music_get_current_root ());
                Play(currentMillis, &note);
            }
        }
        m_sequence_index = (m_sequence_index + 1) % m_ptracks.size();
    }
    else
    {
        Control(currentMillis);
        webserver_handle_event();
    }
    m_previousMillis = currentMillis; // manage time
}
