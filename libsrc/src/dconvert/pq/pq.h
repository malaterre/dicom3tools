#ifndef __Header_pq__
#define __Header_pq__

// NB. a PQ_Conversion object MUST NOT GO OUT OF SCOPE before
// the AttributeList is finished with, otherwise the pixeldatasrc
// will be deleted by ~PQ_Conversion() and the pointer in
// OtherUnspecifiedLargeAttribute will be invalid 

class PQ_Header_BothClass;
class PQ_PixelDataSource;
class AttributeList;
class TransferSyntax;

class PQ_Conversion {
	PQ_Header_BothClass *pqhdr;
	BinaryInputStream *in;
	TextOutputStream *err;
	PQ_PixelDataSource *pixeldatasrc;
public:
	PQ_Conversion(istream &i,ostream &e);

	virtual ~PQ_Conversion();

	bool dumpCommon(ostream &out);

	bool dumpSelectedImage(ostream &out);

	bool convertAll(AttributeList *list,
			TransferSyntax *transfersyntax);

	bool convertHeader(AttributeList *list);

	bool convertPixelData(AttributeList *list,
			TransferSyntax *transfersyntax);
};

#endif /* __Header_pq__ */

