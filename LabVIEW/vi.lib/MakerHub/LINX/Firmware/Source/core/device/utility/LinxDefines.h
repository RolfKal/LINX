/****************************************************************************************
**  LINX header for Linx Support.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_DEFINES_H
#define LINX_DEFINES_H

#ifdef _WIN32
#define Win32 1
#define _CRT_SECURE_NO_WARNINGS 1
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define Unix 1
#endif

#endif