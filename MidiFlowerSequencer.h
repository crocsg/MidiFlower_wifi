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

#ifndef __MIDIFLOWERSEQUENCER_H
#define __MIDIFLOWERSEQUENCER_H

#include "midisequencer.h"
#include "sequence.h"

class CMidiFlowerSequencer : public CMidiSequencer
{
public:
    CMidiFlowerSequencer (uint32_t size) : CMidiSequencer (size), m_sequence_index(0), m_sequence_time(0)
    {
        m_previousMillis = 0;
        m_sequence_time = 0;
        m_lock = false;
    }

    void Loop (void);

    void register_track (CSequence* pseq)
    {
        m_ptracks.push_back (pseq);
    }
    std::vector<CSequence *>& get_tracks ()
    {
        return m_ptracks;
    }

    size_t get_nbtracks ()
    {
        return m_ptracks.size();
    }
    size_t get_track_nbnote (uint8_t track)
    {
        if (track >= m_ptracks.size())
            return (0);
        return (m_ptracks[track]->getnbnotes ());
    }

    uint32_t get_track_mulbpm (uint8_t track)
    {
        if (track >= m_ptracks.size())
            return (0);
        return (m_ptracks[track]->getBpmMultiplier());            
    }

    void set_track_mulbpm (uint8_t track, uint32_t mul)
    {
        if (track >= m_ptracks.size())
            return;
        m_ptracks[track]->setBpmMultiplier(mul);            
    }

    size_t get_track_size (uint8_t track)
    {
        if (track >= m_ptracks.size())
            return (0);
        return (m_ptracks[track]->size());
    }

    void set_track_size (uint8_t track, size_t size)
    {
        if (track >= m_ptracks.size())
            return;
        m_ptracks[track]->setSize(size);
    }

    size_t get_track_maxnote (uint8_t track)
    {
        if (track >= m_ptracks.size())
            return (0);
        return ((m_ptracks[track]->size() * 100) /  m_ptracks[track]->getRatio());   
    }

    uint32_t get_track_ratio (uint8_t track)
    {
        if (track >= m_ptracks.size())
            return (0);
        return (m_ptracks[track]->getRatio());   
    }

    void set_track_ratio (uint8_t track, uint32_t ratio)
    {
        if (track >= m_ptracks.size())
            return;
        m_ptracks[track]->setRatio (ratio);            
    }

    void addNote (uint8_t track, uint32_t time, uint8_t value, uint8_t velocity, uint16_t duration, uint8_t notechannel)
    {
        if (track >= m_ptracks.size())
            return;
        //Serial.printf ("add note chan=%d time=%lu value=%d duration=%d \n", notechannel, time, value, duration);
        m_ptracks[track]->addNote (time, value, velocity, duration, notechannel);
    }

    void setLock (bool lock) { m_lock = lock;}
    
private:
    std::vector<CSequence *> m_ptracks;

    uint32_t m_sequence_time;
    uint16_t m_sequence_index;
    uint32_t m_previousMillis;
    bool     m_lock;
};
#endif // __MIDIFLOWERSEQUENCER_H