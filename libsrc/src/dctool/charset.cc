#include <string.h>
//#include <iostream.h>	// for cerr
//#include <iomanip.h>	// for cerr

#include "charset.h"
#include "attr.h"

SpecificCharacterSetInfo::SpecificCharacterSetInfo(void) {
	setSpecificCharacterSet();
}

SpecificCharacterSetInfo::SpecificCharacterSetInfo(int nCharSetValues,char** charSetValues) {
	setSpecificCharacterSet(nCharSetValues,charSetValues);
}

SpecificCharacterSetInfo::SpecificCharacterSetInfo(Attribute *aSpecificCharacterSet) {
	if (!aSpecificCharacterSet || aSpecificCharacterSet->getVL() == 0) {
		setSpecificCharacterSet();
	}
	else {
		int nCharSetValues =  aSpecificCharacterSet->getVM();
		Assert(nCharSetValues > 0);
		char **charSetValues = new char *[nCharSetValues];
		for (int i=0; i<nCharSetValues; ++i) {
			aSpecificCharacterSet->getValue(i,charSetValues[i]);
		}
		setSpecificCharacterSet(nCharSetValues,charSetValues);
	}
}

SpecificCharacterSetInfo::~SpecificCharacterSetInfo() {
}

void
SpecificCharacterSetInfo::setSpecificCharacterSet(void) {
	flag7BitSingleByte = true;		// default character repertoire
	flag8BitSingleByte = false;
	flagISO2022 = false;
	flagUTF8 = false;
}

void
SpecificCharacterSetInfo::setSpecificCharacterSet(int nCharSetValues,char** charSetValues) {
	flag7BitSingleByte = true;
	flag8BitSingleByte = false;
	flagISO2022 = false;
	flagUTF8 = false;
	if (nCharSetValues > 0 && charSetValues) {
		if (nCharSetValues == 1) {
			const char *value = charSetValues[0];
			if (value) {
				if (strcmp(value,"ISO_IR 100") == 0
				 || strcmp(value,"ISO_IR 101") == 0
				 || strcmp(value,"ISO_IR 109") == 0
				 || strcmp(value,"ISO_IR 110") == 0
				 || strcmp(value,"ISO_IR 144") == 0
				 || strcmp(value,"ISO_IR 127") == 0
				 || strcmp(value,"ISO_IR 126") == 0
				 || strcmp(value,"ISO_IR 138") == 0
				 || strcmp(value,"ISO_IR 148") == 0
				 || strcmp(value,"ISO_IR 166") == 0
				) {
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): found single byte ISO" << endl;
					flag7BitSingleByte = false;
					flag8BitSingleByte = true;
				}
				else if (strcmp(value,"ISO_IR 192") == 0 || strcmp(value,"GB18030") == 0
				) {
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): found ISO_IR 192" << endl;
					flag7BitSingleByte = false;
					flagUTF8 = true;
				}
			}
		}
		else {
			flag7BitSingleByte = false;
			flagISO2022 = true;
		}
	}
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): flag7BitSingleByte = " << flag7BitSingleByte << endl;
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): flag8BitSingleByte = " << flag8BitSingleByte << endl;
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): flagISO2022 = " << flagISO2022 << endl;
//cerr << "SpecificCharacterSetInfo::setSpecificCharacterSet(): flagUTF8 = " << flagUTF8 << endl;
}

bool
SpecificCharacterSetInfo::isValidString(const char *s,int& badCharacterPosition) const {
//cerr << "SpecificCharacterSetInfo::isValidString(): checking string = <" << s << ">" << endl;
	bool success = true;
	const char *p=s;
	while (*p) {
		unsigned char c = unsigned(*p)&0xff;
		if (flag7BitSingleByte) {
			if ((c&0x80) != 0) {
//cerr << "SpecificCharacterSetInfo::isValidString(): flag7BitSingleByte - character has high bit set '" << c << "' (" << hex << c << dec << ")" << endl;
				success = false;
				break;
			}
		}
		else if (flag8BitSingleByte || flagISO2022) {
			// CL	bytes from 00/00 to 01/15
			// GL	bytes from 02/00 to 07/15
			// CR	bytes from 08/00 to 09/15 ... never used
			// GR	bytes from 10/00 to 15/15
			if (c >= 0x80 && c <= 0x9F) {
//cerr << "SpecificCharacterSetInfo::isValidString(): flag8BitSingleByte || flagISO2022 - illegal CR control character '" << c << "' (" << hex << c << dec << ")" << endl;
				success = false;
				break;
			}
		}
		// else if UTF8 assume anything is OK
		++p;
	}
	if (success) {
		badCharacterPosition = -1;
	}
	else {
		badCharacterPosition = p-s;
	}
//cerr << "SpecificCharacterSetInfo::isValidString(): returning " << success << endl;
	return success;
}


