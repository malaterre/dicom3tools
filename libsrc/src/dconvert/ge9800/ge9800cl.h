#include "ptyhdr.h"
#include "ge9800hdrp.h"
#include "ge9800hdrw.h"

class TextOutputStream;
class AttributeList;

class GE9800_Header_BothClass : public GE9800_HeaderClass
{
public:
	GE9800_Header_BothClass(istream *ist) : GE9800_HeaderClass(ist) {}

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

