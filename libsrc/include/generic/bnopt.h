#ifndef __Header_bnopt__
#define __Header_bnopt__

#include "getoptns.h"

class BinaryInputOptions : public ErrorsInClass
{
public:
	const char *	filename;

	BinaryInputOptions(GetNamedOptions &options);
	virtual char *usage(void);
	virtual void done(void);
};

class BinaryInputOptionsWithByteOrder : public BinaryInputOptions
{
public:
	Endian byteorder;

	BinaryInputOptionsWithByteOrder(GetNamedOptions &options);
	char *usage(void);
};

class BinaryOutputOptions : public ErrorsInClass
{
public:
	const char *	filename;

	BinaryOutputOptions(GetNamedOptions &options);
	char *usage(void);
	void done(void);
};

class BinaryOutputOptionsWithByteOrder : public BinaryOutputOptions
{
public:
	Endian byteorder;

	BinaryOutputOptionsWithByteOrder(GetNamedOptions &options);
	char *usage(void);
};

class BinaryInputOpenerFromOptions : public ErrorsInClass {
	istream *str;
public:
	BinaryInputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,istream &cstr);

	~BinaryInputOpenerFromOptions();

	operator istream *(void);
};

class BinaryOutputOpenerFromOptions : public ErrorsInClass {
	ostream *str;
public:
	BinaryOutputOpenerFromOptions(GetNamedOptions &options,
		const char *filename,ostream &cstr);

	~BinaryOutputOpenerFromOptions();

	operator ostream *(void);
};

#endif /* __Header_bnopt__ */

