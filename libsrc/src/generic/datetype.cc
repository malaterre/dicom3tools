#include <ctype.h>
//#include <iostream.h>	// For debugging only

#include "basetype.h"
#include "datetype.h"

static bool
isAllNumeric(const char *src,size_t length)
{
	while (length>0 && isdigit(*src))
		{ --length; ++src; }
	return (length == 0);
}

static bool
isDateDelimeter(char c) { return c == '-' || c == '/' || c == '.'; }

static bool
isTimeDelimeter(char c) { return c == ':' || c == '.'; }

static void
skipDateDelimeter(const char *&src,size_t &length)
{
	if (length>0 && isDateDelimeter(*src))
		{ --length; ++src; }
}

static void
skipTimeDelimeter(const char *&src,size_t &length)
{
	if (length>0 && isTimeDelimeter(*src))
		{ --length; ++src; }
}

static void
skipSpaces(const char *&src,size_t &length)
{
	while (length>0 && isspace(*src))
		{ --length; ++src; }
}

static bool
extractDecimalLong(const char *&src,size_t &length,long &n)
{
	n=0;
	if (length>0 && isdigit(*src)) {
		do {
			--length; n=n*10+(*src++-'0');
		} while (length>0 && isdigit(*src));
		return true;
	}
	else return false;
}

static bool
extractDecimalShort(const char *&src,size_t &length,short &n)
{
	n=0;
	if (length>0 && isdigit(*src)) {
		do {
			--length; n=n*10+(*src++-'0');
		} while (length>0 && isdigit(*src));
		return true;
	}
	else return false;
}

static bool
extractDecimalShortFixedLength(const char *&src,size_t &length,size_t count,short &n)
{
	n=0;
	if (count>0 && length>0 && isdigit(*src)) {
		do {
			--count; --length; n=n*10+(*src++-'0');
		} while (count>0 && length>0 && isdigit(*src));
		return count == 0;
	}
	else return false;
}


void
Date::makeTimezoneOffset(long tm_gmtoff) {
	int gmtoff=tm_gmtoff/60;	// convert from secs to minutes;
	if (gmtoff < 0) {
		tzo[0]='-';
		gmtoff=-gmtoff;
	}
	else {
		tzo[0]='-';
	}
	int gmtoff_mins=gmtoff%60;
	int gmtoff_hrs=gmtoff/60;
	static char decimal_digits[] = { '0','1','2','3','4','5','6','7','8','9' };
	Assert(gmtoff_hrs <= 24);
	tzo[1]=decimal_digits[gmtoff_hrs/10];
	tzo[2]=decimal_digits[gmtoff_hrs%10];
	Assert(gmtoff_mins <= 60);
	tzo[3]=decimal_digits[gmtoff_mins/10];
	tzo[4]=decimal_digits[gmtoff_mins%10];
	tzo[5]=0;
}

char *
Date::makeMMMFromMM(short m) const
{
	char *str=new char[4];
	switch (m) {
		case 1:	 strncpy(str,"Jan",3); break;
		case 2:	 strncpy(str,"Feb",3); break;
		case 3:	 strncpy(str,"Mar",3); break;
		case 4:	 strncpy(str,"Apr",3); break;
		case 5:	 strncpy(str,"May",3); break;
		case 6:	 strncpy(str,"Jun",3); break;
		case 7:	 strncpy(str,"Jul",3); break;
		case 8:	 strncpy(str,"Aug",3); break;
		case 9:	 strncpy(str,"Sep",3); break;
		case 10: strncpy(str,"Oct",3); break;
		case 11: strncpy(str,"Nov",3); break;
		case 12: strncpy(str,"Dec",3); break;
		default: strncpy(str,"bad",3); break;
	}
	str[3]=0;
	return str;
}

bool
Date::extractMMFromMMM(const char *&src,size_t &length,short &m) const
{
	bool ok=true;
	m=0;
	char mmm0=toupper(src[0]);
	char mmm1=tolower(src[1]);
	char mmm2=tolower(src[2]);
	switch (mmm0) {
		case 'A':
			switch (mmm1) {
				case 'p':	m=4; break;
				case 'u':	m=8; break;
			}
			break;
		case 'D':
			m=12; break;
		case 'F':
			m=2; break;
		case 'J':
			switch (mmm1) {
				case 'a':	m=1; break;
				case 'u':
					switch (mmm2) {
						case 'l':
							m=7;
							break;
						case 'n':
							m=6;
							break;
					}
					break;
			}
			break;
		case 'M':
			switch (mmm2) {
				case 'r':	m=3; break;
				case 'y':	m=5; break;
			}
			break;
		case 'N':
			m=11; break;
		case 'O':
			m=10; break;
		case 'S':
			m=9; break;
		default:
			m=0; ok=false; break;
	}
	length-=3;
	src+=3;
	return ok;
}

bool
Date::getDatePossibilityFromString(
	const char *&src,size_t &length,
	short &value,int &value_ismmm) const
{
	if (length) {
		if (isdigit(*src)) {
			value_ismmm=0;
			return extractDecimalShort(src,length,value);
		}
		else {
			value_ismmm=1;
			return extractMMFromMMM(src,length,value);
		}
	}
	else return false;
}

bool
Date::getDatePossibilitiesFromString(
	const char *src,size_t length,
	short &v1,int &v1_ismmm,
	short &v2,int &v2_ismmm,
	short &v3,int &v3_ismmm) const
{
//cerr << "getDatePossibilitiesFromString: string=" << (src ? src : "-zero length-") << endl;

	skipSpaces(src,length);
	if (!getDatePossibilityFromString(src,length,v1,v1_ismmm)) return false;
//cerr << "getDatePossibilitiesFromString: v1=" << v1 << endl;
	skipSpaces(src,length);
	skipDateDelimeter(src,length);
	skipSpaces(src,length);
	if (!getDatePossibilityFromString(src,length,v2,v2_ismmm)) return false;
//cerr << "getDatePossibilitiesFromString: v2=" << v2 << endl;
	skipSpaces(src,length);
	skipDateDelimeter(src,length);
	skipSpaces(src,length);
	if (!getDatePossibilityFromString(src,length,v3,v3_ismmm)) return false;
//cerr << "getDatePossibilitiesFromString: v3=" << v3 << endl;
	skipSpaces(src,length);

//cerr << "getDatePossibilitiesFromString: v1=" << v1 << " v2=" << v2 << " v3=" << v3 << endl;

	return	length == 0
		&& v1 && v2 && v3
		&& (v1_ismmm+v2_ismmm+v3_ismmm) <= 1;
}

bool
Date::validateDateValues(short d,short m,short &y)
{
//cerr << "validateDateValues: y=" << y << " m=" << m << " d=" << d << endl;
	// Y2K OK but this will fail before 1910 or after 2010 ...
	if (y && y < 100) y+=(y < 10 ? 2000 : 1900);
	return y && m && m <= 12 && d && d <= 31;
}

void
Date::assignIndex(int& assignto,int not1,int not2) const
{
	int i=0;
	while(i<3) {
		if (i != not1 && i != not2) {
			assignto=i; break;
		}
		++i;
	}
}

bool
Date::getDDMMYYFromString(const char *src,size_t length,DateOrder order)
{
	bool success=true;

	if (isAllNumeric(src,length)) {
		// if all numeric depend totally on requested order ...

//cerr << "Date::getDDMMYYFromString isAllNumeric" << endl;
		if (length == 8) {
//cerr << "Date::getDDMMYYFromString length == 8" << endl;
			if (order == DateOrderMonthMiddleYearFirst) {		// yyyymmdd
//cerr << "Date::getDDMMYYFromString yyyymmdd" << endl;
				success = extractDecimalShortFixedLength(src,length,4,yyyy)
				       && extractDecimalShortFixedLength(src,length,2,mm)
				       && extractDecimalShortFixedLength(src,length,2,dd);
			}
			else {
				success=false;
			}
		}
		else if (length == 6) {
			if (order == DateOrderMonthMiddleYearFirst) {		// yymmdd
				success = extractDecimalShortFixedLength(src,length,2,yyyy)
				       && extractDecimalShortFixedLength(src,length,2,mm)
				       && extractDecimalShortFixedLength(src,length,2,dd);
			}
			else {
				success=false;
			}
		}
		else {
			success=false;
		}
	}
	else {
		// delimiters or spaces or non-numeric month ... get really creative ...

		short v[3];
		int v_ismmm[3];
		if (!getDatePossibilitiesFromString(
			src,length,
			v[0],v_ismmm[0],
			v[1],v_ismmm[1],
			v[2],v_ismmm[2])) return false;

		int dayindex	=-1;
		int monthindex	=-1;
		int yearindex	=-1;

		unsigned i;

		// Determine actual string order if possible ...

		i=0;
		while (i<3) {
			if (v_ismmm[i]) {
				monthindex=i;
				break;
			}
			++i;
		}
		i=0;
		while(i<3) {
			if (i != monthindex && v[i] > 31) {
				yearindex=i;
				break;
			}
			++i;
		}
		i=0;
		while(i<3) {
			if (yearindex != -1 && i != yearindex && i != monthindex && v[i] > 12) {
				dayindex=i;
				break;
			}
			++i;
		}

		// Actual string order may override requested order ...

		if (order == DateOrderMonthMiddleYearLast) {		// ddmmyy
			if (yearindex != 0 && dayindex < 0)
				assignIndex(dayindex,yearindex,monthindex);
			if (monthindex < 0)
				assignIndex(monthindex,dayindex,yearindex);
			if (yearindex < 0)
				assignIndex(yearindex,dayindex,monthindex);
			if (dayindex < 0)	// just in case yymmdd
				assignIndex(dayindex,yearindex,monthindex);
		}
		else if (order == DateOrderDayMiddleYearLast) {		// mmddyy
			if (yearindex != 0 && monthindex < 0)
				assignIndex(monthindex,yearindex,dayindex);
			if (dayindex < 0)
				assignIndex(dayindex,monthindex,yearindex);
			if (yearindex < 0)
				assignIndex(yearindex,dayindex,monthindex);
			if (monthindex < 0)	// just in case yyddmm
				assignIndex(monthindex,yearindex,dayindex);
		}
//		else if (order == DateOrderDayMiddleYearFirst) {	// yyddmm
//			if (monthindex != 0 && yearindex < 0)
//				assignIndex(yearindex,monthindex,dayindex);
//			if (dayindex < 0)
//				assignIndex(dayindex,monthindex,yearindex);
//			if (monthindex < 0)
//				assignIndex(monthindex,dayindex,yearindex);
//			if (yearindex < 0)	// just in case mmddyy
//				assignIndex(yearindex,monthindex,dayindex);
//		}
		else if (order == DateOrderMonthMiddleYearFirst) {	// yymmdd
			if (monthindex != 0 && yearindex < 0)
				assignIndex(yearindex,monthindex,dayindex);
			if (monthindex < 0)
			assignIndex(monthindex,dayindex,yearindex);
			if (dayindex < 0)
				assignIndex(dayindex,monthindex,yearindex);
			if (yearindex < 0)	// just in case ddmmyy
				assignIndex(yearindex,monthindex,dayindex);
		}

		// all will be assigned by now

		dd=v[dayindex];
		mm=v[monthindex];
		yyyy=v[yearindex];
	}

	return success && validateDateValues(dd,mm,yyyy);
}

Date::Date(short y,short m,short d)
{
	dd=d;
	yyyy=y;
	mm=m;
	mmm=makeMMMFromMM(m);
	goodflag=validateDateValues(dd,mm,yyyy);	// Adds century prn
	tzo[0]=0;
}

Date::Date(short y,const char *mmm,short d)
{
	dd=d;
	yyyy=y;
	size_t length=strlen(mmm);
	goodflag=extractMMFromMMM(mmm,length,mm) && validateDateValues(dd,mm,yyyy);	// Adds century prn
	tzo[0]=0;
}

Date::Date(const char *str,enum DateOrder order)
{
	size_t length=strlen(str);
	goodflag=getDDMMYYFromString(str,length,order);	// Already does validateDateValues()
	mmm=makeMMMFromMM(mm);
	tzo[0]=0;
}

Date::Date(Date const & date)
{
	dd=date.getDD();
	yyyy=date.getYYYY();
	mm=date.getMM();
	mmm=new char[4];
	strncpy(mmm,date.getMMM(),3);
	mmm[3]=0;
	goodflag=validateDateValues(dd,mm,yyyy);
	strncpy(tzo,date.tzo,5);
}

Date::Date(void)
{
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	Assert(lt);
	dd=lt->tm_mday;
	yyyy=lt->tm_year+1900;
	mm=lt->tm_mon+1;
	mmm=new char[4];
	strncpy(mmm,getMMM(),3);
	mmm[3]=0;
	goodflag=validateDateValues(dd,mm,yyyy);
#ifdef USEGLIBCTIMEZONE
#if USEGLIBCTIMEZONE == 1
        makeTimezoneOffset(lt->tm_gmtoff);	// a glibc'ism that works on Linux and Mac but not Solaris
#else
#if USEGLIBCTIMEZONE == -1
	makeTimezoneOffset(lt->tm_isdst == 0 ? timezone : altzone);	// external variables for Solaris (at least prior to Solaris 10)
#else
#if USEGLIBCTIMEZONE == -2
	makeTimezoneOffset(_timezone);	// Cygwin
#else
	makeTimezoneOffset(timezone);		// an external variable set whenever localtime() is called
#endif
#endif
#endif
#else
	makeTimezoneOffset(timezone);		// an external variable set whenever localtime() is called
#endif
}

Date&
Date::operator=(const Date & date)
{
	dd=date.getDD();
	yyyy=date.getYYYY();
	mm=date.getMM();
	mmm=new char[4];
	strncpy(mmm,date.getMMM(),3);
	mmm[3]=0;
	goodflag=validateDateValues(dd,mm,yyyy);
	return *this;
}

Date::~Date(void)
{
	if (mmm) delete[] mmm;
}


bool
Time::validateTimeValues(short h,short m,short s,short ms) const
{
	return h >= 0 && h<= 23 && m >= 0 && m <= 60 && s >= 0 && s <= 60;
}

bool
Time::getHHMMSSFromString(const char *src,size_t length)
{
//cerr << "Time::getHHMMSSFromString: <" << src << ">" << endl;
	hour=0;
	min=0;
	sec=0;
	millisec=0;
	skipSpaces(src,length);
	if (!extractDecimalShortFixedLength(src,length,2,hour)) return false;	// must have at least the hour
//cerr << "Time::getHHMMSSFromString: extracted hour " << hour << endl;
	skipSpaces(src,length);
	skipTimeDelimeter(src,length);
	skipSpaces(src,length);
	if (!extractDecimalShortFixedLength(src,length,2,min)) return length == 0 && validateTimeValues(hour,min,sec,millisec);
//cerr << "Time::getHHMMSSFromString: extracted min " << min << endl;
	skipSpaces(src,length);
	skipTimeDelimeter(src,length);
	skipSpaces(src,length);
	if (!extractDecimalShortFixedLength(src,length,2,sec)) return length == 0 && validateTimeValues(hour,min,sec,millisec);
//cerr << "Time::getHHMMSSFromString: extracted sec " << sec << endl;
	if (*src == '.') { --length; ++src; }	// skip fraction of second delimiter
	if (length && *src) {
		size_t fraclength=length;
		long lmillisec;			// because possibility of truncation if precise fraction
		if (!extractDecimalLong(src,length,lmillisec)) return length == 0 && validateTimeValues(hour,min,sec,millisec);
		fraclength-=length;
		if (fraclength > 3) {
			fraclength-=3;
			while (fraclength-- && lmillisec) lmillisec/=10;
		}
		else if (fraclength < 3) {
			while (fraclength++ < 3 && lmillisec) lmillisec*=10;
		}
		Assert(lmillisec < 32768);
		millisec=(short)lmillisec;
//cerr << "Time::getHHMMSSFromString: extracted millisec " << millisec << endl;
	}
	return length == 0 && validateTimeValues(hour,min,sec,millisec);
}

Time::Time(short h,short m,short s,short ms)
{
	hour=h;
	min=m;
	sec=s;
	millisec=ms;
	goodflag=validateTimeValues(hour,min,sec,millisec);
}

Time::Time(const char *str)
{
	size_t length=strlen(str);
	goodflag=getHHMMSSFromString(str,length); // already does validateTimeValues()
}

Time::Time(Time const & time)
{
	hour=time.getHour();
	min=time.getMinute();
	sec=time.getSecond();
	millisec=time.getMilliSecond();
	goodflag=validateTimeValues(hour,min,sec,millisec);
}

Time::Time(Uint32 totalms)
{
	millisec=Uint16(totalms%1000u);
	totalms/=1000u;
	sec=Uint16(totalms%60u);
	totalms/=60u;
	min=Uint16(totalms%60u);
	totalms/=60u;
	hour=Uint16(totalms%60u);
	goodflag=validateTimeValues(hour,min,sec,millisec);
}

Time::Time(void)
{
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	Assert(lt);
	millisec=Uint16(0);
	sec=Uint16(lt->tm_sec);
	min=Uint16(lt->tm_min);
	hour=Uint16(lt->tm_hour);
	goodflag=validateTimeValues(hour,min,sec,millisec);
}

