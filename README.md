MIoT - DAIoT: Trabajo Final - Sensor
=======================

## Primeros pasos

### Instalar las dependencias

Para correr esté código es necesario contar con `Visual Studio Code` y `PlatformIO`. `PlatformIO` se instala como un plugin de VS Code.


### Descargar el código

Para descargar el código se debe ejecutar el siguiente comando:
```
git clone https://github.com/mmondani/MIoT_DAIoT_Sensor.git
```

## Introducción
El presente desarrollo se basó en el siguiente repositorio:

```
https://github.com/mcastellogh/daiot-esp32
```

A continuación se describen los cambios/agregados que se realizaron en la implementación. Una explicación completa del resto del proyecto se encuentra en el README.md del repositorio antes mencionado.


## Detalles de la implementación

### Gestión de la conexión Wi-Fi

Se implementa una máquina de estados para gestionar la conexión a la red Wi-Fi. Intenta durante 5 segundos conectarse a la red que tiene guardada en la EEPROM y en caso de no poder, pasa a un estado de espera. Si, mediante la web de configuración embebida en el dispositivo, se cambia el SSID y/o el password, la máquina de estado vuelve a probar conectarse a la red.

![FsmWiFi](./doc/FSM_WiFi.png)

### LWT

Cuando el dispositivo se conecta al broker MQTT, configura un LWT con el topic `device/status` y el siguiente contenido:

  > {"Device":"DAIoT01","Status":"offline"}

Una vez completada la conexión, el primer mensaje que envía el dispositivo al broker es, en el topic `device/status`:

  > {"Device":"DAIoT01","Status":"online"}


### Declaración de los certificados

En el archivo `mqtt.c` se debe declarar el contenido del certificado del CA, el del cliente y la llave privada. Para evitar tener que concatener strings en la declaración, se usaron las directivas `R"EOF()EOF"` y `R"KEY()KEY"`, las cuales permiten pegar el contenido de los certificados sin necesidad de agregar ningún caracter adicional.

Por ejemplo, el certificado del CA se declara de la siguiente forma:

```c
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDyTCCArGgAwIBAgIUSm/L0+Xz/zxcH1xGweFHZ4KswvowDQYJKoZIhvcNAQEL
BQAwdDELMAkGA1UEBhMCU0UxEjAQBgNVBAgMCVN0b2NraG9sbTESMBAGA1UEBwwJ
   ....
-----END CERTIFICATE-----
)EOF";
```

Para el caso del certificado del cliente y su llave privada:

```c
static const char client_crt[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDczCCAlsCFHV+oMEQGzZl+VCWVwIoipCAN5sVMA0GCSqGSIb3DQEBCwUAMHQx
CzAJBgNVBAYTAlNFMRIwEAYDVQQIDAlTdG9ja2hvbG0xEjAQBgNVBAcMCVN0b2Nr
   ....
-----END CERTIFICATE-----
)KEY";


static const char client_key[] PROGMEM = R"KEY(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDKqiexUpPzxsb0
mne9eJt4WJLQgjRAY8N1x4uEH4mJnbVZ9a3/0ED+Kat4sbySAXRn/B21FcKG0lTT
   ....
-----END PRIVATE KEY-----
)KEY";
```