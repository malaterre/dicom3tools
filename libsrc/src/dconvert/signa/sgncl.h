#include "ptyhdr.h"
#include "sgnhdrp.h"
#include "sgnhdrw.h"

class TextOutputStream;
class AttributeList;

class SGN_Header_BothClass : public SGN_HeaderClass
{
public:
	SGN_Header_BothClass(istream *ist)
		: SGN_HeaderClass(ist)
		{
		}

	void DumpCommon(TextOutputStream *log);
	void DumpSelectedImage(TextOutputStream *log,unsigned imagenumber=0)
		{
			(void)log; (void)imagenumber;
		}

	void ToDicom_Template   (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualMisc (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualDates(AttributeList *list,unsigned imagenumber=0);
};

