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
		unsigned char LinxApiMajor;
		unsigned char LinxApiMinor;
		unsigned char LinxApiSubminor;

		//Device ID
		unsigned char DeviceFamily;
		unsigned char DeviceId;

		//----Peripherals----

		//DIO

		//AI
		unsigned char AiResolution;
		unsigned int AiRefDefault;
		unsigned int AiRefSet;

		//AO
		unsigned char AoResolution;
		unsigned int AoRefDefault;
		unsigned int AoRefSet;

		//PWM

		//QE

		//UART
		unsigned int UartMaxBaud;

		//I2C

		//SPI

		//CAN

		//Servo

		//User Configured Values
		unsigned short userId;

		unsigned int ethernetIp;
		unsigned short ethernetPort;

		unsigned int WifiIp;
		unsigned short WifiPort;
		unsigned char WifiSsidSize;
		char WifiSsid[32];
		unsigned char WifiSecurity;
		unsigned char WifiPwSize;
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
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length) = 0;

		// Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);		//Values Are ADC Ticks And Not Bit Packed
		virtual int AnalogSetRef(unsigned char mode, unsigned int voltage);
		virtual int AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);

		// DIGITAL
		virtual int DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char *states);		// direction and pull-up/down
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);			// Values Are Bit Packed
		virtual int DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Values Are Not Bit Packed
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);	// Response Not Bit Packed
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration);
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width);

		// QE

		// PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned int* values);

		// SPI
		virtual int SpiOpenMaster(unsigned char channel);
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
		virtual int SpiSetMode(unsigned char channel, unsigned char mode);
		virtual int SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int SpiCloseMaster(unsigned char channel);

		// I2C
		virtual int I2cOpenMaster(unsigned char channel);
		virtual int I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);
		virtual int I2cTransfer(unsigned char channel, unsigned char slaveAddress, int numFrames, int *flags, int *numBytes, unsigned int timeout, unsigned char* sendBuffer, unsigned char* recBuffer);
		virtual int I2cClose(unsigned char channel);

		// UART
		virtual int UartOpen(unsigned char channel, LinxUartChannel **chan = NULL);
		virtual int UartOpen(const unsigned char *deviceName, unsigned char *channel, LinxUartChannel **chan = NULL);
		virtual int UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
		virtual int UartSetParameters(unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned int *numBytes);
		virtual int UartRead(unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, int timeout, unsigned int* numBytesRead);
		virtual int UartWrite(unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer, int timeout);
		virtual int UartClose(unsigned char channel);

		// CAN

		// Servo
		virtual int ServoOpen(unsigned char numChans, unsigned char* channels);
		virtual int ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths);
		virtual int ServoClose(unsigned char numChans, unsigned char* channels);

		// WS2812
		virtual int Ws2812Open(unsigned short numLeds, unsigned char dataChan);
		virtual int Ws2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh);
		virtual int Ws2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh);
		virtual int Ws2812Refresh(void);
		virtual int Ws2812Close(void);

		// General
		virtual void NonVolatileWrite(int address, unsigned char data);
		virtual unsigned char NonVolatileRead(int address);

		virtual unsigned int GetMilliSeconds(void);
		virtual unsigned int GetSeconds(void);
		virtual void DelayMs(unsigned int ms);

		virtual bool ChecksumPassed(unsigned char* buffer, int length);
		virtual unsigned char ComputeChecksum(unsigned char* buffer, int length);

		virtual int EnumerateChannels(int type, unsigned char *buffer = NULL, unsigned int length = 0, unsigned int *reqLen = NULL);

		virtual void DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxFmtChannel *m_Debug;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel* LookupChannel(int type, unsigned char channel);
		virtual LinxChannel* LookupChannel(int type, const unsigned char *channelName, unsigned char *channel);
		virtual unsigned char RegisterChannel(int type, LinxChannel *chan);
		virtual void RegisterChannel(int type, unsigned char channel, LinxChannel *chan);
		virtual void RemoveChannel(int type, unsigned char channel);
		virtual void ClearChannels(int type);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		std::map<unsigned char, LinxChannel*> m_ChannelRegistry[LinxNumChannelTypes];

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_DEVICE_H
