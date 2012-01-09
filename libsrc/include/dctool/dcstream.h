#ifndef __Header_dcstream__
#define __Header_dcstream__

#include "bnstream.h"
#include "transyn.h"

// See comments in bnstream.h about extractors and insertors

class Tag;
class Attribute;

class DicomInputStream : public BinaryInputStream {
private:
	TransferSyntax *	TransferSyntaxToReadDataSet;
	TransferSyntax *	TransferSyntaxToReadMetaHeader;
	TransferSyntax *	TransferSyntaxInUse;

	void initializeTransferSyntax(const char *uid,bool meta);
public:
	DicomInputStream(streambuf *buf,
			  const char *uid=0,
			  bool meta=true);
	DicomInputStream(istream& istr,
			  const char *uid=0,
			  bool meta=true);

	virtual ~DicomInputStream(void);

	void setTransferSyntaxToReadDataSet(TransferSyntax *ts);
	void readingDataSet(void);
	void readingMetaHeader(void);

	bool haveMetaHeader(void) const	{ return TransferSyntaxToReadMetaHeader != 0; }

	TransferSyntax *getTransferSyntaxInUse(void)
		{ return TransferSyntaxInUse; }

	TransferSyntax *getTransferSyntaxToReadDataSet(void)
		{ return TransferSyntaxToReadDataSet; }

	TransferSyntax *getTransferSyntaxToReadMetaHeader(void)
		{ return TransferSyntaxToReadMetaHeader; }

	DicomInputStream& operator>>(Uint8& rhs)
		{
			((BinaryInputStream&)*this).operator>>(rhs);
			return *this;
		}
	DicomInputStream& operator>>(Uint16& rhs)
		{
			((BinaryInputStream&)*this).operator>>(rhs);
			return *this;
		}
	DicomInputStream& operator>>(Uint32& rhs)
		{
			((BinaryInputStream&)*this).operator>>(rhs);
			return *this;
		}

	DicomInputStream& operator>>(Tag& rhs);		// in attrtag.cc
};

class DicomOutputStream : public BinaryOutputStream {
private:
	TransferSyntax *	TransferSyntaxToWriteDataSet;
	TransferSyntax *	TransferSyntaxToWriteMetaHeader;
	TransferSyntax *	TransferSyntaxInUse;
	bool			preambledone;
	bool			tiffinpreamble;
	Uint32			offsetofIFD;

	void initializeTransferSyntax(const char *uid,bool meta,
		bool implicitmeta,bool addtiff);
	void writeMetaHeaderPreamble(void);
public:
	DicomOutputStream(streambuf *buf,
			  const char *uid=DefaultTransferSyntaxUID,
			  bool meta=true,bool implicitmeta=false,
			  bool addtiff=false);

	DicomOutputStream(ostream& ostr,
			  const char *uid=DefaultTransferSyntaxUID,
			  bool meta=true,bool implicitmeta=false,
			  bool addtiff=false);

	virtual ~DicomOutputStream(void);

	void writingDataSet(void);
	void writingMetaHeader(void);

	bool haveMetaHeader(void) const
		{ return TransferSyntaxToWriteMetaHeader != 0; }

	TransferSyntax *getTransferSyntaxInUse(void)
		{ return TransferSyntaxInUse; }

	TransferSyntax *getTransferSyntaxToWriteDataSet(void)
		{ return TransferSyntaxToWriteDataSet; }

	TransferSyntax *getTransferSyntaxToWriteMetaHeader(void)
		{ return TransferSyntaxToWriteMetaHeader; }

	void setIFDOffset(Uint32 o)
		{
			offsetofIFD=o;
		}

	DicomOutputStream& operator<<(Uint8 rhs)
		{
			((BinaryOutputStream&)*this).operator<<(rhs);
			return *this;
		}
	DicomOutputStream& operator<<(Uint16 rhs)
		{
			((BinaryOutputStream&)*this).operator<<(rhs);
			return *this;
		}
	DicomOutputStream& operator<<(Uint32 rhs)
		{
			((BinaryOutputStream&)*this).operator<<(rhs);
			return *this;
		}

	DicomOutputStream& operator<<(const char *rhs)
		{
			((BinaryOutputStream&)*this).operator<<(rhs);
			return *this;
		}

	DicomOutputStream& operator<<(char rhs)
		{
			((BinaryOutputStream&)*this).operator<<(rhs);
			return *this;
		}

	DicomOutputStream& operator<<(Tag rhs);		// in attrtag.cc
	DicomOutputStream& operator<<(Attribute& rhs);	// in attr.cc
};

#endif /* __Header_dcstream__ */


