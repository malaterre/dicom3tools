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

#ifndef __Header_MemSrc__
#define __Header_MemSrc__

#include "srcsink.h"

class Memory_PixelDataSource : public SourceBase<Uint16> {
protected:
	signed rows;
	Uint16 columns;

	signed row;
	Uint16 *buffer;
	Uint16 *bufptr;
public:
	Memory_PixelDataSource(Uint16 *srcbuffer,Uint16 r,Uint16 c)
			: SourceBase<Uint16>()
		{
			buffer=srcbuffer;	// reference not copy
			Assert(buffer);
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);

			bufptr=buffer-columns;	// position BEFORE buffer, since 1st "read" increments it
			row=-1;
		}

	~Memory_PixelDataSource()
		{
			// don't delete buffer ... responsibility of caller of constructor
		}

	size_t read(void)
		{
			bufptr+=columns;
			++row;
			return columns;
		}

	const Uint16 *getBuffer(void)
		{
			Assert (bufptr >= buffer);	// Just in case someone calls getBuffer() without read()
//cerr << "Memory_PixelDataSource::getBuffer: row = " << dec << row << endl;
			return bufptr;
		}

	size_t getBufferCount(void) const	{ return columns; }

	int good(void) const	{ return row < rows; }
};


#endif // __Header_MemSrc__

enum ProjectionType { MIP = 1, SUM, FUZZY };

class ImageFromProjectedFrames {
private:
	Uint16 *projectedfulldepthimage;

	Memory_PixelDataSource *resultsource;

	SupplySource *supplysource;

	Uint16 columns;
	Uint16 rows;
	Uint16 frames;
	Uint16 bitsallocated;
	Uint16 bitsstored;
	Uint16 highbit;
	bool   issigned;

	ProjectionType projectiontype;

	class SourceBase<Uint16> *getSourceSource(void)
		{
			Assert(supplysource);
			return supplysource->getSource();
		}


public:
	ImageFromProjectedFrames(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		Uint16 vPixelRepresentation,
		ProjectionType t)
		{
			projectedfulldepthimage=0;
			resultsource=0;

			supplysource=s;
			columns=vColumns;
			rows=vRows;
			frames=vNumberOfFrames;
			bitsallocated=vBitsAllocated;
			bitsstored=vBitsStored;
			highbit=vHighBit;
			issigned=vPixelRepresentation;

			projectiontype=t;

			Assert(supplysource);
			Assert(columns);
			Assert(rows);
			Assert(frames);
			Assert(bitsallocated);
			Assert(bitsstored);
			Assert(highbit);
		}

	~ImageFromProjectedFrames()
		{
			if (projectedfulldepthimage) delete[] projectedfulldepthimage;
			if (resultsource) delete resultsource;
		}

	bool createProjectedFullDepthImageFromSourceFrames(void);

	class SourceBase<Uint16> *getResultSource(void)
		{
			if (!resultsource) {
				Assert(projectedfulldepthimage);
				resultsource=new Memory_PixelDataSource(projectedfulldepthimage,rows,columns);
			}
			Assert(resultsource);
			return resultsource;
		}
};

bool
ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames(void)
{
cerr << "ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames start" << endl;

	Assert(!projectedfulldepthimage);

	Assert(rows);
	Assert(columns);

	projectedfulldepthimage=new Uint16[columns*rows];
	Assert(projectedfulldepthimage);

	class SourceBase<Uint16> *source = getSourceSource();
	if (!source) return false;


	unsigned row=0;
	unsigned column=0;
	unsigned frame=0;
	size_t n;
	const Uint16 *buffer;
	Uint16 *dstptr=projectedfulldepthimage;
	while (source->read()) {
		n=source->getBufferCount();
		Assert(n);
		buffer=source->getBuffer();
		Assert(buffer);
		while (n--) {
			Uint16 srcvalue=*buffer++;
			Uint16 dstvalue=*dstptr;

//cerr << "srcvalue[" << dec << frame << "," << row << "," << column << "] = " << hex << srcvalue << dec << endl;

			// here is the meat ... map source value into destination value

			if (frame == 0) {		// first pass ... just copy values
				*dstptr=srcvalue;
			}
			else {
				if (projectiontype == MIP) {	// Maximum Intensity Projection
					if (issigned) {
						if (Int16(srcvalue) > Int16(dstvalue)) *dstptr=Uint16(srcvalue);
					}
					else {
						if (srcvalue > dstvalue) *dstptr=srcvalue;
					}
				}
				else if (projectiontype == SUM) {	// simple sum (clipped within 16 bits)
					// should clip properly here ... for now truncate
					if (issigned) {
						*dstptr=Uint16(Int16(srcvalue)+Int16(dstvalue));
					}
					else {
						*dstptr=srcvalue+dstvalue;
					}
				}
				else if (projectiontype == FUZZY) {	// fuzzy segmentation
					long s;
					long d;
					if (issigned) {
						s=Int16(srcvalue);
						d=Int16(dstvalue);
					}
					else {
						s=Uint16(srcvalue);
						d=Uint16(dstvalue);
					}

					long lowrampstartvalue=0;
					long lowrampendvalue=35;
					long highrampstartvalue=150;
					long highrampendvalue=2000;

					Assert(lowrampstartvalue <= lowrampendvalue);
					Assert(lowrampendvalue <= highrampstartvalue);
					Assert(highrampstartvalue <= highrampendvalue);

					long belowrampweight=0;
					long onrampweight=256;
					long aboverampweight=0;

					long contrib;

					if      (s < lowrampstartvalue)  contrib=belowrampweight;
					else if (s > highrampendvalue)   contrib=aboverampweight;
					else if (s > lowrampendvalue
					      && s < highrampstartvalue) contrib=onrampweight;
					else if (s >= lowrampstartvalue
					      && s <= lowrampendvalue) {
						contrib=long((double(s)-lowrampstartvalue)
							     /(lowrampendvalue-lowrampstartvalue+1)
							     *(onrampweight-belowrampweight));
					}
					else if (s >= highrampstartvalue
					      && s <= highrampendvalue) {
						contrib=long(highrampendvalue-((double(s)-highrampstartvalue))
							     /(highrampendvalue-highrampstartvalue+1)
							     *(onrampweight-aboverampweight));
					}
					else {
						Assert(0);
					}
					
					if (issigned) {
						*dstptr=Uint16(Int16(d+contrib));
					}
					else {
						*dstptr=Uint16(d+contrib);
					}
				}
				else {
					Assert(0);
				}
			}

			// end of meat ... increment indices and counts ...

			++dstptr;

			if (++column >= columns) {
				column=0;
//cerr << "Row = " << dec << row << endl;
				if (++row >= rows) {
					row=0;
cerr << "Frame = " << dec << frame << endl;
					Assert(frame<frames);
					++frame;
					dstptr=projectedfulldepthimage;
				}
			}
		}
	}

cerr << "ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames at end: column = " << dec << column << endl;
cerr << "ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames at end: row = " << dec << row << endl;
cerr << "ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames at end: frame = " << dec << frame << endl;

	//Assert(column == 0 && row == 0 && frame == frames);

cerr << "ImageFromProjectedFrames::createProjectedFullDepthImageFromSourceFrames done" << endl;

	return true;
}


int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool quiet=options.get("quiet") || options.get("silent");

	ProjectionType projectiontype = ProjectionType(0);

	if (options.get("mip")) projectiontype=MIP;
	if (options.get("sum")) projectiontype=SUM;
	if (options.get("fuzzy")) projectiontype=FUZZY;

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
			<< " [-mip|-sum|-fuzzy]"
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
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	if (!list.good()) {
		log << list.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	Uint16 vRows = 0;
	Attribute *aRows = list[TagFromName(Rows)];
	if (!aRows) {
		log << EMsgDC(MissingAttribute)
		    << " - \"Rows\""
		    << endl;
		success=false;
	}
	else
		vRows=AttributeValue(aRows);

	Uint16 vColumns = 0;
	Attribute *aColumns = list[TagFromName(Columns)];
	if (!aColumns) {
		log << EMsgDC(MissingAttribute)
		    << " - \"Columns\""
		    << endl;
		success=false;
	}
	else
		vColumns=AttributeValue(aColumns);

	Uint16 vNumberOfFrames = 0;
	Attribute *aNumberOfFrames = list[TagFromName(NumberOfFrames)];
	if (!aNumberOfFrames) {
		log << EMsgDC(MissingAttribute)
		    << " - \"NumberOfFrames\""
		    << endl;
		success=false;
	}
	else
		vNumberOfFrames=AttributeValue(aNumberOfFrames);

	char *vPhotometricInterpretation = 0;
	Attribute *aPhotometricInterpretation = list[TagFromName(PhotometricInterpretation)];
	if (!aPhotometricInterpretation) {
		log << EMsgDC(MissingAttribute)
		    << " - \"PhotometricInterpretation\""
		    << endl;
		success=false;
	}
	else
		vPhotometricInterpretation=AttributeValue(aPhotometricInterpretation);

	Uint16 vSamplesPerPixel = 0;
	Attribute *aSamplesPerPixel = list[TagFromName(SamplesPerPixel)];
	if (!aSamplesPerPixel) {
		log << EMsgDC(MissingAttribute)
		    << " - \"SamplesPerPixel\""
		    << endl;
		success=false;
	}
	else
		vSamplesPerPixel=AttributeValue(aSamplesPerPixel);

	Uint16 vBitsAllocated = 0;
	Attribute *aBitsAllocated = list[TagFromName(BitsAllocated)];
	if (!aBitsAllocated) {
		log << EMsgDC(MissingAttribute)
		    << " - \"BitsAllocated\""
		    << endl;
		success=false;
	}
	else
		vBitsAllocated=AttributeValue(aBitsAllocated);

	Uint16 vBitsStored = 0;
	Attribute *aBitsStored = list[TagFromName(BitsStored)];
	if (!aBitsStored) {
		log << EMsgDC(MissingAttribute)
		    << " - \"BitsStored\""
		    << endl;
		success=false;
	}
	else
		vBitsStored=AttributeValue(aBitsStored);

	Uint16 vHighBit = 0;
	Attribute *aHighBit = list[TagFromName(HighBit)];
	if (!aHighBit) {
		log << EMsgDC(MissingAttribute)
		    << " - \"HighBit\""
		    << endl;
		success=false;
	}
	else
		vHighBit=AttributeValue(aHighBit);

	Uint16 vPixelRepresentation = 0xffff;
	Attribute *aPixelRepresentation = list[TagFromName(PixelRepresentation)];
	if (!aPixelRepresentation) {
		log << EMsgDC(MissingAttribute)
		    << " - \"PixelRepresentation\""
		    << endl;
		success=false;
	}
	else
		vPixelRepresentation=AttributeValue(aPixelRepresentation);

	Attribute *aPixelData = list[TagFromName(PixelData)];

	Uint32 length=0;
	if (!aPixelData) {
		log << EMsgDC(MissingAttribute)
		    << " - \"PixelData\""
		    << endl;
		success=false;
	}
	else {
		length=aPixelData->getVL();
	}

	if (!quiet) {
		log << "Read ..." << endl;
		log << "\tRows = " << dec << vRows << endl;
		log << "\tColumns = " << dec << vColumns << endl;
		log << "\tNumberOfFrames = " << dec << vNumberOfFrames << endl;
		log << "\tPhotometricInterpretation = "
		    << (vPhotometricInterpretation ? vPhotometricInterpretation : "")
		    << endl;
		log << "\tSamplesPerPixel = " << dec << vSamplesPerPixel << endl;
		log << "\tBitsAllocated = " << dec << vBitsAllocated << endl;
		log << "\tBitsStored = " << dec << vBitsStored << endl;
		log << "\tHighBit = " << dec << vHighBit << endl;
		log << "\tPixelRepresentation = " << dec << vPixelRepresentation << endl;
		log << "\tPixelData Value Length = " << hex << length << dec << endl;
	}

	if (vBitsAllocated > 16) {
		log << EMsgDC(Unsupported) << " - BitsAllocated = " << vBitsAllocated << " (> 16)" << endl;
		success=false;
	}

	if (vBitsStored > 16) {
		log << EMsgDC(Unsupported) << " - BitsStored = " << vBitsStored << " (> 16)" << endl;
		success=false;
	}

	if (vHighBit > 15) {
		log << EMsgDC(Unsupported) << " - HighBit = " << vHighBit << " (> 15)" << endl;
		success=false;
	}

	if (vSamplesPerPixel != 1) {
		log << EMsgDC(Unsupported) << " - SamplesPerPixel = " << vSamplesPerPixel << " (!= 1)" << endl;
		success=false;
	}

	Assert((Uint32)vRows*vColumns*vSamplesPerPixel <= length/vNumberOfFrames*8/vBitsAllocated);	// framelengthinwords

	if (!vRows || !vColumns
	 || !vPhotometricInterpretation || !vSamplesPerPixel
	 || !vBitsAllocated) {
		log << EMsgDC(MissingMandatoryAttributes) << endl;
		success=false;
	}

	if (vNumberOfFrames < 2) {
		log << "Single frames not supported" << endl;
		success=false;
	}

	if (din.getTransferSyntaxToReadDataSet()->isEncapsulated()) {
		log << "Encapsulated (compressed) input data not supported" << endl;
		success=false;
	}

	OtherUnspecifiedLargeAttributeBase *oPixelData = 0;
	SupplySourceFromAttribute *sPixelData = 0;
	ImageFromProjectedFrames *image = 0;
	TransferSyntax *transfersyntax = 0;

	if (success) {
		Assert(aPixelData);

		if (!aPixelData->isOtherData()) {
			log << EMsgDC(PixelDataIncorrectVR) << endl;
			success=false;
		}
		else {
			oPixelData = aPixelData->castToOtherData();
			Assert(oPixelData);
			sPixelData = new SupplySourceFromAttribute(oPixelData);
			Assert(sPixelData);

			image=new ImageFromProjectedFrames(sPixelData,vColumns,vRows,vNumberOfFrames,
					vBitsAllocated,vBitsStored,vHighBit,vPixelRepresentation,
					projectiontype);
			Assert(image);
			image->createProjectedFullDepthImageFromSourceFrames();
cerr << "remove old" << endl;
			list-=TagFromName(NumberOfFrames);
			list-=aPixelData;
cerr << "create transfersyntax" << endl;
			transfersyntax = new TransferSyntax (
				dicom_output_options.transfersyntaxuid
				? dicom_output_options.transfersyntaxuid
				: DefaultTransferSyntaxUID);
			Assert(transfersyntax);
cerr << "add new pixel data" << endl;
			list+=new OtherUnspecifiedLargeAttribute(
				TagFromName(PixelData),
				image->getResultSource(),
				vRows,
				vColumns,
				1,	// Number of Frames
				vSamplesPerPixel,
				transfersyntax,
				0,vBitsAllocated,vBitsStored,vHighBit);
cerr << "write it" << endl;
			if (!usualManagedAttributeListWrite(list,dout,
				dicom_output_options,log,verbose)) success=false;
		}
	}
cerr << "delete stuff" << endl;

	if (oPixelData) delete oPixelData;
	if (sPixelData) delete sPixelData;
	if (image) delete image;
	if (transfersyntax) delete transfersyntax;

	return success ? 0 : 1;
}


