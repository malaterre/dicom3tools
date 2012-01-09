#ifndef __Header_lutclass__
#define __Header_lutclass__

class DicomLUT : public ErrorsInClass
{
protected:
	Uint32 nentries;
	Uint32 firstindexvalue;
	Uint16 bitsallocated;
	Uint16 *array;

	bool fillArray(Attribute *data);
public:
	DicomLUT(Attribute *desc,Attribute*data);
	virtual ~DicomLUT();

	virtual Uint16 operator[](Uint32 index)
		{
			Assert(array);
			Assert(index < nentries);
//cerr << "DicomLUT::operator[]"
//     << " index" << dec << index
//     << " value" << hex << array[index]
//     << dec << endl;
			return array[index];
		}

	virtual Uint32 getLength(void)	{ return nentries; }
};

class NormalDicomLUT : public DicomLUT
{
public:
	NormalDicomLUT(Attribute *desc,Attribute*data);
	virtual ~NormalDicomLUT();
};

class LargeDicomLUT : public DicomLUT
{
public:
	LargeDicomLUT(Attribute *desc,Attribute*data);
	virtual ~LargeDicomLUT();
};

class SegmentedDicomLUT : public DicomLUT
{
public:
	SegmentedDicomLUT(Attribute *desc,Attribute*data);
	virtual ~SegmentedDicomLUT();
};

#endif /* __Header_lutclass__ */
