#include "wifi.h"

//--Objetos externos
extern TimerHandle_t wifiReconnectTimer;
extern TimerHandle_t mqttReconnectTimer;


//--Variables externas
extern String ssid;
extern String ssid_pass;
extern String device;
extern String passwd_AP;

void connectToWifi(void) {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), ssid_pass.c_str());
  Serial.println(WiFi.softAP(device.c_str(),passwd_AP.c_str()) ? "AP Ready" : "AP Failed!");
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}