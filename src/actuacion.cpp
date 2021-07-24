#include "actuacion.h"


//--Variables externas
extern uint8_t led_pulso;
extern uint8_t canal1_pin;
extern uint8_t canal2_pin;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern TimerHandle_t freqLedTimer;
extern uint8_t canal1_eeprom_pos;
extern uint8_t canal2_eeprom_pos;


void canal1_on(void){
    digitalWrite(canal1_pin, HIGH);
    canal1_status=1;
    wr_eeprom_canal(1);
    publica_canales(1);
}

void canal1_off(void){
    digitalWrite(canal1_pin, LOW);
    canal1_status=0;
    wr_eeprom_canal(1);
    publica_canales(1);
}

void canal2_on(void){
    digitalWrite(canal2_pin, HIGH);
    canal2_status=1;
    wr_eeprom_canal(2);
    publica_canales(2);
}

void canal2_off(void){
    digitalWrite(canal2_pin, LOW);
    canal2_status=0;
    wr_eeprom_canal(2);
    publica_canales(2);
}

void led_on(int acc){
    if (acc==1){
        digitalWrite(led_pulso, HIGH);

        Serial.println("Led encendido");
    }else if (acc==0){
        digitalWrite(led_pulso, LOW);
        Serial.println("Led apagado");

    }

}

void led_pulse(void){
        //xTimerStart(freqLedTimer, 0);
        digitalWrite(led_pulso, LOW);
        delay(50);
        digitalWrite(led_pulso, HIGH);
        delay(50);
}

void restore_canales(void){
    if (canal1_status==1){
        canal1_on();
    }
    if (canal2_status==1){
        canal2_on();
    }
}


void wr_eeprom_canal(uint8_t canal){
    if (canal==1){
        write_StringEE(canal1_eeprom_pos, String(canal1_status));
    }
    if (canal==2){
        write_StringEE(canal2_eeprom_pos, String(canal2_status));
    }
    EEPROM.commit();
}
