/****************************************************************************************
**  LINX header for generic LINX Device.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_DEVICE_H
#define LINX_DEVICE_H

/****************************************************************************************
**  Includes
****************************************************************************************/
//This Makes It Easy For IDE Users To Define Necessary Settings In One Place
//When Using Make Files Define LINXCONFIG To Ignore Config.h File
#ifndef LINXCONFIG
	#include "../config/LinxConfig.h"
#endif
#include <map>

/****************************************************************************************
**  Defines
****************************************************************************************/
//GPIO

//SPI
#ifndef LSBFIRST
	#define LSBFIRST 0
#endif

#ifndef MSBFIRST
	#define MSBFIRST 1
#endif

//I2C
#define EOF_STOP 0
#define EOF_RESTART 1
#define EOF_RESTART_NOSTOP 2
#define EOF_NOSTOP 3

//UART
#ifndef BIN
	#define BIN 2
#endif
#ifndef OCT
	#define OCT 8
#endif
#ifndef DEC
	#define DEC 10
#endif
#ifndef HEX
	#define HEX 16
#endif

//Non-Volatile Storage Addresses
#define NVS_USERID 0x00
#define NVS_ETHERNET_IP 0x02
#define NVS_ETHERNET_PORT 0x06
#define NVS_WIFI_IP 0x08
#define NVS_WIFI_PORT 0x0C
#define NVS_WIFI_SSID_SIZE 0x0E
#define NVS_WIFI_SSID 0x0F
#define NVS_WIFI_SECURITY_TYPE 0x30
#define NVS_WIFI_PW_SIZE 0x31
#define NVS_WIFI_PW 0x32
#define NVS_SERIAL_INTERFACE_MAX_BAUD 0x72

//DEBUG
#define TX 0
#define RX 1

/****************************************************************************************
**  Typedefs
****************************************************************************************/
typedef enum LinxStatus
{
	L_OK = 0,
	L_FUNCTION_NOT_SUPPORTED,
	L_REQUEST_RESEND,
	L_UNKNOWN_ERROR,
	L_DISCONNECT,
	LERR_RUNNING,
	LERR_BADPARAM,
	LERR_IO,
	LERR_PACKET_NUM,
	LERR_CHECKSUM,
	LERR_INVALID_FRAME,
	LERR_LENGTH_NOT_SUPPORTED,
	LERR_MSG_TO_LONG,
	LERR_CLOSED_BY_PEER,
} LinxStatus;

typedef enum AioStatus
{
	LANALOG_REF_MODE_ERROR = 129,
	LANALOG_REF_VAL_ERROR = 130
} AioStatus;

typedef enum DioStatus
{
	LDIGITAL_PIN_DNE = 128,
} DioStatus;


typedef enum SPIStatus
{
	LSPI_OPEN_FAIL = 128,
	LSPI_TRANSFER_FAIL,
	LSPI_CLOSE_FAIL,
	LSPI_DEVICE_NOT_OPEN,
} SPIStatus;

typedef enum I2CStatus
{
	LI2C_SADDR = 128,
	LI2C_EOF,
	LI2C_WRITE_FAIL,
	LI2C_READ_FAIL,
	LI2C_CLOSE_FAIL,
	LI2C_OPEN_FAIL,
	LI2C_DEVICE_NOT_OPEN,
} I2CStatus;

typedef enum UartStatus
{
	LUART_OPEN_FAIL=128,
	LUART_SET_BAUD_FAIL,
	LUART_AVAILABLE_FAIL,
	LUART_READ_FAIL,
	LUART_WRITE_FAIL,
	LUART_CLOSE_FAIL
} UartStatus;

typedef enum LinxUartParity
{
	None,
	Even,
	Odd
} LinxUartParity;

class LinxDevice
{
	public:
		/****************************************************************************************
		**  Member Variables
		****************************************************************************************/

		//LINX API Version
		unsigned char LinxApiMajor;
		unsigned char LinxApiMinor;
		unsigned char LinxApiSubminor;

		//Device ID
		unsigned char DeviceFamily;
		unsigned char DeviceId;

		//----Peripherals----

		//DIO

		//AI
		unsigned char AiResolution;
		unsigned int AiRefDefault;
		unsigned int AiRefSet;

		//AO
		unsigned char AoResolution;
		unsigned int AoRefDefault;
		unsigned int AoRefSet;

		//PWM

		//QE

		//UART
		unsigned int UartMaxBaud;

		//I2C

		//SPI

		//CAN

		//Servo

		//User Configured Values
		unsigned short userId;

		unsigned int ethernetIp;
		unsigned short ethernetPort;

		unsigned int WifiIp;
		unsigned short WifiPort;
		unsigned char WifiSsidSize;
		char WifiSsid[32];
		unsigned char WifiSecurity;
		unsigned char WifiPwSize;
		char WifiPw[64];

		unsigned int serialInterfaceMaxBaud;

		/****************************************************************************************
		**  Constructors/Destructor
		****************************************************************************************/
		LinxDevice();
		virtual ~LinxDevice();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetAiChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetAoChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetDioChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetQeChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetPwmChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetSpiChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetI2cChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetUartChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetCanChans(unsigned char *buffer, unsigned char length) = 0;
		virtual unsigned char GetServoChans(unsigned char *buffer, unsigned char length) = 0;

		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);		//Values Are ADC Ticks And Not Bit Packed
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage) = 0;
		virtual int AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);

		//DIGITAL
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;			//Values Are Bit Packed
		virtual int DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);		//Values Are Not Bit Packed
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;
		virtual int DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);		//Response Not Bit Packed
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration) = 0;
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width) = 0;

		//QE

		//PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;
		virtual int PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned int* values);

		//SPI
		virtual int SpiOpenMaster(unsigned char channel) = 0;
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder) = 0;
		virtual int SpiSetMode(unsigned char channel, unsigned char mode) = 0;
		virtual int SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
		virtual int SpiCloseMaster(unsigned char channel);

		//I2C
		virtual int I2cOpenMaster(unsigned char channel) = 0;
		virtual int I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer) = 0;
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer) = 0;
		virtual int I2cClose(unsigned char channel) = 0;

		//UART
		virtual int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud) = 0;
		virtual int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
		virtual int UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud) = 0;
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes) = 0;
		virtual int UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead) = 0;
		virtual int UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer) = 0;

		virtual int UartWrite(unsigned char channel, char c);
		virtual int UartWrite(unsigned char channel, const char s[]);
		virtual int UartWrite(unsigned char channel, unsigned char c);
		virtual int UartWrite(unsigned char channel, int n);
		virtual int UartWrite(unsigned char channel, unsigned int n);
		virtual int UartWrite(unsigned char channel, long n);
		virtual int UartWrite(unsigned char channel, unsigned long n);
		virtual int UartWrite(unsigned char channel, long n, int base);
		virtual int UartWriteln(unsigned char channel);
		virtual int UartWriteln(unsigned char channel, char c);
		virtual int UartWriteln(unsigned char channel, const char s[]);
		virtual int UartWriteln(unsigned char channel, unsigned char c);
		virtual int UartWriteln(unsigned char channel, int n);
		virtual int UartWriteln(unsigned char channel, long n);
		virtual int UartWriteln(unsigned char channel, unsigned long n);
		virtual int UartWriteln(unsigned char channel, long n, int base);
		virtual int UartClose(unsigned char channel) = 0;

		//CAN

		//Servo
		virtual int ServoOpen(unsigned char numChans, unsigned char* channels) = 0;
		virtual int ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths) = 0;
		virtual int ServoClose(unsigned char numChans, unsigned char* channels) = 0;

		//WS2812
		virtual int Ws2812Open(unsigned short numLeds, unsigned char dataChan);
		virtual int Ws2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh);
		virtual int Ws2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh);
		virtual int Ws2812Refresh();
		virtual int Ws2812Close();

		//General
		virtual unsigned int GetMilliSeconds() = 0;
		virtual unsigned int GetSeconds() = 0;
		virtual void DelayMs(unsigned int ms);
		virtual void NonVolatileWrite(int address, unsigned char data) = 0;
		virtual unsigned char NonVolatileRead(int address) = 0;
		virtual bool ChecksumPassed(unsigned char* buffer, int length);
		virtual unsigned char ComputeChecksum(unsigned char* buffer, int length);

		//Debug
		virtual int EnableDebug(unsigned char channel);
		virtual int EnableDebug(unsigned char channel, unsigned int baudRate);

		virtual void DebugPrint(char c);
		virtual void DebugPrint(const char s[]);
		virtual void DebugPrint(unsigned char c);
		virtual void DebugPrint(int n);
		virtual void DebugPrint(unsigned int n);
		virtual void DebugPrint(long n);
		virtual void DebugPrint(unsigned long n);
		virtual void DebugPrint(long n, int base);

		virtual void DebugPrintln();
		virtual void DebugPrintln(char c);
		virtual void DebugPrintln(const char s[]);
		virtual void DebugPrintln(unsigned char c);
		virtual void DebugPrintln(int n);
		virtual void DebugPrintln(long n);
		virtual void DebugPrintln(unsigned long n);
		virtual void DebugPrintln(long n, int base);

		virtual void DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char ReverseBits(unsigned char b);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		int m_DebugChannnel;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int UartWriteNumber(unsigned char channel, unsigned long n, unsigned char bases);
};
#endif //LINX_DEVICE_H
