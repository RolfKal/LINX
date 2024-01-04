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

#define NUM_DIGITAL_CHANS 26

#define NUM_PWM_CHANS 0

#define NUM_SPI_CHANS 5
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 1

#define NUM_UART_CHANS 1

#define NUM_SERVO_CHANS 0

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDevice.h"
#include "LinxLinuxChannel.h"

class LinxRaspiDioChannel : public LinxSysfsDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxRaspiDioChannel(LinxFmtChannel *debug, unsigned char linxPin, unsigned char gpioPin) : LinxSysfsDioChannel(debug, linxPin, gpioPin) {};
		virtual ~LinxRaspiDioChannel() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetState(unsigned char state);		// direction and pull-up/down
		virtual int Write(unsigned char value);
		virtual int Read(unsigned char *value);
		virtual int WriteSquareWave(unsigned int freq, unsigned int duration);
		virtual int ReadPulseWidth(unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width);

	private:
		void setState(unsigned char state);
		void setPull(unsigned char pud);
		void setDirection(unsigned char direction);
};

using namespace std;

class LinxRaspberryPi : public LinxDevice
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
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);

	protected:

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System
		static char m_DeviceName[DEVICE_NAME_MAX];
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_RASPBERRYPI2B
