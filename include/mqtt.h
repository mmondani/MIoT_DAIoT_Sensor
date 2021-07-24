#ifndef _mqtt_h_
#define _mqtt_h_


#include <Arduino.h>
#include <AsyncMqttClient.h>
#include "WiFi.h"
#include "actuacion.h"
#include "comandos.h"

//--Prototipos
void connectToMqtt(void);
void onMqttConnect(bool sessionPresent);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttPublish(uint16_t packetId);



#endif
