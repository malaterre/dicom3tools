#ifndef __Header_gaw__
#define __Header_gaw__

#include "gawinfo.h"

// NB. a GAW_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~GAW_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class GAW_Header_BothClass;
class GAW_PixelDataSource;
class AttributeList;
class TransferSyntax;

class GAW_Conversion {
	GAW_Header_BothClass *gawhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	GAW_PixelDataSource *pixeldatasrc;

	GAW_FileStructureInformation fileinfo;

	void init(istream &i,ostream &e);
public:
	GAW_Conversion(istream &i,ostream &e);

	GAW_Conversion(istream &i,ostream &e,
		bool explicitoff,
		bool lengthpresent,
		Uint32 fileptr,
		Uint32 suiteptr,
		Uint32 examptr,
		Uint32 seriesptr,
		Uint32 imageptr);

	virtual ~GAW_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out,unsigned imagenumber=0)
		{
			(void)out; (void)imagenumber;
			return true;
		}


	bool convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber=0);

	bool convertHeader(AttributeList *list,unsigned imagenumber=0);

	bool convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber=0);
};

#endif /* __Header_gaw__ */

