#!/bin/bash

#IP="192.168.0.151";
IP="wificom100.dyndns.org";
SUBJECT_CA="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=CA/CN=$IP";
SUBJECT_SERVER="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=Server/CN=$IP";
SUBJECT_CLIENT="/C=SE/ST=Stockholm/L=Stockholm/O=himinds/OU=Client/CN=$IP";

function generate_CA () { echo "$SUBJECT_CA"; openssl req -x509 -nodes -sha256 -newkey rsa:2048 -subj "$SUBJECT_CA" -days 365 -keyout ca.key -out ca.crt ;}

function generate_server () { echo "$SUBJECT_SERVER"; openssl req -nodes -sha256 -new -subj "$SUBJECT_SERVER" -keyout server.key -out server.csr; openssl x509 -req -sha256 -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365 ;}

function generate_client () { echo "$SUBJECT_CLIENT"; openssl req -new -nodes -sha256 -subj "$SUBJECT_CLIENT" -out client.csr -keyout client.key; openssl x509 -req -sha256 -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 365 ;}

function copy_keys_to_broker () { cp ca.crt ../config/certs/ ; cp server.crt ../config/certs/ ; cp server.key ../config/certs/ ;}
function copy_keys_to_esp32 () { cp ca.crt ../certificado/; client.crt ../certificado/ ; cp client.key ./certificado/ ;}
function move_certs () { mv *.crt ./certs/ ; mv *.key ./certs/ ; mv *.csr ./certs/ ;}

generate_CA;
generate_server;
generate_client;
# Warning: Estos comandos sobreescriben los certificados existentes
copy_keys_to_esp32;
move_certs;
# TODO: Cambiar directorio para copiar claves
#copy_keys_to_broker;

