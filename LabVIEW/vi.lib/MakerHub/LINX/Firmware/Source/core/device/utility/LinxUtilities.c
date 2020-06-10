/****************************************************************************************
**  LINX file for generic LINX Utilities.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "LinxUtilities.h"

int WriteU8ToBuff(unsigned char *buffer, unsigned int offset, unsigned char val)
{
	buffer[offset++] = val;
	return offset;
}

int WriteU16ToBuff(unsigned char *buffer, unsigned int offset, unsigned short val)
{
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

int WriteU32ToBuff(unsigned char *buffer, unsigned int offset, unsigned int val)
{
	buffer[offset++] = ((val >> 24) & 0xFF);
	buffer[offset++] = ((val >> 16) & 0xFF);
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

int WriteU8ArrToBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length)
{
	if (length < 0)
		length = (int)strlen(arr);
	memcpy(buffer + offset, arr, length);
	return offset + length;
}

unsigned char GetU8FromBuff(unsigned char *buffer, unsigned int offset)
{
	return buffer[offset];
}

unsigned short GetU16FromBuff(unsigned char *buffer, unsigned int offset)
{
	return (((unsigned short)buffer[offset + 0] << 8) |
		    ((unsigned short)buffer[offset + 1]));
}

unsigned int GetU32FromBuff(unsigned char *buffer, unsigned int offset)
{
	return (((unsigned int)buffer[offset + 0] << 24) |
		    ((unsigned int)buffer[offset + 1] << 16) |
			((unsigned int)buffer[offset + 2] << 8) |
		    ((unsigned int)buffer[offset + 3]));
}

int ReadU8FromBuff(unsigned char *buffer, unsigned int offset, unsigned char *val)
{
	*val = buffer[offset++];
	return offset;
}

int ReadU16FromBuff(unsigned char *buffer, unsigned int offset, unsigned short *val)
{
	*val = (((unsigned int)buffer[offset + 0] << 8) |
		    ((unsigned int)buffer[offset + 1]));
	return offset + 2;
}

int ReadU32FromBuff(unsigned char *buffer, unsigned int offset, unsigned int *val)
{
	*val = (((unsigned int)buffer[offset + 0] << 24) |
		    ((unsigned int)buffer[offset + 1] << 16) |
		    ((unsigned int)buffer[offset + 2] << 8) |
		    ((unsigned int)buffer[offset + 3]));
	return offset + 4;
}

int ReadU8ArrFromBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length)
{
	memcpy(arr, buffer + offset, length);
	return offset + length;
}

int ReadStringFromBuff(unsigned char *buffer, unsigned int offset, unsigned char *arr, unsigned int length)
{
	memcpy(arr, buffer + offset, length);
	buffer[offset + length] = 0;
	return offset + length;
}

