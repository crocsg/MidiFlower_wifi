#ifndef __MidiSequencer_H
#define __MidiSequencer_H

#include "Arduino.h"
#include <BLEMidi.h>
#include "midinote.h"
#include <vector>

class CMidiSequencer
{
public:
    CMidiSequencer (uint32_t size);
    
    void Play (uint32_t time, MIDImessage* midi);

    void Control (uint32_t time);
private:
    void midiSerial(int type, int channel, int data1, int data2);

    std::vector<MIDImessage> m_playingnotes;    
};
#endif
