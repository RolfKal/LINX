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
#include <set>
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
		LinxClient(const unsigned char *uartDevice, unsigned int *baudrate, unsigned char dataBits, unsigned char stopBits,  LinxUartParity parity, int timeout);
		LinxClient(const unsigned char *netAddress, unsigned short port, int timeout);
		virtual ~LinxClient(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);
		int IsInitialized(void);

		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);		//Values Are ADC Ticks And Not Bit Packed
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage);
		virtual int AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned int* values);

		//DIGITAL
		virtual int DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* values);
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
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned int *numBytes);
		virtual int UartRead(unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, unsigned int* numBytesRead);
		virtual int UartWrite(unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer);
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

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Initialize(LinxCommChannel *channel);	

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned char *m_DeviceName;
		LinxCommChannel *m_CommChannel;

		unsigned int m_ListenerBufferSize;
		unsigned short m_PacketNum;
		unsigned char m_ProtocolVersion;

		int m_Timeout;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		unsigned short GetNextPacketNum(void);
		int PrepareHeader(unsigned char* buffer, unsigned short command, unsigned int dataLength, unsigned int expLength, unsigned int *headerLength);
		int WriteAndRead(unsigned char *buffer, unsigned int buffLength, unsigned int *headerLength, unsigned int dataLength, unsigned int *dataRead);
		void CopyArrayToSet(int type, unsigned char *arr, unsigned int length);

		int GetSyncCommand(bool negotiate);
		int GetU8Parameter(unsigned short command, unsigned char *val);
		int GetU16Parameter(unsigned short command, unsigned short *val);
		int GetU32Parameter(unsigned short command, unsigned int *val);
		int GetU8ArrParameter(unsigned short command, unsigned char *val, unsigned int buffLength, unsigned char param, unsigned int *headerLength, unsigned int *dataRead);
};
#endif //LINX_CLIENT_H