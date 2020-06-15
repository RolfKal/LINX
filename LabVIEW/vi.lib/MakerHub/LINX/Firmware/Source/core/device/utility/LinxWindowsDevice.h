/****************************************************************************************
**  LINX header for Windows support functions used LinxDeviceLib on host
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermater based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_WINDOWSDEVICE_H
#define LINX_WINDOWSDEVICE_H

/****************************************************************************************
**  Defines
****************************************************************************************/

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <map>
#include <string>
#include <windows.h>

#include "LinxDevice.h"

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/

class LinxWindowsDevice : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsDevice();
		~LinxWindowsDevice();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);
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

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int DigitalWrite(unsigned char channel, unsigned char value);
		virtual int digitalSmartOpen(unsigned char numChans, unsigned char* channels);
		virtual int pwmSmartOpen(unsigned char numChans, unsigned char* channels);
		bool fileExists(const char* path);
		bool fileExists(const char* path, int *length);
		bool fileExists(const char* directory, const char* fileName);
		bool fileExists(const char* directory, const char* fileName, unsigned int timout);

	private:
		LARGE_INTEGER Frequency;
};
#endif //LINX_WINDOWSDEVICE_H