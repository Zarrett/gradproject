/***************************************************
  This is a library for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit Fingerprint sensor
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_Fingerprint.h"
#include <thread>

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Instantiates sensor with Hardware Serial
    @param  hs Pointer to HardwareSerial object
    @param  password 32-bit integer password (default is 0)

*/
/**************************************************************************/
Adafruit_Fingerprint::Adafruit_Fingerprint(
            const QString& _portName
        ,     const QString& _baudrate
        ,     uint32_t password
    )
    :    m_hComPort{nullptr}
{
  thePassword = password;
  theAddress = 0xFFFFFFFF;

  if( m_hComPort != nullptr )
      return;

  m_hComPort = new QSerialPort();

  m_hComPort->setPortName( _portName );
  m_hComPort->setBaudRate( _baudrate.toInt() );
  m_hComPort->setDataBits( QSerialPort::Data8 );
  m_hComPort->setParity( QSerialPort::NoParity );
  m_hComPort->setStopBits( QSerialPort::OneStop );
  m_hComPort->setFlowControl( QSerialPort::NoFlowControl );
}

Adafruit_Fingerprint::~Adafruit_Fingerprint()
{
    if( m_hComPort == nullptr )
        return;

    if( m_hComPort->isOpen() )
        m_hComPort->close();
    delete m_hComPort;

}

/**************************************************************************/
/*!
    @brief  Initializes serial interface and baud rate
    @param  baudrate Sensor's UART baud rate (usually 57600, 9600 or 115200)
*/
/**************************************************************************/
void Adafruit_Fingerprint::begin(const QString& _baudrate) {
  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // one second delay to let the sensor 'boot up'

    //connectReadEvent();
    m_hComPort->setBaudRate( _baudrate.toInt() );
    m_hComPort->open( QIODevice::ReadWrite );

}

void Adafruit_Fingerprint::close() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // one second delay to let the sensor 'boot up'

    //connectReadEvent();
    m_hComPort->close( );

}

/**************************************************************************/
/*!
    @brief  Verifies the sensors' access password (default password is
   0x0000000). A good way to also check if the sensors is active and responding
    @returns True if password is correct
*/
/**************************************************************************/
bool Adafruit_Fingerprint::verifyPassword(void) {
  return checkPassword() == FINGERPRINT_OK;
}

uint8_t Adafruit_Fingerprint::checkPassword(void) {
  auto packetReceived = GET_CMD_PACKET(
                FINGERPRINT_VERIFYPASSWORD
              ,     static_cast<uint8_t>(thePassword >> 24)
              ,     static_cast<uint8_t>(thePassword >> 16)
              ,     static_cast<uint8_t>(thePassword >> 8)
              ,     static_cast<uint8_t>(thePassword & 0xFF));
  if( packetReceived.has_value() )
  {
      auto packetResult = packetReceived.value();
      if (packetResult.data[0] == FINGERPRINT_OK)
        return FINGERPRINT_OK;

      return FINGERPRINT_PACKETRECIEVEERR;
  }
  else
    return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to take an image of the finger pressed on surface
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_NOFINGER</code> if no finger detected
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_IMAGEFAIL</code> on imaging error
*/
/**************************************************************************/
uint8_t Adafruit_Fingerprint::getImage(void) {
    if( auto sendCmdResult = SEND_CMD_PACKET(FINGERPRINT_GETIMAGE); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to convert image to feature template
    @param slot Location to place feature template (put one in 1 and another in
   2 for verification to create model)
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_IMAGEMESS</code> if image is too messy
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_FEATUREFAIL</code> on failure to identify
   fingerprint features
    @returns <code>FINGERPRINT_INVALIDIMAGE</code> on failure to identify
   fingerprint features
*/
uint8_t Adafruit_Fingerprint::image2Tz(uint8_t slot) {

 if( auto sendCmdResult = SEND_CMD_PACKET(FINGERPRINT_IMAGE2TZ, slot); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to take two print feature template and create a
   model
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
    @returns <code>FINGERPRINT_ENROLLMISMATCH</code> on mismatch of fingerprints
*/
uint8_t Adafruit_Fingerprint::createModel(void) {
    if( auto sendCmdResult = SEND_CMD_PACKET(FINGERPRINT_REGMODEL); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];
  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to store the calculated model for later matching
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t Adafruit_Fingerprint::storeModel(uint16_t location) {

    if( auto sendCmdResult = SEND_CMD_PACKET( FINGERPRINT_STORE
                                              ,     0x01
                                              ,     static_cast<uint8_t>(location >> 8)
                                              ,     static_cast<uint8_t>(location & 0xFF)); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to load a fingerprint model from flash into buffer 1
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t Adafruit_Fingerprint::loadModel(uint16_t location) {

    if( auto sendCmdResult = SEND_CMD_PACKET(
                FINGERPRINT_LOAD
          ,     0x01
          ,     static_cast<uint8_t>(location >> 8)
          ,     static_cast<uint8_t>(location & 0xFF) );
        sendCmdResult.has_value()
        )
    {
          return sendCmdResult.value().data[0];
    }

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to transfer 256-byte fingerprint template from the
   buffer to the UART
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t Adafruit_Fingerprint::getModel(void) {
    if( auto sendCmdResult = SEND_CMD_PACKET(FINGERPRINT_UPLOAD, 0x01); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to delete a model in memory
    @param   location The model location #
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t Adafruit_Fingerprint::deleteModel(uint16_t location) {
  if( auto sendCmdResult =  SEND_CMD_PACKET(
                    FINGERPRINT_DELETE
              ,     static_cast<uint8_t>(location >> 8)
              ,     static_cast<uint8_t>(location & 0xFF)
              ,     0x00
              ,     0x01);
        sendCmdResult.has_value()
        )
    {
        return sendCmdResult.value().data[0];
    }

  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Ask the sensor to delete ALL models in memory
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_BADLOCATION</code> if the location is invalid
    @returns <code>FINGERPRINT_FLASHERR</code> if the model couldn't be written
   to flash memory
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
uint8_t Adafruit_Fingerprint::emptyDatabase(void) {
    if( auto sendCmdResult = SEND_CMD_PACKET(FINGERPRINT_EMPTY); sendCmdResult.has_value() )
        return sendCmdResult.value().data[0];
  return FINGERPRINT_PACKETRECIEVEERR;

}

/**************************************************************************/
/*!
    @brief   Ask the sensor to search the current slot 1 fingerprint features to
   match saved templates. The matching location is stored in <b>fingerID</b> and
   the matching confidence in <b>confidence</b>
    @returns <code>FINGERPRINT_OK</code> on fingerprint match success
    @returns <code>FINGERPRINT_NOTFOUND</code> no match made
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t Adafruit_Fingerprint::fingerFastSearch(void) {
  // high speed search of slot #1 starting at page 0x0000 and page #0x00A3
  auto packetReceived = GET_CMD_PACKET(FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3);

  if( !packetReceived.has_value() )
      return FINGERPRINT_PACKETRECIEVEERR;

  auto packet = packetReceived.value();

  fingerID = 0xFFFF;
  confidence = 0xFFFF;

  fingerID = packet.data[1];
  fingerID <<= 8;
  fingerID |= packet.data[2];

  confidence = packet.data[3];
  confidence <<= 8;
  confidence |= packet.data[4];

  return packet.data[0];
}

/**************************************************************************/
/*!
    @brief   Ask the sensor for the number of templates stored in memory. The
   number is stored in <b>templateCount</b> on success.
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t Adafruit_Fingerprint::getTemplateCount(void) {
  auto packetReceived = GET_CMD_PACKET(FINGERPRINT_TEMPLATECOUNT);
  if( !packetReceived.has_value() )
      return FINGERPRINT_PACKETRECIEVEERR;

  auto packet = packetReceived.value();


  templateCount = packet.data[1];
  templateCount <<= 8;
  templateCount |= packet.data[2];

  return templateCount;
}

/**************************************************************************/
/*!
    @brief   Set the password on the sensor (future communication will require
   password verification so don't forget it!!!)
    @param   password 32-bit password code
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_PACKETRECIEVEERR</code> on communication error
*/
/**************************************************************************/
uint8_t Adafruit_Fingerprint::setPassword(std::uint32_t password) {
    if( auto sendCmdresult = SEND_CMD_PACKET(
                FINGERPRINT_SETPASSWORD
          ,     static_cast<std::uint8_t>( ( password >> 24 ))
          ,     static_cast<std::uint8_t>( ( password >> 16 ))
          ,     static_cast<std::uint8_t>( ( password >> 8 ))
          ,     static_cast<std::uint8_t>( password ) );
            sendCmdresult.has_value()
       )
    {
        return sendCmdresult.value().data[0];
    }
  return FINGERPRINT_PACKETRECIEVEERR;
}

/**************************************************************************/
/*!
    @brief   Helper function to process a packet and send it over UART to the
   sensor
    @param   packet A structure containing the bytes to transmit
*/
/**************************************************************************/

void Adafruit_Fingerprint::writeStructuredPacket(
    const Adafruit_Fingerprint_Packet &packet) {
  SERIAL_WRITE_U16(packet.start_code);
  SERIAL_WRITE(packet.address[0]);
  SERIAL_WRITE(packet.address[1]);
  SERIAL_WRITE(packet.address[2]);
  SERIAL_WRITE(packet.address[3]);
  SERIAL_WRITE(packet.type);

  uint16_t wire_length = packet.length + 2;
  SERIAL_WRITE_U16(wire_length);

  uint16_t sum = ((wire_length) >> 8) + ((wire_length)&0xFF) + packet.type;
  for (uint8_t i = 0; i < packet.length; i++) {
    SERIAL_WRITE(packet.data[i]);
    sum += packet.data[i];
  }

  SERIAL_WRITE_U16(sum);
  return;
}

/**************************************************************************/
/*!
    @brief   Helper function to receive data over UART from the sensor and
   process it into a packet
    @param   packet A structure containing the bytes received
    @param   timeout how many milliseconds we're willing to wait
    @returns <code>FINGERPRINT_OK</code> on success
    @returns <code>FINGERPRINT_TIMEOUT</code> or
   <code>FINGERPRINT_BADPACKET</code> on failure
*/
/**************************************************************************/
uint8_t
Adafruit_Fingerprint::getStructuredPacket(Adafruit_Fingerprint_Packet *packet,
                                          uint16_t timeout) {
  uint8_t byte;
  uint16_t idx = 0;

  while (true) {
    while (!m_hComPort->waitForReadyRead(timeout))
    {
        return FINGERPRINT_TIMEOUT;
    }
    QByteArray responseData = m_hComPort->readAll();

    while (m_hComPort->waitForReadyRead(10))

     responseData += m_hComPort->readAll();

    QByteArray readPacket = responseData;

       if( static_cast<std::uint8_t>(readPacket[0]) == (FINGERPRINT_STARTCODE >> 8 ) )
       {
           packet->start_code = static_cast<std::uint16_t>( readPacket[0] << 8 );
       }
       packet->start_code |= readPacket[1];

       if (packet->start_code != FINGERPRINT_STARTCODE)
         return FINGERPRINT_BADPACKET;

       packet->address[0] = readPacket[2];
       packet->address[1] = readPacket[3];
       packet->address[2] = readPacket[4];
       packet->address[3] = readPacket[5];
       packet->type = readPacket[6];
       packet->length = static_cast<uint16_t>( readPacket[7] << 8);
       packet->length |= readPacket[8];

       const size_t packetSize = packet->length;

       for( size_t packetDataIndex = 0; packetDataIndex< packetSize; ++packetDataIndex )
       {
         packet->data[packetDataIndex] = readPacket[packetDataIndex +9];
       }
       return FINGERPRINT_OK;
     }
     // Shouldn't get here so...
     return FINGERPRINT_BADPACKET;
}


void
Adafruit_Fingerprint::readData()
{
    QByteArray serialData = m_hComPort->readAll();
}


void
Adafruit_Fingerprint::connectReadEvent() noexcept
{
    connect(
            m_hComPort
        ,   &QSerialPort::readyRead
        ,   this
        ,   &Adafruit_Fingerprint::readData
    );
}
