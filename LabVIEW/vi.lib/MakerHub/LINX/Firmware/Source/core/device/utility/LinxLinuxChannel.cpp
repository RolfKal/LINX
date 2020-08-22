/****************************************************************************************
**  LINX - channel implementation for Lux based systems
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
#define NUM_UART_SPEEDS 18

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <fcntl.h>
#include <algorithm>
#include "LinxDefines.h"
#if Unix
#include <alloca.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <sys/socket.h>
#include <netdb.h>
#include <asm/termbits.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#elif Win32
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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

#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8

/*
 * Let any architecture override either of the following before
 * including this file.
 */

#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_NRMASK	((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK	((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK	((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK	((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC_NONE	0U
#define _IOC_WRITE	1U
#define _IOC_READ	2U

#define _IOC(dir,type,nr,size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))

//#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0)
//#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
//#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
//#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

//#define FIONREAD	_IOR('f', 127, int)
#define TCGETS		_IOR('t', 19, struct termios)
#define TCSETS		_IOW('t', 20, struct termios)
#define TCSBRK		_IO('t', 29)
#define TCXONC		_IO('t', 30)
#define TCFLSH		_IO('t', 31)
#define TCGETS2		_IOR('T', 42, struct termios2)
#define TCSETS2		_IOW('T', 43, struct termios2)

int ioctl(int, unsigned int, ...);

//#define POLLIN 0

//struct pollfd
//{
//    int  fd;
//    short   events;
//   short   revents;
//};

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

struct i2c_msg
{
	unsigned short addr;
	unsigned short flags;
	unsigned short len;
	unsigned char *buf;
};

struct i2c_rdwr_ioctl_data
{
	struct i2c_msg *msgs;
	unsigned int nmsgs;
};

#define I2C_RDRW_IOCTL_MAX_MSGS	42

#define I2C_TIMEOUT			0x702		// type uLong, Sets the transfer timeout in 10ms units
#define I2C_SLAVE			0x703		// type uLong, 
#define I2C_FUNCS			0x705		// type uLong*, 
#define I2C_RDWR			0x707		// struct i2c_rdwr_ioctl_data*

#define I2C_M_RD			0x0001
#define I2C_M_TEN			0x0010		// only if I2C_FUNC return value contains I2C_FUNC_10BIT_ADDR
#define I2C_M_RECV_LEN		0x0400
#define I2C_M_NO_RD_ACK		0x0800		// only if I2C_FUNC return value contains I2C_FUNC_PROTOCOL_MANGLING
#define I2C_M_IGNORE_NAK	0x1000		// only if I2C_FUNC return value contains I2C_FUNC_PROTOCOL_MANGLING
#define I2C_M_REV_DIR_ADDR	0x2000		// only if I2C_FUNC return value contains I2C_FUNC_PROTOCOL_MANGLING
#define I2C_M_NOSTART		0x4000		// only if I2C_FUNC return value contains I2C_FUNC_NOSTART
#define I2C_M_STOP			0x8000		// only if I2C_FUNC return value contains I2C_FUNC_PROTOCOL_MANGLING

#define I2C_FUNC_I2C				0x00000001
#define I2C_FUNC_10BIT_ADDR			0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING	0x00000004
#define I2C_FUNC_SMBUS_PEC			0x00000008
#define I2C_FUNC_NOSTART			0x00000010

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
#include "LinxLinuxChannel.h"

/****************************************************************************************
**  Defines
****************************************************************************************/
#define GPIO_EXPORTED	0x80

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

int LinxSysfsAiChannel::Read(unsigned int *value)
{
	int status = SmartOpen();
	if (status)
		return status;

	m_ValHandle = freopen(m_ChannelName, "r+", m_ValHandle);
	fscanf(m_ValHandle, "%u", value);
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

	if (m_DirHandle != NULL)
	{
		// return to input
		fprintf(m_DirHandle, "in");
		fclose(m_DirHandle);
	}

	if (m_ValHandle != NULL)
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
LinxSysfsPwmChannel::LinxSysfsPwmChannel(LinxFmtChannel *debug, const char *deviceName, const char *enableName, const char *periodName, const char *dutyCycleName, unsigned int defaultPeriod) : LinxPwmChannel(deviceName, debug)
{
	m_PeriodHandle = NULL;
	m_DutyCycleHandle = NULL;
	m_EnableFileName = enableName;
	m_PeriodFileName = periodName;
	m_DutyCycleFileName = dutyCycleName;
	m_DefaultPeriod = defaultPeriod;
}

LinxSysfsPwmChannel::~LinxSysfsPwmChannel()
{
	if (m_DutyCycleHandle != NULL)
	{
		fclose(m_DutyCycleHandle);
		m_DutyCycleHandle = NULL;
	}
	if (m_PeriodHandle != NULL)
	{
		char enablePath[64];
		sprintf(enablePath, "%s%s", m_ChannelName, m_EnableFileName);
		FILE* pwmEnableHandle = fopen(enablePath, "r+w+");
		if (pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "0");
			fclose(pwmEnableHandle);
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmEnableHandle");
		}
		fclose(m_PeriodHandle);
		m_PeriodHandle = NULL;
	}
}

int LinxSysfsPwmChannel::SmartOpen()
{
	char tempPath[64];
	//Open Period Handle If It Is Not Already
	if (m_PeriodHandle == NULL)
	{
		//Turn On PWM		
		sprintf(tempPath, "%s%s", m_ChannelName, m_EnableFileName);			
		FILE* pwmEnableHandle = fopen(tempPath, "r+w+");
		if (pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "1");
			fclose(pwmEnableHandle);		
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmEnableHandle");				
		}	

		sprintf(tempPath, "%s%s", m_ChannelName, m_PeriodFileName);
		m_Debug->Write("Opening ");
		m_Debug->Writeln(tempPath);
		m_PeriodHandle = fopen(tempPath, "r+w+");

		//Initialize PWM Period
		fprintf(m_PeriodHandle, "%u", m_DefaultPeriod);
		m_Period = m_DefaultPeriod;
		fflush(m_PeriodHandle);
	}

	//Open Duty Cycle Handle If It Is Not Already
	if (m_DutyCycleHandle == NULL)
	{
		sprintf(tempPath, "%s%s", m_ChannelName, m_DutyCycleFileName);
		m_Debug->Write("Opening ");
		m_Debug->Writeln(tempPath);
		m_DutyCycleHandle = fopen(tempPath, "r+w+");
		fprintf(m_DutyCycleHandle, "0");
	}
	return L_OK;
}

int LinxSysfsPwmChannel::SetDutyCycle(unsigned char value)
{
	//unsigned int period = 500000;		//Period Defaults To 500,000 nS. To Do Update This When Support For Changing Period / Frequency Is Added
	unsigned int dutyCycle = 0;

	if (value == 0)
	{
		dutyCycle = 0;
	}
	else if (value == 255)
	{
		dutyCycle = m_Period;
	}
	else
	{
		dutyCycle = (unsigned int)(m_Period * (value / 255.0));
	}

	//Update Output
	m_Debug->Write("Setting Duty Cycle = ");
	m_Debug->Write(dutyCycle, DEC);
	fprintf(m_DutyCycleHandle, "%u", dutyCycle);
	m_Debug->Write(" ... Duty Cycle Set ... ");
	fflush(m_DutyCycleHandle);
	m_Debug->Writeln("Flushing.");
	return L_OK;
}



//------------------------------------- UART -------------------------------------
LinxUnixSocketChannel::LinxUnixSocketChannel(LinxFmtChannel *debug, const char *deviceName) : LinxCommChannel(deviceName, debug)
{
	m_Fd = -1;
}

LinxUnixSocketChannel::LinxUnixSocketChannel(LinxFmtChannel *debug, const char *deviceName, int fd) : LinxCommChannel(deviceName, debug)
{
	m_Fd = fd;
}

LinxUnixSocketChannel::LinxUnixSocketChannel(LinxFmtChannel *debug, const char *address, unsigned short port) : LinxCommChannel(address, debug)
{
    struct addrinfo hints, *result, *rp;
	char str[10];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_NUMERICSERV;

	if (!getaddrinfo(address, itoa(port, str, 10), &hints, &result))
	{
		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			NetSocket m_Fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (m_Fd < 0)
				continue;

            if (connect(m_Fd, rp->ai_addr, (socklen_t)rp->ai_addrlen) != -1)
				break;

			closesocket(m_Fd);
		}
		freeaddrinfo(result);
		if (rp == NULL)
		{       
			m_Debug->Write("Could not connect to TCP/IP address: ");
			m_Debug->Writeln(address);
		}
	}
}

LinxUnixSocketChannel::~LinxUnixSocketChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
}

int LinxUnixSocketChannel::SmartOpen()
{
	if (m_Fd < 0)
	{
		struct termios2 options;

		// Open device as read/write, no CTRL-C handling and non-blocking
		m_Fd = open(m_ChannelName, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (m_Fd < 0)
		{
			m_Debug->Write("Unix Socket Fail - Failed to open file handle - ");
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

int LinxUnixSocketChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	int status = SmartOpen();
	if (status)
		return status;

	*numBytesRead = 0;

	if (recBuffer && numBytes)
	{
		struct pollfd fds[1];
		int retval, offset = 0;
		unsigned long long start = getMsTicks();

		fds[0].fd = m_Fd;
		fds[0].events = POLLIN ;

		while (*numBytesRead < numBytes)
		{
			retval = poll(fds, 1, timeout < 0 ? -1 : min(timeout - (int)(getMsTicks() - start), 0));
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

int LinxUnixSocketChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
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

int LinxUnixSocketChannel::Close()
{
	if (m_Fd >= 0)
		close(m_Fd);
	m_Fd = -1;
	return L_OK;
}

//------------------------------------- UART -------------------------------------
static unsigned int g_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
static unsigned int g_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

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

//------------------------------------- I2C -------------------------------------
LinxSysfsI2cChannel::LinxSysfsI2cChannel(const char *channelName, LinxFmtChannel *debug) : LinxI2cChannel(channelName, debug)
{
	m_Fd = -1;
	m_Funcs = 0;
}

LinxSysfsI2cChannel::~LinxSysfsI2cChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
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
		ioctl(m_Fd, I2C_FUNCS, &m_Funcs); 
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

	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg[1];

	work_queue.nmsgs = 1;
	work_queue.msgs = msg;

	msg[0].addr = slaveAddress;
	msg[0].len = numBytes;
	msg[0].flags = 0;
	msg[0].buf = sendBuffer;

	if (ioctl(m_Fd, I2C_RDWR, &work_queue) < 0)
		return LI2C_SADDR;

	return L_OK;
}

int LinxSysfsI2cChannel::Read(unsigned char slaveAddress, unsigned char eofConfig, int numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg[1];

	//Check EOF - Currently Only Support 0x00
	if (eofConfig != EOF_STOP)
	{
		return LI2C_EOF;
	}

	work_queue.nmsgs = 1;
	work_queue.msgs = msg;

	msg[0].addr = slaveAddress;
	msg[0].len = numBytes;
	msg[0].flags = I2C_M_RD;
	msg[0].buf = recBuffer;

	if (ioctl(m_Fd, I2C_RDWR, &work_queue) < 0)
		return LI2C_SADDR;

	return L_OK;
}

int LinxSysfsI2cChannel::Transfer(unsigned char slaveAddress, int numFrames, int *flags, int *numBytes, unsigned int timeout, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	if (numFrames > I2C_RDRW_IOCTL_MAX_MSGS)
		return LERR_BADPARAM;

	if (!(m_Funcs & I2C_FUNC_I2C))
		return L_FUNCTION_NOT_SUPPORTED;

	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg[I2C_RDRW_IOCTL_MAX_MSGS];
	int sendOffset = 0, recvOffset = 0;

	for (int i = 0; i < numFrames; i++)
	{
		msg[i].addr = slaveAddress;
		msg[i].len = numBytes[i];
		msg[i].flags = flags[i];
		if (flags[i] & I2C_M_RD)
		{
			msg[i].buf = recBuffer + recvOffset;
			recvOffset += numBytes[i];
		}
		else
		{
			msg[i].buf = sendBuffer + sendOffset;
			sendOffset += numBytes[i];
		}
	}
	work_queue.nmsgs = numFrames;
	work_queue.msgs = msg;

	if (ioctl(m_Fd, I2C_RDWR, &work_queue) < 0)
		return LI2C_SADDR;

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
LinxSysfsSpiChannel::LinxSysfsSpiChannel(const char *channelName, LinxFmtChannel *debug, LinxDevice *device, unsigned int maxSpeed) : LinxSpiChannel(channelName, debug)
{
	m_Device = device;
	m_CurrentSpeed = maxSpeed;
	m_MaxSpeed = maxSpeed;
	m_Fd = -1;
}

LinxSysfsSpiChannel::~LinxSysfsSpiChannel()
{
	if (m_Fd >= 0)
		close(m_Fd);
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
		ReverseBits(sendBuffer, frameSize);

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
