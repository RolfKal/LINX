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
#include "LinxDefines.h"
#include <string.h>
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
#include "LinxUtilities.h"
#include "LinxLinuxChannel.h"

/****************************************************************************************
**  Defines
****************************************************************************************/

/************************************ Analog Input SysFS Channel ***********************/
/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxSysfsAiChannel::LinxSysfsAiChannel(LinxFmtChannel *debug, const char *channelName, uint8_t resolution) : LinxAiChannel(debug, channelName, resolution)
{
	m_ValHandle = NULL;
}

LinxSysfsAiChannel::~LinxSysfsAiChannel(void)
{
	if (m_ValHandle)
	{
		fclose(m_ValHandle);
	}
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
// Open direction and value handles if it is not already open
int32_t LinxSysfsAiChannel::SmartOpen(void)
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

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int32_t LinxSysfsAiChannel::Read(uint32_t *value)
{
	int32_t status = SmartOpen();
	if (status)
		return status;

	m_ValHandle = freopen(m_ChannelName, "r+", m_ValHandle);
	fscanf(m_ValHandle, "%u", value);
	return L_OK;
}

/************************************ Digtal Input SysFS Channel ***********************/
/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxSysfsDioChannel::LinxSysfsDioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin) : LinxDioChannel(debug, linxPin, gpioPin)
{
	m_ValHandle = NULL;
	m_DirHandle = NULL;
	m_EdgeHandle = NULL;
}

LinxSysfsDioChannel::~LinxSysfsDioChannel(void)
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

		FILE* digitalExportHandle = fopen("/sys/class/gpio/unexport", "r+w+");
		if (digitalExportHandle != NULL)
		{
			fprintf(digitalExportHandle, "%d", m_GpioChan);
			fclose(digitalExportHandle);
		}
	}
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
// Open direction and value handles if it is not already open
int32_t LinxSysfsDioChannel::SmartOpen(void)
{
	char gpioPath[32];

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
			}
			else
			{
				m_Debug->Writeln("Digital Fail - Unable To Open Direction File Handle");
				return LDIGITAL_PIN_NOT_AVAIL;
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
			return LDIGITAL_PIN_NOT_AVAIL;
		}
	}
	return L_OK;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int32_t LinxSysfsDioChannel::setDirection(uint8_t dir)
{
	int32_t status = SmartOpen();
	if (!status)
	{
		status = LinxDIOChannel::setDirection(dir);
		if (!status)
		{
			//Set as input or output
			fprintf(m_DirHandle, dir & GPIO_DIRMASK ? "out" : "in");
			fflush(m_DirHandle);
		}
		else if (status == LDIGITAL_PIN_NOCHANGE)
		{
			return L_OK;
		}
	}
	return status;
}

int32_t LinxSysfsDioChannel::setPull(uint8_t pud)
{
	int32_t status = SmartOpen();
	if (!status)
	{
		status = LinxDIOChannel::setPull(pud);
		if (!status)
		{
			return L_FUNCTION_NOT_SUPPORTED;
		}
		else if (status == LDIGITAL_PIN_NOCHANGE)
		{
			return L_OK;
		}
	}
	return status;
}

int32_t LinxSysfsDioChannel::setValue(uint8_t value)
{
	int32_t status = SmartOpen();
	if (!status)
	{
		// Set value
		status = fprintf(m_ValHandle, value ? "1" : "0");
		fflush(m_ValHandle);
		return status == 1 ? L_OK : LDIGITAL_PIN_NOT_AVAIL;
	}
	return status;
}

int32_t LinxSysfsDioChannel::getValue(uint8_t *value)
{
	int32_t status = SmartOpen();
	if (!status)
	{
		char gpioPath[32];

		//Reopen value handle
		sprintf(gpioPath, "/sys/class/gpio/gpio%d/value", m_GpioChan);
		m_ValHandle = freopen(gpioPath, "r+w+", m_ValHandle);

		//Read from next pin
		status = fscanf(m_ValHandle, "%hhu", value);
		return status == 1 ? L_OK : LDIGITAL_PIN_NOT_AVAIL;
	}
	return status;
}

/************************************ Digtal Input GPIO Channel ************************/
/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxGPIODioChannel::LinxGPIODioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin) : LinxDioChannel(debug, linxPin, gpioPin)
{
}

LinxGPIODioChannel::~LinxGPIODioChannel(void)
{
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
// Open direction and value handles if it is not already open
int32_t LinxGPIODioChannel::SmartOpen(void)
{
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int32_t LinxGPIODioChannel::setDirection(uint8_t dir)
{
}

int32_t LinxGPIODioChannel::setPull(uint8_t pud)
{
}

int32_t LinxGPIODioChannel::setValue(uint8_t value)
{
}

int32_t LinxGPIODioChannel::getValue(uint8_t *value)
{
}

//------------------------------------- UART -------------------------------------
LinxSysfsPwmChannel::LinxSysfsPwmChannel(LinxFmtChannel *debug, const char *deviceName, const char *enableName, const char *periodName, const char *dutyCycleName, uint32_t defaultPeriod) : LinxPwmChannel(debug, deviceName)
{
	m_PeriodHandle = NULL;
	m_DutyCycleHandle = NULL;
	m_EnableFileName = enableName;
	m_PeriodFileName = periodName;
	m_DutyCycleFileName = dutyCycleName;
	m_DefaultPeriod = defaultPeriod;
}

LinxSysfsPwmChannel::~LinxSysfsPwmChannel(void)
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

int32_t LinxSysfsPwmChannel::SmartOpen(void)
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

int32_t LinxSysfsPwmChannel::SetDutyCycle(uint8_t value)
{
	//uint32_t period = 500000;		//Period Defaults To 500,000 nS. To Do Update This When Support For Changing Period / Frequency Is Added
	uint32_t dutyCycle = 0;

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
		dutyCycle = (uint32_t)(m_Period * (value / 255.0));
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


//------------------------------------- Unix Comm -------------------------------------
LinxUnixCommChannel::LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *channelName, OSSocket socket) : LinxCommChannel(debug, channelName)
{
	m_Socket = socket;
}

LinxUnixCommChannel::LinxUnixCommChannel(LinxFmtChannel *debug, const unsigned char *address, uint16_t port) : LinxCommChannel(debug, address)
{
    struct addrinfo hints, *result, *rp;
	char servname[10];
	int32_t retval;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICSERV;

	sprintf(servname, "%hu", port);
	if (!getaddrinfo(address,  servname, &hints, &result))
	{
		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			m_Socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (!IsANetObject(m_Socket))
				continue;

			switch (rp->ai_addr->sa_family)
			{
				case AF_INET:
					((sockaddr_in*)rp->ai_addr)->sin_port = port;
					break;
				case AF_INET6:
					((sockaddr_in6*)rp->ai_addr)->sin6_port = port;
					break;
				default:
					continue;
			}
            retval = connect(m_Socket, rp->ai_addr, (socklen_t)rp->ai_addrlen);
            if (retval != -1)
			{
				u_long nonBlocking = 1;
				retval = ioctl(m_Socket, FIONBIO, &nonBlocking);
				if (retval >= 0)
					break;
			}
			close(m_Socket);
			m_Socket = kInvalNetObject;
		}
		freeaddrinfo(result);
		if (rp == NULL)
		{       
			m_Debug->Write("Could not connect to TCP/IP address: ");
			m_Debug->Writeln(address);
		}
	}
}

LinxUnixCommChannel::~LinxUnixCommChannel(void)
{
	if (m_Socket >= 0)
		close(m_Socket);
}

int32_t LinxUnixCommChannel::Read(unsigned char* recBuffer, int32_t numBytes, uint32_t start, int32_t timeout, int32_t* numBytesRead)
{
	*numBytesRead = 0;

	if (recBuffer && numBytes)
	{
		struct pollfd fds[1];
		int32_t retval, offset = 0;

		fds[0].fd = m_Socket;
		fds[0].events = POLLIN ;

		while (*numBytesRead < numBytes)
		{
			retval = poll(fds, 1, timeout < 0 ? -1 : Min(timeout - (getMsTicks() - start), 0));
			if (!retval)
			{
				return LUART_TIMEOUT;
			}
			else if (retval < 0)
			{
				return LUART_READ_FAIL;
			}
			// Read bytes from input buffer
			retval = read(m_Socket, recBuffer + offset, numBytes - offset);
			if (!retval)
			{
				return L_DISCONNECT;
			}
			else
			{
				return LUART_READ_FAIL;
			}
			*numBytesRead += retval;
		}
	}
	else
	{
		// Check how many bytes are available
		if (ioctl(m_Socket, FIONREAD, numBytesRead) < 0)
		{
			if (errno == EBADF)
			{
				return L_DISCONNECT;
			}
			return LUART_READ_FAIL;
		}
	}
	return L_OK;
}

int32_t LinxUnixCommChannel::Write(const unsigned char* sendBuffer, int32_t numBytes, uint32_t start, nt timeout)
{
	int32_t bytesSent = write(m_Socket, sendBuffer, numBytes);
	if (bytesSent != numBytes)
	{
		if (bytesSent < 0 && errno == EBADF)
		{
			return L_DISCONNECT;
		}
		return LUART_WRITE_FAIL;
	}
	return L_OK;
}

int32_t LinxUnixCommChannel::Close(void)
{
	if (m_Socket >= 0)
		close(m_Socket);
	m_Socket = kInvalNetObject;
	return L_OK;
}

//------------------------------------- UART -------------------------------------
LinxUnixUartChannel::LinxUnixUartChannel(LinxFmtChannel *debug, const char *deviceName) : LinxUartChannel(debug, deviceName)
{
	struct termios2 options;

	// Open device as read/write, no CTRL-C handling and non-blocking
	m_Socket = open(m_ChannelName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_Socket < 0)
	{
		m_Debug->Write("Unix Socket Fail - Failed to open file handle - ");
		m_Debug->Writeln(m_ChannelName);
		return  LUART_OPEN_FAIL;
	}
	if (ioctl(m_Socket, TCGETS, &options) < 0)
		return LERR_IO;

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Use raw input mode
	options.c_oflag &= ~OPOST;							// Use raw output mode

	if (ioctl(m_Socket, TCSETS, &options) < 0)
		return LERR_IO;

	return L_OK;
}

static uint32_t g_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
static uint32_t g_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

int32_t LinxUnixUartChannel::SetSpeed(uint32_t baudRate, uint32_t* actualBaud)
{
	struct termios2 options;
	int32_t ioctlval = TCSETS2;
	int32_t status = SmartOpen();
	if (status)
		return status;

	// If driver supports newer TCGETS2 ioctl call, assume that we can set arbitrary baudrates
	// Actual used baudrate may still be different due to discrete clock generation
	if (ioctl(m_Socket, TCGETS2, &options) < 0)
	{
		int32_t temp;

		if (ioctl(m_Socket, TCGETS, &options) < 0)
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
		if (actualBaud)
			*actualBaud = (uint32_t) *(g_UartSupportedSpeeds + temp);
		options.c_cflag = (options.c_cflag & ~(CBAUD | CIBAUD)) | g_UartSupportedSpeedsCodes[temp] | g_UartSupportedSpeedsCodes[temp] << IBSHIFT;
		ioctlval = TCSETS;
	}
	else
	{
		if (actualBaud)
			*actualBaud = baudRate;
		options.c_ispeed = baudRate;
		options.c_ospeed = baudRate;
		options.c_cflag = ((options.c_cflag & ~(CBAUD | CIBAUD)) | BOTHER | BOTHER << IBSHIFT);
	}

	//Set Baud Rate
	ioctl(m_Socket, TCFLSH, TCIFLUSH);
	if (ioctl(m_Socket, ioctlval, &options) < 0)
		return LERR_IO;
	return  L_OK;
}

#define BIT_SIZE_OFFSET	5
#define NUM_BIT_SIZES	4
#define NUM_PARITY_SIZES	5

static const int32_t BitSizes[NUM_BIT_SIZES] = {CS5, CS6, CS7, CS8};
static const int32_t Parity[NUM_PARITY_SIZES] = {0, PARENB, PARENB | PARODD, PARENB | PARODD | CMSPAR, PARENB | CMSPAR};

int32_t LinxUnixUartChannel::SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity)
{
	int32_t status = SmartOpen();
	if (status)
		return status;

	if (dataBits > 0 && (dataBits < BIT_SIZE_OFFSET || dataBits >= BIT_SIZE_OFFSET + NUM_BIT_SIZES))
		return LERR_BADPARAM;

	if (stopBits > 2)
		return LERR_BADPARAM;

	if (!dataBits && !stopBits)
		return L_OK;

	struct termios options;
	if (ioctl(m_Socket, TCGETS, &options) >= 0)
	{
		if (dataBits)
			options.c_cflag = (options.c_cflag & ~CSIZE) | BitSizes[dataBits - BIT_SIZE_OFFSET];
		if (stopBits == 1)
			options.c_cflag &= ~CSTOPB;
		else if (stopBits == 2)
			options.c_cflag |= CSTOPB;

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
		if (ioctl(m_Socket, TCSETS, &options) >= 0)
			return  L_OK;

		if (ioctl(m_Socket, TCSETS, &options) >= 0)
			return  L_OK;
	}
	return LUART_SET_PARAM_FAIL;
}

//------------------------------------- I2C -------------------------------------
LinxSysfsI2cChannel::LinxSysfsI2cChannel(LinxFmtChannel *debug, const char *channelName) : LinxI2cChannel(debug, channelName)
{
	m_Fd = -1;
	m_Funcs = 0;
}

LinxSysfsI2cChannel::~LinxSysfsI2cChannel(void)
{
	if (m_Fd >= 0)
		close(m_Fd);
}

int32_t LinxSysfsI2cChannel::Open(void)
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

int32_t LinxSysfsI2cChannel::SetSpeed(uint32_t speed, uint32_t* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxSysfsI2cChannel::Write(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, unsigned char* sendBuffer)
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

int32_t LinxSysfsI2cChannel::Read(uint8_t slaveAddress, uint8_t eofConfig, int32_t numBytes, uint32_t timeout, unsigned char* recBuffer)
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

int32_t LinxSysfsI2cChannel::Transfer(uint8_t slaveAddress, int32_t numFrames, int32_t *flags, int32_t *numBytes, uint32_t timeout, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LI2C_DEVICE_NOT_OPEN;

	if (numFrames > I2C_RDRW_IOCTL_MAX_MSGS)
		return LERR_BADPARAM;

	if (!(m_Funcs & I2C_FUNC_I2C))
		return L_FUNCTION_NOT_SUPPORTED;

	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg[I2C_RDRW_IOCTL_MAX_MSGS];
	int32_t sendOffset = 0, recvOffset = 0;

	for (int32_t i = 0; i < numFrames; i++)
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

int32_t LinxSysfsI2cChannel::Close(void)
{
	if ((m_Fd >= 0) && (close(m_Fd) < 0))
		return LI2C_CLOSE_FAIL;
	m_Fd = -1;
	return L_OK;
}

//------------------------------------- SPI -------------------------------------
LinxSysfsSpiChannel::LinxSysfsSpiChannel(LinxFmtChannel *debug, LinxDevice *device, const char *channelName, uint32_t maxSpeed) : LinxSpiChannel(debug, channelName)
{
	m_Device = device;
	m_CurrentSpeed = maxSpeed;
	m_MaxSpeed = maxSpeed;
	m_Fd = -1;
}

LinxSysfsSpiChannel::~LinxSysfsSpiChannel(void)
{
	if (m_Fd >= 0)
		close(m_Fd);
}

int32_t LinxSysfsSpiChannel::Open(void)
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
			uint8_t spi_mode = SPI_MODE_0 | SPI_NO_CS;
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

int32_t LinxSysfsSpiChannel::SetBitOrder(uint8_t bitOrder)
{
	m_BitOrder = bitOrder;
	return L_OK;
}

int32_t LinxSysfsSpiChannel::SetMode(uint8_t mode)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	uint32_t spi_mode;
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

int32_t LinxSysfsSpiChannel::SetSpeed(uint32_t speed, uint32_t* actualSpeed)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	if (m_NumSpiSpeeds)
	{
		int32_t index;
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

int32_t LinxSysfsSpiChannel::WriteRead(uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	if (m_Fd < 0)
		return LSPI_DEVICE_NOT_OPEN;

	uint8_t csInv = ~csLL & 0x01, // Precompute inverse digital value
		          nextByte = 0;			// First Byte Of Next SPI Frame

	//SPI hardware only supports MSB First transfer. If  Configured for LSB First reverse bits in software
	if (m_BitOrder == LSBFIRST)
		ReverseBits(sendBuffer, frameSize);

	struct spi_ioc_transfer transfer = {0};

	//Set CS as output and make sure CS starts idle
	if (csChan)
		m_Device->DigitalWrite(1, &csChan, &csInv);

	for (int32_t i = 0; i < numFrames; i++)
	{
		//Setup Transfer
		transfer.tx_buf = (uint32_t)(sendBuffer + nextByte);
		transfer.rx_buf = (uint32_t)(recBuffer + nextByte);
		transfer.len = frameSize;
		transfer.delay_usecs = 0;
		transfer.speed_hz = m_CurrentSpeed;
		transfer.bits_per_word = 8;

		//CS Active
		if (csChan)
			m_Device->DigitalWrite(1, &csChan, &csLL);

		//Transfer Data
		int32_t retVal = ioctl(m_Fd, SPI_IOC_MESSAGE(1), &transfer);

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

int32_t LinxSysfsSpiChannel::Close(void)
{
	// Close SPI handle
	if ((m_Fd >= 0) && (close(m_Fd) < 0))
		return LSPI_CLOSE_FAIL;
	m_Fd = -1;
	return L_OK;
}