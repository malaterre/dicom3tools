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

	// gray is the real gray value in the image pixel data
	// index is the gray value windowed to a range 0..ncells

	// strategy is ...
	//	allocate lut of gray to cell
	//	request <= 244 cells
	//	load those cells with a linear gray scale
	//	fill graytocell lut with cell numbers
	//	operator[] returns cell number given gray value

class BufferedWindowedGrayImage {
private:
	Uint16 *bufferforfulldepthimage;

	SupplySource *supplysource;

	Uint16 columns;
	Uint16 rows;
	Uint16 frames;
	Uint16 planes;
	Uint16 bitsallocated;
	Uint16 bitsstored;
	Uint16 highbit;
	bool issigned;

	bool inverted;
	bool needdefaultwindowsetting;
	Uint16 mingray;
	Uint16 maxgray;
	Uint16 bottom;
	Uint16 top;
	Uint16 signxor;
	char  *graytocell;
	Uint32 ngrays;
	Uint32 ncells;

	class SourceBase<Uint16> *getSource(void)
		{
			Assert(supplysource);
			return supplysource->getSource();
		}

	char operator[](Uint16 index)
		{
			Assert(index<ngrays);
			Assert(graytocell);
			return graytocell[index];
		}

	char mapPixel(const Uint16 src)
		{
			return operator[](src);
		}

	const char *getPixelMap(Uint32 &mapsize) const
		{
			Assert(graytocell);
			mapsize=ncells;
			return graytocell;
		}

	bool getBufferedFullDepthImage(void);

public:
	BufferedWindowedGrayImage(
		SupplySource *s,Uint16 vColumns,Uint16 vRows,Uint16 vNumberOfFrames,
		Uint16 vBitsAllocated,Uint16 vBitsStored,Uint16 vHighBit,
		Uint16 vPixelRepresentation,bool invertedgrayscale)
		{
			bufferforfulldepthimage=0;

			supplysource=s;
			columns=vColumns;
			rows=vRows;
			frames=vNumberOfFrames;
			planes=1;
			bitsallocated=vBitsAllocated;
			bitsstored=vBitsStored;
			highbit=vHighBit;
			issigned=vPixelRepresentation;
			Assert(supplysource);
			Assert(columns);
			Assert(rows);
			Assert(frames);
			Assert(planes);
			Assert(bitsallocated);
			Assert(bitsstored);
			Assert(highbit);

			Assert(bitsstored);
			inverted=invertedgrayscale;
			mingray=0;
			Uint32 mx=(Uint32(1)<<bitsstored)-1;
			Assert(mx <= Uint16_MAX);
			maxgray=Uint16(mx);
			Assert(maxgray >= mingray);
			ngrays=maxgray-mingray+1;

			signxor=issigned ? 1<<(bitsstored-1) : 0;

//cerr << "BufferedWindowedGrayImage::BufferedWindowedGrayImage" << "\n"
//     << "\t invertedgrayscale=" << dec << invertedgrayscale << "\n"
//     << "\t bitsstored=" << dec << bitsstored << "\n"
//     << "\t issigned=" << dec << issigned << "\n"
//     << "\t mingray=" << dec << mingray << "\n"
//     << "\t mx=" << dec << mx << "\n"
//     << "\t maxgray=" << dec << maxgray << "\n"
//     << "\t ngrays=" << dec << ngrays << "\n"
//     << "\t signxor(hex)=" << hex << signxor << dec << "\n"
//     << endl;

			graytocell=new char[ngrays];
			Assert(graytocell);
			ncells=0;
			needdefaultwindowsetting=true;

		}

	~BufferedWindowedGrayImage()
		{
			if (bufferforfulldepthimage) delete[] bufferforfulldepthimage;

			if (graytocell) delete[] graytocell;
		}

	bool getImageStatistics(Uint16 framecount,
		Uint16 &minval,Uint16 &maxval);

	void setOutputGrayLevelsWanted(unsigned nwanted)
		{
			ncells=nwanted;
//cerr << "BufferedWindowedGrayImage::setOutputGrayLevelsWanted ngrays=" << dec << ncells << endl;
		}

	bool setWindowRange(Uint16 b,Uint16 t);
	bool hasWindowCenterWidth(void)	{ return true; }
	bool setWindowCenterWidth(Uint16 center,Uint16 width);
	bool getWindowCenterWidth(Uint16 &center,Uint16 &width);

	bool setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table);

	bool getSigned(void)	{ return issigned; }
	Uint16 getBits(void)	{ return bitsstored; }

	bool write8BitIndexedPixels(ostream &out);
};

bool
BufferedWindowedGrayImage::write8BitIndexedPixels(ostream &out)
{
//cerr << "BufferedWindowedGrayImage::write8BitIndexedPixels start" << endl;

	if (!bufferforfulldepthimage) getBufferedFullDepthImage();
	Uint16 *ptr=bufferforfulldepthimage;

	unsigned row=0;
	unsigned column=0;

long count=0;
	while (row < rows*frames) {
		char c = mapPixel(*ptr++);
		out.write(&c,1);
count++;
		if (++column >= columns) {
			column=0;
			++row;
		}
	}
	//Assert(column == 0 && row == rows*frames);
//cerr << "BufferedWindowedGrayImage::write8BitIndexedPixels count = " << count << endl;
//cerr << "BufferedWindowedGrayImage::write8BitIndexedPixels count-columns*rows*frames= " << count-columns*rows*frames << endl;
//cerr << "BufferedWindowedGrayImage::write8BitIndexedPixels done" << endl;
	return row >= rows*frames;	// read enough
}

bool
BufferedWindowedGrayImage::getBufferedFullDepthImage(void)
{
//cerr << "BufferedWindowedGrayImage::getBufferedFullDepthImage start" << endl;

//long count=0;
	if (!bufferforfulldepthimage) {
		Assert(rows);
		Assert(columns);
		Assert(planes==1);
		Assert(frames);

		class SourceBase<Uint16> *source = getSource();
		if (!source) return false;

		Assert(!bufferforfulldepthimage);
		bufferforfulldepthimage=new Uint16[columns*rows*frames];
		Assert(bufferforfulldepthimage);
		Uint16 *ptr=bufferforfulldepthimage;
		size_t n;
		const Uint16 *buffer;
		while (source->read()) {
			n=source->getBufferCount();
			//Assert(n);
			buffer=source->getBuffer();
			//Assert(buffer);
			while (n--) {
				*ptr++=*buffer++;
//count++;
			}
		}
	}
	//return row >= rows*frames;	// read enough
//cerr << "BufferedWindowedGrayImage::getBufferedFullDepthImage count = " << count << endl;
//cerr << "BufferedWindowedGrayImage::getBufferedFullDepthImage count-columns*rows*frames= " << count-columns*rows*frames << endl;
//cerr << "BufferedWindowedGrayImage::getBufferedFullDepthImage done" << endl;
	return true;	// ignore premature eof
}

bool
BufferedWindowedGrayImage::getImageStatistics(Uint16 framecount,
		Uint16 &minval,Uint16 &maxval)
{
//cerr << "BufferedWindowedGrayImage::getImageStatistics start" << endl;
	if (!bufferforfulldepthimage) getBufferedFullDepthImage();
	Uint16 *ptr=bufferforfulldepthimage;

	Int16  signedminval=Int16_MAX;
	Int16  signedmaxval=Int16_MIN;
	Uint16 unsignedminval=Uint16_MAX;
	Uint16 unsignedmaxval=0;

//cerr << "BufferedWindowedGrayImage::getImageStatistics start min=" << hex << minval << " max=" << hex << maxval << dec << endl;

	unsigned row=0;
	unsigned column=0;
	while (row < rows*framecount) {
		Uint16 value=*ptr++;
		if (Int16(value) < signedminval) signedminval=value;
		if (Int16(value) > signedmaxval) signedmaxval=value;
		if (value < unsignedminval) unsignedminval=value;
		if (value > unsignedmaxval) unsignedmaxval=value;
		if (++column >= columns) {
			column=0;
			++row;
		}
	}
	//Assert(column == 0 && row == rows*framecount);

//cerr << "BufferedWindowedGrayImage::getImageStatistics signedminval=" << hex << Uint16(signedminval) << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics signedmaxval=" << hex << Uint16(signedmaxval) << dec << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics unsignedminval=" << hex << unsignedminval << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics unsignedmaxval=" << hex << unsignedmaxval << dec << endl;

	Uint16 signedrange=Uint16(signedmaxval-signedminval);
	Uint16 unsignedrange=unsignedmaxval-unsignedminval;
//cerr << "BufferedWindowedGrayImage::getImageStatistics signedrange=" << hex << signedrange << dec << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics unsignedrange=" << hex << unsignedrange << endl;
	if (issigned) {
		if (signedrange > unsignedrange) {
			// This message should be propagated properly ... :(
			cerr << WMsgDC(PixelRepresentationIncorrect)
			     << " - " << MMsgDC(ShouldBe) << " " << MMsgDC(Unsigned)
			     << endl;
			issigned=true;
		}
		minval=Uint16(signedminval);
		maxval=Uint16(signedmaxval);
	}
	else {
		if (signedrange < unsignedrange) {
			// This message should be propagated properly ... :(
			cerr << WMsgDC(PixelRepresentationIncorrect)
			     << " - " << MMsgDC(ShouldBe) << " " << MMsgDC(Signed)
			     << endl;
			issigned=false;
		}
		minval=unsignedminval;
		maxval=unsignedmaxval;
	}

//cerr << "BufferedWindowedGrayImage::getImageStatistics minval before xoring=" << hex << minval << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics maxval before xoring=" << hex << maxval << dec << endl;

	// xor with high bit into unsigned space used to represent pixels ...

	if (issigned) minval=minval^(1<<(bitsstored-1));
	if (issigned) maxval=maxval^(1<<(bitsstored-1));

//cerr << "BufferedWindowedGrayImage::getImageStatistics minval=" << hex << minval << endl;
//cerr << "BufferedWindowedGrayImage::getImageStatistics maxval=" << hex << maxval << dec << endl;

//cerr << "BufferedWindowedGrayImage::getImageStatistics done min=" << hex << minval << " max=" << hex << maxval << dec << endl;

//cerr << "BufferedWindowedGrayImage::getImageStatistics done" << endl;
	//return row >= rows*framecount;	// read enough
	return true;	// ignore premature eof
}

bool
BufferedWindowedGrayImage::setWindowRange(Uint16 b,Uint16 t)
{
	bottom=b;
	top=t;

//cerr << "BufferedWindowedGrayImage::setWindowRange"
//     << " bottom=" << dec << bottom
//     << " top=" << dec << top
//     << endl;

	Assert(bottom<=top);
	Assert(bottom>=mingray);
	Assert(top<=maxgray);
	Assert(graytocell);

	needdefaultwindowsetting=false;

	unsigned long i;
	for (i=0; i<bottom; ++i) {
		graytocell[i^signxor]=(inverted ? (ncells-1) : 0);
	}
	for (i=bottom; i<=top; ++i) {
		long index=((i-bottom)*ncells)/(top-bottom+1);
		Assert(index>=0);
		Assert(index<ncells);
		Uint32 spv = i^signxor;
		long possiblyInvertedIndex = (inverted ? (ncells-1-index) : index);
		Assert(possiblyInvertedIndex>=0);
		Assert(possiblyInvertedIndex<ncells);
//cerr << "BufferedWindowedGrayImage::setWindowRange"
//     << " mapping gray value=" << dec << i
//     << " mapping stored as pixel value=" << dec << spv
//     << " to index=" << dec << index
//     << " used as possiblyInvertedIndex=" << dec << possiblyInvertedIndex
//     << endl;
		graytocell[spv]=possiblyInvertedIndex;
		
	}
	for (i=top+1; i<ngrays; ++i) {
		graytocell[i^signxor]=(inverted ? 0 : (ncells-1));
	}
	return true;
}

bool
BufferedWindowedGrayImage::setWindowCenterWidth(Uint16 center,Uint16 width)
{
//cerr << "BufferedWindowedGrayImage::setImageCenterWidth"
//     << " center=" << dec << center
//     << " width=" << dec << width
//     << endl;
	long lbottom=(long)center-width/2;
//cerr << "BufferedWindowedGrayImage::setImageCenterWidth"
//     << " lbottom=" << dec << lbottom
//     << endl;
	if (lbottom < mingray) lbottom=mingray;
	if (lbottom > maxgray) lbottom=maxgray;	 // just in case
//cerr << "BufferedWindowedGrayImage::setImageCenterWidth"
//     << " lbottom clamped to=" << dec << lbottom
//     << endl;
	long ltop=(long)center+width/2;
//cerr << "BufferedWindowedGrayImage::setImageCenterWidth"
//     << " ltop=" << dec << ltop
//     << endl;
	if (ltop > maxgray) ltop=maxgray;
	if (ltop < mingray) ltop=mingray;	// just in case
//cerr << "BufferedWindowedGrayImage::setImageCenterWidth"
//     << " ltop clamped to=" << dec << ltop
//     << endl;
	Assert(lbottom<=Uint16_MAX);
	Assert(ltop<=Uint16_MAX);
	return setWindowRange(Uint16(lbottom),Uint16(ltop));
}

bool
BufferedWindowedGrayImage::getWindowCenterWidth(Uint16 &center,Uint16 &width)
{
	long lcenter=(top+bottom)/2;
	long lwidth=top-bottom;
	Assert(lcenter<=Uint16_MAX);
	Assert(lwidth<=Uint16_MAX);
	center=Uint16(lcenter);
	width=Uint16(lwidth);
	return true;
}


bool
BufferedWindowedGrayImage::setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table)
{
//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " first=" << dec << first
//     << " number=" << dec << number
//     << " depth=" << dec << depth
//     << endl;

	bottom=first;
	Assert(number >= 1);
	top=first+(number-1);

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " bottom=" << dec << bottom
//     << " top=" << dec << top
//     << endl;

	if (bottom < mingray) bottom=mingray;
	if (top > maxgray) top=maxgray;

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " clamped bottom=" << dec << bottom
//     << " clamped top=" << dec << top
//     << endl;

	Assert(bottom<=top);
	Assert(bottom>=mingray);
	Assert(top<=maxgray);
	Assert(graytocell);

	// theoretically, LUT output range should be related to
	// the depth (in the third LUT descriptor), however it is
	// often sent as much less ... hence the check for the
	// actual range here ...
#ifdef USELUTDEPTHFORRANGE
	unsigned long lutoutputrange=1lu<<depth;
	Uint16 lutmin=0;
#else
	Uint16 lutmin=Uint16((1lu<<depth)-1);
	Uint16 lutmax=0;

	{
		int i;
		for (i=0; i<number; ++i) {
			if (table[i] < lutmin) lutmin=table[i];
			if (table[i] > lutmax) lutmax=table[i];
		}
	}
	unsigned long lutoutputrange=lutmax-lutmin+1;

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " lutmax=" << dec << lutmax
//     << endl;
#endif

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " lutoutputrange=" << dec << lutoutputrange
//     << " lutmin=" << dec << lutmin
//     << endl;

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " ncells=" << dec << ncells
//     << endl;

	unsigned long i;

	unsigned long bottomindex=((table[0]-lutmin)*ncells)/lutoutputrange;
	Assert(bottomindex>=0);
	Assert(bottomindex<ncells);
	char bottomcell=inverted ? (ncells-bottomindex-1) : bottomindex;
	for (i=0; i<=bottom; ++i) {
		graytocell[i^signxor]=bottomcell;
	}

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " bottomindex=" << dec << bottomindex
//     << " bottomcell=" << dec << (unsigned)bottomcell
//     << endl;

	for (i=bottom+1; i<top; ++i) {
		unsigned long lutoutput=table[i-bottom];
		unsigned long index=((lutoutput-lutmin)*ncells)/lutoutputrange;
		Assert(index>=0);
		Assert(index<ncells);
		graytocell[i^signxor]=inverted ? (ncells-index-1) : index;
//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " i=" << dec << i
//     << " i^signxor=" << dec << (i^signxor)
//     << " lutoutput=" << dec << lutoutput
//     << " index=" << dec << index
//     << " inverted ? (ncells-index-1) : index=" << dec << (inverted ? (ncells-index-1) : index)
//     << " cell=" << dec << (unsigned)(unsigned char)(inverted ? (ncells-index-1) : index)
 //    << endl;

	}

	unsigned long topindex=((table[number-1]-lutmin)*ncells)/lutoutputrange;
	Assert(topindex>=0);
	Assert(topindex<ncells);
	char topcell=inverted ? (ncells-topindex-1) : topindex;
	for (i=top; i<ngrays; ++i) {
		graytocell[i^signxor]=topcell;
	}

//cerr << "BufferedWindowedGrayImage::setVOILUT"
//     << " topindex=" << dec << topindex
//     << " topcell=" << dec << (unsigned)topcell
//     << endl;

	needdefaultwindowsetting=false;

	return true;
}

// NB. The WindowCenterWidthTranslator converts an "internal" scale
// of window center and width, ie. disregarding the intercept and slope
// entirely and using the unsigned representation of what is in the
// pixel data, to user visible width and center, in which case the sign,
// slope and intercept are used to translate into "real world" values 

class WindowCenterWidthTranslator {
private:
	long signxor;
	long intercept;
	long slopeby1024;
public:
	WindowCenterWidthTranslator(Uint16 bits,bool issigned,
		double rescaleintercept,double rescaleslope);

	~WindowCenterWidthTranslator() {}

	Uint16 getStoredCenterFromDisplayedCenter(long l)
		{
			return Uint16((l-intercept)*1024/slopeby1024+signxor);
		}
	Uint16 getStoredWidthFromDisplayedWidth(long w)
		{
			return Uint16(w*1024/slopeby1024);
		}
	long getDisplayedCenterFromStoredCenter(long l)
		{
			return (l-signxor)*slopeby1024/1024+intercept;
		}
	long getDisplayedWidthFromStoredWidth(long w)
		{
			return w*slopeby1024/1024;
		}
};

WindowCenterWidthTranslator::WindowCenterWidthTranslator(
	Uint16 bits,bool issigned,
	double rescaleintercept,double rescaleslope)
{
	signxor=issigned ? 1l<<(bits-1) : 0;
	Assert(rescaleintercept<=LONG_MAX);
	Assert(rescaleintercept>=LONG_MIN);
	intercept=long(rescaleintercept);
	Assert(rescaleslope<=LONG_MAX/1024);
	Assert(rescaleslope>=LONG_MIN/1024);
	slopeby1024=long(rescaleslope*1024);
}

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 			options(argc,argv);
	DicomInputOptions 			dicom_input_options(options);
	BinaryOutputOptions			output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool quiet=options.get("quiet") || options.get("silent");

	bool invertedgrayscaleoption=options.get("invertedgray");
	bool forcesigned=options.get("signed");
	bool forceunsigned=options.get("unsigned");
	bool ignorewindowinobject=options.get("ignorewindow");
	bool ignorevoilutinobject=options.get("ignorevoilut");

	unsigned windowwidthvalue=0;
	bool windowwidthset=options.get("windowwidth",windowwidthvalue);
	unsigned windowcentervalue=0;
	bool windowcenterset=options.get("windowlevel",windowcentervalue) || options.get("windowcenter",windowcentervalue);

	unsigned whichvoilut=0;
	bool whichvoilutset=options.get("voilut",whichvoilut);

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

	if (forcesigned && forceunsigned) {
		cerr << EMsgDC(NotBothSignedOrUnsigned) << endl;
		bad=true;
	};

	if ( (windowwidthset && !windowcenterset)
	 || (!windowwidthset &&  windowcenterset)) {
		cerr << EMsgDC(NeedBothWindowCenterWidth) << endl;
		bad=true;
	};

	if (!dicom_input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< output_options.usage()
			<< " [-v|-verbose]"
			<< " [-quiet|-silent]"
			<< " [-windowwidth width]"
			<< " [-windowlevel|windowcenter center]"
			<< " [-ignorewindow]"
			<< " [-voilut numberfromzero]"
			<< " [-ignorevoilut]"
			<< " [-invertedgray]"
			<< " [-signed|unsigned]"
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

	Uint16 vRows = 0;
	Attribute *aRows = list[TagFromName(Rows)];
	if (!aRows)
		log << WMsgDC(MissingAttribute)
		    << " - \"Rows\""
		    << endl;
	else
		vRows=AttributeValue(aRows);

	Uint16 vColumns = 0;
	Attribute *aColumns = list[TagFromName(Columns)];
	if (!aColumns)
		log << WMsgDC(MissingAttribute)
		    << " - \"Columns\""
		    << endl;
	else
		vColumns=AttributeValue(aColumns);

	Uint16 vNumberOfFrames = 0;
	Attribute *aNumberOfFrames = list[TagFromName(NumberOfFrames)];
	if (aNumberOfFrames)	// optional
		vNumberOfFrames=AttributeValue(aNumberOfFrames);

	char *vPhotometricInterpretation = 0;
	Attribute *aPhotometricInterpretation = list[TagFromName(PhotometricInterpretation)];
	if (!aPhotometricInterpretation)
		log << WMsgDC(MissingAttribute)
		    << " - \"PhotometricInterpretation\""
		    << endl;
	else
		vPhotometricInterpretation=AttributeValue(aPhotometricInterpretation);

	Uint16 vSamplesPerPixel = 0;
	Attribute *aSamplesPerPixel = list[TagFromName(SamplesPerPixel)];
	if (!aSamplesPerPixel)
		log << WMsgDC(MissingAttribute)
		    << " - \"SamplesPerPixel\""
		    << endl;
	else
		vSamplesPerPixel=AttributeValue(aSamplesPerPixel);

	Uint16 vBitsAllocated = 0;
	Attribute *aBitsAllocated = list[TagFromName(BitsAllocated)];
	if (!aBitsAllocated)
		log << WMsgDC(MissingAttribute)
		    << " - \"BitsAllocated\""
		    << endl;
	else
		vBitsAllocated=AttributeValue(aBitsAllocated);

	Uint16 vBitsStored = 0;
	Attribute *aBitsStored = list[TagFromName(BitsStored)];
	if (!aBitsStored)
		log << WMsgDC(MissingAttribute)
		    << " - \"BitsStored\""
		    << endl;
	else
		vBitsStored=AttributeValue(aBitsStored);

	Uint16 vHighBit = 0;
	Attribute *aHighBit = list[TagFromName(HighBit)];
	if (!aHighBit)
		log << WMsgDC(MissingAttribute)
		    << " - \"HighBit\""
		    << endl;
	else
		vHighBit=AttributeValue(aHighBit);

	Uint16 vPixelRepresentation = 0xffff;
	Attribute *aPixelRepresentation = list[TagFromName(PixelRepresentation)];
	if (!aPixelRepresentation)
		log << WMsgDC(MissingAttribute)
		    << " - \"PixelRepresentation\""
		    << endl;
	else
		vPixelRepresentation=AttributeValue(aPixelRepresentation);

	Uint16 vPlanarConfiguration = 0xffff;
	Attribute *aPlanarConfiguration = list[TagFromName(PlanarConfiguration)];
	if (vSamplesPerPixel > 1 && !aPlanarConfiguration)
		log << WMsgDC(MissingAttribute)
		    << " - \"PlanarConfiguration\""
		    << endl;
	else
		vPlanarConfiguration=AttributeValue(aPlanarConfiguration);

	Float32 vRescaleIntercept = 0;
	Attribute *aRescaleIntercept = list[TagFromName(RescaleIntercept)];
	if (aRescaleIntercept)	// optional
		vRescaleIntercept=AttributeValue(aRescaleIntercept);

	Float32 vRescaleSlope = 0;
	Attribute *aRescaleSlope = list[TagFromName(RescaleSlope)];
	if (aRescaleSlope)	// optional
		vRescaleSlope=AttributeValue(aRescaleSlope);

	Uint16 vWindowCenter = 0;
	Attribute *aWindowCenter = list[TagFromName(WindowCenter)];
	if (aWindowCenter)	// optional
		vWindowCenter=AttributeValue(aWindowCenter);

	Uint16 vWindowWidth = 0;
	Attribute *aWindowWidth = list[TagFromName(WindowWidth)];
	if (aWindowWidth)	// optional
		vWindowWidth=AttributeValue(aWindowWidth);

	Attribute *aPixelData = list[TagFromName(PixelData)];

	if (!aPixelData) {
		if (!quiet)
			log << "\tPixeldata not in normal place ... checking for ImageLocation attribute" << endl;
		Uint16 groupstart;
		Uint16 groupend;
		Attribute *aImageLocation=list[TagFromName(ImageLocation)];
		if (aImageLocation) {
			Uint16 vImageLocation=AttributeValue(aImageLocation);
			if (!quiet)
				log << "\tImageLocation = " << hex << vImageLocation << dec << endl;
			groupstart=vImageLocation;
			groupend=vImageLocation;
		}
		else {
			groupstart=0x7fe0;
			groupend=0x7ffd;
		}
		if (!quiet)
			log << "\tNow searching for Pixeldata" << endl;
		Uint16 group;
		for (group=groupstart; !aPixelData && group <= groupend; ++group) {
			Uint32 owner;
			for (owner=((group&0x0001) ? 0x0100 : 0x0000);
					!aPixelData && owner <= ((group&0x0001) ? 0xff00 : 0x0000); owner+=0x0100) {
				if ((aPixelData=list[Tag(group,Uint16(owner|0x0010))]) && aPixelData->isOtherData()) {
					if (!quiet)
						log << "\tFound Pixeldata in ";
						aPixelData->getTag().write(log);
						log << endl;
					break;
				}
				aPixelData=0;
			}
		}
	}

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

	Attribute *aVOILUTSequence = list[TagFromName(VOILUTSequence)];
	int nVOILUT=0;
	Uint16 *VOILUT_first = 0;
	Uint16 *VOILUT_number = 0;
	Uint16 *VOILUT_depth = 0;
	Uint16 **VOILUT_table = 0;
	if (aVOILUTSequence) {
//cerr << "Have VOILUTSequence" << endl;
		AttributeList **items;
		nVOILUT=aVOILUTSequence->getLists(&items);
		if (nVOILUT) {
			VOILUT_first = new Uint16[nVOILUT];
			Assert(VOILUT_first);
			VOILUT_number = new Uint16[nVOILUT];
			Assert(VOILUT_number);
			VOILUT_depth = new Uint16[nVOILUT];
			Assert(VOILUT_depth);
			VOILUT_table = new Uint16 *[nVOILUT];
			Assert(VOILUT_table);
			int i;
			for (i=0; i<nVOILUT; ++i) {
				Assert(items[i]);
				Attribute *aLUTDescriptor=items[i]->operator[](TagFromName(LUTDescriptor));
				Assert(aLUTDescriptor);
				Assert(aLUTDescriptor->getVM() == 3);
				Assert(aLUTDescriptor->getValue(0,VOILUT_number[i]));
				Assert(aLUTDescriptor->getValue(1,VOILUT_first[i]));
				Assert(aLUTDescriptor->getValue(2,VOILUT_depth[i]));
//cerr << "VOILUTSequence" << endl
//     << "\t first[" << dec << i << "]=" << VOILUT_first[i] << endl
//     << "\t number[" << dec << i << "]=" << VOILUT_number[i] << endl
//     << "\t depth[" << dec << i << "]=" << VOILUT_depth[i] << endl;

				Attribute *aLUTData=items[i]->operator[](TagFromName(LUTData));
				Assert(aLUTData);
				VOILUT_table[i]=new Uint16[VOILUT_number[i]];
				Assert(VOILUT_table[i]);
				int j;
				for (j=0; j<VOILUT_number[i]; ++j) {
					Assert(aLUTData->getValue(j,VOILUT_table[i][j]));
				}
			}
		}
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
		log << "\tPlanarConfiguration = " << hex << vPlanarConfiguration << dec << endl;
		log << "\tRescaleIntercept = " << dec << vRescaleIntercept << endl;
		log << "\tRescaleSlope = " << dec << vRescaleSlope << endl;
		log << "\tWindowCenter = "; if (aWindowCenter) log << vWindowCenter; log << endl;
		log << "\tWindowWidth = "; if (aWindowWidth) log << vWindowWidth; log << endl;
		log << "\tVOILUTs = "; if (nVOILUT) log << nVOILUT; log << endl;
		log << "\tPixelData Value Length = " << hex << length << dec << endl;
	}

	// Try to guess some missing attributes ...

	if (vRescaleSlope*vRescaleSlope < 0.00001) vRescaleSlope=1;

	if (vPixelRepresentation == 0xffff) vPixelRepresentation=0;
	if (forceunsigned) vPixelRepresentation=0;
	if (forcesigned) vPixelRepresentation=1;

	if (!vNumberOfFrames) vNumberOfFrames=1;

	if (!vSamplesPerPixel && !vPhotometricInterpretation) {
		vSamplesPerPixel=1;
		vPhotometricInterpretation=StrDup("MONOCHROME2");
	}
	else if (!vSamplesPerPixel && vPhotometricInterpretation) {
		if (strcmp(vPhotometricInterpretation,"MONOCHROME1") == 0
		 || strcmp(vPhotometricInterpretation,"MONOCHROME2") == 0
		 || strcmp(vPhotometricInterpretation,"PALETTE COLOR") == 0
		) {
			vSamplesPerPixel=1;
		}
		else if (
		    strcmp(vPhotometricInterpretation,"RGB") == 0
		 || strcmp(vPhotometricInterpretation,"HSV") == 0
		) {
			vSamplesPerPixel=3;
		}
		else if (
		    strcmp(vPhotometricInterpretation,"ARGB") == 0
		 || strcmp(vPhotometricInterpretation,"CMYK") == 0
		) {
			vSamplesPerPixel=4;
		}
	}
	else if (!vPhotometricInterpretation && vSamplesPerPixel) {
		switch (vSamplesPerPixel) {
			case 1:	// could check for presence of palette
				vPhotometricInterpretation=StrDup("MONOCHROME2");
				break;
			case 3:	vPhotometricInterpretation=StrDup("RGB");
				break;
			case 4:	vPhotometricInterpretation=StrDup("ARGB");
				break;
		}
	}

	if (!vBitsAllocated && length && length != 0xffffffff
	 && (vRows && vColumns && vNumberOfFrames && vSamplesPerPixel)) {
		vBitsAllocated=Uint16(length/((Uint32)vRows*vColumns*vNumberOfFrames*vSamplesPerPixel)*8);
	}

	if (!vBitsAllocated && vBitsStored)
		vBitsAllocated=((vBitsStored-1u)/8u+1u)*8u;

	if (!vBitsAllocated) {
		if (strcmp(aPixelData->getVR(),"OW") == 0)
			vBitsAllocated=16;
		else
			vBitsAllocated=8;
	}

	Assert(vBitsAllocated <= 16);

	if (!vBitsStored) vBitsStored=vBitsAllocated;
	if (!vHighBit) vHighBit=vBitsStored-1;

	Uint32 framelengthinwords=length/vNumberOfFrames*8/vBitsAllocated;

	if (!vRows) {
		if (!vColumns) {
			if (!vSamplesPerPixel) {
				vRows=Uint16(sqrt(framelengthinwords));
				vColumns=Uint16(length/vRows);
				vSamplesPerPixel=1;
			}
			else {
				Uint32 left=framelengthinwords/vSamplesPerPixel;
				vRows=Uint16(sqrt(left));
				vColumns=Uint16(left/vRows);
			}
		}
		else {
			if (!vSamplesPerPixel) {
				Uint32 left=framelengthinwords/vColumns;
				vRows=Uint16(sqrt(length));
				vSamplesPerPixel=1;
			}
			else {
				vRows=Uint16(framelengthinwords/(vColumns*vSamplesPerPixel));
			}
		}
	}
	else {
		if (!vColumns) {
			if (!vSamplesPerPixel) {
				vColumns=Uint16(framelengthinwords/vRows);
				vSamplesPerPixel=1;
			}
			else {
				vColumns=Uint16(framelengthinwords/(vRows*vSamplesPerPixel));
			}
		}
		else {
			if (!vSamplesPerPixel) {
				vSamplesPerPixel=Uint16(framelengthinwords/(vRows*vColumns));
			}
			// else we know all three
		}
	}

	// use command line specified window values or the explicit option to suppress use of VOI LUT ...

	if (windowwidthset || windowcenterset || ignorevoilutinobject) {
		if (!quiet) log << "Ignoring any VOILUTs because told to or window values explicitly set" << endl;
		nVOILUT = 0;
	}

	// use object values if present and not zero, unless overridden by command line ...

	if (!windowwidthset && !windowcenterset && !ignorewindowinobject && aWindowCenter && vWindowCenter && aWindowWidth && vWindowWidth) {
		if (!quiet) log << "Setting window values extracted from object because present not set on command line" << endl;
		windowwidthset=true;
		windowwidthvalue=vWindowWidth;
		windowcenterset=true;
		windowcentervalue=vWindowCenter;
	}

	if (!quiet) {
		log << "Using ..." << endl;
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
		log << "\tPlanarConfiguration = " << hex << vPlanarConfiguration << dec << endl;
		log << "\tRescaleIntercept = " << dec << vRescaleIntercept << endl;
		log << "\tRescaleSlope = " << dec << vRescaleSlope << endl;
		log << "\tWindowCenter = "; if (windowcenterset && !nVOILUT) log << windowcentervalue; log << endl;
		log << "\tWindowWidth = "; if (windowwidthset && !nVOILUT) log << windowwidthvalue; log << endl;
		log << "\tVOILUTs = "; if (nVOILUT) log << whichvoilut << " of " << nVOILUT; log << endl;
		log << "\tRows*Columns*SamplesPerPixel*BitsAllocated/8 = "
		    << hex << (Uint32)vRows*vColumns*vSamplesPerPixel*vBitsAllocated/8
		    << dec << endl;
	}

	Assert((Uint32)vRows*vColumns*vSamplesPerPixel <= framelengthinwords);

	if (!vRows || !vColumns
	 || !vPhotometricInterpretation || !vSamplesPerPixel
	 || !vBitsAllocated
	 || (vSamplesPerPixel > 1 && vPlanarConfiguration == 0xffff)) {
		log << EMsgDC(MissingMandatoryAttributes) << endl;
		success=false;
	}

	//if (vNumberOfFrames > 1) {
	//	log << "Multiple frames not supported" << endl;
	//	success=false;
	//}

	if (din.getTransferSyntaxToReadDataSet()->isEncapsulated()) {
		log << "Encapsulated (compressed) input data not supported" << endl;
		success=false;
	}

	OtherUnspecifiedLargeAttributeBase *oPixelData = 0;
	SupplySourceFromAttribute *sPixelData = 0;
	BufferedWindowedGrayImage *image = 0;
	const char *pnmstring=0;

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

			if (vSamplesPerPixel == 1) {
				bool monochrome=false;
				bool invertedgrayscale=false;
				if (strcmp(vPhotometricInterpretation,"MONOCHROME1") == 0) {
					monochrome=true;
					invertedgrayscale=true;
					pnmstring="P5";
				}
				else if (strcmp(vPhotometricInterpretation,"MONOCHROME2") == 0) {
					monochrome=true;
					invertedgrayscale=false;
					pnmstring="P5";
				}
				else {
					log << EMsgDC(Unsupported) << " - PhotometricInterpretation = \""
					    << (vPhotometricInterpretation ? vPhotometricInterpretation : "")
					    << "\"" << endl;
					success=false;
				}

				if (invertedgrayscaleoption) invertedgrayscale=!invertedgrayscale;

				if (vBitsAllocated <= 16) {
					Assert(monochrome);
					image=new BufferedWindowedGrayImage(sPixelData,vColumns,vRows,vNumberOfFrames,
							vBitsAllocated,vBitsStored,vHighBit,
							vPixelRepresentation,invertedgrayscale);
				}
				else {
					log << EMsgDC(Unsupported) << " - BitsStored = " << vBitsStored << endl;
					success=false;
				}
			}
			else {
				log << EMsgDC(Unsupported) << " - SamplesPerPixel = " << vSamplesPerPixel << endl;
				success=false;
			}
		}
	}

	if (success) {
		Assert(image);
		Assert(pnmstring);
		out << pnmstring << "\n";
		out << vColumns << " " << vRows << "\n";
		out << 255 << "\n";

		if (!quiet) log << "Writing ..." << endl;

		image->setOutputGrayLevelsWanted(256);

		WindowCenterWidthTranslator wlwtranslate(
			image->getBits(),	// Probably still same as BitsStored
			image->getSigned(),		// May have been changed by statistics
			vRescaleIntercept,vRescaleSlope);

		bool usewlt = !nVOILUT && image->hasWindowCenterWidth();

		if (nVOILUT) {
			if (!quiet) log << "Applying VOILUT " << whichvoilut << endl;
			image->setVOILUT(VOILUT_first[whichvoilut],
				VOILUT_number[whichvoilut],VOILUT_depth[whichvoilut],VOILUT_table[whichvoilut]);
		}

		if (usewlt) {
			if (windowcenterset && windowwidthset) {
				if (!quiet) log << "Applying supplied displayed window values width " << windowwidthvalue << " center " << windowcentervalue << endl;
				Uint16 center=(Uint16)wlwtranslate.getStoredCenterFromDisplayedCenter(windowcentervalue);
				Uint16 width=(Uint16)wlwtranslate.getStoredWidthFromDisplayedWidth(windowwidthvalue);
				if (!quiet) log << "Applying stored window values width " << width << " center " << center << endl;
				image->setWindowCenterWidth(center,width);
			}
			else {
				Uint16 minval;
				Uint16 maxval;
				image->getImageStatistics(1,minval,maxval);	// just one frame
				if (!quiet) log << "Applying statistical window range minval " << minval << " maxval " << maxval << endl;
				image->setWindowRange(minval,maxval);
			}
		}

		if (!image->write8BitIndexedPixels(out)) {
			log << EMsgDC(PixelDataReadFailed) << endl;
			success=false;
		}
		out.flush();
		if (!out.good()) {
			log << EMsgDC(Writefailed) << endl;
			success=false;
		}
	}

	//if (image) delete image;
	//if (sPixelData) delete sPixelData;
	//if (oPixelData) delete oPixelData;

	return success ? 0 : 1;
}



