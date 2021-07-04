#pragma once

#include <cstdint>

class Adafruit_Fingerprint;

namespace EnrollExample
{

    void runEnroll( Adafruit_Fingerprint& _fingerprintSensor,std::uint8_t _fingerId );
}
