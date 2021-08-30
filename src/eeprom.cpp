#include "eeprom.h"

//--Variables externas
extern String device;
extern String mqtt_server;
extern String fuota_server;
extern uint16_t mqtt_tcp;
extern String mqtt_tcp_str;
extern String passwd_AP;
extern String ssid;
extern String ssid_pass;
extern String ubicacion;
extern uint8_t canal1_status;
extern uint8_t canal2_status;
extern uint8_t sensor;
extern uint32_t device_eeprom_pos;
extern uint32_t mqtt_server_eeprom_pos;
extern uint32_t mqtt_tcp_str_eeprom_pos;
extern uint32_t passwd_AP_eeprom_pos;
extern uint32_t ssid_eeprom_pos;
extern uint32_t ssid_passwd_eeprom_pos;
extern uint32_t ubicacion_eeprom_pos;
extern uint32_t fuota_server_eeprom_pos;
extern uint32_t canal1_eeprom_pos;
extern uint32_t canal2_eeprom_pos;
extern uint32_t sensor_eeprom_pos;
extern uint8_t freset_pin;
extern uint8_t led_pulso;

void read_vars(bool ver){
  device=read_StringEE(device_eeprom_pos, 25);
  mqtt_server=read_StringEE(mqtt_server_eeprom_pos, 25);
  fuota_server=read_StringEE(fuota_server_eeprom_pos, 25);
  mqtt_tcp_str=read_StringEE(mqtt_tcp_str_eeprom_pos, 25);
  passwd_AP=read_StringEE(passwd_AP_eeprom_pos, 25);
  ssid=read_StringEE(ssid_eeprom_pos, 25); 
  ssid_pass=read_StringEE(ssid_passwd_eeprom_pos, 25); 
  ubicacion=read_StringEE(ubicacion_eeprom_pos, 25); 
  canal1_status=read_StringEE(canal1_eeprom_pos,25).toInt();
  canal2_status=read_StringEE(canal2_eeprom_pos,25).toInt();
  sensor=read_StringEE(sensor_eeprom_pos,25).toInt();

  mqtt_tcp=mqtt_tcp_str.toInt();
  if(ver){
    Serial.println("");
    Serial.print("Nombre:");Serial.println(device);
    Serial.print("Broker:");Serial.println(mqtt_server);
    Serial.print("Fuota server:");Serial.println(fuota_server);
    Serial.print("Puerto mqtt:");Serial.println(mqtt_tcp);
    Serial.print("Passwd AP:");Serial.println(passwd_AP);
    Serial.print("SSID:");Serial.println(ssid);
    Serial.print("Passwd SSID:");Serial.println(ssid_pass);
    Serial.print("Ubicacion:");Serial.println(ubicacion);
    Serial.print("Canal1:");Serial.println(canal1_status);
    Serial.print("Canal2:");Serial.println(canal2_status);
    Serial.print("Sensor:");Serial.println(sensor);
  }
}

void write_vars(void){
  bool res;

  res = write_StringEE(device_eeprom_pos, device);
  check_error_updating(res);
  //device=read_StringEE(device_eeprom_pos,25);

  res = write_StringEE(mqtt_server_eeprom_pos, mqtt_server);
  check_error_updating(res);
  //mqtt_server=read_StringEE(mqtt_server_eeprom_pos,25);

  res = write_StringEE(fuota_server_eeprom_pos, fuota_server);
  check_error_updating(res);
  //fuota_server=read_StringEE(fuota_server_eeprom_pos,25);

  res = write_StringEE(mqtt_tcp_str_eeprom_pos, mqtt_tcp_str);
  check_error_updating(res);
  //mqtt_tcp_str=read_StringEE(mqtt_tcp_str_eeprom_pos,25); 

  res = write_StringEE(passwd_AP_eeprom_pos, passwd_AP);
  check_error_updating(res);
  //passwd_AP=read_StringEE(passwd_AP_eeprom_pos,25); 

  res = write_StringEE(ssid_eeprom_pos, ssid);
  check_error_updating(res);
  //ssid=read_StringEE(ssid_eeprom_pos,25); 

  res = write_StringEE(ssid_passwd_eeprom_pos, ssid_pass);
  check_error_updating(res);
  //ssid_pass=read_StringEE(ssid_passwd_eeprom_pos,25); 

  res = write_StringEE(ubicacion_eeprom_pos, ubicacion);
  check_error_updating(res);
  //ubicacion=read_StringEE(ubicacion_eeprom_pos,25);  

  res = write_StringEE(canal1_eeprom_pos, String(canal1_status));
  check_error_updating(res);
  //canal1_status=read_StringEE(canal1_eeprom_pos,25).toInt();

  res = write_StringEE(canal2_eeprom_pos, String(canal2_status));
  check_error_updating(res);
  //canal2_status=read_StringEE(canal2_eeprom_pos,25).toInt();

  res = write_StringEE(sensor_eeprom_pos, String(sensor));
  check_error_updating(res);
  //sensor=read_StringEE(sensor_eeprom_pos,25).toInt();

  EEPROM.commit();

  read_vars(0);
}

void factory_reset(void){
  delay(50);
  if (digitalRead(freset_pin)){
    delay(150);
    if (digitalRead(freset_pin)){
      Serial.println("\r\nFactory reset!!");
      delay(100);
      digitalWrite(led_pulso,HIGH);
      delay(1000);
      digitalWrite(led_pulso,LOW);
      delay(500);
      for (int i=0; i<20; i++){
        digitalWrite(led_pulso,HIGH);
        delay(30);
        digitalWrite(led_pulso,LOW);
        delay(30);
      }
      write_StringEE(device_eeprom_pos, device+String(random(1,1000))); //agrega nº random al nombre
      write_StringEE(mqtt_server_eeprom_pos, mqtt_server);
      write_StringEE(mqtt_tcp_str_eeprom_pos, mqtt_tcp_str);
      write_StringEE(passwd_AP_eeprom_pos, passwd_AP);
      write_StringEE(ssid_eeprom_pos, ssid);
      write_StringEE(ssid_passwd_eeprom_pos, ssid_pass);
      write_StringEE(ubicacion_eeprom_pos, ubicacion);
      write_StringEE(canal1_eeprom_pos, String(canal1_status));
      write_StringEE(canal2_eeprom_pos, String(canal2_status));
      write_StringEE(sensor_eeprom_pos, String(sensor));
      
      EEPROM.commit();
    }
  }
}

//--Funciones de bajo nivel
//---------------------------------------- EEPROM UTILS ------------------------------------------------------------------



// EEPROM UTILS
//
// http://playground.arduino.cc/Code/EepromUtil
//
// Absolute min and max eeprom addresses.
// Actual values are hardware-dependent.
//
// These values can be changed e.g. to protect
// eeprom cells outside this range.
//
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 511;


//
// Writes a string starting at the specified address.
// Returns true if the whole string is successfully written.
// Returns false if the address of one or more bytes
// fall outside the allowed range.
// If false is returned, nothing gets written to the eeprom.
//
boolean eeprom_write_string(int addr, const char* string) {
  // actual number of bytes to be written
  int numBytes;

  // we'll need to write the string contents
  // plus the string terminator byte (0x00)
  numBytes = strlen(string) + 1;

  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}


//
// Reads a string starting from the specified address.
// Returns true if at least one byte (even only the
// string terminator one) is read.
// Returns false if the start address falls outside
// or declare buffer size os zero.
// the allowed range.
// The reading might stop for several reasons:
// - no more space in the provided buffer
// - last eeprom address reached
// - string terminator byte (0x00) encountered.
// The last condition is what should normally occur.
//
boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
  // byte read from eeprom
  byte ch;

  // number of bytes read so far
  int bytesRead;

  // check start address
  if (!eeprom_is_addr_ok(addr)) {
    return false;
  }

  // how can we store bytes in an empty buffer ?
  if (bufSize == 0) {
    return false;
  }

  // is there is room for the string terminator only,
  // no reason to go further
  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }

  // initialize byte counter
  bytesRead = 0;

  // read next byte from eeprom
  ch = EEPROM.read(addr + bytesRead);

  // store it into the user buffer
  buffer[bytesRead] = ch;

  // increment byte counter
  bytesRead++;

  // stop conditions:
  // - the character just read is the string terminator one (0x00)
  // - we have filled the user buffer
  // - we have reached the last eeprom address
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
    // if no stop condition is met, read the next byte from eeprom
    ch = EEPROM.read(addr + bytesRead);

    // store it into the user buffer
    buffer[bytesRead] = ch;

    // increment byte counter
    bytesRead++;
  }

  // make sure the user buffer has a string terminator
  // (0x00) as its last byte
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }

  return true;
}

//
// Writes a sequence of bytes to eeprom starting at the specified address.
// Returns true if the whole array is successfully written.
// Returns false if the start or end addresses aren't between
// the minimum and maximum allowed values.
// When returning false, nothing gets written to eeprom.
//
boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
  // counter
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }

  return true;
}


//
// Returns true if the address is between the
// minimum and maximum allowed values,
// false otherwise.
//
// This function is used by the other, higher-level functions
// to prevent bugs and runtime errors due to invalid addresses.
//
boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

//
// Dump eeprom memory contents over serial port.
// For each byte, address and value are written.
//
void eeprom_serial_dump_column() {
  // counter
  int i;

  // byte read from eeprom
  byte b;

  // buffer used by sprintf
  char buf[10];

  for (i = EEPROM_MIN_ADDR; i <= EEPROM_MAX_ADDR; i++) {
    b = EEPROM.read(i);
    sprintf(buf, "%03X: %02X", i, b);
    Serial.println(buf);
  }
}

// Credits
// http://mario.mtechcreations.com/programing/write-string-to-arduino-eeprom/
//


//Takes in a String and converts it to be used with the EEPROM Functions
//
bool write_StringEE(int Addr, String input)
{
  char cbuff[input.length() + 1]; //Finds length of string to make a buffer
  input.toCharArray(cbuff, input.length() + 1); //Converts String into character array
  return eeprom_write_string(Addr, cbuff); //Saves String
}

//Given the starting address, and the length, this function will return
//a String and not a Char array
String read_StringEE(int Addr, int length)
{

  char cbuff[length + 1];
  eeprom_read_string(Addr, cbuff, length + 1);

  String stemp(cbuff);
  return stemp;

}

// If an EPPROM write fails it commits changes and sends user to error page inviting to try again
void check_error_updating(bool EEPROM_write_res) {

  if (EEPROM_write_res == false) {
    EEPROM.commit();
    Serial.println("Algo salió mal en la grabación de EEPROM");

  }

}


//---------------------------------------- END EEPROM UTILS ------------------------------------------------------------------