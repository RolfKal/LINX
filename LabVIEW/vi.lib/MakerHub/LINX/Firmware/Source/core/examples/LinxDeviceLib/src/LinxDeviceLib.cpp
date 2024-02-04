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

//------------------------------------- Constructor / Destructor -------------------------------------
LibAPI(LinxDevice *) LinxOpenLocalClient(void)
{
	//Instantiate the LINX Device
	return new LINXDEVICETYPE();
}

LibAPI(int32_t) LinxOpen(void)
{
	if (!gLinxDev)
	{
		//Instantiate The LINX Device
		gLinxDev = LinxOpenLocalClient();
	}
	return L_OK;
}

LibAPI(LinxDevice *) LinxOpenSerialClient(const unsigned char *deviceName, uint32_t *baudrate, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity, int32_t timeout)
{
	LinxClient *client = new LinxClient(deviceName, baudrate, dataBits, stopBits, parity, timeout);
	if (client && !client->IsInitialized())
	{
		client->Release();
		return NULL;
	}
	return client;
}

LibAPI(LinxDevice *) LinxOpenTCPClient(const unsigned char *clientAddress, uint16_t port, int32_t timeout)
{
	LinxClient *client = new LinxClient(clientAddress, port, timeout);
	if (client && !client->IsInitialized())
	{
		client->Release();
		return NULL;
	}
	return client;
}

LibAPI(LinxListener *) LinxOpenSerialServer(LinxDevice *dev, const unsigned char *deviceName, uint32_t baudRate, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity, int32_t timeout, bool autostart)
{
	if (!dev)
		dev = LinxOpenLocalClient();
	else
		dev->AddRef();

	LinxSerialListener *listener = new LinxSerialListener(dev, autostart);
	if (listener)
	{
		int32_t status = listener->Start(deviceName, baudRate, dataBits, stopBits, parity, timeout);
		if (status)
		{
			listener->Release();
			return NULL;
		}
	}
	return listener;
}

LibAPI(LinxListener *) LinxOpenTCPServer(LinxDevice *dev, const unsigned char *interfaceAddress, uint16_t port, int32_t timeout, bool autostart)
{
	if (!dev)
		dev = LinxOpenLocalClient();
	else
		dev->AddRef();

	LinxTcpListener *listener = new LinxTcpListener(dev, autostart);
	if (listener)
	{
		int32_t status = listener->Start(interfaceAddress, port, timeout);
		if (status)
		{
			listener->Release();
			return NULL;
		}
	}
	return listener;
}

LibAPI(int32_t) LinxServerProcess(LinxListener *listener, bool loop)
{
	if (listener)
		return listener->ProcessLoop(loop);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxCloseRef(LinxBase *base)
{
	if (base)
		base->Release();
	return L_OK;
}

LibAPI(int32_t) LinxClose(void)
{
	int32_t status = LinxCloseRef(gLinxDev);
	gLinxDev = NULL;
	return status;
}
//------------------------------------- Enumeration -------------------------------------
LibAPI(uint8_t) LinxGetDeviceFamilyRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceFamily;
	return 0;
}

LibAPI(uint8_t) LinxGetDeviceFamily(void)
{
	return LinxGetDeviceFamilyRef(gLinxDev);
}

LibAPI(uint8_t) LinxGetDeviceIdRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceId;
	return 0;
}

LibAPI(uint8_t) LinxGetDeviceId(void)
{
	return LinxGetDeviceIdRef(gLinxDev);
}

LibAPI(int32_t) LinxGetDeviceNameRef(LinxDevice *dev, unsigned char *name, int32_t len)
{
	if (dev)
	{
		dev->GetDeviceName(name, len);
		return L_OK;
	}
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxGetDeviceName(unsigned char *name)
{
	return LinxGetDeviceNameRef(gLinxDev, name, 64);
}

//------------------------------------- General -------------------------------------
LibAPI(uint32_t) LinxGetMilliSecondsRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetMilliSeconds();
	return 0;
}

LibAPI(uint32_t) LinxGetMilliSeconds(void)
{
	return LinxGetMilliSecondsRef(gLinxDev);
}

//------------------------------------- Analog -------------------------------------
LibAPI(uint32_t) LinxAiGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiRefSet;
	return 0;
}

LibAPI(uint32_t) LinxAiGetRefSetVoltage(void)
{
	return LinxAiGetRefSetVoltageRef(gLinxDev);
}

LibAPI(uint32_t) LinxAoGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoRefSet;
	return 0;
}

LibAPI(uint32_t) LinxAoGetRefSetVoltage(void)
{
	return LinxAoGetRefSetVoltageRef(gLinxDev);
}

LibAPI(uint8_t) LinxAiGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiResolution;
	return 0;
}

LibAPI(uint8_t) LinxAiGetResolution(void)
{
	return LinxAiGetResolutionRef(gLinxDev);
}

LibAPI(uint8_t) LinxAoGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoResolution;
	return 0;
}

LibAPI(uint8_t) LinxAoGetResolution(void)
{
	return LinxAoGetResolutionRef(gLinxDev);
}

LibAPI(int32_t) LinxAiGetChansRef(LinxDevice *dev, uint8_t *buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAiChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(int32_t) LinxAoGetChansRef(LinxDevice *dev, uint8_t *buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAoChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxAiGetNumChans(void)
{
	return LinxAiGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(uint8_t) LinxAoGetNumChans(void)
{
	return LinxAoGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxAiGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxAiGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxAoGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxAoGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxAnalogReadRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->AnalogRead(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxAnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxAnalogReadRef(gLinxDev, numChans, channels, values);
}

LibAPI(int32_t) LinxAnalogReadNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint32_t* values)
{
	if (dev)
		return dev->AnalogReadNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxAnalogReadNoPacking(uint8_t numChans, uint8_t* channels, uint32_t* values)
{
	return LinxAnalogReadNoPackingRef(gLinxDev, numChans, channels, values);
}

LibAPI(int32_t) LinxAnalogWriteRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->AnalogWrite(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxAnalogWrite(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxAnalogWriteRef(gLinxDev, numChans, channels, values);;
}

//------------------------------------- CAN -------------------------------------
LibAPI(int32_t) LinxCanGetChansRef(LinxDevice *dev, uint8_t *buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxCanChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxCanGetNumChans(void)
{
	return LinxCanGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxCanGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxCanGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

//------------------------------------- Digital -------------------------------------
LibAPI(int32_t) LinxDigitalGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxDioChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxDigitalGetNumChans(void)
{
	return LinxDigitalGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxDigitalGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxDigitalGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxDigitalSetStateRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* states)
{
	if (dev)
		return dev->DigitalSetState(numChans, channels, states);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxDigitalSetState(uint8_t numChans, uint8_t* channels, uint8_t* states)
{
	return LinxDigitalSetStateRef(gLinxDev, numChans, channels, states);
}

LibAPI(int32_t) LinxDigitalReadRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->DigitalRead(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxDigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxDigitalReadRef(gLinxDev, numChans, channels, values);
}

LibAPI(int32_t) LinxDigitalReadNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->DigitalReadNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxDigitalReadNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxDigitalReadNoPackingRef(gLinxDev, numChans, channels, values);
}

LibAPI(int32_t) LinxDigitalWriteRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->DigitalWrite(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxDigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxDigitalWriteRef(gLinxDev, numChans, channels, values);
}

LibAPI(int32_t) LinxDigitalWriteNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->DigitalWriteNoPacking(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxDigitalWriteNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxDigitalWriteNoPackingRef(gLinxDev, numChans, channels, values);
}

//------------------------------------- I2C -------------------------------------
LibAPI(int32_t) LinxI2cGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxI2cChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxI2cGetNumChans(void)
{
	return LinxI2cGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxI2cGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxI2cGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxI2cOpenMasterRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->I2cOpenMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxI2cOpenMaster(uint8_t channel)
{
	return LinxI2cOpenMasterRef(gLinxDev, channel);
}

LibAPI(int32_t) LinxI2cSetSpeedRef(LinxDevice *dev, uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	if (dev)
		return dev->I2cSetSpeed(channel, speed, actualSpeed);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxI2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	return LinxI2cSetSpeedRef(gLinxDev, channel, speed, actualSpeed);;
}

LibAPI(int32_t) LinxI2cWriteRef(LinxDevice *dev, uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint8_t* sendBuffer)
{
	if (dev)
		return dev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxI2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer)
{
	return LinxI2cWriteRef(gLinxDev, channel, slaveAddress, eofConfig, numBytes, sendBuffer);;
}

LibAPI(int32_t) LinxI2cReadRef(LinxDevice *dev, uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer)
{
	if (dev)
		return dev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxI2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer)
{
	return LinxI2cReadRef(gLinxDev, channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);;
}

LibAPI(int32_t) LinxI2cCloseRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->I2cClose(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxI2cClose(uint8_t channel)
{
	return LinxI2cCloseRef(gLinxDev, channel);
}

//------------------------------------- PWM -------------------------------------
LibAPI(int32_t) LinxPwmGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxPwmChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxPwmGetNumChans(void)
{
	return LinxPwmGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxPwmGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxPwmGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxPwmSetDutyCycleRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	if (dev)
		return dev->PwmSetDutyCycle(numChans, channels, values);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxPwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	return LinxPwmSetDutyCycleRef(gLinxDev, numChans, channels, values);
}

//------------------------------------- QE -------------------------------------
LibAPI(int32_t) LinxQeGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxQeChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxQeGetNumChans(void)
{
	return LinxQeGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxQeGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxQeGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

//------------------------------------- Servo -------------------------------------
LibAPI(int32_t) LinxServoGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxServoChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxServoGetNumChans(void)
{
	return LinxServoGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxServoGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxServoGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

//------------------------------------- SPI -------------------------------------
LibAPI(int32_t) LinxSpiGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxSpiChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxSpiGetNumChans(void)
{
	return LinxSpiGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxSpiGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxSpiGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxSpiOpenMasterRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->SpiOpenMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiOpenMaster(uint8_t channel)
{
	return LinxSpiOpenMasterRef(gLinxDev, channel);
}

LibAPI(int32_t) LinxSpiSetBitOrderRef(LinxDevice *dev, uint8_t channel, uint8_t bitOrder)
{
	if (dev)
		return dev->SpiSetBitOrder(channel, bitOrder);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiSetBitOrder(uint8_t channel, uint8_t bitOrder)
{
	return LinxSpiSetBitOrderRef(gLinxDev, channel, bitOrder);
}

LibAPI(int32_t) LinxSpiSetModeRef(LinxDevice *dev, uint8_t channel, uint8_t mode)
{
	if (dev)
		return dev->SpiSetMode(channel, mode);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiSetMode(uint8_t channel, uint8_t mode)
{
	return LinxSpiSetModeRef(gLinxDev, channel, mode);
}

LibAPI(int32_t) LinxSpiSetSpeedRef(LinxDevice *dev, uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	if (dev)
		return dev->SpiSetSpeed(channel, speed, actualSpeed);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	return LinxSpiSetSpeedRef(gLinxDev, channel, speed, actualSpeed);
}

LibAPI(int32_t) LinxSpiWriteReadRef(LinxDevice *dev, uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (dev)
		return dev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxSpiWriteReadRef(gLinxDev, channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

LibAPI(int32_t) LinxSpiCloseMasterRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->SpiCloseMaster(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxSpiCloseMaster(uint8_t channel)
{
	return LinxSpiCloseMasterRef(gLinxDev, channel);
}

//------------------------------------- UART -------------------------------------
LibAPI(int32_t) LinxUartGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxUartChannel, buffer, bufLen ? *bufLen : 0, bufLen);
	return 0;
}

LibAPI(uint8_t) LinxUartGetNumChans(void)
{
	return LinxUartGetChansRef(gLinxDev, NULL, NULL);
}

LibAPI(int32_t) LinxUartGetChans(uint8_t numChans, uint8_t* channels)
{
	return LinxUartGetChansRef(gLinxDev, channels, (uint32_t*)&numChans);
}

LibAPI(int32_t) LinxUartOpenRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->UartOpen(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartOpenNameRef(LinxDevice *dev, const unsigned char*name, uint8_t *channel)
{
	if (dev)
		return dev->UartOpen(name, channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartSetBaudRateRef(LinxDevice *dev, uint8_t channel, uint32_t baudRate, uint32_t* actualBaud)
{
	if (dev)
		return dev->UartSetBaudRate(channel, baudRate, actualBaud);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud)
{
	return LinxUartSetBaudRateRef(gLinxDev, channel, baudRate, actualBaud);
}

LibAPI(int32_t) LinxUartOpen(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud)
{
	int32_t status = LinxUartOpenRef(gLinxDev, channel);
	if (!status)
		status = LinxUartSetBaudRateRef(gLinxDev, channel, baudRate, actualBaud);
	return status;
}

LibAPI(int32_t) LinxUartSetParametersRef(LinxDevice *dev, uint8_t channel, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity)
{
	if (dev)
		return dev->UartSetParameters(channel, dataBits, stopBits, parity);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartSetParameters(uint8_t channel, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity)
{
	return LinxUartSetParametersRef(gLinxDev, channel, dataBits, stopBits, parity);
}

LibAPI(int32_t) LinxUartGetBytesAvailableRef(LinxDevice *dev, uint8_t channel, uint32_t *numBytes)
{
	if (dev)
		return dev->UartGetBytesAvailable(channel, numBytes);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartGetBytesAvailable(uint8_t channel, uint8_t *numBytes)
{
	return LinxUartGetBytesAvailableRef(gLinxDev, channel, (uint32_t*)numBytes);
}

LibAPI(int32_t) LinxUartReadRef(LinxDevice *dev, uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead)
{
	if (dev)
		return dev->UartRead(channel, numBytes, recBuffer, timeout, numBytesRead);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartRead2(uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead)
{
	return LinxUartReadRef(gLinxDev, channel, numBytes, recBuffer, timeout, numBytesRead);
}

LibAPI(int32_t) LinxUartRead(uint8_t channel, uint8_t numBytes, unsigned char* recBuffer, uint8_t* numBytesRead)
{
	return LinxUartReadRef(gLinxDev, channel, numBytes, recBuffer, TIMEOUT_INFINITE, (uint32_t*)numBytesRead);
}

LibAPI(int32_t) LinxUartWriteRef(LinxDevice *dev, uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout)
{
	if (dev)
		return dev->UartWrite(channel, numBytes, sendBuffer, timeout);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartWrite2(uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout)
{
	return LinxUartWriteRef(gLinxDev, channel, numBytes, sendBuffer, timeout);
}

LibAPI(int32_t) LinxUartWrite(uint8_t channel, uint8_t numBytes, unsigned char* sendBuffer)
{
	return LinxUartWriteRef(gLinxDev, channel, numBytes, sendBuffer, TIMEOUT_INFINITE);
}

LibAPI(int32_t) LinxUartCloseRef(LinxDevice *dev, uint8_t channel)
{
	if (dev)
		return dev->UartClose(channel);
	return L_DISCONNECT;
}

LibAPI(int32_t) LinxUartClose(uint8_t channel)
{
	return LinxUartCloseRef(gLinxDev, channel);
}
