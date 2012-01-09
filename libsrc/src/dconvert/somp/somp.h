#ifndef __Header_somp__
#define __Header_somp__

// NB. a SOMP_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~SOMP_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class SOMP_Header_BothClass;
class SOMP_PixelDataSource;
class AttributeList;
class TransferSyntax;

class SOMP_Conversion {
	SOMP_Header_BothClass *somphdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	SOMP_PixelDataSource *pixeldatasrc;
public:
	SOMP_Conversion(istream &i,ostream &e);

	virtual ~SOMP_Conversion();

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

#endif /* __Header_somp__ */

