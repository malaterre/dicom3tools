#include <fstream.h>

#include "transynu.h"
#include "attrmxls.h"
#include "attrval.h"
#include "elmconst.h"
#include "mesgtext.h"
#include "dcopt.h"

static bool
writeImageInSequence(Attribute *a,
	DicomOutputOptions& dicom_output_options,
	TextOutputStream& log,bool verbose)
{
        bool success=true;
	AttributeList **al;
	int n;
	if ((n=a->getLists(&al)) > 0) {
		int i;
		for (i=0; i<n; ++i) {
			if ((*al[i])[TagFromName(PixelData)] && (*al[i])[TagFromName(SOPInstanceUID)]) {
				ManagedAttributeList list;
				list+=(*al[i]);

				ostrstream ostr;
				if (dicom_output_options.filename)
					ostr << dicom_output_options.filename << ".";
				ostr << String_Use(AttributeValue(
						list[TagFromName(SOPInstanceUID)]))
				     << "." << i
				     << ends;
				char *name=ostr.str();
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
				ofstream out(name,ios::out|ios::trunc|ios::binary);
#else
				ofstream out(name,ios::out|ios::trunc);
#endif
				if (!out || !out.rdbuf()->is_open()) {
					log << EMsgDC(FileWriteOpenFailed)
					    << " - \"" << name << "\"" << endl;
					return false;
				}
				DicomOutputStream dout(out,
					dicom_output_options.transfersyntaxuid,
					dicom_output_options.usemetaheader,
					dicom_output_options.useimplicitmetaheader,
					dicom_output_options.addtiff);
#ifdef CRAP
				ManagedAttributeList::flag_types flags=ManagedAttributeList::flag_types(
	                        	 (dicom_output_options.usemetaheader     ? ManagedAttributeList::metaheader:0)
	                        	|(dicom_output_options.writedataset      ? ManagedAttributeList::dataset:0)
	                        	|(dicom_output_options.addlengths        ? ManagedAttributeList::addlengths:0)
	                        	|(dicom_output_options.addlengthtoend    ? ManagedAttributeList::addlengthtoend:0)
		                        |(dicom_output_options.removeprivate     ? ManagedAttributeList::removeprivate:0)
		                        |(dicom_output_options.removeinstanceuid ? ManagedAttributeList::removeinstanceuid:0)
		                        |(dicom_output_options.adddicom          ? ManagedAttributeList::adddicom:0)
		                        |(dicom_output_options.addtiff           ? ManagedAttributeList::addtiff:0)
		                        |(dicom_output_options.adddisclaimer     ? ManagedAttributeList::adddisclaimer:0));

				if (!list.clean(flags)) {
					cerr << list.errors() << flush;
					cerr << EMsgDC(DatasetWriteFailed) << endl;
					return false;
				}

	                        if (dicom_output_options.deletelist) {
	                        	list-=*(dicom_output_options.deletelist);
	                        	if (verbose) {
	                        		log << "******** After delete ... ********" << endl;
	                        		log << list;
	                        	}
	                        }

	                        if (dicom_output_options.replacebeforelist) {
	                        	list+=*(dicom_output_options.replacebeforelist);
		                        if (verbose) {
		                        	log << "******** After replace before... ********" << endl;
		                        	log << list;
		                        }
	                        }

				if (!list.prepare(dout,flags,ManagedAttributeList::none,dicom_output_options.stamp)) {
					cerr << list.errors() << flush;
					cerr << EMsgDC(DatasetWriteFailed) << endl;
					return false;
				}

	                        if (dicom_output_options.replaceafterlist) {
	                        	list+=*(dicom_output_options.replaceafterlist);
		                        if (verbose) {
		                        	log << "******** After replace after ... ********" << endl;
		                        	log << list;
		                        }
	                        }

				if (!list.finalize(dout)) {
					cerr << list.errors() << flush;
					cerr << EMsgDC(DatasetWriteFailed) << endl;
					return false;
				}

				if (!list.write(dout) || !list.good()) {
					cerr << list.errors() << flush;
					cerr << EMsgDC(DatasetWriteFailed) << endl;
					return false;
				}
				const char *errors=list.errors();
				if (errors) log << errors << flush;
#else
		                if (!usualManagedAttributeListWrite(list,dout,
			                dicom_output_options,log,verbose)) success=false;
#endif
				if (verbose) {
					log << "******** New file \"" << name << "\" ********" << endl;
					log << list;
				}
				if (name) delete[] name;
			}
		}
		delete [] al;
	}
	return success;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	dicom_input_options.done();
	dicom_output_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

#ifdef CRAP
	if (dicom_output_options.replacebeforelist
	 || dicom_output_options.replaceafterlist
	 || dicom_output_options.deletelist) {
		cerr << EMsgDC(NoReplaceOrDelete) << endl;
		exit(1);
	}
#endif
	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream  log(cerr);
	if (verbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	AttributeListIterator i(list);
	while (!i) {
		Attribute *a=i();
		Assert(a);
		if (strcmp(a->getVR(),"SQ") == 0) {
			if (!writeImageInSequence(a,dicom_output_options,log,verbose))
				success=false;
		}
		++i;
	}

	if (verbose) {
		log << "******** What is left ... ********" << endl;
		log << list;
	}

	return success ? 0 : 1;
}

