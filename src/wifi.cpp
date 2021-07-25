#include "wifi.h"

//--Instancias
IPAddress ip_wlan;
IPAddress myIP;
//WiFiClient wifi_client;           //Sin TLS
//WiFiClientSecure wifi_client;       //Con TLS

/**
 * @brief Sólo inicializa las variables de la clase
 * 
 * @param device 
 * @param passwd_AP 
 * @param ssid 
 * @param ssid_pass 
 * @param led 
 */
void MCC_wifi::init(String device,String passwd_AP, String ssid, String ssid_pass, int led){
   this->_device=device;
   this->_passwd_AP=passwd_AP;
   this->_ssid=ssid;
   this->_ssid_pass=ssid_pass;
   this->_led=led;

   WiFi.mode(WIFI_AP_STA);
   WiFi.disconnect() ;
   WiFi.persistent(false);

   //--Access Point 
   Serial.println(WiFi.softAP(_device,_passwd_AP) ? "AP Ready" : "AP Failed!");
   myIP = WiFi.softAPIP();
   Serial.print("AP IP address: ");
   Serial.println(myIP);
   _flag_wifi=WIFI_RETRY_CONN_STATE;
}

/**
 * @brief Pone al micro en modo AP y WiFI. Conecta a la WiFi.
 *        Si el tiempo de conexión supera MAX_wifi_conn_timeout (definido en wifi.h),
 *        pasa a modo AP y sale.
 * 
 * @return int 
 *        Devuelve el resultado de la conexión: 1 conexion exitosa, 0 no se pudo realizar la conexión.
 */
uint8_t MCC_wifi::conn(void){
   if(_flag_wifi==WIFI_WAIT_STATE){
      _setLed(WIFI_WAIT_STATE);
      //Serial.println("periodo de espera de reconexion wifi");
      if (wifi_wait>=MAX_WIFI_WAIT_STATE){
         //Serial.println("FIN periodo de espera de reconexion wifi");
         _flag_wifi=WIFI_RETRY_CONN_STATE;
         _setLed(WIFI_RETRY_CONN_STATE);
      }
      if (live_timeout_wifi >= MAX_WIFI_LIVE_TIMEOUT) {
            Serial.println("########################");
            Serial.println("Reset por WIFI Timeout");
            Serial.println("########################");
            ESP.reset(); 
      }
      return false;
   }
   //--Conexion a la red WiFi 
   WiFi.begin(_ssid, _ssid_pass);
   Serial.print("Intentando conexión a WiFi: ");
   Serial.println(_ssid);
   while (WiFi.status() != 3) {
      Serial.print("[Estado] ");
      _setLed(WiFi.status());
      delay(200);
      if (wifi_conn_timeout>=MAX_WIFI_CONN_TIMEOUT){
         digitalWrite(_led, LOW);
         WiFi.mode(WIFI_AP);  //--Necesario para la configuración cuando no se conecta a wifi
         Serial.println("\r\nTimeout. Falló conexióm WiFi, pasa a modo AP");
         _flag_wifi=WIFI_WAIT_STATE;
         wifi_wait=0;
         live_timeout_wifi=0;
         return false;
         //break;
      }  
   }
   if(WiFi.status()==3){
      Serial.print("\r\nConectado con IP:");
      //digitalWrite(_led, HIGH);
      ip_wlan=WiFi.localIP();
      Serial.println(ip_wlan);
      digitalWrite(_led,HIGH);
      return true;
   }
   return false;
}


/**
 * @brief Controla la conexión a wifi. Si la desconexión perdura por un 
 *        tiempo superior a MAX_WIFI_LIVE_TIMEOUT (definido en wifi.h), 
 *        resetea el microcontrolador.
 * 
 * @return int 
 *        Devuelve 1 si la conexión existe, 0 si no existe.
 */
int MCC_wifi::control(void){
   if (WiFi.status()!=3){
      if(this->conn()){
         return true;
      }
      wifi_conn_timeout=0;
      return false;
   }
   return true;
}


/**
 * @brief Blink del led de wifi según el estado de la conexión
 * 
 */
void MCC_wifi::_setLed(int flag){
    //--Muestra el estado de WiFi
    switch(flag){
        case 0:
           Serial.println("WL_IDLE_STATUS Codigo:0");
           digitalWrite(_led, LOW);
           //delay(500);
           break;
        case 1:
           Serial.println("WL_NO_SSID_AVAIL Codigo:1");
           _blinkLed(1);
           //delay(500);
           break;
        case 2:
           Serial.println("WL_SCAN_COMPLETED Codigo:2");
           _blinkLed(2);
           break;
        case 3:
           // Todo bien WL_CONNECTED
           digitalWrite(_led, HIGH);
           break;
        case 4:
           Serial.println("WL_CONNECT_FAILED Codigo:4");
           _blinkLed(4);
           //delay(500);
           break;
        case 5:
           Serial.println("WL_CONNECTION_LOST Codigo:5");
           _blinkLed(5);
           //delay(500);
           break;
        case 6:
           Serial.println("WL_DISCONNECTED Codigo:6");
           _blinkLed(6);
           //delay(500);
           break;  
         case WIFI_WAIT_STATE:
            _blinkLed(WIFI_WAIT_STATE);
            break;
         case WIFI_RETRY_CONN_STATE:
            _blinkLed(WIFI_RETRY_CONN_STATE);
            break;
         default:
            break;    
    }  
}

/**
 * @brief Función interna para blink led de wifi
 * 
 * @param code 
 */
void MCC_wifi::_blinkLed(int code){
   int i;
   switch (code){
      case WIFI_WAIT_STATE:
         for (i=0;i<3;i++){
            digitalWrite(_led, HIGH);
            delay(VERY_SHORT_DELAY_WIFI_LED);
            digitalWrite(_led, LOW);
            delay(VERY_SHORT_DELAY_WIFI_LED);
         }
         break;
      case WIFI_RETRY_CONN_STATE:
         digitalWrite(_led, !digitalRead(_led));
         delay(SHORT_DELAY_WIFI_LED);
         break;
      default:
         for (i=0;i<code;i++){
            digitalWrite(_led, HIGH);
            delay(80);
            digitalWrite(_led, LOW);
            delay(250);
         }
         delay(500);
         break;
   }
}



/*#include "wifi.h"

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
}*/