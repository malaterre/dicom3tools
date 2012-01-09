#ifndef __Header_dcopt__
#define __Header_dcopt__

#include "getoptns.h"
#include "attrtag.h"

class DicomInputOptions : public ErrorsInClass
{
public:
	const char *	filename;
	bool 		usemetaheader;
	bool 		uselengthtoend;
	bool		ignoreoutofordertags;
	bool		useUSVRForLUTDataIfNotExplicit;
	const char *	transfersyntaxuid;

	DicomInputOptions(GetNamedOptions &options);
	char *usage(void);
	void done(void);
};

class DicomOutputOptions : public ErrorsInClass
{
public:
	const char *	filename;
	bool 		usemetaheader;
	bool 		writedataset;
	bool 		useimplicitmetaheader;
	bool 		addlengths;
	bool 		addlengthtoend;
	bool 		removeprivate;
	bool 		removeinstanceuid;
	bool 		adddicom;
	bool 		addtiff;
	bool 		adddisclaimer;
	bool		disambiguateseriesbydescription;
	const char *	transfersyntaxuid;
	const char *	stamp;
	const char *	instancecreationdate;
	const char *	instancecreationtime;
	const char *	timezoneoffsetfromutc;
	class AttributeList *replaceafterlist;
	class AttributeList *replacebeforelist;
	class AttributeList *deletelist;

	DicomOutputOptions(GetNamedOptions &options);
	~DicomOutputOptions();
	char *usage(void);
	void done(void);
};

class DicomInputOpenerFromOptions : public ErrorsInClass {
	istream *str;
	const char *filenameUsed;
public:
	DicomInputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,istream &cstr);
	DicomInputOpenerFromOptions(GetNamedOptions &options,
		const char *filename);

	~DicomInputOpenerFromOptions();

	operator istream *(void);
	
	const char *getFilename() {return filenameUsed;}
};

class DicomOutputOpenerFromOptions : public ErrorsInClass {
	ostream *str;
	const char *filenameUsed;
public:
	DicomOutputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,ostream &cstr);

	~DicomOutputOpenerFromOptions();

	operator ostream *(void);
	
	const char *getFilename() {return filenameUsed;}
};

bool
usualManagedAttributeListWrite(
	class ManagedAttributeList& list,
	class DicomOutputStream& dout,
	class DicomOutputOptions& dicom_output_options,
	class TextOutputStream& log,
	bool verbose);

bool
getAttributeTagFromStringHexForm(
	const char *arg,
	Tag &tag);

#endif /* __Header_dcopt__ */

