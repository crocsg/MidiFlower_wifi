#ifndef __MIDINOTE_H
#define __MIDINOTE_H



typedef struct _MIDImessage { //build structure for Note and Control MIDImessages
  uint16_t type;
  uint16_t value;
  uint16_t velocity;
  uint16_t duration;
  uint16_t ramp;
  uint16_t period;
  uint16_t channel;
  uint32_t time;
} MIDImessage;

#endif
