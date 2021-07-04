#pragma once

#include <string>
#include <cstdint>
#include <stdio.h>
#include <string.h>
#include <optional>
#include <QtCore/QDebug>

#define FINGERPRINT_OK 0x00
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_NOMATCH 0x08
#define FINGERPRINT_NOTFOUND 0x09
#define FINGERPRINT_ENROLLMISMATCH 0x0A
#define FINGERPRINT_BADLOCATION 0x0B
#define FINGERPRINT_DBRANGEFAIL 0x0C
#define FINGERPRINT_UPLOADFEATUREFAIL 0x0D
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E
#define FINGERPRINT_UPLOADFAIL 0x0F
#define FINGERPRINT_DELETEFAIL 0x10
#define FINGERPRINT_DBCLEARFAIL 0x11
#define FINGERPRINT_PASSFAIL 0x13
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_FLASHERR 0x18
#define FINGERPRINT_INVALIDREG 0x1A
#define FINGERPRINT_ADDRCODE 0x20
#define FINGERPRINT_PASSVERIFY 0x21

#define FINGERPRINT_STARTCODE 0xEF01

#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_DATAPACKET 0x2
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_ENDDATAPACKET 0x8

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

#define FINGERPRINT_GETIMAGE 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_REGMODEL 0x05
#define FINGERPRINT_STORE 0x06
#define FINGERPRINT_LOAD 0x07
#define FINGERPRINT_UPLOAD 0x08
#define FINGERPRINT_DELETE 0x0C
#define FINGERPRINT_EMPTY 0x0D
#define FINGERPRINT_SETPASSWORD 0x12
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_HISPEEDSEARCH 0x1B
#define FINGERPRINT_TEMPLATECOUNT 0x1D

//#define FINGERPRINT_DEBUG

#define DEFAULTTIMEOUT 1000 ///< UART reading timeout in milliseconds

#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <array>


///! Helper class to craft UART packets
struct Adafruit_Fingerprint_Packet {

  /**************************************************************************/
  /*!
      @brief   Create a new UART-borne packet
      @param   type Command, data, ack type packet
      @param   length Size of payload
      @param   data Pointer to bytes of size length we will memcopy into the
     internal buffer
  */
  /**************************************************************************/

    Adafruit_Fingerprint_Packet(std::uint8_t type, std::uint16_t length, std::uint8_t *data) {
    this->start_code = FINGERPRINT_STARTCODE;
    this->type = type;
    this->length = length;
    address[0] = 0xFF;
    address[1] = 0xFF;
    address[2] = 0xFF;
    address[3] = 0xFF;
    if (length < 64)
      memcpy(this->data, data, length);
    else
      memcpy(this->data, data, 64);
  }
  std::uint16_t start_code; ///< "Wakeup" code for packet detection
  std::uint8_t address[4];  ///< 32-bit Fingerprint sensor address
  std::uint8_t type;        ///< Type of packet
  std::uint16_t length;     ///< Length of packet
  std::uint8_t data[64];    ///< The raw buffer for packet payload
};

///! Helper class to communicate with and keep state for fingerprint sensors
class Adafruit_Fingerprint
         :  public QObject
{

     Q_OBJECT

public:
  Adafruit_Fingerprint(
                const QString& _portName
          ,     const QString& _baudrate
          ,     uint32_t password = 0x0
          );

  ~Adafruit_Fingerprint();

  void begin(const QString& _baudrate);
  void close();

  bool verifyPassword(void);
  std::uint8_t getImage(void);
  std::uint8_t image2Tz(std::uint8_t slot = 1);
  std::uint8_t createModel(void);

  std::uint8_t emptyDatabase(void);
  std::uint8_t storeModel(std::uint16_t id);
  std::uint8_t loadModel(std::uint16_t id);
  std::uint8_t getModel(void);
  std::uint8_t deleteModel(std::uint16_t id);
  std::uint8_t fingerFastSearch(void);
  std::uint8_t getTemplateCount(void);
  std::uint8_t setPassword(uint32_t password);
  void writeStructuredPacket(const Adafruit_Fingerprint_Packet &p);
  std::uint8_t getStructuredPacket(Adafruit_Fingerprint_Packet *p,
                              std::uint16_t timeout = DEFAULTTIMEOUT);

  /// The matching location that is set by fingerFastSearch()
  std::uint16_t fingerID;
  /// The confidence of the fingerFastSearch() match, higher numbers are more
  /// confidents
  std::uint16_t confidence;
  /// The number of stored templates in the sensor, set by getTemplateCount()
  std::uint16_t templateCount;

private slots:

    void readData();

private:

    void connectReadEvent() noexcept;

  template< typename ... Args >
  void SERIAL_WRITE(Args ... _argsToWrite )
  {
     std::array chunk = { _argsToWrite... };
     if(m_hComPort )
     {
         m_hComPort->write( reinterpret_cast<char*>(chunk.data()), chunk.size() );

         //m_hComPort->waitForBytesWritten( 10 );
     }

  }

  void SERIAL_WRITE_U16( std::uint16_t _toWrite )
  {
      SERIAL_WRITE(static_cast<uint8_t>(_toWrite >> 8));
      SERIAL_WRITE(static_cast<uint8_t>(_toWrite & 0xFF));
  }

  template< typename ... Args >
  std::optional<Adafruit_Fingerprint_Packet> GET_CMD_PACKET(Args ... _argsToWrite )
  {
      std::array dataToWrite = {static_cast<std::uint8_t>(_argsToWrite)...};
      Adafruit_Fingerprint_Packet packet(FINGERPRINT_COMMANDPACKET, dataToWrite.size(), dataToWrite.data());

      writeStructuredPacket(packet);
      if (getStructuredPacket(&packet) != FINGERPRINT_OK)
        return std::nullopt;
      if (packet.type != FINGERPRINT_ACKPACKET)
        return std::nullopt;
      return packet;
  }

  template< typename ... Args >
  std::optional<Adafruit_Fingerprint_Packet> SEND_CMD_PACKET(Args ... _argsToWrite)
  {
        return GET_CMD_PACKET(_argsToWrite ...);;
  }


private:
  std::uint8_t checkPassword(void);
  uint32_t thePassword;
  uint32_t theAddress;

  QSerialPort* m_hComPort;
};

