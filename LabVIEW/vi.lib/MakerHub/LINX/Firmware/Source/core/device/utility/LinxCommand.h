/****************************************************************************************
**  LINX header for remote commands.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_COMMAND_H
#define LINX_COMMAND_H

#define LCMD_SYNC					0x0000	// Sync
//#define LCMD_FLUSH_BUF			0x0001	// Flush Linx Connection Buffer
//#define LCMD_RESET				0x0002	// Reset
#define LCMD_GET_DEV_ID				0x0003	// Get Device ID
#define LCMD_GET_API_VER			0x0004	// Get LINX API Version
#define LCMD_GET_UART_MAX_BAUD		0x0005	// Get UART Max Baud
#define LCMD_SET_UART_MAX_BAUD		0x0006	// Set UART Listener Interface Max Baud
#define LCMD_GET_MAX_PACK_SIZE		0x0007	// Get Max Packet Size
#define LCMD_GET_DIO_CHANS			0x0008	// Get DIO Channels
#define LCMD_GET_AI_CHANS			0x0009	// Get AI Channels
#define LCMD_GET_AO_CHANS			0x000A	// Get AO Channels
#define LCMD_GET_PWM_CHANS			0x000B	// Get PWM Channels
#define LCMD_GET_QE_CHANS			0x000C	// Get QE Channels
#define LCMD_GET_UART_CHANS			0x000D	// Get UART Channels
#define LCMD_GET_I2C_CHANS			0x000E	// Get I2C Channels
#define LCMD_GET_SPI_CHANS			0x000F	// Get SPI Channels
#define LCMD_GET_CAN_CHANS			0x0010	// Get CAN Channels
#define LCMD_DISCONNECT				0x0011	// Disconnect
#define LCMD_SET_DEVICE_USER_ID		0x0012	// Set Device User Id
#define LCMD_GET_DEVICE_USER_ID		0x0013	// Get Device User Id
#define LCMD_SET_ETH_ADDR			0x0014	// Set Device Ethernet IP
#define LCMD_GET_ETH_ADDR			0x0015	// Get Device Ethernet IP
#define LCMD_SET_ETH_PORT			0x0016	// Set Device Ethernet Port
#define LCMD_GET_ETH_PORT			0x0017	// Get Device Ethernet Port
#define LCMD_SET_WIFI_ADDR			0x0018	// Set Device WIFI IP
#define LCMD_GET_WIFI_ADDR			0x0019	// Get Device WIFI IP
#define LCMD_SET_WIFI_PORT			0x001A	// Set Device WIFI Port
#define LCMD_GET_WIFI_PORT			0x001B	// Get Device WIFI Port
#define LCMD_SET_WIFI_SSID			0x001C	// Set Device WIFI SSID
#define LCMD_GET_WIFI_SSID			0x001D	// Get Device WIFI SSID
#define LCMD_SET_WIFI_SEC			0x001E	// Set Device WIFI Security Type
#define LCMD_GET_WIFI_SEC			0x001F	// Get Device WIFI Security Type
#define LCMD_SET_WIFI_PASS			0x0020	// Set Device WIFI Password
//#define LCMD_GET_WIFI_PASS		0x0021	// Get Device WIFI Password - Intentionally Not Implemented For Security Reasons.
#define LCMD_SET_DEV_MAX_BAUD		0x0022	// Set Device Max Baud
#define LCMD_GET_DEV_MAX_BAUD		0x0023	// Get Device Max Baud
#define LCMD_GET_DEV_NAME			0x0024	// Get Device Name
#define LCMD_GET_SERVO_CHANS		0x0025	// Get Servo Channels
//---0x0026 to 0x003F Reserved---

/****************************************************************************************
**  Digital I/O
****************************************************************************************/
//#define LCMD_SET_PIN_MODE			0x0040	// Set Pin Mode
#define LCMD_DIGITAL_WRITE			0x0041	// Digital Write
#define LCMD_DIGITAL_READ			0x0042	// Digital Read
#define LCMD_SET_SQUARE_WAVE		0x0043	// Write Square Wave
#define LCMD_GET_PULSE_WIDTH		0x0044	// Read Pulse Width
//---0x0045 to 0x005F Reserved---

/****************************************************************************************
**  Analog I/O
****************************************************************************************/
#define LCMD_SET_AI_REF_VOLT		0x0060	// Set AI Reference Voltage
#define LCMD_GET_ÀI_REF_VOLT		0x0061	// Get AI Reference Voltage
//#define LCMD_SET_AI_RESOLUTIOM	0x0062	// Set AI Resolution
//#define LCMD_GET_AI_RESOLUTION	0x0063	// Get AI Resolution
#define LCMD_ANALOG_READ			0x0064	// Analog Read
//#define LCMD_ANALOG_WRITE			0x0065	// Analog Write
//---0x0066 to 0x007F Reserved---

/****************************************************************************************
** PWM
****************************************************************************************/
//#define LCMD_PWM_OPEN				0x0080	// PWM Open
//#define LCMD_SET_PWM_MODE			0x0081	// PWM Set Mode
//#define LCMD_SET_PWN_FREQ			0x0082	// PWM Set Frequency
#define LCMD_SET_PWM_DUTY_CYCLE		0x0083	// PWM Set Duty Cycle
//#define LCMD_PWM_CLOSE			0x0084	// PWM Close

/****************************************************************************************
** QE
****************************************************************************************/
//---0x00A0 to 0x00BF Reserved---

/****************************************************************************************
** UART
****************************************************************************************/
#define LCMD_UART_OPEN				0x00C0	// UART Open
#define LCMD_UART_SET_BAUD			0x00C1	// UART Set Buad Rate
#define LCMD_UART_GET_BYTES			0x00C2	// UART Get Bytes Available
#define LCMD_UART_READ				0x00C3	// UART Read
#define LCMD_UART_WRITE				0x00C4	// UART Write
#define LCMD_UART_CLOSE				0x00C5	// UART Close
//---0x00C6 to 0x00DF Reserved---

/****************************************************************************************
** I2C
****************************************************************************************/
#define LCMD_I2C_OPEN				0x00E0	// I2C Open Master
#define LCMD_I2C_SPEED				0x00E1	// I2C Set Speed
#define LCMD_I2C_WRITE				0x00E2	// I2C Write
#define LCMD_I2C_READ				0x00E3	// I2C Read
#define LCMD_I2C_CLOSE				0x00E4	// I2C Close
//---0x00E5 to 0x00FF Reserved---

/****************************************************************************************
** SPI
****************************************************************************************/
#define LCMD_SPI_OPEN				0x0100	// SPI Open Master
#define LCMD_SPI_SET_ORDER			0x0101	// SPI Set Bit Order
#define LCMD_SPI_SET_FREQ			0x0102	// SPI Set Clock Frequency
#define LCMD_SPI_SET_MODE			0x0103	// SPI Set Mode
//#define LCMD_SPI_FRAME_SIZE		0x0104	//LEGACY - SPI Set Frame Size
//#define LCMD_SPI_SET_LOG			0x0105	//LEGACY - SPI Set CS Logic Level
//#define LCMD_SPI_SET_CS			0x0106	//LEGACY - SPI Set CS Channel
#define LCMD_SPI_TRANSFER			0x0107	// SPI Write Read
#define LCMD_SPI_CLOSE				0x0108	// SPI Close
//---0x0085 to 0x009F Reserved---

/****************************************************************************************
** CAN
****************************************************************************************/
//---0x0120 to 0x013F Reserved---

/****************************************************************************************
** SERVO
****************************************************************************************/
#define LCMD_SERVO_INIT				0x0140	// Servo Init
#define LCMD_SERVO_SET_PULSE		0x0141	// Servo Set Pulse Width
#define LCMD_SERVE_CLOSE			0x0142	// Servo Close

/****************************************************************************************
** WS2812
****************************************************************************************/
#define LCMD_WS2812_OPEN			0x0160	// WS2812 Open
#define LCMD_WS2812_WRITE_ONE		0x0161	// WS2812 Write One Pixel
#define LCMD_WS2812_WRITE_N			0x0162	// WS2812 Write N Pixels
#define LCMD_WS2812_REFRESH			0x0163	// WS2812 Refresh
#define LCMD_WS2812_CLOSE			0x0164	// WS2812 Close

#endif // LINX_COMMAND_H