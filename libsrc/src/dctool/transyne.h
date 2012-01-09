#ifndef __Header_transyne__
#define __Header_transyne__

struct TransferSyntaxDictionaryEntry {
	const char *	UID;
	const char *	Description;
	Endian 		endian;
	VRType 		VRtype;
	bool 		encapsulated;
	Endian 		pixelendian;
};

#endif /* __Header_transyne__ */
