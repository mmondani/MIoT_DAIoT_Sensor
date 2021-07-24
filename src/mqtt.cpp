
#include "mqtt.h"



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
}