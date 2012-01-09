#include "ptyhdr.h"
#include "shimhdrm.h"
#include "shimhdrp.h"
#include "shimhdrw.h"

class TextOutputStream;
class AttributeList;

class SHIM_Header_BothClass : public SHIM_HeaderClass
{
public:
	SHIM_Header_BothClass(istream *ist) : SHIM_HeaderClass(ist) {}

	void DumpCommon(TextOutputStream *log);
	void DumpSelectedImage(TextOutputStream *log,unsigned imagenumber=0);

	void ToDicom_Template   (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualMisc (AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualPlane(AttributeList *list,unsigned imagenumber=0);
	void ToDicom_ManualDates(AttributeList *list,unsigned imagenumber=0);
};

