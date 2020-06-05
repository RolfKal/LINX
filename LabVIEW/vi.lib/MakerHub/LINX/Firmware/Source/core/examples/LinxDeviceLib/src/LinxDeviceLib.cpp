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
	return dev->DeviceFamily;
}

extern "C" int LinxGetDeviceFamily()
{
	return LinxDev->DeviceFamily;
}

extern "C" int LinxGetDeviceIdRef(LinxDevice *dev)
{
	return dev->DeviceId;
}

extern "C" int LinxGetDeviceId()
{
	return LinxDev->DeviceId;
}

extern "C" int LinxGetDeviceNameRef(LinxDevice *dev, string* name)
{
	memcpy(name, dev->DeviceName, dev->DeviceNameLen);
	return L_OK;
}

extern "C" int LinxGetDeviceName(string* name)
{
	memcpy(name, LinxDev->DeviceName, LinxDev->DeviceNameLen);
	return L_OK;
}

//------------------------------------- General -------------------------------------
extern "C" unsigned long LinxGetMilliSecondsRef(LinxDevice *dev)
{
	return dev->GetMilliSeconds();
}

extern "C" unsigned long LinxGetMilliSeconds()
{
	return LinxDev->GetMilliSeconds();
}


//------------------------------------- Analog -------------------------------------
extern "C" unsigned long LinxAiGetRefSetVoltageRef(LinxDevice *dev)
{
	return dev->AiRefSet;
}

extern "C" unsigned long LinxAiGetRefSetVoltage()
{
	return LinxDev->AiRefSet;
}

extern "C" unsigned long LinxAoGetRefSetVoltageRef(LinxDevice *dev)
{
	return dev->AoRefSet;
}

extern "C" unsigned long LinxAoGetRefSetVoltage()
{
	return LinxDev->AoRefSet;
}

extern "C" unsigned char LinxAiGetResolutionRef(LinxDevice *dev)
{
	return dev->AiResolution;
}

extern "C" unsigned char LinxAiGetResolution()
{
	return LinxDev->AiResolution;
}

extern "C" unsigned char LinxAoGetResolutionRef(LinxDevice *dev)
{
	return dev->AoResolution;
}

extern "C" unsigned char LinxAoGetResolution()
{
	return LinxDev->AoResolution;
}

extern "C" unsigned char LinxAiGetNumChansRef(LinxDevice *dev)
{
	return dev->NumAiChans;
}

extern "C" unsigned char LinxAiGetNumChans()
{
	return LinxDev->NumAiChans;
}

extern "C" unsigned char LinxAoGetNumChansRef(LinxDevice *dev)
{
	return dev->NumAoChans;
}

extern "C" unsigned char LinxAoGetNumChans()
{
	return LinxDev->NumAoChans;
}

extern "C" int LinxAiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAiGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxAoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxAoGetChansRef(LinxDev, numChans, channels)
}

extern "C" int LinxAnalogReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->AnalogRead(numChans, channels, values);
}

extern "C" int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogRead(numChans, channels, values);
}

extern "C" int LinxAnalogReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	return dev->AnalogReadNoPacking(numChans, channels, values);
}

extern "C" int LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	return LinxDev->AnalogReadNoPacking(numChans, channels, values);
}

//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChansRef(LinxDevice *dev, )
{
	return dev->NumCanChans;
}

extern "C" unsigned char LinxCanGetNumChans()
{
	return LinxDev->NumCanChans;
}

extern "C" int LinxCanGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
{
	if (numChans > dev->NumCanChans)
	{
		//Copy All Channels
		numChans = dev->NumCanChans;
	}
	memcpy(channels, dev->CanChans, numChans);
	return numChans;
}

extern "C" int LinxCanGetChans(unsigned char numChans, unsigned char* channels)
{
	LinxCanGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- Digital -------------------------------------
extern "C" unsigned char LinxDigitalGetNumChansRef(LinxDevice *dev, )
{
	return dev->NumDigitalChans;
}

extern "C" unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->NumDigitalChans;
}

extern "C" int LinxDigitalGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxDigitalGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxDigitalReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->DigitalRead(numChans, channels, values);
}

extern "C" int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}

extern "C" int LinxDigitalReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->DigitalReadNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalReadNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->DigitalWrite(numChans, channels, values);
}

extern "C" int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}

extern "C" int LinxDigitalWriteNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->DigitalWriteNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWriteNoPacking(numChans, channels, values);
}

//------------------------------------- I2C -------------------------------------
extern "C" unsigned char LinxI2cGetNumChansRef(LinxDevice *dev, )
{
	return dev->NumI2cChans;
}

extern "C" unsigned char LinxI2cGetNumChans()
{
	return LinxDev->NumI2cChans;
}

extern "C" int LinxI2cGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxI2cGetChansRef(LinxDev, numChans, channels)
}

extern "C" int LinxI2cOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	return dev->I2cOpenMaster(channel);
}

extern "C" int LinxI2cOpenMaster(unsigned char channel)
{
	return LinxDev->I2cOpenMaster(channel);
}

extern "C" int LinxI2cSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return dev->I2cSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->I2cSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxI2cWriteRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return dev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

extern "C" int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

extern "C" int LinxI2cReadRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return dev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

extern "C" int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxDev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

extern "C" int LinxI2cCloseRef(LinxDevice *dev, unsigned char channel)
{
	return dev->I2cClose(channel);
}

extern "C" int LinxI2cClose(unsigned char channel)
{
	return LinxDev->I2cClose(channel);
}

//------------------------------------- PWM -------------------------------------
extern "C" unsigned char LinxPwmGetNumChansRef(LinxDevice *dev, )
{
	return dev->NumPwmChans;
}

extern "C" unsigned char LinxPwmGetNumChans()
{
	return LinxDev->NumPwmChans;
}

extern "C" int LinxPwmGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxPwmGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxPwmSetDutyCycleRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return dev->PwmSetDutyCycle(numChans, channels, values);
}

extern "C" int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}

//------------------------------------- QE -------------------------------------
extern "C" unsigned char LinxQeGetNumChansRef(LinxDevice *dev)
{
	return dev->NumQeChans;
}

extern "C" unsigned char LinxQeGetNumChans()
{
	return LinxDev->NumQeChans;
}

extern "C" int LinxQeGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxQeGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChansRef(LinxDevice *dev)
{
	return dev->NumServoChans;
}

extern "C" unsigned char LinxServoGetNumChans()
{
	return LinxDev->NumServoChans;
}

extern "C" int LinxServoGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxServoGetChansRef(LinxDev, numChans, channels);
}

//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChansRef(LinxDevice *dev)
{
	return dev->NumSpiChans;
}

extern "C" unsigned char LinxSpiGetNumChans()
{
	return LinxDev->NumSpiChans;
}

extern "C" int LinxSpiGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxSpiGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxSpiOpenMasterRef(LinxDevice *dev, unsigned char channel)
{
	return dev->SpiOpenMaster(channel);
}

extern "C" int LinxSpiOpenMaster(unsigned char channel)
{
	return LinxDev->SpiOpenMaster(channel);
}

extern "C" int LinxSpiSetBitOrderRef(LinxDevice *dev, unsigned char channel, unsigned char bitOrder)
{
	return dev->SpiSetBitOrder(channel, bitOrder);
}

extern "C" int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxDev->SpiSetBitOrder(channel, bitOrder);
}

extern "C" int LinxSpiSetModeRef(LinxDevice *dev, unsigned char channel, unsigned char mode)
{
	return dev->SpiSetMode(channel, mode);
}

extern "C" int LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxDev->SpiSetMode(channel, mode);
}

extern "C" int LinxSpiSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return dev->SpiSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->SpiSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxSpiWriteReadRef(LinxDevice *dev, unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return dev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

extern "C" int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxDev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

extern "C" int LinxSpiCloseRef(LinxDevice *dev, unsigned char channel)
{
	return dev->SpiClose(channel);
}

extern "C" int LinxSpiClose(unsigned char channel)
{
	return LinxDev->SpiClose(channel);
}

//------------------------------------- UART -------------------------------------
extern "C" unsigned char LinxUartGetNumChansRef(LinxDevice *dev)
{
	return dev->NumUartChans;
}

extern "C" unsigned char LinxUartGetNumChans()
{
	return LinxDev->NumUartChans;
}

extern "C" int LinxUartGetChansRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels)
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

extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	return LinxUartGetChansRef(LinxDev, numChans, channels);
}

extern "C" int LinxUartOpenRef(LinxDevice *dev, unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return dev->UartOpen(channel, baudRate, actualBaud);
}

extern "C" int LinxUartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartOpen(channel, baudRate, actualBaud);
}

extern "C" int LinxUartSetBaudRateRef(LinxDevice *dev, unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return dev->UartSetBaudRate(channel, baudRate, actualBaud);
}

extern "C" int LinxUartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
}

extern "C" int LinxUartGetBytesAvailableRef(LinxDevice *dev, unsigned char channel, unsigned char *numBytes)
{
	return dev->UartGetBytesAvailable(channel, numBytes);
}

extern "C" int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxDev->UartGetBytesAvailable(channel, numBytes);
}

extern "C" int LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return dev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

extern "C" int LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return dev->UartWrite(channel, numBytes, sendBuffer);
}

extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer);
}

extern "C" int LinxUartCloseRef(LinxDevice *dev, unsigned char channel)
{
	return dev->UartClose(channel);
}

extern "C" int LinxUartClose(unsigned char channel)
{
	return LinxDev->UartClose(channel);
}
