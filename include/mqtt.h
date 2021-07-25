#ifndef _mqtt_h_
#define _mqtt_h_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>                      //Librería para TLS
#include <PubSubClient.h>
//#include <LittleFS.h>
#include "FS.h"
#include "SPIFFS.h"
#include "wifi.h"
#include "comandos.h"

/*======================================[macros y definiciones]============================================*/
#define WILL_RETAIN_MESSAGE                 false
#define WILL_QOS                            2
//#define MAX_MQTT_CONNECTION_TIMEOUT         50          //timeout en milisegundos*200 para la conexion mqtt
#define MAX_MQTT_CONNECTION_RETRYS          3             //cantidad de reintentos de conexion mqtt
#define MAX_MQTT_LIVE_TIMEOUT               10            //timeout en minutos de desaparicion de mqtt (en loop)->RESET
#define MAX_MQTT_WAIT_STATE                 3             //minutos de espera de reintento de conexion al broker
#define VERY_SHORT_DELAY_MQTT_LED           25
#define SHORT_DELAY_MQTT_LED                50
#define LONG_DELAY_MQTT_LED                 150
#define VERY_LONG_DELAY_MQTT_LED            300
#define MQTT_DISCONNECTED_STATE             0             //valores para _flag_mqtt
#define MQTT_CONNECTED_STATE                1
#define MQTT_MAX_RETRY_STATE                2
#define MQTT_WAIT_STATE                     3
#define MAX_COUNT_DELAY_LED                 10



class MCC_mqtt{
    private:
        //int mqtt_conn_timeout=0;//sacar                    //contador timeout de la conexion a mqtt en segundos
        const char * _server;
        uint16_t _port_server;
        const char * _device;
        const char * _topic_status;
        const char * _will_mess;
        uint8_t _flag_mqtt;                         
        bool _conn_mqtt_status;
        const char * _topic_sub;
        int _led;
        uint8_t _mqtt_conn_retrys;
        const char* _ca_cert;
        uint8_t _flag_start_timeout_live_mqtt=0;
        uint8_t _countDelayLed=0;
        void _setLed(void);
        void _pubBlinkMqttLed();

    public:
        bool conn(void);
        void init(const char * server, uint16_t port_server, const char * device, const char * topic_status, const char * will_mess, const char * topic_sub, int led, const char * ca_cert);
        bool pub(String topic, String message);
        void on_msg(char* topic, byte* payload, unsigned int length);
        bool sub(String topic);
        bool loop(void);
        bool control(void);
        void setOnMess(std::function<void (char *, uint8_t *, unsigned int)> callb);
        

        uint8_t mqtt_wait;                    //contador de estado de espera de reintento de conexion mqtt en minutos
        uint8_t live_timeout_mqtt;            //contador timeout de la conexion en funcionamiento de mqtt en minutos
        
};















/*
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
void onMqttPublish(uint16_t packetId);*/



#endif
