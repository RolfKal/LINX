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
#define DEVICE_NAME_MAX 128

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
#include "LinxDevice.h"
#include "LinxLinuxDevice.h"
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
		unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System
		static char m_DeviceName[DEVICE_NAME_MAX];
		
		//AI
		
		//AO
		//None
		
		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];
		static const unsigned char m_gpioChan[NUM_DIGITAL_CHANS];
		
		//PWM
		
		//SPI
		static unsigned char m_SpiChans[NUM_SPI_CHANS];
		static const char * m_SpiPaths[NUM_SPI_CHANS];
		static int m_SpiHandles[NUM_SPI_CHANS];
		static unsigned int m_SpiSupportedSpeeds[NUM_SPI_SPEEDS];
		static int m_SpiSpeedCodes[NUM_SPI_SPEEDS];
				
		//I2C
		static unsigned char m_I2cChans[NUM_I2C_CHANS];
		static const char * m_I2cPaths[NUM_I2C_CHANS];
		
		//UART
		static unsigned char m_UartChans[NUM_UART_CHANS];
		static unsigned int m_UartSupportedSpeeds[NUM_UART_SPEEDS];
		static unsigned int m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS];
		static const char * m_UartPaths[NUM_UART_CHANS];
		
		//Servo		
		//none

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_RASPBERRYPI2B
