#ifndef _mqtt_h_
#define _mqtt_h_


#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "FS.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include "actuacion.h"
#include "comandos.h"

//--Prototipos
void mqtt_init(void);
void receivedCallback(char* topic, byte* payload, unsigned int len);
void mqtt_connect(void);




#endif
