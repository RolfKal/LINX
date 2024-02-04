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
		LinxClient(const unsigned char *uartDevice, uint32_t *baudrate, uint8_t dataBits, uint8_t stopBits,  LinxUartParity parity, int32_t timeout);
		LinxClient(const unsigned char *netAddress, uint16_t port, int32_t timeout);
		virtual ~LinxClient(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, uint8_t length);
		int32_t IsInitialized(void);

		//Analog
		virtual int32_t AnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t AnalogReadNoPacking(uint8_t numChans, uint8_t* channels, uint32_t* values);		//Values Are ADC Ticks And Not Bit Packed
		virtual int32_t AnalogReadValues(uint8_t numChans, uint8_t* channels, double* values);
		virtual int32_t AnalogSetRef(uint8_t mode, uint32_t voltage);
		virtual int32_t AnalogWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t AnalogWriteValues(uint8_t numChans, uint8_t* channels, double* values);

		//DIGITAL
		virtual int32_t DigitalSetState(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalWriteNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);		//Values Not Bit Packed
		virtual int32_t DigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalReadNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);		//Response Not Bit Packed
		virtual int32_t DigitalWriteSquareWave(uint8_t channel, uint32_t freq, uint32_t duration);
		virtual int32_t DigitalReadPulseWidth(uint8_t stimChan, uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width);

		//PWM
		virtual int32_t PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values);

		//SPI
		virtual int32_t SpiOpenMaster(uint8_t channel);
		virtual int32_t SpiSetBitOrder(uint8_t channel, uint8_t bitOrder);
		virtual int32_t SpiSetMode(uint8_t channel, uint8_t mode);
		virtual int32_t SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, uint8_t* sendBuffer, uint8_t* recBuffer);
		virtual int32_t SpiCloseMaster(uint8_t channel);

		//I2C
		virtual int32_t I2cOpenMaster(uint8_t channel);
		virtual int32_t I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint8_t* sendBuffer);
		virtual int32_t I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, uint8_t* recBuffer);
		virtual int32_t I2cClose(uint8_t channel);

		//UART
		virtual int32_t UartOpen(uint8_t channel, LinxUartChannel **channelObj = NULL);
		virtual int32_t UartOpen(const unsigned char *deviceName, uint8_t *channel, LinxUartChannel **channelObj = NULL);
//		virtual int32_t UartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
//		virtual int32_t UartGetBytesAvailable(uint8_t channel, uint32_t *numBytes);
//		virtual int32_t UartRead(uint8_t channel, uint32_t numBytes, uint8_t* recBuffer, uint32_t* numBytesRead);
//		virtual int32_t UartWrite(uint8_t channel, uint32_t numBytes, uint8_t* sendBuffer);
//		virtual int32_t UartClose(uint8_t channel);

		//Servo
		virtual int32_t ServoOpen(uint8_t numChans, uint8_t* chans);
		virtual int32_t ServoSetPulseWidth(uint8_t numChans, uint8_t* chans, uint16_t* pulseWidths);
		virtual int32_t ServoClose(uint8_t numChans, uint8_t* chans);

		// General
		virtual void NonVolatileWrite(int32_t address, uint8_t data);
		virtual uint8_t NonVolatileRead(int32_t address);

		int32_t PrepareHeader(uint8_t *buffer, uint16_t command, uint32_t dataLength, uint32_t expLength, uint32_t *headerLength);
		int32_t WriteAndRead(uint8_t *buffer, uint32_t buffLength, uint32_t *headerOffset, uint32_t dataLength, uint32_t *dataRead);
		int32_t WriteAndRead(uint8_t *header, uint32_t headerLength, uint32_t *headerOffset, uint32_t headerData, uint8_t *dataBuffer, uint32_t dataLength, uint32_t *dataRead, uint32_t start, int32_t timeout);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Initialize(LinxCommChannel *channel);	

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned char *m_DeviceName;
		LinxCommChannel *m_CommChannel;

		uint32_t m_ListenerBufferSize;
		uint16_t m_PacketNum;
		uint8_t m_ProtocolVersion;

		int32_t m_Timeout;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		uint16_t GetNextPacketNum(void);
		void CopyArrayToSet(int32_t type, uint8_t *arr, uint32_t length, LinxChannel *chan = NULL);

		int32_t GetSyncCommand(bool negotiate);
		int32_t GetU8Parameter(uint16_t command, uint8_t *val);
		int32_t GetU16Parameter(uint16_t command, uint16_t *val);
		int32_t GetU32Parameter(uint16_t command, uint32_t *val);
		int32_t GetU8ArrParameter(uint16_t command, uint8_t *val, uint32_t buffLength, uint8_t param, uint32_t *headerLength, uint32_t *dataRead);
};

class LinxClientUartChannel : public LinxUartChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxClientUartChannel(LinxFmtChannel *debug, LinxClient *client, uint8_t channel, uint8_t protVersion, const unsigned char *deviceName);
		virtual ~LinxClientUartChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);
		virtual int32_t Read(unsigned char* recBuffer, uint32_t numBytes, uint32_t start, int32_t timeout, uint32_t* numBytesRead);
		virtual int32_t Write(const unsigned char* sendBuffer, uint32_t numBytes, uint32_t start, int32_t timeout);
		virtual int32_t Close(void);

	protected:


	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxClient *m_Client;
		uint8_t m_Channel;
		uint8_t m_ProtocolVersion;

};

#endif //LINX_CLIENT_H