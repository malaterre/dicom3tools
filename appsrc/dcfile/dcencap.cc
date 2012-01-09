#include "attrmxls.h"
#include "mesgtext.h"
#include "getoptns.h"
#include "ioopt.h"
#include "dcopt.h"
#include "elmconst.h"
#include "attrval.h"

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options1(options);
	InputOptions		input_options2(options);
	DicomOutputOptions 	dicom_output_options(options);

	unsigned itemSize=0;
	(void)options.get("itemsize",itemSize);
	if (itemSize%2 != 0) {
		cerr << EMsgDC("Item size must be even length") << endl;
		bad=true;
	}
	
	bool singleitem = options.get("singleitem");
	if (singleitem) {
		if (itemSize != 0) {
			cerr << EMsgDC("Cannot specify -itemsize if -singleitem") << endl;
			bad=true;
		}
	}
	
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool verbose=options.get("verbose") || options.get("v") || veryverbose;

	bool writeListPreceedingPixelData = true;
	bool writePixelDataAttributeBase = true;
	bool writePixelDataFragments = true;
	bool writePixelDataAttributeSequenceDelimiter = true;

	dicom_input_options1.done();
	input_options2.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener1(
		options,dicom_input_options1.filename);
	InputOpenerFromOptions input_opener2(
		options,input_options2.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options1.errors();
	cerr << input_options2.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener1.errors();
	cerr << input_opener2.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options1.good()
	 || !input_options2.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener1.good()
	 || !input_opener2.good()
	 || !output_opener.good()
	 || !options
	|| bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options1.usage()
			<< input_options2.usage()
			<< dicom_output_options.usage()
			<< " [-itemsize nbytes|-singleitem]"
			<< " [-v|-verbose|-vv|-veryverbose]"
			<< MMsgDC(InputFile) << " " << MMsgDC(InputFile)
			<< "[" << MMsgDC(OutputFile) << "]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din1(*(istream *)input_opener1,
		dicom_input_options1.transfersyntaxuid,
		dicom_input_options1.usemetaheader);
	istream in2(input_opener2);
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream  log(cerr);
	if (veryverbose) log << "******** While reading DICOM dataset ... ********" << endl; 
	list.read(din1,&log,veryverbose,0xffffffff,true,dicom_input_options1.uselengthtoend,dicom_input_options1.ignoreoutofordertags);
	
	if (!list.good()) {
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	else {
		if (writeListPreceedingPixelData) {
			if (!usualManagedAttributeListWrite(list,dout,
				dicom_output_options,log,veryverbose)) success=false;
		}
		if (writePixelDataAttributeBase) {
			// now construct and write an undefined length PixelData attribute "manually"
		
			dout << Uint16(0x7fe0);
			dout << Uint16(0x0010);
			if (dout.getTransferSyntaxInUse()->isExplicitVR()) {
				dout << 'O';
				dout << 'B';
				dout << Uint16(0);
			}
			dout << Uint32(0xffffffffl);	// VL
		
			// don't forget the empty item offset table "item" ...
		
			dout << Uint16(0xfffe);
			dout << Uint16(0xe000);
			dout << Uint32(0);		// VL
		}
		if (writePixelDataFragments) {		
			Uint16 numberOfFrames=1;
			Uint32 frameSize=0;
			{
				Attribute *aNumberOfFrames = list[TagFromName(NumberOfFrames)];
				if (aNumberOfFrames) {
					numberOfFrames=AttributeValue(aNumberOfFrames);
				}
			
				Uint16 vRows = 0;
				Attribute *aRows = list[TagFromName(Rows)];
				if (!aRows) {
					log << WMsgDC(MissingAttribute)
					<< " - \"Rows\""
					<< endl;
				}
				else {
					vRows=AttributeValue(aRows);
				}
				Uint16 vColumns = 0;
				Attribute *aColumns = list[TagFromName(Columns)];
				if (!aColumns) {
					log << WMsgDC(MissingAttribute)
					<< " - \"Columns\""
					<< endl;
				}
				else {
					vColumns=AttributeValue(aColumns);
				}
				Uint16 vBitsAllocated = 0;
				Attribute *aBitsAllocated = list[TagFromName(BitsAllocated)];
				if (!aBitsAllocated) {
					log << WMsgDC(MissingAttribute)
					<< " - \"BitsAllocated\""
					<< endl;
				}
				else {
					vBitsAllocated=AttributeValue(aBitsAllocated);
				}
				Uint16 vSamplesPerPixel = 1;
				Attribute *aSamplesPerPixel = list[TagFromName(SamplesPerPixel)];
				if (!aSamplesPerPixel) {
					log << WMsgDC(MissingAttribute)
					<< " - \"SamplesPerPixel\""
					<< endl;
				}
				else {
					vSamplesPerPixel=AttributeValue(aSamplesPerPixel);
				}
				frameSize=((Uint32)vRows)*vColumns*vSamplesPerPixel*((vBitsAllocated-1)/8+1);
			}
			if (frameSize == 0) {
				log << WMsgDC("Cannot compute frame size from Image Pixel Module attributes") << endl;
				frameSize=0xFFFFFFFE;
			}
			{
				if (verbose) {
					log << "Using frameSize " << dec << frameSize << endl; 
				}
				if (singleitem) {
					itemSize=numberOfFrames*frameSize;
					if (itemSize%2 != 0) ++itemSize;
					if (itemSize > 0xFFFFFFFE) itemSize=0xFFFFFFFE;	// maximum length of defined length item
				}
				else {
					if (itemSize == 0) {
						itemSize=(unsigned)frameSize;
						if (itemSize%2 != 0) ++itemSize;
						if (itemSize > 0xFFFFFFFE) itemSize=0xFFFFFFFE;	// maximum length of defined length item
					}
				}
				if (verbose) {
					log << "Using itemSize " << dec << itemSize << endl; 
				}
				unsigned long total=0;
				char *buffer = new char[itemSize];
				Assert(buffer);
				if (singleitem) {
					if (verbose) {
						log << "Doing single item " << endl; 
					}
					in2.read(buffer,itemSize);
					int bytesRead=in2.gcount();
					if (bytesRead > 0) {
						int bytesWritten = bytesRead;
						if (bytesWritten%2 != 0) {
							if (verbose) {
								log << "Padding odd length fragment" << endl; 
							}
							++bytesWritten;
							Assert(bytesWritten <= itemSize);
							buffer[bytesRead]=(char)0xff;	// pad character is 0xff for JPEG etc, after EOI
						}
						// write Item
						dout << Uint16(0xfffe);
						dout << Uint16(0xe000);
						dout << Uint32(bytesWritten);		// VL
						dout.write(buffer,bytesWritten);
						total+=bytesWritten;
					}
				}
				else {
					unsigned f;
					for (f=0; f < numberOfFrames; ++f) {
						if (verbose) {
							log << "Doing frame " << dec << f << endl; 
						}
						Uint32 bytesLeftInFrame=frameSize; 
						while (bytesLeftInFrame > 0) {
							unsigned bytesThisItem = bytesLeftInFrame > itemSize ? itemSize : (unsigned)bytesLeftInFrame;
							if (in2.eof()) {
								// This is not an error if data was compressed; we just reached the end of it ...
								//log << EMsgDC("Premature EOF - giving up") << endl;
								//success=false;
								bytesLeftInFrame=0;
								f=numberOfFrames;
							}
							else {
								in2.read(buffer,bytesThisItem);
								int bytesRead=in2.gcount();
								if (verbose) {
									log << "Wanted " << dec << bytesThisItem << " read " << bytesRead << endl; 
								}
								if (bytesRead > 0) {
									int bytesWritten = bytesRead;
									if (bytesWritten%2 != 0) {
										if (verbose) {
											log << "Padding odd length fragment" << endl; 
										}
										++bytesWritten;
										Assert(bytesWritten <= itemSize);
										buffer[bytesRead]=(char)0xff;	// pad character is 0xff for JPEG etc, after EOI
									}
									// write Item
									dout << Uint16(0xfffe);
									dout << Uint16(0xe000);
									dout << Uint32(bytesWritten);		// VL
									dout.write(buffer,bytesWritten);
									bytesLeftInFrame-=bytesRead;
									Assert(bytesLeftInFrame == 0 || bytesWritten%2 == 0);	// i.e. didn't pad in middle of item
									total+=bytesWritten;
								}
								else {
									// This is not an error if data was compressed; we just reached the end of it ...
									//log << EMsgDC("Read returns zero length - giving up") << endl;
									//success=false;
									bytesLeftInFrame=0;
									f=numberOfFrames;
								}
							}
						}
					}
				}
				if (verbose) {
					log << "Wrote total of " << dec << total << endl; 
				}
			}
			if (writePixelDataAttributeSequenceDelimiter) {
				dout << Uint16(0xfffe);
				dout << Uint16(0xe0dd);
				dout << Uint32(0);	// VL
			}
			dout.flush();
		}
	}

	return success ? 0 : 1;
}

	
