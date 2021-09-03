#include "publica.h"

//--Objetos externos
extern WiFiClientSecure espClient;
extern PubSubClient mqttClient;

//--Variables externas
extern String topic_telem;
extern String topic_led;
extern String topic_act;
extern String topic_resp;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern String ssid;
extern String fversion;
extern String hversion;
extern String device;
extern float tempp;
extern float hump;
extern uint8_t sensor;
extern uint8_t flag_publica;
extern uint8_t flag_rango;
extern uint8_t flag_pub_rango;


//--Variables locales
StaticJsonDocument<200> publica;
char json_pub[200];
uint8_t flag_push=0;
uint8_t flag_push_att=1;
uint16_t packetIdPub;

void send_pub(int elemento){
    //--Verica si hay conexion wifi y mqtt
    if (WiFi.status() != WL_CONNECTED){
        return;
    }
    if (!mqttClient.connected()){
        return;
    }
    serializeJson(publica, json_pub);
    switch (elemento){
        case TELEMETRIA:
            Serial.print("Publica Telemetria:");
            Serial.println(json_pub);
            packetIdPub = mqttClient.publish(topic_telem.c_str(),json_pub);                                                    
            //Serial.printf("Topico %s con QoS 1, packetId: %i\n", topic_telem.c_str(), packetIdPub);
            //Serial.printf("Mensaje: T=%.2f H=%.2f\n", tempp, hump);
            break;
        case ACTUACION:
            Serial.print("Publica Actuacion:");
            Serial.println(json_pub);
            packetIdPub = mqttClient.publish(topic_resp.c_str(), json_pub);
            //Serial.printf("Topico %s con QoS 1, packetId: %i\n", topic_resp.c_str(), packetIdPub);
            break;
    }
}

void publica_medicion(void){
    //-- Payload de medicion {"Device":"DAIoT01","Valores":{"ts":1629934560,"Temperatura":20,"Humedad":51}}
    publica.clear();
    publica["Device"] = device;
    JsonObject val = publica.createNestedObject("Valores");
    val["ts"] = getTime();
    val["Temperatura"] = tempp;
    val["Humedad"] = hump;
    serializeJson(publica, json_pub);
    send_pub(TELEMETRIA);
    
}

void publica_canales(uint8_t canal){
    //-- Payload de respuesta de actuación {"Device":"DAIoT01","Valores":{"Canal":1,"Estado":1}}
    publica.clear();
    publica["Device"]=device;
    JsonObject val = publica.createNestedObject("Valores");
    val["Canal"]=canal;
    
    if (canal==1){
        val["Estado"]=canal1_status;
    }
    if (canal==2){
        val["Estado"]=canal2_status;
    }    
    serializeJson(publica, json_pub);
    send_pub(ACTUACION);
}

void publica_resp_rpc(String comando){
    //-- Payload de respuesta {"Device":"DAIoT01","Comando":"version","Valores":{"Version firmware":"1.0.0","Version hardware":"1.0.0"}}
    publica.clear();
    publica["Device"] = device;
    publica["Comando"]=comando;
    JsonObject val = publica.createNestedObject("Valores");
    if (comando="version"){
        val["Version firmware"]=fversion;
        val["Version hardware"]=hversion;
    }
    serializeJson(publica, json_pub);
    send_pub(ACTUACION);
}