#include "comandos.h"

extern String tipo_device;
extern String fversion;
extern uint32_t flag_update_eeprom_pos;
extern uint32_t fuota_version_eeprom_pos;

String fuota_server="192.168.1.17";
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
    write_StringEE(fuota_version_eeprom_pos,parametro);//470
    write_StringEE(flag_update_eeprom_pos, "1");//500
    //out["Actualizando a:"]="V"+fversion;
    //send_rpc_rta();
    noInterrupts();
    EEPROM.commit();
    interrupts();
    delay(100);
    ESP.restart();
  }
}


void check_update(void){
  String flag_update=read_StringEE(flag_update_eeprom_pos,1);//500
  if(flag_update=="1"){
    connectToWifi();
    WiFi.onEvent(WiFiEvent);
    delay(1000);
    String version=read_StringEE(fuota_version_eeprom_pos,25);//470
    if (version="") version=fversion;
    Serial.print("Se actualizará a la version:");
    Serial.println(version);
    write_StringEE(flag_update_eeprom_pos, "0");//500
    noInterrupts();
    EEPROM.commit();
    interrupts();
    Serial.println("Actualizando.....");

    /*ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);*/
    //WiFiClient wf_client;
    //ESPhttpUpdate.setLedPin(LED_RANGO, LOW);
    //LittleFS.end();
    //ESP.wdtDisable();
  
    

    Serial.println("http://"+fuota_server+"/updates/"+tipo_device+"/V"+version+"/firmware.bin");
    t_httpUpdate_return ret=ESPhttpUpdate.update("http://"+fuota_server+"/updates/"+tipo_device+"/V"+version+"/firmware.bin");
    //auto ret=ESPhttpUpdate.update("http://"+fuota_server+"/updates/"+tipo_device+"/"+version+"/firmware.bin");
    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("[update] Update FAILED (%d): %s\r\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
    }
  }
}