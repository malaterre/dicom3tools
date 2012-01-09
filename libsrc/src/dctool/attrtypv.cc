#include <ctype.h>

#include "attr.h"
#include "attrtype.h"
#include "mesgtext.h"
#include "elmdict.h"

static bool inline iscntrlok(char c) {	// Per PS 3.5 6.1.3
	return (c == 0x1b /*ESC*/ || c == 0x0a /*LF*/ || c == 0x0c /*FF*/ || c == 0x0d /*CR*/);
}

static bool inline isescape(char c) {	// Per PS 3.5 6.1.3
	return (c == 0x1b /*ESC*/);
}

static void
writeWarningVRValue(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value)
{
	log << WMsgDC(ValueDubiousForThisVR) << " - ";
	tag.write(log,dict);
	log << " " << (vr ? vr : 0)
	    << " [" << dec << valuenumber << "]"
	    << " = <" << (value ? value : "") << "> - ";
}

static void
writeErrorBadVR(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr)
{
	log << EMsgDC(ValueInvalidForThisVR) << " - ";
	tag.write(log,dict);
	log << " " << (vr ? vr : 0)
	    << " - ";
}

static void
writeErrorBadVRValue(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value)
{
	log << EMsgDC(ValueInvalidForThisVR) << " - ";
	tag.write(log,dict);
	log << " " << (vr ? vr : 0)
	    << " [" << dec << valuenumber << "]"
	    << " = <" << (value ? value : "") << "> - ";
}

static void
writeErrorBadTrailingChar(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,char c)
{
	writeErrorBadVR(log,dict,tag,vr);
	log << MMsgDC(TrailingCharacterInvalidForThisVR)
	    << " = '";
	if (isprint(c)) log << c;
	log << "' (" << hex << (unsigned(c)&0xff) << dec << ")"
	    << endl;
}

static void
writeErrorBadVRCharNL(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,char c)
{
	writeErrorBadVR(log,dict,tag,vr);
	log << MMsgDC(CharacterInvalidForThisVR)
	    << " = '";
	if (isprint(c)) log << c;
	log << "' (" << hex << (unsigned(c)&0xff) << dec << ")"
	    << endl;
}

static void
writeErrorBadVRCharNL(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value,
	char c)
{
	writeErrorBadVRValue(log,dict,tag,vr,valuenumber,value);
	log << MMsgDC(CharacterInvalidForThisVR)
	    << " = '";
	if (isprint(c)) log << c;
	log << "' (" << hex << (unsigned(c)&0xff) << dec << ")"
	    << endl;
}

static void
writeErrorBadCharacterRepertoireCharNL(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value,
	char c)
{
	writeErrorBadVRValue(log,dict,tag,vr,valuenumber,value);
	log << MMsgDC(CharacterInvalidForCharacterRepertoire)
	    << " = '";
	if (isprint(c)) log << c;
	log << "' (" << hex << (unsigned(c)&0xff) << dec << ")"
	    << endl;
}

static void
writeErrorBadVRLengthNL(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value,
	unsigned length,const char *expected)
{
	writeErrorBadVRValue(log,dict,tag,vr,valuenumber,value);
	log << MMsgDC(LengthInvalidForThisVR)
	    << " = " << dec << length
	    << ", " << MMsgDC(Expected) << " " << (expected ? expected : "")
	    << endl;
}

static void
writeErrorBadVRRange(TextOutputStream& log,ElementDictionary *dict,Tag tag,const char *vr,int valuenumber,const char *value,
	const char *expected)
{
	writeErrorBadVRValue(log,dict,tag,vr,valuenumber,value);
	log << MMsgDC(RangeInvalidForThisVR)
	    << ", " << MMsgDC(Expected) << " " << (expected ? expected : "")
	    << endl;
}

bool
Attribute::validateRetired(TextOutputStream& log,ElementDictionary *dict) const
{
	Tag t = getTag();
	bool retired = dict && dict->isRetired(t);
	if (retired) {
		log << WMsgDC(RetiredAttribute) << " - ";
		tag.write(log,dict);
		log << endl;
	}
	return !retired;
}

bool
ApplicationEntityAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 16) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 16");
			ok=false;
		}
		const char *p=s;
		while (*p) {
			if (iscntrl(*p) || (isspace(*p) && *p != ' ')) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
AgeStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) != 4) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"== 4");
			ok=false;
		}
		else {
			int j;
			for (j=0; j<3; ++j) {
				if (!isdigit(s[j])) {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,s[j]);
					ok=false;
				}
			}
			if (s[3] != 'D' && s[3] != 'W' && s[3] != 'M' && s[3] != 'Y') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,s[3]);
				ok=false;
			}
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingSpace && getVM()%2 == 1) {	// set during StringAttribute::read(); need to check VM, since if odd number of delimiters need some sort of padding, even if standard doesn't specify it :(
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),' ');
		ok=false;
	}
	return ok;
}

bool
CodeStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 16) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 16");
			ok=false;
		}
		const char *p=s;
		while (*p) {
			// should check for invalid embedded spaces :(
			if (!isupper(*p) && !isdigit(*p) && *p != ' ' && *p != '_') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
CodeStringFileComponentAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 8) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 8");
			ok=false;
		}
		const char *p=s;
		while (*p) {
			if (!isupper(*p) && !isdigit(*p) && *p != '_') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}

	// should check for VM <= 8 here :(

	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
DateStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		int l=strlen(s);
		if (l == 8 || l == 10) {
			const char *p=s;
			if (*p != '1' &&  *p != '2') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (*p == '.' && l == 10) ++p;
			if (*p != '0' &&  *p != '1') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (*p == '.' && l == 10) ++p;
			if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
		}
		else {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,l,"== 8 or 10");
			ok=false;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingSpace && getVM()%2 == 1) {	// set during StringAttribute::read(); need to check VM, since if odd number of delimiters need some sort of padding, even if standard doesn't specify it :(
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),' ');
		ok=false;
	}
	return ok;
}

bool
DateTimeStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		int l=strlen(s);
		if (l >= 2 && l <= 26) {
			const char *p=s;
			if (*p != '1' &&  *p != '2') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (*p) {	// Not just CC
				if (!isdigit(*p)) {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
				}
				++p;
				if (!isdigit(*p)) {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
				}
				++p;
				if (*p) {	// Not just CCYY
					if (*p != '0' &&  *p != '1') {
						writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
						ok=false;
					}
					++p;
					if (!isdigit(*p)) {
						writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
						ok=false;
					}
					++p;
					if (*p) {	// Not just CCYYMM
						if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3') {
							writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
							ok=false;
						}
						++p;
						if (!isdigit(*p)) {
							writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
							ok=false;
						}
						++p;
						if (*p) {	// Not just CCYYMMDD
							if (*p != '0' &&  *p != '1' &&  *p != '2') {
								writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
								ok=false;
							}
							++p;
							if (!isdigit(*p)) {
								writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
								ok=false;
							}
							++p;
							if (*p) {	// Not just CCYYMMDDHH
								if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3' &&  *p != '4' &&  *p != '5') {
									writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
									ok=false;
								}
								++p;
								if (!isdigit(*p)) {
									writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
									ok=false;
								}
								++p;
								if (*p) {	// Not just CCYYMMDDHHMM
									if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3' &&  *p != '4' &&  *p != '5') {
										writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
										ok=false;
									}
									++p;
									if (!isdigit(*p)) {
										writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
										ok=false;
									}
									++p;
									if (*p) {	// Not just CCYYMMDDHHMMSS
										if (*p == '.') {	// .FFFFFF
											while (*++p && isdigit(*p));
										}
										if (*p == '+' || *p == '-') {	// + or - ZZZZ
											while (*++p) {
												if (!isdigit(*p)) {
													writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
													ok=false;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				while (*p) {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
					++p;
				}
			}
		}
		else {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,l,">= 2 && <= 26");
			ok=false;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
DecimalStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 16) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 16");
			ok=false;
		}
		const char *p=s;
		if (*p == '+' ||  *p == '-') ++p;
		if (isdigit(*p)) while (isdigit(*++p));
		if (*p == '.') while (isdigit(*++p));
		if (*p == 'e' || *p == 'E') {
			++p;
			if (*p == '+' ||  *p == '-') ++p;
			if (isdigit(*p)) while (isdigit(*++p));
		}
		if (*p){
			writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
			ok=false;
		}

		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
IntegerStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 12) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 12");
			ok=false;
		}
		const char *p=s;
		if (*p == '+' ||  *p == '-') ++p;
		if (isdigit(*p)) while (isdigit(*++p));
		if (*p){
			writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
			ok=false;
		}

		// check in range of -(2^31-1) <= n <= (2^31-1)
		//                   -2147483647        2147483647

		double value = 0.0; 	// :)
		p=s;
		while (*p) {
			if (isdigit(*p)) value=value*10+(toascii(*p)-toascii('0'));
			++p;
		}

		// don't need to worry about sign because abs() max is same for both

		if (value > 2147483647) {
			writeErrorBadVRRange(log,dict,getTag(),getVR(),vn,s,"-(2^31-1) <= n <= (2^31-1)");
			ok=false;
		}

		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
LongStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 64) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 64");
			ok=false;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		const char *p=s;
		while (*p) {
			if ((iscntrl(*p) && !isescape(*p)) || *p == '\\') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
LongTextAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 10240) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 10240");
			ok=false;
		}
		const char *p=s;
		while (*p) {
			if (iscntrl(*p) && !iscntrlok(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
PersonNameAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 64) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 64");
			ok=false;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		int caretsTotal=0;
		int caretsInGroup=0;
		int equals=0;
		const char *p=s;
		while (*p) {
			if ((iscntrl(*p) && !isescape(*p)) || *p == '\\' || (isspace(*p) && *p != ' ')) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			else if (*p == '^') {
				++caretsTotal;
				++caretsInGroup;
//log << "Found ^ - incrementing - now equals = " << equals << ", caretsInGroup = " <<  caretsInGroup<< ", caretsTotal = " << caretsTotal << endl;
			}
			else if (*p == '=') {
				if (caretsInGroup > 4) {
					writeErrorBadVRValue(log,dict,getTag(),getVR(),vn,s);
					log << MMsgDC(TooManyComponentDelimitersInPersonName) << endl;
					ok=false;
				}
				caretsInGroup=0;
				++equals;
//log << "Found = - incrementing - now equals = " << equals << ", caretsInGroup = " <<  caretsInGroup<< ", caretsTotal = " << caretsTotal << endl;
			}
			++p;
		}
		if (caretsTotal == 0) {
			writeWarningVRValue(log,dict,getTag(),getVR(),vn,s);
			log << MMsgDC(RetiredPersonNameForm) << endl;
		}
		else if (caretsInGroup > 4) {
			writeErrorBadVRValue(log,dict,getTag(),getVR(),vn,s);
			log << MMsgDC(TooManyComponentDelimitersInPersonName) << endl;
			ok=false;
		}
		if (equals > 2) {
			log << MMsgDC(TooManyComponentGroupDelimitersInPersonName) << endl;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
ShortStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 16) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 16");
			ok=false;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		const char *p=s;
		while (*p) {
			if ((iscntrl(*p) && !isescape(*p)) || *p == '\\') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
ShortTextAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		if (strlen(s) > 1024) {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 1024");
			ok=false;
		}
		const char *p=s;
		while (*p) {
			if (iscntrl(*p) && !iscntrlok(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
TimeStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		int l=strlen(s);
		if (l >= 2 && l <= 16) {
			const char *p=s;
			if (*p != '0' &&  *p != '1' &&  *p != '2') {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (!isdigit(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
			if (*p) {	// Not just HH
				if (*p == ':') ++p;
				if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3' &&  *p != '4' &&  *p != '5') {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
				}
				++p;
				if (!isdigit(*p)) {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
				}
				++p;
				if (*p) {	// Not just HHMM
					if (*p == ':') ++p;
					if (*p != '0' &&  *p != '1' &&  *p != '2' &&  *p != '3' &&  *p != '4' &&  *p != '5') {
						writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
						ok=false;
					}
					++p;
					if (!isdigit(*p)) {
						writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
						ok=false;
					}
					++p;
					if (*p == '.') {	// .FFFFFF
						while (*++p) {
							if (!isdigit(*p)) {
								writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
								ok=false;
							}
						}
					}
					while (*p) {
						writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
						ok=false;
						++p;
					}
				}
			}
		}
		else {
			writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,l,">= 2 && <= 16");
			ok=false;
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

bool
UIStringAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		int length = strlen(s);
		if (length > 0) {
			if (length > 64) {
				writeErrorBadVRLengthNL(log,dict,getTag(),getVR(),vn,s,strlen(s),"<= 64");
				ok=false;
			}
			const char *p=s;
			int componentlength=0;
			int countleadingzeroes=0;
			bool foundnonzerodigitsincomponent=false;
			bool nothingbutzeroesinallcomponents=true;
			while (*p) {
				if (!isdigit(*p) && *p != '.') {
					writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
					ok=false;
				}

				if (*p == '0') {
					if (!foundnonzerodigitsincomponent) ++countleadingzeroes;
				}
				else {
					if (isdigit(*p)) {
						foundnonzerodigitsincomponent=true;
					}
					if (*p != '.') {
						nothingbutzeroesinallcomponents=false;
					}
				}

				componentlength = (*p == '.') ? 0 : (componentlength+1);

				++p;

				if (!*p || *p == '.') {
					if (componentlength == 0) {
						writeErrorBadVRValue(log,dict,getTag(),getVR(),vn,s);
						log << MMsgDC(EmptyComponent)
						    << endl;
						ok=false;
					}
					else if ((foundnonzerodigitsincomponent && countleadingzeroes > 0) || (!foundnonzerodigitsincomponent && countleadingzeroes > 1)) {
						writeErrorBadVRValue(log,dict,getTag(),getVR(),vn,s);
						log << MMsgDC(LeadingZeroes)
						    << endl;
						ok=false;
					}
					countleadingzeroes=0; foundnonzerodigitsincomponent=false;
				}
			}
			if (nothingbutzeroesinallcomponents) {
				writeErrorBadVRValue(log,dict,getTag(),getVR(),vn,s);
				log << MMsgDC(NothingButZeroComponents)
				    << endl;
				ok=false;
			}
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	// null is OK as trailing character, but trailing space is not
	if (trailingSpace) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),' ');
		ok=false;
	}
	return ok;
}


bool
UnlimitedTextAttribute::validateVR(TextOutputStream& log,SpecificCharacterSetInfo *specificCharacterSetInfo,ElementDictionary *dict) const
{
	bool ok=true;
	int vn=0;
	ValueListIterator<char *> i(values);
	while (!i) {
		char *s=i();
		const char *p=s;
		while (*p) {
			if (iscntrl(*p) && !iscntrlok(*p)) {
				writeErrorBadVRCharNL(log,dict,getTag(),getVR(),vn,s,*p);
				ok=false;
			}
			++p;
		}
		if (specificCharacterSetInfo) {
			int badCharacterPosition;
			if (!specificCharacterSetInfo->isValidString(s,badCharacterPosition)) {
				Assert(badCharacterPosition >= 0);
				writeErrorBadCharacterRepertoireCharNL(log,dict,getTag(),getVR(),vn,s,*(s+badCharacterPosition));
				ok=false;
			}
		}
		++vn; ++i;
		//if (s) delete[] s;
	}
	if (embeddedNullByte) {		// set during StringAttribute::read()
		writeErrorBadVRCharNL(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	if (trailingNullByte) {		// set during StringAttribute::read()
		writeErrorBadTrailingChar(log,dict,getTag(),getVR(),0);
		ok=false;
	}
	return ok;
}

