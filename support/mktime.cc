#include <time.h>
#include <iostream.h>
#include <assert.h>
#include <stdlib.h>

// specify string datetime (UTC) on command line

// return seconds since 00:00:00 UTC, January 1, 1970

time_t
mktime_fromstrings(const char *yyyy,const char *mm,const char *dd,const char *hh,const char *min,const char *ss)
{
	struct tm sourcetime;

	sourcetime.tm_isdst=-1;		// don't mess with DST
	sourcetime.tm_year = 70;	/* years since 1900 */
	sourcetime.tm_mon  = 0;		/* months since January [0, 11] */
	sourcetime.tm_mday = 1;		/* day of the month [1, 31] */
	sourcetime.tm_hour = 0;		/* hour since midnight [0, 23] */
	sourcetime.tm_min  = 0;		/* minutes after the hour [0, 59] */
	sourcetime.tm_sec  = 0;		/* seconds after the minute [0, 61]  */

	time_t timezoneoffset=mktime(&sourcetime);
	assert(timezoneoffset != -1);

	sourcetime.tm_isdst=-1;			// don't mess with DST
	sourcetime.tm_year = atoi(yyyy)-1900;	/* years since 1900 */
	sourcetime.tm_mon  = atoi(mm)-1;	/* months since January [0, 11] */
	sourcetime.tm_mday = atoi(dd);		/* day of the month [1, 31] */
	sourcetime.tm_hour = atoi(hh);		/* hour since midnight [0, 23] */
	sourcetime.tm_min  = atoi(min);		/* minutes after the hour [0, 59] */
	sourcetime.tm_sec  = atoi(ss);		/* seconds after the minute [0, 61]  */

	time_t value = mktime(&sourcetime);
	assert(value != -1);

	cout <<        sourcetime.tm_year+1900
	     << "/" << sourcetime.tm_mon+1
	     << "/" << sourcetime.tm_mday
	     << " " << sourcetime.tm_hour
	     << ":" << sourcetime.tm_min
	     << ":" << sourcetime.tm_sec
	     << " -> " << (value - timezoneoffset) << " seconds since 00:00:00, January 1, 1970"
	     << endl;

	return value;
}

int main(int argc,char **argv)
{
//cerr << "timezone = " << timezone << endl;
//cerr << "daylight = " << daylight << endl;

	if (argc == 7) {
		(void)mktime_fromstrings(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
	}
	else {
		assert(0);
	}
}
