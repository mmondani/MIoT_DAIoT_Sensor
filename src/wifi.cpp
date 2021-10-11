#include "wifi.h"
#include "backend.h"
#include "mqtt.h"


//--Variables externas
extern String ssid;
extern String ssid_pass;
extern String device;
extern String passwd_AP;

/*===================[FSM WIFI]========================================*/
typedef enum {
  wifi_fsm_init = 0,
  wifi_fsm_connecting,
  wifi_fsm_waiting,
  wifi_fsm_connected
} wifi_fsm_state_t;
static wifi_fsm_state_t fsmState = wifi_fsm_init;
static bool stateIn = true, stateOut = false;
static bool newSsidOrPassword = false;
static unsigned long previousSampleMillis      = 0;
static unsigned long currentSampleMillis;

static void gotoState (wifi_fsm_state_t nextState);
/*====================================================================*/


void wifi_connect(void) {
  /*
  Serial.print("Conectando a Wi-Fi..");
  WiFi.begin(ssid.c_str(), ssid_pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.softAP(device.c_str(),passwd_AP.c_str()) ? "AP Ready" : "AP Fallo!");
  */
}

void wifi_handler(void) {
  switch (fsmState)
  {
    case wifi_fsm_init:
      if (stateIn) {
        stateIn = false;

      }
      /***********************************************************************************/
      Serial.println("Iniciando AP: ");
      Serial.println(WiFi.softAP(device.c_str(),passwd_AP.c_str()) ? "AP Ready" : "AP Fallo!");
      Serial.println("AP IP address: ");
      Serial.print(WiFi.softAPIP());

      //--Inicialización del web server embebido
      webserver_init();

      gotoState(wifi_fsm_connecting);
      /***********************************************************************************/
      if (stateOut) {
        stateOut = false;
        stateIn = true;
      }
      break;

    case wifi_fsm_connecting:
      if (stateIn) {
        stateIn = false;

        Serial.println("\nConectando a Wi-Fi..");
        WiFi.begin(ssid.c_str(), ssid_pass.c_str());

        previousSampleMillis = millis();
      }
      /***********************************************************************************/
      if (WiFi.status() == WL_CONNECTED) {
        gotoState(wifi_fsm_connected);
      }
      else if (newSsidOrPassword) {
        newSsidOrPassword = false;

        Serial.println("Se configuraron nuevos SSID y password");
        gotoState(wifi_fsm_connecting);
      }
      else {
        // Si en 5 segundos no se conectó a WiFi se va
        currentSampleMillis = millis();
        if (currentSampleMillis - previousSampleMillis >= 5*1000) {
          previousSampleMillis = currentSampleMillis;

          gotoState(wifi_fsm_waiting);
        }
      }
      /***********************************************************************************/
      if (stateOut) {
        stateOut = false;
        stateIn = true;
      }
      break;

    case wifi_fsm_waiting:
      if (stateIn) {
        stateIn = false;

        Serial.println("Esperando Wi-Fi..");

        previousSampleMillis = millis();
      }
      /***********************************************************************************/

      // En 5 minutos vuelve a probar
      currentSampleMillis = millis();
      if (currentSampleMillis - previousSampleMillis >= 5*60*1000) {
        previousSampleMillis = currentSampleMillis;

        gotoState(wifi_fsm_connecting);
      }
      else if (newSsidOrPassword) {
        newSsidOrPassword = false;

        Serial.println("Se configuraron nuevos SSID y password");
        gotoState(wifi_fsm_connecting);
      }
      /***********************************************************************************/
      if (stateOut) {
        stateOut = false;
        stateIn = true;
      }
      break;

    case wifi_fsm_connected:
      if (stateIn) {
        stateIn = false;

        Serial.println("Conectado");
        Serial.println("IP address: ");
        Serial.print(WiFi.localIP());

        mqtt_connect();
      }
      /***********************************************************************************/
      if (WiFi.status() != WL_CONNECTED) {
        gotoState(wifi_fsm_connected);
      }
      else if (newSsidOrPassword) {
        newSsidOrPassword = false;

        WiFi.disconnect();

        Serial.println("Se configuraron nuevos SSID y password");
        gotoState(wifi_fsm_connecting);
      }
      /***********************************************************************************/
      if (stateOut) {
        stateOut = false;
        stateIn = true;
      }
      break;
    
    default:
      break;
  }
}

/*Esta función genera un evento cuando el SSID o el password han sido modificados*/

void wifi_newSsidOrPasswordEvent (void) {
  newSsidOrPassword = true;
}


void gotoState (wifi_fsm_state_t nextState) {
  fsmState = nextState;

  stateOut = true;
}