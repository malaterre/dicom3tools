#include "bnstream.h"
#include "bnopt.h"
#include "mesgtext.h"
#include "txstream.h"

static bool
bmp_readAndDumpHeader(BinaryInputStream &in,TextOutputStream &log)
{
	// See amongst other references "http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html"
	
	char c1,c2;
	in.get(c1);
	if (!in) return false;
	in.get(c2);
	if (!in) return false;
	if (c1 != 'B' || c2 != 'M') {
		log << EMsgDC(NoBMPMagicNumber)
			<< endl;
		return false;
	}
	log << "magic       = " << c1 << c2 << endl;
	
	Uint32 bfSize = in.read32();
	if (!in) return false;
	log << "bfSize      = " << hex << bfSize << dec << "\t(" << bfSize << " dec)" << endl;
	
	Uint16 bfReserved1 = in.read16();
	if (!in) return false;
	log << "bfReserved1 = " << hex << bfReserved1 << dec << "\t(" << bfReserved1 << " dec)" << endl;
	
	Uint16 bfReserved2 = in.read16();
	if (!in) return false;
	log << "bfReserved2 = " << hex << bfReserved2 << dec << "\t(" << bfReserved2 << " dec)" << endl;
	
	Uint32 bfOffBits = in.read32();
	if (!in) return false;
	log << "bfOffBits   = " << hex << bfOffBits << dec << "\t(" << bfOffBits << " dec)" << endl;
	
	Uint32 biSize = in.read32();
	if (!in) return false;
	log << "biSize   = " << hex << biSize << dec << "\t(" << biSize << " dec)" << endl;
	
	Uint32 biWidth = in.read32();
	if (!in) return false;
	log << "biWidth   = " << hex << biWidth << dec << "\t(" << biWidth << " dec)" << endl;
	
	Uint32 biHeight = in.read32();
	if (!in) return false;
	log << "biHeight   = " << hex << biHeight << dec << "\t(" << biHeight << " dec)" << endl;

	Uint16 biPlanes = in.read16();
	if (!in) return false;
	log << "biPlanes = " << hex << biPlanes << dec << "\t(" << biPlanes << " dec)" << endl;
	
	Uint16 biBitCount = in.read16();
	if (!in) return false;
	log << "biBitCount = " << hex << biBitCount << dec << "\t(" << biBitCount << " dec)" << endl;
	
	Uint32 biCompression = in.read32();
	if (!in) return false;
	log << "biCompression   = " << hex << biCompression << dec << "\t(" << biCompression << " dec)" << endl;
	
	Uint32 biSizeImage = in.read32();
	if (!in) return false;
	log << "biSizeImage   = " << hex << biSizeImage << dec << "\t(" << biSizeImage << " dec)" << endl;
	
	Uint32 biXPelsPerMeter = in.read32();
	if (!in) return false;
	log << "biXPelsPerMeter   = " << hex << biXPelsPerMeter << dec << "\t(" << biXPelsPerMeter << " dec)" << endl;
	
	Uint32 biYPelsPerMeter = in.read32();
	if (!in) return false;
	log << "biYPelsPerMeter   = " << hex << biYPelsPerMeter << dec << "\t(" << biYPelsPerMeter << " dec)" << endl;
	
	Uint32 biClrUsed = in.read32();
	if (!in) return false;
	log << "biClrUsed   = " << hex << biClrUsed << dec << "\t(" << biClrUsed << " dec)" << endl;
	
	Uint32 biClrImportant = in.read32();
	if (!in) return false;
	log << "biClrImportant   = " << hex << biClrImportant << dec << "\t(" << biClrImportant << " dec)" << endl;
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions options(argc,argv);
	BinaryInputOptions input_options(options);

	input_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener(options,input_options.filename,cin);

	cerr << input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< " [" << MMsgDC(InputFile)
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream in(*(istream *)input_opener,LittleEndian);
	TextOutputStream log (cerr);
	bmp_readAndDumpHeader(in,log);
}

