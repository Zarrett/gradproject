#include <QCoreApplication>
#include "Adafruit_Fingerprint.h"

#include "enrollExample.hpp"
#include "fingerprintExample.hpp"
#include "deleteExample.hpp"

static const QString FingerprintSerial = "/dev/serial0";
static const QString FingerPrinttBaudrate = "57600";

Adafruit_Fingerprint finger{
        FingerprintSerial
    ,   FingerPrinttBaudrate
};


void systemSetup()
{
    // set the data rate for the sensor serial port
    finger.begin(FingerPrinttBaudrate);
}


int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    systemSetup();

    static constexpr std::uint8_t FingerprintId = 25;
    //EnrollExample::runExample( finger,FingerprintId );

    //DeleteExample::runExampleDelete( finger, FingerprintId );
    //DeleteExample::runExampleCleanDatabase( finger );
    FingerprintExample::runExample( finger );
    return 0;

}
