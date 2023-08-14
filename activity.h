#ifndef __ACTIVITY_H
#define __ACTIVITY_H

void activity_begin (void);
void activity_event (uint32_t value);
void activity_add_history (uint8_t value);
void activity_show (void);
void activity_clear (void);
void activity_process (void);

#endif
