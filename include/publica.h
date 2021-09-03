#ifndef _publica_h_
#define _publica_h_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "timestamp.h"
#include "mqtt.h"


/*======================================[macros y definiciones]============================================*/
//#define INTERV_PUB                  600                 //Intervalo de publicacion en segundos
#define TELEMETRIA                    1
#define ACTUACION                     2
#define STATUS                        3

void send_pub(int topico);
void publica_medicion(void);
void publica_canales(uint8_t canal);
void publica_resp_rpc(String comando);
void publica_status(bool status);



#endif