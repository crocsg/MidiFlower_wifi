#ifndef __MIDINOTE_H
#define __MIDINOTE_H



typedef struct _MIDImessage { //build structure for Note and Control MIDImessages
  unsigned int type;
  int value;
  int velocity;
  long duration;
  long period;
  int channel;
  unsigned long time;
} MIDImessage;

#endif
