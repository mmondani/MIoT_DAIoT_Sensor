#include "frontend.h"

//--Locales
String header;

//--Objetos Externos
extern ESP32WebServer web_server;
extern void auth(void);

//--Variables Externas
extern String tipo_device;
extern String hversion;
extern String fversion;
extern String device;
extern String tipo_device;
extern String mqtt_server;
extern String fuota_server;
extern String mqtt_tcp_str;
extern String ubicacion;
extern String ssid;
extern String ssid_pass;
extern String passwd_AP;
extern String area;
extern float tempi;
extern float humi;
extern float tempp;
extern float hump;

//------Páginas web-------------------------------------------------------

//--No existe la pagina
void handleNotFound(){
    web_server.send ( 404, "text/html", "El recurso solicitado no existe" );
}

//--Home
void handle_root() {
 const char* Home;
 auth();
 header="<!DOCTYPE html>\
 <html lang='en'>\
  <head>\
   <title>"+tipo_device+"</title>\
    <meta charset='utf-8'>\
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link href='/css/bootstrap.min.css.gz' rel='stylesheet'>\
    <script src='/js/jquery.min.js.gz'></script>\
    <script src='/js/bootstrap.min.js.gz'></script>\
    <script src='/js/funciones.js'></script>\
    <link href='/css/miestilo.css' rel='stylesheet'>\
   </head>\
   <body>\
    <header>\
     <nav class='navbar navbar-inverse navbar-fixed-top' role='navigation'>\
      <div class='container'>\
       <div class='navbar-header'>\
        <button type='button' class='navbar-toggle' data-toggle='collapse' data-target='#navbar'>\
         <span class='icon-bar'></span>\
         <span class='icon-bar'></span>\
         <span class='icon-bar'></span>\
        </button>\
        <a class='navbar-brand' href='#'>"+tipo_device+"</a>\
       </div>\
       <div class='collapse navbar-collapse' id='navbar'>\
        <ul class='nav navbar-nav navbar-right'>\
         <li><a href='#' id='home'>Home</a></li>\
         <li><a href='#' id='config'>Configuración</a></li>\
         <li><a href='#' id='info'>Información</a></li>\
        </ul>\
       </div> \
      </div>\
     </nav>\
    </header>";
 Home="<div id='contenedor'></div>\
      <script>\
        setInterval(function(){\
         refresh();\
        },10000);\
        $(function() {\
         $('.navbar-nav').on('click', function(){\
          if($('.navbar-header .navbar-toggle').css('display') !='none'){\
           $('.navbar-header .navbar-toggle').trigger( 'click' );\
          }\
         });\
        });\
        $(document).ready(function() {\
         $('#home').on('click',function(){\
          $('#contenedor').load('estado.html');\
         });\
         $('#config').on('click',function(){\
          $('#contenedor').load('config.html');\
         });\
         $('#info').on('click',function(){\
          $('#contenedor').load('info.html');\
         });\
         $('#contenedor').load('estado.html');\
         refresh();\
        });\
      </script>\
   </body>\
</html>";
  web_server.send ( 200, "text/html",header+Home);
}

//--Pagina de estado
void handle_estado() {
  String estado;
  auth();
  estado="<!DOCTYPE HTML>\
<html>\
 <head>\
  <script>\
   $(document).ready(function() {\
   refresh();\
   });\
  </script>\
 </head>\
 <body><br><br>\
  <div class='panel panel-primary bg-2' id='home'>\
   <div class='panel-heading'>Home</div>\
   <div class='panel-body'>\
    <form>\
     <div class='well'><h2 class='text-center'>Dispositivo: " + device + "</h2><p>\
     </div>\
     <div class='well'><h2 class='text-center'>Mediciones</h2><p>\
      <label for='vp'>Valores instantáneos</label><p>\
      Temperatura\
      <span class='text-primary' id='ti'>"+tempi+" </span> ºC<p>\
      Humedad\
      <span class='text-primary' id='hi'>"+humi+" </span> %<p>\
      <label for='vp'>Valores promedios</label><p>\
      Temperatura\
      <span class='text-primary' id='tp'>"+tempp+" </span> ºC<p>\
      Humedad\
      <span class='text-primary' id='hp'>"+hump+" </span> %<p>\
     </div>\
     <div class='well'><h2 class='text-center'>Estado canales</h2><p>\
      Canal 1\
      <span class='text-primary' id='stc1'></span><p>\
      Canal 2\
      <span class='text-primary' id='stc2'></span><p>\
     </div>\
     <div class='well'><h2 class='text-center'>Actuación</h2><p>\
      <label for='c1'>Canal 1</label><p>\
       <button type='button' class='btn btn-success btn-lg btn-block' name='c11' id='c11' value='1' onclick='onchangech(11)'>Enciende</button>\
       <button type='button' class='btn btn-danger btn-lg btn-block' name='c10' id='c10' value='0' onclick='onchangech(10)'>Apaga</button>\
       <p>\
      <label for='c2'>Canal 2</label><p>\
       <button type='button' class='btn btn-success btn-lg btn-block' name='c21' id='c21' value='1' onclick='onchangech(21)'>Enciende</button>\
       <button type='button' class='btn btn-danger btn-lg btn-block' name='c20' id='c20' value='0' onclick='onchangech(20)'>Apaga</button>\
       <p>\
     </div>\
    </form>\
   </div>\
  </div>\
 </body>\
</html>";
  web_server.send ( 200, "text/html",estado);
}

//--Pagina de configuracion
void handle_config() {
  String conf;
  auth();
  conf="<!DOCTYPE html>\
<html lang='en'>\
 <body>\
  <br><br>\
  <div id='config' class='panel panel-primary bg-2'>\
   <div class='panel-heading'>Configuración</div>\
   <div class='panel-body'>\
    <form name='actualiza' action='/update_settings' method='post'>\
     <div class='well'><h2 class='text-center'>Red</h2>\
      <div class='form-group'>\
       <label for='mod'>Red</label>\
       <input type='text' class='form-control' name='ssid' id='ssid' value='"+ ssid +"'>\
      </div>\
      <div class='form-group'>\
       <label for='mod'>Passwd WiFi</label>\
       <input type='text' class='form-control' name='ssid_pass' id='ssid_pass' value='"+ ssid_pass +"'>\
      </div>\
      <div class='form-group'>\
       <label for='mod'>Passwd AP</label>\
       <input type='text' class='form-control' name='passwd_AP' id='passwd_AP' value='"+ passwd_AP +"'>\
      </div>\
      <div class='form-group'>\
       <label for='mqtt_server'>Broker</label>\
       <input type='text' class='form-control' name='mqtt_server' id='mqtt_server' value='" + mqtt_server + "'>\
      </div>\
      <div class='form-group'>\
       <label for='mqtt_server'>Puerto Broker</label>\
       <input type='text' class='form-control' name='mqtt_tcp_str' id='mqtt_tcp_str' value='" + mqtt_tcp_str + "'>\
      </div>\
      <div class='form-group'>\
       <label for='mqtt_server'>Fuota server</label>\
       <input type='text' class='form-control' name='fuota_server' id='fuota_server' value='" + fuota_server + "'>\
      </div>\
     </div>\
     <div class='well'><h2 class='text-center'>Dispositivo</h2>\
      <div class='form-group'>\
       <label for='dev'>Nombre</label>\
       <input type='text' class='form-control' name='device' id='device' value='" + device + "'>\
      </div>\
      <div class='form-group'>\
       <label for='ubic'>Ubicacion</label>\
       <input type='text' class='form-control' name='ubic' id='ubic' value='" + ubicacion + "'>\
      </div>\
      <div class='col-xs-4'>\
       <button type='button' onclick='valida(1)' class='btn btn-success'>\
        Guardar\
       </button>\
      </div>\
     </div>\
    </form>\
   </div>\
  </div>\
 </body>\
</html>";
  web_server.send ( 200, "text/html",conf);
}

//--Pagina de info
void handle_info() {
  String info;
  auth();
  info="<!DOCTYPE html>\
<html lang='en'>\
 <body><br><br>\
  <div id='info' class='panel panel-primary bg-2'>\
   <div class='panel-heading'>Información</div>\
   <div class='panel-body text-center'>\
    <div class='form-group'>\
     <p>"+tipo_device+" de monitoreo remoto</p>\
     <p>Desarrollo de Aplicaciones para IoT</p>\
     <p>Ing. Marcelo Castello</p>\
     <p>Carrera de Especialización en Internet de las Cosas<p>\
     <p>Universidad de Buenos Aires</p>\
     <p>Versiones: hardware "+hversion+", firmware "+fversion+"</p>\
    </div>\
   </div>\
  </div>\
 </body>\
</html>";
web_server.send ( 200, "text/html",info);
}