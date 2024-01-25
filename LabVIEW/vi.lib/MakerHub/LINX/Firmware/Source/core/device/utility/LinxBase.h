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

typedef struct {
	int test;
} TestStruct;

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
		virtual unsigned int AddRef(void);
		virtual unsigned int Release(void);
	protected:

	private:
		unsigned int m_Refcount;
};
#endif