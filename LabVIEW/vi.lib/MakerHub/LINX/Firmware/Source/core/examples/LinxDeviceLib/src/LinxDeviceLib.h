/****************************************************************************************
**  Header file for LINX Device Lib (Shared Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_DEVICELIB_H
#define LINX_DEVICELIB_H

#if defined(_MSC_VER)
 #if defined(LINX_EXPORTS)
  #define LibAPI(type)	extern "C" __declspec(dllexport) type
 #else
  #define LibAPI(type)	extern "C" __declspec(dllimport) type
 #endif
#elif defined (__GNUC__) && defined(__unix__)
 #define LibAPI(type)	extern "C" __attribute__ ((__visibility__("default"))) type
#else
 #define LibAPI(type)	extern "C" type
#endif

//------------------------------------- Constructor/Destructor -------------------------------------
LibAPI(LinxDevice *) LinxOpenLocalClient(void);
LibAPI(LinxDevice *) LinxOpenSerialDevice(const unsigned char *deviceName, uint32_t *baudrate, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity, int32_t timeout);
LibAPI(LinxDevice *) LinxOpenTCPClient(const unsigned char *clientAddress, uint16_t port, int32_t timeout);

LibAPI(LinxListener *) LinxOpenSerialServer(LinxDevice *dev, const unsigned char *deviceName, uint32_t baudRate, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity, int32_t timeout, bool autostart);
LibAPI(LinxListener *) LinxOpenTCPServer(LinxDevice *dev, const unsigned char *interfaceAddress, uint16_t port, int32_t timeout, bool autostart);
LibAPI(int32_t) LinxServerProcess(LinxListener *listener, bool loop);

LibAPI(int32_t) LinxCloseRef(LinxBase *base);

//------------------------------------- Enumeration -------------------------------------
LibAPI(uint8_t) LinxGetDeviceFamilyRef(LinxDevice *dev);
LibAPI(uint8_t) LinxGetDeviceIdRef(LinxDevice *dev);
LibAPI(int32_t) LinxGetDeviceNameRef(LinxDevice *dev, unsigned char *name, int32_t nameLen);

//------------------------------------- General -------------------------------------
LibAPI(uint32_t) LinxGetMilliSecondsRef(LinxDevice *dev);

//------------------------------------- Analog -------------------------------------
LibAPI(uint32_t) LinxAiGetRefSetVoltageRef(LinxDevice *dev);
LibAPI(uint32_t) LinxAoGetRefSetVoltageRef(LinxDevice *dev);
LibAPI(int32_t) LinxAiGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxAoGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxAnalogReadRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxAnalogReadNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint32_t* values);
LibAPI(int32_t) LinxAnalogWriteRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- CAN -------------------------------------
LibAPI(int32_t) LinxCanGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);

//------------------------------------- Digital -------------------------------------
LibAPI(int32_t) LinxDigitalGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxDigitalSetStateRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* states);
LibAPI(int32_t) LinxDigitalWriteRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalWriteNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalReadRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalReadNoPackingRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- I2C -------------------------------------
LibAPI(int32_t) LinxI2cGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxI2cOpenMasterRef(LinxDevice *dev, uint8_t channel);
LibAPI(int32_t) LinxI2cSetSpeedRef(LinxDevice *dev, uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
LibAPI(int32_t) LinxI2cWriteRef(LinxDevice *dev, uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer);
LibAPI(int32_t) LinxI2cReadRef(LinxDevice *dev, uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer);		
LibAPI(int32_t) LinxI2cCloseRef(LinxDevice *dev, uint8_t channel);
		
//------------------------------------- PWM -------------------------------------
LibAPI(int32_t) LinxPwmGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxPwmSetDutyCycleRef(LinxDevice *dev, uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- QE -------------------------------------
LibAPI(int32_t) LinxQeGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);

//------------------------------------- Servo -------------------------------------
LibAPI(int32_t) LinxServoGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);

//------------------------------------- SPI -------------------------------------
LibAPI(int32_t) LinxSpiGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxSpiOpenMasterRef(LinxDevice *dev, uint8_t channel);
LibAPI(int32_t) LinxSpiSetBitOrderRef(LinxDevice *dev, uint8_t channel, uint8_t bitOrder);
LibAPI(int32_t) LinxSpiSetModeRef(LinxDevice *dev, uint8_t channel, uint8_t mode);
LibAPI(int32_t) LinxSpiSetSpeedRef(LinxDevice *dev, uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
LibAPI(int32_t) LinxSpiWriteReadRef(LinxDevice *dev, uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
LibAPI(int32_t) LinxSpiCloseMasterRef(LinxDevice *dev, uint8_t channel);
		
//------------------------------------- UART -------------------------------------
LibAPI(int32_t) LinxUartGetChansRef(LinxDevice *dev, uint8_t* buffer, uint32_t *bufLen);
LibAPI(int32_t) LinxUartOpenRef(LinxDevice *dev, uint8_t channel);
LibAPI(int32_t) LinxUartOpenNameRef(LinxDevice *dev, const uint8_t *name, uint8_t *channel);
LibAPI(int32_t) LinxUartSetBaudRateRef(LinxDevice *dev, uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
LibAPI(int32_t) LinxUartSetParametersRef(LinxDevice *dev, uint8_t channel, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);
LibAPI(int32_t) LinxUartGetBytesAvailableRef(LinxDevice *dev, uint8_t channel, uint32_t *numBytes);
LibAPI(int32_t) LinxUartReadRef(LinxDevice *dev, uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead);
LibAPI(int32_t) LinxUartWriteRef(LinxDevice *dev, uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout);
LibAPI(int32_t) LinxUartCloseRef(LinxDevice *dev, uint8_t channel);

// -------------------------------------------------------------------------------------------------
//                                      Old Legacy API
//--------------------------------------------------------------------------------------------------

//------------------------------------- Constructor/Destructor -------------------------------------
LibAPI(int32_t) LinxOpen(void);
LibAPI(int32_t) LinxClose(void);

//------------------------------------- Enumeration -------------------------------------
LibAPI(uint8_t) LinxGetDeviceFamily(void);
LibAPI(uint8_t) LinxGetDeviceId(void);
LibAPI(int32_t) LinxGetDeviceName(unsigned char *name);

//------------------------------------- General -------------------------------------
LibAPI(uint32_t) LinxGetMilliSeconds(void);

//------------------------------------- Analog -------------------------------------
LibAPI(uint32_t) LinxAiGetRefSetVoltage(void);
LibAPI(uint32_t) LinxAoGetRefSetVoltage(void);
LibAPI(uint8_t) LinxAiGetResolution(void);
LibAPI(uint8_t) LinxAoGetResolution(void);
LibAPI(uint8_t) LinxAiGetNumChans(void);
LibAPI(uint8_t) LinxAoGetNumChans(void);
LibAPI(int32_t) LinxAiGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxAoGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxAnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxAnalogReadNoPacking(uint8_t numChans, uint8_t* channels, uint32_t* values);
LibAPI(int32_t) LinxAnalogWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- CAN -------------------------------------
LibAPI(uint8_t) LinxCanGetNumChans(void);
LibAPI(int32_t) LinxCanGetChans(uint8_t numChans, uint8_t* channels);

//------------------------------------- Digital -------------------------------------
LibAPI(uint8_t) LinxDigitalGetNumChans(void);
LibAPI(int32_t) LinxDigitalGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxDigitalSetState(uint8_t numChans, uint8_t* channels, uint8_t* states);
LibAPI(int32_t) LinxDigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalWriteNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values);
LibAPI(int32_t) LinxDigitalReadNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- I2C -------------------------------------
LibAPI(uint8_t) LinxI2cGetNumChans(void);
LibAPI(int32_t) LinxI2cGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxI2cOpenMaster(uint8_t channel);
LibAPI(int32_t) LinxI2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
LibAPI(int32_t) LinxI2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer);
LibAPI(int32_t) LinxI2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, unsigned char numBytes, uint32_t timeout, unsigned char* recBuffer);		
LibAPI(int32_t) LinxI2cClose(uint8_t channel);
		
//------------------------------------- PWM -------------------------------------
LibAPI(uint8_t) LinxPwmGetNumChans(void);
LibAPI(int32_t) LinxPwmGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxPwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values);

//------------------------------------- QE -------------------------------------
LibAPI(uint8_t) LinxQeGetNumChans(void);
LibAPI(int32_t) LinxQeGetChans(uint8_t numChans, uint8_t* channels);

//------------------------------------- Servo -------------------------------------
LibAPI(uint8_t) LinxServoGetNumChans(void);
LibAPI(int32_t) LinxServoGetChans(uint8_t numChans, uint8_t* channels);

//------------------------------------- SPI -------------------------------------
LibAPI(uint8_t) LinxSpiGetNumChans(void);
LibAPI(int32_t) LinxSpiGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxSpiOpenMaster(uint8_t channel);
LibAPI(int32_t) LinxSpiSetBitOrder(uint8_t channel, uint8_t bitOrder);
LibAPI(int32_t) LinxSpiSetMode(uint8_t channel, uint8_t mode);
LibAPI(int32_t) LinxSpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed);
LibAPI(int32_t) LinxSpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
LibAPI(int32_t) LinxSpiCloseMaster(uint8_t channel);
		
//------------------------------------- UART -------------------------------------
LibAPI(uint8_t) LinxUartGetNumChans(void);
LibAPI(int32_t) LinxUartGetChans(uint8_t numChans, uint8_t* channels);
LibAPI(int32_t) LinxUartOpen(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
LibAPI(int32_t) LinxUartSetBaudRate(uint8_t channel, uint32_t baudRate, uint32_t* actualBaud);
LibAPI(int32_t) LinxUartSetParameters(uint8_t channel, uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);
LibAPI(int32_t) LinxUartGetBytesAvailable(uint8_t channel, uint8_t *numBytes);
LibAPI(int32_t) LinxUartRead(uint8_t channel, uint8_t numBytes, unsigned char* recBuffer, uint8_t* numBytesRead);
LibAPI(int32_t) LinxUartRead2(uint8_t channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead);
LibAPI(int32_t) LinxUartWrite(uint8_t channel, uint8_t numBytes, unsigned char* sendBuffer);
LibAPI(int32_t) LinxUartWrite2(uint8_t channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout);
LibAPI(int32_t) LinxUartClose(uint8_t channel);

#endif //LINX_DEVICELIB_H