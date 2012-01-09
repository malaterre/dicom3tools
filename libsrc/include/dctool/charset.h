// public interface for SpecificCharacterSetInfo class

#ifndef __Header_charset__
#define __Header_charset__

class Attribute;

class SpecificCharacterSetInfo {
private:
	bool flag7BitSingleByte;
	bool flag8BitSingleByte;
	bool flagISO2022;
	bool flagUTF8;
public:
	SpecificCharacterSetInfo(void);
	SpecificCharacterSetInfo(int nCharSetValues,char**charSetValues);
	SpecificCharacterSetInfo(Attribute *aSpecificCharacterSet);
	~SpecificCharacterSetInfo();

	void setSpecificCharacterSet();
	void setSpecificCharacterSet(int nCharSetValues,char**charSetValues);

	bool is7BitSingleByte() const { return flag7BitSingleByte; }
	bool is8BitSingleByt() const { return flag8BitSingleByte; }
	bool isISO2022() const { return flagISO2022; }
	bool isUTF8() const { return flagUTF8; }
	
	bool isValidString(const char *s,int& badCharacterPosition) const;
};

#endif // __Header_charset__
