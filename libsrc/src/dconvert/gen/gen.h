#ifndef __Header_gen__
#define __Header_gen__

#include "geninfo.h"

// NB. a GEN_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~GEN_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class GEN_Header_BothClass;
class GEN_PixelDataSource;
class AttributeList;
class TransferSyntax;

class GEN_Conversion {
	GEN_Header_BothClass *genhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	GEN_PixelDataSource *pixeldatasrc;

	GEN_FileStructureInformation fileinfo;

	void init(istream &i,ostream &e);
public:
	GEN_Conversion(istream &i,ostream &e);

	GEN_Conversion(istream &i,ostream &e,
		bool explicitoff,
		bool lengthpresent,
		Uint32 fileptr,
		Uint32 suiteptr,
		Uint32 examptr,
		Uint32 seriesptr,
		Uint32 imageptr);

	virtual ~GEN_Conversion();

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

#endif /* __Header_gen__ */

