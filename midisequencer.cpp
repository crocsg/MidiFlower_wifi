/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 S Godin (Climate Change Lab)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "midisequencer.h"

CMidiSequencer::CMidiSequencer (uint32_t size)
{
    m_playingnotes.resize (size);
    
}

void CMidiSequencer::Play (uint32_t time, MIDImessage* midi)
{
    
    midiSerial(144, midi->channel, midi->value, midi->velocity);

    // find a room to store midi message
    for (auto pMidi = m_playingnotes.begin(); pMidi != m_playingnotes.end (); pMidi++)
    {
        if (pMidi->velocity == 0)
        {
            *pMidi = *midi;
            pMidi->time = time;
            break;
        }
    }
}

void CMidiSequencer::Control (uint32_t time)
{
    for (auto pMidi = m_playingnotes.begin(); pMidi != m_playingnotes.end (); pMidi++)
    {
        if (time - pMidi->time > pMidi->duration && pMidi->velocity != 0)
        {
            midiSerial(128, pMidi->channel, pMidi->value, pMidi->ramp);
            pMidi->velocity = 0;
        }
    }
}

void CMidiSequencer::midiSerial(int type, int channel, int data1, int data2)
{
    //  Note type = 144 // noteon
    //  Control type = 176  // control change
    // remove MSBs on data
    data1 &= 0x7F;  //number
    data2 &= 0x7F;  //velocity
    
    byte statusbyte = (type | ((channel-1) & 0x0F));
    
    //Serial.write(statusbyte);
    //Serial.write(data1);
    //Serial.write(data2);
    switch (type)
    {
      case 128: // note off
        BLEMidiServer.noteOff(channel -1, data1, data2);
        break;
      case 144: // note on
        BLEMidiServer.noteOn(channel -1, data1, data2);
      break;
      case 176: // change note
        BLEMidiServer.controlChange(channel-1, data1, data2);
      break;
    }
}
