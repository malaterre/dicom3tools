#include "basetype.h"
#include "ie.h"

InformationEntity
getInformationEntityFromDescription(const char *d) {
	InformationEntity ie = UnknownIE;
	if (d) {
		if      (strcmp(d,"Directory") == 0)				ie = DirectoryIE;
		else if (strcmp(d,"Document") == 0)					ie = DocumentIE;
		else if (strcmp(d,"EncapsulatedDocument") == 0)		ie = EncapsulatedDocumentIE;
		else if (strcmp(d,"Equipment") == 0)				ie = EquipmentIE;
		else if (strcmp(d,"File") == 0)						ie = FileIE;
		else if (strcmp(d,"FrameOfReference") == 0)			ie = FrameOfReferenceIE;
		else if (strcmp(d,"HangingProtocol") == 0)			ie = HangingProtocolIE;
		else if (strcmp(d,"Image") == 0)					ie = ImageIE;
		else if (strcmp(d,"MRSpectroscopy") == 0)			ie = MRSpectroscopyIE;
		else if (strcmp(d,"Patient") == 0)					ie = PatientIE;
		else if (strcmp(d,"Plan") == 0)						ie = PlanIE;
		else if (strcmp(d,"Presentation") == 0)				ie = PresentationIE;
		else if (strcmp(d,"RawData") == 0)					ie = RawDataIE;
		else if (strcmp(d,"RealWorldValueMapping") == 0)	ie = RealWorldValueMappingIE;
		else if (strcmp(d,"Series") == 0)					ie = SeriesIE;
		else if (strcmp(d,"SpatialFiducials") == 0)			ie = SpatialFiducialsIE;
		else if (strcmp(d,"SpatialRegistration") == 0)		ie = SpatialRegistrationIE;
		else if (strcmp(d,"StructureSet") == 0)				ie = StructureSetIE;
		else if (strcmp(d,"Study") == 0)					ie = StudyIE;
		else if (strcmp(d,"TreatmentRecord") == 0)			ie = TreatmentRecordIE;
		else if (strcmp(d,"Waveform") == 0)					ie = WaveformIE;
	}
	return ie;
}

const char *
describeInformationEntity(InformationEntity ie) {
	const char *d = "Unknown";
	if      (ie == DirectoryIE)				d = "Directory";
	else if (ie == DocumentIE)				d = "Document";
	else if (ie == EncapsulatedDocumentIE)	d = "EncapsulatedDocument";
	else if (ie == EquipmentIE)				d = "Equipment";
	else if (ie == FileIE)					d = "File";
	else if (ie == FrameOfReferenceIE)		d = "FrameOfReference";
	else if (ie == HangingProtocolIE)		d = "HangingProtocol";
	else if (ie == ImageIE)					d = "Image";
	else if (ie == MRSpectroscopyIE)		d = "MRSpectroscopy";
	else if (ie == PatientIE)				d = "Patient";
	else if (ie == PlanIE)					d = "Plan";
	else if (ie == PresentationIE)			d = "Presentation";
	else if (ie == RawDataIE)				d = "RawData";
	else if (ie == RealWorldValueMappingIE)	d = "RealWorldValueMapping";
	else if (ie == SeriesIE)				d = "Series";
	else if (ie == SpatialFiducialsIE)		d = "SpatialFiducials";
	else if (ie == SpatialRegistrationIE)	d = "SpatialRegistration";
	else if (ie == StructureSetIE)			d = "StructureSet";
	else if (ie == StudyIE)					d = "Study";
	else if (ie == TreatmentRecordIE)		d = "TreatmentRecord";
	else if (ie == WaveformIE)				d = "Waveform";
	return d;
}


