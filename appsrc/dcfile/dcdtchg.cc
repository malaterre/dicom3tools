#include <fstream.h>
//#include <time.h>

#include "attrmxls.h"
#include "attrseq.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"
#include "attrtype.h"

#include "hash.h"
#include "datetype.h"

#include "dcdtchg.h"

class EntryString {
	HashKeyString key;
	const char *value;
public:
	EntryString(void)		{}
	EntryString(const char *s,const char *v)
					{ key=HashKeyString(s); value=v; }
	EntryString(EntryString *e)
					{ key=e->key; value=e->value; }

	HashKeyString	getKey(void) const	{ return key; }
	const char *	getValue(void) const	{ return value; }

	bool operator==(EntryString e)
					{ return key == e.getKey(); }
};

class EntryStringList : public SimpleList<EntryString>
{
public:
	~EntryStringList() {}	// only because buggy g++ 2.7.0 freaks
};

class EntryStringListIterator : public SimpleListIterator<EntryString>
{
public:
	EntryStringListIterator(void)
		: SimpleListIterator<EntryString>() {}
	EntryStringListIterator(EntryStringList& list)
		: SimpleListIterator<EntryString>(list) {}
};

class HashMap : public OpenHashTable <EntryString,
					HashKeyString,
					EntryStringList,
					EntryStringListIterator>
{
public:
	HashMap(unsigned long size)
		: OpenHashTable<EntryString,
				HashKeyString,
				EntryStringList,
				EntryStringListIterator>(size)
		{}

	// supply virtual functions for OpenHashTable ...

	unsigned long	hash(const HashKeyString &key,unsigned long size)
		{
			const unsigned nchars = 10;
			const unsigned shift  = 4;

			unsigned n=nchars;
			unsigned long value=0;
			const char *s=key.getString();
			while (n-- && *s) {
				value=(value<<shift)|*s++;
			}
			return value%size; 
		}

	HashKeyString key(const EntryString &e)	{ return e.getKey(); }
};

static bool
isAttributeToBeReplaced(Attribute *a) {
	//	Tag tag = a->getTag();
	//	return tag != TagFromName(Date);
	return true;
}

HashMap *timeHashMap;
HashMap *dateTimeHashMap;
long long dateDelta;
bool dateDeltaSet;
long long baseDateOffsetFromEpoch;
bool forceFirstDay;
bool forceFirstMonth;
bool keepZone;
bool addDeidentificationMethod;
static unsigned shiftToNoonInSeconds = 12*3600u;
static unsigned twentyFourHoursInSeconds = 24*3600u;

static bool
loopOverListsInSequences(Attribute *a,TextOutputStream &log,
		bool (*func)(AttributeList &,TextOutputStream &))
{
	bool succeeded=true;
	if (strcmp(a->getVR(),"SQ") == 0) {
		AttributeList **al;
		int n;
		if ((n=a->getLists(&al)) > 0) {
			int i;
			for (i=0; i<n; ++i) {
				if (!(*func)(*al[i],log)) succeeded=false;
			}
			delete [] al;
		}
	}
	return succeeded;
}

static unsigned long daysInNonLeapYear = 365;
static unsigned long daysInLeapYear = 366;

static unsigned long long milliSecondsInSecond = 1000;

static unsigned long long milliSecondsInMinute = 60ul * milliSecondsInSecond;
static unsigned long long milliSecondsInHour = 60*milliSecondsInMinute;
static unsigned long long milliSecondsInDay = 24*milliSecondsInHour;

static unsigned long long milliSecondsInNonLeapYear = (unsigned long long)(daysInNonLeapYear) * milliSecondsInDay;
static unsigned long long milliSecondsInLeapYear = (unsigned long long)(daysInLeapYear) * milliSecondsInDay;

static unsigned long daysPerMonthInNonLeapYear[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static unsigned long daysPerMonthInLeapYear[] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static unsigned long cummulativeDaysInPreviousMonthInNonLeapYear[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned long cummulativeDaysInPreviousMonthInLeapYear[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static unsigned long
getDaysPerMonth(int mm,bool leapYear) {
	long day = 0;
	if (mm <= 12 && mm > 0) {
		day = (leapYear ?  daysPerMonthInLeapYear : daysPerMonthInNonLeapYear)[mm];
	}
	return day;
}

static unsigned long long
getMilliSecondsPerMonth(int mm,bool leapYear) {
	return getDaysPerMonth(mm,leapYear) * milliSecondsInDay;
}

static void
populateCummulativeDaysPerMonth() {
	unsigned long nonLeapTotal=0;
	unsigned long leapTotal=0;
	for (int i=1; i<=12; ++i) {
		nonLeapTotal += daysPerMonthInNonLeapYear[i-1];
		cummulativeDaysInPreviousMonthInNonLeapYear[i] = nonLeapTotal;
		leapTotal += daysPerMonthInLeapYear[i-1];
		cummulativeDaysInPreviousMonthInLeapYear[i] = leapTotal;
	}
//cerr << "cummulativeDaysInPreviousMonthInNonLeapYear = {";
//for (int i=0; i<=12; ++i) {
//cerr << cummulativeDaysInPreviousMonthInNonLeapYear[i] << ",";
//}
//cerr << "}" << endl;
//cerr << "cummulativeDaysInPreviousMonthInLeapYear = {";
//for (int i=0; i<=12; ++i) {
//cerr << cummulativeDaysInPreviousMonthInLeapYear[i] << ",";
//}
//cerr << "}" << endl;
}

static unsigned long
getCummulativeDaysPerMonth(int mm,bool leapYear) {
	long day = 0;
	if (mm <= 12 && mm > 0) {
		day = (leapYear ?  cummulativeDaysInPreviousMonthInLeapYear : cummulativeDaysInPreviousMonthInNonLeapYear)[mm];
		if (day == 0 && mm > 1) {
			// lazy computation of accumulated days
			populateCummulativeDaysPerMonth();
			// try again
			day = (leapYear ?  cummulativeDaysInPreviousMonthInLeapYear : cummulativeDaysInPreviousMonthInNonLeapYear)[mm];
		}
	}
	return day;
}

static bool isLeapYear(int yyyy) {
	return (yyyy % 4 == 0) && ((yyyy % 100 != 0) || (yyyy % 400 == 0));
}


static char *
getDateAsYYYYMMDDString(short yyyy,short mm,short dd) {
	ostrstream ost;
	ost << dec << setfill('0');
	ost << setw(4) << yyyy
		<< setw(2) << mm
		<< setw(2) << dd;
	ost << ends;
	return ost.str();
}

static char *
getTimeAsHHMMSSFFFString(short hour,short minute,short second,short milliSecond=0) {
	ostrstream ost;
	ost << dec << setfill('0');
	ost << setw(2) << hour
		<< setw(2) << minute
		<< setw(2) << second;
	if (milliSecond) {
		ost << '.' << setw(3) << milliSecond;
	}
	ost << ends;
	return ost.str();
}

static const char *
getDateTimeAsYYYYMMDDHHMMSSFFFString(int yyyy,int mm,int dd,short hour,short minute,short second,short milliSecond=0) {
	ostrstream ost;
	ost << dec << setfill('0');
	ost << setw(4) << yyyy
		<< setw(2) << mm
		<< setw(2) << dd
		<< setw(2) << hour
		<< setw(2) << minute
		<< setw(2) << second;
	if (milliSecond) {
		ost << '.' << setw(3) << milliSecond;
	}
	ost << ends;
	return ost.str();
}

//static char *getTimeAsHHMMSSFFFString(Time t) {
//	return getTimeAsHHMMSSFFFString(t.getHour(),t.getMinute(), t.getSecond(),t.getMilliSecond());
//}

// we use long long, not just long, otherwise for 32 bit long, earliest is 1901/12/13 and latest is 2038/01/19 (see http://en.wikipedia.org/wiki/Unix_time)

static void
getTimeFromMilliSecondsSinceEpoch(long long milliSecondsSinceEpoch,short &year,short &month,short &day,short &hour,short &min,short &sec,short &millisec) {
	year=1970;
	if (milliSecondsSinceEpoch >= 0) {
		while (true) {
			unsigned long long milliSecondsInThisYear = isLeapYear(year) ? milliSecondsInLeapYear : milliSecondsInNonLeapYear;
			if (milliSecondsSinceEpoch < milliSecondsInThisYear) {
				break;
			}
			milliSecondsSinceEpoch -= milliSecondsInThisYear;
//cerr << "getTimeFromMilliSecondsSinceEpoch(): removed year " << year << " is " << (isLeapYear(year) ? " " : "not ") << "leap, milliSecondsSinceEpoch now = "<< milliSecondsSinceEpoch << endl;
			++year;
		}
	}
	else {
		while (true) {
			--year;
			unsigned long long milliSecondsInThisYear = isLeapYear(year) ? milliSecondsInLeapYear : milliSecondsInNonLeapYear;
			milliSecondsSinceEpoch += milliSecondsInThisYear;
			if (milliSecondsSinceEpoch >= 0) {
				break;
			}
//cerr << "getTimeFromMilliSecondsSinceEpoch(): added year " << year << " is " << (isLeapYear(year) ? " " : "not ") << "leap, milliSecondsSinceEpoch now = "<< milliSecondsSinceEpoch << endl;
		}
	}
	{
		bool leap = isLeapYear(year);
		month=1;
		while (true) {
			Assert(month <= 12);
			unsigned long long milliSecondsInMonth = getMilliSecondsPerMonth(month,leap);
			if (milliSecondsSinceEpoch < milliSecondsInMonth) {
				break;
			}
			milliSecondsSinceEpoch -= milliSecondsInMonth;
//cerr << "getMilliSecondsSinceEpoch(): removed month " << month << " milliSecondsInMonth = " << milliSecondsInMonth << ", milliSecondsSinceEpoch now = "<< milliSecondsSinceEpoch << endl;
			++month;
		}
		day = (short)(milliSecondsSinceEpoch / milliSecondsInDay) + 1;
		milliSecondsSinceEpoch = milliSecondsSinceEpoch % milliSecondsInDay;
		
		hour = (short)(milliSecondsSinceEpoch / milliSecondsInHour);
		milliSecondsSinceEpoch = milliSecondsSinceEpoch % milliSecondsInHour;
		
		min = (short)(milliSecondsSinceEpoch / milliSecondsInMinute);
		milliSecondsSinceEpoch = milliSecondsSinceEpoch % milliSecondsInMinute;
		
		sec = (short)(milliSecondsSinceEpoch / milliSecondsInSecond);
		milliSecondsSinceEpoch = milliSecondsSinceEpoch % milliSecondsInSecond;
		
		millisec = (short)milliSecondsSinceEpoch;
		
		Assert(millisec >= 0 && millisec < 1000);
		Assert(sec >= 0 && sec < 60);
		Assert(min >= 0 && min < 60);
		Assert(hour >= 0 && hour < 24);
		Assert(day > 0 && day <= 31);
		Assert(month > 0 && month <= 12);
	}
}

static long long
getMilliSecondsSinceEpoch(short yyyy,short mm,short dd,short h=0,short m=0,short s=0,short ms=0) {
//cerr << "getMilliSecondsSinceEpoch(): " << getDateAsYYYYMMDDString(yyyy,mm,dd) << getTimeAsHHMMSSFFFString(h,m,s,ms) << endl;
	long long value = -1;
	if (mm <= 12 && mm >= 1 && dd <= 31 && dd >= 1) {
		//if (yyyy < 1900) {
		//	yyyy+=1900;
		//}
		value=0;
		if (yyyy >= 1970) {
			// do whole intervening years
			for (int y=1970; y<yyyy; ++y) {
				value += isLeapYear(y) ? daysInLeapYear : daysInNonLeapYear;
//cerr << "getMilliSecondsSinceEpoch(): added year " << y << " is " << (isLeapYear(y) ? " " : "not ") << "leap, value now = "<< value << " days (" << (value*secondsInDay) << " seconds)" << endl;
			}
		}
		else {
			for (int y=1970; y>yyyy; --y) {
				value -= isLeapYear(y-1) ? daysInLeapYear : daysInNonLeapYear;
//cerr << "getMilliSecondsSinceEpoch(): subtracted year " << y << " is " << (isLeapYear(y) ? " " : "not ") << "leap, value now = "<< value << " days (" << (value*secondsInDay) << " seconds)" << endl;
			}
		}
		{
			unsigned cummulativeDaysInPreviousMonth = getCummulativeDaysPerMonth(mm,isLeapYear(yyyy));
			value += cummulativeDaysInPreviousMonth;
//cerr << "getMilliSecondsSinceEpoch(): added month " << mm << " cummulativeDaysInPreviousMonth = " << cummulativeDaysInPreviousMonth << " days (" << (cummulativeDaysInPreviousMonth*secondsInDay) << " seconds)" << ", value now = "<< value << " days (" << (value*secondsInDay) << " seconds)" << endl;
			value += (dd-1);												// want difference from epoch, which is 1st, so -1
//cerr << "getMilliSecondsSinceEpoch(): added day - 1 " << (dd-1) << ", value now = "<< value << " days (" << (value*secondsInDay) << " seconds)" << endl;
			value *= milliSecondsInDay;
		}
	}
	value += h*milliSecondsInHour + m*milliSecondsInMinute + s*milliSecondsInSecond + ms;
//cerr << "getMilliSecondsSinceEpoch(): value in milliseconds " << value << endl;
{
short year,month,day,hour,minute,second,millisec;
getTimeFromMilliSecondsSinceEpoch(value,year,month,day,hour,minute,second,millisec);
//cerr << "getMilliSecondsSinceEpoch(): For " << getDateAsYYYYMMDDString(yyyy,mm,dd) << getTimeAsHHMMSSFFFString(h,m,s,ms) << " value in milliseconds is " << value << ", reversed is " << getDateAsYYYYMMDDString(year,month,day) << getTimeAsHHMMSSFFFString(hour,minute,second,millisec) << endl;
Assert(year == yyyy);
Assert(month == mm);
Assert(day == dd);
Assert(hour == h);
Assert(minute == m);
Assert(second == s);
Assert(millisec == ms);
}
	return value;
}

static long long
getMilliSecondsSinceEpochFromDateTimeString(const char *dateString,const char *timeString,const char *timezoneString) {
	short yyyy = 1970;
	short mm = 1;
	short dd = 1;
	short hour = 0;
	short minute = 0;
	short second = 0;
	short millisec = 0;
	Date *originalDate = new Date(dateString,DateOrderMonthMiddleYearFirst);
	if (originalDate->isgood()) {
		yyyy = originalDate->getYYYY();
		mm = originalDate->getMM();
		dd = originalDate->getDD();
	}
	if (timeString && strlen(timeString) > 0) {
		Time *originalTime = new Time(timeString);
		if (originalTime->isgood()) {
			hour = originalTime->getHour();
			minute = originalTime->getMinute();
			second = originalTime->getSecond();
			millisec = originalTime->getMilliSecond();
		}
	}
	long long milliSecondsSinceEpoch = getMilliSecondsSinceEpoch(yyyy,mm,dd,hour,minute,second,millisec);
	// should add or subtract timezone offset here :(
//cerr << "getMilliSecondsSinceEpochFromDateTimeString(): " << dateString << (timeString ? timeString : "" ) << (timezoneString ? timezoneString : "" ) << "  -> " << milliSecondsSinceEpoch << endl;
	return milliSecondsSinceEpoch;
}

static long long
getMilliSecondsSinceEpochFromDateTimeString(const char *s) {
	// YYYYMMDDHHMMSS[.FFFFFF[&ZZZZ]] where & is + or -
	size_t dateTimeLength = strlen(s);
	char *dateString = new char[9];
	strncpy(dateString,s,8);
	dateString[8]=0;
	char *timeString = NULL;
	char *timezoneString = NULL;
	if (dateTimeLength > 8) {
		size_t timeLength = dateTimeLength-8;
		timeString = new char[timeLength+1];
		strncpy(timeString,s+8,timeLength);
		size_t endOfTimeString = timeLength;
		const char *startOfTimezone = strchr(timeString,'+');
		if (!startOfTimezone) {
			startOfTimezone = strchr(timeString,'-');
		}
		if (startOfTimezone) {
			size_t timezoneLength = timeLength-(startOfTimezone-timeString);
			timezoneString = new char[timezoneLength+1];
			strcpy(timezoneString,startOfTimezone);
			timezoneString[timezoneLength]=0;
			timeLength = startOfTimezone-timeString;
		}
		timeString[timeLength]=0;
	}
	return getMilliSecondsSinceEpochFromDateTimeString(dateString,timeString,timezoneString);
}

static const char *
getReplacementDateTimeUsingCachedDelta(const char *originalString) {
	long long originalOffsetFromEpoch = getMilliSecondsSinceEpochFromDateTimeString(originalString);
	if (!dateDeltaSet) {
		dateDelta = baseDateOffsetFromEpoch - originalOffsetFromEpoch;
cerr << "delta = " << dateDelta << endl;
		dateDeltaSet = true;
	}
	long long replacementOffsetFromEpoch = originalOffsetFromEpoch + dateDelta;
	short year,month,day,hour,minute,second,millisec;
	getTimeFromMilliSecondsSinceEpoch(replacementOffsetFromEpoch,year,month,day,hour,minute,second,millisec);
	if (forceFirstDay) {
		day=1;
	}
	if (forceFirstMonth) {
		month=1;
	}
	const char *replacementString = getDateTimeAsYYYYMMDDHHMMSSFFFString(year,month,day,hour,minute,second,millisec);
//cerr << "getReplacementDateTimeUsingCachedDelta(): replacement for datetime " << originalString << " is " << replacementString << endl;
	return replacementString;
}

static void
getReplacementDateTimeUsingCachedDelta(const char *originalDateString,const char *originalTimeString,const char *&replacementDateString,const char *&replacementTimeString) {
	long long originalOffsetFromEpoch = getMilliSecondsSinceEpochFromDateTimeString(originalDateString,originalTimeString,NULL);
	if (!dateDeltaSet) {
		dateDelta = baseDateOffsetFromEpoch - originalOffsetFromEpoch;
cerr << "delta = " << dateDelta << endl;
		dateDeltaSet = true;
	}
	long long replacementOffsetFromEpoch = originalOffsetFromEpoch + dateDelta;
	short year,month,day,hour,minute,second,millisec;
	getTimeFromMilliSecondsSinceEpoch(replacementOffsetFromEpoch,year,month,day,hour,minute,second,millisec);
	if (forceFirstDay) {
		day=1;
	}
	if (forceFirstMonth) {
		month=1;
	}
	replacementDateString = getDateAsYYYYMMDDString(year,month,day);
	replacementTimeString = getTimeAsHHMMSSFFFString(hour,minute,second,millisec);
//cerr << "getReplacementDateTimeUsingCachedDelta(): replacement for date " << originalDateString << " is " << replacementDateString << endl;
//cerr << "getReplacementDateTimeUsingCachedDelta(): replacement for time " << (originalTimeString ? originalTimeString : "") << " is " << replacementTimeString << endl;
}

static Tag
getTagOfTimeAttributeCorrespondingToDateAttribute(Tag dateTag,TextOutputStream &log) {
	// in map, odd entries are date, succeeding even entry is corresponding time
	Tag *p = mapDateTagToTimeTag;
	while (*p && *p != dateTag) {
		p+=2;
	}
	Tag timeTag = *(p+1);
//log << "getTagOfTimeAttributeCorrespondingToDateAttribute(): for date tag "; dateTag.write(log); log << " time tag is "; timeTag.write(log); log << endl;
	return timeTag;
}

static const char *
getCorrespondingValueOfTimeAttributeCorrespondingToDateAttribute(AttributeList& list,Attribute *aDate,int i,TextOutputStream &log) {
//log << "getCorrespondingValueOfTimeAttributeCorrespondingToDateAttribute(): For date "; if (aDate) aDate->write(log); log << endl;
	char *timeString = NULL;
	Tag dateTag = aDate->getTag();
	Tag timeTag = getTagOfTimeAttributeCorrespondingToDateAttribute(dateTag,log);
	if (timeTag) {
		Attribute *aTime = list[timeTag];
//log << "getCorrespondingValueOfTimeAttributeCorrespondingToDateAttribute(): Got time "; if (aTime) aTime->write(log); log << endl;
		if (aTime && i < aTime->getVM()) {
			aTime->getValue(i,timeString);
		}
	}
	return timeString;
}

static bool
changeDatesAndTimes(AttributeList& list,TextOutputStream &log)
{
	bool succeeded=true;
	if (!keepZone) {
		// will be added with +0000 to root list only; this routine is also called for sequence item lists
		list-=TagFromName(TimezoneOffsetFromUTC);
	}
	if (!dateDeltaSet) {
		// try and make sure that the Study Date and Time of the first image encountered are the "earliest", so they don't get move to the day before
		Attribute *a = list[TagFromName(StudyDate)];
		char *originalDateString;
		if (a && a->getValue(0,originalDateString) && originalDateString && strlen(originalDateString)) {
			const char *originalTimeString = getCorrespondingValueOfTimeAttributeCorrespondingToDateAttribute(list,a,0,log);
			const char *replacementDateString = NULL;
			const char *replacementTimeString = NULL;
			// this next will set dateDeltaSet
			getReplacementDateTimeUsingCachedDelta(originalDateString,originalTimeString,replacementDateString,replacementTimeString);
			// ignore result ... weill do it for real when iterating through whole attribute list
		}
	}
	AttributeListIterator listi(list);
	while (!listi) {
		Attribute *a=listi();
		Assert(a);
		if (!::loopOverListsInSequences(a,log,&::changeDatesAndTimes)) {
			succeeded=false;
		}
//a->write(log); log << endl;
		const char *vr = a->getVR();
		bool isDate = false;
		bool isTime = false;
		bool isDateTime = false;
		if (vr) {
			if (strcmp(vr,"DA") == 0) {
				isDate = true;
			}
			else if (strcmp(vr,"TM") == 0) {
				isTime = true;
			}
			else if (strcmp(vr,"DT") == 0) {
				isDateTime = true;
			}
		}
		if (isDate || isTime || isDateTime) {
//log << "Was: "; a->write(log); log << endl;
			int n = a->getVM();
			int i;
			for (int i=0; i<n; ++i) {
				char *originalString;
				if (a->getValue(i,originalString) && originalString && strlen(originalString) && isAttributeToBeReplaced(a)) {
//cerr << "Original String = " << originalString << endl;
					const char *replacementString = NULL;
					{
						if (isDate) {
							// regardless of whether there might be a cached value ... do it from scratch in order to pick up corresponding time
							// since the same date value may be used in different date attributes
							const char *originalTimeString = getCorrespondingValueOfTimeAttributeCorrespondingToDateAttribute(list,a,i,log);
							const char *replacementTimeString = NULL;
							getReplacementDateTimeUsingCachedDelta(originalString,originalTimeString,replacementString,replacementTimeString);
							// add any replacement time string to the timeHashMap ... will actually be used later
							if (originalTimeString && strlen(originalTimeString) > 0 && replacementTimeString && strlen(replacementTimeString)) {
//cerr << "Add time entry = " << originalTimeString << ", " << replacementTimeString << endl;
								(*timeHashMap)+=new EntryString(StrDup(originalTimeString),replacementTimeString);
							}
						}
						if (isTime) {
							// Only handle time entries are created as pairs with date entries ... assumes date attributes always encountered before corresponding time
							EntryString *entry = (*timeHashMap)[originalString];
							if (entry) {
//cerr << "Found time entry = " << entry->getKey().getString() << ", " << entry->getValue() << endl;
								replacementString = entry->getValue();
							}
						}
						else if (isDateTime) {
							EntryString *entry = (*dateTimeHashMap)[originalString];
							if (entry) {
//cerr << "Found datetime entry = " << entry->getKey().getString() << ", " << entry->getValue() << endl;
								replacementString = entry->getValue();
							}
							if (!replacementString) {
								replacementString = getReplacementDateTimeUsingCachedDelta(originalString);
//cerr << "Add datetime entry = " << originalString << ", " << replacementString << endl;
								if (replacementString) {
									(*dateTimeHashMap)+=new EntryString(StrDup(originalString),replacementString);
								}
							}
						}
					}
					if (replacementString) {
//cerr << "Replace " << originalString << " with " << replacementString << endl;
						a->setValue(i,replacementString);
//log << "---> "; a->write(log); log << endl;
					}
				}
			}
		}
		//	succeeded=false;
		++listi;
	}
	return succeeded;
}

static const char *
makeNewFileName(const char *outdir,const char *filename) {
//cerr << "makeNewFileName(): outdir " << outdir << endl;
//cerr << "makeNewFileName(): filename " << filename << endl;
	Assert(outdir);
	Assert(filename);
	size_t outdirLength = strlen(outdir);
	size_t filenameLength = strlen(filename);
	const char *filenameLastSeparator = strrchr(filename,'/');
	if (filenameLastSeparator != NULL) {
		filename = filenameLastSeparator+1;
		filenameLength = strlen(filename);
//cerr << "makeNewFileName(): found last component " << filename << endl;
	}
	size_t length = outdirLength + 1 + filenameLength;
	char *string=new char[length+1];
	strcpy(string,outdir);
	strcat(string,"/");
	strcat(string,filename);
	string[length]=0;
//cerr << "makeNewFileName(): returning " << string << endl;
	return string;
}

static void handleOneFile(const char *outdir,const char *filename,
			DicomInputOptions &dicom_input_options,
			DicomOutputOptions &dicom_output_options,
			bool listnames,bool success,bool verbose,
			TextOutputStream &log) {

		Assert(filename);
		if (listnames) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *ifstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *ifstr=new ifstream(filename);
#endif
		if (!ifstr || !*ifstr || !ifstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filename) cerr <<" - \"" << filename << "\"";
			cerr << endl;
			success=false;
			return;
		}

		DicomInputStream din(*(istream *)ifstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;

		if (verbose) log << "******** While reading ... " << filename << " ... ********" << endl;
		list.read(din,&log,verbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			return;
		}

		// do NOT close input file yet, else deferred pixel data read for write will fail
		
		changeDatesAndTimes(list,log);
		if (!keepZone) {
			// already removed previously, if present
			Attribute *a = new ShortStringAttribute(TagFromName(TimezoneOffsetFromUTC),"+0000");
			list+=a;
		}

		if (addDeidentificationMethod) {
			Attribute *a = list[TagFromName(DeidentificationMethod)];
			if (a) {
				a->addValue("dcdtchg change dates");
			}
			else {
				a = new LongStringAttribute(TagFromName(DeidentificationMethod),"dcdtchg change dates");
				list += a;
			}
		}
		
		const char *outfilename = makeNewFileName(outdir,filename);
		Assert(outfilename);
		if (listnames) log << "Writing \"" << outfilename << "\"" << endl;
		
#ifdef USEBINARYFLAGFOROUTPUTOPENMODE
		ofstream *ofstr=new ofstream(outfilename,
			ios::out|ios::trunc|ios::binary);
#else
		ofstream *ofstr=new ofstream(outfilename,ios::out|ios::trunc);
#endif
		if (!ofstr || !*ofstr || !ofstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileWriteOpenFailed);
			if (outfilename) cerr <<" - \"" << outfilename << "\"";
			cerr << endl;
			success=false;
			return;
		}
		
		DicomOutputStream dout(*(ostream *)ofstr,
			dicom_output_options.transfersyntaxuid,
			dicom_output_options.usemetaheader,
			dicom_output_options.useimplicitmetaheader,
			dicom_output_options.addtiff);

		if (!usualManagedAttributeListWrite(list,dout,
			dicom_output_options,log,verbose)) success=false;
		
		if (ofstr) {
			ofstr->close();
			delete ofstr;
		}
		if (outfilename) {
			delete outfilename;
		}
		// Now can close input file
		if (ifstr) {
			ifstr->close();
			delete ifstr;
		}
}

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool listnames=options.get("l");

	const char *outdir=0;
	bad = !options.get("outdir",outdir) || bad;

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	const char *filelistfile=0;
	(void)(options.get("filelist",filelistfile) || options.get("f",filelistfile));

	baseDateOffsetFromEpoch = 0;
	{
		const char *baseDateString=0;
		(void)(options.get("basedate",baseDateString));
		if (baseDateString == NULL) {
			baseDateString = "20010101000000+0000";
		}
		baseDateOffsetFromEpoch = getMilliSecondsSinceEpochFromDateTimeString(baseDateString);
	}
	
	forceFirstDay=options.get("forcefirstday");
	forceFirstMonth=options.get("forcefirstmonth");

	keepZone=options.get("keepzone");
	
	addDeidentificationMethod=!options.get("nomethod");
	
	dateDeltaSet = options.get("delta",dateDelta);	// if not supplied, will be set once first date is encountered
//cerr << "delta = " << dateDelta << endl;

	int numberofinputfiles=!options;

	const char **listoffilenames = new const char * [numberofinputfiles];
	const char **ptr = listoffilenames;
	const char *filename;

	while(!options && (filename=options())) {
		++options;
		*ptr++=filename;
	}

	options.done();

	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " -outdir dirname"
			<< " [-v|-verbose]"
			<< " [-l]"
			<< " [-f|-filelist filename]"
			<< " [-delta ms]"
			<< " [-basedate yyyymmdd]"
			<< " [-forcefirstday]"
			<< " [-forcefirstmonth]"
			<< " [-keepzone]"
			<< " [-nomethod]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (!dicom_output_options.transfersyntaxuid)
		dicom_output_options.transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;

	bool success=true;
	TextOutputStream log(cerr);

	timeHashMap = new HashMap(1000);
	dateTimeHashMap = new HashMap(1000);
	
	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		handleOneFile(outdir,listoffilenames[i],dicom_input_options,dicom_output_options,listnames,success,verbose,log);
	}

	if (filelistfile) {
		ifstream *flfstr=new ifstream(filelistfile);
		if (!flfstr || !*flfstr || !flfstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filelistfile) cerr <<" - \"" << filename << "\"";
			cerr << endl;
			bad=true;
		}
		else {
			while (flfstr->peek() != EOF) {
				const int lineBufferSize=2048;
				char lineBuffer[lineBufferSize];
				flfstr->getline(lineBuffer,2048);
				if (strlen(lineBuffer)) {
					handleOneFile(outdir,lineBuffer,dicom_input_options,dicom_output_options,listnames,success,verbose,log);
				}
				// else skip blank lines
			}
		}
	}

	if (numberofinputfiles && listoffilenames) delete[] listoffilenames;
	return success ? 0 : 1;
}

