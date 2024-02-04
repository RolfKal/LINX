/****************************************************************************************
**  LINX - Wiring compatible device code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Defines
****************************************************************************************/		

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "LinxDefines.h"
#include "LinxChannel.h"
#include "LinxDevice.h"
#include "LinxWiringDevice.h"

//Not all wiring devices have these...
#ifndef EXTERNAL
	#define EXTERNAL 0
#endif
#ifndef DEFAULT
	#define DEFAULT 0
#endif

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

/****************************************************************************************
**  Variables
****************************************************************************************/		

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxWiringDevice::LinxWiringDevice(LinxFmtChannel *debug) : LinxDevice(debug)
{
	//LINX API Version
	LinxApiMajor = 3;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;
	
	//Load User Config Data From Non Volatile Storage
	userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
	
}

/****************************************************************************************
**  Functions
****************************************************************************************/
void LinxWiringDevice::DelayMs(uint32_t ms)
{
	delay(ms);
} 

uint32_t LinxWiringDevice::GetMilliSeconds(void)
{
	return millis();
}

uint32_t LinxWiringDevice::GetSeconds(void)
{
	return (millis() / 1000);
}

//--------------------------------------------------------ANALOG-------------------------------------------------------

int32_t LinxWiringDevice::AnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	uint32_t analogValue = 0;
	uint8_t responseByteOffset = 0;
	uint8_t responseBitsRemaining = 8; 
	uint8_t dataBitsRemaining = AiResolution;
  
	values[responseByteOffset] = 0x00;    //Clear First	Response Byte   

	//Loop Over All AI channels In Command Packet
	for (int32_t i = 0; i < numChans; i++)
	{
		analogValue = analogRead(channels[i]);	
		
		dataBitsRemaining = AiResolution;

		//Byte Packet AI Values In Response Packet
		while (dataBitsRemaining > 0)
		{
			*(values + responseByteOffset) |= ( (analogValue >> (AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining));
			//*(values+responseByteOffset) = 69;

			if (responseBitsRemaining > dataBitsRemaining)
			{
				//Current Byte Still Has Empty Bits
				responseBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				//Current Byte Full
				dataBitsRemaining = dataBitsRemaining - responseBitsRemaining;
				responseByteOffset++;
				responseBitsRemaining = 8;
				values[responseByteOffset] = 0x00;    //Clear Next Response Byte     
			}
		}
	}
	
	return L_OK;
}

int32_t LinxWiringDevice::AnalogSetRef(uint8_t mode, uint32_t voltage)
{
	#if NUM_AI_INT_REFS > 0
	switch (mode)
	{
		case 0: //Default
			analogReference(DEFAULT);
			AiRefSet = AiRefDefault;
			break;
		case 1: //Internal
			if (NumAiRefIntVals > 0)
			{
				//Check If Internal AI Ref Value Is Supported
				for (int32_t i = 0; i < NumAiRefIntVals; i++)
				{				
					//Voltage Is Supported
					if (AiRefIntVals[i] == voltage)
					{
						analogReference(AiRefCodes[i]);
						AiRefSet = voltage;
						return L_OK;
					}
				}
				//Didn't Find Voltage
				return LANALOG_REF_VAL_ERROR;
			}
			else
			{
				//No Internal Voltages, So Internal Mode Not Supported
				return LANALOG_REF_MODE_ERROR;
			}			
			break;
		case 2: //External
			if (voltage >= AiRefExtMin && voltage <= AiRefExtMax)
			{
				analogReference(EXTERNAL);
				AiRefSet = voltage;
				return L_OK;
			}
			else
			{
				return LANALOG_REF_VAL_ERROR;
			}
			break;
		default:
			return LANALOG_REF_MODE_ERROR;
			break;	
	}
	return L_OK;
	#endif //NUM_AI_INT_REFS > 0

	return L_FUNCTION_NOT_SUPPORTED;
}

//--------------------------------------------------------DIGITAL-------------------------------------------------------

int32_t LinxWiringDevice::DigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	for (int32_t i = 0; i < numChans; i++)
	{		
		pinMode(channels[i], OUTPUT);
		digitalWrite(channels[i], (values[i / 8] >> i % 8) & 0x01);
	}
	return L_OK;
}

int32_t LinxWiringDevice::DigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	uint8_t bitOffset = 8;
	uint8_t byteOffset = 0;
	uint8_t retVal = 0;
 
	//Loop Over channels To Read
	for (int32_t i = 0; i < numChans; i++)
	{
		//If bitOffset Is 0 We Have To Start A New Byte, Store Old Byte And Increment OFfsets
		if (bitOffset == 0)
		{
			//Insert retVal Into Response Buffer
			values[byteOffset] = retVal;
			retVal = 0x00;
			byteOffset++;
			bitOffset = 7;      
		}
		else
		{
			bitOffset--;
		}
		
		//Read From Next Pin
		uint8_t pinNumber = channels[i];
			
		pinMode(pinNumber, INPUT);											//Set Pin As Input (Might Make This Configurable)    		
		retVal = retVal | (digitalRead(pinNumber) << bitOffset);	//Read Pin And Insert Value Into retVal
	}
	
	//Store Last Byte
	values[byteOffset] = retVal;
	
	return L_OK;
}


int32_t LinxWiringDevice::DigitalWriteSquareWave(uint8_t channel, uint32_t freq, uint32_t duration)
{
	if (freq > 0)
	{
		pinMode(channel, OUTPUT);
		if (duration > 0)
		{
			tone(channel, freq, duration);
		}
		else
		{
			tone(channel, freq);
		}
	}
	else
	{
		noTone(channel);
	}	
	return L_OK;
}

int32_t LinxWiringDevice::DigitalReadPulseWidth(uint8_t stimChan, uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width)
{
	//Stimulus
	if (stimType == 1)
	{
		//High->Low->High
		pinMode(stimChan, OUTPUT);
		
		digitalWrite(stimChan, HIGH);
		delay(1);
		digitalWrite(stimChan, LOW);
		delay(1);
		digitalWrite(stimChan, HIGH);		
	}
	else if (stimType == 2)
	{
		//Low->High->Low
		pinMode(stimChan, OUTPUT);
		
		digitalWrite(stimChan, LOW);
		delay(1);
		digitalWrite(stimChan, HIGH);
		delay(1);
		digitalWrite(stimChan, LOW);		
	}
	
	//Read Pulse
	pinMode(respChan, INPUT);
	
	if (respType == 0)
	{
		*width = pulseIn(respChan, LOW, timeout);
	}
	else if (respType == 1)
	{
		*width = pulseIn(respChan, HIGH, timeout);
	}	
	return L_OK;	
}

//--------------------------------------------------------PWM-----------------------------------------------------------

int32_t LinxWiringDevice::PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	for (int32_t i = 0; i < numChans; i++)
	{		
		pinMode(channels[i], OUTPUT);
		analogWrite(channels[i], values[i]);
	}
	return L_OK;
}

//--------------------------------------------------------SPI-----------------------------------------------------------

int32_t LinxWiringDevice::SpiOpenMaster(uint8_t channel)
{
	SPI.begin();
	return 0;
}

int32_t LinxWiringDevice::SpiSetBitOrder(uint8_t channel, uint8_t bitOrder)
{
	SPI.setBitOrder(bitOrder);
	return 0;
}

int32_t LinxWiringDevice::SpiSetMode(uint8_t channel, uint8_t mode)
{
	 switch(mode)
	 {
		case 0: 
			SPI.setDataMode(SPI_MODE0);
			break;
		case 1: 
			SPI.setDataMode(SPI_MODE1);
			break;
		case 2: 
			SPI.setDataMode(SPI_MODE2);
			break;
		case 3: 
			SPI.setDataMode(SPI_MODE3);
			break;
	}
	return 0;
}

int32_t LinxWiringDevice::SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	//Loop Over All Supported SPI Speeds (SPI Speeds Should Be Fastest -> Slowest)
	for (int32_t index = 0; index < NumSpiSpeeds; index++)
	{
			//If Target Speed Is greater or equal to the current supported speed use current supported speed (it's the fastest supported speed that is less or equal to the target)
			if (speed >= *(SpiSupportedSpeeds + index))
			{
				*actualSpeed = *(SpiSupportedSpeeds + index);
				SPI.setClockDivider(*(SpiSpeedCodes + index));
				break;
			}
			if (index == NumSpiSpeeds-1)
			{
				//Target speed is slower than slowest supported.  Use slowest supported
				*actualSpeed = *(SpiSupportedSpeeds + index);
				SPI.setClockDivider(*(SpiSpeedCodes + index));
			}
	}
	return L_OK;
}

int32_t LinxWiringDevice::SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	//Set CS Pin As DO
	pinMode(csChan, OUTPUT);
	
	//Set CS Pin Idle Before Starting SPI Transfer
	digitalWrite(csChan, (~csLL & 0x01) );  

	//Loop Over Frames
	for (int32_t i = 0; i<numFrames; i++)
	{
		//Start of frame, set CS Pin Active
		digitalWrite(csChan, (csLL & 0x01) );
		
		//Loop Over Bytes In Frame
		for (int32_t j = 0; j < frameSize; j++)
		{
			//Transfer Data
			uint8_t byteNum = (i * frameSize) + j;
			recBuffer[byteNum] = SPI.transfer(sendBuffer[byteNum]);
		}		
		//Frame Complete, Set CS idel
		digitalWrite(csChan, (~csLL & 0x01) );
	}	
	return 0;
}

//--------------------------------------------------------I2C-----------------------------------------------------------

//Helper To Deal With Arduino API Changes
void LinxWireWrite(uint8_t data)
{
  #if ARDUINO_VERSION < 100
    Wire.send(data);
  #else
    Wire.write(data);
  #endif 
}

int32_t LinxWiringDevice::I2cOpenMaster(uint8_t channel)
{
	if (*(I2cRefCount + channel) > 0)
	{
		//Channel Already Open, Increment Ref Count
		*(I2cRefCount + channel) = *(I2cRefCount + channel) + 1;	
	}
	else
	{
		//Channel Not Yet Open, Open And Set Refcount = 1
		 Wire.begin();			//TODO ONLY SUPPORT ONE CHANNEL ATM
	}
	return 0;
}

int32_t LinxWiringDevice::I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	if (actualSpeed)
		*actualSpeed = 100000;  // we only support standard speed
	return L_OK;
}

int32_t LinxWiringDevice::I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer)
{ 
	#if ARDUINO_VERSION >= 100
		Wire.beginTransmission(slaveAddress);
		Wire.write(sendBuffer, numBytes);
		
		if (eofConfig == EOF_STOP)
		{
			Wire.endTransmission(true);
		}
		else if (eofConfig == EOF_RESTART)
		{
			Wire.endTransmission(false);
		}
		else
		{
			//EOF Not Supported, Stop Bus
			Wire.endTransmission(true);
			return LI2C_EOF;
		}	
	return L_OK;
	#else
		if (eofConfig != EOF_STOP)
		{
			//EOF Not Supported, Stop Bus
			return LI2C_EOF;
		}
		Wire.beginTransmission(slaveAddress);
		for (int32_t i = 0; i < numBytes; i++)
		{
			Wire.send(*(sendBuffer+i));
		}
		Wire.endTransmission();
		return 0;
	#endif	
}

int32_t LinxWiringDevice::I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer)
{
	#if ARDUINO_VERSION >= 100
		if (eofConfig == EOF_STOP)
		{
			Wire.requestFrom(slaveAddress, numBytes, (uint8_t)1);
		}
		else if (eofConfig == EOF_RESTART)
		{
			Wire.requestFrom(slaveAddress, numBytes, (uint8_t)0);
		}
		else
		{
			//EOF Not Supported		
			return LI2C_EOF;
		}
	#else
		if (eofConfig != EOF_STOP)
		{
			//EOF Not Supported		
			return LI2C_EOF;
		}
		Wire.requestFrom(slaveAddress, (uint8_t)numBytes);
	#endif
		
		//Wait For Data, Potentially Timeout
		uint32_t tickCount = millis();
		while (Wire.available() < numBytes)
		{
			if( (millis() - tickCount) > timeout)
			{
				return LI2C_READ_FAIL;
			}
		}
		
		//Data Read, Read And Return
		for (int32_t i = 0; i < numBytes; i++)
		{
			#if ARDUINO_VERSION >= 100
				*(recBuffer+i) = Wire.read();
			#else
				*(recBuffer+i) = Wire.receive();
			#endif			
		}	
		return L_OK;
}

int32_t LinxWiringDevice::I2cClose(uint8_t channel)
{
	//Function Not Supported
	return L_FUNCTION_NOT_SUPPORTED;
}
		
//--------------------------------------------------------UART----------------------------------------------------------

int32_t LinxWiringDevice::UartOpen(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud)
{
	int32_t index = 0;
	
	for (index = 0; index < NumUartSpeeds; index++)
	{
			if (baudRate < *(UartSupportedSpeeds+index))
			{		
				//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
				break;
			}			
	}
	
	//Once Loop Complets Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
	//If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud
	if (index != 0)
	{
		index = index -1;
	}
	if (channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if (channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if (channel == 2)
	{
		#if NUM_UART_CHANS > 2
			Serial2.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if (channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	return L_OK;
}

int32_t LinxWiringDevice::UartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud)
{
	UartClose(channel);
	int32_t retVal = UartOpen(channel, baudRate, actualBaud);
	return retVal;
}

int32_t LinxWiringDevice::UartGetBytesAvailable(uint8_t channel, uint32_t *numBytes)
{
	if (channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			*numBytes = Serial.available();
		#endif
	}
	if (channel == 1)
	{
		#if NUM_UART_CHANS > 1
			*numBytes = Serial1.available();
		#endif
	}
	if (channel == 2)
	{
		#if NUM_UART_CHANS > 2
		*numBytes = Serial2.available();
		#endif
	}
	if (channel == 3)
	{
		#if NUM_UART_CHANS > 3
			*numBytes = Serial3.available();
		#endif
	}
	return L_OK;
}

int32_t LinxWiringDevice::UartRead(uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, uint32_t* numBytesRead)
{
	#if ARDUINO_VERSION >= 100
	
		if (channel == 0)
		{	
			#if NUM_UART_CHANS > 0
				*numBytesRead = Serial.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if (channel == 1)
		{
			#if NUM_UART_CHANS > 1
				*numBytesRead = Serial1.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if (channel == 2)
		{
			#if NUM_UART_CHANS > 2
				*numBytesRead = Serial2.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if (channel == 3)
		{
			#if NUM_UART_CHANS > 3
				*numBytesRead = Serial3.readBytes((char*)recBuffer, numBytes);
			#endif
		}	
				
		if (*numBytesRead !=numBytes)
		{
			return LUART_READ_FAIL;
		}
		
		return L_OK;	
	
	#else
		for (int32_t i = 0; i < numBytes; i++)
		{
			int32_t data = -1;
			
			if (channel == 0)
			{	
				#if NUM_UART_CHANS > 0
					data = Serial.read();
				#endif
			}
			else if (channel == 1)
			{
				#if NUM_UART_CHANS > 1
					data = Serial1.read();
				#endif
			}
			else if (channel == 2)
			{
				#if NUM_UART_CHANS > 2
					data = Serial2.read();
				#endif
			}
			else if (channel == 3)
			{
				#if NUM_UART_CHANS > 3
					data = Serial3.read();
				#endif
			}			
			
			if (data < 0)
			{
				return LUART_READ_FAIL;
			}
			else
			{
				*(recBuffer+i) = (char)data;
			}
			
			//Read All Bytes Without Error.  Return Num Bytes Read So Listener Can Pass It To PacketizeAndSend()
			*numBytesRead = numBytes;
		}	
		return L_OK;
	#endif
}

int32_t LinxWiringDevice::UartWrite(uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer)
{
	if (channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.write(sendBuffer, numBytes);
		#endif
	}
	if (channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.write(sendBuffer, numBytes);
		#endif
	}
	if (channel == 2)
	{
		#if NUM_UART_CHANS > 2
			Serial2.write(sendBuffer, numBytes);
		#endif
	}
	if (channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.write(sendBuffer, numBytes);
		#endif
	}
	return L_OK;
}

int32_t LinxWiringDevice::UartClose(uint8_t channel)
{
	if (channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.end();
		#endif
	}
	if (channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.end();
		#endif
	}
	if (channel == 2)
	{
		#if NUM_UART_CHANS > 2
		Serial2.end();
		#endif
	}
	if (channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.end();
		#endif
	}
	return L_OK;
}

//--------------------------------------------------------SERVO----------------------------------------------------------
int32_t LinxWiringDevice::ServoOpen(uint8_t numChans, uint8_t* chans)
{
	for(int32_t i=0; i<numChans; i++)
	{
		uint8_t pin = chans[i];
		if (Servos[pin] == 0)
		{
			//Servo Not Yet Intialized On Specified Channel, Init
			Servos[pin] = new Servo();
			Servos[pin]->attach(pin);
			
			DebugPrint("Created New Servo On Channel ");
			DebugPrintln(pin, DEC);
		}
	}
	return L_OK;
}

int32_t LinxWiringDevice::ServoSetPulseWidth(uint8_t numChans, uint8_t* chans, uint16_t* pulseWidths)
{
	
	for (int32_t i = 0; i < numChans; i++)
	{	
		
		DebugPrint("Servo ");
		DebugPrint((uint32_t)Servos[chans[i]], DEC);
		DebugPrint(" : ");
		DebugPrintln(pulseWidths[i], DEC);
		Servos[chans[i]]->writeMicroseconds(pulseWidths[i]);		
	}
	return L_OK;
}



int32_t LinxWiringDevice::ServoClose(uint8_t numChans, uint8_t* chans)
{
	for (int32_t i = 0; i < numChans; i++)
	{
		Servos[chans[i]]->detach();
		Servos[chans[i]] = 0;
	}
	return L_OK;
}
		

//--------------------------------------------------------GENERAL----------------------------------------------------------
void LinxWiringDevice::NonVolatileWrite(int32_t address, uint8_t data)
{
	EEPROM.write(address, data);
}

unsigned char LinxWiringDevice::NonVolatileRead(int32_t address)
{
	return EEPROM.read(address);
}
