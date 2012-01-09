#ifndef __Header_shim__
#define __Header_shim__

// NB. a SHIM_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~SHIM_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class SHIM_Header_BothClass;
class SHIM_PixelDataSource;
class AttributeList;
class TransferSyntax;

class SHIM_Conversion {
	SHIM_Header_BothClass *shimhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	SHIM_PixelDataSource *pixeldatasrc;
public:
	SHIM_Conversion(istream &i,ostream &e);

	virtual ~SHIM_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out,unsigned imagenumber=0);

	bool convertAll(AttributeList *list,TransferSyntax *transfersyntax,unsigned imagenumber=0);

	bool convertHeader(AttributeList *list,unsigned imagenumber=0);

	bool convertPixelData(AttributeList *list,TransferSyntax *transfersyntax,unsigned imagenumber=0);
};

#endif /* __Header_shim__ */

