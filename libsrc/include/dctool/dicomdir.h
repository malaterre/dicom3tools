#ifndef __Header_dicomdir__
#define __Header_dicomdir__

bool parseDicomdir(ManagedAttributeList &list,
	TextOutputStream &log,bool verbose,bool veryverbose,bool showrecordinfo,bool showabstract,bool showpaths,bool showdescription);

#endif /* __Header_dicomdir__ */


