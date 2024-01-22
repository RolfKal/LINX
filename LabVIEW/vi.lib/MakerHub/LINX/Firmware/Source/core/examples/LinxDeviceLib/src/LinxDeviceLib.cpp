/****************************************************************************************
**  LINX Device Abstraction Layer (Helper Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written by Sam Kristoff
**  Modifications by Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

#include <stdio.h>
#include <iostream>
#include <string.h>

#include "LinxDefines.h"
#include "LinxChannel.h"
#include "LinxDevice.h"
#include "LinxClient.h"
#include "utility/LinxListener.h"
#include "LinxSerialListener.h"
#include "LinxTcpListener.h"
#include "LinxDeviceLib.h"

#if Win32
  #define LINXDEVICETYPE LinxWindowsDevice
  #include "LinxWindowsDevice.h"
#elif Unix
 #if LINX_DEVICE_FAMILY == 4 
//------------------------------------- Raspberry Pi -------------------------------------
  #if LINX_DEVICE_ID >= 3	//RPI 2 B
   #define LINXDEVICETYPE LinxRaspberryPi
   #include "LinxRaspberryPi.h"
  #endif
 #elif LINX_DEVICE_FAMILY == 6
//------------------------------------- Beagle Bone -------------------------------------
  #if LINX_DEVICE_ID == 1
   #define LINXDEVICETYPE LinxBeagleBoneBlack
   #include "LinxBeagleBoneBlack.h"
  #endif
 #else
   #define LINXDEVICETYPE LinxBeagleBoneBlack
 #endif
#elif Arduino

#endif

LinxDevice* gLinxDev = NULL;

static LinxDevice * LinxGetRef(LinxDevice * dev)
{
	if (!dev)
	{	
		  dev = gLinxDev;
	}
	return dev;
}

//------------------------------------- Constructor / Destructor -------------------------------------
LibAPI(LinxDevice *) LinxOpenRef(void)
{
	//Instantiate The LINX Device
	return new LINXDEVICETYPE();
}

LibAPI(int) LinxOpen(void)
{
	if (!gLinxDev)
	{
		//Instantiate The LINX Device
		gLinxDev = LinxOpenRef();
	}
	return L_OK;
}

LibAPI(LinxDevice *) LinxOpenSerialClient(const unsigned char *deviceName, unsigned int *baudrate, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity, int timeout)
{
	LinxClient *client = new LinxClient(deviceName, baudrate, dataBits, stopBits, parity, timeout);
	if (client && !client->IsInitialized())
	{
		delete client;
		return NULL;
	}
	return client;
}

LibAPI(LinxDevice *) LinxOpenTCPClient(const unsigned char *address, unsigned short port, int timeout)
{
	LinxClient *client = new LinxClient(address, port, timeout);
	if (client && !client->IsInitialized())
	{
		delete client;
		return NULL;
	}
	return client;
}

LibAPI(LinxListener *) LinxOpenUartServer(LinxDevice *dev, const unsigned char *deviceName, unsigned int baudRate, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity, int timeout, bool autostart)
{
	int status = L_DISCONNECT;
	LinxSerialListener *listener = new LinxSerialListener(dev, autostart);
	if (listener)
	{
		status = listener->Start(deviceName, baudRate, dataBits, stopBits, parity, timeout);
		if (status)
		{
			delete listener;
			return NULL;
		}
	}
	return listener;
}

LibAPI(LinxListener *) LinxOpenTCPServer(LinxDevice *dev, const unsigned char *interfaceAddress, short port, int timeout, bool autostart)
{
	int status = L_DISCONNECT;
	LinxTcpListener *listener = new LinxTcpListener(dev, autostart);
	if (listener)
	{
		status = listener->Start(interfaceAddress, port, timeout);
		if (status)
		{
			delete listener;
			return NULL;
		}
	}
	return listener;
}

LibAPI(int) LinxCloseRef(LinxDevice *dev)
{
	if (dev)
		delete dev;
	return L_OK;
}

LibAPI(int) LinxClose()
{
	int status = LinxCloseRef(gLinxDev);
	gLinxDev = NULL;
	return status;
}

//------------------------------------- Enumeration -------------------------------------
LibAPI(unsigned char) LinxGetDeviceFamilyRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceFamily;
	return 0;
}

LibAPI(unsigned char) LinxGetDeviceFamily()
{
	return LinxGetDeviceFamilyRef(gLinxDev);
}

LibAPI(unsigned char) LinxGetDeviceIdRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceId;
	return 0;
}

LibAPI(unsigned char) LinxGetDeviceId()
{
	return LinxGetDeviceIdRef(gLinxDev);
}

LibAPI(int) LinxGetDeviceNameRef(LinxDevice *dev, unsigned char *name, int len)
{
	if (dev)
	{
		dev->GetDeviceName(name, len);
		return L_OK;
	}
	return L_DISCONNECT;
}

LibAPI(int) LinxGetDeviceName(unsigned char *name)
{
	return LinxGetDeviceNameRef(gLinxDev, name, 64);
}

//------------------------------------- General -------------------------------------
LibAPI(unsigned int) LinxGetMilliSecondsRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetMilliSeconds();
	return 0;
}

LibAPI(unsigned int) LinxGetMilliSeconds()
{
	return LinxGetMilliSecondsRef(gLinxDev);
}

//------------------------------------- Analog -------------------------------------
LibAPI(unsigned int) LinxAiGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiRefSet;
	return 0;
}

LibAPI(unsigned int) LinxAiGetRefSetVoltage()
{
	return LinxAiGetRefSetVoltageRef(gLinxDev);
}

LibAPI(unsigned int) LinxAoGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoRefSet;
	return 0;
}

LibAPI(unsigned int) LinxAoGetRefSetVoltage()
{
	return LinxAoGetRefSetVoltageRef(gLinxDev);
}

LibAPI(unsigned char) LinxAiGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiResolution;
	return 0;
}

LibAPI(unsigned char) LinxAiGetResolution()
{
	return LinxAiGetResolutionRef(gLinxDev);
}

LibAPI(unsigned char) LinxAoGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoResolution;
	return 0;
}

LibAPI(unsigned char) LinxAoGetResolution()
{
	return LinxAoGetResolutionRef(gLinxDev);
}

LibAPI(int) LinxAiGetChansRef(LinxDevice *dev, unsigned char *buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAiChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(int) LinxAoGetChansRef(LinxDevice *dev, unsigned char *buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAoChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxAiGetNumChans()
{
	return LinxAiGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(unsigned char) LinxAoGetNumChans()
{
	return LinxAoGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAiGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAoGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxAnalogReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->AnalogRead(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxAnalogReadRef(gLinxDev, numChans, channels, values);
}

LibAPI(int) LinxAnalogReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	if (dev)
		return dev->AnalogReadNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return LinxAnalogReadNoPackingRef(gLinxDev, numChans, channels, values);
}

LibAPI(int) LinxAnalogWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->AnalogWrite(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxAnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxAnalogWriteRef(gLinxDev, numChans, channels, values);;
}

//------------------------------------- CAN -------------------------------------
LibAPI(int) LinxCanGetChansRef(LinxDevice *dev, unsigned char *buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxCanChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxCanGetNumChans()
{
	return LinxCanGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxCanGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxCanGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

//------------------------------------- Digital -------------------------------------
LibAPI(int) LinxDigitalGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxDioChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxDigitalGetNumChans()
{
	return LinxDigitalGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDigitalGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxDigitalSetStateRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	if (dev)
		return dev->DigitalSetState(numChans, channels, states);
	return L_DISCONNECT;
}

LibAPI(int) LinxDigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	return LinxDigitalSetStateRef(gLinxDev, numChans, channels, states);
}

LibAPI(int) LinxDigitalReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalRead(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDigitalReadRef(gLinxDev, numChans, channels, values);
}

LibAPI(int) LinxDigitalReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalReadNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDigitalReadNoPackingRef(gLinxDev, numChans, channels, values);
}

LibAPI(int) LinxDigitalWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWrite(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDigitalWriteRef(gLinxDev, numChans, channels, values);
}

LibAPI(int) LinxDigitalWriteNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWriteNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDigitalWriteNoPackingRef(gLinxDev, numChans, channels, values);
}

//------------------------------------- I2C -------------------------------------
LibAPI(int) LinxI2cGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxI2cChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxI2cGetNumChans()
{
	return LinxI2cGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxI2cGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxI2cOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cOpenMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxI2cOpenMaster(unsigned char channel)
{
	return LinxI2cOpenMasterRef(gLinxDev, channel);
}

LibAPI(int) LinxI2cSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	if (dev)
		return dev->I2cSetSpeed(channel, speed, actualSpeed);
	return L_DISCONNECT;
}

LibAPI(int) LinxI2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return LinxI2cSetSpeedRef(gLinxDev, channel, speed, actualSpeed);;
}

LibAPI(int) LinxI2cWriteRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	if (dev)
		return dev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
	return L_DISCONNECT;
}

LibAPI(int) LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxI2cWriteRef(gLinxDev, channel, slaveAddress, eofConfig, numBytes, sendBuffer);;
}

LibAPI(int) LinxI2cReadRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	if (dev)
		return dev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
	return L_DISCONNECT;
}

LibAPI(int) LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxI2cReadRef(gLinxDev, channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);;
}

LibAPI(int) LinxI2cCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cClose(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxI2cClose(unsigned char channel)
{
	return LinxI2cCloseRef(gLinxDev, channel);
}

//------------------------------------- PWM -------------------------------------
LibAPI(int) LinxPwmGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxPwmChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxPwmGetNumChans()
{
	return LinxPwmGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxPwmGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxPwmSetDutyCycleRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->PwmSetDutyCycle(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int) LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxPwmSetDutyCycleRef(gLinxDev, numChans, channels, values);
}

//------------------------------------- QE -------------------------------------
LibAPI(int) LinxQeGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxQeChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxQeGetNumChans()
{
	return LinxQeGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxQeGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

//------------------------------------- Servo -------------------------------------
LibAPI(int) LinxServoGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxServoChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxServoGetNumChans()
{
	return LinxServoGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxServoGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

//------------------------------------- SPI -------------------------------------
LibAPI(int) LinxSpiGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxSpiChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxSpiGetNumChans()
{
	return LinxSpiGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxSpiGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxSpiOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiOpenMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiOpenMaster(unsigned char channel)
{
	return LinxSpiOpenMasterRef(gLinxDev, channel);
}

LibAPI(int) LinxSpiSetBitOrderRef(LinxDevice *dev, unsigned char channel, unsigned char bitOrder)
{
	if (dev)
		return dev->SpiSetBitOrder(channel, bitOrder);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxSpiSetBitOrderRef(gLinxDev, channel, bitOrder);
}

LibAPI(int) LinxSpiSetModeRef(LinxDevice *dev, unsigned char channel, unsigned char mode)
{
	if (dev)
		return dev->SpiSetMode(channel, mode);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxSpiSetModeRef(gLinxDev, channel, mode);
}

LibAPI(int) LinxSpiSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	if (dev)
		return dev->SpiSetSpeed(channel, speed, actualSpeed);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return LinxSpiSetSpeedRef(gLinxDev, channel, speed, actualSpeed);
}

LibAPI(int) LinxSpiWriteReadRef(LinxDevice *dev, unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (dev)
		return dev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxSpiWriteReadRef(gLinxDev, channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

LibAPI(int) LinxSpiCloseMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiCloseMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxSpiCloseMaster(unsigned char channel)
{
	return LinxSpiCloseMasterRef(gLinxDev, channel);
}

//------------------------------------- UART -------------------------------------
LibAPI(int) LinxUartGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxUartChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(unsigned char) LinxUartGetNumChans()
{
	return LinxUartGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int) LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxUartGetChansRef(gLinxDev, channels, (unsigned int*)&numChans);
}

LibAPI(int) LinxUartOpenRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->UartOpen(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartOpenNameRef(LinxDevice *dev, const unsigned char *name, unsigned char *channel)
{
	if (dev)
		return dev->UartOpen(name, channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartSetBaudRateRef(LinxDevice *dev, unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	if (dev)
		return dev->UartSetBaudRate(channel, baudRate, actualBaud);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return LinxUartSetBaudRateRef(gLinxDev, channel, baudRate, actualBaud);
}

LibAPI(int) LinxUartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	int status = LinxUartOpenRef(gLinxDev, channel);
	if (!status)
		status = LinxUartSetBaudRateRef(gLinxDev, channel, baudRate, actualBaud);
	return status;
}

LibAPI(int) LinxUartSetParametersRef(LinxDevice *dev, unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	if (dev)
		return dev->UartSetParameters(channel, dataBits, stopBits, parity);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartSetParameters(unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	return LinxUartSetParametersRef(gLinxDev, channel, dataBits, stopBits, parity);
}

LibAPI(int) LinxUartGetBytesAvailableRef(LinxDevice *dev, unsigned char channel, unsigned int *numBytes)
{
	if (dev)
		return dev->UartGetBytesAvailable(channel, numBytes);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxUartGetBytesAvailableRef(gLinxDev, channel, (unsigned int*)numBytes);
}

LibAPI(int) LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, int timeout, unsigned int* numBytesRead)
{
	if (dev)
		return dev->UartRead(channel, numBytes, recBuffer, timeout, numBytesRead);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartRead2(unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, int timeout, unsigned int* numBytesRead)
{
	return LinxUartReadRef(gLinxDev, channel, numBytes, recBuffer, timeout, numBytesRead);
}

LibAPI(int) LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxUartReadRef(gLinxDev, channel, numBytes, recBuffer, TIMEOUT_INFINITE, (unsigned int*)numBytesRead);
}

LibAPI(int) LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer, int timeout)
{
	if (dev)
		return dev->UartWrite(channel, numBytes, sendBuffer, timeout);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartWrite2(unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer, int timeout)
{
	return LinxUartWriteRef(gLinxDev, channel, numBytes, sendBuffer, timeout);
}

LibAPI(int) LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxUartWriteRef(gLinxDev, channel, numBytes, sendBuffer, TIMEOUT_INFINITE);
}

LibAPI(int) LinxUartCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->UartClose(channel);
	return L_DISCONNECT;
}

LibAPI(int) LinxUartClose(unsigned char channel)
{
	return LinxUartCloseRef(gLinxDev, channel);
}
