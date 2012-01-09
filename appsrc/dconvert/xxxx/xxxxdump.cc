#include "basetype.h"
#include "bnstream.h"
#include "txstream.h"
#include "mesgtext.h"
#include "bnopt.h"

#include "xxxx.h"

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);
	BinaryInputOptions 	binary_input_options(options);

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
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< binary_input_options.usage()
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream bin(*(istream *)input_opener);

	XXXX_Conversion convertor(bin,cerr);
	convertor.dumpCommon(cout);
	convertor.dumpSelectedImage(cout,imagenumber);

	return 0;
}

