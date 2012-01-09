#ifndef __Header_ge9800__
#define __Header_ge9800__

// NB. a GE9800_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~GE9800_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class GE9800_Header_BothClass;
class GE9800_PixelDataSource;
class AttributeList;
class TransferSyntax;

class GE9800_Conversion {
	GE9800_Header_BothClass *ge9800hdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	GE9800_PixelDataSource *pixeldatasrc;
public:
	GE9800_Conversion(istream &i,ostream &e);

	virtual ~GE9800_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out,unsigned imagenumber=0);

	bool convertAll(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber=0);

	bool convertHeader(AttributeList *list,unsigned imagenumber=0);

	bool convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax,
			unsigned imagenumber=0);
};

#endif /* __Header_ge9800__ */

