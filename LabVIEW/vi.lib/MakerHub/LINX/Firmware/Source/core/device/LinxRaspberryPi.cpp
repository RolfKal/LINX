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
#include "LinxDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef Posix
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#elif Win32
#include <io.h>
#define MAP_FAILED (volatile int*)-1
#define PROT_READ 0
#define PROT_WRITE 0
#define MAP_SHARED 0
#define O_SYNC 0x10
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void munmap(void *addr, size_t length);
enum {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};
#endif
#include "LinxUtilities.h"
#include "LinxDevice.h"
#include "LinxLinuxDevice.h"
#include "LinxRaspberryPi.h"

/****************************************************************************************
**  Defines
****************************************************************************************/
#define FSEL_OFFSET                 0   // 0x0000
#define SET_OFFSET                  7   // 0x001c / 4
#define CLR_OFFSET                  10  // 0x0028 / 4
#define PINLEVEL_OFFSET             13  // 0x0034 / 4
#define EVENT_DETECT_OFFSET         16  // 0x0040 / 4
#define RISING_ED_OFFSET            19  // 0x004c / 4
#define FALLING_ED_OFFSET           22  // 0x0058 / 4
#define HIGH_DETECT_OFFSET          25  // 0x0064 / 4
#define LOW_DETECT_OFFSET           28  // 0x0070 / 4
#define PULLUPDN_OFFSET             37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET          38  // 0x0098 / 4

#define PULLUPDN_OFFSET_2711_0      57
#define PULLUPDN_OFFSET_2711_1      58
#define PULLUPDN_OFFSET_2711_2      59
#define PULLUPDN_OFFSET_2711_3      60

#define BLOCK_SIZE (4 * 1024)

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System

//AI
//None 

//AO
//None

//DIGITAL

//PWM
//None

//QE
//None

//SPI
static unsigned char g_SpiChans[NUM_SPI_CHANS] = {0, 1};
static const char * g_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev0.0", "/dev/spidev0.1"};
static unsigned int g_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
static int g_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
static unsigned char g_I2cChans[NUM_I2C_CHANS] = {1};
static const char * g_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1"};

//UART
static unsigned char g_UartChans[NUM_UART_CHANS] = {0};
static const char * g_UartPaths[NUM_UART_CHANS] = {"/dev/ttyAMA0"};

//SERVO
//None

static void ShortWait(void)
{
    for (int i = 0; i < 150; i++)
	{
		// wait 150 cycles
#ifndef _MSC_VER
		asm volatile("nop");
#endif
    }
}

/****************************************************************************************
**  Digital Channels
****************************************************************************************/
// Raspberry Pi GPIO memory map pointer
static volatile int *gpio_map = MAP_FAILED;

// Raspberry Pi GPIO pins
static const unsigned char g_LinxDioChans[NUM_DIGITAL_CHANS] = {7, 11, 12, 13, 15, 16, 18, 22, 29, 31, 32, 33, 35, 36, 37, 38, 40};
static const unsigned char g_GpioDioChans[NUM_DIGITAL_CHANS] = {4, 17, 18, 27, 22, 23, 24, 25,  5,  6, 12, 13, 19, 16, 26, 20, 21};

LinxRaspiDioChannel::LinxRaspiDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin)
: LinxSysfsDioChannel(debug, linxPin, gpioPin)
{
}

LinxRaspiDioChannel::~LinxRaspiDioChannel()
{
}

LinxChannel *LinxRaspiDioChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxRaspiDioChannel)
	{
		AddRef();
		return this;
	}
	return LinxSysfsDioChannel::QueryInterface(interfaceId);
}

int LinxRaspiDioChannel::SetState(unsigned char state)
{
	if (gpio_map != MAP_FAILED)
	{
		// Set direction and pull-up/down
		setState(state);
		return L_OK;
	}
	return LinxSysfsDioChannel::SetState(state);
}

int LinxRaspiDioChannel::Write(unsigned char value)
{
	if (gpio_map != MAP_FAILED)
	{
		int offset = (m_GpioChan / 32) + (value ? SET_OFFSET : CLR_OFFSET),
			mask = 1 << (m_GpioChan % 32);

		// Set direction
		if ((m_State & GPIO_DIRMASK) != GPIO_OUTPUT)
		{
			setDirection(GPIO_OUTPUT);
		}
		
		*(gpio_map + offset) = mask;
		return L_OK;
	}
	return LinxSysfsDioChannel::Write(value);
}

int LinxRaspiDioChannel::Read(unsigned char *value)
{
	if (gpio_map != MAP_FAILED)
	{
		int offset = PINLEVEL_OFFSET + (m_GpioChan / 32),
			mask = 1 << (m_GpioChan % 32);

		// Set direction
		if ((m_State & GPIO_DIRMASK) != GPIO_INPUT)
		{
			setDirection(GPIO_INPUT);
		}

		*value = *(gpio_map + offset) & mask;
		return L_OK;
	}
	return LinxSysfsDioChannel::Read(value);
}

void LinxRaspiDioChannel::setPull(unsigned char pud)
{
	// Check GPIO register
	int is2711 = *(gpio_map + PULLUPDN_OFFSET_2711_3) != 0x6770696f;
	if (is2711)
	{
		// Pi 4 Pull-up/down method
		int pullreg = PULLUPDN_OFFSET_2711_0 + (m_GpioChan >> 4);
		int pullshift = (m_GpioChan & 0xf) << 1;
		unsigned int pullbits;
		unsigned int pull = 0;
		switch (pud)
		{
			case GPIO_PULLUP:
				pull = 1;
				break;
			case GPIO_PULLDOWN:
				pull = 2;
				break;
			default:
				pull = 0; // switch pull-up and pull-down off
				break;
		}
        pullbits = *(gpio_map + pullreg);
	    pullbits &= ~(3 << pullshift);
		pullbits |= (pull << pullshift);
        *(gpio_map + pullreg) = pullbits;
	}
	else
	{
		// Legacy Pull-up/down method
        int clk_offset = PULLUPDNCLK_OFFSET + (m_GpioChan / 32);
        int shift = (m_GpioChan % 32);
	    if (pud == GPIO_PULLDOWN)
		{
	        *(gpio_map + PULLUPDN_OFFSET) = (*(gpio_map + PULLUPDN_OFFSET) & ~3) | 0x01;
        }
		else if (pud == GPIO_PULLUP)
		{
			*(gpio_map + PULLUPDN_OFFSET) = (*(gpio_map + PULLUPDN_OFFSET) & ~3) | 0x02;
        }
		else
		{
			// pud == PUD_OFF
			*(gpio_map + PULLUPDN_OFFSET) &= ~3;
		}
		ShortWait();
		*(gpio_map + clk_offset) = 1 << shift;
		ShortWait();
		*(gpio_map + PULLUPDN_OFFSET) &= ~3;
		*(gpio_map + clk_offset) = 0;
	}
	m_State = ((m_State & ~GPIO_PULLMASK) | pud);
}

void LinxRaspiDioChannel::setDirection(unsigned char direction)
{
    int offset = FSEL_OFFSET + (m_GpioChan / 10);
	int shift = (m_GpioChan % 10) * 3;

	if ((direction & GPIO_DIRMASK) == GPIO_OUTPUT)
	{
	    *(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift)) | (1 << shift);
	}
	else  // direction == INPUT
	{
		*(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift));
	}
	m_State = ((m_State & ~GPIO_DIRMASK) | direction);
}

void LinxRaspiDioChannel::setState(unsigned char state)
{
	unsigned char dir = state & GPIO_DIRMASK;
	if (dir != (m_State & GPIO_DIRMASK))
	{
		setDirection(dir);
	}

	unsigned char pud = state & GPIO_PULLMASK;
	if (pud && (pud != (m_State & GPIO_PULLMASK)))
	{
		setPull(pud); 
	}
}

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
		// If detection failed, fall back to some sane value
		DeviceId = 0x03;	// Raspberry Pi 2 Model B
		strcpy(m_DeviceName, "Raspberry Pi");
	}

	//------------------------------------- AI ---------------------------------------
	AiResolution = 0;
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	AiRefCodes = NULL;
	
	AiRefIntVals = NULL;
	
	AiRefExtMin = 0;
	AiRefExtMax = 0;

	//------------------------------------- AO ---------------------------------------
	AoResolution = 0;
	AoRefDefault = 0;
	AoRefSet = 0;

	//------------------------------------- DIO --------------------------------------
    // try /dev/gpiomem first - this does not require root privilegs
	if (gpio_map == MAP_FAILED)
	{
		int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
		if (mem_fd > 0)
			gpio_map = (int*)mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
		close(mem_fd);
    }
		
	// Initialize the digital lookup map
	for (int i = 0; i < NUM_DIGITAL_CHANS; i++)
	{
		LinxChannel *chan;
		if (gpio_map == MAP_FAILED)
			chan = new LinxSysfsDioChannel(m_Debug, g_LinxDioChans[i], g_GpioDioChans[i]);
		else
			chan = new LinxRaspiDioChannel(m_Debug, g_LinxDioChans[i], g_GpioDioChans[i]);
		RegisterChannel(IID_LinxDioChannel, g_LinxDioChans[i], chan);
	}

	//------------------------------------- PWM --------------------------------------

	//------------------------------------- QE ---------------------------------------

	//------------------------------------- UART -------------------------------------
	// Store Uart channels in the registry map
	for (int i = 0; i < NUM_UART_CHANS; i++)
	{
		RegisterChannel(IID_LinxUartChannel, g_UartChans[i], new LinxUnixUartChannel(g_UartPaths[i], m_Debug));
	}

	//------------------------------------- I2C -------------------------------------
	// Store I2C master channels in the registry map
	for (int i = 0; i < NUM_I2C_CHANS; i++)
	{
		RegisterChannel(IID_LinxI2cChannel, g_I2cChans[i], new LinxSysfsI2cChannel(g_I2cPaths[i], m_Debug));
	}

	//------------------------------------- SPI -------------------------------------
	// Store SPI channels in the registry map
	SpiDefaultSpeed = 3900000;
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		if (fileExists(g_SpiPaths[i]))
		{
			RegisterChannel(IID_LinxSpiChannel, g_SpiChans[i], new LinxSysfsSpiChannel(g_SpiPaths[i], m_Debug, this, SpiDefaultSpeed));
		}
	}

	//------------------------------------- CAN -------------------------------------

	//------------------------------------ SERVO ------------------------------------

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
 	//------------------------------------- Digital -------------------------------------
    // close gpio memory map if opened 
	if (gpio_map != MAP_FAILED)
	{
		munmap((void *)gpio_map, BLOCK_SIZE);
		gpio_map = MAP_FAILED;
	}
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/

/****************************************************************************************
**  Private Functions
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxRaspberryPi::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	strncpy((char*)buffer, m_DeviceName, length);
	return (unsigned char)strlen(m_DeviceName);
}