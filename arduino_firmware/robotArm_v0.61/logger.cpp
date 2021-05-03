#include "logger.h"
#include "config.h"

void Logger::log(String message, int level) {
  if(LOG_LEVEL >= level) {
    String logMsg;
    switch(level) {
      case LOG_ERROR:
        logMsg = "ERROR: ";
      break;
      case LOG_INFO:
        logMsg = "INFO: ";
      break;
      case LOG_DEBUG:
        logMsg = "DEBUG: ";
      break;
    }
    logMsg = logMsg + message;
    Serial.println(logMsg);
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