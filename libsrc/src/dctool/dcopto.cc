#include <fstream.h>

#include "attr.h"
#include "attrlist.h"
#include "attrnew.h"
#include "elmdict.h"
#include "transynu.h"
#include "dcopt.h"
#include "mesgtext.h"

#ifndef REPLACESTANDARDIOBUFSIZE
#define REPLACESTANDARDIOBUFSIZE 16384
#endif

static const char *options_output_file[] = {
	"output-file",
	"of",
	0
};

static const char *options_output_nometaheader[] = {
	"output-nometa",
	"nometa",
	"n",
	0
};

static const char *options_output_justmeta[] = {
	"output-justmeta",
	"justmeta",
	0
};

static const char *options_output_implicitmetaheader[] = {
	"output-implicitmeta",
	"implicitmeta",
	0
};

static const char *options_output_transfersyntax[] = {
	"output-transfersyntax",
	"output-ts",
	"ts",
	0
};

static const char *options_output_defaulttransfersyntax[] = {
	"output-default",
	"default",
	0
};

static const char *options_output_byteorder[] = {
	"output-byteorder",
	"output-endian",
	"byteorder",
	"endian",
	0
};

static const char *options_output_vr[] = {
	"output-vr",
	"vr",
	0
};

static const char *options_output_stamp[] = {
	"stamp",
	"s",
	0
};

static const char *options_output_instancecreationdate[] = {
	"instancecreationdate",
	0
};

static const char *options_output_instancecreationtime[] = {
	"instancecreationtime",
	0
};

static const char *options_output_timezoneoffsetfromutc[] = {
	"timezoneoffsetfromutc",
	0
};

static const char *options_output_addlengths[] = {
	"addlengths",
	0
};

static const char *options_output_addlengthtoend[] = {
	"addlengthtoend",
	0
};

static const char *options_output_addtiff[] = {
	"tiff",
	"tif",
	0
};

static const char *options_output_removeprivate[] = {
	"removeprivate",
	0
};

static const char *options_output_removeinstanceuid[] = {
	"removeinstanceuid",
	0
};

static const char *options_output_noadddicom[] = {
	"noadddicom",
	0
};

static const char *options_output_noadddisclaimer[] = {
"nodisclaimer",
0
};

static const char *options_output_disambiguateseriesbydescription[] = {
"disambiguateseriesbydescription",
0
};

static const char *options_output_replace_after[] = {
	"replace-after",
	"ra",
	0
};

static const char *options_output_replace_before[] = {
	"replace",
	"replace-before",
	"r",
	"rb",
	0
};

static const char *options_output_delete[] = {
	"delete",
	"d",
	0
};

DicomOutputOptions::DicomOutputOptions(GetNamedOptions &options)
		: ErrorsInClass()
{
	filename=0;
	options.get(options_output_file,filename);

	usemetaheader=!(options.get(options_output_nometaheader));

	writedataset=true;
	if (options.get(options_output_justmeta)) {
		usemetaheader=true;
		writedataset=false;
	}

	useimplicitmetaheader=(options.get(options_output_implicitmetaheader));

	if (!usemetaheader && useimplicitmetaheader) {
		good_flag=false;
	}

	transfersyntaxuid=0;
	options.get(options_output_transfersyntax,transfersyntaxuid);

	if (options.get(options_output_defaulttransfersyntax)) {
		if (transfersyntaxuid) {
			errorstream << EMsgDC(AlreadyHaveTransferSyntax)
				    << endl;
			good_flag=false;
		}
		else
			transfersyntaxuid=DefaultTransferSyntaxUID;
	}

	const char *byteorder=0;
	if (options.get(options_output_byteorder,byteorder)) {
		if (strcmp(byteorder,"big") != 0
		 && strcmp(byteorder,"little") != 0) {
			errorstream << EMsgDC(BadByteOrder) << endl;
			good_flag=false;
		}
	}
	if (byteorder && transfersyntaxuid) {
		errorstream << EMsgDC(AlreadyHaveTransferSyntax) << endl;
		good_flag=false;
	}

	const char *vr=0;
	if (options.get(options_output_vr,vr)) {
		if (strcmp(vr,"explicit") != 0
		 && strcmp(vr,"implicit") != 0) {
			errorstream << EMsgDC(BadByteOrder) << endl;
			good_flag=false;
			vr=0;
		}
	}
	if (vr && transfersyntaxuid) {
		errorstream << EMsgDC(AlreadyHaveTransferSyntax) << endl;
		good_flag=false;
		vr=0;
	}

	if ((byteorder && !vr) || (vr && !byteorder)) {
		errorstream << EMsgDC(NeedBothVRAndByteOrder) << endl;
		good_flag=false;
	}

	if (!transfersyntaxuid && byteorder && vr) {
		if (strcmp(byteorder,"little") == 0) {
			if (strcmp(vr,"explicit") == 0)
				transfersyntaxuid=
					ExplicitVRLittleEndianTransferSyntaxUID;
			else
				transfersyntaxuid=
					ImplicitVRLittleEndianTransferSyntaxUID;
		}
		else {
			if (strcmp(vr,"explicit") == 0)
				transfersyntaxuid=
					ExplicitVRBigEndianTransferSyntaxUID;
			else {
				errorstream << EMsgDC(BadImplicitBig) << endl;
				good_flag=false;
			}
		}
	}

	if (!transfersyntaxuid && !byteorder && !vr) {
		transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;
	}

	stamp=0;
	options.get(options_output_stamp,stamp);
	instancecreationdate=0;
	options.get(options_output_instancecreationdate,instancecreationdate);
	instancecreationtime=0;
	options.get(options_output_instancecreationtime,instancecreationtime);
	timezoneoffsetfromutc=0;
	options.get(options_output_timezoneoffsetfromutc,timezoneoffsetfromutc);

	addlengths=options.get(options_output_addlengths);
	addlengthtoend=options.get(options_output_addlengthtoend);
	addtiff=options.get(options_output_addtiff);
	removeprivate=options.get(options_output_removeprivate);
	removeinstanceuid=options.get(options_output_removeinstanceuid);
	adddicom=!(options.get(options_output_noadddicom));
	adddisclaimer=!(options.get(options_output_noadddisclaimer));
	disambiguateseriesbydescription=options.get(options_output_disambiguateseriesbydescription);
//cerr << "DicomOutputOptions::DicomOutputOptions: disambiguateseriesbydescription = " << disambiguateseriesbydescription << endl;
	
	replacebeforelist=0;
	replaceafterlist=0;
	deletelist=0;

	ElementDictionary dictionary;

	const char *args[2];
	int n;
	while ((n=options.get(options_output_replace_before,args,2)) != -1) {
		if (n == 2) {
			Tag tag;
			if (getAttributeTagFromStringHexForm(args[0],tag)
			 || dictionary.getTag(args[0],tag)) {
				const char *vr=dictionary.getValueRepresentation(tag);
				if(!vr) vr="UN";
				Attribute *a=newAttribute(vr,tag);
				Assert(a);
				a->addValues(args[1]);
				if (!replacebeforelist) {
					replacebeforelist=new AttributeList;
					Assert(replacebeforelist);
				}
				(*replacebeforelist)+=a;
			}
			else {
				errorstream << "-" << options_output_replace_before[0] << ": "
					    << EMsgDC(UnrecognizedElement)
					    << " - \"" << args[0] << "\""
					    << endl;
				good_flag=false;
			}
		}
		else {
			errorstream << "-" << options_output_replace_before[0] << ": "
				    << EMsgDC(WantedElementAndDelimitedValues)
				    << " - \"" << args[0] << "\""
				    << endl;
			good_flag=false;
		}
	}

	while ((n=options.get(options_output_replace_after,args,2)) != -1) {
		if (n == 2) {
			Tag tag;
			if (getAttributeTagFromStringHexForm(args[0],tag)
			 || dictionary.getTag(args[0],tag)) {
				const char *vr=dictionary.getValueRepresentation(tag);
				if(!vr) vr="UN";
				Attribute *a=newAttribute(vr,tag);
				Assert(a);
				a->addValues(args[1]);
				if (!replaceafterlist) {
					replaceafterlist=new AttributeList;
					Assert(replaceafterlist);
				}
				(*replaceafterlist)+=a;
			}
			else {
				errorstream << "-" << options_output_replace_after[0] << ": "
					    << EMsgDC(UnrecognizedElement)
					    << " - \"" << args[0] << "\""
					    << endl;
				good_flag=false;
			}
		}
		else {
			errorstream << "-" << options_output_replace_after[0] << ": "
				    << EMsgDC(WantedElementAndDelimitedValues)
				    << " - \"" << args[0] << "\""
				    << endl;
			good_flag=false;
		}
	}

	const char *arg;
	while (options.get(options_output_delete,arg)) {
		Assert(arg);
		Tag tag;
		if (getAttributeTagFromStringHexForm(arg,tag)
		 || dictionary.getTag(arg,tag)) {
			const char *vr=dictionary.getValueRepresentation(tag);
			if(!vr) vr="UN";
			Attribute *a=newAttribute(vr,tag);
			Assert(a);
			if (!deletelist) {
				deletelist=new AttributeList;
				Assert(deletelist);
			}
			(*deletelist)+=a;
		}
		else {
			errorstream << "-" << options_output_delete[0] << ": "
				    << EMsgDC(UnrecognizedElement)
				    << " - \"" << arg << "\""
				    << endl;
			good_flag=false;
		}
	}
}

DicomOutputOptions::~DicomOutputOptions()
{
#ifdef TRACE_DESTRUCTORS
cerr << "DicomOutputOptions::~DicomOutputOptions" << endl;
#endif
	if (replaceafterlist) delete replaceafterlist;
	if (replacebeforelist) delete replacebeforelist;
	if (deletelist)  delete deletelist;
}

char *
DicomOutputOptions::usage(void)
{
	ostrstream ostr;
	const char **ptr;
	const char *lead;

	ostr << " [";
	for (ptr=options_output_nometaheader,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_justmeta,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_implicitmetaheader,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_transfersyntax,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(UID) << "]";

	ostr << " [";
	for (ptr=options_output_defaulttransfersyntax,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_byteorder,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " big|little]";

	ostr << " [";
	for (ptr=options_output_vr,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " implicit|explicit]";

	ostr << " [";
	for (ptr=options_output_stamp,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " uidstamp]";

	ostr << " [";
	for (ptr=options_output_instancecreationdate,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " date]";

	ostr << " [";
	for (ptr=options_output_instancecreationtime,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " time]";

	ostr << " [";
	for (ptr=options_output_timezoneoffsetfromutc,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " offset]";

	ostr << " [";
	for (ptr=options_output_addlengths,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_addlengthtoend,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_addtiff,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_removeprivate,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_removeinstanceuid,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_noadddicom,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_noadddisclaimer,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";
	
	ostr << " [";
	for (ptr=options_output_disambiguateseriesbydescription,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << "]";

	ostr << " [";
	for (ptr=options_output_replace_before,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " keyword delimitedvalues]";

	ostr << " [";
	for (ptr=options_output_replace_after,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " keyword delimitedvalues]";

	ostr << " [";
	for (ptr=options_output_delete,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " keyword]";

	ostr << " [";
	for (ptr=options_output_file,lead=""; *ptr; ++ptr,lead="|")
		ostr << lead << "-" << *ptr;
	ostr << " " << MMsgDC(OutputFile) << "]";

	ostr << ends;

	// string deletion becomes callers responsibility ...
	return ostr.str();
}

void
DicomOutputOptions::done(void)
{
}

DicomOutputOpenerFromOptions::DicomOutputOpenerFromOptions(
		GetNamedOptions &options,
		const char *filename,
		ostream &cstr)
	: ErrorsInClass()
{
	if (!filename && !options) {
		filename=options();
		++options;
	}
	filenameUsed=filename;

	if (filename) {
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ofstream *fstr=new ofstream(filename,
			ios::out|ios::trunc|ios::binary);
#else
		ofstream *fstr=new ofstream(filename,ios::out|ios::trunc);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			str=0;
		}
		else
			str=fstr;
	}
	else {
		streambuf *sbuf  = cstr.rdbuf();
		str=new ostream(sbuf);
		// default ANSI cin/cout is unbuffered, slow, so ...
//		char *buf=new char[REPLACESTANDARDIOBUFSIZE];
//		if (buf) sbuf->setbuf(buf,REPLACESTANDARDIOBUFSIZE);
		// this buffer will never be delete[]'d but doesn't
		// matter ... how often would we be reusing cin/cout ?
	}

	if (!str || !*str) {
		errorstream << AMsgDC(FileWriteOpenFailed);
		if (filename) errorstream <<" - \"" << filename << "\"";
		errorstream << endl;
		good_flag=false;
	}
}

DicomOutputOpenerFromOptions::~DicomOutputOpenerFromOptions()
{
#ifdef TRACE_DESTRUCTORS
cerr << "DicomOutputOpenerFromOptions::~DicomOutputOpenerFromOptions" << endl;
#endif
	if (str) delete str;
}

DicomOutputOpenerFromOptions::operator ostream *(void)
{
	return str;
}

