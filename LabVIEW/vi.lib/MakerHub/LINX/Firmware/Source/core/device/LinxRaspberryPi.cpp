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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "utility/LinxDevice.h"
#include "utility/LinxLinuxDevice.h"
#include "LinxRaspberryPi.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
unsigned char LinxRaspberryPi::m_DeviceName[] = "Raspberry Pi Unknown";

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
unsigned long LinxRaspberryPi::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxRaspberryPi::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxRaspberryPi::m_I2cChans[NUM_I2C_CHANS] = {1};
string LinxRaspberryPi::m_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1"};

//UART
unsigned char LinxRaspberryPi::m_UartChans[NUM_UART_CHANS] = {0};
string LinxRaspberryPi::m_UartPaths[NUM_UART_CHANS] = {"/dev/ttyAMA0"};
unsigned long LinxRaspberryPi::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
unsigned long LinxRaspberryPi::m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

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
	DeviceFamily = 0x04;	//Raspberry Pi Family Code
	DeviceName = NULL;

	int fd = open("/proc/device-tree/model", O_RDONLY);
	if (fd >= 0)
	{
		struct stat buf;
		if (fstat(fd, &buf) == 0)
		{
			DeviceName = (unsigned char*)malloc(buf.st_size + 1);
			if (DeviceName)
			{
				DeviceNameLen = read(fd, DeviceName, buf.st_size);
				if (DeviceNameLen > 0)
				{
					DeviceName[DeviceNameLen] = 0;
					if (!strcmp((char*)DeviceName, "Raspberry Pi ") && DeviceNameLen >= 14)
					{
						unsigned char *ptr = DeviceName + 13;
						switch (*ptr)
						{
							case 'A':
								// Raspberry Pi A
								DeviceId = 0;
								break;
							case 'B':
								if (DeviceNameLen >= 14 && *(DeviceName + 14) == '+')
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
								if (DeviceNameLen >= 23 && *(DeviceName + 23) == '+')
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
								DeviceNameLen = 0;
								break;
						}
					}
					else
						DeviceNameLen = 0;
				}
				if (DeviceNameLen <= 0)
					free(DeviceName);
			}
		}
		close(fd);
	}
	if (!DeviceNameLen)
	{
		// Fall back on default setting
		DeviceName = m_DeviceName;
		DeviceNameLen = strlen((char*)DeviceName);
		DeviceId = 0x03;		// Raspberry Pi 2 Model B
	}

	//DIGITAL
	NumDigitalChans = NUM_DIGITAL_CHANS;
	DigitalChans = m_DigitalChans;

	//AI
	NumAiChans = NUM_AI_CHANS;
	AiChans = 0;
	AiResolution = 0;
	AiRefSet = 0;
		
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	AiRefCodes = NULL;
	
	NumAiRefIntVals = NUM_AI_INT_REFS;
	AiRefIntVals = NULL;
	
	AiRefExtMin = 0;
	AiRefExtMax = 0;

	//AO
	NumAoChans = NUM_AO_CHANS;
	AoChans = 0;
	AoResolution = 0;
	AoRefDefault = 0;
	AoRefSet = 0;

	//PWM
	NumPwmChans = NUM_PWM_CHANS;
	PwmChans = 0;

	//QE
	NumQeChans = 0;
	QeChans = 0;

	//------------------------------------- UART -------------------------------------
	NumUartChans = NUM_UART_CHANS;
	UartChans = m_UartChans;
	UartMaxBaud = m_UartSupportedSpeeds[NUM_UART_SPEEDS - 1];
	NumUartSpeeds = NUM_UART_SPEEDS;
	UartSupportedSpeeds = m_UartSupportedSpeeds;
	UartSupportedSpeedsCodes = m_UartSupportedSpeedsCodes;

	for (int i = 0; i < NumUartChans; i++)
	{
		UartPaths[m_UartChans[i]] = m_UartPaths[i];
		UartHandles[m_UartChans[i]] = 0;
	}

	//------------------------------------- I2C -------------------------------------
	NumI2cChans = NUM_I2C_CHANS;
	I2cChans = m_I2cChans;
	//Store I2C Master Paths In Map
	for (int i = 0; i < NumI2cChans; i++)
	{
		I2cPaths[I2cChans[i]] = m_I2cPaths[i];
	}

	//------------------------------------- SPI -------------------------------------
	SpiDefaultSpeed = 3900000;
	NumSpiChans = 0;
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		if (fileExists(m_SpiPaths[i]))
		{
			m_SpiChanBuf[NumSpiChans] = m_SpiChans[i];
			SpiPaths[m_SpiChans[i]] = m_SpiPaths[i];
			SpiHandles[m_SpiChans[i]] = -1;
			SpiBitOrders[m_SpiChans[i] = MSBFIRST;		//MSB First
			SpiSetSpeeds[m_SpiChans[i] = SpiDefaultSpeed;
			NumSpiChans++;
		}
	}
	SpiChans = m_SpiChanBuf;
	NumSpiSpeeds = NUM_SPI_SPEEDS;
	SpiSupportedSpeeds = m_SpiSupportedSpeeds;
	SpiSpeedCodes = m_SpiSpeedCodes;

	//CAN
	NumCanChans = NUM_CAN_CHANS;
	CanChans = 0;

	//Servo 
	NumServoChans = NUM_SERVO_CHANS;
	ServoChans = 0;

	//------------------------------------- Digital -------------------------------------
	//Export GPIO - Set All Digital Handles To NULL
	for (int i = 0; i < NumDigitalChans; i++)
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
	if (DeviceName != m_DeviceName)
		free(DeviceName);

	//Close GPIO Handles If They Are Open
	for (int i = 0; i < NumDigitalChans; i++)
	{
		if (DigitalDirHandles[m_DigitalChans[i]] != NULL)
		{
			fclose(DigitalDirHandles[m_DigitalChans[i]]);
		}
		if (DigitalValueHandles[m_DigitalChans[i]] != NULL)
		{
			fclose(DigitalValueHandles[m_DigitalChans[i]]);
		}
	}

	//Close I2C Handles
	for (int i = 0; i < NumI2cChans; i++)
	{
		if (I2cHandles[m_I2cChans[i]] != 0)
		{
			close(I2cHandles[m_I2cChans[i]]);
		}
	}

	//Close SPI Handles
	for (int i = 0; i < NumSpiChans; i++)
	{
		if (SpiHandles[m_SpiChans[i]] != 0)
		{
			close(SpiHandles[m_SpiChans[i]]);
		}
	}

	//Close UART Handles
	for (int i = 0; i < NumUartChans; i++)
	{
		if (UartHandles[m_UartChans[i]] != 0)
		{
			close(UartHandles[m_UartChans[i]]);
		}
	}
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/

