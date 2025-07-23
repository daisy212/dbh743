#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "db_hardware_def".h"

extern const uint8_t DM_keyb[];

void KeyboardInit(void);
void TASK_KBD(void);


#endif // KEYBOARD_H
