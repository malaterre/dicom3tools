#include "attr.h"
#include "errclass.h"
#include "lutclass.h"
#include "mesgtext.h"

bool
DicomLUT::fillArray(Attribute *data)
{
	// current definitions in the standard make these LUTs OW all the time
	if (data->isOtherWordNonPixel()) {
		// retrieve entire OW array at once and copy into our array
		const Uint16 *values;
		Uint32 lengthinwords;
		if (!data->getValue(values,lengthinwords) || !values) {
			errorstream << EMsgDC(LUTDataBad)
				    << " - VR=\"" << data->getVR() << "\""
				    << " VM=" << data->getVM()
				    << " VL=" << data->getVL()
				    << endl;
			good_flag=false;
			return false;
		}
		if (lengthinwords != nentries-firstindexvalue) {
			errorstream << EMsgDC(LUTDataWrongLength)
				    << " - length in words=" << lengthinwords
				    << " but n entries wanted=" << nentries
				    << " and first index value=" << firstindexvalue
				    << endl;
			good_flag=false;
			return false;
		}
		Uint32 i,j;
		for (i=firstindexvalue,j=0; i<nentries; ++i) {
			array[i]=values[j];
			if (i >= firstindexvalue) ++j;
		}

		return true;
	}
	
	// leave old one-value-at-a-time US approach in case old explicit VR data encountered ...
	
	if (!data->isNumeric()) {
		errorstream << EMsgDC(LUTDataNotNumeric)
			    << " - \"" << data->getVR() << "\"" << endl;
		good_flag=false;
		return false;
	}
	if (data->getVM() != nentries) {
		errorstream << EMsgDC(LUTDataWrongLength)
			    << " - VR=\"" << data->getVR() << "\""
			    << " VM=" << data->getVM()
			    << " VL=" << data->getVL()
			    << " needed=" << nentries
			    << endl;
		good_flag=false;
		return false;
	}

	Uint32 i,j;
	for (i=firstindexvalue,j=0; i<nentries; ++i) {
		Uint16 lutvalue;
		if (!data->getValue(Uint16(j),lutvalue)) {
			errorstream << EMsgDC(LUTDataBad)
				    << " - VR=\"" << data->getVR() << "\""
				    << " VM=" << data->getVM()
				    << " VL=" << data->getVL()
				    << endl;
			good_flag=false;
			return false;
		}
		array[i]=lutvalue;
		if (i >= firstindexvalue) ++j;
	}
		
	return true;
}

DicomLUT::DicomLUT(Attribute *desc,Attribute*data)
		{
			Assert(desc);
			Assert(data);

			array=0;
			nentries=0;
			firstindexvalue=0;
			bitsallocated=0;

			if (!desc->isNumeric()) {
				errorstream << EMsgDC(LUTDescriptorNotNumeric)
					    << " - \"" << desc->getVR() << "\"" << endl;
				good_flag=false;
				return;
			}
			if (desc->getVM() >= 3 && desc->getVM() <= 4) {
				if (!desc->getValue(Uint16(0),nentries)
				 || !desc->getValue(1,firstindexvalue)
				 || !desc->getValue(2,bitsallocated)) {
					errorstream << EMsgDC(LUTDescriptorBad)
						    << " - \"" << desc->getVR() << "\"" << endl;
					good_flag=false;
					return;
				}
				if (nentries == 0) nentries=0x10000;	// as per Supplement 5 US

				if (nentries) {
					array=new Uint16[nentries];
					Assert(array);
					if (!fillArray(data)) return;
				}
			}
			else {
				errorstream << EMsgDC(LUTDescriptorWrongVM)
					    << " - " << desc->getVM() << endl;
				good_flag=false;
				return;
			}

			if (desc->getVM() == 4) {
				// who knows what this was for ?
			}
//cerr << "DicomLUT::DicomLUT - dumping ..." << endl;
//{
//Uint32 i;
//i=0;
//while (i < nentries) { cerr << "[" << dec << i << "]=" << hex << array[i] << dec << endl; ++i; }
//}
		}

DicomLUT::~DicomLUT()
{
	if (array) delete[] array;
}

NormalDicomLUT::NormalDicomLUT(Attribute *desc,Attribute*data)
	: DicomLUT(desc,data)
{}

NormalDicomLUT::~NormalDicomLUT()
{}

LargeDicomLUT::LargeDicomLUT(Attribute *desc,Attribute*data)
	: DicomLUT(desc,data)
{}

LargeDicomLUT::~LargeDicomLUT()
{}

SegmentedDicomLUT::SegmentedDicomLUT(Attribute *desc,Attribute*data)
	: DicomLUT(desc,data)
{ Assert(0); }

SegmentedDicomLUT::~SegmentedDicomLUT()
{}
