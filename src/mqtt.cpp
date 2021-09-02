
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
const char *command;
const char *param;
const char *dev_rec;
uint8_t canal_rec;
uint8_t accion_rec;
uint8_t TRYS_LOAD_FILES = 3;

const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
"MIIDxTCCAq2gAwIBAgIUU0Fkb04lnfDf6CQNYz9a94l8wIkwDQYJKoZIhvcNAQEL\n"
"BQAwcjELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAGA1UEBwwJ\n"
"U3RvY2tob2xtMRAwDgYDVQQKDAdoaW1pbmRzMQswCQYDVQQLDAJDQTEcMBoGA1UE\n"
"AwwTZGFpb3QudHBsaW5rZG5zLmNvbTAeFw0yMTA5MDExOTM2MDJaFw0yMjA5MDEx\n"
"OTM2MDJaMHIxCzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNV\n"
"BAcMCVN0b2NraG9sbTEQMA4GA1UECgwHaGltaW5kczELMAkGA1UECwwCQ0ExHDAa\n"
"BgNVBAMME2RhaW90LnRwbGlua2Rucy5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IB\n"
"DwAwggEKAoIBAQCkYCLdNJJztLnO3UyFIOHW8j5KpZJMRS3xBCXzMJGwxC30leqC\n"
"BUBRQPUZzM7eZxb7/1O6HqgCrXAG2AxIMoKoc9zZBDaonCAECgnLar4xf79FxFhF\n"
"M2rgMmjXyRiXfm4a4/vfcrECmfViAPcb4hlx0NPPLfC5pzY8yVewfLdTSxp1iaBE\n"
"g+F9RV3qYbYhG6qwAJ/JpKrpjp8u4PXfewkzX78T8ruUQJc/Y37TR30hg375+Irt\n"
"JOMiUsNRXzcAtxU/Erx1YCYX4Bgzp0QSs8m2N9NIyzUub9I8YIwOnRQWZd/Yiydf\n"
"OXqxLvhWjkVlGxwlaJlHKz5qTE96DIwrLdtpAgMBAAGjUzBRMB0GA1UdDgQWBBTk\n"
"G7tJNTFVbGUSuERZW8bLR2lB6TAfBgNVHSMEGDAWgBTkG7tJNTFVbGUSuERZW8bL\n"
"R2lB6TAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQArf5XS3EUX\n"
"Thp5iOnSd7QkI3OV6x585VKpGsBQUT3BcgJntHLTG+YbCfOm797kU8F+GLMKRO+O\n"
"OrV3/SLPFSD6kgfGWVbrB+R0AvgSGjO1IBGzflgplEug9eKsLIrfITPkcE0k2Ef+\n"
"dHbDctPrN9nhfbNOuhkVxBsJCzssxdWUUfwGghMU5xPyViUl7g4Y6zy6AXh84FgQ\n"
"zMjfVRhPg3UmDN92MT+tROvFjM73L2RGuazS9gqFn3gm4FXcYRhhE00EJpSKmD32\n"
"3u/WJxWulj8j/qv/az6KfUj0LpGrW4EcqDAhHgPDmO60y7YF0KGwV1SSZWd78O5Q\n"
"HRxw7aSu1zvz\n"
"-----END CERTIFICATE-----\n";

const char *client_crt =
    "-----BEGIN CERTIFICATE-----\n"
"MIIDbzCCAlcCFDtwA2zSfC5Wbt+hmif+kUGsfaDWMA0GCSqGSIb3DQEBCwUAMHIx\n"
"CzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcMCVN0b2Nr\n"
"aG9sbTEQMA4GA1UECgwHaGltaW5kczELMAkGA1UECwwCQ0ExHDAaBgNVBAMME2Rh\n"
"aW90LnRwbGlua2Rucy5jb20wHhcNMjEwOTAxMTkzNjAzWhcNMjIwOTAxMTkzNjAz\n"
"WjB2MQswCQYDVQQGEwJTRTESMBAGA1UECAwJU3RvY2tob2xtMRIwEAYDVQQHDAlT\n"
"dG9ja2hvbG0xEDAOBgNVBAoMB2hpbWluZHMxDzANBgNVBAsMBkNsaWVudDEcMBoG\n"
"A1UEAwwTZGFpb3QudHBsaW5rZG5zLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEP\n"
"ADCCAQoCggEBAJ0a3IdNbf+XPEywhC0X/IFVEwYG6IdwjIJO0Fq8cvAM0KSqopuK\n"
"8KZVb9gl7wO6JSh3RgYVzCcCjycfsdyVhaexKP5Bx4YnEuL1LWQedI1wB+ER81yW\n"
"3FGy2TRSNDj/JiAzEY4JrwKcuFgRKllQbp9sQstYUXndpZiijT5WxIgyemDVeDO6\n"
"81nUJzP3Unp9pW9pSKDutUzZXVrPA+/U/CRDvTCZokrzqobpqV6WlRnfyJhUiFq8\n"
"C/xwkTlre1G8IP0vu+Cv/0tusN2IcLWlCxPTWpYzrE5aDp+4KrESHi8wgWA4mfB1\n"
"Boha6vM4gyTvoTjM8adjO9Zh/3x5iQwwWecCAwEAATANBgkqhkiG9w0BAQsFAAOC\n"
"AQEATSlo3WGk8k/KC+Du4TtqEmJ3u8QPkPnIZTnXRgfRJXkOZLHaqdleHvzTeXbf\n"
"sl2UfD/xc+Fo/jy8GjJ+XEzHch7tF07T1YXj8acbNNYm7PPhBuqFZDa0ujWZodmJ\n"
"WvP7kT2ZZtAdYXjUYVPWM2iA9nO0AS9hlqvkcNgfzNsiiBJfCMfbX4yQvYxk+9f8\n"
"vXU2qArXaeSY4BCuEXJK4JdXTC8qE2An97s+Y6WaQpBBZTo62xPasSRBBHsaS1Lg\n"
"kx1PKUNlm3us29uxM3upryK9/ShVj7Nkx5s0E444FfI+E1yvV3KA6r4lD2InjBNf\n"
"1xMBlJPWJ2nQOrmwo2jpWjomKQ==\n"
"-----END CERTIFICATE-----\n";

const char *client_key =
    "-----BEGIN PRIVATE KEY-----\n"
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCdGtyHTW3/lzxM\n"
"sIQtF/yBVRMGBuiHcIyCTtBavHLwDNCkqqKbivCmVW/YJe8DuiUod0YGFcwnAo8n\n"
"H7HclYWnsSj+QceGJxLi9S1kHnSNcAfhEfNcltxRstk0UjQ4/yYgMxGOCa8CnLhY\n"
"ESpZUG6fbELLWFF53aWYoo0+VsSIMnpg1XgzuvNZ1Ccz91J6faVvaUig7rVM2V1a\n"
"zwPv1PwkQ70wmaJK86qG6alelpUZ38iYVIhavAv8cJE5a3tRvCD9L7vgr/9LbrDd\n"
"iHC1pQsT01qWM6xOWg6fuCqxEh4vMIFgOJnwdQaIWurzOIMk76E4zPGnYzvWYf98\n"
"eYkMMFnnAgMBAAECggEAE3mM9MjIZfKdavRGrkBW8MFXBlw83u04vb5/XYAWz2MH\n"
"SiJM3fMvOa9Fmgm5w0CD8qw2ZfK1x/AtRrBLIAj49HyllD8bOKoHUVMLOWn9ELte\n"
"fqv2+vdIgGgnPLEkABzA4STzfCP/Kde7kk8ubLvnuqN09DgSgYTUTkN4Ns4Str85\n"
"YwNYUbpwfR8zNb1waeI8L32wxxCSuU2Mz1iHHAN1v1vrRaafcfWwFW7+/R8mqxBG\n"
"C6CI9HyI9W/fEqXphgjCe6bLKd7addEdtfgj/hv1GNrwz0fdNQ1bow3OYfB+iu6i\n"
"sSrtfBW5zdWta3yTLnG7UyjVmUyH1IEJZ4I2swQPAQKBgQDQM2tlr11gNt/2S6Zp\n"
"a154XGYJ5dvdQkdaa8aoaXeiur1l0dCULHAV80GrXWFu+c35vf239uJH0Kc65nUX\n"
"3dXi/O6jNb3oy0CMNyfJEjngQ8Wu6frpVHcT05CbWlRXeddEs4l2Y783bEw+vBTL\n"
"rYi1IipgKl/tmXbLStw7S3A8RwKBgQDBLGIGwswsbDJSR7ZvkmDoMtfgbKFYjKQ4\n"
"ui1Wd0i/jAJJtTGDUundldP4E6tCy0YVEFTnrSmiVBhaJvD2jOkf/TBemj52JhIM\n"
"ut1g+0ZCvGOU6hbW/RVAHCYAiVI+WCnK3GeWSGsBCau/980qMw5hyZ2g7L+1kDVC\n"
"SzbCLlLlYQKBgFPb8i0h5k1jrO/KZrvdaE64gM0JzXYgRRcxg/G/kVV1O+YwDYAx\n"
"KfBda5z3XtBRQ9qM4Q3hHiFo9IKv4of7bTQhRolskxKaE2Smrpq5gaBIC+65BbxH\n"
"rjLJK1y3lHMrez6Lxhlucc2EDG3UWdf0DLVQjprvVnYgMw4NGAm2B/3vAoGBALDt\n"
"LqdUmV6ChqB30C9mWV2y26hQtoJ85hiS5N2SCU5vNNU/oKbQNSTfirJniW928Kxw\n"
"NeOv5TcpYNiJXB/lNcw3wKqVCWp7OgLwz7x4yIRtOaP1t4+XY3RBk8jf/TTe2QBV\n"
"WIb7sw3TRrxfgUkKVDzaRYZT8M6h0zXx36iAdyphAoGBAJ1ObD/XxfgbkghIMeHa\n"
"veM1KCylXwg3moqtl3LLx9SRlTKOjNidPXdJZhrTBtG4AhEoFqaI5AyQbJGWaHRv\n"
"+1OF5BSz8BjNqxnS0KyCLhPUF/res9Nutnoorf04wfPWFfayU9F5LGOWE0GXkc/C\n"
"BtZTgScNMQnMT3WTppuWipJH\n"
"-----END PRIVATE KEY-----\n";

void load_client_key(void){
  File client_key = SPIFFS.open("/client.key", "r");
  if (!client_key) {
    Serial.println("Fallo apertura del certificado");
  }
  else{
    Serial.println("Certificado abierto");
  }
  delay(300);
  if (espClient.loadPrivateKey(client_key,sizeof(client_key)+1)){
    Serial.println("Certificado cargado");
  }
  else{
    Serial.println("Certificado NO cargado");
  }
  
}

void mqtt_init(void)
{
  // TODO: pasar certificados al FS
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_crt);
  //load_client_key();
  espClient.setPrivateKey(client_key);
  mqttClient.setServer(mqtt_server.c_str(), mqtt_tcp_str.toInt()); //mqttClient.setServer("192.168.1.42", 8883);
  mqttClient.setCallback(receivedCallback);
}

void mqtt_connect()
{
  /* Loop until reconnected */
  while (!mqttClient.connected())
  {
    Serial.print("Conectando a MQTT...");
    Serial.println(mqtt_server.c_str());
    Serial.println(mqtt_tcp_str.toInt());
    /* connect now */
    if (mqttClient.connect(device.c_str()))
    {
      //TODO: Manejar desconexiones
      Serial.println("Conectado");
      /* subscribe topic */
      mqttClient.subscribe(topic_act.c_str());
      //TODO: Agregar mensaje LWT 
    }
    else
    {
      Serial.print("falla, codigo status =");
      Serial.print(mqttClient.state());
      Serial.println("Nuevo intento en 5 segundos");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Publicacion recibida.");
  payload[length] = '\0';
  topic_rec = String((char *)topic);
  msg_rec = String((char *)payload);
  Serial.print("Topic: ");
  Serial.println(topic_rec);
  Serial.print("Payload: ");
  Serial.println(msg_rec);

  StaticJsonDocument<200> parse_payload;
  DeserializationError error = deserializeJson(parse_payload, msg_rec);
  if (error)
  {
    Serial.print("[DEBUG] deserializeJson() fallo: ");
    Serial.println(error.f_str());
    return;
  }

  //Modelo de entrada: {"Device":"DAIoT01","Command":"on", "Parameter":"1"}
  dev_rec = parse_payload["Device"];
  command = parse_payload["Command"];
  if (parse_payload.containsKey("Parameter"))
  {
    param = parse_payload["Parameter"];
  }
  else
  {
    param = "";
  }

  //--Debug del parseo de payload entrante
  //Serial.println("Comando:");
  //Serial.println(dev_rec);
  //Serial.println(command);
  //Serial.println(param);

  //--Si el comando es para este dispositivo, procesa
  if ((String)dev_rec == device)
  {
    command_proc(command, param);
  }
}