#include "attr.h"
#include "attrlist.h"
#include "errclass.h"
#include "lutclass.h"
#include "lutextr.h"
#include "elmconst.h"
#include "mesgtext.h"

#define GENERATEEXTRACTNEWLOOKUPTABLE(color,desctype,datatype,luttype) \
	a##color##LUTData=list[TagFromName(datatype##color##PaletteColorLookupTableData)]; \
	if (a##color##LUTData) { \
		if (color##LUT) { \
			log << EMsgDC(LUTDataMultiplyDefined) \
			    << " - \"" << #datatype << #color << "PaletteColorLookupTableData\"" \
			    << endl; \
			return false; \
		} \
		else { \
			a##color##LUTDescriptor=list[TagFromName(desctype##color##PaletteColorLookupTableDescriptor)]; \
			if (a##color##LUTData) \
				color##LUT=new luttype##DicomLUT(a##color##LUTDescriptor,a##color##LUTData); \
			else { \
				log << EMsgDC(LUTDataWithoutDescriptor) \
				    << " - \"" << #datatype << #color << "PaletteColorLookupTableData\"" \
				    << " needs \"" << #desctype << #color << "PaletteColorLookupTableDescriptor\"" \
				    << endl; \
				return false; \
			} \
		} \
	}

bool
extractLookUpTables(TextOutputStream &log,AttributeList& list,
	DicomLUT *&RedLUT,
	DicomLUT *&GreenLUT,
	DicomLUT *&BlueLUT)
{
//cerr << "extractLookUpTables: start" << endl;
	Attribute *aRedLUTDescriptor = 0;
	Attribute *aGreenLUTDescriptor = 0;
	Attribute *aBlueLUTDescriptor = 0;
	Attribute *aRedLUTData = 0;
	Attribute *aGreenLUTData = 0;
	Attribute *aBlueLUTData = 0;

	RedLUT = 0;
	GreenLUT = 0;
	BlueLUT = 0;

	GENERATEEXTRACTNEWLOOKUPTABLE(Red,,,Normal);
	GENERATEEXTRACTNEWLOOKUPTABLE(Red,Large,Large,Large);
	GENERATEEXTRACTNEWLOOKUPTABLE(Red,,Segmented,Segmented);
	GENERATEEXTRACTNEWLOOKUPTABLE(Green,,,Normal);
	GENERATEEXTRACTNEWLOOKUPTABLE(Green,Large,Large,Large);
	GENERATEEXTRACTNEWLOOKUPTABLE(Green,,Segmented,Segmented);
	GENERATEEXTRACTNEWLOOKUPTABLE(Blue,,,Normal);
	GENERATEEXTRACTNEWLOOKUPTABLE(Blue,Large,Large,Large);
	GENERATEEXTRACTNEWLOOKUPTABLE(Blue,,Segmented,Segmented);

	if (!RedLUT || !GreenLUT || !BlueLUT) {
		log << EMsgDC(MissingLUTs) << endl;
		return false;
	}
	if (!RedLUT->good() || !GreenLUT->good() || !BlueLUT->good()) {
		log << RedLUT->errors();
		log << GreenLUT->errors();
		log << BlueLUT->errors();
		return false;
	}

//cerr << "extractLookUpTables: success" << endl;
	return true;
}

