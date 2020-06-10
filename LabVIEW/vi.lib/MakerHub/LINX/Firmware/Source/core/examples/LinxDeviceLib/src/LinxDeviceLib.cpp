/****************************************************************************************
**  LINX Device Abstraction Layer (Helper Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#include <stdio.h>
#include <iostream>
#include <string.h>

#include "LinxDevice.h"
#include "LinxLinuxDevice.h"

//------------------------------------- Raspberry Pi -------------------------------------
#if LINX_DEVICE_FAMILY == 4 
	#if LINX_DEVICE_ID >= 3	//RPI 2 B
			#define LINXDEVICETYPE LinxRaspberryPi
			#include "LinxRaspberryPi.h"
	#endif
//------------------------------------- Beagle Bone -------------------------------------
#elif LINX_DEVICE_FAMILY == 6
	#if LINX_DEVICE_ID == 1
			#define LINXDEVICETYPE LinxBeagleBoneBlack
			#include "LinxBeagleBoneBlack.h"
	#endif
#else
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

extern "C" LinxDevice * LinxOpenTCP(char address, short port, int timeout)
{
	return NULL;
}

extern "C" LinxDevice * LinxOpenSerial(char address, int timeout)
{
	return NULL;
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
		return dev->GetAiChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxAiGetNumChans()
{
	return LinxDev->GetAiChans(NULL, 0);
}

extern "C" unsigned char LinxAoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetAoChans(NULL, 0);;
	return 0;
}

extern "C" unsigned char LinxAoGetNumChans()
{
	return LinxDev->GetAoChans(NULL, 0);;
}

extern "C" int LinxAiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->GetAiChans(channels, numChans);
	return 0;
}

extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetAiChans(channels, numChans);
}

extern "C" int LinxAoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->GetAoChans(channels, numChans);
	return 0;
}

extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetAoChans(channels, numChans);
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

//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetCanChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxCanGetNumChans()
{
	return LinxDev->GetCanChans(NULL, 0);
}

extern "C" int LinxCanGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		return dev->GetCanChans(channels, numChans);
	return 0;
}

extern "C" int LinxCanGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxCanGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- Digital -------------------------------------
extern "C" unsigned char LinxDigitalGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetDioChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->GetDioChans(NULL, 0);
}

extern "C" int LinxDigitalGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetDioChans(channels, numChans);
	return 0;
}

extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetDioChans(channels, numChans);
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
		return dev->GetI2cChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxI2cGetNumChans()
{
	return LinxDev->GetI2cChans(NULL, 0);
}

extern "C" int LinxI2cGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetI2cChans(channels, numChans);
	return 0;
}

extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetI2cChans(channels, numChans);
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
		return dev->GetPwmChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxPwmGetNumChans()
{
	return LinxDev->GetPwmChans(NULL, 0);
}

extern "C" int LinxPwmGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetPwmChans(channels, numChans);
	return 0;
}

extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetPwmChans(channels, numChans);
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
		return dev->GetQeChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxQeGetNumChans()
{
	return LinxDev->GetQeChans(NULL, 0);
}

extern "C" int LinxQeGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetQeChans(channels, numChans);
	return 0;
}

extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetQeChans(channels, numChans);
}

//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetServoChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxServoGetNumChans()
{
	return LinxDev->GetServoChans(NULL, 0);
}

extern "C" int LinxServoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetServoChans(channels, numChans);
	return 0;
}

extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetServoChans(channels, numChans);
}

//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetSpiChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxSpiGetNumChans()
{
	return LinxDev->GetSpiChans(NULL, 0);
}

extern "C" int LinxSpiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetSpiChans(channels, numChans);
	return 0;
}

extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetSpiChans(channels, numChans);
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
		return dev->GetUartChans(NULL, 0);
	return 0;
}

extern "C" unsigned char LinxUartGetNumChans()
{
	return LinxDev->GetUartChans(NULL, 0);
}

extern "C" int LinxUartGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
		dev->GetUartChans(channels, numChans);
	return 0;
}

extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDev->GetUartChans(channels, numChans);
}

extern "C" int LinxUartOpenRef(LinxDevice *dev, unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	if (dev)
		return dev->UartOpen(channel, baudRate, actualBaud);
	return LERR_BADPARAM;
}

extern "C" int LinxUartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return LinxDev->UartOpen(channel, baudRate, actualBaud);
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

extern "C" int LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	if (dev)
		return dev->UartRead(channel, numBytes, recBuffer, numBytesRead);
	return LERR_BADPARAM;
}

extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

extern "C" int LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	if (dev)
		return dev->UartWrite(channel, numBytes, sendBuffer);
	return LERR_BADPARAM;
}

extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer);
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
