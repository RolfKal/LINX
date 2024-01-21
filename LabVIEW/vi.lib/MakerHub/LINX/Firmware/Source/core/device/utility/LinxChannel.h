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

// Forward declaration
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
		LinxChannel(const unsigned char *channelName);
		LinxChannel(LinxFmtChannel *debug, const unsigned char *channelName);
		virtual ~LinxChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned int AddRef();
		virtual unsigned int Release();

		virtual unsigned char GetName(unsigned char* buffer, unsigned char numBytes);

	protected:
		char *m_ChannelName;
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
		LinxAiChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxAiChannel() {};


		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxAoChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxAoChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxDioChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxDioChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxPwmChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxPwmChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxQeChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxQeChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxCommChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxCommChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, unsigned int numBytes, int timeout, unsigned int* numBytesRead) = 0;
		virtual int Write(const unsigned char* sendBuffer, unsigned int numBytes, int timeout) = 0;
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
		LinxUartChannel(LinxFmtChannel *debug, const unsigned char *deviceName) : LinxCommChannel(debug, deviceName) {};
		virtual ~LinxUartChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int SetParameters(unsigned char dataBits, unsigned char stopBits, LinxUartParity parity) = 0;
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
		LinxI2cChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxI2cChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Open() = 0;
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed) = 0;
		virtual int Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer) = 0;
		virtual int Write(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned char* sendBuffer) = 0;
		virtual int Transfer(unsigned char slaveAddress, int numFrames, int *flags, int *numBytes, unsigned int timeout, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
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
		LinxSpiChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxSpiChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
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
		LinxCanChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxCanChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		/* Just a placeholder currently, actual methods will need to be determined later */
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
		LinxServoChannel(LinxFmtChannel *debug, const unsigned char *channelName) : LinxChannel(debug, channelName) {};
		virtual ~LinxServoChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetPulseWidth(unsigned short width) = 0;
		virtual int Close() = 0;
};

// A channel that can wrap a LinxCommChannel and provide formatting functions for simple debug output
class LinxFmtChannel : public LinxChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxFmtChannel(int timeout = TIMEOUT_INFINITE, const unsigned char *channelName = (unsigned char *)"FormatChannel");
		LinxFmtChannel(LinxCommChannel *channel, int timeout = TIMEOUT_INFINITE, const unsigned char *channelName = (unsigned char *)"FormatChannel");
		virtual ~LinxFmtChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Write(char c);
		virtual int Write(const char s[]);
		virtual int Write(const char s[], int len);
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

		int SetTimeout(int timeout);
		int SetDebugChannel(LinxCommChannel *channel);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxCommChannel *m_Channel;
		int m_Timeout;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int WriteNumber(unsigned long n, unsigned char base);
};

#endif // LINX_CHANNEL_H