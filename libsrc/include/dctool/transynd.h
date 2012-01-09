#ifndef __Header_transynd__
#define __Header_transynd__

void
dumpTransferSyntaxUID(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	if (ts->getUID())
		cerr << prefix << "UID\t\t"
		     << ts->getUID() << endl;
}

void
dumpTransferSyntaxDescription(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	if (ts->getDescription())
		cerr << prefix << "Description\t\""
		     << ts->getDescription() << "\"" << endl;
}

void
dumpTransferSyntaxByteOrder(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	cerr << prefix << "ByteOrder\t";
	switch (ts->getEndian()) {
		case NoEndian:		cerr << "None"; break;
		case LittleEndian:	cerr << "Little"; break;
		case BigEndian:		cerr << "Big"; break;
		case ByteEndian:	cerr << "Byte"; break;
		default:		cerr << "**unrecognized**"; break;
	}
	cerr << endl;

}

void
dumpTransferSyntaxPixelByteOrder(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	cerr << prefix << "PixelByteOrder\t";
	switch (ts->getPixelEndian()) {
		case NoEndian:		cerr << "None"; break;
		case LittleEndian:	cerr << "Little"; break;
		case BigEndian:		cerr << "Big"; break;
		case ByteEndian:	cerr << "Byte"; break;
		default:		cerr << "**unrecognized**"; break;
	}
	cerr << endl;

}

void
dumpTransferSyntaxVR(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	cerr << prefix << "VR\t\t";
	switch (ts->getVR()) {
		case NoVR:		cerr << "None"; break;
		case ImplicitVR:	cerr << "Implicit"; break;
		case ExplicitVR:	cerr << "Explicit"; break;
		default:		cerr << "**unrecognized**"; break;
	}
	cerr << endl;
}

void
dumpTransferSyntaxEncapsulation(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	cerr << prefix << "Encapsulated\t"
	     << (ts->isEncapsulated() ? "Yes" : "No")
	     << endl;
}


void
dumpTransferSyntax(TransferSyntax *ts,const char *prefix=0)
{
	Assert(ts);
	if (!prefix) prefix="";

	dumpTransferSyntaxUID(ts,prefix);
	dumpTransferSyntaxDescription(ts,prefix);
	dumpTransferSyntaxByteOrder(ts,prefix);
	dumpTransferSyntaxVR(ts,prefix);
	dumpTransferSyntaxEncapsulation(ts,prefix);

}

#endif /* __Header_transynd__ */
