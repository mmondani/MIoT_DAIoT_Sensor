
#include "mqtt.h"

//--Instancias
//extern  WiFiClient wifi_client;              //Sin TLS
WiFiClientSecure wifi_client;           //Con TLS  
PubSubClient mqtt(wifi_client);


/**
 * @brief Inicializa la clase: setea el servidor mqtt y el puerto
 * 
 * @param server ip del broker
 * @param port_server puerto tcp del broker
 * @param device nombre del dispositivo (se usa como id de sesión mqtt)
 * @param topic_status topic del status de la conexión
 * @param will_mess mensaje de desconexión
 * @param topic_sub 1er topico de subscripcion
 * @param led led de estado mqtt
 */
void MCC_mqtt::init(const char * server, uint16_t port_server, const char * device, const char * topic_status, const char * will_mess, const char * topic_sub, int led, const char * ca_cert){
    this->_server=server;
    this->_port_server=port_server;
    this->_device=device;
    this->_topic_status=topic_status;
    this->_will_mess=will_mess;
    this->_topic_sub=topic_sub;
    this->_led=led;
    this->_ca_cert=ca_cert;
    //LittleFS.begin();
    /*File cert = SPIFFS.open("/mqttserver.der", "r");
    if (!cert) {
        Serial.println("Fallo apertura del certificado");
    }
    else{
        Serial.println("Certificado abierto");
    }
    delay(300);
    if (wifi_client.loadCertificate(cert))
        Serial.println("Certificado cargado");
    else
        Serial.println("Certificado NO cargado");*/

    mqtt.setCACert(ca_cert);
    mqtt.setServer(_server, _port_server);
}

/**
 * @brief Genera la función callback para la entrada de mensajes
 * 
 * @param callb 
 */
void MCC_mqtt::setOnMess(std::function<void (char *, uint8_t *, unsigned int)> callb){
    mqtt.setCallback(callb);
}

/**
 * @brief Conecta al broker. Si el tiempo de conexión supera MAX_MQTT_CONNECTION_TIMEOUT (definido en mqtt.h),
 *        sale.
 * 
 * @return bool 
 *        Devuelve el resultado de la conexión: true conexion exitosa, false no se pudo realizar la conexión.
 */
bool MCC_mqtt::conn(void){
    _flag_mqtt=MQTT_DISCONNECTED_STATE;
    //mqtt_conn_timeout=0;//sacar
    while (!mqtt.connected()) {
        if (_mqtt_conn_retrys==0){
            Serial.print("Intentando conexión a Broker MQTT (");Serial.print(_mqtt_conn_retrys+1);Serial.print(") ");
        }else{
            Serial.print("Reintento conexión a Broker MQTT  (");Serial.print(_mqtt_conn_retrys+1);Serial.print(") ");
        }
        Serial.print(_server);
        Serial.print(":");
        Serial.print(_port_server);
        Serial.print(" - ");
        Serial.print(_device);
        Serial.println("...");
        //_conn_mqtt_status=mqtt.connect(_device,_topic_status,WILL_QOS, WILL_RETAIN_MESSAGE, _will_mess); 
        _conn_mqtt_status=mqtt.connect(_device,_device,""); 
        _mqtt_conn_retrys++;
        this->_setLed();
        //delay(100);
        if (_conn_mqtt_status) {
            _flag_mqtt=MQTT_CONNECTED_STATE;
            _mqtt_conn_retrys=0;
            _flag_start_timeout_live_mqtt=0;
            Serial.println("Conectado a MQTT");
            this->sub(_topic_sub);
            this->_setLed();
            return true;
        }
        //delay(300);
        if (_mqtt_conn_retrys>=MAX_MQTT_CONNECTION_RETRYS){
            _flag_mqtt=MQTT_MAX_RETRY_STATE;
            _mqtt_conn_retrys=0;
            Serial.print("MAX Retrys. Fallo reiterado de conexióm MQTT, rc=");
            Serial.println(mqtt.state());
            this->_setLed();
            return false;
        }
    }
    _flag_mqtt=MQTT_CONNECTED_STATE;
    this->_setLed();
    return true;
}

/**
 * @brief Subscribe a un tópico. Si no está conectado, va a conexión
 * 
 * @param topic 
 * 
 * @return bool 
 *        Devuelve el resultado de la suscripción: true exitosa, false no se suscribió
 */
bool MCC_mqtt::sub(String topic){
    if (!mqtt.connected()){
        if (this->conn()){
            mqtt.subscribe(topic.c_str());
            this->_setLed();
            return true;
        }else{
            Serial.println("No se pudo suscribir al topico");
            this->_setLed();
            return false;
        }
    }
    mqtt.subscribe(topic.c_str());
    return true;
}

/**
 * @brief Loop principal del mqtt. Si no está conectado, va a conexión
 * 
 * @return bool
 *        Devuelve resultado del loop: true se pudo hacer, false no se pudo hacer
 */
bool MCC_mqtt::loop(void){
    if (!mqtt.connected()){
        if (_flag_mqtt==MQTT_MAX_RETRY_STATE){
            _flag_mqtt=MQTT_WAIT_STATE;
            mqtt_wait=0;
            this->_setLed();
            return false;
        }
        if (_flag_mqtt==MQTT_WAIT_STATE){
            if (mqtt_wait>=MAX_MQTT_WAIT_STATE){
                _flag_mqtt=MQTT_DISCONNECTED_STATE;
                this->_setLed();
                return false;
            }
            else{
                this->_setLed();
                return false;
            }
        }
        if (this->conn()){
            mqtt.loop();
            this->_setLed();
            return true;
        }else{
            Serial.println("No se pudo hacer broker loop");
            this->_setLed();
            return false;
        }
    }
    mqtt.loop();
    this->_setLed();
    return true;
}


/**
 * @brief Publica un mensaje en el broker. Si no está conectado, va a conexión
 * 
 * @param topic 
 * @param message 
 */
bool MCC_mqtt::pub(String topic, String message){
    if (!mqtt.connected()){
        if (this->conn()){
            if (mqtt.publish(topic.c_str(),message.c_str())){
                this->_pubBlinkMqttLed();
                return true;
            }
            else{
                Serial.println("Falló la publicación");
                return false;
            }
        }
        Serial.println("Falló la publicación. No se pudo conectar al broker");
        return false;
    }
    else{
        if (mqtt.publish(topic.c_str(),message.c_str())){
            this->_pubBlinkMqttLed();
            return true;
        }else{
            Serial.println("Falló la publicación");
           return false;
        }
    }
}

/**
 * @brief Controla la conexión al broker. Si la desconexión perdura por un 
 *        tiempo superior a MAX_MQTT_LIVE_TIMEOUT (definido en mqtt.h), 
 *        resetea el microcontrolador.
 * 
 * @return int 
 *        Devuelve el estado de la conexión: 1 conectado, 0 no conectado
 */
bool MCC_mqtt::control(void){
    if (!mqtt.connected()){  
        if (_flag_start_timeout_live_mqtt==0){
            live_timeout_mqtt=0;
            _flag_start_timeout_live_mqtt=1;
        }      
        if (live_timeout_mqtt >= MAX_MQTT_LIVE_TIMEOUT) {   
            Serial.println("########################");
            Serial.println("Reset por MQTT Timeout");
            Serial.println("########################");
            ESP.reset(); 
        }
        else{
            return false;
        }
    }
    //live_timeout_mqtt=millis();
    return true;
}

void MCC_mqtt::_pubBlinkMqttLed(void){
    for (int i=0;i<3;i++){
        digitalWrite(_led, HIGH);
        delay(VERY_SHORT_DELAY_MQTT_LED);
        digitalWrite(_led, LOW);
        delay(VERY_SHORT_DELAY_MQTT_LED);
    }

}

void MCC_mqtt::_setLed(void){
    //Serial.println(_flag_mqtt);
    int i;
    switch(_flag_mqtt){
        case MQTT_CONNECTED_STATE:
            //digitalWrite(_led, !digitalRead(_led));
            _countDelayLed++;
            if (_countDelayLed>MAX_COUNT_DELAY_LED){
                _countDelayLed=0;
                digitalWrite(_led,HIGH);
                delay(VERY_SHORT_DELAY_MQTT_LED);
                digitalWrite(_led,LOW);
            }
            //flag_seg == 0 ? digitalWrite(_led,HIGH):digitalWrite(_led,LOW);//digitalWrite(LED, !digitalRead(LED));
            break;
        case MQTT_MAX_RETRY_STATE:
            digitalWrite(_led, !digitalRead(_led));
            delay(SHORT_DELAY_MQTT_LED);
            break;       
        case MQTT_DISCONNECTED_STATE:
            digitalWrite(_led, LOW);
            delay(LONG_DELAY_MQTT_LED);
            for(i=0;i<_mqtt_conn_retrys+1;i++){
                digitalWrite(_led, HIGH);
                delay(LONG_DELAY_MQTT_LED);
                digitalWrite(_led, LOW);
                delay(LONG_DELAY_MQTT_LED);
            }
            break;
        case MQTT_WAIT_STATE:
            for (i=0;i<3;i++){
                digitalWrite(_led, HIGH);
                delay(VERY_SHORT_DELAY_MQTT_LED);
                digitalWrite(_led, LOW);
                delay(VERY_SHORT_DELAY_MQTT_LED);
            }
            break;  
    }

}


/*
//--Objetos externos
extern AsyncMqttClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;

//--Variables externas
extern String topic_act;
extern String device;

//--Variables locales
String topic_rec;
String msg_rec;
const char* command;
const char* param;
const char* dev_rec;
uint8_t canal_rec;
uint8_t accion_rec;


void connectToMqtt(void) {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(topic_act.c_str(), 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub,packetIdSub);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  String str_payload = String(payload).substring(0, len);
  Serial.println(str_payload);



  //--parseo de la payload (msg_rec)
  payload[len] = '\0';
  topic_rec = String((char*)topic);
  msg_rec = String((char*)payload);
  StaticJsonDocument<200> parse_payload;
  DeserializationError error = deserializeJson(parse_payload, msg_rec);
    if (error) {
        Serial.print("[DEBUG] deserializeJson() fallo: ");
        Serial.println(error.f_str());
        return;
    }
  //Modelo de entrada: {"Device":"DAIoT01","Actuacion":{"Canal":1,"Accion":"0n"}}
  //{"Device":"DAIoT01","Command":set, "Parameter":1}


  dev_rec=parse_payload["Device"];
  command=parse_payload["Command"];
  param=parse_payload["Parameter"];

  //canal_rec=parse_payload["Actuacion"]["Canal"];
  //accion_rec=parse_payload["Actuacion"]["Accion"];

  //--Debug del parseo de payload entrante
  //Serial.println("Comando:");
  //Serial.println(dev_rec);
  //Serial.println(canal_rec);
  //Serial.println(accion_rec);

  //--Si el comando es para este dispositivo, procesa
  if ((String)dev_rec==device){
    command_proc(command, param);
  }

  //  Serial.print("  qos: ");
  //  Serial.println(properties.qos);
  //  Serial.print("  dup: ");
  //  Serial.println(properties.dup);
  //  Serial.print("  retain: ");
  //  Serial.println(properties.retain);
  //  Serial.print("  len: ");
  //  Serial.println(len);
  //  Serial.print("  index: ");
  //  Serial.println(index);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}*/