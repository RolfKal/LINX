/****************************************************************************************
**  LINX header for Linux support functions used in Beaglebone Black and Raspberry Pi Device
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermater based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_LINUXDEVICE_H
#define LINX_LINUXDEVICE_H

/****************************************************************************************
**  Defines
****************************************************************************************/
#define NUM_UART_SPEEDS 18

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDefines.h"
#include <stdio.h>
#include <string>
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
		~LinxSysfsAiChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Read(unsigned int *value);

	protected:
		char m_State;			// Current DIO Direction and Pull-State

	private:
		FILE *m_ValHandle;	// File Handles For Digital Pin Value

		int SmartOpen();
};

class LinxSysfsDioChannel : public LinxDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin);
		~LinxSysfsDioChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

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

		int SmartOpen();
};

class LinxUnixUartChannel : public LinxUartChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxUnixUartChannel(const char *channelName, LinxFmtChannel *debug);
		~LinxUnixUartChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int SetBitSizes(unsigned char dataBits, unsigned char stopBits);
		virtual int SetParity(LinxUartParity parity);
		virtual int GetBytesAvail(int* numBytesAvailable);
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SmartOpen();

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		int m_Fd;
		bool m_init;
};

class LinxSysfsI2cChannel : public LinxI2cChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxSysfsI2cChannel(const char *channelName, LinxFmtChannel *debug);
		~LinxSysfsI2cChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Open();
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer);
		virtual int Write(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned char* sendBuffer);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		int m_Fd;
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
		LinxSysfsSpiChannel(const char *channelName, LinxFmtChannel *debug, LinxLinuxDevice *device, unsigned int speed);
		~LinxSysfsSpiChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Open();
		virtual int SetBitOrder(unsigned char bitOrder);
		virtual int SetMode(unsigned char mode);
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int WriteRead(unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int Close();

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
		LinxLinuxDevice *m_Device;
		int m_Fd;
		unsigned char m_BitOrder;
		unsigned int m_CurrentSpeed;
		unsigned int m_MaxSpeed;

};

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/

class LinxLinuxDevice : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxLinuxDevice();
		~LinxLinuxDevice();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage);

		//DIGITAL
		virtual int DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char *states);		// direction and pull-up/down
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);			// Values Are Bit Packed
		virtual int DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Values Are Not Bit Packed
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Response Not Bit Packed
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration);
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width);

		//PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);
 
		//SPI
		virtual int SpiOpenMaster(unsigned char channel);
		virtual int SpiOpenMaster(const char *deviceName, unsigned char channel);
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
		virtual int SpiSetMode(unsigned char channel, unsigned char mode);
		virtual int SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int SpiCloseMaster(unsigned char channel);

		//I2C
		virtual int I2cOpenMaster(unsigned char channel);
		virtual int I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);
		virtual int I2cClose(unsigned char channel);

		//UART
		virtual int UartOpen(const char *deviceName, unsigned char nameLength, unsigned char *channel, LinxUartChannel **chan = NULL);
		virtual int UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes);
		virtual int UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead);
		virtual int UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int UartClose(unsigned char channel);

		//Servo
		virtual int ServoOpen(unsigned char numChans, unsigned char* chans);
		virtual int ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths);
		virtual int ServoClose(unsigned char numChans, unsigned char* chans);

		// General
		virtual void NonVolatileWrite(int address, unsigned char data);
		virtual unsigned char NonVolatileRead(int address);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System

		//DIO

		//PWM
		std::map<unsigned char, std::string> PwmDirPaths;		//PWM Device Tree Overlay Names
		std::map<unsigned char, FILE*> PwmPeriodHandles;		//File Handles For PWM Period Values
		std::map<unsigned char, FILE*> PwmDutyCycleHandles;		//File Handles For PWM Duty Cycle Values
		std::map<unsigned char, unsigned long> PwmPeriods;		//Current PWM  Values
		unsigned int PwmDefaultPeriod;							//Default Period For PWM Channels (nS)
		std::string PwmDutyCycleFileName;
		std::string PwmPeriodFileName;
		std::string PwmEnableFileName;

		//AI
		std::map<unsigned char, FILE*> AiValueHandles;			//AI Value Handles
		std::map<unsigned char, std::string> AiValuePaths;		//AI Value Paths
		unsigned char NumAiRefIntVals;							//Number Of Internal AI Reference Voltages
		const unsigned long* AiRefIntVals;						//Supported AI Reference Voltages (uV)
		const int* AiRefCodes;									//AI Ref Values (AI Ref Macros In Wiring Case)
		unsigned int AiRefExtMin;								//Min External AI Ref Value (uV)
		unsigned int AiRefExtMax;					   			//Max External AI Ref Value (uV)

		//AO
		std::map<unsigned char, FILE*> AoValueHandles;			//AO Value Handles

		//UART
//		std::map<unsigned char, std::string> UartPaths;			//UART Channel File Paths
//		std::map<unsigned char, int> UartHandles;				//File Handles For UARTs - Must Be Int For Termios Functions
//		std::map<unsigned char, std::string> UartDtoNames;		//UART Device Tree Overlay Names
		unsigned char NumUartSpeeds;							//Number Of Support UART Buads

		//SPI
//		std::map<unsigned char, std::string> SpiDtoNames;  		//Device Tree Overlay Names For SPI Master(s)
//		std::map<unsigned char, std::string> SpiPaths;  		//File Paths For SPI Master(s)
//		std::map<unsigned char, int> SpiHandles;				//File Handles For SPI Master(s)

		unsigned int SpiDefaultSpeed;

		//I2C
//		std::map<unsigned char, std::string> I2cPaths;			//File Paths For I2C Master(s)
///		std::map<unsigned char, int> I2cHandles;				//File Handles For I2C Master(s)

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int pwmSmartOpen(unsigned char numChans, unsigned char* channels);
		bool uartSupportsVarBaudrate(const char* path, int baudrate);

	private:
		int UartSetBaudRate(int fd, unsigned int baudRate, unsigned int* actualBaud, bool init);
		int UartSetBitSize(int fd, unsigned char dataBits, unsigned char stopBits);
		int UartSetParity(int fd, LinxUartParity parity);

};
#endif //LINX_LINUXDEVICE_H