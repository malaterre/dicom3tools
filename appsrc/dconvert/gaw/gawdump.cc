#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "mesgtext.h"
#include "bnopt.h"

#include "gaw.h"

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);

	BinaryInputOptions 	binary_input_options(options);

	int useoptioncount=0;

	bool useawct=options.get("awct");
	if (useawct) ++useoptioncount;

	bool useawmr=options.get("awmr");
	if (useawmr) ++useoptioncount;

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

	// there is no default ...

	if (!useoptioncount && noffset == -1) {
		cerr << EMsgDC(NeedOption) << " -awct|-awmr|-offset" << endl;
		bad=true; 
	}

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
			<< " -awct|-awmr"
			<< "|[-offset file suite exam series image [-lengthpresent]]]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream bin(*(istream *)input_opener);

	GAW_Conversion *convertor=0;
	if (noffset == 5)
		convertor=new GAW_Conversion(bin,cerr,
			true,
			lengthpresent,
			offsetargs[0],
			offsetargs[1],
			offsetargs[2],
			offsetargs[3],
			offsetargs[4]);
	else if (useawct)
		convertor=new GAW_Conversion(bin,cerr,
			true,	// explicit
			false,	// length field
			3240,	// file header
			0,	// suite header
			116,	// exam header
			1156,	// series header
			2184);	// image header
	else if (useawmr)
		convertor=new GAW_Conversion(bin,cerr,
			true,	// explicit
			false,	// length field
			3228,	// file header
			0,	// suite header
			116,	// exam header
			1156,	// series header
			2184);	// image header
	Assert(convertor);

	convertor->dumpCommon(cout);
	convertor->dumpSelectedImage(cout);

	return 0;
}

