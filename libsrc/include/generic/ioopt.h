#ifndef __Header_ioopt__
#define __Header_ioopt__

#include "getoptns.h"

class InputOptions : public ErrorsInClass
{
public:
	const char *	filename;

	InputOptions(GetNamedOptions &options);
	char *usage(void);
	void done(void);
};

class OutputOptions : public ErrorsInClass
{
public:
	const char *	filename;

	OutputOptions(GetNamedOptions &options);
	char *usage(void);
	void done(void);
};

class InputOpenerFromOptions : public ErrorsInClass {
	istream *str;
public:
	InputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,istream &cstr,bool raw=true);

	~InputOpenerFromOptions();

	operator streambuf *(void);
};

class OutputOpenerFromOptions : public ErrorsInClass {
	ostream *str;
public:
	OutputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,ostream &cstr,bool raw=true);

	~OutputOpenerFromOptions();

	operator streambuf *(void);
};

#endif /* __Header_ioopt__ */

