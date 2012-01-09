#ifndef __Header_himr__
#define __Header_himr__

// NB. a HIMR_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~HIMR_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class HIMR_Header_BothClass;
class HIMR_PixelDataSource;
class AttributeList;
class TransferSyntax;

class HIMR_Conversion {
	HIMR_Header_BothClass *himrhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	HIMR_PixelDataSource *pixeldatasrc;
public:
	HIMR_Conversion(istream &i,ostream &e);

	virtual ~HIMR_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out);

	bool convertAll(AttributeList *list,
			TransferSyntax *transfersyntax);

	bool convertHeader(AttributeList *list);

	bool convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax);
};

#endif /* __Header_himr__ */

