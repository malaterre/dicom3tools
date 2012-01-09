#ifndef __Header_strtype__
#define __Header_strtype__

//#include <string.h>	// always include basetype.h which includes this

template <int Length>
class String {
	char string[Length];
public:
	String(void)
		{
		}
	String(const char *s)
		{
			strncpy(string,s,Length);
		}
	operator const char *(void) const
		{
			// not delimited unless originally delimited
			return string;
		}
	operator char *(void) const
		{
			// makes a delimited copy that caller must delete[]
			char *s = new char[Length+1];
			Assert(s);
			strncpy(s,string,Length);
			s[Length]=0;
			return s;
		}
};

// used as:
//
//	String<10> s;
//
//	ostr << String_Use(s);

class String_Use {
	char *string;
public:
	String_Use(char *s)
		{
			string=s;
		}
	~String_Use()
		{
			if (string) delete[] string;
		}
	operator const char *(void) const
		{
			return string;
		}
};

class String_Cat_Use {
	char *string;
public:
	String_Cat_Use(const char *s1,const char *s2)
		{
			size_t length=0;
			if (s1) length+=strlen(s1);
			if (s2) length+=strlen(s2);
			string=new char[length+1];
			string[0]=0;
			if (s1) strcat(string,s1);
			if (s2) strcat(string,s2);
			string[length]=0;
		}
	~String_Cat_Use()
		{
			if (string) delete[] string;
		}
	operator const char *(void) const
		{
			return string;
		}
};

static inline
char *StrDup(const char *s)
{
	char *d;
	if (s) {
		d=new char[strlen(s)+1];
		Assert(d);
		strcpy(d,s);
	}
	else
		d=0;
	return d;
}

#ifdef CRAP
class StringCopy {
private:
	char *string;
public:
	StringCopy(void)
		{
			string=0;
		}
	StringCopy(const char *from)
		{
			string=StrDup(from);
		}
	StringCopy(const StringCopy& from)
		{
			string=StrDup(from);
		}

	~StringCopy(void)
		{
			if (string) delete[] string;
		}

	StringCopy& operator=(const StringCopy& from)
		{
			string=StrDup(from.string);
			return *this;
		}

	operator const char *() const
		{
			return string;
		}

	operator char *(void) const
		{
			return StrDup(string);
		}
};
#endif

#endif /* __Header_strtype__ */
