/****************************************************************************************
**  LINX Raspberry Pi 2 Model B Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <termios.h>
#else
#include <io.h>
enum {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};
#endif
#include "LinxDevice.h"
#include "LinxLinuxDevice.h"
#include "LinxRaspberryPi.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System

//AI
//None 

//AO
//None

//DIGITAL
const unsigned char LinxRaspberryPi::m_DigitalChans[NUM_DIGITAL_CHANS] = {7, 11, 12, 13, 15, 16, 18, 22, 29, 31, 32, 33, 35, 36, 37, 38, 40};
const unsigned char LinxRaspberryPi::m_gpioChan[NUM_DIGITAL_CHANS] =     {4, 17, 18, 27, 22, 23, 24, 25,  5,  6, 12, 13, 19, 16, 26, 20, 21};

//PWM
//None

//QE
//None

//SPI
unsigned char LinxRaspberryPi::m_SpiChans[NUM_SPI_CHANS] = {0, 1};
const char * LinxRaspberryPi::m_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev0.0", "/dev/spidev0.1"};
unsigned int LinxRaspberryPi::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxRaspberryPi::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxRaspberryPi::m_I2cChans[NUM_I2C_CHANS] = {1};
const char * LinxRaspberryPi::m_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1"};

//UART
unsigned char LinxRaspberryPi::m_UartChans[NUM_UART_CHANS] = {0};
const char * LinxRaspberryPi::m_UartPaths[NUM_UART_CHANS] = {"/dev/ttyAMA0"};
unsigned int LinxRaspberryPi::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
unsigned int LinxRaspberryPi::m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

//SERVO
//None

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxRaspberryPi::LinxRaspberryPi()
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;

	//-------------------------------- Device Detection ------------------------------
	DeviceFamily = 0x04;	// Raspberry Pi Family Code
	DeviceId = 0xFF;		// Raspberry Pi 2 Model B

	int fd = open("/proc/device-tree/model", O_RDONLY);
	if (fd >= 0)
	{
		struct stat buf;
		if (fstat(fd, &buf) == 0)
		{
			int len = read(fd, m_DeviceName, min(buf.st_size, (off_t)(DEVICE_NAME_MAX - 1)));
			if (len > 0)
			{
				m_DeviceName[len] = 0;
				if (!strcmp(m_DeviceName, "Raspberry Pi ") && len >= 14)
				{
					switch (m_DeviceName[13])
					{
						case 'A':
							// Raspberry Pi A
							DeviceId = 0;
							break;
						case 'B':
							if (len >= 14 && *(m_DeviceName + 14) == '+')
							{
								// Raspberry Pi B+
								DeviceId = 2;
							}
							else
							{
								// Raspberry Pi B
								DeviceId = 1;
							}
							break;
						case '2':
							// Raspberry Pi 2 Model B
							DeviceId = 3;
							break;
						case '3':
							if (len >= 23 && *(m_DeviceName + 23) == '+')
							{
								// Raspberry Pi 3 Model B+ (with Wifi)
								DeviceId = 5;
							}
							else
							{
								// Raspberry Pi 3 Model B
								DeviceId = 4;
							}
							break;
						case '4':	// Raspberry Pi 4 Model B (with Wifi)
							DeviceId = 6;
							break;
						default:
							len = 0;
							break;
					}
				}
				else
					len = 0;
			}
		}
		close(fd);
	}
	if (DeviceId == 0xFF)
	{
		DeviceId = 0x03;	// Raspberry Pi 2 Model B
		strcpy(m_DeviceName, "Raspberry Pi");
	}

	//DIGITAL

	//AI
	AiResolution = 0;
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	AiRefCodes = NULL;
	
	AiRefIntVals = NULL;
	
	AiRefExtMin = 0;
	AiRefExtMax = 0;

	//AO
	AoResolution = 0;
	AoRefDefault = 0;
	AoRefSet = 0;

	//PWM

	//QE

	//------------------------------------- UART -------------------------------------
	UartMaxBaud = m_UartSupportedSpeeds[NUM_UART_SPEEDS - 1];
	UartSupportedSpeeds = m_UartSupportedSpeeds;
	UartSupportedSpeedsCodes = m_UartSupportedSpeedsCodes;

	for (int i = 0; i < NUM_UART_CHANS; i++)
	{
		UartPaths[m_UartChans[i]] = m_UartPaths[i];
		UartHandles[m_UartChans[i]] = 0;
	}

	//------------------------------------- I2C -------------------------------------
	//Store I2C Master Paths In Map
	for (int i = 0; i < NUM_I2C_CHANS; i++)
	{
		I2cPaths[m_I2cChans[i]] = m_I2cPaths[i];
	}

	//------------------------------------- SPI -------------------------------------
	SpiDefaultSpeed = 3900000;
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		if (fileExists(m_SpiPaths[i]))
		{
			SpiPaths[m_SpiChans[i]] = m_SpiPaths[i];
			SpiHandles[m_SpiChans[i]] = -1;
			SpiBitOrders[m_SpiChans[i]] = MSBFIRST;		//MSB First
			SpiSetSpeeds[m_SpiChans[i]] = SpiDefaultSpeed;
		}
	}
	SpiSupportedSpeeds = m_SpiSupportedSpeeds;
	SpiSpeedCodes = m_SpiSpeedCodes;

	//CAN

	//Servo 

	//------------------------------------- Digital -------------------------------------
	//Export GPIO - Set All Digital Handles To NULL
	for (int i = 0; i < NUM_DIGITAL_CHANS; i++)
	{
		FILE* digitalExportHandle = fopen("/sys/class/gpio/export", "w");
		fprintf(digitalExportHandle, "%d", m_gpioChan[i]);
		fclose(digitalExportHandle);
		
		DigitalDirHandles[m_DigitalChans[i]] = NULL;
		DigitalValueHandles[m_DigitalChans[i]] = NULL;
		DigitalChannels[m_DigitalChans[i]] = m_gpioChan[i];
		DigitalDirs[m_DigitalChans[i]] = INPUT;
	}


	//If Debuging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED > -1
		EnableDebug(DEBUG_ENABLED);
	#endif

	// TODO Load User Config Data From Non Volatile Storage
	//userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
}

//Destructor
LinxRaspberryPi::~LinxRaspberryPi()
{
	//Close GPIO Handles If They Are Open
	map<unsigned char, FILE*> mf = DigitalDirHandles;
	for (map<unsigned char, FILE*>::iterator it = mf.begin(); it != mf.end(); ++it)
	{
		if (it->second != NULL)
			fclose(it->second);
	}

	mf = DigitalValueHandles;
	for (map<unsigned char, FILE*>::iterator it = mf.begin(); it != mf.end(); ++it)
	{
		if (it->second != NULL)
			fclose(it->second);
	}


	//Close I2C Handles
	map<unsigned char, int> mi = I2cHandles;
	for (map<unsigned char, int>::iterator it = mi.begin(); it != mi.end(); ++it)
	{
		if (it->second > 0)
			close(it->second);
	}

	//Close SPI Handles
	mi = SpiHandles;
	for (map<unsigned char, int>::iterator it = mi.begin(); it != mi.end(); ++it)
	{
		if (it->second > 0)
			close(it->second);
	}

	//Close UART Handles
	mi = UartHandles;
	for (map<unsigned char, int>::iterator it = mi.begin(); it != mi.end(); ++it)
	{
		if (it->second > 0)
			close(it->second);
	}
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxRaspberryPi::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	unsigned char slen = (unsigned char)strlen(m_DeviceName);
	if (slen && buffer)
	{
		slen = min(length, slen);
		memcpy(buffer, m_DeviceName, slen);
		if (length > slen)
			buffer[slen] = 0;
	}
	return slen;
}
