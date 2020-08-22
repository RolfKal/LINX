/****************************************************************************************
**  LINX header for Linx Support.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_DEFINES_H
#define LINX_DEFINES_H

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define Win32	1
#define NetSocket SOCKET
#elif ARDUINO_VERSION
#define Arduino 1
#if ARDUINO_VERSION >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define Unix	1
#define NetSocket int
#define INVALID_SOCKET -1
#define closesocket(fd) close(fd)
#elif defined(__APPLE__) && defined(__MACH__)
#define MacOSX	1
#elif defined(__WXWORKS__) || defined(__vxworks__)
#define VxWorks 1
#endif

// GPIO Values
#define GPIO_LOW		0
#define GPIO_HIGH		1

#define GPIO_INPUT		0x00
#define GPIO_OUTPUT		0x01
#define GPIO_ALT0		0x04
#define GPIO_IOMASK		0x07
#define GPIO_DIRMASK	0x07

#define GPIO_PULLNONE	0x00
#define GPIO_PULLDOWN	0x10
#define GPIO_PULLUP		0x20
#define GPIO_PULLOFF	0x30
#define GPIO_PULLMASK	0x30

// SPI
#ifndef LSBFIRST
	#define LSBFIRST 0
#endif

#ifndef MSBFIRST
	#define MSBFIRST 1
#endif

//I2C
#define EOF_STOP 0
#define EOF_RESTART 1
#define EOF_RESTART_NOSTOP 2
#define EOF_NOSTOP 3

//UART
#ifndef BIN
	#define BIN 2
#endif
#ifndef OCT
	#define OCT 8
#endif
#ifndef DEC
	#define DEC 10
#endif
#ifndef HEX
	#define HEX 16
#endif

#define TIMEOUT_INFINITE -1

//Non-Volatile Storage Addresses
#define NVS_USERID 0x00
#define NVS_ETHERNET_IP 0x02
#define NVS_ETHERNET_PORT 0x06
#define NVS_WIFI_IP 0x08
#define NVS_WIFI_PORT 0x0C
#define NVS_WIFI_SSID_SIZE 0x0E
#define NVS_WIFI_SSID 0x0F
#define NVS_WIFI_SECURITY_TYPE 0x30
#define NVS_WIFI_PW_SIZE 0x31
#define NVS_WIFI_PW 0x32
#define NVS_SERIAL_INTERFACE_MAX_BAUD 0x72

#define UNDEFINED_CHANNEL 0xFF

/****************************************************************************************
**  Typedefs
****************************************************************************************/
typedef enum LinxStatus
{
	L_OK = 0,
	L_FUNCTION_NOT_SUPPORTED,
	L_REQUEST_RESEND,
	L_UNKNOWN_ERROR,
	L_DISCONNECT,
	LERR_RUNNING,
	LERR_MEMORY,
	LERR_BADPARAM,
	LERR_BADCHAN,
	LERR_IO,
	LERR_PACKET_NUM,
	LERR_CHECKSUM,
	LERR_INVALID_FRAME,
	LERR_LENGTH_NOT_SUPPORTED,
	LERR_MSG_TO_LONG,
	LERR_CLOSED_BY_PEER,
} LinxStatus;

typedef enum AioStatus
{
	LANALOG_REF_MODE_ERROR = 129,
	LANALOG_REF_VAL_ERROR = 130
} AioStatus;

typedef enum DioStatus
{
	LDIGITAL_PIN_DNE = 128,
} DioStatus;


typedef enum SPIStatus
{
	LSPI_OPEN_FAIL = 128,
	LSPI_TRANSFER_FAIL,
	LSPI_CLOSE_FAIL,
	LSPI_DEVICE_NOT_OPEN,
} SPIStatus;

typedef enum I2CStatus
{
	LI2C_SADDR = 128,
	LI2C_EOF,
	LI2C_WRITE_FAIL,
	LI2C_READ_FAIL,
	LI2C_CLOSE_FAIL,
	LI2C_OPEN_FAIL,
	LI2C_DEVICE_NOT_OPEN,
} I2CStatus;

typedef enum UartStatus
{
	LUART_OPEN_FAIL = 128,
	LUART_SET_BAUD_FAIL,
	LUART_AVAILABLE_FAIL,
	LUART_READ_FAIL,
	LUART_WRITE_FAIL,
	LUART_CLOSE_FAIL,
	LUART_TIMEOUT
} UartStatus;

typedef enum LinxUartParity
{
	Ignore,
	None,
	Odd,
	Even,
	Mark,
	Space,
} LinxUartParity;

#define Min(a, b) (((a) < (b)) ? (a) : (b) 
#define Max(a, b) (((a) > (b)) ? (a) : (b) 

#endif