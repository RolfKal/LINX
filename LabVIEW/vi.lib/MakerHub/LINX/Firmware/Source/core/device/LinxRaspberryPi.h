/****************************************************************************************
**  LINX header for Raspberry Pi 2 Model B
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_RASPBERRYPI_H
#define LINX_RASPBERRYPI_H

/****************************************************************************************
**  Defines
****************************************************************************************/
#define NUM_AI_CHANS 0
#define AI_RES_BITS 0
#define AI_REFV 0
#define NUM_AI_INT_REFS 0

#define NUM_AO_CHANS 0

#define NUM_CAN_CHANS 0

#define NUM_DIGITAL_CHANS 17

#define NUM_PWM_CHANS 0

#define NUM_SPI_CHANS 2
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 1

#define NUM_UART_CHANS 1
#define NUM_UART_SPEEDS 18

#define NUM_SERVO_CHANS 0

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <string>
#include <map>
#include "utility/LinxDevice.h"
#include "utility/LinxLinuxDevice.h"
#include "LinxRaspberryPi.h"

using namespace std;

class LinxRaspberryPi : public LinxLinuxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxRaspberryPi();
		virtual ~LinxRaspberryPi();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System
		static unsigned char m_DeviceName[];

		//AI
		//None

		//AO
		//None

		//CAN
		//None

		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];
		static const unsigned char m_gpioChan[NUM_DIGITAL_CHANS];

		//PWM
		//None

		//SPI
		unsigned char m_SpiChanBuf[NUM_SPI_CHANS];
		static unsigned char m_SpiChans[NUM_SPI_CHANS];
		static const char *m_SpiPaths[NUM_SPI_CHANS];
		static unsigned long m_SpiSupportedSpeeds[NUM_SPI_SPEEDS];
		static int m_SpiSpeedCodes[NUM_SPI_SPEEDS];

		//I2C
		static unsigned char m_I2cChans[NUM_I2C_CHANS];
		static string m_I2cPaths[NUM_I2C_CHANS];

		//UART
		static unsigned char m_UartChans[NUM_UART_CHANS];
		static unsigned long m_UartSupportedSpeeds[NUM_UART_SPEEDS];
		static unsigned long m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS];
		static string m_UartPaths[NUM_UART_CHANS];

		//Servo
		//None

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};

#endif //LINX_RASPBERRYPI2B
