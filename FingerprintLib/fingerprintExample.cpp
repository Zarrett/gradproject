#include "fingerprintExample.hpp"
#include "Adafruit_Fingerprint.h"

#include "Logger.hpp"
#include <thread>

namespace
{

uint8_t getFingerprintID( Adafruit_Fingerprint& _fingerprintSensor )
{
  uint8_t p = _fingerprintSensor.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Logger::Instance().logDebugEndl("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Logger::Instance().logDebugEndl("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Logger::Instance().logDebugEndl("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Logger::Instance().logDebugEndl("Imaging error");
      return p;
    default:
      Logger::Instance().logDebugEndl("Unknown error");
      return p;
  }

  // OK success!

  p = _fingerprintSensor.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Logger::Instance().logDebugEndl("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Logger::Instance().logDebugEndl("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Logger::Instance().logDebugEndl("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Logger::Instance().logDebugEndl("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Logger::Instance().logDebugEndl("Could not find fingerprint features");
      return p;
    default:
      Logger::Instance().logDebugEndl("Unknown error");
      return p;
  }

  // OK converted!
  p = _fingerprintSensor.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Logger::Instance().logDebugEndl("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Logger::Instance().logDebugEndl("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Logger::Instance().logDebugEndl("Did not find a match");
    return p;
  } else {
    Logger::Instance().logDebugEndl("Unknown error");
    return p;
  }

  // found a match!
  Logger::Instance().logDebug("Found ID #"); Logger::Instance().logDebug(_fingerprintSensor.fingerID);
  Logger::Instance().logDebug(" with confidence of "); Logger::Instance().logDebugEndl(_fingerprintSensor.confidence);

  return _fingerprintSensor.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez( Adafruit_Fingerprint& _fingerprintSensor ) {
  uint8_t p = _fingerprintSensor.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = _fingerprintSensor.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = _fingerprintSensor.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Logger::Instance().logDebug("Found ID #"); Logger::Instance().logDebug(_fingerprintSensor.fingerID);
  Logger::Instance().logDebug(" with confidence of "); Logger::Instance().logDebugEndl(_fingerprintSensor.confidence);
  return _fingerprintSensor.fingerID;
}
}
namespace FingerprintExample
{

int runFind( Adafruit_Fingerprint& _fingerprintSensor )
{
    _fingerprintSensor.getTemplateCount();
    Logger::Instance().logDebugEndl("Sensor contains ");
    Logger::Instance().logDebug( _fingerprintSensor.templateCount );
    Logger::Instance().logDebug(" templates");
    Logger::Instance().logDebugEndl("Waiting for valid _fingerprintSensor...");

    int res = -1;

    using namespace  std::chrono_literals;
    while(res == -1)
    {
        res = getFingerprintIDez( _fingerprintSensor );
        std::this_thread::sleep_for(50ms);
    }

    return res;
}

}
