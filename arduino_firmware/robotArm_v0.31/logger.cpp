#include "logger.h"
#include "config.h"

void Logger::log(String message, int level) {
  if(LOG_LEVEL >= level) {
    switch(level) {
      case LOG_ERROR:
        Serial.print("ERROR: ");
      break;
      case LOG_INFO:
        Serial.print("INFO: ");
      break;
      case LOG_DEBUG:
        Serial.print("DEBUG: ");
      break;
    }
    Serial.println(message);
  }
}
void Logger::logERROR(String message) {
  log(message, LOG_ERROR);
}
void Logger::logINFO(String message) {
  log(message, LOG_INFO);
}
void Logger::logDEBUG(String message) {
  log(message, LOG_DEBUG);
}