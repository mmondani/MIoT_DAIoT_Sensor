/*===================[File header]===============================================*/
// File:                main.cpp
// Licence:             GPLV3+
// Version:             2.0.0
// Date:                Diciembre 2020
// Info:                Trabajo práctico Desarrollo de Aplicaciones para IoT
// Autor:               Ing. Marcelo Castello
// Grupo:               Arancibia Pablo, Castello Marcelo, Montes de Oca Federico
//
// TODO:    
//        
//
// BUGS:    
//                   
//
// COMMIT:  
//
// Para subir FileSystem: platformio run --target uploadfs

// Versión adaptada de un trabajo de
// Rui Santos
// Complete project details at https://RandomNerdTutorials.com/esp32-mqtt-publish-dht11-dht22-arduino/
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files.
//  
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.





/*===================[Inclusiones]===============================================*/
#include "DHT.h"
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <ESP32WebServer.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "backend.h"
#include "publica.h"
#include "wifi.h"
#include "mqtt.h"
#include "eeprom.h"
#include "timestamp.h"
#include "mediciones.h"


/*===================[Definiciones de hardware]==================================*/
// Digital pin connected to the DHT sensor
#define DHTPIN                        26  
//#define DHTTYPE DHT11               DHT 11
#define DHTTYPE                       DHT22     // DHT 22(AM2302), AM2321
//#define DHTTYPE                     DHT21     // DHT 21 (AM2301)   
#define LED_BUILTIN                   2
#define FRESET                        25        // pin para factory reset
#define CANAL1                        27        // pin salida canal 1
#define CANAL2                        23        // pin salida canal 2
#define LED_PULSO                     2         // led de pulso

/*===================[Definiciones de software]==================================*/
#define SERIAL_BAUDRATE               115200
#define PORT_WEB_SERVER               80
#define FORMAT_SPIFFS_IF_FAILED       true
#define MAX_BYTE_EEPROM               512

/*===================[Objetos instanciados]======================================*/
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
TimerHandle_t freqLedTimer;
DHT dht(DHTPIN, DHTTYPE);
ESP32WebServer web_server(PORT_WEB_SERVER);

/*===================[Posicion de variables EEPROM]==============================*/
uint32_t device_eeprom_pos        =   0;
uint32_t mqtt_server_eeprom_pos   =   30;
uint32_t mqtt_tcp_str_eeprom_pos  =   60;
uint32_t passwd_AP_eeprom_pos     =   90;
uint32_t ssid_eeprom_pos          =   120;
uint32_t ssid_passwd_eeprom_pos   =   150;
uint32_t ubicacion_eeprom_pos     =   180;
uint32_t sensor_eeprom_pos        =   210;
uint32_t canal1_eeprom_pos        =   240;
uint32_t canal2_eeprom_pos        =   270;
uint32_t fuota_version_eeprom_pos =   300;
uint32_t flag_update_eeprom_pos   =   500;

/*===================[Variables de Factory reset]================================*/
String hardware                   =   "ESP32";
String device                     =   "DAIoT";               //usado como template
String mqtt_server                =   "192.168.1.42";
String mqtt_tcp_str               =   "1883";
String ssid                       =   "MAC_E";
String ssid_pass                  =   "wpaadministrator08";
String passwd_AP                  =   "Lancast3r";
String ubicacion                  =   "Sala Servidores";
uint8_t sensor                    =   1;
uint8_t canal1_status             =   0;
uint8_t canal2_status             =   0;

/*===================[Variables harcodeadas]=====================================*/
const char* ntpServer             =   "pool.ntp.org";
String www_username               =   "admin";
String topic_telem                =   "device/telemetry";
String topic_act                  =   "device/command";
String topic_resp                 =   "device/action";
String tipo_device                =   "Termohigrometro";
//-- Versiones
String fversion                   =   "1.0.0";          
String hversion                   =   "1.0.0";

/*===================[Variables globales]========================================*/
unsigned long previousSampleMillis      = 0;            // Stores last time temperature was published
const long sampleInterval               = 1000;         // Interval at which to publish sensor readings
uint8_t averageInterval                 = 60;           // Cantidad de mediciones para promediar
float tempi;
float humi;
float tempp;
float hump;
uint16_t mqtt_tcp;
uint8_t led_pulso=LED_PULSO;
uint8_t canal1_pin=CANAL1;
uint8_t canal2_pin=CANAL2;
uint8_t freset_pin=FRESET;
uint8_t pin_dht=DHTPIN;
uint8_t tipo_dht=DHTTYPE;


void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  EEPROM.begin(MAX_BYTE_EEPROM);

  //--Inicializacion de los pines
  pinMode(led_pulso,OUTPUT);
  pinMode(canal1_pin,OUTPUT);
  pinMode(canal2_pin,OUTPUT);
  pinMode(freset_pin,INPUT);

  //--Bienvenida
  for(int i=0;i<10;i++){
    digitalWrite(LED_PULSO,HIGH);
    delay(50);
    digitalWrite(LED_PULSO,LOW);
    delay(50);
  }

  factory_reset();
  read_vars(1); 
  restore_canales();
  check_update();

  dht.begin();

  //--Timers de RTOS
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  connectToWifi();
  WiFi.onEvent(WiFiEvent);
  

  //--Callbacks de la conexión MQTT
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(mqtt_server.c_str(), mqtt_tcp_str.toInt());


  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");

  //connectToWifi();

  //--Montaje de SPIFFS
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("Falla de montaje de SPIFFS");
        return;
    }
  //--Inicialización del web server embebido
  init_webserver();
  
  //--Start del NTP
  configTime(0, 0, ntpServer);
}

void loop() {
  web_server.handleClient();
  unsigned long currentSampleMillis = millis();
  
  //--Intervalo de muestreo
  if (currentSampleMillis - previousSampleMillis >= sampleInterval) {
    previousSampleMillis = currentSampleMillis;
    //--Lee sensor DHT 
    medicion();
  }
}