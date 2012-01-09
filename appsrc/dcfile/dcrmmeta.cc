#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <ctype.h>

#include "attrtag.h"
#include "dcstream.h"
//#include "elmconst.h"
//#include "elmdict.h"
#include "mesgtext.h"
#include "ioopt.h"
#include "dcopt.h"

static void copyBytes(istream& i,ostream& o) {
	const int bufsize=1024;
	char buffer[1024];
	int n;
	while (n=i.read(buffer,bufsize).gcount()) o.write(buffer,n);
}

static bool
readMeta(DicomInputStream& stream,TextOutputStream& log)
{
	if (stream.haveMetaHeader()) {
		Tag tag;
		stream >> tag;
		if (stream.fail()) {
			log << EMsgDC(TagReadFailed) << endl;
			return false;
		}
		// Get mandatory metaheader group length value ...
		Uint32 metaHeaderGroupLength = 0;
		if (tag.isMetaheaderGroup() && tag.getElement() == 0x0000) {
			char vr[3];
			Uint32 vl;
			if (stream.getTransferSyntaxInUse()->isExplicitVR()) {		// should always be
				stream.read(vr,2);
				if (stream.fail()) {
					log << EMsgDC(VRReadFailed) << endl;
					return false;
				}
				vr[2]=0;
				if (strcmp(vr,"UL") == 0) {
					vl=stream.read16();
				}
				else {
					log << EMsgDC(MetaHeaderGroupLengthNotUL) << endl;
					return false;
				}
			}
			else {
				log << WMsgDC(MetaHeaderIsImplicitVR) << endl;
				stream >> vl;
			}
			if (stream.fail()) {
				log << EMsgDC(VLReadFailed) << endl;
				return false;
			}
			if (vl == 4) {
				stream >> metaHeaderGroupLength;
				if (stream.fail()) {
					log << EMsgDC(MetaHeaderGroupLengthValueReadFailed) << endl;
					return false;
				}
				// skip it
				stream.seekg(metaHeaderGroupLength,ios::cur);
				if (stream.fail()) {
					log << EMsgDC(MetaHeaderReadFailed) << " - " << EMsgDC(SeekFailed) << endl;
					return false;
				}
				else {
					return true; // done
				}
			}
			else {
				log << EMsgDC(MetaHeaderGroupLengthHasWrongVL) << endl;
				return false;
			}
		}
		else {
			log << EMsgDC(MetaHeaderMissingGroupLength) << endl;
			return false;
		}			
	}
	else {
		return true;
	}
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	OutputOptions		output_options(options);

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

	if (!dicom_input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< output_options.usage()
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
	TextOutputStream log(cerr);

	bool success=readMeta(din,log);
	if (success) {
		copyBytes(din,out);
		if (!out.good()) {
			log << EMsgDC(Writefailed) << endl;
			success=false;
		}
	}

	return success ? 0 : 1;
}

	
