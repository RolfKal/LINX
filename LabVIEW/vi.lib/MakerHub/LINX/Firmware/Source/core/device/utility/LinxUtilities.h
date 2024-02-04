/****************************************************************************************
**  LINX header for generic LINX Utilities.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_UTILITIES_H
#define LINX_UTILITIES_H

#define getMilliSeconds()	getMsTicks()
#define getSeconds()		(getMsTicks() / 1000)
#include <list>
#include <string>
#include "LinxDefines.h"

uint32_t WriteU8ToBuff(uint8_t *buffer, uint32_t offset, uint8_t val);
uint32_t WriteU16ToBuff(uint8_t *buffer, uint32_t offset, uint16_t val);
uint32_t WriteU32ToBuff(uint8_t *buffer, uint32_t offset, uint32_t val);
uint32_t WriteU8ArrToBuff(uint8_t *buffer, uint32_t offset, uint8_t *arr, uint32_t length);

uint8_t GetU8FromBuff(uint8_t *buffer, uint32_t offset);
uint16_t GetU16FromBuff(uint8_t *buffer, uint32_t offset);
uint32_t GetU32FromBuff(uint8_t *buffer, uint32_t offset);

uint32_t ReadU8FromBuff(uint8_t *buffer, uint32_t offset, uint8_t *val);
uint32_t ReadU16FromBuff(uint8_t *buffer, uint32_t offset, uint16_t *val);
uint32_t ReadU32FromBuff(uint8_t *buffer, uint32_t offset, uint32_t *val);
uint32_t ReadU8ArrFromBuff(uint8_t *buffer, uint32_t offset, uint8_t *arr, uint32_t length);
uint32_t ReadStringFromBuff(uint8_t *buffer, uint32_t offset, unsigned char *arr, uint32_t length);

uint8_t ComputeChecksum(uint8_t checksum, uint8_t* buffer, int32_t length);

uint8_t ReverseBits(uint8_t b);
void ReverseBits(uint8_t *buffer, uint32_t length);

uint64_t getUsTicks(void);
uint32_t getMsTicks(void);
void delayMs(uint32_t ms);

int32_t fileExists(const char* path);
int32_t fileExists(const char* path, int32_t *length);
int32_t fileExists(const char* directory, const char* fileName, uint32_t timout = 0);

int32_t listDirectory(const char* path, std::list<std::string> list);

#endif // LINX_UTILITIES_H