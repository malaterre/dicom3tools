#include "ptyhdr.h"
#include "gawinfo.h"
#include "gawhdrp.h"
#include "gawhdrw.h"

class TextOutputStream;
class AttributeList;

class GAW_Header_BothClass : public GAW_HeaderClass
{
	GAW_FileStructureInformation fileinfo;
public:
	GAW_Header_BothClass(istream *ist,GAW_FileStructureInformation &info)
		: GAW_HeaderClass(ist,info)
		{
			fileinfo=info;
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

