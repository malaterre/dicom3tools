#ifndef __Header_xxxx__
#define __Header_xxxx__

// NB. a XXXX_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~XXXX_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class XXXX_Header_BothClass;
class XXXX_PixelDataSource;
class AttributeList;
class TransferSyntax;

class XXXX_Conversion {
	XXXX_Header_BothClass *xxxxhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	XXXX_PixelDataSource *pixeldatasrc;
public:
	XXXX_Conversion(istream &i,ostream &e);

	virtual ~XXXX_Conversion();

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

#endif /* __Header_xxxx__ */

