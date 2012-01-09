#ifndef __Header_uidgen__
#define __Header_uidgen__

class GeneratedUID {
	ostrstream ost;
	char *string;
	const char *stamp;
protected:
	void	setSOPInstance();
	void	setStudyInstance();
	void	setSeriesInstance();
	void	setFrameOfReference();
	void	setSynchronizationFrameOfReference();
	void	setDirInstance();
	void	setDimensionOrganization();
	void	setConcatenation();
	void	setIrradiationEvent();
	void	setRawData();
	void	setUnsigned(unsigned u);
public:
	GeneratedUID(void);
	GeneratedUID(const char *s);
	~GeneratedUID();
	operator char *();
};

class GeneratedSOPInstanceUID : public GeneratedUID {
public:
	GeneratedSOPInstanceUID(void)
		: GeneratedUID()
		{
			setSOPInstance();
		}
	GeneratedSOPInstanceUID(const char *s)
		: GeneratedUID(s)
		{
			setSOPInstance();
		}
	GeneratedSOPInstanceUID
			(unsigned study,unsigned series,unsigned instance,int inConcatenationNumber)
		: GeneratedUID()
		{
			setSOPInstance();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
			if (inConcatenationNumber != -1) setUnsigned(inConcatenationNumber);
		}
	GeneratedSOPInstanceUID
			(const char *s,unsigned study,unsigned series,unsigned instance,int inConcatenationNumber)
		: GeneratedUID(s)
		{
			setSOPInstance();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
			if (inConcatenationNumber != -1) setUnsigned(inConcatenationNumber);
		}
};

class GeneratedStudyInstanceUID : public GeneratedUID {
public:
	GeneratedStudyInstanceUID(unsigned study)
		: GeneratedUID()
		{
			setStudyInstance();
			setUnsigned(study);
		}
	GeneratedStudyInstanceUID(const char *s,unsigned study)
		: GeneratedUID(s)
		{
			setStudyInstance();
			setUnsigned(study);
		}
};

class GeneratedSeriesInstanceUID : public GeneratedUID {
public:
	GeneratedSeriesInstanceUID(unsigned study,unsigned series)
		: GeneratedUID()
		{
			setSeriesInstance();
			setUnsigned(study);
			setUnsigned(series);
		}
	GeneratedSeriesInstanceUID(const char *s,unsigned study,
					unsigned series)
		: GeneratedUID(s)
		{
			setSeriesInstance();
			setUnsigned(study);
			setUnsigned(series);
		}
};

class GeneratedFrameOfReferenceUID : public GeneratedUID {
public:
	GeneratedFrameOfReferenceUID(void)
		: GeneratedUID()
		{
			setFrameOfReference();
		}
	GeneratedFrameOfReferenceUID(const char *s)
		: GeneratedUID(s)
		{
			setFrameOfReference();
		}
	GeneratedFrameOfReferenceUID(unsigned study)
		: GeneratedUID()
		{
			setFrameOfReference();
			setUnsigned(study);
		}
	GeneratedFrameOfReferenceUID(const char *s,unsigned study)
		: GeneratedUID(s)
		{
			setFrameOfReference();
			setUnsigned(study);
		}
	GeneratedFrameOfReferenceUID(unsigned study,unsigned series)
		: GeneratedUID()
		{
			setFrameOfReference();
			setUnsigned(study);
			setUnsigned(series);
		}
	GeneratedFrameOfReferenceUID(const char *s,unsigned study,
						unsigned series)
		: GeneratedUID(s)
		{
			setFrameOfReference();
			setUnsigned(study);
			setUnsigned(series);
		}
};

class GeneratedSynchronizationFrameOfReferenceUID : public GeneratedUID {
public:
	GeneratedSynchronizationFrameOfReferenceUID(void)
		: GeneratedUID()
		{
			setSynchronizationFrameOfReference();
		}
	GeneratedSynchronizationFrameOfReferenceUID(const char *s)
		: GeneratedUID(s)
		{
			setSynchronizationFrameOfReference();
		}
	GeneratedSynchronizationFrameOfReferenceUID(unsigned study)
		: GeneratedUID()
		{
			setSynchronizationFrameOfReference();
			setUnsigned(study);
		}
	GeneratedSynchronizationFrameOfReferenceUID(const char *s,unsigned study)
		: GeneratedUID(s)
		{
			setSynchronizationFrameOfReference();
			setUnsigned(study);
		}
	GeneratedSynchronizationFrameOfReferenceUID(unsigned study,unsigned series)
		: GeneratedUID()
		{
			setSynchronizationFrameOfReference();
			setUnsigned(study);
			setUnsigned(series);
		}
	GeneratedSynchronizationFrameOfReferenceUID(const char *s,unsigned study,
						unsigned series)
		: GeneratedUID(s)
		{
			setSynchronizationFrameOfReference();
			setUnsigned(study);
			setUnsigned(series);
		}
};

class GeneratedDirInstanceUID : public GeneratedUID {
public:
	GeneratedDirInstanceUID(void)
		: GeneratedUID()
		{
			setDirInstance();
		}
	GeneratedDirInstanceUID(const char *s)
		: GeneratedUID(s)
		{
			setDirInstance();
		}
};

class GeneratedDimensionOrganizationUID : public GeneratedUID {
public:
	GeneratedDimensionOrganizationUID(unsigned study,unsigned series,int dimensionOrganization)
		: GeneratedUID()
		{
			setDimensionOrganization();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(dimensionOrganization);
		}
	GeneratedDimensionOrganizationUID(const char *s,unsigned study,unsigned series,int dimensionOrganization)
		: GeneratedUID(s)
		{
			setDimensionOrganization();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(dimensionOrganization);
		}
};

class GeneratedConcatenationUID : public GeneratedUID {
public:
	GeneratedConcatenationUID(unsigned study,unsigned series,unsigned instance)
		: GeneratedUID()
		{
			setConcatenation();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
		}
	GeneratedConcatenationUID(const char *s,unsigned study,unsigned series,unsigned instance)
		: GeneratedUID(s)
		{
			setConcatenation();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
		}
};

class GeneratedRawDataUID : public GeneratedUID {
public:
	GeneratedRawDataUID(unsigned study,unsigned acquisition)
		: GeneratedUID()
		{
			setRawData();
			setUnsigned(study);
			setUnsigned(acquisition);
		}
	GeneratedRawDataUID(const char *s,unsigned study,unsigned acquisition)
		: GeneratedUID(s)
		{
			setRawData();
			setUnsigned(study);
			setUnsigned(acquisition);
		}
};

class GeneratedIrradiationEventUID : public GeneratedUID {
public:
	GeneratedIrradiationEventUID(unsigned study,unsigned series,unsigned instance)
		: GeneratedUID()
		{
			setIrradiationEvent();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
		}
	GeneratedIrradiationEventUID(const char *s,unsigned study,unsigned series,unsigned instance)
		: GeneratedUID(s)
		{
			setIrradiationEvent();
			setUnsigned(study);
			setUnsigned(series);
			setUnsigned(instance);
		}
};

#endif /* __Header_uidgen__ */

