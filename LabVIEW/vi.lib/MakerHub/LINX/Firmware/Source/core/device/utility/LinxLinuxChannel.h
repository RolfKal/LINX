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
		LinxSysfsAiChannel(LinxFmtChannel *debug, const char *channelName, uint8_t resolution);
		virtual ~LinxSysfsAiChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(uint32_t *value);

	protected:

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value

		int32_t SmartOpen(void);
};

class LinxSysfsAoChannel : public LinxAoChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsAoChannel(LinxFmtChannel *debug, const char *channelName, uint8_t resolution);
		virtual ~LinxSysfsAoChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Write(uint32_t value);

	protected:

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value

		int32_t SmartOpen(void);
};

class LinxSysfsDioChannel : public LinxDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsDioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin);
		virtual ~LinxSysfsDioChannel(void);

	protected:
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
		FILE *m_ValHandle;	// File Handles For Digital Pin Value
		FILE *m_DirHandle;	// File Handles For Digital Pin Direction
		FILE *m_EdgeHandle;	// File Handles For Digital Pin Edge

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int32_t SmartOpen(void);
};

class LinxGPIODioChannel : public LinxDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxGPIODioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin);
		virtual ~LinxGPIODioChannel(void);

	protected:
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

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int32_t SmartOpen(void);
};

class LinxSysfsPwmChannel : public LinxPwmChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsPwmChannel(LinxFmtChannel *debug, const char *deviceName, const char *enableFileName, const char *periodName, const char *dutyCycleName, uint32_t defaultPeriod);
		virtual ~LinxSysfsPwmChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetDutyCycle(uint8_t values);
		virtual int32_t SetFrequency(uint32_t values);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		FILE *m_PeriodHandle;		// File Handles For Period
		FILE *m_DutyCycleHandle;	// File Handles For Duty Cycle
		const char *m_EnableFileName; 
		const char *m_PeriodFileName;
		const char *m_DutyCycleFileName;
		uint32_t m_DefaultPeriod;
		uint32_t m_Period;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int32_t SmartOpen(void);
};

class LinxUnixCommChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *channelName, NetObject socket);
		LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *address, uint16_t port);
		virtual ~LinxUnixCommChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(unsigned char* recBuffer, int32_t numBytes, uint32_t start, int32_t timeout, int32_t* numBytesRead);
		virtual int32_t Write(const unsigned char* sendBuffer, int32_t numBytes, uint32_t start, int32_t timeout);
		virtual int32_t Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SmartOpen(void);
		int32_t m_Fd;

	private:

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
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);

private:

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
		virtual int32_t Open(void);
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t Read(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, uint32_t timeout, unsigned char* recBuffer);
		virtual int32_t Write(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, unsigned char* sendBuffer);
		virtual int32_t Transfer(uint8_t slaveAddress, int32_t numFrames, int32_t *flags, int32_t *numBytes, uint32_t timeout, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int32_t Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		int32_t m_Fd;
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
		LinxSysfsSpiChannel(LinxFmtChannel *debug, LinxDevice *device, const char *channelName, uint32_t maxSpeed);
		virtual ~LinxSysfsSpiChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Open(void);
		virtual int32_t SetBitOrder(uint8_t bitOrder);
		virtual int32_t SetMode(uint8_t mode);
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t WriteRead(uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int32_t Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		uint8_t m_NumSpiSpeeds;								//Number Of Supported SPI Speeds
		uint32_t* m_SpiSupportedSpeeds;						//Supported SPI Clock Frequencies
		int32_t* m_SpiSpeedCodes;										//SPI Speed Values (Clock Divider Macros In Wiring Case)

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
//		LinxDevice *m_Device;
		int32_t m_Fd;
		LinxDevice *m_Device;
		uint8_t m_BitOrder;
		uint32_t m_CurrentSpeed;
		uint32_t m_MaxSpeed;

};
#endif // LINX_LINUX_CHANNEL_H