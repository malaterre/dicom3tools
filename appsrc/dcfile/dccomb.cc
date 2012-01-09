#include "attrmxls.h"
#include "attrothr.h"
#include "attrval.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "srcsink.h"
#include "elmconst.h"

template<class Tin1,class Tin2,class Tout>
class FilterTwoSourcesToSinkBufferedBase : public SourceBuffered<Tout> {
protected:
	SourceBase<Tin1> *src1;
	SourceBase<Tin2> *src2;
public:
	FilterTwoSourcesToSinkBufferedBase(SourceBase<Tin1>& i1,SourceBase<Tin2>& i2): SourceBuffered<Tout>() {
//cerr << "FilterTwoSourcesToSinkBufferedBase::FilterTwoSourcesToSinkBufferedBase()\n" << flush;
		src1=&i1;
		src2=&i2;
	}
};

template<class Tin1,class Tin2,class Tout>
class CombineFilter {
public:
	CombineFilter() {}
	virtual Tout filter(Tin1,Tin2) = 0;
};


template<class Tin1,class Tin2,class Tout>
class ConvertTwoSourcesToSinkWithFilter : public FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout> {
private:
	CombineFilter<Tin1,Tin2,Tout> *filter;
public:
	ConvertTwoSourcesToSinkWithFilter(SourceBase<Tin1>& i1,SourceBase<Tin2>& i2,CombineFilter<Tin1,Tin2,Tout> *f)
			: FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>(i1,i2)
		{
//cerr << "ConvertTwoSourcesToSinkWithFilter::ConvertTwoSourcesToSinkWithFilter\n" << flush;
			Assert(f);
			filter=f;
		}

	size_t read(void)
		{
//cerr << "ConvertTwoSourcesToSinkWithFilter::read(void)\n" << flush;
			FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::buffer.clear();
			//Assert(FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1);
			//Assert(FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2);
			if (FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1->read() && FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2->read()) {
//cerr << "ConvertTwoSourcesToSinkWithFilter::read(void) success\n" << flush;
				size_t n1=FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1->getBufferCount();
//cerr << "ConvertTwoSourcesToSinkWithFilter::read(void) source 1: " << n1 << " values\n" << flush;
				const Tin1 *inptr1=FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1->getBuffer();
				size_t n2=FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2->getBufferCount();
//cerr << "ConvertTwoSourcesToSinkWithFilter::read(void) source 2: " << n2 << " values\n" << flush;
				const Tin2 *inptr2=FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2->getBuffer();
				while (n1-- && n2--) FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::buffer.add((Tout)(filter->filter(*inptr1++,*inptr2++)));
			}
//cerr << "ConvertTwoSourcesToSinkWithFilter::read(void) return " << getBufferCount() << " values\n" << flush;
			return FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::getBufferCount();
		}

	int good(void) const	{
//cerr << "ConvertTwoSourcesToSinkWithFilter::good(void)\n" << flush;
		//Assert(FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1);
		//Assert(FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2);
		return FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src1->good() && FilterTwoSourcesToSinkBufferedBase<Tin1,Tin2,Tout>::src2->good();
	}
};

// our versions of filter ...

class OurCombineFilter : public CombineFilter<Uint16,Uint16,Uint16> {
private:
	double firstMultiplier;
	double secondMultiplier;
	Uint16 secondOffset;
	Uint16 thresholdSecond;
public:
	OurCombineFilter(Uint16 useBitsFirst,Uint16 useBitsSecond,Uint16 thresholdSecond,Uint16 newFirstHighestValue,Uint16 newBits)
			: CombineFilter<Uint16,Uint16,Uint16>()
		{
			Assert(useBitsFirst > 0);
			Assert(useBitsSecond > 0);
			Assert(newFirstHighestValue > 0);
			Assert(newBits > 0);
			Assert(useBitsFirst <= 16);
			Assert(useBitsSecond <= 16);
			Assert(newFirstHighestValue <= 0xffff);
			Assert(newBits <= 16);
			
			Assert(newFirstHighestValue < 1l<<newBits);
			
			Assert(thresholdSecond < 1l<<useBitsSecond);
			this->thresholdSecond=thresholdSecond;

			firstMultiplier=double(newFirstHighestValue)/(1l<<useBitsFirst);
			secondMultiplier=(double(1l<<newBits) - 1 - newFirstHighestValue)/(1l<<useBitsSecond);
			secondOffset=newFirstHighestValue+1;
//cerr << "OurCombineFilter::OurCombineFilter() firstMultiplier = " << dec << firstMultiplier << endl;
//cerr << "OurCombineFilter::OurCombineFilter() secondMultiplier = " << dec << secondMultiplier << endl;
//cerr << "OurCombineFilter::OurCombineFilter() secondOffset = " << dec << secondOffset << endl;

//cerr << "OurCombineFilter::OurCombineFilter() lowest first = " << dec << filter(0,0) << endl;
//cerr << "OurCombineFilter::OurCombineFilter() highest first = " << dec << filter(Uint16(1l<<useBitsFirst)-1,0) << endl;
//cerr << "OurCombineFilter::OurCombineFilter() lowest second = " << dec << filter(0,thresholdSecond) << endl;
//cerr << "OurCombineFilter::OurCombineFilter() highest second = " << dec << filter(0,Uint16(1l<<useBitsSecond)-1) << endl;
//cerr << "OurCombineFilter::OurCombineFilter() count of possible second values after offset = " << dec << (filter(0,Uint16(1l<<useBitsSecond)-1) - secondOffset + 1) << endl;
//cerr << "OurCombineFilter::OurCombineFilter() count of possible second values after threshold = " << dec << (filter(0,Uint16(1l<<useBitsSecond)-1) - filter(0,thresholdSecond) + 1) << endl;
		}

	Uint16 filter(Uint16 value1,Uint16 value2)
		{
			return Uint16(value2 >= thresholdSecond ? value2*secondMultiplier+secondOffset : value1*firstMultiplier);
		}
};

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

static const char *
getStringValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	const char *value=AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	return a ? value : "";
}

static Uint16
getIntegerValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	Uint16 value=AttributeValue(a);
	return value;
}

static Uint16
getMatchingIntegerValueElseError(AttributeList &list1,AttributeList &list2,Tag tag,const char *label,const char *filename1,const char *filename2,TextOutputStream &log) {
	Uint16 v1 = getIntegerValueElseError(list1,tag,label,filename1,log);
	Uint16 v2 = getIntegerValueElseError(list2,tag,label,filename2,log);
	if (v1 != v2) {
		log << EMsgDC(DifferentValues)
		    << " - " << label
		    << " (" << dec << v1 << "," << v2 << ")"
		    << endl;
		return 0;
	}
	else {
		return v1;
	}
}


int
main(int argc, char *argv[])
{
	bool bad = false;
	
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options1(options);
	DicomInputOptions 	dicom_input_options2(options);
	DicomOutputOptions 	dicom_output_options(options);

	int useBitsFirst;
	int useBitsSecond;
	int thresholdSecond;
	int newFirstHighestValue;
	int newBits;

	bad = bad || !options.get("usebitsfirst",useBitsFirst);
	bad = bad || !options.get("usebitssecond",useBitsSecond);
	bad = bad || !options.get("thresholdsecond",thresholdSecond);
	bad = bad || !options.get("newfirsthighestvalue",newFirstHighestValue);
	bad = bad || !options.get("newbits",newBits);

	bool verbose=options.get("verbose") || options.get("v");

	dicom_input_options1.done();
	dicom_input_options2.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener1(
		options,dicom_input_options1.filename);
	DicomInputOpenerFromOptions input_opener2(
		options,dicom_input_options2.filename);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options1.errors();
	cerr << dicom_input_options2.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener1.errors();
	cerr << input_opener2.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options1.good()
	 || !dicom_input_options2.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener1.good()
	 || !input_opener2.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options1.usage()
			<< dicom_input_options2.usage()
			<< dicom_output_options.usage()
			<< " -usebitsfirst n"
			<< " -usebitssecond n"
			<< " -thresholdsecond n"
			<< " -newfirsthighestvalue n"
			<< " -newbits n"
			<< " [-v|-verbose]"
			<< MMsgDC(InputFile) << " " << MMsgDC(InputFile)
			<< "[" << MMsgDC(OutputFile) << "]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din1(*(istream *)input_opener1,
		dicom_input_options1.transfersyntaxuid,
		dicom_input_options1.usemetaheader);
	DicomInputStream din2(*(istream *)input_opener2,
		dicom_input_options2.transfersyntaxuid,
		dicom_input_options2.usemetaheader);
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);
		
	const char *file1 = dicom_input_options1.filename ? dicom_input_options1.filename : "1st file";
	const char *file2 = dicom_input_options2.filename ? dicom_input_options2.filename : "2nd file";

	ManagedAttributeList list1;
	ManagedAttributeList list2;
	
	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading 1st dataset ... ********" << endl; 
	list1.read(din1,&log,verbose,0xffffffff,true,dicom_input_options1.uselengthtoend,dicom_input_options1.ignoreoutofordertags);
	if (verbose) log << "******** While reading 2nd dataset ... ********" << endl; 
	list2.read(din2,&log,verbose,0xffffffff,true,dicom_input_options2.uselengthtoend,dicom_input_options2.ignoreoutofordertags);

	if (!list1.good()) {
		log << list1.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	if (!list2.good()) {
		log << list2.errors()
		    << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}
	Uint16 rows;
	Uint16 cols;
	if (success) {
		rows = getMatchingIntegerValueElseError(list1,list2,TagFromName(Rows),"Rows",file1,file2,log);
		cols = getMatchingIntegerValueElseError(list1,list2,TagFromName(Columns),"Columns",file1,file2,log);
		if (!rows || !cols) success=false;
	}
	if (success) {
		Uint16 frames = 1;
		Uint16 samples = 1;
		Uint16 bitsAllocated = 16;
		Uint16 bitsStored = 16;
		Uint16 highBit = 15;

		Attribute *aPixelData1 = list1[TagFromName(PixelData)];
		Attribute *aPixelData2 = list2[TagFromName(PixelData)];
	
		aPixelData1->castToOtherData()->activateSource();
		class SourceBase<Uint16> *srcpixeldata1 = aPixelData1->castToOtherData()->get16BitSource();
		Assert(srcpixeldata1);
		aPixelData2->castToOtherData()->activateSource();
		class SourceBase<Uint16> *srcpixeldata2 = aPixelData2->castToOtherData()->get16BitSource();
		Assert(srcpixeldata2);

		CombineFilter<Uint16,Uint16,Uint16> *filter = new OurCombineFilter(useBitsFirst,useBitsSecond,thresholdSecond,newFirstHighestValue,newBits);
	
		class SourceBase<Uint16> *srcpixeldatatouse = new ConvertTwoSourcesToSinkWithFilter<Uint16,Uint16,Uint16>(*srcpixeldata1,*srcpixeldata2,filter);
		TransferSyntax transfersyntax(
			dicom_output_options.transfersyntaxuid
			? dicom_output_options.transfersyntaxuid
			: DefaultTransferSyntaxUID);

		list1-=TagFromName(PixelData);
		list1+=new OtherUnspecifiedLargeAttribute(
			TagFromName(PixelData),
			srcpixeldatatouse,
			rows,
			cols,
			frames,
			samples,
			&transfersyntax,
			0,bitsAllocated,bitsStored,highBit);

		if (!usualManagedAttributeListWrite(list1,dout,
			dicom_output_options,log,verbose)) success=false;
	}

	return success ? 0 : 1;
}

	
