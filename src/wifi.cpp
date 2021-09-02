#include "wifi.h"


//--Variables externas
extern String ssid;
extern String ssid_pass;
extern String device;
extern String passwd_AP;

void wifi_connect(void) {
  Serial.print("Conectando a Wi-Fi..");
  WiFi.begin(ssid.c_str(), ssid_pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //TODO: Reiniciar despues de varios intentos 
  }
  Serial.println("Conectado");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.softAP(device.c_str(),passwd_AP.c_str()) ? "AP Ready" : "AP Fallo!");
}