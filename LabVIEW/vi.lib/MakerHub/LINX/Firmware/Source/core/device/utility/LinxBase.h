/****************************************************************************************
**  LINX header for generic LINX base object for refcounting.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/
#ifndef LINX_BASE_H_
#define LINX_BASE_H_

#ifndef LINXCONFIG
	#include "../config/LinxConfig.h"
#endif
#include "LinxDefines.h"

class LinxBase
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxBase(void);
		virtual ~LinxBase(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual uint32_t AddRef(void);
		virtual uint32_t Release(void);
	protected:

	private:
		uint32_t m_Refcount;
};
#endif