#include "ptyhdr.h"
#include "imtnhdrm.h"
#include "imtnhdrp.h"
#include "imtnhdrw.h"

class TextOutputStream;
class AttributeList;

class IMTN_Header_BothClass : public IMTN_HeaderClass
{
public:
	IMTN_Header_BothClass(istream *ist) : IMTN_HeaderClass(ist) {}

	void DumpCommon(TextOutputStream *log);
	void DumpSelectedImage(TextOutputStream *log,unsigned imagenumber=0);

	void ToDicom_Template   (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualMisc (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualDates(AttributeList *list,unsigned imagenumber=0);
};

