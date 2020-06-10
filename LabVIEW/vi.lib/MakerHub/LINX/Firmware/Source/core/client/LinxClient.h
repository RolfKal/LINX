/****************************************************************************************
**  LINX header for generic LINX Client interface.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_CLIENT_H
#define LINX_CLIENT_H

/****************************************************************************************
** Defines
****************************************************************************************/

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDevice.h"

/****************************************************************************************
**  Typedefs
****************************************************************************************/

class LinxClient : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxClient();
		virtual ~LinxClient();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);

		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);		//Values Are ADC Ticks And Not Bit Packed
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
		virtual int Initialize(); 

		virtual int WriteCommand(unsigned char *packetBuffer, unsigned int packetLength, unsigned int timeout) = 0;
		virtual int ReadResponse(unsigned char *packetBuffer, unsigned int packetLength, unsigned int timeout) = 0;

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned char *m_DeviceName;

		//DIO
		unsigned char *m_DigitalChans;

		//AI
		unsigned char *m_AiChans;

		//AO
		unsigned char *m_AoChans;

		//PWM
		unsigned char *m_PwmChans;

		//QE
		unsigned char *m_QeChans;

		//UART
		unsigned char *m_UartChans;

		//I2C
		unsigned char *m_I2cChans;

		//SPI
		unsigned char *m_SpiChans;

		//CAN
		unsigned char *m_CanChans;

		//Servo
		unsigned char *m_ServoChans;

		unsigned int m_Timeout;

		unsigned int ListenerBufferSize;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned short GetNextPacketNum();
		virtual unsigned char ComputeChecksum(unsigned char* buffer, unsigned int size);
		virtual bool ChecksumPassed(unsigned char* buffer, unsigned int size);
		virtual unsigned int PrepareHeader(unsigned char* buffer, unsigned short command, unsigned short packetNum, unsigned int dataSize);
		virtual int WriteAndRead(unsigned char *buffer, unsigned int *offset, unsigned short packetNum, unsigned int timeout, unsigned int writeData, unsigned int readData, unsigned int *dataRead);

		virtual int GetNoParameter(unsigned short command);
		virtual int GetU8Parameter(unsigned short command, unsigned char *val);
		virtual int GetU32Parameter(unsigned short command, unsigned int *val);
		virtual int GetU8ArrParameter(unsigned short command, unsigned char *val, unsigned int *offset, unsigned int length, unsigned int *dataRead);
};
#endif //LINX_CLIENT_H