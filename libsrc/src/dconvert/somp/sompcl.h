#include "ptyhdr.h"
#include "somphdrp.h"
#include "somphdrw.h"

class TextOutputStream;
class AttributeList;

class SOMP_Header_BothClass : public SOMP_HeaderClass
{
public:
	SOMP_Header_BothClass(istream *ist) : SOMP_HeaderClass(ist) {}

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

