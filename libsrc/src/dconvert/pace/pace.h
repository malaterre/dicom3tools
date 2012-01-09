#ifndef __Header_pace__
#define __Header_pace__

// NB. a PACE_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~PACE_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class PACE_Header_BothClass;
class PACE_PixelDataSource;
class AttributeList;
class TransferSyntax;

class PACE_Conversion {
	PACE_Header_BothClass *pacehdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	PACE_PixelDataSource *pixeldatasrc;
public:
	PACE_Conversion(istream &i,ostream &e);

	virtual ~PACE_Conversion();

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

#endif /* __Header_pace__ */

