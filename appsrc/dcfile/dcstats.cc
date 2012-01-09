#include "attrmxls.h"
#include "attrothr.h"
#include "attrval.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "dcopt.h"

static unsigned
howManySignificantBits(Uint16 test)
{
	unsigned bits;
	for (bits=0; test; test=test>>1,++bits);
	return bits;
}

static bool
dumpImageStatistics(ManagedAttributeList& list,TextOutputStream& log)
{
	bool success;

	Attribute *aBitsStored = list[TagFromName(BitsStored)];
	Uint16 vBitsStored=AttributeValue(aBitsStored);
	Attribute *aPixelRepresentation = list[TagFromName(PixelRepresentation)];
	Uint16 vPixelRepresentation=AttributeValue(aPixelRepresentation);
	Attribute *aLargestImagePixelValue = list[TagFromName(LargestImagePixelValue)];
	Uint16 vLargestImagePixelValue=AttributeValue(aLargestImagePixelValue);
	Attribute *aSmallestImagePixelValue = list[TagFromName(SmallestImagePixelValue)];
	Uint16 vSmallestImagePixelValue=AttributeValue(aSmallestImagePixelValue);

	Attribute *aRows = list[TagFromName(Rows)];
	if (!aRows)
		log << EMsgDC(MissingAttribute) << " - \"Rows\"" << endl;
	Uint16 vRows=AttributeValue(aRows);

	Attribute *aColumns = list[TagFromName(Columns)];
	if (!aColumns)
		log << EMsgDC(MissingAttribute) << " - \"Columns\"" << endl;
	Uint16 vColumns=AttributeValue(aColumns);

	Attribute *aNumberOfFrames = list[TagFromName(NumberOfFrames)];
	Uint16 vNumberOfFrames=AttributeValue(aNumberOfFrames,1);

	Attribute *aPixelData = list[TagFromName(PixelData)];
	if (!aPixelData)
		log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;

	if (!aPixelData || !aRows || !aColumns) {
		success=false;
	}
	else {
		SupplySourceFromAttribute sPixelData(aPixelData);
		class SourceBase<Uint16> *source = sPixelData.getSource();
		if (!source) {
			log << EMsgDC(BadAttributeValue)
			    << " - \"PixelData\""
			    << endl;
			success=false;
		}
		else {
			Int16  signedminval=Int16_MAX;
			Int16  signedmaxval=Int16_MIN;
			Uint16 unsignedminval=Uint16_MAX;
			Uint16 unsignedmaxval=0;
			Int32  signedtotal=0;
			Uint32 unsignedtotal=0;

			unsigned row=0;
			unsigned column=0;
			while (row < vRows*vNumberOfFrames && source->read()) {
				size_t n=source->getBufferCount();
				Assert(n);
				const Uint16 *buffer=source->getBuffer();
				Assert(buffer);
				while (n-- && row < vRows*vNumberOfFrames) {
					Uint16 value=*buffer++;
					if (Int16(value) < signedminval) signedminval=value;
					if (Int16(value) > signedmaxval) signedmaxval=value;
					if (value < unsignedminval) unsignedminval=value;
					if (value > unsignedmaxval) unsignedmaxval=value;
					unsignedtotal+=value;
					signedtotal+=Int16(value);
					if (++column >= vColumns) {
						column=0;
						++row;
					}
				}
			}
			//Assert(column == 0 && row == vRows*vNumberOfFrames);

			log << "Signed minimum value = " << hex << Uint16(signedminval) << dec << "\t(" << Uint16(signedminval) << " dec)" << endl;
			log << "Signed maximum value = " << hex << Uint16(signedmaxval) << dec << "\t(" << Uint16(signedmaxval) << " dec)" << endl;
			log << "Unsigned minimum value = " << hex << unsignedminval << dec << "\t(" << unsignedminval << " dec)" << endl;
			log << "Unsigned maximum value = " << hex << unsignedmaxval << dec << "\t(" << unsignedmaxval << " dec)" << endl;

			Uint16 signedrange=Uint16(signedmaxval-signedminval);
			Uint16 unsignedrange=unsignedmaxval-unsignedminval;

			log << "Signed range = "   << hex << signedrange   << dec << "\t(" << signedrange   << " dec)" << endl;
			log << "Unsigned range = " << hex << unsignedrange << dec << "\t(" << unsignedrange << " dec)" << endl;

			Int16 signedmean=Int16(signedtotal/(vColumns*vRows*vNumberOfFrames));
			Uint16 unsignedmean=Uint16(unsignedtotal/(vColumns*vRows*vNumberOfFrames));

			double signedmeanpercentofrange=signedrange ? ((signedmean-signedminval)*100.0/signedrange) : 0;
			double unsignedmeanpercentofrange=unsignedrange ? ((unsignedmean-unsignedminval)*100.0/unsignedrange) : 0;

			log << "Signed mean = "   << hex << Uint16(signedmean) << dec << "\t(" << Uint16(signedmean) << " dec)" << "\t(" << signedmeanpercentofrange   << "%)" << endl;
			log << "Unsigned mean = " << hex << unsignedmean       << dec << "\t(" << unsignedmean       << " dec)" << "\t(" << unsignedmeanpercentofrange << "%)" << endl;

			log << "Bits Stored = " << dec << vBitsStored << endl;
			if (signedrange < unsignedrange) {
				if (vPixelRepresentation == 0) {
					log << WMsgDC(PixelRepresentationIncorrect)
					    << " - " << MMsgDC(ShouldBe) << " " << MMsgDC(Signed)
					    << endl;
				}
				log << "Significant positive bits = " << dec << howManySignificantBits(signedmaxval) << endl;
				log << "Significant negative bits = " << dec << howManySignificantBits(-signedminval) << endl;
				log << "Offset to make unsigned in hex is = " << hex << Uint16(-signedminval) << endl;
				log << "Offset to make unsigned in dec is = " << dec << Uint16(-signedminval) << endl;
			}
			else if (signedrange > unsignedrange) {
				if (vPixelRepresentation == 1) {
					log << WMsgDC(PixelRepresentationIncorrect)
					    << " - " << MMsgDC(ShouldBe) << " " << MMsgDC(Unsigned)
					    << endl;
				}
				log << "Significant unsigned bits = " << dec << howManySignificantBits(unsignedmaxval) << endl;
			}
			else {
				if (vPixelRepresentation == 1) {
					log << WMsgDC(PixelRepresentationUnnecessary)
					    << " - " << MMsgDC(CouldBe) << " " << MMsgDC(Unsigned)
					    << endl;
				}
				log << "Significant unsigned bits = " << dec << howManySignificantBits(unsignedmaxval) << endl;
			}
			success=true;
		}
	}
	return success;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool showfilename=options.get("filename");

	dicom_input_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);

	cerr << dicom_input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!dicom_input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< " [-v|-verbose]"
			<< " [-filename]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	
	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		cerr << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}

	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		success=dumpImageStatistics(list,log);
	}

	return success ? 0 : 1;
}

