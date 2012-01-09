#include "attrmxls.h"
#include "attrothr.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

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

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	int which_voilut = 0;		// 0 is first VOI lut; use by default
	options.get("voilutnumber",which_voilut);

	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");

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
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
			<< " [-voilutnumber 0..n-1]"
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

	Uint16 VOILUT_first = 0;
	Uint16 VOILUT_number = 0;
	Uint16 VOILUT_depth = 0;
	const Uint16 *vLUTData = NULL;
	if (success) {
//cerr << "Extracting LUT ..." << endl;
		Attribute *aVOILUTSequence = list[TagFromName(VOILUTSequence)];
		int nVOILUT=0;
		if (aVOILUTSequence) {
//cerr << "Got VOILUTSequence" << endl;
			AttributeList **items;
			nVOILUT=aVOILUTSequence->getLists(&items);
			if (which_voilut >= 0 && which_voilut < nVOILUT) {
				Assert(items[which_voilut]);
				Attribute *aLUTDescriptor=items[which_voilut]->operator[](TagFromName(LUTDescriptor));
				if (!aLUTDescriptor) {
					log << EMsgDC("Missing LUT Descriptor Attribute") << endl;
					success=false;
				}
				else {
					unsigned vm=aLUTDescriptor->getVM();
					if (vm != 3) {
						log << EMsgDC("Wrong VM for LUT Descriptor") << " = " << dec << vm << endl;
						success=false;
					}
					else {
						Assert(aLUTDescriptor->getValue(0,VOILUT_number));
						Assert(aLUTDescriptor->getValue(1,VOILUT_first));
						Assert(aLUTDescriptor->getValue(2,VOILUT_depth));
						Attribute *aLUTData=items[which_voilut]->operator[](TagFromName(LUTData));
						if (!aLUTData) {
							log << EMsgDC("Missing LUT Data Attribute") << endl;
							success=false;
						}
						else {
							int j;
							Uint16 VOILUT_value;
							Uint32 nLUTData;
							if (aLUTData->isOtherWordNonPixel()) {
								if (!aLUTData->getValue(vLUTData,nLUTData)) {
									log << EMsgDC("Cannot get OW LUT Data") << endl;
									success=false;
								}
							}
							else if (aLUTData->isNumericBinary()) {		// e.g. US VR
								nLUTData = aLUTData->getVM();
								Uint16 *mutableData = new Uint16[nLUTData];
								vLUTData = mutableData;
								int i;
								for (i=0; i < VOILUT_number; ++i) {
									Uint16 VOILUT_value;
									if (!aLUTData->getValue(i,VOILUT_value)) {
										log << EMsgDC("Premature End of LUT Data") << endl;
										success=false;
										break;
									}
									mutableData[i] = VOILUT_value;
								}
							}
						}
					}
				}
			}
			else {
				log << EMsgDC("Selected VOI LUT Sequence Item missing") << " - requested " << dec << which_voilut << " but only " << nVOILUT << " available" << endl;
			}
		}
		else {
			log << EMsgDC("Missing VOI LUT Sequence") << endl;
		}
	}

	if (success && vLUTData != NULL && VOILUT_number > 0 && VOILUT_depth > 0) {
//cerr << "Performing burn ..." << endl;
	
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
				list-=TagFromName(VOILUTSequence);
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

	
