#ifndef __Header_usepal__
#define __Header_usepal__

class UseablePalette
{
protected:
	DicomLUT *RedLUT;
	DicomLUT *GreenLUT;
	DicomLUT *BlueLUT;
public:
	UseablePalette(DicomLUT *r,DicomLUT *g,DicomLUT *b);
	~UseablePalette();
	Uint32 getLength(void);

	bool get16BitColorIndex(
			Uint16 index,
			Uint16 &red,
			Uint16 &green,
			Uint16 &blue)
		{
//cerr << "UseablePalette::get16BitColorIndex index=" << index << endl;
			if (index >= getLength()) return false;
			red=(*RedLUT)[index];
			green=(*GreenLUT)[index];
			blue=(*BlueLUT)[index];
//cerr << "UseablePalette::get16BitColorIndex"
//     << " index=" << dec << index
//     << " red=" << hex << red
//     << " green=" << hex << green
//     << " blue=" << hex << blue
//     << dec << endl;
			return true;
		}
};

#endif /* __Header_usepal__ */
