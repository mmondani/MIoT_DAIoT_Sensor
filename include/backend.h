#ifndef _backend_h_
#define _backend_h_

#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
//#include <LITTLEFS.h>
//#include <LittleFS.h>
#include <ESP32WebServer.h>
#include "eeprom.h"
#include "frontend.h"
//#include "funciones.h"
#include "actuacion.h"

/*======================================[macros y definiciones]============================================*/
#define TRYS_LOAD_FILES               3   //Cantidad de intentos de carga de archivos pedidos por el browser

void init_webserver(void);
void auth(void);
void handle_funciones(void);
void handleUpdateSettings();
void handle_refresh();
void handle_canal(void);
void handle_bootstrap_css(void);
void handle_bootstrap_js(void);
void handle_jquery(void);
void handle_font(void);
void handle_estilo(void);
#endif