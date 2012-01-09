#include <iomanip.h>
#include <ctype.h>
#include <string.h>

#include "basetype.h"
#include "getoptns.h"
#include "mesgtext.h"

// many of the get and extract methods should be a template
// but can't figure out how to have template functions declared within
// a class

bool
GetNamedOptions::match(const char *arg,const char *index)
{
	return (arg && arg[0] == '-' && index
		&& ((arg[1] == 0 && index[0] == 0)	// "-"
		     || strcmp(arg+1,index) == 0));
}

bool
GetNamedOptions::extract(const char *index,const char * src,long long &dst)
{
	while (*src == ' ') ++src;
	istrstream istr(src);
	istr >> resetiosflags(ios::basefield) // force adaptive
	     >> dst;
//	if (istr.good())
		return true;
//	else {
//		errorstream << "-" << index
//			    << ": " << EMsgDC(ArgumentNotInteger) << endl;
//		good_flag=false;
//		return false;
//	}
}

#define GENERATE_EXTRACT_INDEX_INTEGER(T)				\
bool									\
GetNamedOptions::extract(const char *index,const char * src,T &dst)	\
{									\
	long long l;								\
	if (extract(index,src,l)) {					\
		dst=(T)l;						\
		return true;						\
	}								\
	else								\
		return false;						\
}

GENERATE_EXTRACT_INDEX_INTEGER(unsigned long)
GENERATE_EXTRACT_INDEX_INTEGER(unsigned int)
GENERATE_EXTRACT_INDEX_INTEGER(int)
GENERATE_EXTRACT_INDEX_INTEGER(long)

bool
GetNamedOptions::extract(const char *index,const char * src,double &dst)
{
//cerr << "GetNamedOptions::extract(const char *index,const char * src,double &dst):" << endl;
	while (*src == ' ') ++src;
	istrstream istr(src);
	istr >> dst;
	if (istr.good()
	 // fudge because fails if no trailing f or d :(
		 || ((isdigit(*src) || *src == '-' || *src == '+') && isdigit(src[strlen(src)-1]))
	)
		return true;
	else {
		errorstream << "-" << index
			    << ": " << EMsgDC(ArgumentNotFloat) << endl;
		good_flag=false;
		return false;
	}
}

bool
GetNamedOptions::extract(const char *index,const char * src,float &dst)
{
//cerr << "GetNamedOptions::extract(const char *index,const char * src,float &dst):" << endl;
	double d;
	if (extract(index,src,d)) {
		dst=(float)d;
		return true;
	}
	else
		return false;
}

bool
GetNamedOptions::extract(const char *index,const char * src,const char * &dst)
{
	(void)good_flag; (void)errorstream;
	(void)index; dst=src; return true;
}

int
GetNamedOptions::findUnusedOption(void)
{
//cerr << "GetNamedOptions::findUnusedOption(void):" << endl;
	int i;
	for (i=0; i<argc_start; ++i) {
		if (!argv_used[i]
		 && argv_start[i]
		 && argv_start[i][0] == '-') {
//cerr << "GetNamedOptions::findUnusedOption(void): argv_start[i] = " << argv_start[i] << endl;
			return i;
		}
	}
	return -1;
}

int
GetNamedOptions::findUnusedOption(const char *index)
{
//cerr << "GetNamedOptions::findUnusedOption(const char *index):" << endl;
	if (!index) return -1;
	int i;
	for (i=0; i<argc_start; ++i) {
		if (!argv_used[i] && match(argv_start[i],index)) {
//cerr << "GetNamedOptions::findUnusedOption(const char *index): argv_start[i] = " << argv_start[i] << endl;
			return i;
		}
	}
	return -1;
}

int
GetNamedOptions::findUnusedOption(const char *const *indexes,int& which)
{
//cerr << "GetNamedOptions::findUnusedOption(const char *const *indexes,int& which):" << endl;
	which=-1;
	int found=-1;
	int iw=0;
	int count=0;
	while (*indexes) {
		int i=findUnusedOption(*indexes);
		if (i != -1) {
			found=i;
			which=iw;
			++count;
			if (count > 1) {
				errorstream << "-" << *indexes
					    << ": "
					    << EMsgDC(OptionRepeated)
					    << endl;
			}
		}
		++indexes;
		++iw;
	}
	return found;
}

int
GetNamedOptions::findUnusedValue(int n)
{
	int i;
	for (i=0; i<argc_start; ++i) {
		if (!argv_used[i]) {
			if (n == 0) return i;
			--n;
		}
	}
	return -1;
}

void
GetNamedOptions::firstValue(void)
{
}

int
GetNamedOptions::areMoreValues(void)
{
	int i=0;
	while (findUnusedValue(i) != -1) ++i;
	return i;
}

void
GetNamedOptions::bumpValue(void)
{
	int i=findUnusedValue(0);
	if (i != -1) argv_used[i]=true;
}

const char *
GetNamedOptions::value(int n)
{
	int i=findUnusedValue(n);
	return (i == -1) ? 0 : argv_start[i];
}

GetNamedOptions::GetNamedOptions(int argc,const char *const *argv)
	: ErrorsInClass()
{
	command_string=argv[0];
	argv_start=argv+1;
	argc_left=argc_start=argc-1;
	argv_used=argc_start ? new bool[argc_start] : 0;
	if (argc_start && !argv_used)
		good_flag=false;
	else {
		int i;
		for (i=0; i<argc_start; ++i) argv_used[i]=false;
	}
}

GetNamedOptions::~GetNamedOptions()
{
	if (argv_used) delete[] argv_used;
}

bool
GetNamedOptions::get(const char *index)
{
	int i=findUnusedOption(index);
	if (i != -1) {
		argv_used[i]=true;
		return true;
	}
	else
		return false;
}

bool
GetNamedOptions::operator[](const char *index)
{
	return get(index);
}

bool
GetNamedOptions::get(const char *const *indexes)
{
	int which;
	int i=findUnusedOption(indexes,which);
	if (i != -1) {
		argv_used[i]=true;
		return true;
	}
	else
		return false;
}

bool
GetNamedOptions::operator[](const char *const *indexes)
{
	return get(indexes);
}

bool
GetNamedOptions::get(const char *index,const char * &value)
{
	int i=findUnusedOption(index);
	if (i == -1) return false;
	argv_used[i]=true;
//cerr << "GetNamedOptions::get(const char *index,const char * &value): argv_start[i+1] = " << argv_start[i+1] << endl;
	if (i+1 >= argc_start
	 || argv_used[i+1]
	 || argv_start[i+1] == 0
	 || argv_start[i+1][0] == 0
//	 || argv_start[i+1][0] == '-'
	) {
		errorstream << "-" << index
			    << ": " << EMsgDC(ArgumentMissing) << endl;
		good_flag=false;
		return false;
	}
	argv_used[i+1]=true;
	value=argv_start[i+1];
//cerr << "GetNamedOptions::get(const char *index,const char * &value): value = " << value << endl;
	return true;
}

#define GENERATE_GET_ONE_ARGUMENT(T)					\
bool									\
GetNamedOptions::get(const char *index,T &value)			\
{									\
	const char *s;							\
	bool rv;							\
	if (!get(index,s))						\
		rv=false;						\
	else								\
		rv=extract(index,s,value);				\
	return rv;							\
}

GENERATE_GET_ONE_ARGUMENT(int)
GENERATE_GET_ONE_ARGUMENT(unsigned int)
GENERATE_GET_ONE_ARGUMENT(unsigned long)
GENERATE_GET_ONE_ARGUMENT(long)
GENERATE_GET_ONE_ARGUMENT(long long)
GENERATE_GET_ONE_ARGUMENT(float)
GENERATE_GET_ONE_ARGUMENT(double)

bool
GetNamedOptions::get(const char *const *indexes,const char * &value,int& which)
{
	int found=findUnusedOption(indexes,which);
	if (found == -1) return false;
	argv_used[found]=true;
//cerr << "GetNamedOptions::get(const char *const *indexes,const char * &value,int& which): argv_start[found+1] = " << argv_start[found+1] << endl;
	if (found+1 >= argc_start
	 || argv_used[found+1]
	 || argv_start[found+1] == 0
	 || argv_start[found+1][0] == 0
//	 || argv_start[found+1][0] == '-'
	) {
		errorstream << "-" << indexes[which]
			    << ": " << EMsgDC(ArgumentMissing) << endl;
		good_flag=false;
		return false;
	}
	argv_used[found+1]=true;
	value=argv_start[found+1];
//cerr << "GetNamedOptions::get(const char *const *indexes,const char * &value,int& which): value = " << value << endl;
	return true;
}

#define GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(T)		\
bool									\
GetNamedOptions::get(const char *const *indexes,T &value)		\
{									\
	const char *s;							\
	bool rv;							\
	int which;							\
	if (!get(indexes,s,which))					\
		rv=false;						\
	else								\
		rv=extract(indexes[which],s,value);			\
	return rv;							\
}

GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(int)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(unsigned int)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(unsigned long)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(long)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(long long)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(float)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(double)
GENERATE_GET_ONE_ARGUMENT_FROM_MULTIPLE_OPTIONS(const char *)

#define GENERATE_GET_MULTIPLE_ARGUMENTS(T)				\
int 									\
GetNamedOptions::get(const char *index,T *ptr,int num)			\
{									\
	int i=findUnusedOption(index);					\
	int n=-1;							\
	if (i != -1) {							\
		argv_used[i]=true;					\
		++i;							\
		for (n=0;  n < num && i+n < argc_start			\
				&& !argv_used[i+n]			\
				&& argv_start[i+n]			\
				&& argv_start[i+n][0]			\
				&& argv_start[i+n][0] != '-'		\
			  ; ++n) {					\
			argv_used[i+n]=true;				\
			(void)extract(					\
				index,argv_start[i+n],ptr[n]);		\
		}							\
	}								\
	return n;							\
}

GENERATE_GET_MULTIPLE_ARGUMENTS(unsigned int)
GENERATE_GET_MULTIPLE_ARGUMENTS(int)
GENERATE_GET_MULTIPLE_ARGUMENTS(unsigned long)
GENERATE_GET_MULTIPLE_ARGUMENTS(long)
GENERATE_GET_MULTIPLE_ARGUMENTS(long long)
GENERATE_GET_MULTIPLE_ARGUMENTS(float)
GENERATE_GET_MULTIPLE_ARGUMENTS(double)
GENERATE_GET_MULTIPLE_ARGUMENTS(const char *)

#define GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(T)		\
int 									\
GetNamedOptions::get(const char *const *indexes,T *ptr,int num)		\
{									\
	int which;							\
	int i=findUnusedOption(indexes,which);				\
	int n=-1;							\
	if (i != -1) {							\
		argv_used[i]=true;					\
		++i;							\
		for (n=0;  n < num && i+n < argc_start			\
				&& !argv_used[i+n]			\
				&& argv_start[i+n]			\
				&& argv_start[i+n][0]			\
				&& argv_start[i+n][0] != '-'		\
			  ; ++n) {					\
			argv_used[i+n]=true;				\
			(void)extract(					\
				indexes[which],				\
				argv_start[i+n],ptr[n]);		\
		}							\
	}								\
	return n;							\
}

GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(unsigned int)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(int)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(unsigned long)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(long)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(long long)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(float)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(double)
GENERATE_GET_MULTIPLE_ARGUMENT_FROM_MULTIPLE_OPTIONS(const char *)

int
GetNamedOptions::operator!()
{
	return areMoreValues();
}

void
GetNamedOptions::operator++()
{
	bumpValue();
}

void
GetNamedOptions::operator++(int)
{
	bumpValue();
}

const char *
GetNamedOptions::operator()()
{
	return value(0);
}

const char *
GetNamedOptions::operator[](int num)
{
	return value(num);
}

void
GetNamedOptions::done(void)
{
	int i;
	while ((i=findUnusedOption()) != -1) {
		good_flag=false;
		errorstream << argv_start[i] << ": unrecognized option" << endl;
		argv_used[i]=true;
	}
}

const char *
GetNamedOptions::command(void) const
{
	return command_string;
}

