#include "attrmxls.h"
#include "attrothr.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"

class BurnInOverlaysFilter : public PointFilterDependentOnOffset<Uint16,Uint16> {
private:
	Uint16 valuetoburnin;
	const Uint16 *graphics_buffer;
	unsigned long graphics_buffer_size;
public:
	BurnInOverlaysFilter(int v,const Uint16 *b,unsigned long s)
			: PointFilterDependentOnOffset<Uint16,Uint16>()
		{
			valuetoburnin=Uint16(v);
			graphics_buffer=b;
			graphics_buffer_size=s;
//cerr << "BurnInOverlaysFilter::BurnInOverlaysFilter(): v=" << dec << v << endl;
//cerr << "BurnInOverlaysFilter::BurnInOverlaysFilter(): value=0x" << hex << value << dec << endl;
		}

	Uint16 filter(Uint16 originalvalue,size_t offset)
		{
			unsigned long wordOffset = offset/16;
			Assert(wordOffset < graphics_buffer_size);
			Uint16 bit = (graphics_buffer[wordOffset] >> (offset%16)) & 0x0001;
		
			return bit ? valuetoburnin : originalvalue;
		}
};

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	int valuetoburnin=0xffff;
	options.get("value",valuetoburnin);

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
			<< " [-value value]"
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
	
		PointFilterDependentOnOffset<Uint16,Uint16> *filter = 0;

		// should check Tag(OverlayRows_GROUP+u,OverlayRows_ELEMENT) = Rows
		// should check Tag(OverlayColumns_GROUP+u,OverlayColumns_ELEMENT) = Rows
		// should check Tag(OverlayType_GROUP+u,OverlayType_ELEMENT) = "G"
		// should check Tag(OverlayBitsAllocated_GROUP+u,OverlayBitsAllocated_ELEMENT) = 1
		// should check Tag(OverlayBitPosition_GROUP+u,OverlayBitPosition_ELEMENT) = 0
		// should check Tag(OverlayOrigin_GROUP+u,OverlayOrigin_ELEMENT) = 1\1
	
		unsigned u=0x0000;	// Burn in only Overlay Group 0x6000 for now
		Attribute *aOverlayData = list[Tag(OverlayData_GROUP+u,OverlayData_ELEMENT)];
		if (aOverlayData) {
			unsigned long graphics_buffer_size;
			const Uint16 *graphics_buffer;
			aOverlayData->getValue(graphics_buffer,graphics_buffer_size);
			Assert(graphics_buffer);
			
			filter = new BurnInOverlaysFilter(valuetoburnin,graphics_buffer,graphics_buffer_size);
			Assert(filter);
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
				if (filter) oPixelData->insertPixelPointTransform(filter);
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

	
