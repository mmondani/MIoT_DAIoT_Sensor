
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
uint8_t TRYS_LOAD_FILES=3;

const char* ca_cert =\
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

const char* client_crt=\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDYTCCAkkCFE9WpIffzfrNAYRuaezqu+2XmDsWMA0GCSqGSIb3DQEBCwUAMGsx\n" \
"CzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcMCVN0b2Nr\n" \
"aG9sbTEQMA4GA1UECgwHaGltaW5kczELMAkGA1UECwwCQ0ExFTATBgNVBAMMDDE5\n" \
"Mi4xNjguMS40MjAeFw0yMTA3MjMxOTQyMzBaFw0yMjA3MjMxOTQyMzBaMG8xCzAJ\n" \
"BgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcMCVN0b2NraG9s\n" \
"bTEQMA4GA1UECgwHaGltaW5kczEPMA0GA1UECwwGQ2xpZW50MRUwEwYDVQQDDAwx\n" \
"OTIuMTY4LjEuNDIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCuR/+V\n" \
"c5plHL6BO8e2TN4b2OTmJ+yOb9G99v5hsgbwdxYJcXJxOrUxRaw+2fHvgmE4qPmk\n" \
"xTkiI0vlfEaiJITM2XkokeE9rjcdslunJHEeE+DUnAnw8eQzUCATbgmxWSxcVvRK\n" \
"ALj/4xTHkSAOWweDMLuIFI1uTD05X2vww6VKDOh0KCNCUuNif1wbz4V4YqEVx5UL\n" \
"aAjoe77h1SBccmED4myEu2UozQU9Lo9aZbu+ImpiJ5s9DtNstQZWfx/1VCRUMXaj\n" \
"BEhmOwCp05x1S5im3ejEd4sg1ttf8EgputylIX7CqtyMxigR3AsAqH3D86pi+FPp\n" \
"A4jahKRnR92eVzRBAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAG+p8t0Kn3pfTEN6\n" \
"xILQefMRPfqwfVOpZNk9iMB675RcxbkpvR3VCg4XwlDo/HL+/kDiRdC92xkv3l2X\n" \
"azFwedH8gLj5X2DSHqdRTB9EKBI5mmKxlQk+bL7dHQHMoMbB4XEObsGWsyeM0Ddh\n" \
"EoNnQZRTs1FGpR7dLT2AcHiiXmDDY/VcFYmlra2PQWlcCVTDWrN+nqLJkJvlv5dF\n" \
"Nj9RDXDA3LuuKPDGPk0gJ8Ai4EdrfGmseHjwisiYIoMBRRctuffMhRu6oG4xfxma\n" \
"pcoa1geqHmSoFFjFs3Hnb7O/HmN5aN3X/CiWpsxI4005djedZxWc3gllf+CcCNrP\n" \
"ipm6d+Y=\n" \
"-----END CERTIFICATE-----\n";

const char* client_key=\
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCuR/+Vc5plHL6B\n" \
"O8e2TN4b2OTmJ+yOb9G99v5hsgbwdxYJcXJxOrUxRaw+2fHvgmE4qPmkxTkiI0vl\n" \
"fEaiJITM2XkokeE9rjcdslunJHEeE+DUnAnw8eQzUCATbgmxWSxcVvRKALj/4xTH\n" \
"kSAOWweDMLuIFI1uTD05X2vww6VKDOh0KCNCUuNif1wbz4V4YqEVx5ULaAjoe77h\n" \
"1SBccmED4myEu2UozQU9Lo9aZbu+ImpiJ5s9DtNstQZWfx/1VCRUMXajBEhmOwCp\n" \
"05x1S5im3ejEd4sg1ttf8EgputylIX7CqtyMxigR3AsAqH3D86pi+FPpA4jahKRn\n" \
"R92eVzRBAgMBAAECggEAQnCAJuFjQpiqp2m6wOyc39DXskYWEFbghqLaZzQPISzE\n" \
"4eS11CSlXptJBszfRodVuNvyD7aACVubW2j9JGL3nYBqh+TVEUaRDGkqN5XU6bp6\n" \
"Q5MY1xnf/Q/WqpGQcGOmXa1og62y2RogmdPhfJeHTaQOWdo78Ihv7qdJ5wcuxAN2\n" \
"BZoX9uKn/hBcKzE7PuLqEwpV4g2tGseAyC/Nn6k+Iigxxy1m0cNUgDisb0f1ZaOW\n" \
"635IisqcDyn0nCcrAkcvlK1nnRx8V21xtGlB03c+k8FRVfoZ/AuESdj578v1RaE8\n" \
"qIz36Erp+IFOWW1gCWi448k8bF3eArcW55afLrGIAQKBgQDnNDpSi91rGuCKi3Hp\n" \
"ypT2gxCVS5a5EwWd3ELOgAHHJsu5VE4FYvBA0gvbzjIYBOY1MBmLzt9txQvG1y8O\n" \
"OM+wQkmWe7CiE/mv6BXCSSm5kvOly60ae1seu00MFNa72uKRPezQRaTc25IRz4R9\n" \
"99m2xY2VwT5bK6WAGcG3d6CpUQKBgQDA+PCLmZPko9X2lD64czZA2X5tthvAYJp8\n" \
"/3/l/aZ6wt/m65Qq6MK3wiMlPnJh7J8vDpbptknizwc7Vgq6qrT2SNuzH/08X281\n" \
"W+bGSWKw0Ijoe0F25TOZAxkSg35ZK7nx7T2XqLr4DpSpfJleZ+xZ88tN4K0313pM\n" \
"kJpTrGcf8QKBgA58PTMyxBqTwoGrnuVw5chUCdPwum65/F4ZkK3zYykhCx5/Y6v+\n" \
"uOK2xjttPcI3fFzUuKUDvwfXkpdQoAgaExkN7iJZeYWODtSwi7xybQTNaip3Ck9g\n" \
"byMLxRX+sD1wx0UmRhH+awfTUeF/yONDhgZndiltxAReRa4y7uoZup0hAoGAJ+4b\n" \
"EDfqM2B3cLuCMvacHlUi5R7OpUMYFI9l7rhKz5ItdIIAudRKHm4MibNfRcYJtloF\n" \
"fjzbExSVwYBvXere4St4Uptm8iwpPdhTa2paAo5UFg8TH8ATqjDK6lrNNTRoGJhn\n" \
"q08dySBzQJLoxXEppf9dbTL1hN2Qj/FIDdJZ0HECgYEAiWbUynbaElnqC/T3ZN9W\n" \
"VETRcDyXvrfXP9MPFInpPEOq81tzxhx8V19r+ZvXAvLToSpB299NpXXd7Vlil2yr\n" \
"P+vIlM/pqbFZ1MCoVcfHCBTO9EHeDiXO5ooLDHFZn33VSLoZx1Ou+HRGGgbS3ymZ\n" \
"lI3HxyQKYbK/BaCSkbCnj2A=\n" \
"-----END PRIVATE KEY-----\n";

/*void load_client_key(void){
  File client_key = SPIFFS.open("/client.key", "r");
  if (!client_key) {
    Serial.println("Fallo apertura del certificado");
  }
  else{
    Serial.println("Certificado abierto");
  }
  delay(300);
  if (espClient.loadPrivateKey(client_key,sizeof(client_key))){
    Serial.println("Certificado cargado");
  }
  else{
    Serial.println("Certificado NO cargado");
  }
  
}*/


void mqtt_init(void){
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_crt);
  //load_client_key();
  espClient.setPrivateKey(client_key);
  mqttClient.setServer(mqtt_server.c_str(), mqtt_tcp_str.toInt());//mqttClient.setServer("192.168.1.42", 8883);
  mqttClient.setCallback(receivedCallback);
}

void mqtt_connect() {
  /* Loop until reconnected */
  while (!mqttClient.connected()) {
    Serial.print("Conectando a MQTT...");
    /* connect now */
    if (mqttClient.connect(device.c_str())){
      Serial.println("Conectado");
      /* subscribe topic */
      mqttClient.subscribe(topic_act.c_str());
    } else {
      Serial.print("falla, codigo status =");
      Serial.print(mqttClient.state());
      Serial.println("Nuevo intento en 5 segundos");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Publicacion recibida.");
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

  //Modelo de entrada: {"Device":"DAIoT01","Command":"on", "Parameter":"1"} 
  dev_rec=parse_payload["Device"];
  command=parse_payload["Command"];
  if (parse_payload.containsKey("Parameter")){
    param=parse_payload["Parameter"];
  }else{
    param="";
  }

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