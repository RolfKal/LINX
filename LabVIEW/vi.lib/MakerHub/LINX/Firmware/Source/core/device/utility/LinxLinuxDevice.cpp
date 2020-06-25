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
#define GPIO_EXPORTED	0x80

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDefines.h"

#include <map>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#if Unix
#include <alloca.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#elif Win32
#include <io.h>
#include <malloc.h>

#define open	_open
#define read	_read
#define write	_write
#define close	_close

#define O_NOCTTY	1
#define O_NONBLOCK	2

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

#define TCFLSH 0
#define TCSETS 0
#define TCGETS 0
#define TCSETS2 0
#define TCGETS2 0
#define FIONREAD 0

int ioctl(int, unsigned int, ...);

#define POLLIN 0

struct pollfd
{
    int  fd;
    short   events;
    short   revents;
};

int poll(struct pollfd *fdArray, unsigned int fds, int timeout);

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

#define I2C_SLAVE 0

#define CLOCK_MONOTONIC 0
typedef struct
{
	long tv_sec;
	unsigned long tv_nsec;
}
timespec;
void clock_gettime(int, timespec *mTime);
void usleep(long);
#endif

#include "LinxUtilities.h"
#include "LinxDevice.h"
#include "LinxLinuxDevice.h"

//------------------------------------- Digital -------------------------------------
LinxSysfsAiChannel::LinxSysfsAiChannel(LinxFmtChannel *debug, const char *channelName) : LinxAiChannel(channelName, debug)
{
	m_ValHandle = NULL;
}

LinxSysfsAiChannel::~LinxSysfsAiChannel()
{
	if (m_ValHandle)
	{
		fclose(m_ValHandle);
	}
}

LinxChannel *LinxSysfsAiChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxSysfsAiChannel)
	{
		AddRef();
		return this;
	}
	return LinxAiChannel::QueryInterface(interfaceId);
}

// Open direction and value handles if it is not already open
int LinxSysfsAiChannel::SmartOpen()
{
	// Open value handle if it is not already open
	if (m_ValHandle == NULL)
	{
		m_Debug->Writeln("Opening AI Value Handle");
		m_ValHandle = fopen(m_ChannelName, "r+w+");
		if (m_ValHandle == NULL)
		{
			m_Debug->Writeln("AI Fail - Unable To Open Value File Handle");
			return L_UNKNOWN_ERROR;
		}
	}
	return L_OK;
}

//------------------------------------- Digital -------------------------------------
LinxSysfsDioChannel::LinxSysfsDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin) : LinxDioChannel("LinxDioPin", debug)
{
	m_GpioChan = gpioPin;
	m_LinxChan = linxPin;
	m_ValHandle = NULL;
	m_DirHandle = NULL;
	m_EdgeHandle = NULL;
}

LinxSysfsDioChannel::~LinxSysfsDioChannel()
{
	if (m_EdgeHandle != NULL)
	{			
		fclose(m_EdgeHandle);
	}

	if (m_DirHandle = NULL)
	{
		// return to input
		fprintf(m_DirHandle, "in");
		fclose(m_DirHandle);
	}

	if (m_ValHandle)
	{
		fclose(m_ValHandle);
		if (m_State & GPIO_EXPORTED)
		{
			FILE* digitalExportHandle = fopen("/sys/class/gpio/unexport", "r+w+");
			if (digitalExportHandle != NULL)
			{
				fprintf(digitalExportHandle, "%d", m_GpioChan);
				fclose(digitalExportHandle);
			}
		}
	}
}

LinxChannel *LinxSysfsDioChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxSysfsDioChannel)
	{
		AddRef();
		return this;
	}
	return LinxDioChannel::QueryInterface(interfaceId);
}

// Open direction and value handles if it is not already open
int LinxSysfsDioChannel::SmartOpen()
{
	char gpioPath[64];

	// Open direction handle if it is not already open
	if (m_DirHandle == NULL)
	{
		m_Debug->Write("Opening Digital Direction Handle For LINX DIO ");
		m_Debug->Write(m_LinxChan, DEC);
		m_Debug->Write("(GPIO ");
		m_Debug->Write(m_GpioChan, DEC);
		m_Debug->Writeln(")");

		sprintf(gpioPath, "/sys/class/gpio/gpio%d/direction", m_GpioChan);
		if (!fileExists(gpioPath))
		{
			FILE* digitalExportHandle = fopen("/sys/class/gpio/export", "r+w+");
			if (digitalExportHandle != NULL)
			{
				fprintf(digitalExportHandle, "%d", m_GpioChan);
				fclose(digitalExportHandle);
				m_State |= GPIO_EXPORTED;
			}
			else
			{
				m_Debug->Writeln("Digital Fail - Unable To Open Direction File Handle");
				return L_UNKNOWN_ERROR;
			}
		}
		m_DirHandle = fopen(gpioPath, "r+w+");
	}

	// Open value handle if it is not already open
	if (m_ValHandle == NULL)
	{
		m_Debug->Writeln("Opening Digital Value Handle");
		sprintf(gpioPath, "/sys/class/gpio/gpio%d/value", m_GpioChan);
		m_ValHandle = fopen(gpioPath, "r+w+");
		if (m_ValHandle == NULL)
		{
			m_Debug->Writeln("Digital Fail - Unable To Open Value File Handle");
			return L_UNKNOWN_ERROR;
		}
	}
	return L_OK;
}

int LinxSysfsDioChannel::SetState(unsigned char state)
{
	char direction = state & GPIO_DIRMASK;
	if ((m_State & GPIO_DIRMASK) != direction)
	{
		//Set As Input or Output
		fprintf(m_DirHandle, direction ? "out" : "in");
		fflush(m_DirHandle);
		m_State = (m_State & ~GPIO_DIRMASK) | direction;
	}
	return L_OK;
}

int LinxSysfsDioChannel::Write(unsigned char value)
{
	// Set direction
	int status = SetState(GPIO_OUTPUT);
	if (!status)
	{
		// Set Value
		fprintf(m_ValHandle, value ? "1" : "0");
		fflush(m_ValHandle);
	}
	return status;
}

int LinxSysfsDioChannel::Read(unsigned char *value)
{
	char valPath[128];

	// Set direction
	int status = SetState(GPIO_INPUT);
	if (!status)
	{
		//Reopen Value Handle
		sprintf(valPath, "/sys/class/gpio/gpio%d/value", m_GpioChan);
		m_ValHandle = freopen(valPath, "r+w+", m_ValHandle);

		//Read From Next Pin
		fscanf(m_ValHandle, "%hhu", value);
	}
	return status;
}

int LinxSysfsDioChannel::WriteSquareWave(unsigned int freq, unsigned int duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxSysfsDioChannel::ReadPulseWidth(unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//------------------------------------- UART -------------------------------------
static unsigned int g_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
static unsigned int g_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

LinxUnixUartChannel::LinxUnixUartChannel(const char *channelName, LinxFmtChannel *debug) : LinxUartChannel(channelName, debug)
{
	m_Fd = -1;
}

LinxUnixUartChannel::~LinxUnixUartChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
}

LinxChannel *LinxUnixUartChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxUnixUartChannel)
	{
		AddRef();
		return this;
	}
	return LinxUartChannel::QueryInterface(interfaceId);
}

int LinxUnixUartChannel::SmartOpen()
{
	if (m_Fd < 0)
	{
		struct termios2 options;

		// Open device as read/write, no CTRL-C handling and non-blocking
		m_Fd = open(m_ChannelName, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (m_Fd < 0)
		{
			m_Debug->Write("UART Fail - Failed To Open UART Handle - ");
			m_Debug->Writeln(m_ChannelName);
			return  LUART_OPEN_FAIL;
		}
		if (ioctl(m_Fd, TCGETS, &options) < 0)
			return LERR_IO;

		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Use raw input mode
		options.c_oflag &= ~OPOST;							// Use raw output mode
	}
	return L_OK;
}

int LinxUnixUartChannel::SetSpeed(unsigned int baudRate, unsigned int* actualBaud)
{
	struct termios2 options;
	int ioctlval = TCSETS2;
	int status = SmartOpen();
	if (status)
		return status;

	// If driver supporst newer TCGETS2 ioctl call, assume that we can set arbitrary baudrates
	// Actual used baudrate may still be different due to discrete clock generation
	if (ioctl(m_Fd, TCGETS2, &options) < 0)
	{
		int temp;

		if (ioctl(m_Fd, TCGETS, &options) < 0)
			return LERR_IO;

		// Get Closest Support Baud Rate Without Going Over
		// Loop Over All Supported Uart Speeds
		for (temp = 0; temp < NUM_UART_SPEEDS; temp++)
		{
			if (baudRate < *(g_UartSupportedSpeeds + temp))
			{
				//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
				break;
			}
		}
		// Once Loop Completes Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
		// If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud
		if (temp != 0)
			temp--;

		//Store Actual Baud Used
		*actualBaud = (unsigned int) *(g_UartSupportedSpeeds + temp);
		options.c_cflag = (options.c_cflag & ~(CBAUD | CIBAUD)) | g_UartSupportedSpeedsCodes[temp] | g_UartSupportedSpeedsCodes[temp] << IBSHIFT;
		ioctlval = TCSETS;
	}
	else
	{
		*actualBaud = baudRate;
		options.c_ispeed = baudRate;
		options.c_ospeed = baudRate;
		options.c_cflag = ((options.c_cflag & ~(CBAUD | CIBAUD)) | BOTHER | BOTHER << IBSHIFT);
	}

	//Set Baud Rate
	ioctl(m_Fd, TCFLSH, TCIFLUSH);
	if (ioctl(m_Fd, ioctlval, &options) < 0)
		return LERR_IO;
	m_init = false;
	return  L_OK;
}

#define BIT_SIZE_OFFSET	5
#define NUM_BIT_SIZES	4

static const int BitSizes[NUM_BIT_SIZES] = {CS5, CS6, CS7, CS8};

int LinxUnixUartChannel::SetBitSizes(unsigned char dataBits, unsigned char stopBits)
{
	int status = SmartOpen();
	if (status)
		return status;

	if (dataBits > 0 && (dataBits < BIT_SIZE_OFFSET || dataBits >= BIT_SIZE_OFFSET + NUM_BIT_SIZES))
		return LERR_BADPARAM;

	if (stopBits > 2)
		return LERR_BADPARAM;

	if (!dataBits && !stopBits)
		return L_OK;

	struct termios options;
	if (ioctl(m_Fd, TCGETS, &options) >= 0)
	{
		if (dataBits)
			options.c_cflag = (options.c_cflag & ~CSIZE) | BitSizes[dataBits - BIT_SIZE_OFFSET];
		if (stopBits == 1)
			options.c_cflag &= ~CSTOPB;
		else if (stopBits == 2)
			options.c_cflag |= CSTOPB;

		if (ioctl(m_Fd, TCSETS, &options) >= 0)
			return  L_OK;
	}
	return LERR_IO;
}

#define NUM_PARITY_SIZES	5

static const int Parity[NUM_PARITY_SIZES] = {0, PARENB, PARENB | PARODD, PARENB | PARODD | CMSPAR, PARENB | CMSPAR};

int LinxUnixUartChannel::SetParity(LinxUartParity parity)
{
	int status = SmartOpen();
	if (status)
		return status;

	if (parity > NUM_PARITY_SIZES)
		return LERR_BADPARAM;

	struct termios options;
	if (ioctl(m_Fd, TCGETS, &options) >= 0)
	{
		if (parity)
		{
			// Use selected parity
			options.c_cflag = (options.c_cflag & ~(PARENB | PARODD | CMSPAR)) | Parity[parity - 1];
			options.c_iflag &= ~IGNPAR; // Don't ignore parity errors
		}
		else
		{
			// Ignore parity
			options.c_cflag = (options.c_cflag & ~(PARENB | PARODD | CMSPAR));
			options.c_iflag |= IGNPAR; // Ignore parity errors
		}
		options.c_iflag |= (INPCK | ISTRIP);
		if (ioctl(m_Fd, TCSETS, &options) >= 0)
			return  L_OK;
	}
	return LERR_IO;
}

int LinxUnixUartChannel::GetBytesAvail(int* numBytesAvailable)
{
	// Read the number of bytes that are available at the input queue
	if (ioctl(m_Fd, FIONREAD, numBytesAvailable) < 0)
		return LUART_READ_FAIL;
	return L_OK;
}

int LinxUnixUartChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	int status = SmartOpen();
	if (status)
		return status;

	*numBytesRead = 0;

	if (recBuffer && numBytes)
	{
		struct pollfd fds[1];
		int retval, offset = 0;
		unsigned long long start = getUsTicks();

		fds[0].fd = m_Fd;
		fds[0].events = POLLIN ;

		while (*numBytesRead < numBytes)
		{
			retval = poll(fds, 1, timeout < 0 ? -1 : min(timeout - (int)(getUsTicks() - start), 0));
			if (retval <= 0)
				return retval ? LUART_READ_FAIL : LUART_TIMEOUT;

			// Read bytes from input buffer
			retval = read(m_Fd, recBuffer + offset, numBytes - offset);
			if (retval < 0)
				return LUART_READ_FAIL;
			*numBytesRead += retval;
		}
	}
	else
	{
		// Check how many bytes are available
		if (ioctl(m_Fd, FIONREAD, numBytesRead) < 0)
			return LUART_READ_FAIL;
	}
	return status;
}

int LinxUnixUartChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	int status = SmartOpen();
	if (status)
		return status;

	int bytesSent = write(m_Fd, sendBuffer, numBytes);
	if (bytesSent != numBytes)
	{
		return LUART_WRITE_FAIL;
	}
	return  L_OK;
}

int LinxUnixUartChannel::Close()
{
	if (m_Fd >= 0)
		close(m_Fd);
	return L_OK;
}

//------------------------------------- I2C -------------------------------------
LinxSysfsI2cChannel::LinxSysfsI2cChannel(const char *channelName, LinxFmtChannel *debug) : LinxI2cChannel(channelName, debug)
{
	m_Fd = -1;
}

LinxSysfsI2cChannel::~LinxSysfsI2cChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
}

LinxChannel *LinxSysfsI2cChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxSysfsI2cChannel)
	{
		AddRef();
		return this;
	}
	return LinxI2cChannel::QueryInterface(interfaceId);
}

int LinxSysfsI2cChannel::Open()
{
	if (m_Fd < 0)
	{
		m_Fd = open(m_ChannelName, O_RDWR);
		if (m_Fd < 0)
		{
			m_Debug->Writeln("I2C Fail - Failed To Open I2C Channel");
			return  LI2C_OPEN_FAIL;
		}
	}
	return L_OK;
}

int LinxSysfsI2cChannel::SetSpeed(unsigned int speed, unsigned int* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxSysfsI2cChannel::Write(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned char* sendBuffer)
{
	if (m_Fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	//Check EOF - Currently Only Support 0x00
	if (eofConfig != EOF_STOP)
	{
		m_Debug->Writeln("I2C Fail - EOF Not Supported");
		return LI2C_EOF;
	}

	//Set Slave Address
	if (ioctl(m_Fd, I2C_SLAVE, slaveAddress) < 0)
	{
		//Failed To Set Slave Address
		m_Debug->Writeln("I2C Fail - Failed To Set Slave Address");
		return LI2C_SADDR;
	}

	//Write Data
	if (write(m_Fd, sendBuffer, numBytes) != numBytes)
	{
		m_Debug->Writeln("I2C Fail - Failed To Write All Data");
		return LI2C_WRITE_FAIL;
	}
	return L_OK;
}

int LinxSysfsI2cChannel::Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	//Check EOF - Currently Only Support 0x00
	if (eofConfig != EOF_STOP)
	{
		return LI2C_EOF;
	}

	//Set Slave Address
	if (ioctl(m_Fd, I2C_SLAVE, slaveAddress) < 0)
	{
		//Failed To Set Slave Address
		return LI2C_SADDR;
	}

	if (read(m_Fd, recBuffer, numBytes) < numBytes)
	{
		return LI2C_READ_FAIL;
	}
	return L_OK;
}

int LinxSysfsI2cChannel::Close()
{
	if ((m_Fd >= 0) && (close(m_Fd) < 0))
		return LI2C_CLOSE_FAIL;
	m_Fd = -1;
	return L_OK;
}

//------------------------------------- SPI -------------------------------------
LinxSysfsSpiChannel::LinxSysfsSpiChannel(const char *channelName, LinxFmtChannel *debug, LinxLinuxDevice *device, unsigned int maxSpeed) : LinxSpiChannel(channelName, debug)
{
//	device->AddRef();
	m_Device = device;
	m_CurrentSpeed = maxSpeed;
	m_MaxSpeed = maxSpeed;
	m_Fd = -1;
}

LinxSysfsSpiChannel::~LinxSysfsSpiChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
//	m_Device->Release();
}

LinxChannel *LinxSysfsSpiChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxSysfsSpiChannel)
	{
		AddRef();
		return this;
	}
	return LinxSpiChannel::QueryInterface(interfaceId);
}

int LinxSysfsSpiChannel::Open()
{
	if (m_Fd < 0)
	{
		m_Fd = open(m_ChannelName, O_RDWR);
		if (m_Fd < 0)
		{
			return LSPI_OPEN_FAIL;
		}
		else
		{
			// Default To Mode 0 With No CS (LINX Uses GPIO When Performing Write)
			unsigned char spi_mode = SPI_MODE_0 | SPI_NO_CS;
			if (ioctl(m_Fd, SPI_IOC_WR_MODE, &spi_mode) < 0)
			{
				m_Debug->Writeln("Failed To Set SPI Mode");
				m_Debug->Writeln(spi_mode, BIN);
				close(m_Fd);
				return LSPI_OPEN_FAIL;
			}

			//Default Max Speed To
			if (ioctl(m_Fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_MaxSpeed) < 0)
			{
				m_Debug->Write("SPI Fail - Failed To Set SPI Max Speed - ");
				m_Debug->Writeln(m_MaxSpeed, DEC);
				close(m_Fd);
				return LSPI_OPEN_FAIL;
			}
		}
	}
	return L_OK;
}

int LinxSysfsSpiChannel::SetBitOrder(unsigned char bitOrder)
{
	m_BitOrder = bitOrder;
	return L_OK;
}

int LinxSysfsSpiChannel::SetMode(unsigned char mode)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	unsigned int spi_mode;
	if (ioctl(m_Fd, SPI_IOC_RD_MODE, &spi_mode) < 0)
	{
		m_Debug->Writeln("Failed To Set SPI Mode");
		return  L_UNKNOWN_ERROR;
	}

	if (mode != spi_mode)
	{
		if (ioctl(m_Fd, SPI_IOC_WR_MODE, &mode) < 0)
		{
			m_Debug->Writeln("Failed To Set SPI Mode");
			m_Debug->Writeln(mode, BIN);
			return  L_UNKNOWN_ERROR;
		}
	}
	return L_OK;
}

int LinxSysfsSpiChannel::SetSpeed(unsigned int speed, unsigned int* actualSpeed)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	if (m_NumSpiSpeeds)
	{
		int index;
		//Loop Over All Supported SPI Speeds
		for (index = 0; index < m_NumSpiSpeeds; index++)
		{
			if (speed < *(m_SpiSupportedSpeeds + index))
			{
				//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
				break;
			}
		}
		if (index != 0)
			index--;
		*actualSpeed = *(m_SpiSupportedSpeeds + index);
	}
	else
	{
		*actualSpeed = speed;
	}
	m_CurrentSpeed = *actualSpeed;
	return L_OK;
}

int LinxSysfsSpiChannel::WriteRead(unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	unsigned char csInv = ~csLL & 0x01, // Precompute inverse digital value
		          nextByte = 0;			// First Byte Of Next SPI Frame

	//SPI hardware only supports MSB First transfer. If  Configured for LSB First reverse bits in software
	if (m_BitOrder == LSBFIRST)
	{
		for (int i = 0; i < frameSize * numFrames; i++)
		{
			sendBuffer[i] = m_Device->ReverseBits(sendBuffer[i]);
		}
	}

	struct spi_ioc_transfer transfer = {0};

	//Set CS as output and make sure CS starts idle
	if (csChan)
		m_Device->DigitalWrite(1, &csChan, &csInv);

	for (int i = 0; i < numFrames; i++)
	{
		//Setup Transfer
		transfer.tx_buf = (unsigned long)(sendBuffer + nextByte);
		transfer.rx_buf = (unsigned long)(recBuffer + nextByte);
		transfer.len = frameSize;
		transfer.delay_usecs = 0;
		transfer.speed_hz = m_CurrentSpeed;
		transfer.bits_per_word = 8;

		//CS Active
		if (csChan)
			m_Device->DigitalWrite(1, &csChan, &csLL);

		//Transfer Data
		int retVal = ioctl(m_Fd, SPI_IOC_MESSAGE(1), &transfer);

		//CS Idle
		if (csChan)
			m_Device->DigitalWrite(1, &csChan, &csInv);

		if (retVal < 0)
		{
			m_Debug->Writeln("SPI Fail - Failed To Transfer Data");
			return  LSPI_TRANSFER_FAIL;
		}
		nextByte += frameSize;
	}
	return L_OK;
}

int LinxSysfsSpiChannel::Close()
{
	// Close SPI handle
	if ((m_Fd >= 0) && (close(m_Fd) < 0))
		return LSPI_CLOSE_FAIL;
	m_Fd = -1;
	return L_OK;
}

/****************************************************************************************
**  Constructors / Destructors
****************************************************************************************/
LinxLinuxDevice::LinxLinuxDevice()
{
	UartMaxBaud = g_UartSupportedSpeeds[NUM_UART_SPEEDS - 1];
}

LinxLinuxDevice::~LinxLinuxDevice()
{
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/

//------------------------------------- Analog -------------------------------------
int LinxLinuxDevice::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::AnalogSetRef(unsigned char mode, unsigned int voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//------------------------------------- PWM -------------------------------------
int LinxLinuxDevice::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxLinuxDevice::UartOpen(const char *channelName, unsigned char nameLength, unsigned char *channel, LinxUartChannel **chan)
{
	LinxUartChannel *temp = NULL;
	unsigned char numChannels = EnumerateChannels(IID_LinxUartChannel, NULL, 0);
	if (numChannels)
	{
		unsigned char *channels = (unsigned char*)malloc(numChannels);
		if (channels)
		{
			char buffer[32];
			numChannels = EnumerateChannels(IID_LinxUartChannel, channels, numChannels);
			for (int i = 0; i < numChannels; i++)
			{
				temp = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channels[i]);
				if (temp)
				{
					temp->GetName(buffer, 32);
					if (!strncmp(channelName, buffer, nameLength))
					{
						*channel = channels[i];
						if (chan)
							*chan = temp;
						else
							temp->Release();
						free(channels);
						return L_OK;
					}
					temp->Release();
				}
			}
			free(channels);
		}
	}
	temp = new LinxUnixUartChannel(channelName, m_Debug);
	*channel = RegisterChannel(IID_LinxUartChannel, temp);
	if (chan)
		*chan = temp;
	return LERR_BADPARAM;
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
int LinxLinuxDevice::pwmSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for (int i = 0; i < numChans; i++)
	{
		//Open Period Handle If It Is Not Already
		if (PwmPeriodHandles[channels[i]] == NULL)
		{
			char periodPath[64];
			sprintf(periodPath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmPeriodFileName.c_str());
			m_Debug->Write("Opening ");
			m_Debug->Writeln(periodPath);
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
			m_Debug->Write("Opening ");
			m_Debug->Writeln(dutyCyclePath);
			PwmDutyCycleHandles[channels[i]] = fopen(dutyCyclePath, "r+w+");
		}
	}
	return L_OK;
}

bool LinxLinuxDevice::uartSupportsVarBaudrate(const char* path, int baudrate)
{
	return false;
}

