#ifndef __Header_datetype__
#define __Header_datetype__

#ifndef DEFAULTGUESSEDDATEORDER
#define DEFAULTGUESSEDDATEORDER DateOrderDayMiddleYearLast
#endif

enum DateOrder {
	DateOrderMonthMiddleYearLast,
	DateOrderDayMiddleYearLast,
//	DateOrderDayMiddleYearFirst,
	DateOrderMonthMiddleYearFirst
};

class Date {
protected:
	short dd;
	short mm;
	char *mmm;
	short yyyy;
	char tzo[6];
	bool goodflag;

	char *makeMMMFromMM(short m) const;
	bool extractMMFromMMM(const char *&src,size_t &length,short &m) const;

	bool getDatePossibilityFromString(
		const char *&src,size_t &length,
		short &value,int &value_ismmm) const;

	bool getDatePossibilitiesFromString(
		const char *src,size_t length,
		short &v1,int &v1_ismmm,
		short &v2,int &v2_ismmm,
		short &v3,int &v3_ismmm) const;

	bool validateDateValues(short d,short m,short &y);	// May update year
	void assignIndex(int& assignto,int not1,int not2) const;
	bool getDDMMYYFromString(const char *src,size_t length,DateOrder order);
	void makeTimezoneOffset(long tm_gmtoff);
public:
	Date(short y,short m,short d);
	Date(short y,const char *mmm,short d);
	Date(const char *str,enum DateOrder order=DEFAULTGUESSEDDATEORDER);
	Date(Date const & date);
	Date(void);
	Date& operator=(const Date & date);
	~Date(void);

	short getDD(void) const		{ return dd; }
	short getMM(void) const		{ return mm; }
	const char *getMMM(void) const	{ return mmm; }
	short getYYYY(void) const	{ return yyyy; }
	short getYY(void) const		{ return yyyy%100; }
	const char *getTZOffset(void) const { return tzo; }
	bool isgood() const		{ return goodflag; }

	operator const Uint32(void) const
		{
		  return (Uint32)yyyy*10000u + mm*100u + dd;
		}
};

class Time {
protected:
	short hour;
	short min;
	short sec;
	short millisec;
	bool goodflag;

	bool validateTimeValues(short h,short m,short s,short ms) const;
	bool getHHMMSSFromString(const char *src,size_t length);
public:
	Time(short h,short m,short s,short ms=0);
	Time(const char *str);
	Time(Time const & time);
	Time(Uint32 totalms);
	Time(void);

	short getHour(void) const		{ return hour; }
	short getMinute(void) const		{ return min; }
	short getSecond(void) const		{ return sec; }
	short getMilliSecond(void) const	{ return millisec; }
	bool isgood() const			{ return goodflag; }

	operator const Uint32(void) const
		{
		  return (Uint32)hour*3600000u + min*60000u + sec*1000u + millisec;
		}
};

class DateTime : public Date, public Time {
public:
	DateTime(short year,short month,short day,short hour,short min,short sec)
		: Date(year,month,day), Time(hour,min,sec) {}

	DateTime(const char *d,const char *t,enum DateOrder order=DEFAULTGUESSEDDATEORDER)
		: Date(d,order), Time(t) {}

	DateTime(short year,const char *month,short day,short hour,short min,short sec)
		: Date(year,month,day), Time(hour,min,sec) {}

	DateTime(Date const & date,Time const & time)
		: Date(date), Time(time) {}

	bool isgood() const		{ return Date::isgood() && Time::isgood(); }

};

#endif /* __Header_datetype__ */
