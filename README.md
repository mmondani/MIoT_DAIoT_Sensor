# ESP32 + DHT22

Autor: Esp. Ing. Marcelo Castello - 2021


## Contenidos


- [ESP32 + DHT22](#esp32--dht22)
  - [Contenidos](#contenidos)
  - [Introducción](#introducción)
  - [Características](#características)
    - [Medición](#medición)
    - [Actuación](#actuación)
    - [Comunicación](#comunicación)
    - [Configuración](#configuración)
    - [Circuito](#circuito)
    - [Resumen de características](#resumen-de-características)
  - [Testing](#testing)
    - [Node-red](#node-red)
    - [Mosquitto](#mosquitto)
  - [Visualización](#visualización)
- [Mosquitto y PubSub con TLS](#mosquitto-y-pubsub-con-tls)
  - [Crear los certificados](#crear-los-certificados)
  - [Configurar mosquitto](#configurar-mosquitto)
  - [Probar comunicacion](#probar-comunicacion)
  - [Firmware en el esp32:](#firmware-en-el-esp32)
  - [Licencia](#licencia)


## Introducción
El presente desarrollo forma parte del trabajo final de la asignatura Desarrollo de Aplicaciones para IoT de la Carrera de Especialización en  Internet de las Cosas.
El proyecto implementa un microcontrolador ESP32 que incorpora un sensor de temperatura y humedad DHT22 y transmite los valores medidos a través de protocolo MQTT con capa de seguridad TLS a un servidor central donde se aloja el resto del sistema: base de datos y aplicación para visualización y configuración.

## Características
### Medición
El sistema posee un sensor de temperatura y humedad DHT22. La medición se realiza cada ```sampleInterval``` milisegundos. Al cabo de ```averageInterval``` mediciones, se realiza el promedio y  se envía una telemtría al servidor.
Los valores de estas variables están configurados por defecto y son los siguientes:\
```sampleInterval``` 1000 ms\
```averageInterval``` 60 mediciones


### Actuación
Posee dos canales de actuación. Los mismos pueden controlarse desde la página web local o remotamente con comandos via mqtt. Los estados de los canales son almacenados en EEPROM, de manera tal que al reiniciarse el nodo mantendrán su estado anterior.

### Comunicación
La comunicación se realiza por protocolo MQTT con capa de seguridad TLS.

### Configuración
La configuración se realiza a través de un portal embebido en el microcontrolador. Este portal dispone de un menú con tres opciones.
- Home
- Configuración
- Información

El portal posee autenticación. Como el requerimiento es sólo para configuración inicial, existe un solo usuario: ```admin```.\
En la figura se aprecia la ventana de login visto desde un móvil.

![Pantalla de login](./doc/login.png)

La sección ```Home``` muestra las mediciones instantáneas y promedios y el estado de los canales. Tambén es posible desde esta página hacer la actuación de los mismos.

![home](./doc/home_ESP32.png)

En la sección ```configuración``` se pueden modificar las variables para el entorno de emplazamiento del dispositivo. Al guardar los cambios, el microcontrolador guardará estos valores en EEPROM para luego recuperarlos en casos de reinio o rearranque.

![config](./doc/configuracion.png)

La sección ```información``` muestra información inherente al sistema

![info](./doc/info.png)


### Circuito
La figura muestra el circuito implementado con NodeMCU32, DHT22 y componentes auxiliares.

![Circuito](./doc/circuitoESP32.png)

### Resumen de características

- File system (SPIFFS) para almacenar los archivos necesarios para el portal de configuración.
- Portal de configuración de parámetros accesible desde el propio dispositivo (modo AP) o desde la red local a la cual se conecta (dispositivo en modo estación).
- Autenticación del portal de configuración con un solo rol de usuario.
- Parámetros de configuración:
  >hardware: plataforma\
  >device: nombre del dispositivo\
  >mqtt_server: dirección ip del broker mqtt\
  >mqtt_tcp_str: puerto tcp del broker mqtt\
  >FUOTA server: dirección ip del servidor de actualizaciones\
  >ssid: nombre de la red WiFi a la cual se conectará\
  >ssid_pass: passwd de red WiFi a la cual se conectará\
  >passwd_AP: passwd del portal de configuración\
  >ubicacion: lugar de emplazamiento del dispositivo\
  >sensor: tipo de sensor (TC1047|LM35|DHT22)
- Factory reset. Poniendo el pin 25 en '1' al arrancar, se consigue que todas las variables tomen los valores configurados en main.cpp. El FR es indicado por el led ```pulso``` con un destello largo y 20 destellos cortos.
- Valores de factory reset:
  >hardware     =   "ESP32"\
  >device       =   "DAIoTxx"\
  >mqtt_server  =   192.168.1.42\
  >FUOTA server =   192.168.1.17\
  >mqtt_tcp_str =   8883\
  >ssid         =   "MiSSID"\
  >ssid_pass    =   "MiPasswd"\
  >passwd_AP    =   "12345678"\
  >ubicacion    =   "MiUbicacion"\
  >sensor       =   1
- En factory reset la variable ```device``` (nombre de dispositivo), se carga con un template del tipo DAIoTxx, donde xx es un valor aleatorio generado en el momento del factory reset.
- Grabación en EEPROM de los parámetros de configuración, ya sea por cambios desde el portal o por factory reset.
- Dos canales de actuación (pines 12 y 14). Actuados desde el portal de configuración o por comandos mqtt.
- Respuesta mqtt a las actuaciones de los canales con tópico ```device/command``` y payload json:
  > para canal1 encendido: {"Device":"DAIoT01","Valores":{"Canal":1,"Estado":1}}\
  > para canal 1 apagado: {"Device":"DAIoT01","Valores":{"Canal":1,"Estado":0}}\
  > para canal2 encendido: {"Device":"DAIoT01","Valores":{"Canal":2,"Estado":1}}\
  > para canal 2 apagado: {"Device":"DAIoT01","Valores":{"Canal":2,"Estado":0}}\
- Publicación de telemetría cada 60 mediciones por protocolo mqtt:
  > topico: ```device/telemetry```\
  > payload: {"Device":"DAIoT01","Valores":{"ts":1629990934,"Temperatura":20,"Humedad":42}}\
- Recepción de comandos por protocolo mqtt al tópico ```device/command```  y payload con el siguiente modelo:
  > {"Device":"DAIoT01","Command":"on","Parameter":"1"}
- Restablecimiento del estado de los canales después de un apagado del dispositivo.
- Se implementa una máquina de estados para gestionar la conexión a la red Wi-Fi. Intenta durante 5 segundos conectarse a la red que tiene guardada en la EEPROM y en caso de no poder, pasa a un estado de espera. Si, mediante la web de configuración embebida en el dispositivo, se cambia el SSID y/o el password, la máquina de estado vuelve a probar conectarse a la red.
![FsmWiFi](./doc/FSM_WiFi.png)

- Cuando el dispositivo se conecta al broker MQTT, configura un LWT (con el flag retained) con el topic `device/status` y el siguiente contenido:
  > {"Device":"DAIoT01","Status":"offline"}
- Una vez completada la conexión, el primer mensaje (con el flag retained) que envía el dispositivo al broker es, en el topic `device/status`:
  > {"Device":"DAIoT01","Status":"online"}
  
## Testing

Se utilizó node-red y mosquitto_clients para testear el firmware. 

### Node-red

En la figura se aprecia el flujo utilizado

![node-red](./doc/node-red.png)

El primer flujo es un ```mqtt in``` que escucha mensajes en el tópico ```device/#``` o sea todos mensajes cuyos tópicos que comiencen con ```device```. Esta salida se dirige a un nodo ```json``` donde se parsea la payload para poder observar el mensaje en un  nodo ```debug```.

En la figura se muestra un mensaje de telemetría:

![debug_node-red_telemetria](./doc/debug_node-red_telemetria.png)

topic: ```device/telemetry```\
payload: ```{"Device":"DAIoT01","Valores"":{"ts":1616809152,"Temperatura":25.1,"Humedad":80}}```


El segundo flujo, se utiliza para comprobar el procesamiento de comandos mqtt. Se inyectan mensajes con el nodo ```inject``` y se lo dirigen a través de una salida mqtt hacia el dispositivo. Esta salida mqtt envía mensajes al tópico ```device/command```.\
Al inyectar los mensajes, se observa el comportamiento del dispositivo, encendiendo o apagando los leds correspondientes a cada canal.\
Se muestran a continuación los mensajes de node-red debug para una actuación sobre el canal 1.\
En la ventana ```debug``` se observa:

![debug_node-red_comandos](./doc/debug_node-red_comandos.png)
Inyección del comando:\
topic: ```device/command```\
payload: ```{"Device":"DAIoT01","Command":"on","Parameter":"1"}```

Respuesta del dispositivo:\
topic: ```device/accion```\
payload: ```{"Device":"DAIoT01","Valores":{"Canal":1,"Estado":1}}```


###  Mosquitto

También se pueden utilizar los clientes del broker mosquitto para testear la aplicación. Esto clientes son dos:
- mosquitto_sub: 
  cliente para subscripción a mensajes

- mosquitto_pub:
  cliente para publicación de mensajes

Debemos posicionarnos en el directorio donde están los certificados TLS para poder realizar la conexión al broker:
- client.crt
- ca.crt
- client.key
  
Ver los mensajes con ```mosquitto_sub```\
Nos suscribimos a todos los tópicos que comiencen con ```device/```
```sh
mosquitto_sub -h '192.168.1.42' -t '#' -p 8883 --cert client.crt --key client.key --cafile ca.crt -v
```


Publicar mensajes con ```mosquitto_pub```\
Enviamos un mensaje de encendido del canal 1 al tópico ```device/command```
```sh
mosquitto_pub -h '192.168.1.42' -t 'device/command'  -m '{"Device":"DAIoT01","Command":"on","Parameter":"1"}' -p 8883 --cert client.crt --cafile ca.crt --key client.key
```

El dispositivo contesta esta actuación con un mensaje de respuesta en el tópico ```/device/action```, veremos la misma en la terminal desde donde nos hayamos suscripto al tópico ```device/#```.

## Visualización 
Para esto, se utiliza node-red dashboard que genera una sencilla página web para visualizar los datos. En nuestro caso se configuran dos gauges, para temperatura y humedad y un gráfico tipo chart para representar la serie de tiempos.
En la figura se ve la configuración de node-red.

![node-red_dashboard](./doc/node-red_dashboard.png)

En la siguiente figura, se aprecia la visualización.

![node-red_visualizacion](./doc/node-red_visualizacion.png)


# Mosquitto y PubSub con TLS

## Crear los certificados
fuente:
https://medium.com/himinds/mqtt-broker-with-secure-tls-communication-on-ubuntu-18-04-lts-and-an-esp32-mqtt-client-5c25fd7afe67


Ejecutar en bash el siguiente script

#!/bin/bash

IP="192.168.1.42"
SUBJECT_CA="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=CA/CN=$IP"
SUBJECT_SERVER="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=Server/CN=$IP"
SUBJECT_CLIENT="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=Client/CN=$IP"

function generate_CA () {
   echo "$SUBJECT_CA"
   openssl req -x509 -nodes -sha256 -newkey rsa:2048 -subj "$SUBJECT_CA"  -days 365 -keyout ca.key -out ca.crt
}

function generate_server () {
   echo "$SUBJECT_SERVER"
   openssl req -nodes -sha256 -new -subj "$SUBJECT_SERVER" -keyout server.key -out server.csr
   openssl x509 -req -sha256 -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365
}

function generate_client () {
   echo "$SUBJECT_CLIENT"
   openssl req -new -nodes -sha256 -subj "$SUBJECT_CLIENT" -out client.csr -keyout client.key 
   openssl x509 -req -sha256 -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 365
}

function copy_keys_to_broker () {
   cp ca.crt ../mosquitto/certs/
   cp server.crt ../mosquitto/certs/
   cp server.key ../mosquitto/certs/
}

generate_CA
generate_server
generate_client
copy_keys_to_broker

## Configurar mosquitto
listener 8883
cafile /etc/mosquitto/certs/ca.crt
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key
//no->require_certificate true
//no->use_identity_as_username true

## Probar comunicacion
mosquitto_sub -h '192.168.1.42' -t 'topico' --cafile Scaricati/ca.crt  -p 8883 --cert Scaricati/client.crt --key Scaricati/client.key -v

mosquitto_pub -h '192.168.1.42' -t 'topico' --cafile Scaricati/ca.crt  -m 'hola' -p 8883 --cert Scaricati/client.crt --key Scaricati/client.key

## Firmware en el esp32:
const char* ca_cert = aquí va el ca.crt

## Licencia

Este proyecto está licenciado bajo GPLV3
