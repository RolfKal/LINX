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
LibAPI(LinxDevice *) LinxOpenSerialDevice(const unsigned char *deviceName, unsigned int *baudrate, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity, int timeout);
LibAPI(LinxDevice *) LinxOpenTCPClient(const unsigned char *clientAddress, unsigned short port, int timeout);

LibAPI(LinxListener *) LinxOpenSerialServer(LinxDevice *dev, const unsigned char *deviceName, unsigned int baudRate, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity, int timeout, bool autostart);
LibAPI(LinxListener *) LinxOpenTCPServer(LinxDevice *dev, const unsigned char *interfaceAddress, short port, int timeout, bool autostart);
LibAPI(int) LinxServerProcess(LinxListener *listener, bool loop);

LibAPI(int) LinxCloseRef(LinxBase *base);

//------------------------------------- Enumeration -------------------------------------
LibAPI(unsigned char) LinxGetDeviceFamilyRef(LinxDevice *dev);
LibAPI(unsigned char) LinxGetDeviceIdRef(LinxDevice *dev);
LibAPI(int) LinxGetDeviceNameRef(LinxDevice *dev, unsigned char *name, int nameLen);

//------------------------------------- General -------------------------------------
LibAPI(unsigned int) LinxGetMilliSecondsRef(LinxDevice *dev);

//------------------------------------- Analog -------------------------------------
LibAPI(unsigned int) LinxAiGetRefSetVoltageRef(LinxDevice *dev);
LibAPI(unsigned int) LinxAoGetRefSetVoltageRef(LinxDevice *dev);
LibAPI(int) LinxAiGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxAoGetChansRefRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxAnalogReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxAnalogReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned int* values);
LibAPI(int) LinxAnalogWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- CAN -------------------------------------
LibAPI(int) LinxCanGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);

//------------------------------------- Digital -------------------------------------
LibAPI(int) LinxDigitalGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxDigitalSetStateRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* states);
LibAPI(int) LinxDigitalWriteRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalWriteNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalReadRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalReadNoPackingRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- I2C -------------------------------------
LibAPI(int) LinxI2cGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxI2cOpenMasterRef(LinxDevice *dev, unsigned char channel);
LibAPI(int) LinxI2cSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
LibAPI(int) LinxI2cWriteRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
LibAPI(int) LinxI2cReadRef(LinxDevice *dev, unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);		
LibAPI(int) LinxI2cCloseRef(LinxDevice *dev, unsigned char channel);
		
//------------------------------------- PWM -------------------------------------
LibAPI(int) LinxPwmGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxPwmSetDutyCycleRef(LinxDevice *dev, unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- QE -------------------------------------
LibAPI(int) LinxQeGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);

//------------------------------------- Servo -------------------------------------
LibAPI(int) LinxServoGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);

//------------------------------------- SPI -------------------------------------
LibAPI(int) LinxSpiGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxSpiOpenMasterRef(LinxDevice *dev, unsigned char channel);
LibAPI(int) LinxSpiSetBitOrderRef(LinxDevice *dev, unsigned char channel, unsigned char bitOrder);
LibAPI(int) LinxSpiSetModeRef(LinxDevice *dev, unsigned char channel, unsigned char mode);
LibAPI(int) LinxSpiSetSpeedRef(LinxDevice *dev, unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
LibAPI(int) LinxSpiWriteReadRef(LinxDevice *dev, unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
LibAPI(int) LinxSpiCloseMasterRef(LinxDevice *dev, unsigned char channel);
		
//------------------------------------- UART -------------------------------------
LibAPI(int) LinxUartGetChansRef(LinxDevice *dev, unsigned char* buffer, unsigned int *bufLen);
LibAPI(int) LinxUartOpenRef(LinxDevice *dev, unsigned char channel);
LibAPI(int) LinxUartOpenNameRef(LinxDevice *dev, const unsigned char *name, unsigned char *channel);
LibAPI(int) LinxUartSetBaudRateRef(LinxDevice *dev, unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
LibAPI(int) LinxUartSetParametersRef(LinxDevice *dev, unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
LibAPI(int) LinxUartGetBytesAvailableRef(LinxDevice *dev, unsigned char channel, unsigned int *numBytes);
LibAPI(int) LinxUartReadRef(LinxDevice *dev, unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, int timeout, unsigned int* numBytesRead);
LibAPI(int) LinxUartWriteRef(LinxDevice *dev, unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer, int timeout);
LibAPI(int) LinxUartCloseRef(LinxDevice *dev, unsigned char channel);

// -------------------------------------------------------------------------------------------------
//                                      Old Legacy API
//--------------------------------------------------------------------------------------------------

//------------------------------------- Constructor/Destructor -------------------------------------
LibAPI(int) LinxOpen(void);
LibAPI(int) LinxClose(void);

//------------------------------------- Enumeration -------------------------------------
LibAPI(unsigned char) LinxGetDeviceFamily(void);
LibAPI(unsigned char) LinxGetDeviceId(void);
LibAPI(int) LinxGetDeviceName(unsigned char *name);

//------------------------------------- General -------------------------------------
LibAPI(unsigned int) LinxGetMilliSeconds(void);

//------------------------------------- Analog -------------------------------------
LibAPI(unsigned int) LinxAiGetRefSetVoltage(void);
LibAPI(unsigned int) LinxAoGetRefSetVoltage(void);
LibAPI(unsigned char) LinxAiGetResolution(void);
LibAPI(unsigned char) LinxAoGetResolution(void);
LibAPI(unsigned char) LinxAiGetNumChans(void);
LibAPI(unsigned char) LinxAoGetNumChans(void);
LibAPI(int) LinxAiGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxAoGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values);
LibAPI(int) LinxAnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- CAN -------------------------------------
LibAPI(unsigned char) LinxCanGetNumChans(void);
LibAPI(int) LinxCanGetChans(unsigned char numChans, unsigned char* channels);

//------------------------------------- Digital -------------------------------------
LibAPI(unsigned char) LinxDigitalGetNumChans(void);
LibAPI(int) LinxDigitalGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxDigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states);
LibAPI(int) LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
LibAPI(int) LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- I2C -------------------------------------
LibAPI(unsigned char) LinxI2cGetNumChans(void);
LibAPI(int) LinxI2cGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxI2cOpenMaster(unsigned char channel);
LibAPI(int) LinxI2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
LibAPI(int) LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
LibAPI(int) LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);		
LibAPI(int) LinxI2cClose(unsigned char channel);
		
//------------------------------------- PWM -------------------------------------
LibAPI(unsigned char) LinxPwmGetNumChans(void);
LibAPI(int) LinxPwmGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- QE -------------------------------------
LibAPI(unsigned char) LinxQeGetNumChans(void);
LibAPI(int) LinxQeGetChans(unsigned char numChans, unsigned char* channels);

//------------------------------------- Servo -------------------------------------
LibAPI(unsigned char) LinxServoGetNumChans(void);
LibAPI(int) LinxServoGetChans(unsigned char numChans, unsigned char* channels);

//------------------------------------- SPI -------------------------------------
LibAPI(unsigned char) LinxSpiGetNumChans(void);
LibAPI(int) LinxSpiGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxSpiOpenMaster(unsigned char channel);
LibAPI(int) LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
LibAPI(int) LinxSpiSetMode(unsigned char channel, unsigned char mode);
LibAPI(int) LinxSpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed);
LibAPI(int) LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
LibAPI(int) LinxSpiCloseMaster(unsigned char channel);
		
//------------------------------------- UART -------------------------------------
LibAPI(unsigned char) LinxUartGetNumChans(void);
LibAPI(int) LinxUartGetChans(unsigned char numChans, unsigned char* channels);
LibAPI(int) LinxUartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
LibAPI(int) LinxUartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);
LibAPI(int) LinxUartSetParameters(unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
LibAPI(int) LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes);
LibAPI(int) LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead);
LibAPI(int) LinxUartRead2(unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, int timeout, unsigned int* numBytesRead);
LibAPI(int) LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer);
LibAPI(int) LinxUartWrite2(unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer, int timeout);
LibAPI(int) LinxUartClose(unsigned char channel);

#endif //LINX_DEVICELIB_H