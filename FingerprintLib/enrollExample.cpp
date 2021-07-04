#include "enrollExample.hpp"
#include "Adafruit_Fingerprint.h"

#include "Logger.hpp"
#include <thread>

namespace
{

uint8_t getFingerprintEnroll(
        Adafruit_Fingerprint& _fingerprintSensor
     ,  std::uint8_t _fingerID
    )
{

  int p = -1;
  Logger::Instance().logDebug("Waiting for valid finger to enroll as #"); Logger::Instance().logDebugEndl(_fingerID);
  while (p != FINGERPRINT_OK) {
    p = _fingerprintSensor.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Logger::Instance().logDebugEndl("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Logger::Instance().logDebugEndl(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Logger::Instance().logDebugEndl("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Logger::Instance().logDebugEndl("Imaging error");
      break;
    default:
      Logger::Instance().logDebugEndl("Unknown error");
      break;
    }
  }

  // OK success!

  p = _fingerprintSensor.image2Tz(1);
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

  using namespace std::chrono_literals;

  Logger::Instance().logDebugEndl("Remove finger");
  std::this_thread::sleep_for(2000ms);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = _fingerprintSensor.getImage();
  }
  Logger::Instance().logDebug("ID "); Logger::Instance().logDebugEndl(_fingerID);
  p = -1;
  Logger::Instance().logDebugEndl("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = _fingerprintSensor.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Logger::Instance().logDebugEndl("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Logger::Instance().logDebug(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Logger::Instance().logDebugEndl("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Logger::Instance().logDebugEndl("Imaging error");
      break;
    default:
      Logger::Instance().logDebugEndl("Unknown error");
      break;
    }
  }

  // OK success!

  p = _fingerprintSensor.image2Tz(2);
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
  Logger::Instance().logDebug("Creating model for #");  Logger::Instance().logDebugEndl(_fingerID);

  p = _fingerprintSensor.createModel();
  if (p == FINGERPRINT_OK) {
    Logger::Instance().logDebugEndl("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Logger::Instance().logDebugEndl("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Logger::Instance().logDebugEndl("Fingerprints did not match");
    return p;
  } else {
    Logger::Instance().logDebugEndl("Unknown error");
    return p;
  }

  Logger::Instance().logDebug("ID "); Logger::Instance().logDebugEndl(_fingerID);
  p = _fingerprintSensor.storeModel(_fingerID);
  if (p == FINGERPRINT_OK) {
    Logger::Instance().logDebugEndl("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Logger::Instance().logDebugEndl("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Logger::Instance().logDebugEndl("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Logger::Instance().logDebugEndl("Error writing to flash");
    return p;
  } else {
    Logger::Instance().logDebugEndl("Unknown error");
    return p;
  }
    return p;
}
}
namespace EnrollExample
{

void runEnroll( Adafruit_Fingerprint& _fingerprintSensor,std::uint8_t _fingerId )
{
    Logger::Instance().logDebugEndl("Ready to enroll a fingerprint!");
    Logger::Instance().logDebugEndl("Please type in the ID # (from 1 to 127) you want to save this finger as...");

    static const std::uint8_t FingerId = _fingerId;

    Logger::Instance().logDebug("Enrolling ID #");
    Logger::Instance().logDebugEndl(FingerId);

    using namespace  std::chrono_literals;
    while ( getFingerprintEnroll( _fingerprintSensor, FingerId ) != 0x00);

}

}
