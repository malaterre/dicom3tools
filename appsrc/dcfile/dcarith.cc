#include "attrmxls.h"
#include "attrothr.h"
#include "attrval.h"
#include "attrtype.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

class MaskHighBitFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	Uint16 mask;
public:
	MaskHighBitFilter(unsigned lowbitstokeep)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			Assert(lowbitstokeep > 0);
			Assert(lowbitstokeep <= 16);
			mask = Uint16((Uint32(1)<<lowbitstokeep)-1u);
//cerr << "MaskHighBitFilter::MaskHighBitFilter(): lowbitstokeep=" << dec << lowbitstokeep << endl;
//cerr << "MaskHighBitFilter::MaskHighBitFilter(): mask=0x" << hex << mask << dec << endl;
		}

	Uint16 filter(Uint16 value)
		{
			return value&mask;
		}
};

class AddValueOffsetFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	int offset;
public:
	AddValueOffsetFilter(int o)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			offset=o;
//cerr << "AddValueOffsetFilter::AddValueOffsetFilter(): offset=" << dec << offset << endl;
		}

	Uint16 filter(Uint16 value)
		{
			return (Uint16(value+offset));
		}
};

class ZeroValueFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	Uint16 valuetozero;
public:
	ZeroValueFilter(int v)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			valuetozero=Uint16(v);
//cerr << "ZeroValueFilter::ZeroValueFilter(): v=" << dec << v << endl;
//cerr << "ZeroValueFilter::ZeroValueFilter(): valuetozero=0x" << hex << valuetozero << dec << endl;
		}

	Uint16 filter(Uint16 value)
		{
			return value==valuetozero ? Uint16(0) : value;
		}
};

class ReplaceValueFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	Uint16 valuetostartreplace;
	Uint16 valuetoendreplace;
	Uint16 replacementvalue;
public:
	ReplaceValueFilter(int rs,int re,int v)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			valuetostartreplace=Uint16(rs);
			valuetoendreplace=Uint16(re);
			replacementvalue=Uint16(v);
		}

	Uint16 filter(Uint16 value)
		{
			return (value >= valuetostartreplace && value <= valuetoendreplace) ? replacementvalue : value;
		}
};

class ScaleValueFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	float scalefactor;
public:
	ScaleValueFilter(float s)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			scalefactor=s;
		}

	Uint16 filter(Uint16 value)
		{
			return value=(Uint16)(scalefactor*(float)value);
		}
};

class MakeIdentityRescaleFilter : public PointFilterIndependentOfOffset<Uint16,Uint16> {
private:
	Uint16 maskInput;
	Uint16 maskOutput;
	float slope;
	float intercept;
public:
	MakeIdentityRescaleFilter(float rescaleslope,float rescaleintercept,unsigned inputBits,unsigned outputBits)
			: PointFilterIndependentOfOffset<Uint16,Uint16>()
		{
			Assert(inputBits > 8);
			Assert(inputBits <= 16);
			maskInput = Uint16((Uint32(1)<<inputBits)-1u);
			Assert(outputBits > 8);
			Assert(outputBits <= 16);
			maskOutput = Uint16((Uint32(1)<<outputBits)-1u);
			
			slope = rescaleslope;
			intercept = rescaleintercept;
		}

	Uint16 filter(Uint16 value)
		{
			// assumes input is always unsigned (doesn't sign extend beyond mask)
			float hu = (slope*(float)(value&maskInput)) + intercept;
			return value=(Uint16)(hu)&maskOutput;
		}
};

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool bad = false;
	
	unsigned bits=0;
	bool operation_maskhighbits=options.get("maskhighbits",bits);

	int offset=0;
	bool operation_addvalueoffset=options.get("addvalueoffset",offset);

	int valuetozero=0;
	bool operation_zerovalue=options.get("zerovalue",valuetozero);

	int valuetostartreplace = 0;
	int valuetoendreplace = 0;
	int replacementvalue = 0;
	bool operation_replacevalue=false;
	{
		int replacevalueArgs[3];
		int n = options.get("replacevalue",replacevalueArgs,3);
		if (n > 0) {
			if (n == 3) {
				operation_replacevalue=true;
				valuetostartreplace = replacevalueArgs[0];
				valuetoendreplace = replacevalueArgs[1];
				replacementvalue = replacevalueArgs[2];
			}
			else {
				cerr << "replacevalue needs 3 arguments, not " << dec << n << endl;
				bad=true;
			}
		}
	}

	float scalefactor=1.0;
	bool operation_scale=options.get("scale",scalefactor);

	bool makeidentityrescale=options.get("makeidentityrescale");

	Assert(!(operation_maskhighbits && operation_addvalueoffset && operation_zerovalue && operation_replacevalue && operation_scale));

	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");
	bool pixelpaddingvalue=options.get("pixelpaddingvalue");
	if (makeidentityrescale) {
		pixelpaddingvalue = true;
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
			<< " [-v|-verbose]"
			<< " [-ignorereaderrors]"
			<< " [-pixelpaddingvalue]"
			<< " [-maskhighbits bits]"
			<< " [-addvalueoffset offset]"
			<< " [-zerovalue value]"
			<< " [-replacevalue startvalue endvalue newvalue]"
			<< " [-scale scalefactor]"
			<< " [-makeidentityrescale]"
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

	if (success) {
		PointFilterIndependentOfOffset<Uint16,Uint16> *filter = 0;
		if (operation_maskhighbits) {
			filter = new MaskHighBitFilter(bits);
		}
		else if (operation_addvalueoffset) {
			filter = new AddValueOffsetFilter(offset);
		}
		else if (operation_zerovalue) {
			filter = new ZeroValueFilter(valuetozero);
		}
		else if (operation_replacevalue) {
			filter = new ReplaceValueFilter(valuetostartreplace,valuetoendreplace,replacementvalue);
		}
		else if (operation_scale) {
			filter = new ScaleValueFilter(scalefactor);
		}
		else if (makeidentityrescale) {
			Uint16 vBitsStored = AttributeValue(list[TagFromName(BitsStored)]);
			float vRescaleSlope = AttributeValue(list[TagFromName(RescaleSlope)]);
			float vRescaleIntercept = AttributeValue(list[TagFromName(RescaleIntercept)]);
			Uint16 newBitsStored = 16;
			filter = new MakeIdentityRescaleFilter(vRescaleSlope,vRescaleIntercept,vBitsStored,newBitsStored);
			list -= TagFromName(RescaleSlope);
			list -= TagFromName(RescaleIntercept);
			list -= TagFromName(PixelRepresentation);
			list -= TagFromName(BitsStored);
			list -= TagFromName(HighBit);
			list += new DecimalStringAttribute(TagFromName(RescaleSlope),1.0);
			list += new DecimalStringAttribute(TagFromName(RescaleIntercept),0.0);
			list += new UnsignedShortAttribute(TagFromName(PixelRepresentation),1);
			list += new UnsignedShortAttribute(TagFromName(BitsStored),newBitsStored);
			list += new UnsignedShortAttribute(TagFromName(HighBit),newBitsStored-1);
			// NB. do not forget to setSuppressScalingOnBitDepthConversion() later, else any switch in bit depth will cause undesirable scaling
		}
		Assert(filter);
		
		if (pixelpaddingvalue) {
			Attribute *aPixelPaddingValue = list[TagFromName(PixelPaddingValue)];
			if (aPixelPaddingValue) {
				Uint16 vPixelPaddingValue = AttributeValue(aPixelPaddingValue);
				vPixelPaddingValue = filter->filter(vPixelPaddingValue);
				list -= TagFromName(PixelPaddingValue);
				list += new UnspecifiedShortAttribute(TagFromName(PixelPaddingValue),vPixelPaddingValue);
			}
		}

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
				oPixelData->insertPixelPointTransform(filter);
				oPixelData->setSuppressScalingOnBitDepthConversion(makeidentityrescale);
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

	
