/****************************************************************************************
**  LINX Raspberry Pi 2 Model B Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written by Sam Kristoff
**  Modifications by Rolf Kalbermatter
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
#include "LinxDefines.h"
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#include "LinxUtilities.h"
#include "LinxDevice.h"
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
#define ASYNC_RISING_EDGE_OFFSET	31  // 0x007c / 4
#define ASYNC_FALING_EDGE_OFFSET	34  // 0x0088 / 4

#define PULLUPDN_OFFSET             37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET          38  // 0x0098 / 4

#define PULLUPDN_OFFSET_2711_0      57	// 0x00e4 / 4
#define PULLUPDN_OFFSET_2711_1      58  // 0x00e8 / 4
#define PULLUPDN_OFFSET_2711_2      59  // 0x00ec / 4
#define PULLUPDN_OFFSET_2711_3      60  // 0x00f0 / 4

#define BLOCK_SIZE (4 * 1024)

#define CPU_MODEL_BCM2835			0
#define CPU_MODEL_BCM2836			1
#define CPU_MODEL_BCM2837			2
#define CPU_MODEL_BCM2711			3
#define CPU_MODEL_BCM2712			4

#define __nop()   		asm volatile("nop");

/****************************************************************************************
**  Channel implementations
****************************************************************************************/
static void ShortWait(void)
{
    for (int i = 0; i < 150; i++)
	{
		// wait 150 cycles
		__nop();
    }
}

/****************************************************************************************
**  Digital Channels
****************************************************************************************/
// Raspberry Pi GPIO memory map pointer
static volatile unsigned int *gpio_map = (volatile unsigned int*)MAP_FAILED;

// Raspberry Pi GPIO pins
static const unsigned char g_LinxDioChans[NUM_DIGITAL_CHANS] = {3,  5,  7,  8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24, 26, 29, 31, 32, 33, 35, 36, 37, 38, 40};
static const unsigned char g_GpioDioChans[NUM_DIGITAL_CHANS] = {2,  3,  4, 14, 15, 17, 18, 27, 22, 23, 24, 10,  9, 25, 11,  8,  7,  5,  6, 12, 13, 19, 16, 26, 20, 21};
static const unsigned char g_EnabDioChans[NUM_DIGITAL_CHANS] = {2,  2,  0,  1,  1,  0,  8,  0,  0,  0,  0,  4,  4,  0,  4,  4,  4,  0,  0, 16, 16,  8,  0,  0,  8,  8};
/* Enab values: 1: uart pins, 2: I2C pins, 4: SPI pins, 8, PCM pins, 16: PWM pins */


int LinxRaspiDioChannel::LinxRaspiDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin, unsigned char cpuModel) : LinxSysfsDioChannel(debug, linxPin, gpioPin)
{
	m_CpuModel = cpuModel;
}

int LinxRaspiDioChannel::SetState(unsigned char state)
{
	if (gpio_map != MAP_FAILED)
	{
		// Set direction and pull-up/down
		return setState(state);
	}
	return LinxSysfsDioChannel::SetState(state);
}

int LinxRaspiDioChannel::Write(unsigned char value)
{
	if (gpio_map != MAP_FAILED)
	{
		// Set direction
		if ((m_State & GPIO_DIRMASK) != GPIO_OUTPUT)
		{
			if (setDirection(GPIO_OUTPUT))
				return LDIGITAL_PIN_DNE
		}
		
		if (m_CpuModel >= CPU_MODEL_BCM2712)
		{

		}
		else
		{
			int offset = (m_GpioChan / 32) + (value ? SET_OFFSET : CLR_OFFSET);
			unsigned int mask = (1 << (m_GpioChan % 32));

			*(gpio_map + offset) = mask;
		}
		return L_OK;
	}
	return LinxSysfsDioChannel::Write(value);
}

int LinxRaspiDioChannel::Read(unsigned char *value)
{
	if (gpio_map != MAP_FAILED)
	{
		// Set direction
		if ((m_State & GPIO_DIRMASK) != GPIO_INPUT)
		{
			if (setDirection(GPIO_INPUT))
				return LDIGITAL_PIN_DNE
		}

		if (m_CpuModel >= CPU_MODEL_BCM2712)
		{

		}
		else
		{
			int offset = PINLEVEL_OFFSET + (m_GpioChan / 32);
			unsigned int mask = 1 << (m_GpioChan % 32);

			*value = *(gpio_map + offset) & mask;
		}
		return L_OK;
	}
	return LinxSysfsDioChannel::Read(value);
}

int LinxRaspiDioChannel::setPull(unsigned char pud)
{
	pud &= GPIO_PULLMASK;

	// Check GPIO register
	switch (m_CpuModel)
	{
		case CPU_MODEL_BCM2712:
		{
			// Pi 5 Pull-up/down method

			break;
		}
		case CPU_MODEL_BCM2711:
		{
			// Pi 4 Pull-up/down method
			int pullreg = PULLUPDN_OFFSET_2711_0 + (m_GpioChan >> 4);
			int pullshift = (m_GpioChan & 0xf) << 1;
			unsigned int pullbits = *(gpio_map + pullreg), pull = 0;
		
			switch (pud)
			{
				case GPIO_PULLUP:
					pull = 1;
					break;
				case GPIO_PULLDOWN:
					pull = 2;
					break;
				default:
					pull = 0;
					break;
			}

			pullbits &= ~(3 << pullshift);
			pullbits |= (pull << pullshift);
			
			*(gpio_map + pullreg) = pullbits;
			break;
		}
		default:
		{
			// Legacy Pull-up/down method
			int clk_offset = PULLUPDNCLK_OFFSET + (m_GpioChan / 32);
			unsigned int bitval = (1 << (m_GpioChan % 32));
			
			switch (pud)
			{
				case GPIO_PULLUP:
					*(gpio_map + PULLUPDN_OFFSET) = 0x02;
					break;
				case GPIO_PULLDOWN:
					*(gpio_map + PULLUPDN_OFFSET) = 0x01;
					break;
				default:
					*(gpio_map + PULLUPDN_OFFSET) = 0x00;
					break;
			}

			ShortWait();
			*(gpio_map + clk_offset) = bitval;
			ShortWait();
			*(gpio_map + PULLUPDN_OFFSET) = 0;
			*(gpio_map + clk_offset) = 0;
			break;
		}
	}
	m_State = ((m_State & ~GPIO_PULLMASK) | pud);
	return L_OK;
}

int LinxRaspiDioChannel::setDirection(unsigned char direction)
{
	direction &= GPIO_DIRMASK;
	if (m_CpuModel >= CPU_MODEL_BCM2712)
	{

	}
	else
	{
		int offset = FSEL_OFFSET + (m_GpioChan / 10);
		unsigned int shift = (m_GpioChan % 10) * 3;
		unsigned int value = *(gpio_map + offset); 

		// if the according pin is set as alternate function, abort
		if (value &	(GPIO_ALTMASK << shift))
			return LDIGITAL_PIN_DNE;

		if (direction == GPIO_OUTPUT)
		{
			*(gpio_map + offset) = value | (1 << shift);
		}
		else
		{
			*(gpio_map + offset) = value & ~(1 << shift);
		}
	}
	m_State = ((m_State & ~GPIO_DIRMASK) | direction);
	return L_OK;
}
int LinxRaspiDioChannel::setState(unsigned char state)
{
	unsigned char pud = state & GPIO_PULLMASK;
	if (state & GPIO_DIRMASK != (m_State & GPIO_DIRMASK))
	{
		if (setDirection(state))
			return LDIGITAL_PIN_DNE;
	}

	if (pud && (pud != (m_State & GPIO_PULLMASK)))
	{
		setPull(pud); 
	}
	return L_OK;
}

//SPI
static unsigned char g_SpiChans[NUM_SPI_CHANS] = {0, 1, 2, 3, 4};
static const char * g_SpiPaths[NUM_SPI_CHANS] = {"/dev/spidev0.0", "/dev/spidev0.1", "/dev/spidev1.0", "/dev/spidev1.1", "/dev/spidev1.2"};
static int g_SpiDefaultSpeed = 3900000;

//I2C
static unsigned char g_I2cChans[NUM_I2C_CHANS] = {1};
static const char * g_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1"};

//UART
static unsigned char g_UartChans[NUM_UART_CHANS] = {0};
static const char * g_UartPaths[NUM_UART_CHANS] = {"/dev/serial0"};


/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxRaspberryPi::LinxRaspberryPi(LinxFmtChannel *debug) : LinxDevice(debug)
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;

	//-------------------------------- Device Detection ------------------------------
	DeviceFamily = LINX_FAMILY_RASPBERRY;	// Raspberry Pi Family Code
	DeviceId = 0xFF;						// Raspberry Pi Unknown

	m_DeviceCode = 0;
	m_SerialNum = 0;

	unsigned char CpuModel;
	FILE *fp = fopen("/proc/device-tree/serial-number", "r");
	if (fp)
	{
		if (fread(&m_SerialNum, sizeof(m_SerialNum), fp))
		{
		}
		fclose(fp);
	}

	if (fp = fopen("/proc/device-tree/revision", "r"))
	{
		if (fread(&m_DeviceCode, sizeof(m_DeviceCode), fp))
		{
			m_DeviceCode = ntohl(m_DeviceCode);
		}
		fclose(fp);
	}
	else if (fp = fopen("/proc/device-tree/system/linux,revision", "r"))
	{
		if (fread(&m_DeviceCode, sizeof(m_DeviceCode), fp))
		{
			m_DeviceCode = ntohl(m_DeviceCode);
		}
		fclose(fp);
	}
	else if ((fp = fopen("/proc/cpuinfo", "r")))
	{
		char buffer[1024];
		while (!feof(fp) && fgets(buffer, sizeof(buffer), fp))
		{
			sscanf(buffer, "Hardware	: %s", hardware);
			if (strcmp(hardware, "BCM2708") == 0 ||
                strcmp(hardware, "BCM2709") == 0 ||
                strcmp(hardware, "BCM2835") == 0 ||
                strcmp(hardware, "BCM2836") == 0 ||
                strcmp(hardware, "BCM2837") == 0 )
			{
				sscanf(buffer, "Revision	: %x", m_DeviceCode);
			}
		}
		fclose(fp);
	}
	if (m_DeviceCode))
	{
		DeviceId = (m_DeviceCode & 0x8000000) ? (unsigned char)((m_DeviceCode & 0xFF0) >> 4) : 0;
		CpuModel = (m_DeviceCode & 0x8000000) ? (unsigned char)((m_DeviceCode & 0xF000) >> 12) : 0;
	}

	int fd = open("/proc/device-tree/model", O_RDONLY);
	if (fd >= 0)
	{
		struct stat buf;
		if (fstat(fd, &buf) == 0)
		{
			int len = read(fd, m_DeviceName, Min(buf.st_size, (off_t)(DEVICE_NAME_MAX - 1)));
			if (len > 0)
			{
				m_DeviceName[len] = 0;
			}
		}
		close(fd);
	}

	if (DeviceId == 0xFF)
	{
		// If detection failed, fall back to some basic value
		DeviceId = 0x04;	// Raspberry Pi 2 Model B
		strcpy(m_DeviceName, "Raspberry Pi");
	}

	//------------------------------------- AI ---------------------------------------
	AiResolution = 0;
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	
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
		LinxChannel *channel;
		if (gpio_map == MAP_FAILED)
			channel = new LinxSysfsDioChannel(m_Debug, g_LinxDioChans[i], g_GpioDioChans[i]);
		else
			channel = new LinxRaspiDioChannel(m_Debug, g_LinxDioChans[i], g_GpioDioChans[i], CpuModel);
		if (channel)
			RegisterChannel(IID_LinxDioChannel, g_LinxDioChans[i], channel);
	}

	//------------------------------------- PWM --------------------------------------

	//------------------------------------- QE ---------------------------------------

	//------------------------------------- UART -------------------------------------
	// Store Uart channels in the registry map
	for (int i = 0; i < NUM_UART_CHANS; i++)
	{
		LinxChannel *channel = new LinxUnixUartChannel(m_Debug, g_UartPaths[i]);
		if (channel)
			RegisterChannel(IID_LinxUartChannel, g_UartChans[i], channel);
	}

	//------------------------------------- I2C -------------------------------------
	// Store I2C master channels in the registry map
	for (int i = 0; i < NUM_I2C_CHANS; i++)
	{
		LinxChannel *channel = new LinxSysfsI2cChannel(g_I2cPaths[i], m_Debug);
		if (channel)
			RegisterChannel(IID_LinxI2cChannel, g_I2cChans[i], channel);
	}

	//------------------------------------- SPI -------------------------------------
	// Store SPI channels in the registry map
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		if (fileExists(g_SpiPaths[i]))
		{
			LinxChannel *channel = new LinxSysfsSpiChannel(g_SpiPaths[i], m_Debug, this, g_SpiDefaultSpeed);
			if (channel)
				RegisterChannel(IID_LinxSpiChannel, g_SpiChans[i], channel);
		}
	}

	//------------------------------------- CAN -------------------------------------

	//------------------------------------ SERVO ------------------------------------

	// TODO Load User Config Data From Non Volatile Storage
	//userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
}

//Destructor
LinxRaspberryPi::~LinxRaspberryPi(void)
{
 	//------------------------------------- Digital -------------------------------------
    // close gpio memory map if opened 
	if (gpio_map != MAP_FAILED)
	{
		munmap((void *)gpio_map, BLOCK_SIZE);
		gpio_map = (volatile int*)MAP_FAILED;
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