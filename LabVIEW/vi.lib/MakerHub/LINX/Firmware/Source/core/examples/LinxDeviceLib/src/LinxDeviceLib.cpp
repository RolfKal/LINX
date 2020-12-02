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
#elif Win32
			#define LINXDEVICETYPE LinxWindowsDevice
			#include "LinxWindowsDevice.h"
#endif

LinxDevice* LinxDev;

//------------------------------------- Constructor / Destructor -------------------------------------
extern "C" LinxDevice * LinxOpenRef()
{
	//Instantiate The LINX Device
	return new LINXDEVICETYPE();
}

extern "C" int LinxOpen()
{
	//Instantiate The LINX Device
	LinxDev = new LINXDEVICETYPE();
	return L_OK;
}

extern "C" LinxDevice * LinxOpenSerialDevice(const char *deviceName, unsigned int baudrate, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity, int timeout)
{
	LinxClient *client = new LinxClient(deviceName, baudrate, dataBits, stopBits, parity, timeout);
	if (client)
		client->Initialize();
	return client;
}

extern "C" LinxDevice * LinxOpenTCP(const char *address, unsigned short port, int timeout)
{
	LinxClient *client = new LinxClient(address, port, timeout);
	if (client)
		client->Initialize();
	return client;
}

extern "C" int LinxCloseRef(LinxDevice *dev)
{
	if (dev)
		delete dev;
	return L_OK;
}

extern "C" int LinxClose()
{
	delete LinxDev;
	return L_OK;
}

//------------------------------------- Enumeration -------------------------------------
extern "C" int LinxGetDeviceFamilyRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceFamily;
	return 0;
}

extern "C" int LinxGetDeviceFamily()
{
	return LinxDev->DeviceFamily;
}

extern "C" int LinxGetDeviceIdRef(LinxDevice *dev)
{
	if (dev)
		return dev->DeviceId;
	return 0;
}

extern "C" int LinxGetDeviceId()
{
	return LinxDev->DeviceId;
}

extern "C" int LinxGetDeviceNameRef(LinxDevice *dev, unsigned char *name, int len)
{
	if (dev)
	{
		dev->GetDeviceName(name, len);
		return L_OK;
	}
	return LERR_BADPARAM;
}

extern "C" int LinxGetDeviceName(unsigned char *name)
{
	return LinxDev->GetDeviceName(name, 64);
}

//------------------------------------- General -------------------------------------
extern "C" unsigned int LinxGetMilliSecondsRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetMilliSeconds();
	return 0;
}

extern "C" unsigned int LinxGetMilliSeconds()
{
	return LinxDev->GetMilliSeconds();
}

//------------------------------------- Analog -------------------------------------
extern "C" unsigned int LinxAiGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiRefSet;
	return 0;
}

extern "C" unsigned int LinxAiGetRefSetVoltage()
{
	return LinxDev->AiRefSet;
}

extern "C" unsigned int LinxAoGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoRefSet;
	return 0;
}

extern "C" unsigned int LinxAoGetRefSetVoltage()
{
	return LinxDev->AoRefSet;
}

extern "C" unsigned char LinxAiGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiResolution;
	return 0;
}

extern "C" unsigned char LinxAiGetResolution()
{
	return LinxDev->AiResolution;
}

extern "C" unsigned char LinxAoGetResolutionRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoResolution;
	return 0;
}

extern "C" unsigned char LinxAoGetResolution()
{
	return LinxDev->AoResolution;
}

extern "C" unsigned char LinxAiGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAiChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxAiGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxAiChannel, NULL, 0);
}

extern "C" unsigned char LinxAoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAoChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxAoGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxAoChannel, NULL, 0);
}

extern "C" int LinxAiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAiChannel, channels, numChans);
	return 0;
}

extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxAiChannel, channels, numChans);
}

extern "C" int LinxAoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxAoChannel, channels, numChans);
	return 0;
}

extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxAoChannel, channels, numChans);
}

extern "C" int LinxAnalogReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->AnalogRead(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogRead(numChans, channels, values);
}

extern "C" int LinxAnalogReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	if (dev)
		return dev->AnalogReadNoPacking(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return LinxDev->AnalogReadNoPacking(numChans, channels, values);
}

extern "C" int LinxAnalogWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->AnalogWrite(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxAnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogWrite(numChans, channels, values);
}

//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxCanChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxCanGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxCanChannel, NULL, 0);
}

extern "C" int LinxCanGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxCanChannel, channels, numChans);
	return 0;
}

extern "C" int LinxCanGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxCanChannel, channels, numChans); 
}

//------------------------------------- Digital -------------------------------------
extern "C" unsigned char LinxDigitalGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxDioChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxDioChannel, NULL, 0);
}

extern "C" int LinxDigitalGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxDioChannel, channels, numChans);
	return 0;
}

extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxDioChannel, channels, numChans);
}

extern "C" int LinxDigitalSetStateRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	if (dev)
		return dev->DigitalSetState(numChans, channels, states);
	return LERR_BADPARAM;
}

extern "C" int LinxDigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	return LinxDev->DigitalSetState(numChans, channels, states);
}

extern "C" int LinxDigitalReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalRead(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}

extern "C" int LinxDigitalReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalReadNoPacking(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalReadNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWrite(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}

extern "C" int LinxDigitalWriteNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWriteNoPacking(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWriteNoPacking(numChans, channels, values);
}

//------------------------------------- I2C -------------------------------------
extern "C" unsigned char LinxI2cGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxI2cChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxI2cGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxI2cChannel, NULL, 0);
}

extern "C" int LinxI2cGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxI2cChannel, channels, numChans);
	return 0;
}

extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxI2cChannel, channels, numChans);
}

extern "C" int LinxI2cOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cOpenMaster(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxI2cOpenMaster(unsigned char channel)
{
	return LinxDev->I2cOpenMaster(channel);
}

extern "C" int LinxI2cSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	if (dev)
		return dev->I2cSetSpeed(channel, speed, actualSpeed);
	return LERR_BADPARAM;
}

extern "C" int LinxI2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return LinxDev->I2cSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxI2cWriteRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	if (dev)
		return dev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
	return LERR_BADPARAM;
}

extern "C" int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

extern "C" int LinxI2cReadRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	if (dev)
		return dev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
	return LERR_BADPARAM;
}

extern "C" int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxDev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

extern "C" int LinxI2cCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cClose(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxI2cClose(unsigned char channel)
{
	return LinxDev->I2cClose(channel);
}

//------------------------------------- PWM -------------------------------------
extern "C" unsigned char LinxPwmGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxPwmChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxPwmGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxPwmChannel, NULL, 0);
}

extern "C" int LinxPwmGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxPwmChannel, channels, numChans);
	return 0;
}

extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxPwmChannel, channels, numChans);
}

extern "C" int LinxPwmSetDutyCycleRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->PwmSetDutyCycle(numChans, channels, values);
	return LERR_BADPARAM;
}

extern "C" int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}

//------------------------------------- QE -------------------------------------
extern "C" unsigned char LinxQeGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxQeChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxQeGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxQeChannel, NULL, 0);
}

extern "C" int LinxQeGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxQeChannel, channels, numChans);
	return 0;
}

extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxQeChannel, channels, numChans);
}

//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxServoChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxServoGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxServoChannel, NULL, 0);
}

extern "C" int LinxServoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxServoChannel, channels, numChans);
	return 0;
}

extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxServoChannel, channels, numChans);
}

//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxSpiChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxSpiGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxSpiChannel, NULL, 0);
}

extern "C" int LinxSpiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxSpiChannel, channels, numChans);
	return 0;
}

extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxSpiChannel, channels, numChans);
}

extern "C" int LinxSpiOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiOpenMaster(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiOpenMaster(unsigned char channel)
{
	return LinxDev->SpiOpenMaster(channel);
}

extern "C" int LinxSpiSetBitOrderRef(LinxDevice *dev, unsigned char channel, unsigned char bitOrder)
{
	if (dev)
		return dev->SpiSetBitOrder(channel, bitOrder);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxDev->SpiSetBitOrder(channel, bitOrder);
}

extern "C" int LinxSpiSetModeRef(LinxDevice *dev, unsigned char channel, unsigned char mode)
{
	if (dev)
		return dev->SpiSetMode(channel, mode);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxDev->SpiSetMode(channel, mode);
}

extern "C" int LinxSpiSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	if (dev)
		return dev->SpiSetSpeed(channel, speed, actualSpeed);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return LinxDev->SpiSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxSpiWriteReadRef(LinxDevice *dev, unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (dev)
		return dev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxDev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

extern "C" int LinxSpiCloseMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiCloseMaster(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxSpiCloseMaster(unsigned char channel)
{
	return LinxDev->SpiCloseMaster(channel);
}

//------------------------------------- UART -------------------------------------
extern "C" unsigned char LinxUartGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->EnumerateChannels(IID_LinxUartChannel, NULL, 0);
	return 0;
}

extern "C" unsigned char LinxUartGetNumChans()
{
	return LinxDev->EnumerateChannels(IID_LinxUartChannel, NULL, 0);
}

extern "C" int LinxUartGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->EnumerateChannels(IID_LinxUartChannel, channels, numChans);
	return 0;
}

extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->EnumerateChannels(IID_LinxUartChannel, channels, numChans);
}

extern "C" int LinxUartOpenRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->UartOpen(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxUartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	int status = LinxDev->UartOpen(channel);
	if (!status)
		status = LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
	return status;
}

extern "C" int LinxUartOpenByNameRef(LinxDevice *dev, const char *name, unsigned char *channel)
{
	if (dev)
		return dev->UartOpen(name, channel);
	return LERR_BADPARAM;
}

extern "C" int LinxUartSetBaudRateRef(LinxDevice *dev, unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	if (dev)
		return dev->UartSetBaudRate(channel, baudRate, actualBaud);
	return LERR_BADPARAM;
}

extern "C" int LinxUartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
}

extern "C" int LinxUartSetBitSizesRef(LinxDevice *dev, unsigned char channel, unsigned char dataBits, unsigned char stopBits)
{
	if (dev)
		return dev->UartSetBitSizes(channel, dataBits, stopBits);
	return LERR_BADPARAM;
}

extern "C" int LinxUartSetParityRef(LinxDevice *dev, unsigned char channel, LinxUartParity parity)
{
	if (dev)
		return dev->UartSetParity(channel, parity);
	return LERR_BADPARAM;
}

extern "C" int LinxUartGetBytesAvailableRef(LinxDevice *dev, unsigned char channel, unsigned char *numBytes)
{
	if (dev)
		return dev->UartGetBytesAvailable(channel, numBytes);
	return LERR_BADPARAM;
}

extern "C" int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxDev->UartGetBytesAvailable(channel, numBytes);
}

extern "C" int LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, int timeout, unsigned char* numBytesRead)
{
	if (dev)
		return dev->UartRead(channel, numBytes, recBuffer, timeout, numBytesRead);
	return LERR_BADPARAM;
}

extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, TIMEOUT_INFINITE, numBytesRead);
}

extern "C" int LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer, int timeout)
{
	if (dev)
		return dev->UartWrite(channel, numBytes, sendBuffer, timeout);
	return LERR_BADPARAM;
}

extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer, TIMEOUT_INFINITE);
}

extern "C" int LinxUartCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->UartClose(channel);
	return LERR_BADPARAM;
}

extern "C" int LinxUartClose(unsigned char channel)
{
	return LinxDev->UartClose(channel);
}
