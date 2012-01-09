#ifndef __Header_sgn__
#define __Header_sgn__

// NB. a SGN_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~SGN_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class SGN_Header_BothClass;
class SGN_PixelDataSource;
class AttributeList;
class TransferSyntax;

class SGN_Conversion {
	SGN_Header_BothClass *sgnhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	SGN_PixelDataSource *pixeldatasrc;

	void init(istream &i,ostream &e);
public:
	SGN_Conversion(istream &i,ostream &e);

	virtual ~SGN_Conversion();

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

#endif /* __Header_sgn__ */

