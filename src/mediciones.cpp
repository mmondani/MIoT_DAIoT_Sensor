#include "mediciones.h"

//--Variables locales
uint16_t cant_mediciones=0;
uint32_t tempi_sum=0;
uint32_t humi_sum=0;

//--Variables externas
extern float humi;
extern float tempi;
extern float hump;
extern float tempp;
extern uint8_t pin_dht;
extern uint8_t tipo_dht;
extern uint8_t averageInterval;
extern uint8_t flag_publica;
//--Instancias externas
extern DHT dht;

void medicion(void){
    humi = dht.readHumidity();
    tempi = dht.readTemperature();
    //--Check if any reads failed and exit early (to try again).
    if (isnan(tempi) || isnan(humi)) {
        Serial.println(F("Fallo la lectura del sensor DHT!"));
        return;
    }
    cant_mediciones++;
    tempi_sum=tempi_sum+tempi;
    humi_sum=humi_sum+humi;
    if (cant_mediciones>=averageInterval){
        promedia_dht();
        publica_medicion();
    }
}

void promedia_dht(void){
    tempp=tempi_sum/cant_mediciones;
    tempi_sum=0;
    hump=humi_sum/cant_mediciones;
    humi_sum=0;
    cant_mediciones=0;
}