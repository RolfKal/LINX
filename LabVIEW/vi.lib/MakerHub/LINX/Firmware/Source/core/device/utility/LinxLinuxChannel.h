/****************************************************************************************
**  LINX header for channel defitions for use on Linux based systems
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermater based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_LINUXCHANNEL_H
#define LINX_LINUXCHANNEL_H

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <string>
#include "LinxDefines.h"
#include "LinxChannel.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Typedefs
****************************************************************************************/

class LinxLinuxDevice;

class LinxSysfsAiChannel : public LinxAiChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsAiChannel(LinxFmtChannel *debug, const char *channelName);
		virtual ~LinxSysfsAiChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned int *value);

	protected:
//		char m_State;			// Current DIO Direction and Pull-State

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value

		int SmartOpen(void);
};

class LinxSysfsAoChannel : public LinxAoChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsAoChannel(LinxFmtChannel *debug, const char *channelName);
		virtual ~LinxSysfsAoChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Write(unsigned int value);

	protected:
//		char m_State;			// Current DIO Direction and Pull-State

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value

		int SmartOpen(void);
};

class LinxSysfsDioChannel : public LinxDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin);
		virtual ~LinxSysfsDioChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetState(unsigned char state);		// direction and pull-up/down
		virtual int Write(unsigned char value);
		virtual int Read(unsigned char *value);
		virtual int WriteSquareWave(unsigned int freq, unsigned int duration);
		virtual int ReadPulseWidth(unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width);

	protected:
		short m_GpioChan;		// Maps LINX DIO Channel Number To GPIO Channel
		short m_LinxChan;		// Maps LINX DIO Channel Number To GPIO Channel
		char m_State;			// Current DIO Direction and Pull-State

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value
		FILE *m_DirHandle;	// File Handles For Digital Pin Direction
		FILE *m_EdgeHandle;	// File Handles For Digital Pin Edge

		int SmartOpen(void);
};

class LinxSysfsPwmChannel : public LinxPwmChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsPwmChannel(LinxFmtChannel *debug, const char *deviceName, const char *enableFileName, const char *periodName, const char *dutyCycleName, unsigned int defaultPeriod);
		virtual ~LinxSysfsPwmChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetDutyCycle(unsigned char values);
		virtual int SetFrequency(unsigned int values);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		FILE *m_PeriodHandle;		// File Handles For Period
		FILE *m_DutyCycleHandle;	// File Handles For Duty Cycle
		const char *m_EnableFileName; 
		const char *m_PeriodFileName;
		const char *m_DutyCycleFileName;
		unsigned int m_DefaultPeriod;
		unsigned int m_Period;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int SmartOpen(void);
};

class LinxUnixCommChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *channelName, NetObject socket);
		LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *address, unsigned short port);
		virtual ~LinxUnixCommChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(const unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close(void);

	private:
		NetObject m_Socket;
}

class LinxUnixUartChannel : public LinxUartChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUnixUartChannel(LinxFmtChannel *debug, const char *deviceName);
		virtual ~LinxUnixUartChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int SetParameters(unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
		virtual int Read(unsigned char* recBuffer, unsigned int numBytes, int timeout, unsigned int* numBytesRead);
		virtual int Write(const unsigned char* sendBuffer, unsigned int numBytes, int timeout);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SmartOpen(void);

private:
		int m_Fd;
};

class LinxSysfsI2cChannel : public LinxI2cChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsI2cChannel(LinxFmtChannel *debug, const char *channelName);
		virtual ~LinxSysfsI2cChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Open(void);
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer);
		virtual int Write(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned char* sendBuffer);
		virtual int Transfer(unsigned char slaveAddress, int numFrames, int *flags, int *numBytes, unsigned int timeout, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		int m_Fd;
		unsigned long m_Funcs;
};

class LinxSysfsSpiChannel : public LinxSpiChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsSpiChannel(LinxFmtChannel *debug, LinxDevice *device, const char *channelName, unsigned int maxSpeed);
		virtual ~LinxSysfsSpiChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Open(void);
		virtual int SetBitOrder(unsigned char bitOrder);
		virtual int SetMode(unsigned char mode);
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int WriteRead(unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned char m_NumSpiSpeeds;								//Number Of Supported SPI Speeds
		unsigned int* m_SpiSupportedSpeeds;						//Supported SPI Clock Frequencies
		int* m_SpiSpeedCodes;										//SPI Speed Values (Clock Divider Macros In Wiring Case)

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
//		LinxDevice *m_Device;
		int m_Fd;
		LinxDevice *m_Device;
		unsigned char m_BitOrder;
		unsigned int m_CurrentSpeed;
		unsigned int m_MaxSpeed;

};
#endif // LINX_LINUX_CHANNEL_H