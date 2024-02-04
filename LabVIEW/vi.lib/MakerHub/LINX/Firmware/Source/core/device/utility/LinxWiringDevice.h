/****************************************************************************************
**  LINX header for Wiring compatible devices.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_WIRING_H
#define LINX_WIRING_H

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxDefines.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Variables
****************************************************************************************/		

class LinxWiringDevice : public LinxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		uint8_t NumAiRefIntVals;				//Number Of Internal AI Reference Voltages
		const uint32_t* AiRefIntVals;			//Supported AI Reference Voltages (uV)
		const int32_t* AiRefCodes;				//AI Ref Values (AI Ref Macros In Wiring Case)
		
		uint32_t AiRefExtMin;					//Min External AI Ref Value (uV)
		uint32_t AiRefExtMax;					//Min External AI Ref Value (uV)		
		
		uint8_t NumUartSpeeds;					//Number Of Support UART Buads
		uint32_t* UartSupportedSpeeds;			//Supported UART Bauds Frequencies
		
		uint8_t NumSpiSpeeds;						//Number Of Supported SPI Speeds
		uint32_t* SpiSupportedSpeeds;			//Supported SPI Clock Frequencies
		int32_t* SpiSpeedCodes;						//SPI Speed Values (Clock Divider Macros In Wiring Case)
		
		unsigned char* I2cRefCount;					//Number Opens - Closes On I2C Channel
		
//		Servo** Servos;								//Array Servo Pointers
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWiringDevice(LinxFmtChannel *debug = NULL);
		virtual ~LinxWiringDevice(void) {};
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		//Analog
		virtual int32_t AnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t AnalogSetRef(uint8_t mode, uint32_t voltage);
		
		//DIGITAL
		virtual int32_t DigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
		virtual int32_t DigitalWriteSquareWave(uint8_t channel, uint32_t freq, uint32_t duration);
		virtual int32_t DigitalReadPulseWidth(uint8_t stimChan, uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width);
		
		//PWM
		virtual int32_t PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values);
		
		//SPI
		virtual int32_t SpiOpenMaster(uint8_t channel);
		virtual int32_t SpiSetBitOrder(uint8_t channel, uint8_t bitOrder);
		virtual int32_t SpiSetMode(uint8_t channel, uint8_t mode);
		virtual int32_t SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		
		//I2C
		virtual int32_t I2cOpenMaster(uint8_t channel);
		virtual int32_t I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer);
		virtual int32_t I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer);		
		virtual int32_t I2cClose(uint8_t channel);
		
		//UART
		virtual int32_t UartOpen(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
		virtual int32_t UartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
		virtual int32_t UartGetBytesAvailable(uint8_t channel, uint8_t *numBytes);
		virtual int32_t UartRead(uint8_t channel, uint8_t numBytes, unsigned char* recBuffer, uint8_t* numBytesRead);
		virtual int32_t UartWrite(uint8_t channel, uint8_t numBytes, unsigned char* sendBuffer);
		virtual int32_t UartClose(uint8_t channel);
		
		//Servo
		virtual int32_t ServoOpen(uint8_t numChans, uint8_t* chans);
		virtual int32_t ServoSetPulseWidth(uint8_t numChans, uint8_t* chans, uint16_t* pulseWidths);
		virtual int32_t ServoClose(uint8_t numChans, uint8_t* chans);
		
		//General - 
		virtual void NonVolatileWrite(int32_t address, uint8_t data);
		virtual uint8_t NonVolatileRead(int32_t address);
		
	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel* LookupChannel(int32_t type, uint8_t channel);
		virtual uint8_t RegisterChannel(int32_t type, LinxChannel *chan);
		virtual void RegisterChannel(int32_t type, uint8_t channel, LinxChannel *chan);
		virtual void RemoveChannel(int32_t type, uint8_t channel);
		virtual void ClearChannels(int32_t type);
		
	private:
	/****************************************************************************************
	**  Functions
	****************************************************************************************/
		void LinxWireWrite(void);
				
};


#endif //LINX_WIRING_H
