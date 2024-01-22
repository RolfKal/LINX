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

#define getMilliSeconds()	(unsigned int)(getMsTicks() & UINT_MAX)
#define getSeconds()		(unsigned int)(getMsTicks() / 1000)
#include <list>
#include <string>

int WriteU8ToBuff(unsigned char *buffer, int offset, unsigned char val);
int WriteU16ToBuff(unsigned char *buffer, int offset, unsigned short val);
int WriteU32ToBuff(unsigned char *buffer, int offset, unsigned int val);
int WriteU8ArrToBuff(unsigned char *buffer, int offset, unsigned char *arr, int length);

unsigned char GetU8FromBuff(unsigned char *buffer, int offset);
unsigned short GetU16FromBuff(unsigned char *buffer, int offset);
unsigned int GetU32FromBuff(unsigned char *buffer, int offset);

int ReadU8FromBuff(unsigned char *buffer, int offset, unsigned char *val);
int ReadU16FromBuff(unsigned char *buffer, int offset, unsigned short *val);
int ReadU32FromBuff(unsigned char *buffer, int offset, unsigned int *val);
int ReadU8ArrFromBuff(unsigned char *buffer, int offset, unsigned char *arr, int length);
int ReadStringFromBuff(unsigned char *buffer, int offset, unsigned char *arr, int length);

unsigned char ReverseBits(unsigned char b);
void ReverseBits(unsigned char *buffer, int length);

unsigned long long getMsTicks(void);
void delayMs(unsigned int ms);

int fileExists(const char* path);
int fileExists(const char* path, int *length);
int fileExists(const char* directory, const char* fileName, unsigned int timout = 0);

int listDirectory(const char* path, std::list<std::string> list);

#endif // LINX_UTILITIES_H