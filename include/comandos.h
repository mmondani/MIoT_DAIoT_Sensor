#ifndef _comandos_h_
#define _comandos_h_


#include <Arduino.h>
#include "actuacion.h"
#include "wifi.h"
#include <ESP32httpUpdate.h>


//--Prototipos
void command_proc(const char* command,const char* param);
void check_update(void);

#endif