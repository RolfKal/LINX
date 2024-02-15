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

class LinxRaspiDioChannel : public LinxDioChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxRaspiDioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin, uint8_t cpuModell);
		virtual ~LinxRaspiDioChannel(void) {};

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t setDirection(uint8_t dir);
		virtual int32_t setPull(uint8_t pud);
		virtual int32_t setValue(uint8_t value);
		virtual int32_t getValue(uint8_t *value);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		static uint8_t m_CpuModel;
};

using namespace std;

class LinxRaspberryPi : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxRaspberryPi(LinxFmtChannel *debug = NULL);
		virtual ~LinxRaspberryPi(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual uint8_t GetDeviceName(unsigned char *buffer, uint8_t length);

	protected:

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System
		static char m_DeviceName[DEVICE_NAME_MAX];
		static uint32_t m_DeviceCode;
		static uint64 m_SerialNum;
		
};
#endif //LINX_RASPBERRYPI2B
