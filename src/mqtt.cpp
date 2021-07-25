
#include "mqtt.h"


//--Objetos externos
extern WiFiClientSecure espClient;
extern PubSubClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;
extern String mqtt_server;
extern String mqtt_tcp_str;

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
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDtzCCAp+gAwIBAgIUNpIuXcx4wH/lIFjmHQz5Gv6Qj7kwDQYJKoZIhvcNAQEL\n" \
"BQAwazELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAGA1UEBwwJ\n" \
"U3RvY2tob2xtMRAwDgYDVQQKDAdoaW1pbmRzMQswCQYDVQQLDAJDQTEVMBMGA1UE\n" \
"AwwMMTkyLjE2OC4xLjQyMB4XDTIxMDcyMzE5NDIyNVoXDTIyMDcyMzE5NDIyNVow\n" \
"azELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAGA1UEBwwJU3Rv\n" \
"Y2tob2xtMRAwDgYDVQQKDAdoaW1pbmRzMQswCQYDVQQLDAJDQTEVMBMGA1UEAwwM\n" \
"MTkyLjE2OC4xLjQyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxJFB\n" \
"83pPIfIw3YqPEuSa6NacikeoAdgdNGpaBQ2uzxQ7IMWdxsmoHTG+uGG4ML/07FHo\n" \
"UAsX9qH6dzgKK/BfMrsDir2KexHBWNa1+QzNsK9+iRXLbtwGGX5GIfU5tbbzmE4d\n" \
"O+nlcYdZpEgvrOwEuQHXLWlu4tYEP3PxWM+PBIU2LPjnVuh9jHjUY6KPTCtRQrIF\n" \
"eWm/4xpLx48r0Wtdxgb2kIYSaVdiULMYUWNROpqbZKNC+dRAqf6qqsfem6wimgQP\n" \
"Wn0AmRaAe7QKJAm9DXVN8G5s5I+ch0w6xbuqGl3TZc1EXO9Z6NQdfO1ArhdLatnH\n" \
"KWd/QSmkVz1aZxNx4wIDAQABo1MwUTAdBgNVHQ4EFgQUMj4aZN8Ds3YScQUm77cq\n" \
"++z6QIkwHwYDVR0jBBgwFoAUMj4aZN8Ds3YScQUm77cq++z6QIkwDwYDVR0TAQH/\n" \
"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEABgAaPU7Vyw5ruT/eaklpVwH/ckQ+\n" \
"TSTQnb5nvLEcO358WmoPfQ90SjGOgSOHZeivRtYyLhpPwDAOhcpjW651gTCvX5Bp\n" \
"Kptwus4LBOVoQUxon2X/G3/tSWedySnGx6EbC1sgX5XU/vAb3+8yffC9w+hJcsIX\n" \
"b0T99VbiIt2rCkM45/UodEEw2msN7WSiGg36jjS5l6ERAAit1QZIoNZctW/6zdyc\n" \
"RwGVVjVUEvX64LjthDVJwqxjG+pNzn4YwTnI6bzA2+zGZjA6aKiDc0GcmLLoPRS0\n" \
"szRulfxnlHXN76PImjcO3iBOGYFxbtRmk6w2FEcdEj8erCDWO7c1a1FONw==\n" \
"-----END CERTIFICATE-----\n";


void mqtt_init(void){
  espClient.setCACert(ca_cert);
  mqttClient.setServer(mqtt_server.c_str(), mqtt_tcp_str.toInt());//mqttClient.setServer("192.168.1.42", 8883);
  mqttClient.setCallback(receivedCallback);

}

void mqtt_connect() {
  /* Loop until reconnected */
  while (!mqttClient.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    //String clientId = "mterbtheca01";
    /* connect now */
    if (mqttClient.connect("DAIoT01")){
      Serial.println("connected");
      /* subscribe topic */
      mqttClient.subscribe(topic_act.c_str());
    } else {
      Serial.print("failed, status code =");
      Serial.print(mqttClient.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Publish received.");
  payload[length] = '\0';
  topic_rec = String((char*)topic);
  msg_rec = String((char*)payload);
  Serial.print("Topic: ");
  Serial.println(topic_rec);
  Serial.print("Payload: ");
  Serial.println(msg_rec);

  StaticJsonDocument<200> parse_payload;
  DeserializationError error = deserializeJson(parse_payload, msg_rec);
  if (error) {
    Serial.print("[DEBUG] deserializeJson() fallo: ");
    Serial.println(error.f_str());
    return;
  }

  //Modelo de entrada: {"Device":"DAIoT01","Command":"set", "Parameter":1} 
  dev_rec=parse_payload["Device"];
  command=parse_payload["Command"];
  param=parse_payload["Parameter"];

  //--Debug del parseo de payload entrante
  //Serial.println("Comando:");
  //Serial.println(dev_rec);
  //Serial.println(command);
  //Serial.println(param);

  //--Si el comando es para este dispositivo, procesa
  if ((String)dev_rec==device){
    command_proc(command, param);
  }
}