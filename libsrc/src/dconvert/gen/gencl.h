#include "ptyhdr.h"
#include "geninfo.h"
#include "genhdrp.h"
#include "genhdrw.h"

class TextOutputStream;
class AttributeList;

class GEN_Header_BothClass : public GEN_HeaderClass
{
	GEN_FileStructureInformation fileinfo;
public:
	GEN_Header_BothClass(istream *ist,GEN_FileStructureInformation &info)
		: GEN_HeaderClass(ist,info)
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

