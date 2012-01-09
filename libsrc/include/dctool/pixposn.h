#ifndef __Header_pixposn__
#define __Header_pixposn__

class PositionOfPixel {
	TextOutputStream *logp;

	bool havePixelSpacing;
	Attribute *aPixelSpacing;
	Float64 vPixelSpacingRow;
	Float64 vPixelSpacingCol;

	bool haveImagePositionPatient;
	Attribute *aImagePositionPatient;
	Float64 vImagePositionPatientX;
	Float64 vImagePositionPatientY;
	Float64 vImagePositionPatientZ;

	bool haveImageOrientationPatient;
	Float64 vImageOrientationPatientRowX;
	Float64 vImageOrientationPatientRowY;
	Float64 vImageOrientationPatientRowZ;
	Float64 vImageOrientationPatientColX;
	Float64 vImageOrientationPatientColY;
	Float64 vImageOrientationPatientColZ;
	Attribute *aImageOrientationPatient;

public:
	PositionOfPixel(AttributeList &list,TextOutputStream &log,
		bool verbose=false);

	bool good();
	bool getPosition(Uint16 row,Uint16 col,Float64 &x,Float64 &y,Float64 &z);
};

#endif /* __Header_pixposn__ */
