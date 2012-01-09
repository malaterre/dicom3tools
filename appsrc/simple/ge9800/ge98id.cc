#include <iostream.h>

#include "basetype.h"

// The following includes and definitions mirror libsrc/include/dctool/fltype.h ...

#include <float.h>

#ifdef POWINTEGEREXPONENTTYPE
#define	powi(m,e)	pow(m,(POWINTEGEREXPONENTTYPE)(e))
#else
#define	powi(m,e)	pow(m,(long)(e))
#endif

static Uint16
read16(istream& instream)
{
	Uint16 u;
	unsigned char buffer[2];
	instream.read((char *)buffer,2);
	// DataGeneral is a Big Endian machine
	u =  (Uint16)buffer[0];
	u <<= 8;
	u |= (Uint16)buffer[1];
	return u;
}

static double
readReal(istream& instream)
{
	double value;
	typedef struct {
		unsigned	sign : 1;
		unsigned	exponent : 7;
		unsigned	mantissa : 24;
	} DG_FLOAT;
	
	DG_FLOAT number;

	//unsigned char buffer[4];
	char buffer[4];
	instream.read(buffer,4);
	// DataGeneral is a Big Endian machine
	memcpy ((char *)(&number),buffer,4);

	unsigned char	sign = number.sign;
	Uint16		exponent = number.exponent;
	Uint32		mantissa = number.mantissa;

	value = (double) mantissa / (1 << 24) * powi (16.0, (long)(exponent) - 64);
	value = (sign == 0) ? value : -value;
	return value;
}

static OurStreamPos
seekword(istream& instream,unsigned block,unsigned word)
{
	// 256 word blocks,16 bit words ...
	const unsigned offset = (block*256+word)*2;
	instream.seekg(offset,ios::beg);
	return instream.tellg();
}

int
main(int argc,char *argv[])
{
	if (argc != 1) {
		cerr << "Usage: " << argv[0] << " < infile\n" << flush;
		return 1;
	}

	(void)seekword(cin,0,34);
	unsigned examheaderblock=((unsigned)read16(cin)-1)/256;
	(void)seekword(cin,0,35);
	unsigned imageheaderblock=((unsigned)read16(cin)-1)/256;

#define doascii(b,o,s,n)				\
	{						\
		char tmp[s+1];				\
		(void)seekword(cin,b,o);		\
		cin.read(tmp,s);			\
		tmp[s]=0;				\
		cout << n << "=\"" << tmp << "\"\n";	\
	}

#define doword(b,o,n)					\
	{						\
		(void)seekword(cin,b,o);		\
		unsigned tmp=read16(cin);		\
		cout << n << "=\"" << tmp << "\"\n";	\
	}

#define doreal(b,o,n)					\
	{						\
		(void)seekword(cin,b,o);		\
		double tmp=readReal(cin);		\
		cout << n << "=\"" << tmp << "\"\n";	\
	}

	doword (examheaderblock ,3    ,"StudyID");		// Exam Number
	doascii(examheaderblock ,11,12,"PatientID");		// Patient ID
	doascii(examheaderblock ,17,30,"PatientName");		// Patient Name
	doascii(examheaderblock ,47,30,"InstitutionName");	// Hospital Name
	doascii(examheaderblock ,76,30,"StudyDescription");	// Exam Description

	doword (imageheaderblock,10   ,"SeriesNumber");		// Position (study) number
	doword (imageheaderblock,46   ,"AcquisitionNumber");	// Scan Number
	doword (imageheaderblock,47   ,"ImageNumber");		// Image Number

	doreal (imageheaderblock,88   ,"SliceThickness");	// Aperture (1.5,3,5,10) (fp)
	doword (imageheaderblock,90   ,"KVP");				// Generator KVP (80,120,140)
	doword (imageheaderblock,91   ,"TubeCurrent");		// Tube current (10-360)
	doreal (imageheaderblock,92   ,"GantryAngle");		// Gantry Angle 
	doreal (imageheaderblock,94   ,"TableHeight");		// Table Height (mm) (fp)
	doreal (imageheaderblock,96   ,"SlicePosition");	// Axial Table Location (fp)
	doreal (imageheaderblock,98   ,"ScanTime");			// Scan time (1.4-9.4) (fp) 
	doreal (imageheaderblock,215  ,"ReconPixelSize");	// actual image pixel size (mm,fp) (scout), standard pixel size/target factor (recon) 

	return 0;
}
