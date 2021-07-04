#pragma once

#include <cstdint>

class Adafruit_Fingerprint;

namespace DeleteExample
{

    void runExampleDelete( Adafruit_Fingerprint& _fingerprintSensor,std::uint8_t _fingerId );

    void runExampleCleanDatabase( Adafruit_Fingerprint& _fingerprintSensor );
}
