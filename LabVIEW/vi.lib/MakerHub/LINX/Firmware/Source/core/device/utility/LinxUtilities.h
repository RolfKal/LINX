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

int WriteU8ToBuff(unsigned char *buffer, unsigned int offset, unsigned char val);
int WriteU16ToBuff(unsigned char *buffer, unsigned int offset, unsigned short val);
int WriteU32ToBuff(unsigned char *buffer, unsigned int offset, unsigned int val);
int WriteU8ArrToBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length);

unsigned char GetU8FromBuff(unsigned char *buffer, unsigned int offset);
unsigned short GetU16FromBuff(unsigned char *buffer, unsigned int offset);
unsigned int GetU32FromBuff(unsigned char *buffer, unsigned int offset);

int ReadU8FromBuff(unsigned char *buffer, unsigned int offset, unsigned char *val);
int ReadU16FromBuff(unsigned char *buffer, unsigned int offset, unsigned short *val);
int ReadU32FromBuff(unsigned char *buffer, unsigned int offset, unsigned int *val);
int ReadU8ArrFromBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length);
int ReadStringFromBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length);

#endif // LINX_UTILITIES_H