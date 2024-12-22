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
#include <Arduino.h>
#include <BLEMidi.h>
#include <MIDI.h>
#include "board.h"
#include "midisequencer.h"


#if (MIDI_SERIAL_OUTPUT)
struct Serial2MIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = 31250;
  //static const int8_t RxPin  = PIN_RX_MIDI;
  //static const int8_t TxPin  = PIN_TX_MIDI;
};
#endif


static char bleserverid[64] = "";

#if (MIDI_SERIAL_OUTPUT)
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, SerialMidiServer, Serial2MIDISettings);  
#endif

CMidiSequencer::CMidiSequencer (uint32_t size)
{
    m_playingnotes.resize (size);
    
}

void CMidiSequencer::Init (uint32_t chipId)
{
  

  // start BLE Midi server
  #if (MIDI_BLE_OUTPUT)
  snprintf(bleserverid, sizeof(bleserverid), "BioData_%08lx MIDI device", chipId); // build BLE Midi name
  BLEMidiServer.begin(bleserverid);                          // initialize bluetooth midi
  
  #endif

  // start Midi Serial server
  #if (MIDI_SERIAL_OUTPUT)
  SerialMidiServer.begin(MIDI_CHANNEL_OMNI);
  Serial2.setPins (PIN_RX_MIDI, PIN_TX_MIDI);
  #endif
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
        
        #if (MIDI_BLE_OUTPUT)
        BLEMidiServer.noteOff(channel -1, data1, data2);
        #endif
        #if (MIDI_SERIAL_OUTPUT)
        SerialMidiServer.sendNoteOff(data1, data2, channel);
        #endif
        break;
      case 144: // note on
        
        #if (MIDI_BLE_OUTPUT)
        BLEMidiServer.noteOn(channel -1, data1, data2);
        #endif
        #if (MIDI_SERIAL_OUTPUT)
        SerialMidiServer.sendNoteOn(data1, data2, channel);
        #endif
      break;
      case 176: // change note
        #if (MIDI_BLE_OUTPUT)
        BLEMidiServer.controlChange(channel-1, data1, data2);
        #endif
        #if (MIDI_SERIAL_OUTPUT)
        SerialMidiServer.sendControlChange(data1, data2, channel);
        #endif
      break;
    }
}
