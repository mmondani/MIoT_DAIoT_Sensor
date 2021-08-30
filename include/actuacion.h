#ifndef _actuacion_h_
#define _actuacion_h_
#include <Arduino.h>
#include "publica.h"
#include "eeprom.h"


void canal1_on(void);
void canal1_off(void);
void canal2_on(void);
void canal2_off(void);
void led_pulse(void);
void restore_canales(void);
void wr_eeprom_canal(uint8_t canal);




#endif