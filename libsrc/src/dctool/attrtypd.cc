#include "attrtype.h"

DateStringAttribute::DateStringAttribute(Tag t,Date const & date)
		: NonNumericStringAttribute(t)
{
			ostrstream ost;
			ost << dec << setfill('0');
			if (date.isgood()) {
				ost << setw(4) << date.getYYYY()
				    << setw(2) << date.getMM()
				    << setw(2) << date.getDD();
			}
			ost << ends;
			char *s = ost.str();
			addValue(s);
			if (s) delete[] s;
}

DateStringAttribute::DateStringAttribute(Tag t,DateTime const & datetime)
		: NonNumericStringAttribute(t)
		{
			ostrstream ost;
			ost << dec << setfill('0');
			if (datetime.isgood()) {
				ost << setw(4) << datetime.getYYYY()
				    << setw(2) << datetime.getMM()
				    << setw(2) << datetime.getDD();
			}
			ost << ends;
			char *s = ost.str();
			addValue(s);
			if (s) delete[] s;
		}

DateTimeStringAttribute::DateTimeStringAttribute(Tag t,Date const & date,Time const & time)
		: NonNumericStringAttribute(t)
		{
			ostrstream ost;
			ost << dec << setfill('0');
			if (date.isgood()) {
				ost << setw(4) << date.getYYYY()
				    << setw(2) << date.getMM()
				    << setw(2) << date.getDD();
				if (time.isgood()) {
					ost << setw(2) << time.getHour()
					    << setw(2) << time.getMinute()
					    << setw(2) << time.getSecond();
					if (time.getMilliSecond())
						ost << '.' << setw(3) << time.getMilliSecond();
				}
			}
			ost << ends;
			char *s = ost.str();
			addValue(s);
			if (s) delete[] s;
		}
TimeStringAttribute::TimeStringAttribute(Tag t,Time const & time)
		: NonNumericStringAttribute(t)
		{
			ostrstream ost;
			ost << dec << setfill('0');
			if (time.isgood()) {
				ost << setw(2) << time.getHour()
				    << setw(2) << time.getMinute()
				    << setw(2) << time.getSecond();
				if (time.getMilliSecond())
					ost << '.' << setw(3) << time.getMilliSecond();
			}
			ost << ends;
			char *s = ost.str();
			addValue(s);
			if (s) delete[] s;
		}

TimeStringAttribute::TimeStringAttribute(Tag t,DateTime const & datetime)
		: NonNumericStringAttribute(t)
		{
			ostrstream ost;
			ost << dec << setfill('0');
			if (datetime.isgood()) {
				ost << setw(2) << datetime.getHour()
				    << setw(2) << datetime.getMinute()
				    << setw(2) << datetime.getSecond();
				if (datetime.getMilliSecond())
					ost << '.' << setw(3) << datetime.getMilliSecond();
			}
			ost << ends;
			char *s = ost.str();
			addValue(s);
			if (s) delete[] s;
		}
