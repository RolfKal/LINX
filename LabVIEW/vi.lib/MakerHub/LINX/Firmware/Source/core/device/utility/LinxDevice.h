/****************************************************************************************
**  LINX header for generic LINX Device.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_DEVICE_H
#define LINX_DEVICE_H

/****************************************************************************************
**  Includes
****************************************************************************************/
//This Makes It Easy For IDE Users To Define Necessary Settings In One Place
//When Using Make Files Define LINXCONFIG To Ignore Config.h File
#ifndef LINXCONFIG
	#include "../config/LinxConfig.h"
#endif
#include <stddef.h>
#include <map>
#include "LinxBase.h"
#include "LinxChannel.h"

/****************************************************************************************
**  Defines
****************************************************************************************/

//DEBUG
#define TX 0
#define RX 1

class LinxDevice : public LinxBase
{
	public:
		/****************************************************************************************
		**  Member Variables
		****************************************************************************************/

		//LINX API Version
		uint8_t LinxApiMajor;
		uint8_t LinxApiMinor;
		uint8_t LinxApiSubminor;

		//Device ID
		uint8_t DeviceFamily;
		uint8_t DeviceId;

		//----Peripherals----

		//DIO

		//AI
		uint8_t AiResolution;
		uint32_t AiRefDefault;
		uint32_t AiRefSet;

		//AO
		uint8_t AoResolution;
		uint32_t AoRefDefault;
		uint32_t AoRefSet;

		//PWM

		//QE

		//UART
		uint32_t UartMaxBaud;

		//I2C

		//SPI

		//CAN

		//Servo

		//User Configured Values
		uint16_t userId;

		uint32_t ethernetIp;
		uint16_t ethernetPort;

		uint32_t WifiIp;
		uint16_t WifiPort;
		uint8_t WifiSsidSize;
		char WifiSsid[32];
		uint8_t WifiSecurity;
		uint8_t WifiPwSize;
		char WifiPw[64];

		/****************************************************************************************
		**  Constructors/Destructor
		****************************************************************************************/
		LinxDevice(LinxFmtChannel *debug = NULL);
		virtual ~LinxDevice(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		// System Support
		virtual uint8_t GetDeviceName(unsigned char *buffer, uint8_t length) = 0;

		// Analog
		virtual int32_t AnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t AnalogReadNoPacking(uint8_t numChans, uint8_t* channels, uint32_t* values);	//Values Are ADC Ticks And Not Bit Packed
		virtual int32_t AnalogReadValues(uint8_t numChans, uint8_t* channels, double* values);		
		virtual int32_t AnalogSetRef(uint8_t mode, uint32_t voltage);
		virtual int32_t AnalogWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t AnalogWriteValues(uint8_t numChans, uint8_t* channels, double *values);

		// DIGITAL
		virtual int32_t DigitalSetState(uint8_t numChans, uint8_t* channels, uint8_t *states);		// direction and pull-up/down
		virtual int32_t DigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);			// Values Are Bit Packed
		virtual int32_t DigitalWriteNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);	// Values Are Not Bit Packed
		virtual int32_t DigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalReadNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);	// Response Not Bit Packed
		virtual int32_t DigitalWriteSquareWave(uint8_t channel, uint32_t freq, uint32_t duration);
		virtual int32_t DigitalReadPulseWidth(uint8_t stimChan, uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width);

		// QE

		// PWM
		virtual int32_t PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t PwmSetFrequency(uint8_t numChans, uint8_t* channels, uint32_t* values);

		// SPI
		virtual int32_t SpiOpenMaster(uint8_t channel);
		virtual int32_t SpiSetBitOrder(uint8_t channel, uint8_t bitOrder);
		virtual int32_t SpiSetMode(uint8_t channel, uint8_t mode);
		virtual int32_t SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int32_t SpiCloseMaster(uint8_t channel);

		// I2C
		virtual int32_t I2cOpenMaster(uint8_t channel);
		virtual int32_t I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer);
		virtual int32_t I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer);
		virtual int32_t I2cTransfer(uint8_t channel, uint8_t slaveAddress, int32_t numFrames, int32_t *flags, int32_t *numBytes, uint32_t timeout, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int32_t I2cClose(uint8_t channel);

		// UART
		virtual int32_t UartOpen(uint8_t channel, LinxUartChannel **channelObj = NULL);
		virtual int32_t UartOpen(const unsigned char *deviceName, uint8_t *channel, LinxUartChannel **channelObj = NULL);
		virtual int32_t UartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
		virtual int32_t UartSetParameters(uint8_t channel, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);
		virtual int32_t UartGetBytesAvailable(uint8_t channel, uint32_t *numBytes);
		virtual int32_t UartRead(uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead);
		virtual int32_t UartWrite(uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout);
		virtual int32_t UartClose(uint8_t channel);

		// CAN

		// Servo
		virtual int32_t ServoOpen(uint8_t numChans, uint8_t* channels);
		virtual int32_t ServoSetPulseWidth(uint8_t numChans, uint8_t* channels, uint16_t* pulseWidths);
		virtual int32_t ServoClose(uint8_t numChans, uint8_t* channels);

		// WS2812
		virtual int32_t Ws2812Open(uint16_t numLeds, uint8_t dataChan);
		virtual int32_t Ws2812WriteOnePixel(uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue, uint8_t refresh);
		virtual int32_t Ws2812WriteNPixels(uint16_t startPixel, uint16_t numPixels, unsigned char* data, uint8_t refresh);
		virtual int32_t Ws2812Refresh(void);
		virtual int32_t Ws2812Close(void);

		// General
		virtual void NonVolatileWrite(int32_t address, uint8_t data);
		virtual uint8_t NonVolatileRead(int32_t address);

		virtual uint32_t GetMilliSeconds(void);
		virtual uint32_t GetSeconds(void);
		virtual void DelayMs(uint32_t ms);

		virtual int32_t EnumerateChannels(int32_t type, unsigned char *buffer = NULL, uint32_t length = 0, uint32_t *reqLen = NULL);

		virtual void DebugPrintPacket(uint8_t direction, const unsigned char* packetBuffer);
		virtual void DebugEnable(LinxCommChannel *channel);
		virtual void DebugDisable(void);
		virtual LinxFmtChannel* GetDebug(void);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxFmtChannel *m_Debug;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t VerifyChannels(int32_t type, uint8_t numChans, uint8_t *channel);
		virtual LinxChannel* LookupChannel(int32_t type, const unsigned char *channelName, uint8_t *channel);
		virtual LinxChannel* LookupChannel(int32_t type, uint8_t channel);
		virtual uint8_t RegisterChannel(int32_t type, LinxChannel *chan);
		virtual void RegisterChannel(int32_t type, uint8_t channel, LinxChannel *chan = NULL);
		virtual void RemoveChannel(int32_t type, uint8_t channel);
		virtual void ClearChannels(int32_t type);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		std::map<uint8_t, LinxChannel*> m_ChannelRegistry[LinxNumChannelTypes];

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_DEVICE_H
