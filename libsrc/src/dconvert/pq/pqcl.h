#include "ptyhdr.h"
#include "pqhdrp.h"
#include "pqhdrw.h"

class TextOutputStream;
class AttributeList;

class PQ_Header_BothClass : public PQ_HeaderClass
{
public:
	PQ_Header_BothClass(istream *ist) : PQ_HeaderClass(ist) {}

	void DumpCommon(TextOutputStream *log);
	void DumpSelectedImage(TextOutputStream *log)
		{
			(void)log;
		}

	void ToDicom_Template   (AttributeList *list);
	void ToDicom_ManualMisc (AttributeList *list);
	void ToDicom_ManualPlane(AttributeList *list);
	void ToDicom_ManualDates(AttributeList *list);
};

