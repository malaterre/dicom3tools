#ifndef __Header_transyn__
#define __Header_transyn__

#include "misctype.h"
#include "transynu.h"

class TransferSyntax {
private:
	const char *		UID;
	const char *		Description;
	Endian 			endian;
	VRType 			VRtype;
	bool 			encapsulated;
	Endian 			pixelendian;
public:	
	TransferSyntax(const char *uid=DefaultTransferSyntaxUID);

	TransferSyntax(VRType vr,Endian e,bool encap=false);

	const char *	getUID(void) const		{ return UID; }
	const char *	getDescription(void) const	{ return Description; }
	Endian		getEndian(void) const		{ return endian; }
	VRType		getVR(void) const		{ return VRtype; }
	bool		getEncapsulated(void) const	{ return encapsulated; }
	Endian		getPixelEndian(void) const	{ return pixelendian; }

	bool	isLittleEndian(void) const	{ return endian == LittleEndian; }
	bool	isBigEndian(void) const		{ return endian == BigEndian; }

	bool	isPixelLittleEndian(void) const	{ return pixelendian == LittleEndian; }
	bool	isPixelBigEndian(void) const	{ return pixelendian == BigEndian; }

	bool	isImplicitVR(void) const	{ return VRtype == ImplicitVR; }
	bool	isExplicitVR(void) const	{ return VRtype == ExplicitVR; }

	bool	isEncapsulated(void) const	{ return encapsulated; }
	bool	isNotEncapsulated(void) const	{ return !encapsulated; }

	bool	isValid(void) const		{ return UID != 0; }

	bool operator==(const TransferSyntax& t2) const;
};

#endif /* __Header_transyn__ */
