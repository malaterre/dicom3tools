#include "ptyhdr.h"
#include "xxxxhdrp.h"
#include "xxxxhdrw.h"

class TextOutputStream;
class AttributeList;

class XXXX_Header_BothClass : public XXXX_HeaderClass
{
public:
	XXXX_Header_BothClass(istream *ist) : XXXX_HeaderClass(ist) {}

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

