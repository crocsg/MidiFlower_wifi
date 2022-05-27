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

#include "sequence.h"


CSequence::CSequence (size_t maxsize, uint32_t bpm, uint32_t bpmmulti, uint32_t noteratio)
{
    
    m_tempo = 0;
    m_seq.resize (maxsize);
    m_noteratio = noteratio;
    m_size = maxsize;
    
    m_lastplay = 0;
    m_cntnote = 0;
    m_bpm_multi = bpmmulti;
    m_bpm = bpm;
    m_tempo = (60 * 1000) /  (m_bpm * m_bpm_multi);
    UpdateTempo ();
}

void CSequence::addNote (uint32_t time, uint8_t value, uint8_t velocity, uint16_t duration, uint16_t ramp, uint8_t notechannel)
{
    //Serial.printf("addNote vel=%d tempo =%d", velocity, m_tempo );    
    if (velocity == 0 || m_tempo == 0)
        return;
    MIDImessage mes;
    mes.type = 0;
    mes.value = value;
    mes.velocity = velocity;
    mes.duration = duration;
    mes.channel = notechannel;
    mes.ramp = ramp;

    size_t pos = (time / m_tempo) % m_size;
    if (m_seq[pos].velocity == 0)
        m_cntnote++;
    m_seq[pos] = mes;
    //Serial.printf("addNote pos=%d time =%lu size=%d, ch=%d, nbnote=%d tempo=%ld\n", pos, time, m_size, notechannel, m_cntnote, m_tempo);
    
    // delete a note if sequence is full
    if (m_cntnote > (m_size * m_noteratio ) / 100)
    {
        for (auto it = m_seq.begin () + pos + 1; it != m_seq.end (); it++)
        {
            if (it->velocity > 0)
            {
                it->velocity = 0;
                m_cntnote--;
            }
        }
    }
}

uint8_t CSequence::play (uint32_t time, MIDImessage* mes)
{
    if (m_tempo == 0)
        return (0);
    if (time - m_lastplay < m_tempo)
        return (0);

    m_lastplay = time;    
    return play_seq (time, mes);
    
}

uint8_t CSequence::play_seq (uint32_t time, MIDImessage* mes)
{
    if (m_tempo == 0)
        return (0);
    size_t pos = (time / m_tempo) % m_size;
    
    if (m_seq[pos].velocity != 0)
    {
        //Serial.printf ("node pos=%d, ch=%d, val=%d, vel=%d\n", pos, m_seq[pos].channel, m_seq[pos].value, m_seq[pos].velocity);
        *mes = m_seq[pos];
        return (1);
    }
    return (0);
}

void CSequence::clear (void)
{
    for (auto it = m_seq.begin () ; it != m_seq.end (); it++)
        it->velocity = 0;
    m_cntnote = 0;
}

void CSequence::mute (int vel)
{
    for (auto it = m_seq.begin () ; it != m_seq.end (); it++)
        if (it->velocity > vel)
            it->velocity = (it->velocity * vel) / 100;
    
}

void CSequence::UpdateNbNote (void)
{
    m_cntnote = 0;
    for (auto it = m_seq.begin () ; it != m_seq.end () && it != m_seq.begin () + m_size; it++)
    {
        if (it->velocity > 0)
            m_cntnote++;
    }
}

