#include "ptyhdr.h"
#include "pacehdrp.h"
#include "pacehdrw.h"

class TextOutputStream;
class AttributeList;

class PACE_Header_BothClass : public PACE_HeaderClass
{
public:
	PACE_Header_BothClass(istream *ist) : PACE_HeaderClass(ist) {}

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

