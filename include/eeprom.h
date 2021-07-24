#ifndef _eeprom_h_
#define _eeprom_h_
#include <Arduino.h>
#include <EEPROM.h>

//--Prototipos
void read_vars(bool ver);
void write_vars(void);
void factory_reset(void);

//--Prototipos de bajo nivel
boolean eeprom_write_string(int addr, const char* string) ;
boolean eeprom_read_string(int addr, char* buffer, int bufSize);
boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes);
boolean eeprom_is_addr_ok(int addr);
void eeprom_serial_dump_column();
bool write_StringEE(int Addr, String input);
String read_StringEE(int Addr, int length);
void check_error_updating(bool EEPROM_write_res);

#endif