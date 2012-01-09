#include <iostream.h>

#include "transyn.h"
#include "transynd.h"

static TransferSyntaxDictionary tsd;

TransferSyntax::TransferSyntax(const char *uid)
{
	Assert(uid);
	const TransferSyntaxDictionaryEntry *e = tsd[uid];
	if (e) {
		UID=e->UID;
		Description=e->Description;
		endian=e->endian;
		VRtype=e->VRtype;
		encapsulated=e->encapsulated;
		pixelendian=e->pixelendian;
	}
	else {
		// Unrecognized transfer syntax, so assume EVRLE encapsulated, since that is what most new standard ones are
		// Could attempt to "guess" using same logic as in dcstream.cc, but late at this point
		UID=uid;
		Description="Unrecognized transfer syntax";
		endian=LittleEndian;
		VRtype=ExplicitVR;
		encapsulated=false;
		pixelendian=LittleEndian;
	}
}

TransferSyntax::TransferSyntax(VRType vr,Endian e,bool encap)
{
	VRtype=vr;
	endian=e;
	pixelendian=e;
	encapsulated=encap;
	Description=0;
	UID=0;
}

bool
TransferSyntax::operator==(const TransferSyntax& t2) const
{
//cerr << "TransferSyntax::operator==(const TransferSyntax& t2):" << endl;
	if ((!getEncapsulated() && !t2.getEncapsulated())
	  || !getUID() || !t2.getUID()) {
//cerr << "TransferSyntax::operator== comparing components:" << endl;
		return getEndian() == t2.getEndian()
		    && getVR() == t2.getVR()
		    && getPixelEndian() == t2.getPixelEndian()
		    && !getEncapsulated()
		    && !t2.getEncapsulated();
	}
	else {
//cerr << "TransferSyntax::operator== comparing UID strings:" << endl;
//cerr << "TransferSyntax::operator== string1 <" << getUID() << ">" << endl;
//cerr << "TransferSyntax::operator== string2 <" << t2.getUID() << ">" << endl;
		return strcmp(getUID(),t2.getUID()) == 0;
	}
}
