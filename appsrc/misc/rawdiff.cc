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

	bool verbose=options.get("verbose") || options.get("v");

	unsigned cols=0;
	if (verbose &&
	    !options.get("columns",cols) && !options.get("width",cols) && !options.get("w",cols)) {
		cerr << EMsgDC(NeedOption) << " - columns" << endl;
		bad=true;
	}

	bool just1stcol=options.get("just1stcol");

	input_options.done();
	output_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener1(
		options,input_options.filename,cin);
	BinaryInputOpenerFromOptions input_opener2(
		options,0,cin);

	BinaryOutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	cerr << input_options.errors();
	cerr << output_options.errors();
	cerr << options.errors();
	cerr << input_opener1.errors();
	cerr << input_opener2.errors();
	cerr << output_opener.errors();

	if (!input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener1.good()
	 || !input_opener2.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< " -columns|-width|-w columns"
			<< " [-v|-verbose [-just1stcol]"
			<< input_options.usage()
			<< output_options.usage()
			<< "[" << MMsgDC(InputFile) << "]" << " " << MMsgDC(InputFile)
			<< "[" << MMsgDC(OutputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream in1(*(istream *)input_opener1,input_options.byteorder);
	BinaryInputStream in2(*(istream *)input_opener2,input_options.byteorder);
	BinaryOutputStream out(*(ostream *)output_opener,output_options.byteorder);

	if (verbose)
		cerr << "[row,col]: file2 (hex) - file1 (hex) = diff (hex)" << endl;

	long count=0;
	bool success;
	while (!in1.eof() && !in2.eof()) {
		Uint16 value1,value2;
		in1 >> value1;
		in2 >> value2;
		if (in1.eof() || in2.eof()) {
			if (in1.eof() && in2.eof())
				success=true;
			else {
				cerr << "Different length files" << endl;
				success=false;
			}
			break;
		}
		if (!in1 || !in2) { success=false; break; }

		Uint16 diff=value2-value1;
		out << diff;
		if (verbose && diff)
			if (!just1stcol || count%cols == 0)
				cerr << "[" << count/cols << "," << count%cols
				     << "]: 0x" << hex << value2 << " - 0x" << value1
				     << " = " << ((Int16(diff) < 0) ? "-" : "")
				     << "0x" << ((Int16(diff) < 0) ? -Int16(diff) : Int16(diff))
				     << dec << endl;
		if (!out) { success=false; break; }
		++count;
	}

	return success ? 0 : 1;
}

