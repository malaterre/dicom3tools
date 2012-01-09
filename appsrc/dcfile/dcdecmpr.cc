//#define ZEROATSTARTOFROW
//#define OVERRIDE_ESCAPELENGTH	16

// pick one ...
//#define READBITFROMLSBOFBYTE
//#define READBITFROMMSBOFBYTE
#define READBITFROMMSBOF16LE
//#define READBITFROMLSBOF16LE
//#define READBITFROMMSBOF16BE
//#define READBITFROMLSBOF16BE

//#define READBITFROMMSBOF32LE
//#define READBITFROMLSBOF32LE

#include "attrmxls.h"
#include "attrothr.h"
#include "attrval.h"
#include "mesgtext.h"
#include "bnopt.h"
#include "ioopt.h"
#include "dcopt.h"
#include "elmconst.h"
#include "transynd.h"
#include "transyn.h"

static void
writeTheValue(ostream &out,Uint16 accumulatedvalue)
{
	// little endian;
	out.put((unsigned char)(accumulatedvalue&0xff));
	out.put((unsigned char)((accumulatedvalue&0xff00)>>8));
}


static Uint16			actualVariableDataGroup;
static const unsigned char *	vVariablePixelData;
static Uint32			lVariablePixelData;
static unsigned 		readBitByteCount;
static short			readBitCount;
static Uint32			readBitValue;

static void
initializeReadBit(void)
{
	readBitByteCount=0;
	readBitCount=0;
	readBitValue=0;
}

static void
setVariableDataLocation(Uint16 vImageLocation)
{
	actualVariableDataGroup=vImageLocation;
	vVariablePixelData=0;
	lVariablePixelData=0;
}

static bool
getMoreVariableData(AttributeList &list,TextOutputStream &log)
{
	if (actualVariableDataGroup == 0) return false;

	Tag tVariablePixelDataGroupLength(actualVariableDataGroup,0x0000);
	Tag tVariablePixelData(actualVariableDataGroup,0x0010);
	Tag tVariableNextDataGroup(actualVariableDataGroup,0x0011);

	Uint32 vVariablePixelDataGroupLength = 0;
	Attribute *aVariablePixelDataGroupLength = list[tVariablePixelDataGroupLength];
	if (!aVariablePixelDataGroupLength)
		log << WMsgDC(MissingAttribute)
		    << " - \"VariablePixelDataGroupLength\""
		    << endl;
	else {
		vVariablePixelDataGroupLength=AttributeValue(aVariablePixelDataGroupLength);
//cerr << "VariablePixelDataGroupLength = <0x" << hex << vVariablePixelDataGroupLength << dec << ">" << endl;
	}

	Uint16 vVariableNextDataGroup = 0;
	Attribute *aVariableNextDataGroup = list[tVariableNextDataGroup];
	if (!aVariableNextDataGroup)
		log << WMsgDC(MissingAttribute)
		    << " - \"VariableNextDataGroup\""
		    << endl;
	else {
		vVariableNextDataGroup=AttributeValue(aVariableNextDataGroup);
//cerr << "VariableNextDataGroup = <0x" << hex << vVariableNextDataGroup << dec << ">" << endl;
	}

	Attribute *aVariablePixelData = list[tVariablePixelData];
	if (!aVariablePixelData)
		log << WMsgDC(MissingAttribute)
		    << " - \"VariablePixelData\""
		    << endl;
	else {
		Uint32 length = aVariablePixelData->getVL();
		if (vVariablePixelData) delete[] vVariablePixelData;
		//vVariablePixelData=new unsigned char[length];
		//Assert(vVariablePixelData);
		//lVariablePixelData=length;

		if (!aVariablePixelData->getValue(vVariablePixelData,lVariablePixelData)) Assert(0);
//cerr << "lVariablePixelData = <0x" << hex << lVariablePixelData << dec << ">" << endl;
	}

	actualVariableDataGroup=vVariableNextDataGroup;		// for next pass

	return true;
}

#ifdef READBITFROMMSBOF32LE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 4);
			Assert(lVariablePixelData%4 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint32)(vVariablePixelData[readBitByteCount+3])<<24)
			    |((Uint32)(vVariablePixelData[readBitByteCount+2])<<16)
			    |((Uint32)(vVariablePixelData[readBitByteCount+1])<<8)
			    |         vVariablePixelData[readBitByteCount];
//cerr << "readBitByteCount =" << readBitByteCount << endl;
//cerr << "readBitValue =0x" << hex << readBitValue << dec << endl;
		readBitByteCount+=2;
		readBitCount=32;
	}
	bit=(readBitValue>>(--readBitCount)) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMLSBOF32LE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 4);
			Assert(lVariablePixelData%4 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint32)(vVariablePixelData[readBitByteCount+3])<<24)
			    |((Uint32)(vVariablePixelData[readBitByteCount+2])<<16)
			    |((Uint32)(vVariablePixelData[readBitByteCount+1])<<8)
			    |         vVariablePixelData[readBitByteCount];
	}
	bit=(readBitValue>>(15-(--readBitCount))) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMMSBOF16LE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 2);
			Assert(lVariablePixelData%2 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint16)(vVariablePixelData[readBitByteCount+1])<<8)|vVariablePixelData[readBitByteCount];
//cerr << "readBitByteCount =" << readBitByteCount << endl;
//cerr << "readBitValue =0x" << hex << readBitValue << dec << endl;
		readBitByteCount+=2;
		readBitCount=16;
	}
	bit=(readBitValue>>(--readBitCount)) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMLSBOF16LE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 2);
			Assert(lVariablePixelData%2 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint16)(vVariablePixelData[readBitByteCount+1])<<8)|vVariablePixelData[readBitByteCount];
		readBitByteCount+=2;
		readBitCount=16;
	}
	bit=(readBitValue>>(15-(--readBitCount))) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMMSBOF16BE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 2);
			Assert(lVariablePixelData%2 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint16)(vVariablePixelData[readBitByteCount])<<8)|vVariablePixelData[readBitByteCount+1];
//cerr << "readBitByteCount =" << readBitByteCount << endl;
//cerr << "readBitValue =0x" << hex << readBitValue << dec << endl;
		readBitByteCount+=2;
		readBitCount=16;
	}
	bit=(readBitValue>>(--readBitCount)) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMLSBOF16BE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			Assert(lVariablePixelData >= 2);
			Assert(lVariablePixelData%2 == 0);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		// little endian word ...
		readBitValue=((Uint16)(vVariablePixelData[readBitByteCount])<<8)|vVariablePixelData[readBitByteCount+1];
		readBitByteCount+=2;
		readBitCount=16;
	}
	bit=(readBitValue>>(15-(--readBitCount))) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMMSBOFBYTE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		readBitValue=vVariablePixelData[readBitByteCount++];
		readBitCount=8;
	}
	bit=(readBitValue>>(--readBitCount)) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

#ifdef READBITFROMLSBOFBYTE
static bool
readBit(Uint32 &bit,AttributeList &list,TextOutputStream &log)
{
//cerr << "readBit():readBitByteCount = " << readBitByteCount << endl;
//cerr << "readBit():lVariablePixelData = " << lVariablePixelData << endl;

	// first bits are read from lsb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		if (readBitByteCount >= lVariablePixelData) {
			if (!getMoreVariableData(list,log)) return false;
			Assert(lVariablePixelData);
			readBitByteCount=0;
		}
		Assert(vVariablePixelData);
		readBitValue=vVariablePixelData[readBitByteCount++];
		readBitCount=8;
	}
	bit=(readBitValue>>(7-(--readBitCount))) & 1;
//cerr << (bit ? "1" : "0");
	return true;
}
#endif

static void
getValuesOfAttributes(AttributeList &list,ostream &out,TextOutputStream &log,bool quiet)
{

	Uint16 vBitsStored = 0;
	Attribute *aBitsStored = list[TagFromName(BitsStored)];
	if (!aBitsStored)
		log << WMsgDC(MissingAttribute)
		    << " - \"BitsStored\""
		    << endl;
	else {
		vBitsStored=AttributeValue(aBitsStored);
cerr << "BitsStored = <" << vBitsStored << ">" << endl;
	}

	Uint16 vColumns = 0;
	Attribute *aColumns = list[TagFromName(Columns)];
	if (!aColumns)
		log << WMsgDC(MissingAttribute)
		    << " - \"Columns\""
		    << endl;
	else {
		vColumns=AttributeValue(aColumns);
cerr << "Columns = <" << vColumns << ">" << endl;
	}

	Uint16 vRows = 0;
	Attribute *aRows = list[TagFromName(Rows)];
	if (!aRows)
		log << WMsgDC(MissingAttribute)
		    << " - \"Rows\""
		    << endl;
	else {
		vRows=AttributeValue(aRows);
cerr << "Rows = <" << vRows << ">" << endl;
	}

	char *vCompressionRecognitionCode = 0;
	Attribute *aCompressionRecognitionCode = list[TagFromName(CompressionRecognitionCode)];
	if (!aCompressionRecognitionCode)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionRecognitionCode\""
		    << endl;
	else {
		vCompressionRecognitionCode=AttributeValue(aCompressionRecognitionCode);
cerr << "CompressionRecognitionCode = <" << vCompressionRecognitionCode << ">" << endl;
	}

	char *vCompressionCode = 0;
	Attribute *aCompressionCode = list[TagFromName(CompressionCode)];
	if (!aCompressionCode)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionCode\""
		    << endl;
	else {
		vCompressionCode=AttributeValue(aCompressionCode);
cerr << "CompressionCode = <" << vCompressionCode << ">" << endl;
	}

	char *vCompressionOriginator = 0;
	Attribute *aCompressionOriginator = list[TagFromName(CompressionOriginator)];
	if (!aCompressionOriginator)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionOriginator\""
		    << endl;
	else {
		vCompressionOriginator=AttributeValue(aCompressionOriginator);
cerr << "CompressionOriginator = <" << vCompressionOriginator << ">" << endl;
	}

	char *vCompressionLabel = 0;
	Attribute *aCompressionLabel = list[TagFromName(CompressionLabel)];
	if (!aCompressionLabel)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionLabel\""
		    << endl;
	else {
		vCompressionLabel=AttributeValue(aCompressionLabel);
cerr << "CompressionLabel = <" << vCompressionLabel << ">" << endl;
	}

	int nCompressionSequence = 0;
	char ** vCompressionSequence = 0;
	Attribute *aCompressionSequence = list[TagFromName(CompressionSequence)];
	if (!aCompressionSequence)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionSequence\""
		    << endl;
	else {
		nCompressionSequence=aCompressionSequence->getVM();
		if (nCompressionSequence) {
			vCompressionSequence=new char *[nCompressionSequence];
			Assert(vCompressionSequence);
			int i;
			for (i=0; i<nCompressionSequence; ++i) {
				bool success=aCompressionSequence->getValue(i,vCompressionSequence[i]);
				Assert(success);
cerr << "CompressionSequence[" << i << "] = <" << vCompressionSequence[i] << ">" << endl;
			}
		}
	}

	int nCompressionStepPointers = 0;
	Uint32 *vCompressionStepPointers = 0;
	Attribute *aCompressionStepPointers = list[TagFromName(CompressionStepPointers)];
	if (!aCompressionStepPointers)
		log << WMsgDC(MissingAttribute)
		    << " - \"CompressionStepPointers\""
		    << endl;
	else {
		nCompressionStepPointers=aCompressionStepPointers->getVM();
		if (nCompressionStepPointers) {
			vCompressionStepPointers=new Uint32[nCompressionStepPointers];
			Assert(vCompressionStepPointers);
			int i;
			for (i=0; i<nCompressionStepPointers; ++i) {
				bool success=aCompressionStepPointers->getValue(i,vCompressionStepPointers[i]);
				Assert(success);
cerr << "CompressionStepPointers[" << i << "] = <0x" << hex << vCompressionStepPointers[i] << dec << ">" << endl;
			}
		}
	}

	Uint16 vPerimeterValue = 0;
	Attribute *aPerimeterValue = list[TagFromName(PerimeterValue)];
	if (!aPerimeterValue)
		log << WMsgDC(MissingAttribute)
		    << " - \"PerimeterValue\""
		    << endl;
	else {
		vPerimeterValue=AttributeValue(aPerimeterValue);
cerr << "PerimeterValue = <0x" << hex << vPerimeterValue << dec << ">" << endl;
	}

	Uint16 vPredictorRows = 0;
	Attribute *aPredictorRows = list[TagFromName(PredictorRows)];
	if (!aPredictorRows)
		log << WMsgDC(MissingAttribute)
		    << " - \"PredictorRows\""
		    << endl;
	else {
		vPredictorRows=AttributeValue(aPredictorRows);
cerr << "PredictorRows = <" << vPredictorRows << ">" << endl;
	}

	Uint16 vPredictorColumns = 0;
	Attribute *aPredictorColumns = list[TagFromName(PredictorColumns)];
	if (!aPredictorColumns)
		log << WMsgDC(MissingAttribute)
		    << " - \"PredictorColumns\""
		    << endl;
	else {
		vPredictorColumns=AttributeValue(aPredictorColumns);
cerr << "PredictorColumns = <" << vPredictorColumns << ">" << endl;
	}

	int nPredictorConstants = 0;
	Uint16 *vPredictorConstants = 0;
	Attribute *aPredictorConstants = list[TagFromName(PredictorConstants)];
	if (!aPredictorConstants)
		log << WMsgDC(MissingAttribute)
		    << " - \"PredictorConstants\""
		    << endl;
	else {
		nPredictorConstants=aPredictorConstants->getVM();
		if (nPredictorConstants) {
			vPredictorConstants=new Uint16[nPredictorConstants];
			Assert(vPredictorConstants);
			int i;
			for (i=0; i<nPredictorConstants; ++i) {
				bool success=aPredictorConstants->getValue(i,vPredictorConstants[i]);
				Assert(success);
cerr << "nPredictorConstants[" << i << "] = <" << hex << vPredictorConstants[i] << dec << ">" << endl;
			}
		}
	}

	Uint16 vImageLocation = 0;
	Attribute *aImageLocation = list[TagFromName(ImageLocation)];
	if (!aImageLocation)
		log << WMsgDC(MissingAttribute)
		    << " - \"ImageLocation\""
		    << endl;
	else {
		vImageLocation=AttributeValue(aImageLocation);
cerr << "ImageLocation = <0x" << hex << vImageLocation << dec << ">" << endl;
	}

cerr << "Ignoring ImageLocation and using <0x7f00>" << endl;

	setVariableDataLocation(0x7f00);







	int nCodeLabel = 0;
	char ** vCodeLabel = 0;
	Attribute *aCodeLabel = list[TagFromName(CodeLabel)];
	if (!aCodeLabel)
		log << WMsgDC(MissingAttribute)
		    << " - \"CodeLabel\""
		    << endl;
	else {
		nCodeLabel=aCodeLabel->getVM();
		if (nCodeLabel) {
			vCodeLabel=new char *[nCodeLabel];
			Assert(vCodeLabel);
			int i;
			for (i=0; i<nCodeLabel; ++i) {
				bool success=aCodeLabel->getValue(i,vCodeLabel[i]);
				Assert(success);
cerr << "CodeLabel[" << i << "] = <" << vCodeLabel[i] << ">" << endl;
			}
		}
	}

	Uint16 vNumberOfTables = 0;
	Attribute *aNumberOfTables = list[TagFromName(NumberOfTables)];
	if (!aNumberOfTables)
		log << WMsgDC(MissingAttribute)
		    << " - \"NumberOfTables\""
		    << endl;
	else {
		vNumberOfTables=AttributeValue(aNumberOfTables);
cerr << "NumberOfTables = <" << vNumberOfTables << ">" << endl;
	}


	Uint32 vCodeTableLocation = 0;
	Attribute *aCodeTableLocation = list[TagFromName(CodeTableLocation)];
	if (!aCodeTableLocation)
		log << WMsgDC(MissingAttribute)
		    << " - \"CodeTableLocation\""
		    << endl;
	else {
		/* vCodeTableLocation=AttributeValue(aCodeTableLocation); */ // Doesn`t work
		bool success=aCodeTableLocation->getValue(0,vCodeTableLocation);
		Assert(success);
cerr << "CodeTableLocation = <0x" << hex << vCodeTableLocation << dec << ">" << endl;
	}

	struct huffmantableindexentry {
		unsigned short bits;
		struct huffmantableentry *ptr;
	};

	unsigned huffmantableindexsize=0;
	struct huffmantableindexentry *huffmantableindex = 0;

	if (vNumberOfTables) {
		Assert(vCodeTableLocation);
		int i;
		Uint16 g;
		Uint16 e;

		for (i=0,g=Uint16((vCodeTableLocation&0xffff0000)>>16),
		     e=Uint16(e=vCodeTableLocation&0xffff); i < vNumberOfTables; ++i,e+=0x0010) {

			Uint16 vEscapeTripletNumberOfActualValueBits = 0;
			Uint16 vEscapeTripletNumberOfEscapeCodeBits = 0;
			Uint16 vEscapeTripletEscapeCode = 0;
			Attribute *aEscapeTriplet=list[Tag(g,e)];
			if (!aEscapeTriplet)
				log << WMsgDC(MissingAttribute)
				    << " - \"EscapeTriplet\""
				    << endl;
			else {
				Assert(aEscapeTriplet->getVM() == 3);
				bool success=aEscapeTriplet->getValue(0,vEscapeTripletNumberOfActualValueBits);
				Assert(success);
				success=aEscapeTriplet->getValue(1,vEscapeTripletNumberOfEscapeCodeBits);
				Assert(success);
				success=aEscapeTriplet->getValue(2,vEscapeTripletEscapeCode);
				Assert(success);
cerr << "EscapeTripletNumberOfActualValueBits[" << i << "] = <" << vEscapeTripletNumberOfActualValueBits << ">" << endl;
cerr << "EscapeTripletNumberOfEscapeCodeBits[" << i << "] = <" << vEscapeTripletNumberOfEscapeCodeBits << ">" << endl;
cerr << "EscapeTripletEscapeCode[" << i << "] = <0x" << hex << vEscapeTripletEscapeCode << dec << ">" << endl;
			}

			Uint16 useEscapeTripletNumberOfActualValueBits=vEscapeTripletNumberOfActualValueBits;
#ifdef OVERRIDE_ESCAPELENGTH
			useEscapeTripletNumberOfActualValueBits=OVERRIDE_ESCAPELENGTH;
#endif
cerr << "use EscapeTripletNumberOfActualValueBits[" << i << "] = <" << useEscapeTripletNumberOfActualValueBits << ">" << endl;

			Uint16 vHuffmanTableSize = 0;
			Attribute *aHuffmanTableSize=list[Tag(g,e+2)];
			if (!aHuffmanTableSize)
				log << WMsgDC(MissingAttribute)
				    << " - \"HuffmanTableSize\""
				    << endl;
			else {
				vHuffmanTableSize=AttributeValue(aHuffmanTableSize);
cerr << "HuffmanTableSize = <" << vHuffmanTableSize << ">" << endl;
			}

			Assert(sizeof(Int16) == 2);	// else would need to sign extend

			Int16 *vHuffmanTableTripletValue = 0;
			Uint16 *vHuffmanTableTripletNumberOfCodeBits = 0;
			Uint16 *vHuffmanTableTripletCode = 0;

			Attribute *aHuffmanTableTriplet=list[Tag(g,e+3)];
			if (!aHuffmanTableTriplet)
				log << WMsgDC(MissingAttribute)
				    << " - \"HuffmanTableTriplet\""
				    << endl;
			else {
				Assert(vHuffmanTableSize*6 == aHuffmanTableTriplet->getVL());
				vHuffmanTableTripletValue = new Int16[vHuffmanTableSize];
				vHuffmanTableTripletNumberOfCodeBits = new Uint16[vHuffmanTableSize];
				vHuffmanTableTripletCode = new Uint16[vHuffmanTableSize];
				Assert(vHuffmanTableTripletValue);
				Assert(vHuffmanTableTripletNumberOfCodeBits);
				Assert(vHuffmanTableTripletCode);
				int j,k;
				for (j=0,k=0; j<vHuffmanTableSize; ++j,k+=3) {
					bool success=aHuffmanTableTriplet->getValue(k,vHuffmanTableTripletValue[j]);
					Assert(success);
					success=aHuffmanTableTriplet->getValue(k+1,vHuffmanTableTripletNumberOfCodeBits[j]);
					Assert(success);
					success=aHuffmanTableTriplet->getValue(k+2,vHuffmanTableTripletCode[j]);
					Assert(success);
cerr << "HuffmanTableTripletValue[" << i << "," << j << "] = <0x" << hex << vHuffmanTableTripletValue[j] << dec << ">" << endl;
cerr << "HuffmanTableTripletNumberOfCodeBits[" << i << "," << j << "] = <" << vHuffmanTableTripletNumberOfCodeBits[j] << ">" << endl;
cerr << "HuffmanTableTripletCode[" << i << "," << j << "] = <0x" << hex << vHuffmanTableTripletCode[j] << dec << ">" << endl;
				}
			}

			// Build a set of huffman decoder tables ...

			unsigned maxcodelength = 0;
			unsigned j;
			for (j=0; j<vHuffmanTableSize; ++j)
				if (maxcodelength < vHuffmanTableTripletNumberOfCodeBits[j])
					maxcodelength=vHuffmanTableTripletNumberOfCodeBits[j];
			Assert(maxcodelength > 0);
cerr << "maxcodelength = " << maxcodelength << endl;

			// Find out which code lengths are in use ...
			
			unsigned *codelengthsused = new unsigned [maxcodelength+1];
			Assert(codelengthsused);
			for (j=1; j<=maxcodelength; ++j) codelengthsused[j]=0;
			for (j=0; j<vHuffmanTableSize; ++j) codelengthsused[vHuffmanTableTripletNumberOfCodeBits[j]]++;
			codelengthsused[vEscapeTripletNumberOfEscapeCodeBits]++;

for (j=1; j<=maxcodelength; ++j) cerr << "codelengthsused [ " << j << "] = " << codelengthsused[j] << endl;
			
			// How many code lengths are in use ?
			
			unsigned numberofcodelengthsused=0;
			for (j=1; j<=maxcodelength; ++j) if (codelengthsused[j]) ++numberofcodelengthsused;
cerr << "numberofcodelengthsused = " << numberofcodelengthsused << endl;

			// create a table for use during decoding with code lengths for only those in use ...

			unsigned *codelengthtable = new unsigned [numberofcodelengthsused];
			Assert(codelengthtable);
			unsigned k;
			for (j=1,k=0; j<=maxcodelength; ++j) if (codelengthsused[j]) codelengthtable[k++]=j;

for (k=0; k<numberofcodelengthsused; ++k) cerr << "codelengthtable [ " << k << "] = " << codelengthtable[k] << endl;

			// create a table of tables of actions and values (sparse)

			struct huffmandecodetableentry {
#define action_VALUE 1
#define action_ESCAPE 2
#define action_NEXTLENGTH 3
				unsigned action;
				Int16 value;
			};

			struct huffmandecodetableentry **huffmandecodetable = new struct huffmandecodetableentry *[numberofcodelengthsused];
			Assert(huffmandecodetable);
			
			for (k=0; k<numberofcodelengthsused; ++k) {
				unsigned numberofentries = 1<<codelengthtable[k];
				Assert(numberofentries);
cerr << "numberofentries[" << k << "] = " << numberofentries << endl;
				huffmandecodetable[k]=new struct huffmandecodetableentry[numberofentries];
				Assert(huffmandecodetable[k]);

				// default action is go to next length ...
				
				for (j=0; j<numberofentries; ++j) {
					huffmandecodetable[k][j].action=action_NEXTLENGTH;
					huffmandecodetable[k][j].value=0;       // not used
				}
								
				// search tables for codes of this length and values ...
				
				for (j=0; j<vHuffmanTableSize; ++j) {
					if (vHuffmanTableTripletNumberOfCodeBits[j] == codelengthtable[k]) {
						Uint16 code=vHuffmanTableTripletCode[j];
						Assert(code < numberofentries);
						Assert(huffmandecodetable[k][code].action == action_NEXTLENGTH);
						huffmandecodetable[k][code].action=action_VALUE;
						huffmandecodetable[k][code].value=vHuffmanTableTripletValue[j];
					}
				}
				
				// fill in table with escape value ...
				
				if (vEscapeTripletNumberOfEscapeCodeBits == codelengthtable[k]) {
					Uint16 code=vEscapeTripletEscapeCode;
					Assert(code < numberofentries);
					Assert(huffmandecodetable[k][code].action == action_NEXTLENGTH);
					huffmandecodetable[k][code].action=action_ESCAPE;
cerr << "setting escape huffmandecodetable[" << k << "][" << code << " ] action = " << huffmandecodetable[k][code].action << endl;
				}

			}
//#ifdef CRAP
			// dump table ...       
				
			for (k=0; k<numberofcodelengthsused; ++k) {
				unsigned numberofentries = 1<<codelengthtable[k];
				for (j=0; j<numberofentries; ++j) {
					cerr << "huffmandecodetable[" << k << "][" << j << " ] action = "
						<< huffmandecodetable[k][j].action 
						<< " value = " << huffmandecodetable[k][j].value
						<< endl;
				}
			}
//#endif
			// now let's try it ...
cerr << "Now let's try it ..." << endl;
			
			{
				initializeReadBit();

				Uint16 escapeValueSignBitMask = 1<<(useEscapeTripletNumberOfActualValueBits-1);
				Uint16 escapeValueSignExtendMask = (0xffff>>useEscapeTripletNumberOfActualValueBits)<<useEscapeTripletNumberOfActualValueBits;
cerr << "escapeValueSignBitMask = 0x" << hex << escapeValueSignBitMask << dec << endl;
cerr << "escapeValueSignExtendMask = 0x" << hex << escapeValueSignExtendMask << dec << endl;

				Uint16 valueMask = ~((0xffff>>vBitsStored)<<vBitsStored);
cerr << "valueMask = 0x" << hex << valueMask << dec << endl;

				Uint16 accumulatedvalue=0;
				Uint16 count_columns=0;
				Uint16 count_rows=0;

				Uint32 bits=0;
				unsigned index=0;
				Assert(index < numberofcodelengthsused);
				int needbits=codelengthtable[index];
				while (count_rows < vRows) {
				//while (true) {
//cerr << "index = " << index << endl;
					Assert(index < numberofcodelengthsused);
//cerr << "needbits = " << needbits << endl;
					Assert(needbits < sizeof(Uint32)*8);
					Uint32 nextbit;
					while (needbits-- > 0 && readBit(nextbit,list,log)) bits=(bits<<1) | nextbit;
					if (needbits != -1) break;
					
					Uint16 action=huffmandecodetable[index][bits].action;
					
//cerr << "huffmandecodetable[" << index << "][" << bits << " ] action = " << action << endl;
						
					if (action == action_NEXTLENGTH) {
						needbits=codelengthtable[index];			// what we already have
						++index;
						Assert(index < numberofcodelengthsused);
						needbits=codelengthtable[index]-needbits;       // what more we need (not total)
					}
					else if (action == action_ESCAPE) {
cerr << "escape" << endl;
						int bitsinescapevalue=useEscapeTripletNumberOfActualValueBits;
						Int16 escapevalue=0;
						while (bitsinescapevalue-- > 0 && readBit(nextbit,list,log)) escapevalue=(escapevalue<<1) | nextbit;
						Assert(bitsinescapevalue == -1);	// can't run out here
						
cerr << "[" << count_rows << "," << count_columns << "] before = " << accumulatedvalue << endl;
cerr << "escapevalue before sign extend = 0x" << hex << escapevalue << dec << "(" << escapevalue << ")" << endl;
//#define TWOSCOMPLEMENTESCAPE
#ifdef TWOSCOMPLEMENTESCAPE
						// need to sign extend escape value ...
						if (escapevalue & escapeValueSignBitMask) escapevalue|=escapeValueSignExtendMask;
cerr << "escapevalue after  sign extend = 0x" << hex << escapevalue << dec << "(" << escapevalue << ")" << endl;
						accumulatedvalue+=escapevalue;
						//accumulatedvalue=escapevalue;
#else
						if (escapevalue & escapeValueSignBitMask) {
cerr << "escapevalue after  sign removal = 0x" << hex << (escapevalue-escapeValueSignBitMask) << dec << "(" << (escapevalue-escapeValueSignBitMask) << ")" << endl;
							accumulatedvalue-=(escapevalue-escapeValueSignBitMask);
						}
						else
							accumulatedvalue+=escapevalue;
#endif						

cerr << "[" << count_rows << "," << count_columns << "] after = " << accumulatedvalue << endl;
						accumulatedvalue&=valueMask;
//cerr << "escapevalue = " << escapevalue << endl;
cerr << "[" << count_rows << "," << count_columns << "] masked = " << accumulatedvalue << endl;
						writeTheValue(out,accumulatedvalue);

						++count_columns;

						index=0;
						needbits=codelengthtable[index];
						bits=0;
					}
					else if (action == action_VALUE) {
//cerr << "bits = 0x" << hex << bits << dec << endl;
						accumulatedvalue+=huffmandecodetable[index][bits].value;
						//accumulatedvalue=huffmandecodetable[index][bits].value;
						accumulatedvalue&=valueMask;
//cerr << "value = " << huffmandecodetable[index][bits].value << endl;
cerr << "[" << count_rows << "," << count_columns << "] = " << accumulatedvalue << endl;
						writeTheValue(out,accumulatedvalue);

						++count_columns;

						index=0;
						needbits=codelengthtable[index];
						bits=0;
					}
					else {
						Assert(0);
					}

					if (count_columns >= vColumns) {
						count_columns=0;
						++count_rows;
#ifdef ZEROATSTARTOFROW
						accumulatedvalue=0;
#endif
					}
				}
			}
cerr << "Normal exit from loop" << endl;
		}
	}
	out.flush();
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 			options(argc,argv);
	DicomInputOptions 			dicom_input_options(options);
	BinaryOutputOptionsWithByteOrder	output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool quiet=options.get("quiet") || options.get("silent");

	dicom_input_options.done();
	output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);
	OutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< output_options.usage()
			<< " [-v|-verbose]"
			<< " [-quiet|-silent]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);
	ostream out(output_opener);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	getValuesOfAttributes(list,out,log,quiet);

#ifdef CRAP
	if (vNumberOfFrames > 1) {
		log << "Multiple frames not supported" << endl;
		success=false;
	}

	if (din.getTransferSyntaxToReadDataSet()->isEncapsulated()) {
		log << "Encapsulated (compressed) input data not supported" << endl;
		success=false;
	}

	if (vBitsAllocated > 8) {
		if (output_options.byteorder == ByteEndian || output_options.byteorder == NoEndian) {
			log << "Need to specify output byte order for > 8 bit data" << endl;
			success=false;
		}
		else if (din.getEndian() != output_options.byteorder) {
			log << "Need to specify same byte order as input for > 8 bit data" << endl;
			success=false;
		}
	}

	const char *pnmstring=0;

	if (vSamplesPerPixel == 1
	 && vPhotometricInterpretation
	 && (strcmp(vPhotometricInterpretation,"MONOCHROME1") == 0
	  || strcmp(vPhotometricInterpretation,"MONOCHROME2") == 0) ) {
		pnmstring="P5";
	}
	else if (vSamplesPerPixel == 3
		&& vPhotometricInterpretation && strcmp(vPhotometricInterpretation,"RGB") == 0
		&& vPlanarConfiguration == 0) {
		pnmstring="P6";
	}
	else {
		log << "Only greyscale and RGB interleaved images supported" << endl;
		success=false;
	}

	if (success) {
		Assert(pnmstring);
		out << pnmstring << "\n";
		out << vColumns << " " << vRows << "\n";
		out << ((vBitsAllocated > 8 && vBitsStored < 8) ? 256 : (1<<vBitsStored)-1) << "\n";
	}


	Attribute *apixeldata=list[TagFromName(PixelData)];
	if (!apixeldata) {
		log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;
		success=false;
	}
	else if (!apixeldata->isOtherData()) {
		log << EMsgDC(PixelDataIncorrectVR) << endl;
		success=false;
	}

	if (success) {
		OtherUnspecifiedLargeAttributeBase *
			opixeldata = apixeldata->castToOtherData();
		Assert(opixeldata);

		if (!quiet) log << "Writing ..." << endl;

		opixeldata->writeRaw(out);

		if (!out.good()) {
			log << EMsgDC(Writefailed) << endl;
			success=false;
		}
	}
#endif
	return success ? 0 : 1;
}

	
