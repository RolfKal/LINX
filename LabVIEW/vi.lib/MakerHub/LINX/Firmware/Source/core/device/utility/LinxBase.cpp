/****************************************************************************************
**  LINX - Generic LINX channel implementation.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDefines.h"
#include "LinxBase.h"
/************************************ Linx Base *************************************/

/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxBase::LinxBase(void)
{
	// Start with refcount 1
	m_Refcount = 1;
}

/****************************************************************************************
**  Functions
****************************************************************************************/
uint32_t LinxBase::AddRef(void)
{
#if Win32
	return InterlockedIncrement(&m_Refcount);
#elif Unix
	return __sync_fetch_and_add(&m_Refcount, 1);
#endif
}

uint32_t LinxBase::Release(void)
{
	uint32_t refcount = 
#if Win32
	InterlockedDecrement(&m_Refcount);
#elif Unix
	__sync_sub_and_fetch(&m_Refcount, 1);
#endif
	if (!refcount)
		delete this;
	return refcount;
}
