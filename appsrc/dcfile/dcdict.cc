#include "attrtag.h"
#include "elmdict.h"
#include "mesgtext.h"
#include "getoptns.h"

static const char *options_output_key[] = {
	"key",
	"k",
	0
};

static const char *options_output_tag[] = {
	"tag",
	"t",
	0
};

int
main(int argc,char **argv)
{
	GetNamedOptions 	options(argc,argv);

	bool bad=false;

	ElementDictionary optdict;
	TextOutputStream  log(cerr);

	const char *keyarg;
	while (options.get(options_output_key,keyarg)) {
		Assert(keyarg);
		Tag tag;
		if (optdict.getTag(keyarg,tag)) {
			tag.write(log,&optdict);
			log << endl;
		}
		else {
			log << "-" << options_output_key[0] << ": "
			    << EMsgDC(UnrecognizedElement)
			    << " - \"" << keyarg << "\""
			    << endl;
			bad=true;
		}
	}

	unsigned tagargs[2];
	int n;
	while ((n=options.get(options_output_tag,tagargs,2)) != -1) {
		if (n == 2) {
			Tag tag=Tag(Uint16(tagargs[0]),Uint16(tagargs[1]));
			const char *keyword=optdict.getKeyword(tag);
			if (keyword) {
				tag.write(log,&optdict);
				log << endl;
			}
			else {
				log << "-" << options_output_tag[0] << ": "
				    << EMsgDC(UnrecognizedElement)
				    << " - ";
				tag.write(log);
				log << endl;
				bad=true;
			}
		}
		else {
			log << "-" << options_output_tag[0] << ": "
			    << EMsgDC(WrongNumberOfArguments)
			    << endl;
			bad=true;
		}
	}

	options.done();

	cerr << options.errors();

	if (!options.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< " [-key|k attributename]"
			<< " [-tag|t group element]"
			<< endl;
		return 1;
	}

	return 0;
}

