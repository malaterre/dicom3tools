#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "mesgtext.h"
#include "bnopt.h"

#include "gen.h"

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);

	BinaryInputOptions 	binary_input_options(options);

	int useoptioncount=0;

	bool useximg=options.get("ximg");
	if (useximg) ++useoptioncount;

	bool usedat=options.get("dat");
	if (options.get("tape")) usedat=true;
	if (usedat) ++useoptioncount;

	int noffset;
	Uint32 offsetargs[5];
	if ((noffset=options.get("offset",offsetargs,5)) != -1 && noffset != 5) {
		cerr << EMsgDC(OptionIncomplete)
		     << " - offset"
		     << endl;
		bad=true;
	}

	bool lengthpresent=options.get("lengthpresent");

	if (useoptioncount && (lengthpresent || noffset != -1)) {
		cerr << EMsgDC(OptionsIncompatible) << endl;
		bad=true; 
	}

	// the default if no options is to read ximg format ...

	if (!useoptioncount && noffset == -1) useximg=true;

	binary_input_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener(
		options,binary_input_options.filename,cin);

	cerr << binary_input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!binary_input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< binary_input_options.usage()
			<< " [-ximg|-dat|-tape"
			<< "|[-offset file suite exam series image [-lengthpresent]]]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream bin(*(istream *)input_opener);

	GEN_Conversion *convertor=0;
	if (noffset == 5)
		convertor=new GEN_Conversion(bin,cerr,
			true,
			lengthpresent,
			offsetargs[0],
			offsetargs[1],
			offsetargs[2],
			offsetargs[3],
			offsetargs[4]);
	else if (usedat)
		convertor=new GEN_Conversion(bin,cerr,
			true,	// explicit
			true,	// length field
			3180,	// file header
			0,	// suite header
			114,	// exam header
			1138,	// series header
			2158);	// image header
	else if (useximg)
		convertor=new GEN_Conversion(bin,cerr);
	Assert(convertor);

	convertor->dumpCommon(cout);
	convertor->dumpSelectedImage(cout);

	return 0;
}

