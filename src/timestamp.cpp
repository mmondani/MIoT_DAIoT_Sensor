#include "timestamp.h"

//--Variables externas
extern const char* ntpServer;

unsigned long getTime(void) {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}