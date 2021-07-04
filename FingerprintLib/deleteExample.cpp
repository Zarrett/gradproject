#include "enrollExample.hpp"
#include "Adafruit_Fingerprint.h"

#include "Logger.hpp"
#include <thread>

namespace
{
uint8_t deleteFingerprint(Adafruit_Fingerprint& _fingerprintSensor,uint8_t id)
{
  uint8_t p = -1;

  p = _fingerprintSensor.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Logger::Instance().logDebugEndl("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Logger::Instance().logDebugEndl("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Logger::Instance().logDebugEndl("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Logger::Instance().logDebugEndl("Error writing to flash");
    return p;
  } else {
    Logger::Instance().logDebug("Unknown error: 0x"); Logger::Instance().logDebugEndl(p);
    return p;
  }
  return p;
}


void printTemplatesCount(Adafruit_Fingerprint& _fingerprintSensor)
{
    _fingerprintSensor.getTemplateCount();
    Logger::Instance().logDebugEndl("Sensor contains ");
    Logger::Instance().logDebug( _fingerprintSensor.templateCount );
    Logger::Instance().logDebugEndl(" templates");
}

}
namespace DeleteExample
{

void runExampleDelete( Adafruit_Fingerprint& _fingerprintSensor,std::uint8_t _fingerId )
{
    printTemplatesCount( _fingerprintSensor );

    Logger::Instance().logDebug("Deleting ID #");
    Logger::Instance().logDebugEndl(_fingerId);

    deleteFingerprint(_fingerprintSensor,_fingerId);

    printTemplatesCount( _fingerprintSensor );

}

void runExampleCleanDatabase( Adafruit_Fingerprint& _fingerprintSensor )
{
    printTemplatesCount( _fingerprintSensor );
    _fingerprintSensor.emptyDatabase();
    Logger::Instance().logDebugEndl("Now database is empty :)");
    printTemplatesCount( _fingerprintSensor );
}

}
