#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"

bool
usualManagedAttributeListWrite(
	ManagedAttributeList& list,
	DicomOutputStream& dout,
	DicomOutputOptions& dicom_output_options,
	TextOutputStream& log,
	bool verbose)
{
	ManagedAttributeList::flag_types flags=ManagedAttributeList::flag_types(
		 (dicom_output_options.usemetaheader     ? ManagedAttributeList::metaheader:0)
		|(dicom_output_options.writedataset      ? ManagedAttributeList::dataset:0)
		|(dicom_output_options.addlengths        ? ManagedAttributeList::addlengths:0)
		|(dicom_output_options.addlengthtoend    ? ManagedAttributeList::addlengthtoend:0)
		|(dicom_output_options.removeprivate     ? ManagedAttributeList::removeprivate:0)
		|(dicom_output_options.removeinstanceuid ? ManagedAttributeList::removeinstanceuid:0)
		|(dicom_output_options.adddicom          ? ManagedAttributeList::adddicom:0)
		|(dicom_output_options.addtiff           ? ManagedAttributeList::addtiff:0)
		|(dicom_output_options.adddisclaimer     ? ManagedAttributeList::adddisclaimer:0)
		|(dicom_output_options.disambiguateseriesbydescription ? ManagedAttributeList::disambiguateseriesbydescription:0)
	);

	if (verbose) {
		log << "******** As read ... ********" << endl;
		log << list;
	}

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

	if (!list.prepare(dout,flags,ManagedAttributeList::none,dicom_output_options.stamp,
			dicom_output_options.instancecreationdate,
			dicom_output_options.instancecreationtime,
			dicom_output_options.timezoneoffsetfromutc)) {
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

	if (verbose) {
		log << "******** As written ... ********" << endl;
		log << list;
	}
	return true;
}

bool
getAttributeTagFromStringHexForm(
	const char *arg,
	Tag &tag)
{
	bool success=false;
	if (arg && arg[0] == '(') {			// handle "(0xgggg,0xeeee)"
		// based on ancreate.cc approach
		istrstream istr(arg+1);
		Uint16 group;
		istr >> resetiosflags(ios::basefield) >> group;
		if (istr.good()) {
			char c;
			istr >> c;
			if (istr.good() && c == ',') {
				Uint16 element;
				istr >> resetiosflags(ios::basefield) >> element;
				if (istr.good()) {
					tag=Tag(group,element);
					success=true;
				}
			}
		}
	}
	return success;
}
