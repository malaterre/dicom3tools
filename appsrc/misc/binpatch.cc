#include <iostream.h>          // for listsimp.h
#include <iomanip.h>           // for listsimp.h
#include <fstream.h>
#include <stdlib.h>            // for atoi()
#include <string.h>            // for strlen()
#include <ctype.h>             // for isdigit()

#include "basetype.h"
#include "listsort.h"
#include "ioopt.h"
#include "mesgtext.h"

class Patch {
	unsigned	offset;
	unsigned	length;
	const char *	string;
	const char *	Padstring;
	char *		binarystring;
	unsigned	binarystringlength;
	char *		binarypadstring;
	unsigned long	binaryvalue;

	bool		isstring;

	char getEscape(const char * & str)
		{
			unsigned char rval;
			++str;	// skip "\"
			switch (*str) {
				case 'r':
					rval='\r'; ++str; break;
				case 'n':
					rval='\n'; ++str; break;
				case 't':
					rval='\t'; ++str; break;
				case 'f':
					rval='\f'; ++str; break;
				case 'b':
					rval='\b'; ++str; break;
				default:	
					if (isdigit(*str)) {
						char *term;
						rval=(unsigned char)
							strtol(str,&term,0);
						str=term;
					}
					else
						rval=*str++;
					break;
			}
			return (char)rval;
		}
public:
	Patch(unsigned o,unsigned l,const char *s,const char *p)
		{
			isstring=true;
			offset=o;
			length=l;
			string=s;
			Padstring=p;
			binarystring=new char[strlen(string)+1];
			Assert(binarystring);
			const char *from=string;
			char *to=binarystring;
			while (*from) {
				if (*from == '\\') *to=getEscape(from);
				else *to=*from++;
				++to;
			}
			*to='\0';
			binarystringlength=strlen(binarystring);

			binarypadstring=new char[strlen(Padstring)+1];
			Assert(binarypadstring);
			from=Padstring;
			to=binarypadstring;
			while (*from) {
				if (*from == '\\') *to=getEscape(from);
				else *to=*from++;
				++to;
			}
			*to='\0';
		}

	Patch(unsigned o,unsigned l,unsigned long v,char endian)	// unsigned variety
		{
			isstring=false;
			offset=o;
			length=l;
			binaryvalue=v;

			string=0;
			Padstring=0;

			Assert(length <= sizeof(binaryvalue));
			Assert(endian == 'l' || endian == 'b');

			binarystring=new char[length];
			Assert(binarystring);
			int i;
			char *ptr;
			for (i=0,ptr=binarystring; i < length; ++i) {
				if (endian == 'l')
					*ptr++=(binaryvalue>>(8*i))&0xff;		// little endian
				else
					*ptr++=(binaryvalue>>(8*(length-1-i)))&0xff;	// big endian
			}
			binarystringlength=length;
		}

	void		put(ostream& stream)
		{
			if (isstring) {
				stream << dec << "@" << offset
					      << "\t[" << length
					      << "]\t<" << string
					      << ">\t<" << Padstring
					      << ">";
			}
			else {
				stream << dec << "@" << offset
					      << "\t[" << length
					      << "]\t<" << binaryvalue
					      << ">";
			}
		}

	unsigned	getOffset(void)		{ return offset; }
	unsigned	getLength(void)		{ return length; }
	unsigned	getStringLength(void)	{ return binarystringlength; }
	const char *	getString(void)		{ return binarystring; }
	const char *	getPadString(void)	{ return binarypadstring; }
};
	
// In order for the sorted list template to know how to sort one needs
// to define a type that can order itself, hence this ...

class PatchPtr {
	Patch *p;
public:
	PatchPtr(void)				{ p=0; }
	PatchPtr(Patch *ptr)			{ p=ptr; }

	PatchPtr(const PatchPtr& t)		{ p=t.p; }
	void operator=(const PatchPtr& t)	{ p=t.p; }
	operator Patch *(void)			{ return p; }

	Patch * operator->(void)		{ return p; }

	int operator==(const PatchPtr& p2)	{ return p->getOffset() == p2.p->getOffset(); }
	int operator<(const PatchPtr& p2)	{ return p->getOffset() <  p2.p->getOffset(); }
	int operator<=(const PatchPtr& p2)	{ return p->getOffset() <= p2.p->getOffset(); }
	int operator>(const PatchPtr& p2)	{ return p->getOffset() >  p2.p->getOffset(); }
	int operator>=(const PatchPtr& p2)	{ return p->getOffset() >= p2.p->getOffset(); }
};	

static bool
checkPatchList(SortedList<PatchPtr>& patchlist)
{
	SortedListIterator<PatchPtr> pi(patchlist);
	bool bad=false;
	PatchPtr last = 0;
	while (!pi) {
		bool bothbad=false;
		PatchPtr current=pi();
		if (last && last > current) {
			cerr << EMsgDC(PatchesNotInOrder) << endl;
			bothbad=true;
		}
		if (last && last == current) {
			cerr << EMsgDC(PatchesSameOffset) << endl;
			bothbad=true;
		}
		if (last
		 &&  last->getOffset()
		   + last->getLength()
		   > current->getOffset()
		) {
			cerr << EMsgDC(PatchesOverlap) << endl;
			bothbad=true;
		}
		if (bothbad) {
			last->put(cerr);
			cerr << " cf. ";
			current->put(cerr);
			cerr << endl;
			bad=true;
		}
		bool currentbad=false;
		if (current->getLength() < current->getStringLength()) {
			cerr << EMsgDC(PatchStringTooLong) << endl;
			currentbad=true;
		}
		if (currentbad) {
			current->put(cerr);
			cerr << endl;
			bad=true;
		}
		last=current;
		++pi;
	}
	return bad;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions options(argc,argv);
	InputOptions    input_options(options);
	OutputOptions   output_options(options);

	bool verbose=options.get("verbose") || options.get("v");

	bool bad=false;
	SortedList<PatchPtr> patchlist;
	const char *args[4];
	int n;

	while ((n=options.get("r",args,4)) != -1) {
		if (n == 4) {
			unsigned offset=(unsigned)strtol(args[0],0,0);
			unsigned length=(unsigned)strtol(args[1],0,0);
			const char *string=args[2];
			const char *pad=args[3];
			patchlist+=new Patch(offset,length,string,pad);
		}
		else {
			cerr << "-r: "
			     << EMsgDC(WrongNumberOfArguments)
			     << endl;
			bad=true;
		}
	}

	while ((n=options.get("i",args,4)) != -1) {
		if (n == 4) {
			unsigned offset=(unsigned)strtol(args[0],0,0);
			unsigned length=(unsigned)strtol(args[1],0,0);
			unsigned long value=strtoul(args[2],0,0);
			char endian=*(args[3]);
			patchlist+=new Patch(offset,length,value,endian);
		}
		else {
			cerr << "-i: "
			     << EMsgDC(WrongNumberOfArguments)
			     << endl;
			bad=true;
		}
	}

	input_options.done();
	output_options.done();
	options.done();

	InputOpenerFromOptions  input_opener(options,input_options.filename,cin,true);
	OutputOpenerFromOptions output_opener(options,output_options.filename,cout,true);

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
			<< " [-v|-verbose]"
			<< " [-r offset length string padvalue]"
			<< " [-i offset length integervalue l|b]"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (verbose) {
		SortedListIterator<PatchPtr> pi(patchlist);
		while (!pi) {
			pi()->put(cerr);
			cerr << endl;
			++pi;
		}
	}

	if (checkPatchList(patchlist)) {
		cerr << EMsgDC(BadPatches) << endl;
		exit(1);
	}

	istream in(input_opener);
	ostream out(output_opener);

	SortedListIterator<PatchPtr> pi(patchlist);
	Patch *patch=!pi ? (Patch *)pi() : 0;
	unsigned offset=0;
	while (in.good() && in.peek() != EOF) {
		char c;
		in.read(&c,1);
		if (!in.good()) {
			cerr << EMsgDC(ReadFailed) << endl;
			exit(1);
		}
		if (patch
		 && offset >= patch->getOffset()
		 && offset <  patch->getOffset()+patch->getLength()) {
			unsigned index=offset-patch->getOffset();
			if (index < patch->getStringLength()) {
				out.write(patch->getString()+index,1);
			}
			else {
				out.write(patch->getPadString(),1);
			}
			if (++offset >= patch->getOffset()+patch->getLength()) {
				++pi;
				patch=!pi ? (Patch *)pi() : 0;
			}
		}
		else {
			out.write(&c,1);
			++offset;
		}
	}
	if (!out.good()) {
		cerr << EMsgDC(WriteFailed) << endl;
		exit(1);
	}
	return 0;
}

