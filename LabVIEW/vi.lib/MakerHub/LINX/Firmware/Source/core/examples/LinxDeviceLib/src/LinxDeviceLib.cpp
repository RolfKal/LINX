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

//------------------------------------- Raspberry Pi -------------------------------------
#if LINX_DEVICE_FAMILY == 4 
	#if LINX_DEVICE_ID == 3	//RPI 2 B
			#define LINXDEVICETYPE LinxRaspberryPi2B
			#include "LinxRaspberryPi.h"
			#include "LinxRaspberryPi2B.h"
	#endif
//------------------------------------- Beagle Bone -------------------------------------
#elif LINX_DEVICE_FAMILY == 6
	#if LINX_DEVICE_ID == 1
			#define LINXDEVICETYPE LinxBeagleBoneBlack
			#include "LinxBeagleBone.h"
			#include "LinxBeagleBoneBlack.h"
	#endif
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
		memcpy(name, dev->DeviceName, min(dev->DeviceNameLen, len - 1);
		name[len - 1] = '\0';
		return L_OK;
	}
	return L_BADPARAM;
}

extern "C" int LinxGetDeviceName(unsigned char *name)
{
	return LinxGetDeviceNameRef(LinxDev, name, 64);
}

//------------------------------------- General -------------------------------------
extern "C" unsigned long LinxGetMilliSecondsRef(LinxDevice *dev)
{
	if (dev)
		return dev->GetMilliSeconds();
	return 0;
}

extern "C" unsigned long LinxGetMilliSeconds()
{
	return LinxDev->GetMilliSeconds();
}

//------------------------------------- Analog -------------------------------------
extern "C" unsigned long LinxAiGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AiRefSet;
	return 0;
}

extern "C" unsigned long LinxAiGetRefSetVoltage()
{
	return LinxDev->AiRefSet;
}

extern "C" unsigned long LinxAoGetRefSetVoltageRef(LinxDevice *dev)
{
	if (dev)
		return dev->AoRefSet;
	return 0;
}

extern "C" unsigned long LinxAoGetRefSetVoltage()
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
		return dev->NumAiChans;
	return 0;
}

extern "C" unsigned char LinxAiGetNumChans()
{
	return LinxDev->NumAiChans;
}

extern "C" unsigned char LinxAoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumAoChans;
	return 0;
}

extern "C" unsigned char LinxAoGetNumChans()
{
	return LinxDev->NumAoChans;
}

extern "C" int LinxAiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumAiChans)
		{
			//Copy All Channels
			numChans = dev->NumAiChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->AiChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAiGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxAoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumAoChans)
		{
			//Copy All Channels
			numChans = dev->NumAoChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->AoChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAoGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxAnalogReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->AnalogRead(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogRead(numChans, channels, values);
}

extern "C" int LinxAnalogReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	if (dev)
		return dev->AnalogReadNoPacking(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	return LinxDev->AnalogReadNoPacking(numChans, channels, values);
}

//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumCanChans;
	return 0;
}

extern "C" unsigned char LinxCanGetNumChans()
{
	return LinxDev->NumCanChans;
}

extern "C" int LinxCanGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumCanChans)
		{
			//Copy All Channels
			numChans = dev->NumCanChans;
		}
		memcpy(channels, dev->CanChans, numChans);
		return numChans;
	}
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
		return dev->NumDigitalChans;
	return 0;
}

extern "C" unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->NumDigitalChans;
}

extern "C" int LinxDigitalGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumDigitalChans)
		{
			//Copy All Channels
			numChans = dev->NumDigitalChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->DigitalChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDigitalGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxDigitalReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalRead(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}

extern "C" int LinxDigitalReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalReadNoPacking(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalReadNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWrite(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}

extern "C" int LinxDigitalWriteNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->DigitalWriteNoPacking(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWriteNoPacking(numChans, channels, values);
}

//------------------------------------- I2C -------------------------------------
extern "C" unsigned char LinxI2cGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumI2cChans;
	return 0;
}

extern "C" unsigned char LinxI2cGetNumChans()
{
	return LinxDev->NumI2cChans;
}

extern "C" int LinxI2cGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumI2cChans)
		{
			//Copy All Channels
			numChans = dev->NumI2cChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->I2cChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxI2cGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxI2cOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cOpenMaster(channel);
	return L_BADPARAM;
}

extern "C" int LinxI2cOpenMaster(unsigned char channel)
{
	return LinxDev->I2cOpenMaster(channel);
}

extern "C" int LinxI2cSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	if (dev)
		return dev->I2cSetSpeed(channel, speed, actualSpeed);
	return L_BADPARAM;
}

extern "C" int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->I2cSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxI2cWriteRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	if (dev)
		return dev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
	return L_BADPARAM;
}

extern "C" int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

extern "C" int LinxI2cReadRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	if (dev)
		return dev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
	return L_BADPARAM;
}

extern "C" int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxDev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

extern "C" int LinxI2cCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->I2cClose(channel);
	return L_BADPARAM;
}

extern "C" int LinxI2cClose(unsigned char channel)
{
	return LinxDev->I2cClose(channel);
}

//------------------------------------- PWM -------------------------------------
extern "C" unsigned char LinxPwmGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumPwmChans;
	return 0;
}

extern "C" unsigned char LinxPwmGetNumChans()
{
	return LinxDev->NumPwmChans;
}

extern "C" int LinxPwmGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumPwmChans)
		{
			//Copy All Channels
			numChans = dev->NumPwmChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->PwmChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxPwmGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxPwmSetDutyCycleRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (dev)
		return dev->PwmSetDutyCycle(numChans, channels, values);
	return L_BADPARAM;
}

extern "C" int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}

//------------------------------------- QE -------------------------------------
extern "C" unsigned char LinxQeGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumQeChans;
	return 0;
}

extern "C" unsigned char LinxQeGetNumChans()
{
	return LinxDev->NumQeChans;
}

extern "C" int LinxQeGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumQeChans)
		{
			//Copy All Channels
			numChans = dev->NumQeChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->QeChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxQeGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumServoChans;
	return 0;
}

extern "C" unsigned char LinxServoGetNumChans()
{
	return LinxDev->NumServoChans;
}

extern "C" int LinxServoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumServoChans)
		{
			//Copy All Channels
			numChans = dev->NumServoChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->ServoChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxServoGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumSpiChans;
	return 0;
}

extern "C" unsigned char LinxSpiGetNumChans()
{
	return LinxDev->NumSpiChans;
}

extern "C" int LinxSpiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumSpiChans)
		{
			//Copy All Channels
			numChans = dev->NumSpiChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->SpiChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxSpiGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxSpiOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiOpenMaster(channel);
	return L_BADPARAM;
}

extern "C" int LinxSpiOpenMaster(unsigned char channel)
{
	return LinxDev->SpiOpenMaster(channel);
}

extern "C" int LinxSpiSetBitOrderRef(LinxDevice *dev, unsigned char channel, unsigned char bitOrder)
{
	if (dev)
		return dev->SpiSetBitOrder(channel, bitOrder);
	return L_BADPARAM;
}

extern "C" int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxDev->SpiSetBitOrder(channel, bitOrder);
}

extern "C" int LinxSpiSetModeRef(LinxDevice *dev, unsigned char channel, unsigned char mode)
{
	if (dev)
		return dev->SpiSetMode(channel, mode);
	return L_BADPARAM;
}

extern "C" int LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxDev->SpiSetMode(channel, mode);
}

extern "C" int LinxSpiSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	if (dev)
		return dev->SpiSetSpeed(channel, speed, actualSpeed);
	return L_BADPARAM;
}

extern "C" int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->SpiSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxSpiWriteReadRef(LinxDevice *dev, unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (dev)
		return dev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
	return L_BADPARAM;
}

extern "C" int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxDev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

extern "C" int LinxSpiCloseMasterRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->SpiCloseMaster(channel);
	return L_BADPARAM;
}

extern "C" int LinxSpiCloseMaster(unsigned char channel)
{
	return LinxDev->SpiCloseMaster(channel);
}

//------------------------------------- UART -------------------------------------
extern "C" unsigned char LinxUartGetNumChansRef(LinxDevice *dev)
{
	if (dev)
		return dev->NumUartChans;
	return 0;
}

extern "C" unsigned char LinxUartGetNumChans()
{
	return LinxDev->NumUartChans;
}

extern "C" int LinxUartGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (dev)
	{
		if (numChans > dev->NumUartChans)
		{
			//Copy All Channels
			numChans = dev->NumUartChans;
		}
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, dev->UartChans, numChans);
		return numChans;
	}
	return 0;
}

extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxUartGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxUartOpenRef(LinxDevice *dev, unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	if (dev)
		return dev->UartOpen(channel, baudRate, actualBaud);
	return L_BADPARAM;
}

extern "C" int LinxUartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartOpen(channel, baudRate, actualBaud);
}

extern "C" int LinxUartSetBaudRateRef(LinxDevice *dev, unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	if (dev)
		return dev->UartSetBaudRate(channel, baudRate, actualBaud);
	return L_BADPARAM;
}

extern "C" int LinxUartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
}

extern "C" int LinxUartGetBytesAvailableRef(LinxDevice *dev, unsigned char channel, unsigned char *numBytes)
{
	if (dev)
		return dev->UartGetBytesAvailable(channel, numBytes);
	return L_BADPARAM;
}

extern "C" int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxDev->UartGetBytesAvailable(channel, numBytes);
}

extern "C" int LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	if (dev)
		return dev->UartRead(channel, numBytes, recBuffer, numBytesRead);
	return L_BADPARAM;
}

extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

extern "C" int LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	if (dev)
		return dev->UartWrite(channel, numBytes, sendBuffer);
	return L_BADPARAM;
}

extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer);
}

extern "C" int LinxUartCloseRef(LinxDevice *dev, unsigned char channel)
{
	if (dev)
		return dev->UartClose(channel);
	return L_BADPARAM;
}

extern "C" int LinxUartClose(unsigned char channel)
{
	return LinxDev->UartClose(channel);
}
