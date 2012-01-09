#include "attrmxls.h"
#include "attrval.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

#include "errclass.h"

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool showfilename=options.get("filename");

	unsigned input_min = 0x0000;
	unsigned input_max = 0xffff;
	bool minpresent=options.get("min",input_min);
	bool maxpresent=options.get("max",input_max);

	bool ancreate=options.get("ancreate");
	
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
			<< " [-ancreate]"
			<< " [-min value]"
			<< " [-max value]"
			<< " [-v|-verbose]"
			<< " [-filename]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		return 1;
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList list;

	TextOutputStream  log(cerr);
	
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
		//return 1;	// Press on regardless ...
	}


	Attribute *aVOILUTSequence = list[TagFromName(VOILUTSequence)];
	int nVOILUT=0;
	if (aVOILUTSequence) {
		if (ancreate) {
			log << "(0x0028,0x3010) SQ VOI LUT Sequence VR=<SQ> VL=<0xffffffff> []" << endl;
		}
		else {
			log << "VOILUTSequence" << endl;
		}
		AttributeList **items;
		nVOILUT=aVOILUTSequence->getLists(&items);
		if (!ancreate) {
			log << "Contains " << dec << nVOILUT << " items" << endl;
		}
		int item;
		for (item=0; item<nVOILUT; ++item) {
			if (ancreate) {
				log << "%item" << endl;
			}
			else {
				log << "\t Item " << dec << item << endl;
			}
			Assert(items[item]);
			Attribute *aLUTExplanation = items[item]->operator[](TagFromName(LUTExplanation));
			if (aLUTExplanation) {
				const char *vLUTExplanation=AttributeValue(aLUTExplanation);
				if (!ancreate) {
					log << "\t\t LUTExplanation  = " << vLUTExplanation << endl;
				}
			}
			
			Attribute *aLUTDescriptor=items[item]->operator[](TagFromName(LUTDescriptor));
			if (!aLUTDescriptor) {
				log << "\t\t Missing LUT Descriptor Attribute" << endl;
			}
			else {
				if (ancreate) {
					aLUTDescriptor->write(log,list.getDictionary(),false);
					log << endl;
					aLUTExplanation->write(log,list.getDictionary(),false);			// must go after descriptor
					log << endl;
				}
				unsigned vm=aLUTDescriptor->getVM();
				if (vm != 3) {
					log << "\t\t Wrong VM for LUT Descriptor = " << dec << vm << endl;
				}
				else {
					Uint16 VOILUT_first;
					Uint16 VOILUT_number;
					Uint16 VOILUT_depth;
					Assert(aLUTDescriptor->getValue(0,VOILUT_number));
					Assert(aLUTDescriptor->getValue(1,VOILUT_first));
					Assert(aLUTDescriptor->getValue(2,VOILUT_depth));
					if (!ancreate) {
						log << "\t\t first  = " << dec << VOILUT_first << endl
							<< "\t\t number = " << dec << VOILUT_number << endl
							<< "\t\t depth  = " << dec << VOILUT_depth << endl;
					}
					Attribute *aLUTData=items[item]->operator[](TagFromName(LUTData));
					if (!aLUTData) {
						log << "\t\t Missing LUT Data Attribute" << endl;
					}
					else {
						if (ancreate) {
							//aLUTData->write(log,list.getDictionary(),false);
							//log << endl;
							log << "(0x0028,0x3006) XO LUT Data      VR=<OW>   VL=<" << hex << (VOILUT_number*2) << dec << "> [";
						}
						//else {
						{
							const char *prefix = "";
							int j;
							Uint16 VOILUT_value;
							Uint32 nLUTData;
							if (aLUTData->isOtherWordNonPixel()) {
//cerr << "checking > 8 bit OW" << endl;
								const Uint16 *vLUTData = NULL;
								if (aLUTData->getValue(vLUTData,nLUTData)) {
//cerr << "got value" << endl;
									if (!ancreate) {
										if (minpresent && VOILUT_number && nLUTData) {
											for (j=input_min; j < VOILUT_first; ++j) {
												cout << dec << j << "\t" << VOILUT_value << endl;
											}
										}
									}
									int i;
									for (i=0,j=VOILUT_first; i < VOILUT_number; ++i,++j) {
										VOILUT_value = vLUTData[i];
										if (ancreate) {
											log << prefix << hex << VOILUT_value << dec << "";
											prefix=",";
										}
										else {
											if (j >= input_min && j <= input_max) {
												cout << dec << j << "\t" << VOILUT_value << endl;
											}
										}
									}
								}
								else {
									log << "\t\t Cannot get OW LUT Data" << endl;
								}
							}
							else if (aLUTData->isNumericBinary()) {		// e.g. US VR
//cerr << "checking " << aLUTData->getVR() << endl;
								nLUTData = aLUTData->getVM();
								if (minpresent && VOILUT_number && nLUTData && aLUTData->getValue(0,VOILUT_value)) {
									if (!ancreate) {
										for (j=input_min; j < VOILUT_first; ++j) {
											cout << dec << j << "\t" << VOILUT_value << endl;
										}
									}
								}
								int i;
								for (i=0,j=VOILUT_first; i < VOILUT_number; ++i,++j) {
									if (aLUTData->getValue(i,VOILUT_value)) {
										if (ancreate) {
											log << prefix << hex << VOILUT_value << dec << "";
											prefix=",";
										}
										else {
											if (j >= input_min && j <= input_max) {
												cout << dec << j << "\t" << VOILUT_value << endl;
											}
										}
									}
									else {
										log << "\t\t Premature End of LUT Data" << endl;
										break;
									}
								}
							}
							// at this point, for either encoding, VOILUT_value will contain the last value
							if (!ancreate) {
								if (maxpresent && VOILUT_number) {
									for (j=VOILUT_first+VOILUT_number; j <= input_max; ++j) {
										cout << dec << j << "\t" << VOILUT_value << endl;
									}
								}
							}
						}
						if (ancreate) {
							log << "]" << endl;
						}
					}
				}
			}
			if (ancreate) {
				log << "%enditem" << endl;
			}
		}
		if (ancreate) {
			log << "%endseq" << endl;
		}
	}

	return 0;
}

