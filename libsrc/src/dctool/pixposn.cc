#include "attr.h"
#include "attrlist.h"
#include "elmdict.h"
#include "attrval.h"
#include "elmconst.h"
#include "mesgtext.h"

#include "pixposn.h"

PositionOfPixel::PositionOfPixel(AttributeList &list,TextOutputStream &log,bool verbose)
{
	// if ! verbose and no relevant attributes at all, then doesn't complain

	logp=&log;

	havePixelSpacing=false;
	aPixelSpacing = list[TagFromName(PixelSpacing)];
	if (aPixelSpacing) {
		switch(aPixelSpacing->getVM()) {
		case 1:
			(*logp) << WMsgDC(OnlyOnePixelSpacingValue)
		 	    << endl;
			aPixelSpacing->getValue(0,vPixelSpacingRow);
			vPixelSpacingCol=vPixelSpacingRow;
//(*logp) << "vPixelSpacing both=" << vPixelSpacingCol << endl;
			havePixelSpacing=true;
			break;
		case 2:
			aPixelSpacing->getValue(0,vPixelSpacingRow);
			aPixelSpacing->getValue(1,vPixelSpacingCol);
//(*logp) << "vPixelSpacingRow=" << vPixelSpacingRow << endl;
//(*logp) << "vPixelSpacingCol=" << vPixelSpacingCol << endl;
			havePixelSpacing=true;
			break;
		default:
			(*logp) << EMsgDC(BadAttributeValueMultiplicity)
			    << " - \"PixelSpacing\""
			    << endl;
			break;
		}
	}

	haveImagePositionPatient=false;
	aImagePositionPatient = list[TagFromName(ImagePositionPatient)];
	if (aImagePositionPatient) {
		if (aImagePositionPatient->getVM() == 3) {
			aImagePositionPatient->getValue(0,vImagePositionPatientX);
			aImagePositionPatient->getValue(1,vImagePositionPatientY);
			aImagePositionPatient->getValue(2,vImagePositionPatientZ);
			haveImagePositionPatient=true;
//(*logp) << "vImagePositionPatientX=" << vImagePositionPatientX << endl;
//(*logp) << "vImagePositionPatientY=" << vImagePositionPatientY << endl;
//(*logp) << "vImagePositionPatientZ=" << vImagePositionPatientZ << endl;
		}
		else {
			(*logp) << EMsgDC(BadAttributeValueMultiplicity)
			    << " - \"ImagePositionPatient\""
			    << endl;
		}
	}

	haveImageOrientationPatient=false;
	aImageOrientationPatient = list[TagFromName(ImageOrientationPatient)];
	if (aImageOrientationPatient) {
		if (aImageOrientationPatient->getVM() == 6) {
			aImageOrientationPatient->getValue(0,vImageOrientationPatientRowX);
			aImageOrientationPatient->getValue(1,vImageOrientationPatientRowY);
			aImageOrientationPatient->getValue(2,vImageOrientationPatientRowZ);
			aImageOrientationPatient->getValue(3,vImageOrientationPatientColX);
			aImageOrientationPatient->getValue(4,vImageOrientationPatientColY);
			aImageOrientationPatient->getValue(5,vImageOrientationPatientColZ);
//(*logp) << "vImageOrientationPatientRowX=" << vImageOrientationPatientRowX << endl;
//(*logp) << "vImageOrientationPatientRowY=" << vImageOrientationPatientRowY << endl;
//(*logp) << "vImageOrientationPatientRowZ=" << vImageOrientationPatientRowZ << endl;
//(*logp) << "vImageOrientationPatientColX=" << vImageOrientationPatientColX << endl;
//(*logp) << "vImageOrientationPatientColY=" << vImageOrientationPatientColY << endl;
//(*logp) << "vImageOrientationPatientColZ=" << vImageOrientationPatientColZ << endl;
			haveImageOrientationPatient=true;
		}
		else {
			(*logp) << EMsgDC(BadAttributeValueMultiplicity)
			    << " - \"ImageOrientationPatient\""
			    << endl;
		}
	}

	if (!(haveImageOrientationPatient && haveImagePositionPatient && havePixelSpacing)) {
		// Report limited subset of missing attributes ...
		if (verbose || aImagePositionPatient || aImageOrientationPatient) {
			(*logp) << EMsgDC(InsufficientInformationToCalculatePosition) << endl;
			if (!aPixelSpacing)
				(*logp) << EMsgDC(MissingAttribute) << " - \"PixelSpacing\"" << endl;
			if (!aImagePositionPatient)
				(*logp) << EMsgDC(MissingAttribute) << " - \"ImagePositionPatient\"" << endl;
			if (!aImageOrientationPatient)
				(*logp) << EMsgDC(MissingAttribute) << " - \"ImageOrientationPatient\"" << endl;
		}
	}
}

bool
PositionOfPixel::good()
{
	return haveImageOrientationPatient && haveImagePositionPatient && havePixelSpacing;
}

bool
PositionOfPixel::getPosition(Uint16 row,Uint16 col,Float64 &x,Float64 &y,Float64 &z)
{
	if (haveImageOrientationPatient && haveImagePositionPatient && havePixelSpacing) {
		// row/col are pixels from TLHC along rows/cols respectively
		// PixelSpacing Row/Col is space BETWEEN rows/cols respectively
		// ImageOrientationPatient Row/Col are unit vectors ALONG rows/cols respectively
		// - hence row pixels of size PixelSpacingRow along ImageOrientationPatientCol

		// Add 0.5 because assume position is that of center of pixel

		x=vImagePositionPatientX
		 +vImageOrientationPatientColX*(row+0.5)*vPixelSpacingRow
		 +vImageOrientationPatientRowX*(col+0.5)*vPixelSpacingCol;
		y=vImagePositionPatientY
		 +vImageOrientationPatientColY*(row+0.5)*vPixelSpacingRow
		 +vImageOrientationPatientRowY*(col+0.5)*vPixelSpacingCol;
		z=vImagePositionPatientZ
		 +vImageOrientationPatientColZ*(row+0.5)*vPixelSpacingRow
		 +vImageOrientationPatientRowZ*(col+0.5)*vPixelSpacingCol;

		return true;	
	}
	else {
		// make return values ridiculous in case anyone uses them
		x=-99999999.99999;
		y=-99999999.99999;
		z=-99999999.99999;
		return false;
	}
}

