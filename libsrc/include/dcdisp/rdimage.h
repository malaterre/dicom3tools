#ifndef __Header_rdimage__
#define __Header_rdimage__

class ReadableImage
{
private:
	SupplySource *supplysource;
protected:
	Uint16 columns;
	Uint16 rows;
	Uint16 frames;
	Uint16 planes;
	Uint16 bitsneeded;
	Uint16 bitsallocated;
	Uint16 bitsstored;
	Uint16 highbit;
	bool issigned;

	class SourceBase<Uint16> *getSource(void);
public:
	ReadableImage(
		SupplySource *s,Uint16 c,Uint16 r,Uint16 f,Uint16 p,
		Uint16 bn,Uint16 ba,Uint16 bs,Uint16 hb,
		bool issgn=false);

	virtual ~ReadableImage();

	virtual bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);

	virtual bool getColorCellsWanted(unsigned &nwanted,unsigned &nminimum);
	virtual bool setColorCellsAvailable(unsigned n,unsigned long *table);
	virtual bool getColorCellValues(unsigned n,
		unsigned short *&red,
		unsigned short *&green,
		unsigned short *&blue);

	virtual bool hasWindowLevelWidth(void)	{ return false; }
	virtual bool setWindowLevelWidth(Uint16 level,Uint16 width);
	virtual bool getWindowLevelWidth(Uint16 &level,Uint16 &width);
	virtual bool needToResetColorCells(void);
	virtual bool needToResetIndexedPixels(void);

	virtual bool setVOILUT(Uint16 first,Uint16 number,Uint16 depth,const Uint16 *table);


	bool getSigned(void)	{ return issigned; }
	Uint16 getBits(void)	{ return bitsstored; }
};

class ReadableSinglePlaneImage :
	public ReadableImage
{
private:
	Uint16 *bufferforfulldepthimage;

	virtual bool getBufferedFullDepthImage(void);
protected:
	virtual char mapPixel(const Uint16 src)
		{ Assert(0); (void)src; return 0; }
	virtual const char *getPixelMap(Uint32 &mapsize) const
		{ Assert(0); mapsize=0; return 0; }
public:
	ReadableSinglePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsneeded,Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit,
		bool issigned=false);

	virtual ~ReadableSinglePlaneImage();

	virtual bool getImageStatistics(Uint16 framecount,
		Uint16 &minval,Uint16 &maxval,
		bool excludepadvalue,Uint16 padvalue);

	virtual bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);
};

class ReadableMultiplePlaneImage :
	public ReadableImage
{
protected:
	virtual char mapPixel(const Uint16 v1,const Uint16 v2,const Uint16 v3)
		{ Assert(0); (void)v1; (void)v2; (void)v3; return 0; }
public:
	ReadableMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,Uint16 planes,
		Uint16 bitsneeded,Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);

	virtual ~ReadableMultiplePlaneImage();
};

class Readable8BitSinglePlaneImage :
	public ReadableSinglePlaneImage
{
public:
	Readable8BitSinglePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit,
		bool issigned=false);

	virtual ~Readable8BitSinglePlaneImage();

	bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);
};

class Readable16BitSinglePlaneImage :
	public ReadableSinglePlaneImage
{
public:
	Readable16BitSinglePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit,
		bool issigned=false);

	virtual ~Readable16BitSinglePlaneImage();

	bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);
};

class Readable16BitMultiplePlaneImage :
	public ReadableMultiplePlaneImage
{
public:
	Readable16BitMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);

	virtual ~Readable16BitMultiplePlaneImage();
};

class ReadableInterleaved24BitMultiplePlaneImage :
	public ReadableMultiplePlaneImage
{
public:
	ReadableInterleaved24BitMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);


	virtual ~ReadableInterleaved24BitMultiplePlaneImage();

	virtual bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);
};

class ReadableNonInterleaved24BitMultiplePlaneImage :
	public ReadableMultiplePlaneImage
{
public:
	ReadableNonInterleaved24BitMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);

	virtual ~ReadableNonInterleaved24BitMultiplePlaneImage();

	virtual bool put8BitIndexedPixels(
			char *dest,
			unsigned long bytes_per_row,
			unsigned long tlhc_col,
			unsigned long tlhc_row,
			unsigned long brhc_col,
			unsigned long brhc_row);
};

class ReadableInterleaved32BitMultiplePlaneImage :
	public ReadableMultiplePlaneImage
{
public:
	ReadableInterleaved32BitMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);

	virtual ~ReadableInterleaved32BitMultiplePlaneImage();
};

class ReadableNonInterleaved32BitMultiplePlaneImage :
	public ReadableMultiplePlaneImage
{
public:
	ReadableNonInterleaved32BitMultiplePlaneImage(
		SupplySource *s,Uint16 columns,Uint16 rows,Uint16 frames,
		Uint16 bitsallocated,Uint16 bitsstored,Uint16 highbit);

	virtual ~ReadableNonInterleaved32BitMultiplePlaneImage();
};

#endif /* __Header_rdimage__ */
