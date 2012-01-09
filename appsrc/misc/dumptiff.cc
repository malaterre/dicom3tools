#include <iostream.h>

#include "basetype.h"
#include "bnstream.h"
#include "errclass.h"
#include "mesgtext.h"

class TIFF_Reader : public ErrorsInClass {
private:
	BinaryInputStream *bin;

	Uint32 nextIFDOffset;
	bool nextIFDOffsetIsGood;

	bool readImageFileHeader(istream &in);
	bool readNextIFD(void);
public:
	TIFF_Reader(istream &in);
};

TIFF_Reader::TIFF_Reader(istream &in)
{
	nextIFDOffsetIsGood=false;
	if (!readImageFileHeader(in)) {
		errorstream << EMsgDC(FileHeaderReadFailed) << endl;
		good_flag=false;
	}
	while (readNextIFD()) {
	}
}

bool
TIFF_Reader::readImageFileHeader(istream &in)
{
	char Identifier[2];
	in.read(Identifier,2);
	if (!in) {
		errorstream
			<< EMsgDC(ReadFailed)
			<< " - TIFF Identifier"
			<< endl;
		good_flag=false;
		return false;
	}
	Endian endian;
	if (strncmp(Identifier,"II",2) == 0)
		endian=LittleEndian;
	else if (strncmp(Identifier,"II",2) == 0)
		endian=BigEndian;
	else {
		errorstream
			<< EMsgDC(UnrecognizedFormat)
			<< " - TIFF Identifier - \""
			<< Identifier[0] << Identifier[1]
			<< "\""
			<< endl;
		good_flag=false;
		return false;
	}

	bin=new BinaryInputStream(in.rdbuf(),endian);
	Assert(bin);

	Uint16 Version;
	(*bin) >> Version;
	if (bin->fail()) {
		errorstream
			<< EMsgDC(ReadFailed)
			<< " - TIFF Version"
			<< endl;
		good_flag=false;
		return false;
	}
	if (Version != 0x002a) {
		errorstream
			<< EMsgDC(UnrecognizedFormat)
			<< " - TIFF Version - "
			<< Version
			<< endl;
		good_flag=false;
		return false;
	}

	(*bin) >> nextIFDOffset;
	if (bin->fail()) {
		errorstream
			<< EMsgDC(ReadFailed)
			<< " - TIFF ImageFileHeader IFDOffset"
			<< endl;
		good_flag=false;
		return false;
	}

	nextIFDOffsetIsGood=true;

cerr << "TIFF_Reader::readImageFileHeader"
     << " Identifier = " << Identifier[0] << Identifier[1]
     << " (endian = " << (endian == LittleEndian ? "little" : "big") << ")"
     << " Version = " << Version
     << " ImageFileHeader IFDOffset = " << nextIFDOffset
     << endl;
	return true;
}

bool
TIFF_Reader::readNextIFD(void)
{
	if (!nextIFDOffsetIsGood) return false;

	bin->seekg(nextIFDOffset,ios::beg);
	if (bin->fail()) {
		errorstream << EMsgDC(SeekFailed) << endl;
		good_flag=0;
		return false;
	}

	//(*bin) >> NumDirEntries;
	
}

int
main(int,char **)
{
	TIFF_Reader reader(cin);

cerr << "Not implemented yet ..." << endl;

	return 0;
}

