#include "bnstream.h"
#include "bnopt.h"
#include "mesgtext.h"

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions				options(argc,argv);
	BinaryInputOptionsWithByteOrder		input_options(options);
	BinaryOutputOptionsWithByteOrder	output_options(options);

	unsigned bits=0;
	if (!options.get("bits",bits) && !options.get("depth",bits)) {
		//cerr << EMsgDC(NeedOption) << " - bits" << endl;
		bad=true;
	}

	input_options.done();
	output_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	BinaryOutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	cerr << input_options.errors();
	cerr << output_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< output_options.usage()
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream in(*(istream *)input_opener,input_options.byteorder);
	BinaryOutputStream out(*(ostream *)output_opener,output_options.byteorder);


	bool success;

	Assert(bits <= 16);
	if (bits <= 8) {
		unsigned char mask = (unsigned char)((Uint32(1)<<bits)-1u);
		while (!in.eof()) {
			unsigned char value;
			in >> value;
			if (in.eof()) { success=true; break; }
			if (!in) { success=false; break; }
			out << (unsigned char)(value&mask);
			if (!out) { success=false; break; }
		}
	}
	else {
		Uint16 mask = Uint16((Uint32(1)<<bits)-1u);
		while (!in.eof()) {
			Uint16 value;
			in >> value;
			if (in.eof()) { success=true; break; }
			if (!in) { success=false; break; }
			out << Uint16(value&mask);
			if (!out) { success=false; break; }
		}
	}

	return success ? 0 : 1;
}

