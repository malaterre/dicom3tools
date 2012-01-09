#ifndef __Header_ie__
#define __Header_ie__

enum InformationEntity {
	UnknownIE,
	DirectoryIE,
	DocumentIE,
	EncapsulatedDocumentIE,
	EquipmentIE,
	FileIE,
	FrameOfReferenceIE,
	HangingProtocolIE,
	ImageIE,
	MRSpectroscopyIE,
	PatientIE,
	PlanIE,
	PresentationIE,
	RawDataIE,
	RealWorldValueMappingIE,
	SeriesIE,
	SpatialFiducialsIE,
	SpatialRegistrationIE,
	StructureSetIE,
	StudyIE,
	TreatmentRecordIE,
	WaveformIE,
	OverlayIE,
	CurveIE,
	ModalityLUTIE,
	VOILUTIE,
	ColorPaletteIE
};

InformationEntity getInformationEntityFromDescription(const char *d);
const char *describeInformationEntity(InformationEntity ie);

#endif // __Header_ie__
