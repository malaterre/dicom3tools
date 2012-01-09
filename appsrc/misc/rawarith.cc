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

	bool usingsigned=options.get("signed");

	int offset;
	bool usingoffset=options.get("offset",offset);

	int clipbelow;
	bool usingclipbelow=options.get("clipbelow",clipbelow);

	int clipabove;
	bool usingclipabove=options.get("clipabove",clipabove);

	int mask;
	bool usingmask=options.get("mask",mask);

	bool usingpredictor=options.get("predictor");
	bool usingunpredictor=options.get("unpredictor");

	if (!usingoffset && !clipbelow && !clipabove && !mask && !usingpredictor && !usingunpredictor) {
		//cerr << EMsgDC(NeedOption) << " - offset, clipbelow, clipabove, mask or predictor" << endl;
		bad=true;
	}
	
	if (usingunpredictor) usingsigned=true;		// input is always a delta value

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
			<< " -signed"
			<< " -offset n"
			<< " -clipbelow n"
			<< " -clipabove n"
			<< " -mask n"
			<< " -predictor"
			<< " -unpredictor"
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
	int predictor=0;
	while (!in.eof()) {
		Uint16 uvalue;
		in >> uvalue;
		if (in.eof()) { success=true; break; }
		if (!in) { success=false; break; }

		int value;

		if (usingsigned)
			value=Int16(uvalue);	// should sign extend 16 bit word
		else
			value=Uint16(uvalue);	// should not sign extend

		if (usingoffset) value+=offset;
		if (usingclipbelow && value < clipbelow) value=clipbelow;
		if (usingclipabove && value > clipabove) value=clipabove;
		if (usingmask) value&=mask;
		if (usingpredictor) {
			int delta=value-predictor;
			predictor=value;
			value=delta;
		}
		if (usingunpredictor) {
			value=value+predictor;
			predictor=value;
		}
		out << Uint16(value);
		if (!out) { success=false; break; }
	}

	return success ? 0 : 1;
}

