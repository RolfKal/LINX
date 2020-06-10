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
#include <fstream>
#include <sys/stat.h>
#ifndef _MSC_VER
#include <alloca.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#else
#include <io.h>
#include <malloc.h>
/* c_cc characters */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSWTC 7
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define VEOL 11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

/* c_iflag bits */
#define IGNBRK	0000001
#define BRKINT	0000002
#define IGNPAR	0000004
#define PARMRK	0000010
#define INPCK	0000020
#define ISTRIP	0000040
#define INLCR	0000100
#define IGNCR	0000200
#define ICRNL	0000400
#define IUCLC	0001000
#define IXON	0002000
#define IXANY	0004000
#define IXOFF	0010000
#define IMAXBEL	0020000
#define IUTF8	0040000

/* c_oflag bits */
#define OPOST	0000001
#define OLCUC	0000002
#define ONLCR	0000004
#define OCRNL	0000010
#define ONOCR	0000020
#define ONLRET	0000040
#define OFILL	0000100
#define OFDEL	0000200
#define NLDLY	0000400
#define   NL0	0000000
#define   NL1	0000400
#define CRDLY	0003000
#define   CR0	0000000
#define   CR1	0001000
#define   CR2	0002000
#define   CR3	0003000
#define TABDLY	0014000
#define   TAB0	0000000
#define   TAB1	0004000
#define   TAB2	0010000
#define   TAB3	0014000
#define   XTABS	0014000
#define BSDLY	0020000
#define   BS0	0000000
#define   BS1	0020000
#define VTDLY	0040000
#define   VT0	0000000
#define   VT1	0040000
#define FFDLY	0100000
#define   FF0	0000000
#define   FF1	0100000

/* c_cflag bit meaning */
#define CBAUD	0010017
#define  B0	0000000		/* hang up */
#define  B50	0000001
#define  B75	0000002
#define  B110	0000003
#define  B134	0000004
#define  B150	0000005
#define  B200	0000006
#define  B300	0000007
#define  B600	0000010
#define  B1200	0000011
#define  B1800	0000012
#define  B2400	0000013
#define  B4800	0000014
#define  B9600	0000015
#define  B19200	0000016
#define  B38400	0000017
#define EXTA B19200
#define EXTB B38400
#define CSIZE	0000060
#define   CS5	0000000
#define   CS6	0000020
#define   CS7	0000040
#define   CS8	0000060
#define CSTOPB	0000100
#define CREAD	0000200
#define PARENB	0000400
#define PARODD	0001000
#define HUPCL	0002000
#define CLOCAL	0004000
#define CBAUDEX 0010000
#define    BOTHER 0010000
#define    B57600 0010001
#define   B115200 0010002
#define   B230400 0010003
#define   B460800 0010004
#define   B500000 0010005
#define   B576000 0010006
#define   B921600 0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017
#define CIBAUD	  002003600000	/* input baud rate */
#define CMSPAR	  010000000000	/* mark or space (stick) parity */
#define CRTSCTS	  020000000000	/* flow control */

#define IBSHIFT	  16		/* Shift from CBAUD to CIBAUD */

/* c_lflag bits */
#define ISIG	0000001
#define ICANON	0000002
#define XCASE	0000004
#define ECHO	0000010
#define ECHOE	0000020
#define ECHOK	0000040
#define ECHONL	0000100
#define NOFLSH	0000200
#define TOSTOP	0000400
#define ECHOCTL	0001000
#define ECHOPRT	0002000
#define ECHOKE	0004000
#define FLUSHO	0010000
#define PENDIN	0040000
#define IEXTEN	0100000
#define EXTPROC	0200000

/* tcflow() and TCXONC use these */
#define	TCOOFF		0
#define	TCOON		1
#define	TCIOFF		2
#define	TCION		3

/* tcflush() and TCFLSH use these */
#define	TCIFLUSH	0
#define	TCOFLUSH	1
#define	TCIOFLUSH	2

/* tcsetattr uses these */
#define	TCSANOW		0
#define	TCSADRAIN	1
#define	TCSAFLUSH	2
typedef unsigned char	cc_t;
typedef unsigned int	speed_t;
typedef unsigned int	tcflag_t;
#define NCCS 19
struct termios {
	tcflag_t c_iflag;		/* input mode flags */
	tcflag_t c_oflag;		/* output mode flags */
	tcflag_t c_cflag;		/* control mode flags */
	tcflag_t c_lflag;		/* local mode flags */
	cc_t c_line;			/* line discipline */
	cc_t c_cc[NCCS];		/* control characters */
};

struct termios2 {
	tcflag_t c_iflag;		/* input mode flags */
	tcflag_t c_oflag;		/* output mode flags */
	tcflag_t c_cflag;		/* control mode flags */
	tcflag_t c_lflag;		/* local mode flags */
	cc_t c_line;			/* line discipline */
	cc_t c_cc[NCCS];		/* control characters */
	speed_t c_ispeed;		/* input speed */
	speed_t c_ospeed;		/* output speed */
};

#define FIONREAD 0
#define TCFLSH 0
#define TCSETS 0
#define TCGETS 0
#define TCSETS2 0
#define TCGETS2 0

int ioctl(int, int, void*);

#define SPI_CPHA		0x01
#define SPI_CPOL		0x02

#define SPI_MODE_0		(0|0)
#define SPI_MODE_1		(0|SPI_CPHA)
#define SPI_MODE_2		(SPI_CPOL|0)
#define SPI_MODE_3		(SPI_CPOL|SPI_CPHA)

#define SPI_CS_HIGH		0x04
#define SPI_LSB_FIRST		0x08
#define SPI_3WIRE		0x10
#define SPI_LOOP		0x20
#define SPI_NO_CS		0x40
#define SPI_READY		0x80
#define SPI_TX_DUAL		0x100
#define SPI_TX_QUAD		0x200
#define SPI_RX_DUAL		0x400
#define SPI_RX_QUAD		0x800

#define SPI_IOC_MAGIC			'k'

struct spi_ioc_transfer {
	unsigned long		tx_buf;
	unsigned long		rx_buf;

	unsigned int		len;
	unsigned int		speed_hz;

	unsigned short		delay_usecs;
	unsigned char		bits_per_word;
	unsigned char		cs_change;
	unsigned char		tx_nbits;
	unsigned char		rx_nbits;
	unsigned char		pad;

	/* If the contents of 'struct spi_ioc_transfer' ever change
	 * incompatibly, then the ioctl number (currently 0) must change;
	 * ioctls with constant size fields get a bit more in the way of
	 * error checking than ones (like this) where that field varies.
	 *
	 * NOTE: struct layout is the same in 64bit and 32bit userspace.
	 */
};

/* not all platforms use <asm-generic/ioctl.h> or _IOC_TYPECHECK() ... */
#define SPI_IOC_MESSAGE(N) 0


/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) (limited to 8 bits) */
#define SPI_IOC_RD_MODE		0
#define SPI_IOC_WR_MODE		0

/* Read / Write SPI bit justification */
#define SPI_IOC_RD_LSB_FIRST	0
#define SPI_IOC_WR_LSB_FIRST	0

/* Read / Write SPI device word length (1..N) */
#define SPI_IOC_RD_BITS_PER_WORD	0
#define SPI_IOC_WR_BITS_PER_WORD	0

/* Read / Write SPI device default max speed hz */
#define SPI_IOC_RD_MAX_SPEED_HZ		0
#define SPI_IOC_WR_MAX_SPEED_HZ		0

/* Read / Write of the SPI mode field */
#define SPI_IOC_RD_MODE32		0
#define SPI_IOC_WR_MODE32		0




#endif

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
**  Protected Functions
****************************************************************************************/
int LinxLinuxDevice::DigitalWrite(unsigned char channel, unsigned char value)
{
	return DigitalWrite(1, &channel, &value);
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxLinuxDevice::GetAiChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AiValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}


unsigned char LinxLinuxDevice::GetAoChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}


unsigned char LinxLinuxDevice::GetDioChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = DigitalValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetQeChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetPwmChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetSpiChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetI2cChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetUartChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetCanChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

unsigned char LinxLinuxDevice::GetServoChans(unsigned char *buffer, unsigned char length)
{
	map<unsigned char, FILE*> m = AoValueHandles;
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (map<unsigned char, FILE*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

//------------------------------------- Analog -------------------------------------
int LinxLinuxDevice::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::AnalogSetRef(unsigned char mode, unsigned int voltage)
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
	int numDirBytes = ((numChans + 7) >> 3);
	unsigned char *directions = (unsigned char *)alloca(numDirBytes);
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
	int numDirBytes = ((numChans + 7) >> 3);
	unsigned char *directions = (unsigned char *)alloca(numDirBytes);
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
	int numDirBytes = ((numChans + 7) >> 3);
	unsigned char *directions = (unsigned char *)alloca(numDirBytes);
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
	int numDirBytes = ((numChans + 7) >> 3);
	unsigned char *directions = (unsigned char *)alloca(numDirBytes);
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

int LinxLinuxDevice::DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width)
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

int LinxLinuxDevice::SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
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

	struct spi_ioc_transfer transfer = {0};

	//Set CS As Output And Make Sure CS Starts Idle
	if (csChan)
		DigitalWrite(csChan, (~csLL & 0x01) );

	for (int i = 0; i < numFrames; i++)
	{
		//Setup Transfer
		transfer.tx_buf = (unsigned long)(sendBuffer + nextByte);
		transfer.rx_buf = (unsigned long)(recBuffer + nextByte);
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
			I2cHandles[channel] = fd;
		}
	}
	return L_OK;
}

int LinxLinuxDevice::I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
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
int LinxLinuxDevice::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return UartOpen(channel, baudRate, actualBaud, 8, 1, None);
}

int LinxLinuxDevice::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	int fd = UartHandles[channel];
	bool init = false;

	//Open UART	Handle If Not Already Open
	if (fd < 0)
	{
		fd = open(UartPaths[channel].c_str(),  O_RDWR);
		if (fd < 0)
		{
			DebugPrint("UART Fail - Failed To Open UART Handle - ");
			DebugPrintln(UartPaths[channel].c_str());
			return  LUART_OPEN_FAIL;
		}
		else
		{
			UartHandles[channel] = fd;
			init = true;
		}
	}
	int status = UartSetBaudRate(fd, baudRate, actualBaud, init);
	if (status != L_OK)
	{
		DebugPrintln("Failed to set baud rate");
	}
	else
	{
		status = UartSetBitSize(fd, dataBits, stopBits);
		if (status != L_OK)
		{
			DebugPrintln("Failed to set databits and stop bits");
		}
		else
		{
			status = UartSetParity(fd, parity);
			if (status != L_OK)
			{
				DebugPrintln("Failed to set parity");
			}
		}
	}
	return status;
}

int LinxLinuxDevice::UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return UartSetBaudRate(UartHandles[channel], baudRate, actualBaud, false);
}

int LinxLinuxDevice::UartSetBaudRate(int fd, unsigned int baudRate, unsigned int* actualBaud, bool init)
{
	int temp;
	struct termios2 options;
	int ioctlval = TCSETS2;
	if (ioctl(fd, TCGETS2, &options) < 0)
	{
		if (ioctl(fd, TCGETS, &options) < 0)
			return LERR_IO;

		//Get Closest Support Baud Rate Without Going Over
		//Loop Over All Supported Uart Speeds
		for (temp = 0; temp < NumUartSpeeds; temp++)
		{
			if (baudRate < *(UartSupportedSpeeds + temp))
			{
				//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
				break;
			}
		}
		//Once Loop Completes Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
		//If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud
		if (temp != 0)
			temp--;

		//Store Actual Baud Used
		*actualBaud = (unsigned int) *(UartSupportedSpeeds + temp);
		temp = (options.c_cflag & ~(CBAUD | CIBAUD)) | UartSupportedSpeedsCodes[temp] | UartSupportedSpeedsCodes[temp] << IBSHIFT;
		ioctlval = TCSETS;
	}
	else
	{
		*actualBaud = baudRate;
		options.c_ispeed = baudRate;
		options.c_ospeed = baudRate;
		temp = ((options.c_cflag & ~(CBAUD | CIBAUD)) | BOTHER | BOTHER << IBSHIFT);
	}

	//Set Baud Rate
	options.c_cflag = temp;
	if (init)
	{
		options.c_iflag = IGNPAR; // Ignore parity errors
		options.c_oflag = 0;
		options.c_lflag = 0;
	}
	else
	{
		options.c_iflag |= IGNPAR; // Ignore parity errors
	}
	ioctl(fd, TCFLSH, TCIFLUSH);
	if (ioctl(fd, ioctlval, &options) < 0)
		return LERR_IO;

	return  L_OK;
}

#define BIT_SIZE_OFFSET	5
#define NUM_BIT_SIZES	4

static const int BitSizes[NUM_BIT_SIZES] = {CS5, CS6, CS7, CS8};

int LinxLinuxDevice::UartSetBitSize(int fd, unsigned char dataBits, unsigned char stopBits)
{
	if (dataBits > 0 && (dataBits < BIT_SIZE_OFFSET || dataBits >= BIT_SIZE_OFFSET + NUM_BIT_SIZES))
		return LERR_BADPARAM;

	if (stopBits > 2)
		return LERR_BADPARAM;

	if (!dataBits && !stopBits)
		return L_OK;

	struct termios options;
	if (ioctl(fd, TCGETS, &options) >= 0)
	{
		if (dataBits)
			options.c_cflag = (options.c_cflag & ~CSIZE) | BitSizes[dataBits - BIT_SIZE_OFFSET];
		if (stopBits)
			options.c_cflag &= ~CSTOPB;
		if (stopBits == 2)
			options.c_cflag |= CSTOPB;

		if (ioctl(fd, TCSETS, &options) >= 0)
			return  L_OK;
	}
	return LERR_IO;
}

#define NUM_PARITY_SIZES	3

static const int Parity[NUM_PARITY_SIZES] = {0, PARENB, PARENB | PARODD};

int LinxLinuxDevice::UartSetParity(int fd, unsigned char parity)
{
	if (parity <= NUM_PARITY_SIZES)
		return LERR_BADPARAM;

	struct termios options;
	if (ioctl(fd, TCGETS, &options) >= 0)
	{
		options.c_cflag = (options.c_cflag & ~(PARENB | PARODD)) | Parity[parity];
		if (ioctl(fd, TCSETS, &options) >= 0)
			return  L_OK;
	}
	return LERR_IO;
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
unsigned int LinxLinuxDevice::GetMilliSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	//return (mTime.tv_nsec / 1000000);
	return ( ((unsigned long) mTime.tv_sec * 1000) + mTime.tv_nsec / 1000000);
}

unsigned int LinxLinuxDevice::GetSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	return mTime.tv_sec;
}

void LinxLinuxDevice::DelayMs(unsigned int ms)
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
			fprintf(PwmPeriodHandles[channels[i]], "%u", PwmDefaultPeriod);
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

bool LinxLinuxDevice::uartSupportsVarBaudrate(const char* path, int baudrate)
{
	return false;
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

bool LinxLinuxDevice::fileExists(const char* directory, const char* fileName, unsigned int timeout)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);
	struct stat buffer;

	unsigned int startTime = GetMilliSeconds();
	while (GetMilliSeconds() - startTime < timeout)
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
