#ifndef __Header_imtn__
#define __Header_imtn__

// NB. a IMTN_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~IMTN_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class IMTN_Header_BothClass;
class IMTN_PixelDataSource;
class AttributeList;
class TransferSyntax;

class IMTN_Conversion {
	IMTN_Header_BothClass *imtnhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	IMTN_PixelDataSource *pixeldatasrc;
public:
	IMTN_Conversion(istream &i,ostream &e);

	virtual ~IMTN_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out,unsigned imagenumber=0);

	bool convertAll(AttributeList *list,TransferSyntax *transfersyntax,unsigned imagenumber=0);

	bool convertHeader(AttributeList *list,unsigned imagenumber=0);

	bool convertPixelData(AttributeList *list,TransferSyntax *transfersyntax,unsigned imagenumber=0);
};

#endif /* __Header_imtn__ */

