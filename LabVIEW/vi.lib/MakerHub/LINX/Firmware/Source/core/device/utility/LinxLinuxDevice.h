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
#ifndef INPUT
	#define INPUT 0x00
#endif

#ifndef  OUTPUT
	#define OUTPUT 0x01
#endif

#ifndef HIGH
	#define HIGH 0x01
#endif

#ifndef LOW
	#define LOW 0x00
#endif

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDevice.h"
#include <stdio.h>
#include <string>

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
		virtual unsigned char GetAiChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetAoChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetDioChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetQeChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetPwmChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetSpiChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetI2cChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetUartChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetCanChans(unsigned char *buffer, unsigned char length);
		virtual unsigned char GetServoChans(unsigned char *buffer, unsigned char length);

		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage);

		//DIGITAL
		virtual int DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);		//Values Not Bit Packed
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);		//Response Not Bit Packed
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration);
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width);

		//PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);
 
		//SPI
		virtual int SpiOpenMaster(unsigned char channel);
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
		virtual int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
		virtual int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
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
		virtual unsigned int GetMilliSeconds();
		virtual unsigned int GetSeconds();
		virtual void DelayMs(unsigned int ms);
		virtual void NonVolatileWrite(int address, unsigned char data);
		virtual unsigned char NonVolatileRead(int address);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System

		//DIO
		std::map<unsigned char, unsigned char> DigitalChannels;	//Maps LINX DIO Channel Numbers To BB GPIO Channels
		std::map<unsigned char, unsigned char> DigitalDirs;		//Current DIO Direction Values
		std::map<unsigned char, FILE*> DigitalDirHandles;		//File Handles For Digital Pin Directions
		std::map<unsigned char, FILE*> DigitalValueHandles;		//File Handles For Digital Pin Values

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
		std::map<unsigned char, std::string> UartPaths;			//UART Channel File Paths
		std::map<unsigned char, int> UartHandles;				//File Handles For UARTs - Must Be Int For Termios Functions
		std::map<unsigned char, std::string> UartDtoNames;		//UART Device Tree Overlay Names
		unsigned char NumUartSpeeds;							//Number Of Support UART Buads
		unsigned int* UartSupportedSpeeds;						//Supported UART Bauds Frequencies
		unsigned int* UartSupportedSpeedsCodes;					//Supported UART Baud Divider Codes

		//SPI
		std::map<unsigned char, std::string> SpiDtoNames;  		//Device Tree Overlay Names For SPI Master(s)
		std::map<unsigned char, std::string> SpiPaths;  		//File Paths For SPI Master(s)
		std::map<unsigned char, int> SpiHandles;				//File Handles For SPI Master(s)
		unsigned char NumSpiSpeeds;								//Number Of Supported SPI Speeds
		unsigned int* SpiSupportedSpeeds;						//Supported SPI Clock Frequencies
		int* SpiSpeedCodes;										//SPI Speed Values (Clock Divider Macros In Wiring Case)
		std::map<unsigned char, unsigned char> SpiBitOrders;	//Stores Bit Orders For SPI Channels (LSBFIRST / MSBFIRST)
		std::map<unsigned char, unsigned long> SpiSetSpeeds; 	//Stores The Set Clock Rate Of Each SPI Channel
		unsigned int SpiDefaultSpeed;

		//I2C
		std::map<unsigned char, std::string> I2cPaths;			//File Paths For I2C Master(s)
		std::map<unsigned char, int> I2cHandles;				//File Handles For I2C Master(s)
		std::map<unsigned char, std::string> I2cDtoNames;		//Device Tree Overlay Names For I2C Master(s)

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int DigitalWrite(unsigned char channel, unsigned char value);
		virtual int digitalSmartOpen(unsigned char numChans, unsigned char* channels);
		virtual int pwmSmartOpen(unsigned char numChans, unsigned char* channels);
		bool uartSupportsVarBaudrate(const char* path, int baudrate);
		bool fileExists(const char* path);
		bool fileExists(const char* path, int *length);
		bool fileExists(const char* directory, const char* fileName);
		bool fileExists(const char* directory, const char* fileName, unsigned int timout);

	private:
		int UartSetBaudRate(int fd, unsigned int baudRate, unsigned int* actualBaud, bool init);
		int UartSetBitSize(int fd, unsigned char dataBits, unsigned char stopBits);
		int UartSetParity(int fd, unsigned char parity);

};
#endif //LINX_LINUXDEVICE_H