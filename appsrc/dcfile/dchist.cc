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
dumpImageHistogram(ManagedAttributeList& list,TextOutputStream& log,bool showhist,bool showcsv,bool withHighBits,bool highBitsOnly)
{
	bool success;

	Attribute *aBitsStored = list[TagFromName(BitsStored)];
	Uint16 vBitsStored=AttributeValue(aBitsStored);

	Attribute *aBitsAllocated = list[TagFromName(BitsAllocated)];
	Uint16 vBitsAllocated=AttributeValue(aBitsAllocated);

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
		SupplySourceFromAttribute sPixelData(aPixelData,withHighBits || highBitsOnly);
		class SourceBase<Uint16> *source = sPixelData.getSource();
		if (!source) {
			log << EMsgDC(BadAttributeValue)
			    << " - \"PixelData\""
			    << endl;
			success=false;
		}
		else {
			int bitsUsed;
			Uint16 maskLowBits = 0;
			if (withHighBits) {
				bitsUsed = vBitsAllocated;
			}
			else if (highBitsOnly) {
				bitsUsed = vBitsAllocated;
				int i;
				for (i=0; i<vBitsStored; ++i) {
					maskLowBits = (maskLowBits<<1) | 1;
				}
			}
			else {
				bitsUsed = vBitsStored;
			}
			maskLowBits = (~maskLowBits)&0xffff;
			Uint32 largestsymbolplusone = ((Uint32)1) << (bitsUsed ? bitsUsed : 16);
			Uint32 *histogram = new Uint32[largestsymbolplusone];
			Assert(histogram);
			{
				Uint32 i;
				Uint32 *histptr;
				for (i=0,histptr=histogram; i<largestsymbolplusone; ++i) *histptr++=0;
			}
			Uint32 count=0;
			Uint16 smallestsymbol=0xffff;
			Uint16 largestsymbol=0;

			unsigned row=0;
			unsigned column=0;
			while (row < vRows*vNumberOfFrames && source->read()) {
				size_t n=source->getBufferCount();
				Assert(n);
				const Uint16 *buffer=source->getBuffer();
				Assert(buffer);
				while (n-- && row < vRows*vNumberOfFrames) {
					Uint16 symbol=*buffer++;
					symbol = symbol & maskLowBits;

					Assert(symbol<largestsymbolplusone);
					Assert(histogram[symbol] < 0xffffffff);
					++histogram[symbol];
					++count;

					if (symbol < smallestsymbol) smallestsymbol=symbol;
					if (symbol > largestsymbol) largestsymbol=symbol;

					if (++column >= vColumns) {
						column=0;
						++row;
					}
				}
			}
			//Assert(column == 0 && row == vRows*vNumberOfFrames);

			Assert(count == vColumns*vRows*vNumberOfFrames);
			Assert(count);

			double entropy;
			{
				Uint32 i;
				Uint32 *histptr;
				for (i=smallestsymbol,histptr=histogram+smallestsymbol,entropy=0; i<=largestsymbol; ++i,++histptr) {
					if (*histptr) {
						double probsymbol=double(*histptr)/count;
						double contributes=-probsymbol*ourlog2(probsymbol);
						entropy+=contributes;
						if (showhist) {
							log << "[" << hex << i << "]\t"
							    << dec << *histptr << "\t"
							    << "p=" << dec << probsymbol << "\t"
							    << "e=" << dec << contributes << "\t"
							    << "cum=" << dec << entropy << endl;
						}
						if (showcsv) {
							log << dec << i << "," << *histptr << endl;
						}
					}
				}
			}

			if (showhist) log << endl;

			if (!showcsv) {
				log << dec << entropy << "\tZero order entropy (bits per pixel)" << endl;
				log << hex << smallestsymbol << "\tSmallest symbol value" << endl;
				log << dec << howManySignificantBits(smallestsymbol) << "\tSmallest symbol length (bits)" << endl;
				log << hex << largestsymbol << "\tLargest symbol value" << endl;
				int mostSignificantBits = howManySignificantBits(largestsymbol);
				log << dec << mostSignificantBits << "\tLargest symbol length (bits)" << endl;
				if (mostSignificantBits > vBitsStored) {
					log << "ALERT\tEmbedded overlay bits or sign extension bits present above Bits Stored" << endl;
				}
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
	bool showhist=options.get("histogram") || options.get("h");
	bool showcsv=options.get("csv");
	bool withHighBits=options.get("withhighbits");
	bool highBitsOnly=options.get("highbitsonly");
	
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
			<< " [-h|-histogram]"
			<< " [-csv]"
			<< " [-v|-verbose]"
			<< " [-withhighbits|-highbitsonly]"
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
		log << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
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
		success=dumpImageHistogram(list,log,showhist,showcsv,withHighBits,highBitsOnly);
	}

	return success ? 0 : 1;
}

