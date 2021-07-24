#ifndef _webserver_h_
#define _webserver_h_
#include <Arduino.h>

#include <ESP32WebServer.h>

void handleNotFound();
void handle_root();
void handle_estado();
void handle_config();
void handle_info();

#endif