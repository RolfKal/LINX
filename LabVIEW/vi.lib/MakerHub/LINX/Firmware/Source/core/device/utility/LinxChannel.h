/****************************************************************************************
**  LINX header for generic LINX channel and their generic chanel types.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_CHANNEL_H
#define LINX_CHANNEL_H

/****************************************************************************************
**  Includes
****************************************************************************************/
//This Makes It Easy For IDE Users To Define Necessary Settings In One Place
//When Using Make Files Define LINXCONFIG To Ignore Config.h File
#ifndef LINXCONFIG
	#include "../config/LinxConfig.h"
#endif
#include "LinxDefines.h"
#include "LinxBase.h"

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
#define LinxNumChannelTypes			IID_LinxServoChannel

class LinxFmtChannel;
class LinxCommChannel;

class LinxChannel : public LinxBase
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxChannel(const unsigned char *channelName);
		LinxChannel(LinxFmtChannel *debug, const unsigned char *channelName);
		virtual ~LinxChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual uint32_t GetName(unsigned char* buffer, uint8_t numBytes);
		virtual uint32_t PrintName(void);
		virtual int32_t EnableDebug(LinxCommChannel *channel);
		virtual int32_t DisableDebug(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		// virtual uint32_t SetName(unsigned char* buffer);

		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxFmtChannel *m_Debug;

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		char *m_ChannelName;
};

class LinxAnalogChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxAnalogChannel(LinxFmtChannel *debug, const unsigned char *channelName, uint8_t resolution);
		virtual ~LinxAnalogChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetResolution(uint8_t resolution);
		virtual int32_t GetResolution(uint8_t *resolution);
	protected:
		int8_t m_ResOffset;
	
	private:
		uint8_t m_Resolution;
};

class LinxAiChannel : public LinxAnalogChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxAiChannel(LinxFmtChannel *debug, const unsigned char *channelName, uint8_t resolution) : LinxAnalogChannel(debug, channelName, resolution) {};
		virtual ~LinxAiChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(uint32_t *value);
};

class LinxAoChannel : public LinxAnalogChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxAoChannel(LinxFmtChannel *debug, const unsigned char *channelName, uint8_t resolution) : LinxAnalogChannel(debug, channelName, resolution) {};
		virtual ~LinxAoChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Write(int32_t value);
};

class LinxDioChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxDioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin);
		virtual ~LinxDioChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetState(uint8_t state);		// direction and pull-up/down
		virtual int32_t Read(uint8_t *value);
		virtual int32_t Write(uint8_t value);
		virtual int32_t WriteSquareWave(uint32_t freq, uint32_t duration);
		virtual int32_t ReadPulseWidth(uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		uint16_t m_GpioChan;		// Maps LINX DIO Channel Number To GPIO Channel
		uint16_t m_LinxChan;		// Maps LINX DIO Channel Number To GPIO Channel

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t setDirection(uint8_t dir);	// direction
		virtual int32_t setPull(uint8_t pud);			// pull-up/down
		virtual int32_t setValue(uint8_t value);
		virtual int32_t getValue(uint8_t *value);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		uint8_t m_State;
		uint8_t m_Value;
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
		LinxPwmChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxPwmChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetDutyCycle(uint8_t value) = 0;
		virtual int32_t SetFrequency(uint32_t value) = 0;
};

class LinxQeChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxQeChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxQeChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(uint32_t *value) = 0;
};

class LinxCommChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxCommChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxCommChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(unsigned char* recBuffer, uint32_t numBytes, int32_t timeout, uint32_t* numBytesRead);
		virtual int32_t Read(unsigned char* recBuffer, uint32_t numBytes, uint32_t start, int32_t timeout, uint32_t* numBytesRead) = 0;
		virtual int32_t Write(const unsigned char* sendBuffer, uint32_t numBytes, int32_t timeout);
		virtual int32_t Write(const unsigned char* sendBuffer, uint32_t numBytes, uint32_t start, int32_t timeout) = 0;
		virtual int32_t Close(void) = 0;
};

class LinxUartChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUartChannel(LinxFmtChannel *debug, const unsigned char *deviceName) : LinxCommChannel(debug, deviceName) {};
		virtual ~LinxUartChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed) = 0;
		virtual int32_t SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity) = 0;
};

class LinxI2cChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxI2cChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxI2cChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Open(void) = 0;
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed) = 0;
		virtual int32_t Read(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, uint32_t timeout, unsigned char* recBuffer) = 0;
		virtual int32_t Write(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, unsigned char* sendBuffer) = 0;
		virtual int32_t Transfer(uint8_t slaveAddress, int32_t numFrames, int32_t *flags, int32_t *numBytes, uint32_t timeout, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
		virtual int32_t Close(void) = 0;
};

class LinxSpiChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSpiChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxSpiChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Open(void) = 0;
		virtual int32_t SetBitOrder(uint8_t bitOrder) = 0;
		virtual int32_t SetMode(uint8_t mode) = 0;
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed) = 0;
		virtual int32_t WriteRead(uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
		virtual int32_t Close(void)= 0;
};

class LinxCanChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxCanChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxCanChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		/* Just a placeholder currently, actual methods will need to be determined later */
		virtual int32_t Read(double *value) = 0;
		virtual int32_t Write(double *value) = 0;
};

class LinxServoChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxServoChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxServoChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetPulseWidth(uint16_t width) = 0;
		virtual int32_t Close(void) = 0;
};

// A channel that can wrap a LinxCommChannel and provide formatting functions for simple debug output
class LinxFmtChannel : public LinxBase
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxFmtChannel(int32_t timeout = TIMEOUT_INFINITE);
		LinxFmtChannel(LinxCommChannel *channel, int32_t timeout = TIMEOUT_INFINITE);
		virtual ~LinxFmtChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Write(const char s[]);
		virtual int32_t Write(const char s[], int32_t len);
		virtual int32_t Write(int8_t c);
		virtual int32_t Write(uint8_t c);
		virtual int32_t Write(int32_t n);
		virtual int32_t Write(uint32_t n);
		virtual int32_t Write(int64_t n);
		virtual int32_t Write(uint64_t n);
		virtual int32_t Write(int64_t n, int8_t base);
		virtual int32_t Writeln(void);
		virtual int32_t Writeln(const char s[]);
		virtual int32_t Writeln(const char s[], int32_t len);
		virtual int32_t Writeln(int8_t c);
		virtual int32_t Writeln(uint8_t c);
		virtual int32_t Writeln(int32_t n);
		virtual int32_t Writeln(uint32_t n);
		virtual int32_t Writeln(int64_t n);
		virtual int32_t Writeln(uint64_t n);
		virtual int32_t Writeln(int64_t n, int8_t base);
		virtual int32_t Close(void);

		int32_t SetTimeout(int32_t timeout);
		int32_t SetDebugChannel(LinxCommChannel *channel);

	protected:

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxCommChannel *m_Channel;
		int32_t m_Timeout;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int32_t WriteNumber(uint64_t n, uint8_t base);
};
#endif // LINX_CHANNEL_H