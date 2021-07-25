#ifndef _wifi_h_
#define _wifi_h_

#include <Arduino.h>

//--Necesario para usar las librerías anteriores y que se conecte a mqtt/TLS!!!!
//-- Info de: /home/mcastello/.platformio/packages/framework-arduinoespressif8266/libraries/ESP8266WiFi/src/WiFiClientSecure.h
//#define USING_AXTLS
#include <WiFi.h>
#include <WiFiClientSecure.h>
//#include "WiFiClientSecureAxTLS.h"
//using namespace axTLS;
//------------------------------------------------------------------------------

/*======================================[macros y definiciones]============================================*/
#define MAX_WIFI_CONN_TIMEOUT              20            //timeout en segundos para la conexion wifi
#define MAX_WIFI_LIVE_TIMEOUT              10            //timeout en minutos de desaparicion de wifi (en loop)->RESET
#define MAX_WIFI_WAIT_STATE                4             //espera en minutos para reconectarse a wifi después de MAX_WIFI_CONN_TIMEOUT
#define VERY_SHORT_DELAY_WIFI_LED          25
#define SHORT_DELAY_WIFI_LED               50      
#define WIFI_WAIT_STATE                    13            //valores para _flag_wifi
#define WIFI_RETRY_CONN_STATE              14

class MCC_wifi{
    private:    
        void _blinkLed(int code);
        void _setLed(int flag);

        String _device;
        String _passwd_AP;
        String _ssid;
        String _ssid_pass;
        int _led;
        uint8_t _flag_wifi=0;
        
    public:
        void init(String device,String passwd_AP, String ssid, String ssid_pass, int led);
        uint8_t conn(void);
        int control(void);

        uint8_t wifi_wait=0;
        uint8_t live_timeout_wifi=0;            //contador timeout de la conexion wifi en minutos (en loop)->RESET
        uint8_t wifi_conn_timeout=0;            //contador timeout de la conexion wifi en segundos
};

#endif

/*
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

#endif*/