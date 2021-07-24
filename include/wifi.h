#ifndef _wifi_h_
#define _wifi_h_

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <Arduino.h>
#include "WiFi.h"
#include "mqtt.h"

//--Prototipos
void connectToWifi(void);
void WiFiEvent(WiFiEvent_t event);

#endif