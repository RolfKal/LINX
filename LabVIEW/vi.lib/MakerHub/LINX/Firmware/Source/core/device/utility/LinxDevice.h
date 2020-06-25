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
// GPIO Values
#define GPIO_LOW		0
#define GPIO_HIGH		1

#define GPIO_INPUT		0x00
#define GPIO_OUTPUT		0x01
#define GPIO_ALT0		0x04
#define GPIO_IOMASK		0x07
#define GPIO_DIRMASK	0x07

#define GPIO_PULLNONE	0x00
#define GPIO_PULLDOWN	0x10
#define GPIO_PULLUP		0x20
#define GPIO_PULLOFF	0x30
#define GPIO_PULLMASK	0x30

// SPI
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

#define TIMEOUT_INFINITE -1

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

#define UNDEFINED_CHANNEL 0xFF

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
	LERR_MEMORY,
	LERR_BADPARAM,
	LERR_BADCHAN,
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
	LUART_CLOSE_FAIL,
	LUART_TIMEOUT
} UartStatus;

typedef enum LinxUartParity
{
	Ignore,
	None,
	Odd,
	Even,
	Mark,
	Space,
} LinxUartParity;

#define IID_LinxChannel				0
#define IID_LinxAiChannel			1
#define IID_LinxAoChannel			2
#define IID_LinxDioChannel			3
#define IID_LinxPwmChannel			4
#define IID_LinxQeChannel			5
#define IID_LinxUartChannel			6
#define IID_LinxI2cChannel			7
#define IID_LinxSpiChannel			8
#define IID_LinxCanChannel			9
#define IID_LinxServoChannel		10

#define IID_LinxSysfsAiChannel		20
#define IID_LinxSysfsAoChannel		21

#define IID_LinxSysfsDioChannel		30
#define IID_LinxRaspiDioChannel		31

#define IID_LinxCommChannel			60
#define IID_LinxFmtChannel			61
#define IID_LinxUnixUartChannel		62
#define IID_LinxWinUartChannel		63

#define IID_LinxSysfsI2cChannel		70

#define IID_LinxSysfsSpiChannel		80

#define LinxNumChanelTypes			IID_LinxServoChannel

class LinxFmtChannel;

class LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxChannel(const char *channelName, LinxFmtChannel *debug);
		virtual ~LinxChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned int AddRef();
		virtual unsigned int Release();
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int GetName(char* buffer, unsigned char numBytes);

	protected:
		const char *m_ChannelName;
		LinxFmtChannel *m_Debug;

	private:
		unsigned int m_Refcount;
};

class LinxAiChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxAiChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};


		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Read(unsigned int *value) = 0;
};

class LinxAoChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxAoChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Write(int value) = 0;
};

class LinxDioChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxDioChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int SetState(unsigned char state) = 0;		// direction and pull-up/down
		virtual int Read(unsigned char *value) = 0;
		virtual int Write(unsigned char value) = 0;
		virtual int WriteSquareWave(unsigned int freq, unsigned int duration) = 0;
		virtual int ReadPulseWidth(unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width) = 0;
};

class LinxPwmChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxPwmChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int SetDutyCycle(unsigned char value) = 0;
		virtual int SetFrequency(unsigned int value) = 0;
};

class LinxQeChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxQeChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Read(unsigned int *value) = 0;
};

class LinxCommChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxCommChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead) = 0;
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout) = 0;
		virtual int Close() = 0;
};

class LinxUartChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUartChannel(const char *channelName, LinxFmtChannel *debug) : LinxCommChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int SetBitSizes(unsigned char dataBits, unsigned char stopBits) = 0;
		virtual int SetParity(LinxUartParity parity) = 0;
		virtual int GetBytesAvail(int* numBytesAvailable) = 0;
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead) = 0;
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout) = 0;
		virtual int Close() = 0;
};

class LinxI2cChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxI2cChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Open() = 0;
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer) = 0;
		virtual int Write(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned char* sendBuffer) = 0;
		virtual int Close() = 0;
};

class LinxSpiChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSpiChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Open() = 0;
		virtual int SetBitOrder(unsigned char bitOrder) = 0;
		virtual int SetMode(unsigned char mode) = 0;
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int WriteRead(unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
		virtual int Close()= 0;
};

class LinxCanChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxCanChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Read(double *value) = 0;
		virtual int Write(double *value) = 0;
};

class LinxServoChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxServoChannel(const char *channelName, LinxFmtChannel *debug) : LinxChannel(channelName, debug) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId) = 0;

		virtual int Read(double *value) = 0;
		virtual int Write(double *value) = 0;
};

// A channel that can wrap a LinxCommChannel and provide formatting functions for easy debug output
class LinxFmtChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxFmtChannel();
		LinxFmtChannel(LinxCommChannel *channel);
		~LinxFmtChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int GetName(char* buffer, unsigned char numBytes);
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Write(char c);
		virtual int Write(const char s[]);
		virtual int Write(unsigned char c);
		virtual int Write(int n);
		virtual int Write(unsigned int n);
		virtual int Write(long n);
		virtual int Write(unsigned long n);
		virtual int Write(long n, int base);
		virtual int Writeln();
		virtual int Writeln(char c);
		virtual int Writeln(const char s[]);
		virtual int Writeln(unsigned char c);
		virtual int Writeln(int n);
		virtual int Writeln(long n);
		virtual int Writeln(unsigned long n);
		virtual int Writeln(long n, int base);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxFmtChannel *m_Debug;

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxCommChannel *m_Channel;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int WriteNumber(unsigned long n, unsigned char base);
};

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

		/****************************************************************************************
		**  Constructors/Destructor
		****************************************************************************************/
		LinxDevice();
		virtual ~LinxDevice();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		// System Support
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length) = 0;

		// Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);		//Values Are ADC Ticks And Not Bit Packed
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage);
		virtual int AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);

		// DIGITAL
		virtual int DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char *states);		// direction and pull-up/down
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;		// Values Are Bit Packed
		virtual int DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Values Are Not Bit Packed
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values) = 0;
		virtual int DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Response Not Bit Packed
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration) = 0;
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width) = 0;

		// QE

		// PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned int* values);

		// SPI
		virtual int SpiOpenMaster(unsigned char channel) = 0;
		virtual int SpiOpenMaster(const char *deviceName, unsigned char channel);
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
		virtual int SpiSetMode(unsigned char channel, unsigned char mode);
		virtual int SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int SpiCloseMaster(unsigned char channel);

		// I2C
		virtual int I2cOpenMaster(unsigned char channel) = 0;
		virtual int I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);
		virtual int I2cClose(unsigned char channel);

		// UART
		virtual int UartOpen(unsigned char channel, LinxUartChannel **chan = NULL);
		virtual int UartOpen(const char *deviceName, unsigned char nameLength, unsigned char *channel, LinxUartChannel **chan = NULL) = 0;
		virtual int UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
		virtual int UartSetBitSizes(unsigned char channel, unsigned char dataBits, unsigned char stopBits);
		virtual int UartSetParity(unsigned char channel, LinxUartParity parity);
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes);
		virtual int UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, int timeout, unsigned char* numBytesRead);
		virtual int UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer, int timeout);
		virtual int UartClose(unsigned char channel);

		// CAN

		// Servo
		virtual int ServoOpen(unsigned char numChans, unsigned char* channels) = 0;
		virtual int ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths);
		virtual int ServoClose(unsigned char numChans, unsigned char* channels);

		// WS2812
		virtual int Ws2812Open(unsigned short numLeds, unsigned char dataChan);
		virtual int Ws2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh);
		virtual int Ws2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh);
		virtual int Ws2812Refresh();
		virtual int Ws2812Close();

		// General
		virtual void NonVolatileWrite(int address, unsigned char data) = 0;
		virtual unsigned char NonVolatileRead(int address) = 0;

		virtual unsigned int GetMilliSeconds();
		virtual unsigned int GetSeconds();
		virtual void DelayMs(unsigned int ms);

		virtual unsigned char ReverseBits(unsigned char b);

		virtual bool ChecksumPassed(unsigned char* buffer, int length);
		virtual unsigned char ComputeChecksum(unsigned char* buffer, int length);

		unsigned char EnumerateChannels(int type, unsigned char *buffer, unsigned char length);


		// Debug
		virtual int EnableDebug(LinxCommChannel *channel);

		virtual void DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxFmtChannel *m_Debug;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		unsigned char RegisterChannel(int type, LinxChannel *chan);
		LinxChannel* LookupChannel(int type, unsigned char channel);
		void RegisterChannel(int type, unsigned char channel, LinxChannel *chan);
		void RemoveChannel(int type, unsigned char channel);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		std::map<unsigned char, LinxChannel*> m_ChannelRegistry[LinxNumChanelTypes];

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual void ClearChannels(int type);
};
#endif //LINX_DEVICE_H
