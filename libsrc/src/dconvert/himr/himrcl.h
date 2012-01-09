#include "ptyhdr.h"
#include "himrhdrp.h"
#include "himrhdrw.h"

class TextOutputStream;
class AttributeList;

class HIMR_Header_BothClass : public HIMR_HeaderClass
{
public:
	HIMR_Header_BothClass(istream *ist) : HIMR_HeaderClass(ist) {}

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

