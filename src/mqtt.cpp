
#include "mqtt.h"

//--Objetos externos
extern WiFiClientSecure espClient;
extern PubSubClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;
extern String mqtt_server;
extern String mqtt_tcp_str;

//--Variables externas
extern String topic_act;
extern String topic_status;
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


static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDyTCCArGgAwIBAgIUSm/L0+Xz/zxcH1xGweFHZ4KswvowDQYJKoZIhvcNAQEL
BQAwdDELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAGA1UEBwwJ
U3RvY2tob2xtMRAwDgYDVQQKDAdoaW1pbmRzMQswCQYDVQQLDAJDQTEeMBwGA1UE
AwwVd2lmaWNvbTEwMC5keW5kbnMub3JnMB4XDTIxMDkwMjIzNDQ1NloXDTIyMDkw
MjIzNDQ1NlowdDELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAG
A1UEBwwJU3RvY2tob2xtMRAwDgYDVQQKDAdoaW1pbmRzMQswCQYDVQQLDAJDQTEe
MBwGA1UEAwwVd2lmaWNvbTEwMC5keW5kbnMub3JnMIIBIjANBgkqhkiG9w0BAQEF
AAOCAQ8AMIIBCgKCAQEAo+9zy4ARfc6VM+e9NPNqzF2aXT3b6NtIG3NuqrKJLDHc
lyYCsF6cHeAjCf3tDQrRF4Km3G3WbJOd8QFq7iCQrGR+a64SJa+PZzyPh84Z4kQZ
I+A2uM8orpGOsid10QhMs2SVWyQ0yjc0qYL6LBcP0N2sS6xmL0ysntv0Z/SzvhAn
hMfxqjmFzabNViS+oFWU2cgbnzBqRR2tOd4sOEd4AC2YM/hY2NzqsjfFAMCOhrxq
3k2nL3zbXAc02CrXw+zQeIT7o66wPc69qcPShxGecz+0Vq8qgoBsH4yQt9PDZUUZ
MSV+ZWC7Ot7KuFLHGnJXZGUcwsZd6rjlb33R0ztPIwIDAQABo1MwUTAdBgNVHQ4E
FgQUuzVrcFQvTOuxpPmDYPmye6GnxQQwHwYDVR0jBBgwFoAUuzVrcFQvTOuxpPmD
YPmye6GnxQQwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAgPlp
fSDXKqgddFP4RRkjYIhOW/ZHN3CUogIaf6Vn28LzE/6bSXrCgV+1WKwxejhFyVG/
m7pLbv7sr2txB31d8B7GSfFBoGZZl3Bd2qqhaeJc0z7UTKS7uo5Dz/pUNwEiDu7o
6u+wqYYZzj/bX+TVB88K8YGEnX8Xz+4wmVWCgocxUDMJb0zctJh5O8sAWlQ0FEbr
2bOFnmeQC3U355cwptyE2jil/Q5pr0BCHQ6lOU6lQYKYa38XlrI8ly1TM2QWdg51
fVZK+fJ/ThvKqnu6tf5eA8/geGR5ekZp6FlNwk8xWZsYm016HX/wWTdR/hzCozCB
ERd4TPhacfXpdHoFRg==
-----END CERTIFICATE-----
)EOF";

static const char client_crt[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDczCCAlsCFHV+oMEQGzZl+VCWVwIoipCAN5sVMA0GCSqGSIb3DQEBCwUAMHQx
CzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcMCVN0b2Nr
aG9sbTEQMA4GA1UECgwHaGltaW5kczELMAkGA1UECwwCQ0ExHjAcBgNVBAMMFXdp
Zmljb20xMDAuZHluZG5zLm9yZzAeFw0yMTA5MDIyMzQ0NTdaFw0yMjA5MDIyMzQ0
NTdaMHgxCzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcM
CVN0b2NraG9sbTEQMA4GA1UECgwHaGltaW5kczEPMA0GA1UECwwGQ2xpZW50MR4w
HAYDVQQDDBV3aWZpY29tMTAwLmR5bmRucy5vcmcwggEiMA0GCSqGSIb3DQEBAQUA
A4IBDwAwggEKAoIBAQDKqiexUpPzxsb0mne9eJt4WJLQgjRAY8N1x4uEH4mJnbVZ
9a3/0ED+Kat4sbySAXRn/B21FcKG0lTTtXJNWAnf8W7aM8X/xs5oV5JStynvg+83
Q5S0ks1SX91RpggvuuG8Y9V6/YS/zVhkp9igK+2yRjTj2jiozH8NdYwLGpdP6SLZ
DOHC6K5qKeWq41yEACoxcPVjLHx/gIPJIqr+qwGmsqQcjaskusjTbknTGwAhBAWk
SY/H9Si/XBhXFGt1kyGGDTmEDHfY4t14B9cr4/jxnwDCsox5sBgBXcfqg5C44ZDq
O/+lk1j46nFwt4kREkcgCoFJ51Zr+pKGSDXjqQ+pAgMBAAEwDQYJKoZIhvcNAQEL
BQADggEBADRRjWdgc50YrjT7wRrg6hMtcByNBpj0HiCWCKzWETFc4yI3eKiGtYnd
DcrZid1QFZOFwtV8E2WTk8ltNaf3tfFhrpthl3vJpEY7X48jTOF9FlU3s87q/KUf
eBYaPb5Y2rB1/KU2poXetRQ/jF2D33bistzVqebcd+G5REF5/pNdUSNjogkdakJW
07NsR5OmofMCQzXe5ACBvnZGDCb7N/GcCpuc4aUjOrf+U/socUWW8jmYquLlK/9x
CbSq/O/vYoyKkuuEV3IF7s+2t0Fdg/vBej2iWsjSXGZbJhzdUUlocJ/v9GGDfO+p
QIWn6OVQpO/4Aehu09M1AsdJQZtUms0=
-----END CERTIFICATE-----
)KEY";

static const char client_key[] PROGMEM = R"KEY(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDKqiexUpPzxsb0
mne9eJt4WJLQgjRAY8N1x4uEH4mJnbVZ9a3/0ED+Kat4sbySAXRn/B21FcKG0lTT
tXJNWAnf8W7aM8X/xs5oV5JStynvg+83Q5S0ks1SX91RpggvuuG8Y9V6/YS/zVhk
p9igK+2yRjTj2jiozH8NdYwLGpdP6SLZDOHC6K5qKeWq41yEACoxcPVjLHx/gIPJ
Iqr+qwGmsqQcjaskusjTbknTGwAhBAWkSY/H9Si/XBhXFGt1kyGGDTmEDHfY4t14
B9cr4/jxnwDCsox5sBgBXcfqg5C44ZDqO/+lk1j46nFwt4kREkcgCoFJ51Zr+pKG
SDXjqQ+pAgMBAAECggEAbe11sj00e1oquKrAoEk3K0P//pFhfjue3dfq2TOyCq88
mxsj0kpHtpUrqT/fJOMdHPonsv2mkjKsSFl3MnS6hnoSwtCqaV6QvxAhEjS/zmcl
7jf+BSRLso7FIXCJqDoaD3GnbhrtUowzkgVo/xhUygpCzgSkDws9LzO/TpaR1ImN
BU7jGj1ryFkncSFX1Z6XKdoROzG9JlfTdKy3oJIcjwebh7M7KXTo4k/enul4cGqr
ObkW7E0zQPVqsBGgp5otNnMLq0sQIq7xutyo4TsvChVn4sWlMXLjUZxx0TjmiKHD
vBL8pyyYzh3jhXhrofw1qOPeTkhK6fEwI9T4v0FocQKBgQDyYtB/cZpvwDBC/+YN
PLVFTJCUbhVgB4/8Cg6J8I/gO2/ARdk22gHdQL51RTmV2fFx12d8Yttv46RNrmqG
IcFKRDDyXIBuSOccCOqnznw/KrPgz2yWaCFYCN+ClqfdmVqED30l6B7RuApyhUil
JfYNBaM6pOX12MXGEFKyE1OBMwKBgQDWDDOCQWahBi1B66dRA6rIt+4Y2LlCJXXr
H7jPwUR87YV4p/Yj8lg64cEUFJspZITTMyR/fokPm1TYXB+fCBjI0iYBoOM1c0yV
JWHGNXq8XEZlsEm8LMAAuSZ5T4SIvQIqOTXByV82Qoa+HEnsklwXusGPZLD/4rXd
KTYm03VjswKBgEiWAu5HbQJOQaWu0IviRD6ugRWSKeCKIzpNILF6UvvJkNu5jyT4
AFrryp1/z5DqpX18DkeXqzNErq18r/XfIe1FoorR3LwqJNumZ+88zMR120xVkaUx
JdshPR30U+zuwcaOhOHXM3OGc8p705dxWM/pgkpLm3oWIpP/y2P8maM1AoGASW/0
OCtiH7rzyhsk05nTfXwFd4bbwMMDewDuMnFXoxkG4weVXm3mMYWE7fPH7GKtY5y3
V/q84ZrkeDrbKn/nqPbYzVQ6v/rn+n/R1ORCUN/D012xXptn4JeliJdsnFFkrcHl
HLCPNxmKCy2/eygX6phLknv0ckaBKPjbuGRycRsCgYByF+k2ntW7qIEiOLCShwLY
ICor3INW5/EQ+YV/qApWwPEttp1fFEGtnAQEHns1S49OdwRJLcnkbbKPCZs/Gf7t
KnLeGFuHC7mpxyFBGSbg+Ehi8rHkUMcS6qynZe59Mv6ALiq1WCW7baEqAiDiuiQ7
O83JKGs1C71DFQJuJakdcQ==
-----END PRIVATE KEY-----
)KEY";


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
    Serial.println("\nConectando a MQTT...");
    Serial.println(mqtt_server.c_str());
    Serial.println(mqtt_tcp_str.toInt());

    /* connect now */
    String lwt_message = "{\"Device\":\"" + device + "\",\"Status\":\"offline\"}";

    if (mqttClient.connect(
            device.c_str(),
            topic_status.c_str(),
            0,
            true,
            lwt_message.c_str()))
    {
      //TODO: Manejar desconexiones
      Serial.println("Conectado");
      /* subscribe topic */
      mqttClient.subscribe(topic_act.c_str());
      //TODO: Agregar mensaje LWT 

      publica_status(true);
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