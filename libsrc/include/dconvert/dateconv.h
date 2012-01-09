#ifndef __Header_dateconv__
#define __Header_dateconv__

//#include "generic/datetype.h"		// assume already included

const unsigned long	SecondsPerDay       =60*60*24;
const unsigned long	SecondsPerHour      =60*60;
const unsigned long	SecondsPerMinute    =60;

static bool
isleap(short year)
{
	return year%400 == 0 || (year%4 == 0 && year%100 != 0);
	//return year%4 == 0 && year%20 != 0;
}

static unsigned long
SecondsPerYear(short year)
{
	return 60*60*24*(isleap(year) ? 366 : 365);
}

static unsigned SecondsPerMonthNormal [13] = {
	0,
	31*SecondsPerDay,
	28*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay
};

static unsigned SecondsPerMonthLeap [13] = {
	0,
	31*SecondsPerDay,
	29*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay,
	30*SecondsPerDay,
	31*SecondsPerDay
};

static unsigned long
SecondsPerMonth(short year,short month)
{
	return isleap(year)
		? SecondsPerMonthLeap[month]
		: SecondsPerMonthNormal[month];
}

template <class T,int epochyear>
class DateTimeEpoch_Base {
	T binary;
public:
	DateTimeEpoch_Base(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	DateTimeEpoch_Base(unsigned long secs)
		{
			binary=secs;
		}

	DateTimeEpoch_Base<T,epochyear>& operator=(const DateTimeEpoch_Base<T,epochyear>& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator DateTime() const
		{
			// epoch is 00:00:00 1 Jan on epochyear

			unsigned long SecondsSinceEpoch=binary;
			unsigned long Seconds = SecondsSinceEpoch;
			short Year 	= epochyear;
			while (Seconds >= SecondsPerYear(Year)) {
				Seconds-=SecondsPerYear(Year);
				++Year;
			}

			short Month = 0;
			while (Seconds >= SecondsPerMonth(Year,Month)) {
				Seconds-=SecondsPerMonth(Year,Month);
				++Month;
			}

			short Day=(short)(Seconds/SecondsPerDay);
			Seconds-=Day*SecondsPerDay;
			++Day;	// days are numbered from 1 not 0 !
			short Hours=(short)(Seconds/SecondsPerHour);
			Seconds-=Hours*SecondsPerHour;
			short Minutes=(short)(Seconds/SecondsPerMinute);
			Seconds-=Minutes*SecondsPerMinute;

			return DateTime(Year,Month,Day,Hours,Minutes,(short)Seconds);
		}
};

typedef DateTimeEpoch_Base<Uint32_L,1970> Unix_DateTime_L;
typedef DateTimeEpoch_Base<Uint32_B,1970> Unix_DateTime_B;

static unsigned long
DaysPerYear(short year)
{
	return isleap(year) ? 366 : 365;
}

static unsigned DaysPerMonthNormal [13] = {
	0,
	31,
	28,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};

static unsigned DaysPerMonthLeap [13] = {
	0,
	31,
	29,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};

static unsigned long
DaysPerMonth(short year,short month)
{
	return isleap(year)
		? DaysPerMonthLeap[month]
		: DaysPerMonthNormal[month];
}

template <class T,int epochyear>
class DateEpoch_Base {
	T binary;
public:
	DateEpoch_Base(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	DateEpoch_Base(unsigned long secs)
		{
			binary=secs;
		}

	DateEpoch_Base<T,epochyear>& operator=(const DateEpoch_Base<T,epochyear>& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator Date() const
		{
			// epoch is 0 Jan on epochyear

			unsigned long Days=binary-1;
			short Year 	= epochyear;
			while (Days >= DaysPerYear(Year)) {
				Days-=DaysPerYear(Year);
				++Year;
			}

			short Month = 0;
			while (Days >= DaysPerMonth(Year,Month)) {
				Days-=DaysPerMonth(Year,Month);
				++Month;
			}

			return Date(Year,Month,(short)Days);
		}
};

typedef DateEpoch_Base<Uint32_B,1980> Pace_Date;

template <class T>
class Time_Milliseconds_Base {
	T binary;
public:
	Time_Milliseconds_Base(void)
		{
			memset((char *)this,0,sizeof *this);
		}

	Time_Milliseconds_Base(unsigned long ms)
		{
			binary=ms;
		}

	Time_Milliseconds_Base<T>& operator=(const Time_Milliseconds_Base<T>& i)
		{
			memcpy((char *)this,(const char *)&i,sizeof *this);
			return *this;
		}

	operator Time() const
		{
			// epoch is 0 Jan on epochyear

			unsigned long ticks=binary;
			short Milliseconds=(short)(ticks%1000);
			short Seconds=(short)((ticks/1000)%60);
			short Minutes=(short)((ticks/(1000*60))%60);
			short Hours  =(short)((ticks/(1000*60*60))%60);
			return Time(Hours,Minutes,Seconds);
		}
};

typedef Time_Milliseconds_Base<Uint32_B> Time_Milliseconds_B;
typedef Time_Milliseconds_Base<Uint32_L> Time_Milliseconds_L;

#endif /* __Header_dateconv__ */
