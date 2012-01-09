#include "attrmxls.h"
#include "attrtype.h"
#include "attrothr.h"
#include "attrval.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"
#include "memsrc.h"

static Int16 *
createSubtractedImage(ManagedAttributeList& list1,ManagedAttributeList& list2,Uint16 &rows,Uint16 &columns,bool clamptozero,TextOutputStream& log)
{
	Int16 *result = 0;
	bool success = true;

	// first image ...

	Attribute *aRows1 = list1[TagFromName(Rows)];
	if (!aRows1)
		log << EMsgDC(MissingAttribute) << " - \"Rows\"" << endl;
	Uint16 vRows1=AttributeValue(aRows1);

	Attribute *aColumns1 = list1[TagFromName(Columns)];
	if (!aColumns1)
		log << EMsgDC(MissingAttribute) << " - \"Columns\"" << endl;
	Uint16 vColumns1=AttributeValue(aColumns1);

	Attribute *aPixelData1 = list1[TagFromName(PixelData)];
	if (!aPixelData1)
		log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;

	// second image ...

	Attribute *aRows2 = list2[TagFromName(Rows)];
	if (!aRows2)
		log << EMsgDC(MissingAttribute) << " - \"Rows\"" << endl;
	Uint16 vRows2=AttributeValue(aRows2);

	Attribute *aColumns2 = list2[TagFromName(Columns)];
	if (!aColumns2)
		log << EMsgDC(MissingAttribute) << " - \"Columns\"" << endl;
	Uint16 vColumns2=AttributeValue(aColumns2);

	Attribute *aPixelData2 = list2[TagFromName(PixelData)];
	if (!aPixelData2)
		log << EMsgDC(MissingAttribute) << " - \"PixelData\"" << endl;

	if (!aPixelData1 || !aRows1 || !aColumns1 || !aPixelData2 || !aRows2 || !aColumns2) {
		success=false;
	}
	else {
		if (vRows1 != vRows2) {
			log << "Different number of Rows" << endl;
			success=false;
		}
		rows=vRows1;
		if (vColumns1 != vColumns2) {
			log << "Different number of Rows" << endl;
			success=false;
		}
		columns=vColumns1;
	}

	if (success) {
		SupplySourceFromAttribute sPixelData1(aPixelData1);
		class SourceBase<Uint16> *source1 = sPixelData1.getSource();
		if (!source1) {
			log << EMsgDC(BadAttributeValue)
			    << " - \"PixelData\""
			    << endl;
			success=false;
		}

		SupplySourceFromAttribute sPixelData2(aPixelData2);
		class SourceBase<Uint16> *source2 = sPixelData2.getSource();
		if (!source2) {
			log << EMsgDC(BadAttributeValue)
			    << " - \"PixelData\""
			    << endl;
			success=false;
		}

		if (success) {
			result = new Int16[rows*columns];
			Assert(result);

			Int16 *result_ptr=result;
			unsigned row=0;
			unsigned column=0;
			size_t n1=0;
			size_t n2=0;
			const Uint16 *buffer1;
			const Uint16 *buffer2;

			while (row < rows) {
				if (n1 <= 0) {
					if (!source1->read()) {
						log << "premature end of data 1" << endl;
						success=false;
						break;
					}
					n1=source1->getBufferCount();
					buffer1=source1->getBuffer();
				}
				Assert(n1);
				Assert(buffer1);

				if (n2 <= 0) {
					if (!source2->read()) {
						log << "premature end of data 2" << endl;
						success=false;
						break;
					}
					n2=source2->getBufferCount();
					buffer2=source2->getBuffer();
				}
				Assert(n1);
				Assert(buffer1);

				Uint16 symbol1=*buffer1++;
				--n1;
				Uint16 symbol2=*buffer2++;
				--n2;

				if (clamptozero) {
					Int16 value=symbol2-symbol1;
					if (value < 0) value=0;
					*result_ptr++=value;
				}
				else {
					*result_ptr++=symbol2-symbol1;
				}

				if (++column >= columns) {
					column=0;
					++row;
				}
			}

			Assert(column == 0 && row == rows);

			success=true;
		}
	}

	return success ? result : 0;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool ignorereaderrors=options.get("ignorereaderrors");
	bool clamptozero=options.get("clamptozero");

	dicom_input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener1(
		options,dicom_input_options.filename,cin);
	DicomInputOpenerFromOptions input_opener2(
		options,0,cin);
	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener1.errors();
	cerr << input_opener2.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener1.good()
	 || !input_opener2.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< " [-clamptozero]"
			<< " [-ignorereaderrors]"
			<< " "  << MMsgDC(InputFile)
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din1(*(istream *)input_opener1,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);
	DicomInputStream din2(*(istream *)input_opener2,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);
	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	TransferSyntax transfersyntax(
		dicom_output_options.transfersyntaxuid
		? dicom_output_options.transfersyntaxuid
		: DefaultTransferSyntaxUID);

	ManagedAttributeList list1;
	ManagedAttributeList list2;

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading file 1 ... ********" << endl; 
	list1.read(din1,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags);

	if (!list1.good()) {
		if (!ignorereaderrors) {
			log << list1.errors();
			success=false;
		}
		log << EMsgDC(DatasetReadFailed) << endl;
	}
	if (verbose) log << "******** While reading file 2 ... ********" << endl; 
	list2.read(din2,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags);

	if (!list2.good()) {
		if (!ignorereaderrors) {
			log << list2.errors();
			success=false;
		}
		log << EMsgDC(DatasetReadFailed) << endl;
	}

	if (success) {
		Uint16 rows;
		Uint16 columns;
		Int16 *result=createSubtractedImage(list1,list2,rows,columns,clamptozero,log);

		if (result) {
			list1-=TagFromName(SOPInstanceUID);
			list1-=TagFromName(SeriesInstanceUID);
			list1-=TagFromName(StudyInstanceUID);

			list1-=TagFromName(SeriesNumber);
			list1+=new IntegerStringAttribute(TagFromName(SeriesNumber));

			// output of subtraction is signed

			list1-=TagFromName(PixelRepresentation);
			list1+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),1);
	
			list1-=TagFromName(PixelData);

			Memory_PixelDataSource<Uint16> *pixeldatasrc = new Memory_PixelDataSource<Uint16>(rows,columns,(Uint16 *)result);
			Assert(pixeldatasrc);

			list1+=new OtherUnspecifiedLargeAttribute(
				TagFromName(PixelData),
				pixeldatasrc,
				rows,
				columns,
				1,
				1,
				&transfersyntax,
				0,16,16,15);

			// Don't write list warnings yet ...
			// done in usualManagedAttributeListWrite ...

			if (!usualManagedAttributeListWrite(list1,dout,
				dicom_output_options,log,verbose)) success=false;
		}
		else {
			success=false;
		}
	}

	return success ? 0 : 1;
}

	
