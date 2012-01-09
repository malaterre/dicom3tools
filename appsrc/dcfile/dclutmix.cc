#include "attrtype.h"
#include "attrmxls.h"
#include "attrseq.h"
#include "attrothr.h"
#include "attrval.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

static Attribute *
isValuePresentElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a && a->getVM())
		return a;
	else {
		log << filename << ": " 
		    << EMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		return 0;
	}
}

static Uint16
getIntegerValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	Uint16 value=AttributeValue(a);
	return value;
}

class BurnInLUTFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	const Uint16 *data;
	Uint16 numberOfEntries;
	Uint16 firstValueMapped;
	Uint16 lastValueMapped;
	Uint16 firstValue;
	Uint16 lastValue;
public:
	BurnInLUTFilter(const Uint16 *data,Uint16 numberOfEntries,Uint16 firstValueMapped)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			this->data = data;
			this->numberOfEntries = numberOfEntries;
			this->firstValueMapped = firstValueMapped;
			lastValueMapped = firstValueMapped + numberOfEntries - 1;
			if (numberOfEntries > 0) {
				firstValue = data[0];
				lastValue = data[numberOfEntries-1];
			}
		}

	Uint16 filter(Uint16 originalvalue)
		{
			Uint16 newValue;
			if (originalvalue < firstValueMapped) {
				newValue = firstValue;
			}
			else if (originalvalue > lastValueMapped) {
				newValue = lastValue;
			}
			else {
				newValue = data[originalvalue-firstValueMapped];
			}
			return newValue;
		}
};

static SequenceAttribute *
addVOILUT(Uint32 numberofentries,
	Uint16 firstvaluemapped,
	Uint16 bitsallocated,
	Uint16 *values,
	const char *explanation,
	TextOutputStream &log)
{
	Assert(bitsallocated <= 16);
	Assert(numberofentries <= 65536);
	Assert(values);

	Attribute *aLUTDescriptor = new UnsignedShortAttribute(TagFromName(LUTDescriptor));
	Assert(aLUTDescriptor);
	aLUTDescriptor->addValue(numberofentries == 65536 ? 0 : numberofentries);
	aLUTDescriptor->addValue(firstvaluemapped);
	aLUTDescriptor->addValue(bitsallocated);

	Attribute *aLUTData = new OtherWordSmallNonPixelAttribute(TagFromName(LUTData));
	Assert(aLUTData);
	aLUTData->setValue(values,numberofentries);

	SequenceAttribute *aLUTSequence = new SequenceAttribute(TagFromName(VOILUTSequence));
	Assert(aLUTSequence);

	AttributeList *lLUTSequence = new AttributeList();
	Assert(lLUTSequence);

	(*lLUTSequence)+=aLUTData;
	(*lLUTSequence)+=aLUTDescriptor;
	(*lLUTSequence)+=new LongStringAttribute(TagFromName(LUTExplanation),explanation ? explanation : "");

	(*aLUTSequence)+=lLUTSequence;

	return aLUTSequence;
}

static Uint16 scramble(Uint16 value,int significantBits) {
	// just reverse all bits
	Uint16 newValue=0;
	int i;
	for (i=0; i<significantBits; ++i) {
		newValue = (newValue<<1) | (value&0x0001);
		value = value >> 1;
	}
	return newValue;
}

static Uint16 invert(Uint16 value,int significantBits) {
	return (~value)&((Uint32(1)<<significantBits)-1);
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool bad = false;
	
	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");
	
	bool useScramble=options.get("scramble");
	bool useInvert=options.get("invert");
	bool useIdentity=options.get("identity");
	
	if (!useScramble && !useInvert) {
		bad = true;
	}

	dicom_input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " -scramble|invert|identity"
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
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
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!list.good()) {
		if (!ignorereaderrors) {
			log << list.errors();
			success=false;
		}
		log << EMsgDC(DatasetReadFailed) << endl;
	}
	
	Uint16 vBitsStored = getIntegerValueElseError(list,TagFromName(BitsStored),"Bits Stored","",log);
	Uint16 nLUTEntriesNeeded = 1 << vBitsStored;
	
	Uint16 *forwardLUT = new Uint16[nLUTEntriesNeeded];
	Uint16 *inverseLUT = new Uint16[nLUTEntriesNeeded];
	
	int i;
	for (i=0; i<nLUTEntriesNeeded; ++i) {
		Uint16 newValue;
		if (useScramble) {
			newValue = scramble(i,vBitsStored);
		}
		else if (useInvert) {
			newValue = invert(i,vBitsStored);
		}
		else if (useIdentity) {
			newValue = i;
		}
		else {
			Assert(0);
		}
cerr << hex << i << " -> " << newValue << dec << endl;
		forwardLUT[i] = newValue;
		inverseLUT[newValue] = i;
	}
	
	list-=TagFromName(VOILUTSequence);
	list+=addVOILUT(nLUTEntriesNeeded,0,vBitsStored,inverseLUT,"unscramble",log);
	
	Uint16 VOILUT_first = 0;
	Uint16 VOILUT_number = nLUTEntriesNeeded;
	Uint16 VOILUT_depth = vBitsStored;
	const Uint16 *vLUTData = forwardLUT;

	if (success && vLUTData != NULL && VOILUT_number > 0 && VOILUT_depth > 0) {
		BurnInLUTFilter *filter = new BurnInLUTFilter(vLUTData,VOILUT_number,VOILUT_first);
		Assert(filter);
	
		Attribute *aPixelData = list[TagFromName(PixelData)];

		if (aPixelData) {
			OtherUnspecifiedLargeAttributeBase *oPixelData = 0;

			if (!aPixelData->isOtherData()) {
				log << EMsgDC(PixelDataIncorrectVR) << endl;
				success=false;
			}
			else {
				oPixelData = aPixelData->castToOtherData();
				Assert(oPixelData);
				if (filter) oPixelData->insertPixelPointTransform(filter);
			}
		}
		else {
			log << EMsgDC(MissingAttribute)
			    << " - \"PixelData\""
			    << endl;
			success=false;
		}
		
		// Don't write list warnings yet ...
		// done in usualManagedAttributeListWrite ...

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	
