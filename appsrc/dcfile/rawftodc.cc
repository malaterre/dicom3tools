#include "attrtype.h"
#include "attrothr.h"
#include "attrmxls.h"
#include "transynu.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"
#include "srcsink.h"
#include "convtype.h"
#include "fltype.h"

class RawFloat_PixelDataSourceBase : public SourceBase<Uint16> {
protected:
	istream *istr;
	long offset;
	Uint16 rows;
	Uint16 columns;
	Uint16 frames;
	Uint16 samples;
	double scaleFactor;
	Uint16 componentOffset;
	Uint16 componentNumber;

	Uint32 totalrows;
	Uint32 row;
	Uint16 col;
	Uint16 frame;
	Uint16 *buffer;
public:
	RawFloat_PixelDataSourceBase(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s,double scf,Uint16 cOffset,Uint16 cNumber)
			: SourceBase<Uint16>()
		{
			istr=&i;
			offset=off;
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);
			frames=f;
			Assert(frames);
			samples=s;
			Assert(samples);
			scaleFactor=scf;
			componentOffset=cOffset;
			componentNumber=cNumber;

			totalrows=((Uint32)rows)*frames;
			row=0;
			col=0;
			frame=0;
			buffer=new Uint16[c*s];
			Assert(buffer);
		}

	virtual ~RawFloat_PixelDataSourceBase()
		{
			if (buffer) delete[] buffer;
		}

	virtual size_t read(void) = 0;

	const Uint16 *getBuffer(void)		{ return buffer; }

	size_t getBufferCount(void) const	{ return col*samples; }

	int good(void) const	{ return istr && istr->good() && row < totalrows; }
};

class RawFloat_PixelDataSource_LittleEndian : public RawFloat_PixelDataSourceBase {
public:
	RawFloat_PixelDataSource_LittleEndian(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s,double scf,Uint16 cOffset,Uint16 cNumber)
			: RawFloat_PixelDataSourceBase(i,off,r,c,f,s,scf,cOffset,cNumber)
		{}

	size_t read(void)
		{
			if (row == 0 && frame == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!istr->good() || row++ >= totalrows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				Uint16 sample=0;
				while (sample < samples) {
					int component;
					IEEE_Float32_L value;
					unsigned char *bytes = (unsigned char *)(&value);
					for (component=0; component<componentOffset; ++component) {		// skip components before the one we want
						istr->read((char *)bytes,4);
						if (!istr->good()) return 0;
					}
					istr->read((char *)bytes,4);
					if (!istr->good()) return 0;
					Uint16 pixel = (Uint16)(value * scaleFactor);
//cerr << value << ",";
					*ptr++=pixel; ++sample;
					for (component=componentOffset+1; component<componentNumber; ++component) {	// skip components after the one we want
						istr->read((char *)bytes,4);
						if (!istr->good()) return 0;
					}
				}
				++col;
			}
//cerr << endl;
			return col*samples;
		}
};

class RawFloat_PixelDataSource_BigEndian : public RawFloat_PixelDataSourceBase {
public:
	RawFloat_PixelDataSource_BigEndian(istream& i,long off,Uint16 r,Uint16 c,
		Uint16 f,Uint16 s,double scf,Uint16 cOffset,Uint16 cNumber)
			: RawFloat_PixelDataSourceBase(i,off,r,c,f,s,scf,cOffset,cNumber)
		{}

	size_t read(void)
		{
			if (row == 0 && frame == 0) {
				Assert(istr);
				istr->seekg(offset,ios::beg);
			}
			col=0;
			if (!istr->good() || row++ >= totalrows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {
				Uint16 sample=0;
				while (sample < samples) {
					int component;
					IEEE_Float32_B value;
					unsigned char *bytes = (unsigned char *)(&value);
					for (component=0; component<componentOffset; ++component) {		// skip components before the one we want
						istr->read((char *)bytes,4);
						if (!istr->good()) return 0;
					}
					istr->read((char *)bytes,4);
					if (!istr->good()) return 0;
					Uint16 pixel = (Uint16)(value * scaleFactor);
//cerr << value << ",";
					*ptr++=pixel; ++sample;
					for (component=componentOffset+1; component<componentNumber; ++component) {	// skip components after the one we want
						istr->read((char *)bytes,4);
						if (!istr->good()) return 0;
					}
				}
				++col;
			}
//cerr << endl;
			return col*samples;
		}
};

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	InputOptions		input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	long offset=0;
	(void)options.get("offset",offset);

	unsigned rows=0;
	if (!options.get("rows",rows) && !options.get("height",rows) && !options.get("h",rows)) {
		cerr << EMsgDC(NeedOption) << " - rows" << endl;
		bad=true;
	}
	
	unsigned cols=0;
	if (!options.get("columns",cols) && !options.get("width",cols) && !options.get("w",cols)) {
		cerr << EMsgDC(NeedOption) << " - columns" << endl;
		bad=true;
	}
	
	unsigned frames=1;
	options.get("frames",frames);
	
	// e.g. for real of real and imaginary pair, componentOffset=0,componentNumber=2
	
	unsigned componentOffset=0;
	options.get("componentoffset",componentOffset);
	
	unsigned componentNumber=1;
	options.get("componentnumber",componentNumber);
	
	double scaleFactor=1;
	options.get("scale",scaleFactor);	// use 65535 to convert 0.0-1.0 to 16 but unsigned
	
	unsigned bits=16;

	bool littleendian=options.get("little") || options.get("l");
	bool bigendian=options.get("big") || options.get("b");

	if (littleendian && bigendian) {
		cerr << EMsgDC(OptionsIncompatible) << " - big & little" << endl;
		bad=true;
	}

	unsigned samples;
	bool samplespresent=options.get("samples",samples);

	const char *photometricInterpretation=0;

	int graypresent=(options.get("gray") || options.get("grey")) ? 1 : 0;
	int rgbpresent=options.get("rgb") ? 1 : 0;
	int argbpresent=options.get("argb") ? 1 : 0;
	int hsvpresent=options.get("hsv") ? 1 : 0;
	int cmykpresent=options.get("cmyk") ? 1 : 0;
	int photometricpresent=options.get("photometric-interpretation",photometricInterpretation) ? 1 : 0;

	int sumofphotometric=graypresent+rgbpresent+argbpresent+hsvpresent+cmykpresent+photometricpresent;

	if (sumofphotometric == 0) {
		if (samplespresent) {
			if (samples == 1) {
				photometricInterpretation="MONOCHROME2";
			}
			else if (samples == 3) {
				photometricInterpretation="RGB";
			}
		}
		else {
			photometricInterpretation="MONOCHROME2";
			samples=1;
		}
	}
	else if (sumofphotometric > 1) {
		cerr << EMsgDC(OptionsIncompatible) << " - gray,rgb,argb,hsv,cmyk,photometric-interpretation" << endl;
		bad=true;
	}
	else {
		if (graypresent) {
			if (samplespresent && samples != 1) {
				cerr << EMsgDC(OptionsIncompatible) << " - samples != 1 & gray" << endl;
				bad=true;
			}
			else
				samples=1;
			photometricInterpretation="MONOCHROME2";
		}
		else if (rgbpresent) {
			if (samplespresent && samples != 3) {
				cerr << EMsgDC(OptionsIncompatible) << " - samples != 3 & rgb" << endl;
				bad=true;
			}
			else
				samples=3;
			photometricInterpretation="RGB";
		}
		else if (argbpresent) {
			if (samplespresent && samples != 4) {
				cerr << EMsgDC(OptionsIncompatible) << " - samples != 4 & argb" << endl;
				bad=true;
			}
			else
				samples=4;
			photometricInterpretation="ARGB";
		}
		else if (hsvpresent) {
			if (samplespresent && samples != 3) {
				cerr << EMsgDC(OptionsIncompatible) << " - samples != 3 & hsv" << endl;
				bad=true;
			}
			else
				samples=3;
			photometricInterpretation="HSV";
		}
		else if (cmykpresent) {
			if (samplespresent && samples != 4) {
				cerr << EMsgDC(OptionsIncompatible) << " - samples != 4 & cmyk" << endl;
				bad=true;
			}
			else
				samples=4;
			photometricInterpretation="CMYK";
		}
		else if (photometricpresent) {
			if (!samplespresent) {
				cerr << EMsgDC(OptionRequired) << " - samples" << endl;
				bad=true;
			}
		}
	}

	Assert(photometricInterpretation);
	Assert(samples);

	bool bypixelpresent=options.get("color-by-pixel");
	bool byplanepresent=options.get("color-by-plane");

	unsigned planarConfiguration;

	if (bypixelpresent && byplanepresent) {
		cerr << EMsgDC(OptionsIncompatible) << " - color-by-pixel & color-by-plane" << endl;
		bad=true;
	}
	if (samples == 1) {
		if (bypixelpresent || byplanepresent) {
			cerr << EMsgDC(OptionsIncompatible) << " - (color-by-pixel|color-by-plane) & samples = 1" << endl;
			bad=true;
		}
	}
	else {
		if (bypixelpresent)
			planarConfiguration=0;
		else if (byplanepresent)
			planarConfiguration=1;
		else {
			cerr << EMsgDC(OptionRequired) << " - color-by-pixel | color-by-plane" << endl;
			bad=true;
		}
	}
			
	input_options.done();
	dicom_output_options.done();
	options.done();

	InputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< dicom_output_options.usage()
			<< " -rows|-height|-h rows"
			<< " -columns|-width|-w columns"
			<< " [-frames frames]"
			<< " [-scale scalefactor]"
			<< " [-componentoffset 0|1]"
			<< " [-componentnumber 2]"
			<< " [-little|-l|-big]"
			<< " [-samples n]"
			<< " [-offset bytes]"
			<< " [[-gray|-grey|-rgb|-argb|-hsv|-cmyk]|[-photometric-interpretation xxx]]"
			<< " [-color-by-pixel|-color-by-plane]"
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	istream in(input_opener);
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	ManagedAttributeList list;

	TransferSyntax transfersyntax(
		dicom_output_options.transfersyntaxuid
		? dicom_output_options.transfersyntaxuid
		: DefaultTransferSyntaxUID);

	TextOutputStream log (cerr);
	if (verbose) log << "Converting ..." << endl;

	Uint16 bitsAllocated=((bits-1)/8+1)*8;
	Uint16 bitsStored=bits;
	Uint16 highBit=bits-1;
	//Uint16 frames=1;

	list+=new UnsignedShortAttribute(TagFromName(BitsAllocated),bitsAllocated);
	list+=new UnsignedShortAttribute(TagFromName(BitsStored),bitsStored);
	list+=new UnsignedShortAttribute(TagFromName(HighBit),highBit);
	list+=new UnsignedShortAttribute(TagFromName(Rows),rows);
	list+=new UnsignedShortAttribute(TagFromName(Columns),cols);
	list+=new IntegerStringAttribute(TagFromName(NumberOfFrames),(Uint16)frames);
	list+=new UnsignedShortAttribute(TagFromName(SamplesPerPixel),samples);
	list+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),0u);
	list+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),photometricInterpretation);
	if (samples > 1) list+=new UnsignedShortAttribute(TagFromName(PlanarConfiguration),planarConfiguration);

	// various Type 1 and Type 2 attributes for mandatory SC modules ...

	list+=new PersonNameAttribute(TagFromName(PatientName),"^^^^");
	list+=new LongStringAttribute(TagFromName(PatientID));
	list+=new DateStringAttribute(TagFromName(PatientBirthDate));
	list+=new CodeStringAttribute(TagFromName(PatientSex));
	list+=new ShortStringAttribute(TagFromName(StudyID));
	list+=new DateStringAttribute(TagFromName(StudyDate));
	list+=new TimeStringAttribute(TagFromName(StudyTime));
	list+=new PersonNameAttribute(TagFromName(ReferringPhysicianName),"^^^^");
	list+=new ShortStringAttribute(TagFromName(AccessionNumber));
	list+=new IntegerStringAttribute(TagFromName(SeriesNumber));
	list+=new IntegerStringAttribute(TagFromName(InstanceNumber));

	list+=new CodeStringAttribute(TagFromName(Modality),"OT");
	list+=new CodeStringAttribute(TagFromName(ConversionType),"WSD");

	list+=new LongStringAttribute(TagFromName(Manufacturer));
	list+=new CodeStringAttribute(TagFromName(PatientOrientation));

	RawFloat_PixelDataSourceBase *pixeldatasrc=0;
	if (littleendian) {
		pixeldatasrc=new RawFloat_PixelDataSource_LittleEndian(in,offset,rows,cols,frames,samples,scaleFactor,componentOffset,componentNumber);
	}
	else if (bigendian) {
		pixeldatasrc=new RawFloat_PixelDataSource_BigEndian(in,offset,rows,cols,frames,samples,scaleFactor,componentOffset,componentNumber);
	}
	Assert(pixeldatasrc);

	list+=new OtherUnspecifiedLargeAttribute(
		TagFromName(PixelData),
		pixeldatasrc,
		rows,
		cols,
		frames,
		samples,
		&transfersyntax,
		0,bitsAllocated,bitsStored,highBit);

	if (!usualManagedAttributeListWrite(list,dout,
		dicom_output_options,log,verbose)) return 1;

	return 0;
}

