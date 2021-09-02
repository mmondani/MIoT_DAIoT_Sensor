#include "comandos.h"

extern String tipo_device;
extern String fversion;
extern uint32_t flag_update_eeprom_pos;
extern uint32_t fuota_version_eeprom_pos;

extern String fuota_server;
String comando;
String parametro;

void command_proc(const char* command,const char* param){
  comando=String(command);
  parametro=String(param);
  
  Serial.println(comando);
  Serial.println(parametro);

  //--Comando on
  if (comando=="on"){
    if (parametro=="1"){
      canal1_on();
    }else if (parametro=="2"){
      canal2_on();
    }
  }

  //--Comando off
  if (comando=="off"){
    if (parametro=="1"){
      canal1_off();
    }else if (parametro=="2"){
      canal2_off();
    }
  }

  //--Comando version
  if (comando=="version"){
    Serial.print("version:");
    Serial.println(fversion);
    publica_resp_rpc("version");
  }

  //--Comando update
  if (comando=="update"){
    write_StringEE(fuota_version_eeprom_pos, parametro);
    Serial.print("Version solicitada:");Serial.println(parametro);
    write_StringEE(flag_update_eeprom_pos, "1");
    EEPROM.commit();
    delay(100);
    ESP.restart();
  }
}


void check_update(void){
  String flag_update=read_StringEE(flag_update_eeprom_pos,1);
  if(flag_update=="1"){
    wifi_connect();
    delay(1000);
    String version=read_StringEE(fuota_version_eeprom_pos,25);
    if (version=="") version=fversion;
    Serial.print("Se actualizará a la version:");
    Serial.println(version);
    write_StringEE(flag_update_eeprom_pos, "0");
    noInterrupts();
    EEPROM.commit();
    interrupts();
    Serial.println("Actualizando.....");
    Serial.println("http://"+fuota_server+"/updates/"+tipo_device+"/V"+version+"/firmware.bin");
    t_httpUpdate_return ret=ESPhttpUpdate.update("http://"+fuota_server+"/updates/"+tipo_device+"/V"+version+"/firmware.bin");
    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("[update] Update FAILED (%d): %s\r\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok.");
        break;
    }
  }
}