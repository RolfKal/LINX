/****************************************************************************************
**  LINX - BeagleBone code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

/****************************************************************************************
**  Defines
****************************************************************************************/

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDevice.h"
#include "LinxLinuxDevice.h"

#include <map>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

using namespace std;


/****************************************************************************************
**  Constructors / Destructors
****************************************************************************************/
LinxLinuxDevice::LinxLinuxDevice()
{

}

LinxLinuxDevice::~LinxLinuxDevice()
{

}
/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxLinuxDevice::DigitalWrite(unsigned char channel, unsigned char value)
{
	return DigitalWrite(1, &channel, &value);
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/

//------------------------------------- Analog -------------------------------------
int LinxLinuxDevice::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::AnalogSetRef(unsigned char mode, unsigned long voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//------------------------------------- Digital -------------------------------------
int LinxLinuxDevice::DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if (digitalSmartOpen(numChans, channels) != L_OK)
	{
		DebugPrintln("Smart Open Failed");
		return L_UNKNOWN_ERROR;
	}

	//Set Directions
	for (int i = 0; i < numChans; i++)
	{
		if (((values[i / 8] >> (i % 8)) & 0x01) == OUTPUT && DigitalDirs[channels[i]] != OUTPUT)
		{
			//Set As Output
			fprintf(DigitalDirHandles[channels[i]], "out");
			fflush(DigitalDirHandles[channels[i]]);
			DigitalDirs[channels[i]] = OUTPUT;
		}
		else if (((values[i / 8] >> (i % 8)) & 0x01) == INPUT && DigitalDirs[channels[i]] != INPUT)
		{
			//Set As Input
			fprintf(DigitalDirHandles[channels[i]], "in");
			fflush(DigitalDirHandles[channels[i]]);
			DigitalDirs[channels[i]] = INPUT;
		}
	}
	return L_OK;
}

int LinxLinuxDevice::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Output Direction Array
	int numDirBytes = ((numChans + 7) / 8);
	unsigned char directions[numDirBytes];
	for (int i = 0; i < numDirBytes; i++)
	{
		directions[i] = 0xFF;
	}

	if (DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}

	for (int i = 0; i < numChans; i++)
	{
		//Set Value
		if (((values[i / 8] >> i % 8) & 0x01) == LOW)
		{
			fprintf(DigitalValueHandles[channels[i]], "0");
			fflush(DigitalValueHandles[channels[i]]);
		}
		else
		{
			fprintf(DigitalValueHandles[channels[i]], "1");
			fflush(DigitalValueHandles[channels[i]]);
		}
	}
	return L_OK;
}

int LinxLinuxDevice::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Output Direction Array
	int numDirBytes = ((numChans + 7) / 8);
	unsigned char directions[numDirBytes];
	for (int i = 0; i < numDirBytes; i++)
	{
		directions[i] = 0xFF;
	}

	if (DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}

	for (int i = 0; i < numChans; i++)
	{
		//Set Value
		if (values[i] == LOW)
		{
			fprintf(DigitalValueHandles[channels[i]], "0");
			fflush(DigitalValueHandles[channels[i]]);
		}
		else
		{
			fprintf(DigitalValueHandles[channels[i]], "1");
			fflush(DigitalValueHandles[channels[i]]);
		}
	}
	return L_OK;
}

int LinxLinuxDevice::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Input Direction Array
	int numDirBytes = ((numChans + 7) / 8);
	unsigned char directions[numDirBytes];
	for (int i = 0; i < numDirBytes; i++)
	{
		directions[i] = 0x00;
	}

	//Set Directions To Inputs
	if (DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}

	unsigned char bitOffset = 8;
	unsigned char byteOffset = 0;
	unsigned char retVal = 0;
	int diVal = 0;

	//Loop Over channels To Read
	for (int i = 0; i < numChans; i++)
	{

		//If bitOffset Is 0 We Have To Start A New Byte, Store Old Byte And Increment OFfsets
		if (bitOffset == 0)
		{
			//Insert retVal Into Response Buffer
			values[byteOffset] = retVal;
			retVal = 0x00;
			byteOffset++;
			bitOffset = 7;
		}
		else
		{
			bitOffset--;
		}

		//Reopen Value Handle
		char valPath[64];
		sprintf(valPath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
		DigitalValueHandles[channels[i]] = freopen(valPath, "r+w+", DigitalValueHandles[channels[i]]);

		//Read From Next Pin
		fscanf(DigitalValueHandles[channels[i]], "%u", &diVal);

		retVal = retVal | ((unsigned char)diVal << bitOffset);	//Read Pin And Insert Value Into retVal
	}

	//Store Last Byte
	values[byteOffset] = retVal;

	return L_OK;
}

int LinxLinuxDevice::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Input Direction Array
	int numDirBytes = ((numChans + 7) / 8);
	unsigned char directions[numDirBytes];
	for (int i = 0; i < numDirBytes; i++)
	{
		directions[i] = 0x00;
	}

	//Set Directions To Inputs
	if (DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}

	//Loop Over channels To Read
	for (int i = 0; i < numChans; i++)
	{
		//Reopen Value Handle
		char valPath[64];
		sprintf(valPath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
		DigitalValueHandles[channels[i]] = freopen(valPath, "r+w+", DigitalValueHandles[channels[i]]);

		//Read From Next Pin
		fscanf(DigitalValueHandles[channels[i]], "%hhu", values+i);
	}
	return L_OK;
}

int LinxLinuxDevice::DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//------------------------------------- PWM -------------------------------------
int LinxLinuxDevice::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//--------------------------------------------------------SPI-------------------------------------------------------
int LinxLinuxDevice::SpiOpenMaster(unsigned char channel)
{
	// Only open it if it is not already open
	int fd = SpiHandles[channel];
	if (fd < 0)
	{
		fd = open(SpiPaths[channel].c_str(), O_RDWR);
		if (fd < 0)
		{
			return LSPI_OPEN_FAIL;
		}
		else
		{
			//Default To Mode 0 With No CS (LINX Uses GPIO When Performing Write)
			unsigned char spi_mode = SPI_MODE_0 | SPI_NO_CS;
			if (ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) < 0)
			{
				DebugPrintln("Failed To Set SPI Mode");
				DebugPrintln(spi_mode, BIN);
				close(fd);
				return LSPI_OPEN_FAIL;
			}

			//Default Max Speed To
			if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &SpiDefaultSpeed) < 0)
			{
				DebugPrint("SPI Fail - Failed To Set SPI Max Speed - ");
				DebugPrintln(SpiDefaultSpeed, DEC);
				close(fd);
				return LSPI_OPEN_FAIL;
			}
		}
		SpiHandles[channel] = fd;
	}
	return L_OK;
}

int LinxLinuxDevice::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	SpiBitOrders[channel] = bitOrder;
	return L_OK;
}

int LinxLinuxDevice::SpiSetMode(unsigned char channel, unsigned char mode)
{
	int fd = SpiHandles[channel];
	if (fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	unsigned int spi_mode;
	if (ioctl(fd, SPI_IOC_RD_MODE, &spi_mode) < 0)
	{
		DebugPrintln("Failed To Set SPI Mode");
		return  L_UNKNOWN_ERROR;
	}

	if (mode != spi_mode)
	{
		if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
		{
			DebugPrintln("Failed To Set SPI Mode");
			DebugPrintln(mode, BIN);
			return  L_UNKNOWN_ERROR;
		}
	}
	return L_OK;
}

int LinxLinuxDevice::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	if (NumSpiSpeeds)
	{
		int index;
		//Loop Over All Supported SPI Speeds
		for (index = 0; index < NumSpiSpeeds; index++)
		{
			if (speed < *(SpiSupportedSpeeds + index))
			{
				//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
				break;
			}
		}
		if (index != 0)
			index--;
		*actualSpeed = *(SpiSupportedSpeeds + index);
	}
	else
	{
		*actualSpeed = speed;
	}
	SpiSetSpeeds[channel] = *actualSpeed;
	return L_OK;
}

int LinxLinuxDevice::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	int fd = SpiHandles[channel];
	if (fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	unsigned char nextByte = 0;	//First Byte Of Next SPI Frame

	//SPI Hardware Only Supports MSb First Transfer. If  Configured for LSb First Reverse Bits In Software
	if (SpiBitOrders[channel] == LSBFIRST)
	{
		for (int i = 0; i < frameSize * numFrames; i++)
		{
			sendBuffer[i] = ReverseBits(sendBuffer[i]);
		}
	}

	struct spi_ioc_transfer transfer = {};

	//Set CS As Output And Make Sure CS Starts Idle
	if (csChan)
		DigitalWrite(csChan, (~csLL & 0x01) );

	for (int i = 0; i < numFrames; i++)
	{
		//Setup Transfer
		transfer.tx_buf = (unsigned long)(sendBuffer+nextByte);
		transfer.rx_buf = (unsigned long)(recBuffer+nextByte);
		transfer.len = frameSize;
		transfer.delay_usecs = 0;
		transfer.speed_hz = SpiSetSpeeds[channel];
		transfer.bits_per_word = 8;

		//CS Active
		if (csChan)
			DigitalWrite(csChan, csLL);

		//Transfer Data
		int retVal = ioctl(SpiHandles[channel], SPI_IOC_MESSAGE(1), &transfer);

		//CS Idle
		if (csChan)
			DigitalWrite(csChan, (~csLL & 0x01) );

		if (retVal < 0)
		{
			DebugPrintln("SPI Fail - Failed To Transfer Data");
			return  LSPI_TRANSFER_FAIL;
		}
		nextByte += frameSize;
	}
	return L_OK;
}

int LinxLinuxDevice::SpiCloseMaster(unsigned char channel)
{
	int fd = SpiHandles[channel];
	SpiHandles[channel] = -1;
	// Close SPI handle
	if ((fd >= 0) && (close(fd) < 0))
		return LSPI_CLOSE_FAIL;

	return L_OK;
}


//------------------------------------- I2C -------------------------------------
int LinxLinuxDevice::I2cOpenMaster(unsigned char channel)
{
	int fd = I2cHandles[channel];
	if (fd < 0)
	{
		fd = open(I2cPaths[channel].c_str(), O_RDWR);
		if (fd < 0)
		{
			DebugPrintln("I2C Fail - Failed To Open I2C Channel");
			return  LI2C_OPEN_FAIL;
		}
		else
		{
			I2cHandles[channel] = handle;
		}
	}
	return L_OK;
}

int LinxLinuxDevice::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	int fd = I2cHandles[channel];
	if (fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	//Check EOF - Currently Only Support 0x00
	if (eofConfig != EOF_STOP)
	{
		DebugPrintln("I2C Fail - EOF Not Supported");
		return LI2C_EOF;
	}

	//Set Slave Address
	if (ioctl(fd, I2C_SLAVE, slaveAddress) < 0)
	{
		//Failed To Set Slave Address
		DebugPrintln("I2C Fail - Failed To Set Slave Address");
		return LI2C_SADDR;
	}

	//Write Data
	if (write(fd, sendBuffer, numBytes) != numBytes)
	{
		DebugPrintln("I2C Fail - Failed To Write All Data");
		return LI2C_WRITE_FAIL;
	}
	return L_OK;
}

int LinxLinuxDevice::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	int fd = I2cHandles[channel];
	if (fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	//Check EOF - Currently Only Support 0x00
	if (eofConfig != EOF_STOP)
	{
		return LI2C_EOF;
	}

	//Set Slave Address
	if (ioctl(fd, I2C_SLAVE, slaveAddress) < 0)
	{
		//Failed To Set Slave Address
		return LI2C_SADDR;
	}

	if (read(fd, recBuffer, numBytes) < numBytes)
	{
		return LI2C_READ_FAIL;
	}
	return L_OK;
}

int LinxLinuxDevice::I2cClose(unsigned char channel)
{
	int fd = I2cHandles[channel];
	I2cHandles[channel] = -1;
	//Close I2C Channel
	if ((fd >= 0) && (close(fd) < 0))
		return LI2C_CLOSE_FAIL;

	return L_OK;
}


//------------------------------------- UART -------------------------------------
int LinxLinuxDevice::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	//Open UART	Handle If Not Already Open
	if (UartHandles[channel] < 0)
	{
		int handle = open(UartPaths[channel].c_str(),  O_RDWR);

		if (handle < 0)
		{
			DebugPrint("UART Fail - Failed To Open UART Handle -  ");
			DebugPrintln(UartPaths[channel].c_str());
			return  LUART_OPEN_FAIL;
		}
		else
		{
			UartHandles[channel] = handle;
		}
	}

	if (UartSetBaudRate(channel, baudRate, actualBaud) != L_OK)
	{
		DebugPrintln("Failed to set baud rate");
	}
	return L_OK;
}

int LinxLinuxDevice::UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	//Get Closest Support Baud Rate Without Going Over
	//Loop Over All Supported Uart Speeds
	int index;
	for (index = 0; index < NumUartSpeeds; index++)
	{
		if (baudRate < *(UartSupportedSpeeds + index))
		{
			//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
			break;
		}
	}

	//Once Loop Completes Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
	//If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud
	if (index != 0)
		index--;

	//Store Actual Baud Used
	*actualBaud = (unsigned long) *(UartSupportedSpeeds + index);

	//Set Baud Rate
	struct termios options;
	tcgetattr(UartHandles[channel], &options);

	options.c_cflag = *(UartSupportedSpeedsCodes + index) | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

	tcflush(UartHandles[channel], TCIFLUSH);
	tcsetattr(UartHandles[channel], TCSANOW, &options);

	return  L_OK;
}

int LinxLinuxDevice::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	int bytesAtPort = -1;
	ioctl(UartHandles[channel], FIONREAD, &bytesAtPort);

	if (bytesAtPort < 0)
	{
		return LUART_AVAILABLE_FAIL;
	}
	else
	{
		*numBytes = (unsigned char) bytesAtPort;
	}
	return L_OK;
}

int LinxLinuxDevice::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	//Check If Enough Bytes Are Available
	unsigned char bytesAvailable = -1;
	UartGetBytesAvailable(channel, &bytesAvailable);

	if (bytesAvailable >= numBytes)
	{
		//Read Bytes From Input Buffer
		int bytesRead = read(UartHandles[channel], recBuffer, numBytes);
		*numBytesRead = (unsigned char) bytesRead;

		if (bytesRead != numBytes)
		{
			return LUART_READ_FAIL;
		}
	}
	return  L_OK;
}

int LinxLinuxDevice::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	int bytesSent = write(UartHandles[channel], sendBuffer, numBytes);
	if (bytesSent != numBytes)
	{
		return LUART_WRITE_FAIL;
	}
	return  L_OK;
}

int LinxLinuxDevice::UartClose(unsigned char channel)
{
	//Close UART Channel, Return OK or Error
	if (close(UartHandles[channel]) < 0)
	{
		return LUART_CLOSE_FAIL;
	}
	UartHandles[channel] = 0;

	return  L_OK;
}


//------------------------------------- Servo -------------------------------------
int LinxLinuxDevice::ServoOpen(unsigned char numChans, unsigned char* channels)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::ServoClose(unsigned char numChans, unsigned char* channels)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//------------------------------------- General -------------------------------------
unsigned long LinxLinuxDevice::GetMilliSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	//return (mTime.tv_nsec / 1000000);
	return ( ((unsigned long) mTime.tv_sec * 1000) + mTime.tv_nsec / 1000000);
}

unsigned long LinxLinuxDevice::GetSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	return mTime.tv_sec;
}

void LinxLinuxDevice::DelayMs(unsigned long ms)
{
	usleep(ms * 1000);
}

void LinxLinuxDevice::NonVolatileWrite(int address, unsigned char data)
{

}

unsigned char LinxLinuxDevice::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/

//Open Direction And Value Handles If They Are Not Already Open And Set Direction
int LinxLinuxDevice::digitalSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for (int i = 0; i < numChans; i++)
	{
		//Open Direction Handle If It Is Not Already
		if (DigitalDirHandles[channels[i]] == NULL)
		{
			DebugPrint("Opening Digital Direction Handle For LINX DIO ");
			DebugPrint(channels[i], DEC);
			DebugPrint("(GPIO ");
			DebugPrint(DigitalChannels[channels[i]], DEC);
			DebugPrintln(")");

			char dirPath[64];
			sprintf(dirPath, "/sys/class/gpio/gpio%d/direction", DigitalChannels[channels[i]]);
			DigitalDirHandles[channels[i]] = fopen(dirPath, "r+w+");

			if (DigitalDirHandles[channels[i]] == NULL)
			{
				DebugPrintln("Digital Fail - Unable To Open Direction File Handles");
				return L_UNKNOWN_ERROR;
			}
		}

		//Open Value Handle If It Is Not Already
		if (DigitalValueHandles[channels[i]] == NULL)
		{
			DebugPrintln("Opening Digital Value Handle");
			char valuePath[64];
			sprintf(valuePath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
			DigitalValueHandles[channels[i]] = fopen(valuePath, "r+w+");

			if (DigitalValueHandles[channels[i]] == NULL)
			{
				DebugPrintln("Digital Fail - Unable To Open Value File Handles");
				return L_UNKNOWN_ERROR;
			}
		}
	}
	return L_OK;
}

//Open Direction And Value Handles If They Are Not Already Open And Set Direction
//Open Direction And Value Handles If They Are Not Already Open And Set Direction
int LinxLinuxDevice::pwmSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for (int i = 0; i < numChans; i++)
	{
		//Open Period Handle If It Is Not Already
		if (PwmPeriodHandles[channels[i]] == NULL)
		{
			char periodPath[64];
			sprintf(periodPath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmPeriodFileName.c_str());
			DebugPrint("Opening ");
			DebugPrintln(periodPath);
			PwmPeriodHandles[channels[i]] = fopen(periodPath, "r+w+");

			//Initialize PWM Period
			fprintf(PwmPeriodHandles[channels[i]], "%lu", PwmDefaultPeriod);
			PwmPeriods[channels[i]] = PwmDefaultPeriod;
			fflush(PwmPeriodHandles[channels[i]]);
		}

		//Open Duty Cycle Handle If It Is Not Already
		if (PwmDutyCycleHandles[channels[i]] == NULL)
		{
			char dutyCyclePath[64];
			sprintf(dutyCyclePath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmDutyCycleFileName.c_str());
			DebugPrint("Opening ");
			DebugPrintln(dutyCyclePath);
			PwmDutyCycleHandles[channels[i]] = fopen(dutyCyclePath, "r+w+");
		}
	}
	return L_OK;
}

//Return True If File Specified By path Exists.
bool LinxLinuxDevice::fileExists(const char* path)
{
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

bool LinxLinuxDevice::fileExists(const char* path, int *length)
{
	struct stat buffer;
	int ret = stat(path, &buffer);
	if (ret == 0)
		*length = buffer.st_size;
	return (ret == 0);
}

bool LinxLinuxDevice::fileExists(const char* directory, const char* fileName)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);
	struct stat buffer;
	return (stat(fullPath, &buffer) == 0);
}

bool LinxLinuxDevice::fileExists(const char* directory, const char* fileName, unsigned long timeout)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);
	struct stat buffer;

	unsigned long startTime = GetMilliSeconds();
	while (GetMilliSeconds()-startTime < timeout)
	{
		if (stat(fullPath, &buffer) == 0)
		{
			DebugPrint("DTO Took ");
			DebugPrintln(GetMilliSeconds() - startTime < timeout, DEC);
			return true;
		}
	}
	DebugPrintln("Timeout");
	return false;
}
