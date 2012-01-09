#ifndef __Header_SMPTESrc__
#define __Header_SMPTESrc__

#include "srcsink.h"
#include "smptetxt.h"

static const unsigned char SMPTE_Data_minval = 0;
static const unsigned char SMPTE_Data_maxval = 100;

class SMPTE_PixelDataSource : public SourceBase<Uint16> {
protected:
	Uint16 rows;
	Uint16 columns;
	bool signedpixrep;
	bool inverted;
	Int32  minval;
	Uint32 src_range;
	Uint32 dst_range;

	Uint16 row;
	Uint16 col;
	Uint16 *buffer;

	Uint16 background;
	Uint16 linevalue;

	Uint16 crosshatch_size;
	Uint16 crosshatch_offset_row;
	Uint16 crosshatch_offset_col;

	Uint16 border_thickness;
	Uint16 border_inset;

	Uint16 left_edge;
	Uint16 right_edge;
	Uint16 top_edge;
	Uint16 bottom_edge;

	Uint16 percent100;
	Uint16 percent95;
	Uint16 percent90;
	Uint16 percent80;
	Uint16 percent70;
	Uint16 percent60;
	Uint16 percent50;
	Uint16 percent40;
	Uint16 percent30;
	Uint16 percent20;
	Uint16 percent10;
	Uint16 percent5;
	Uint16 percent0;

	Uint16 highcontres_off;
	Uint16 highcontres_on;

	Uint16 contreslow_off;
	Uint16 contreslow_on;
	Uint16 contresmid_off;
	Uint16 contresmid_on;
	Uint16 contreshigh_off;
	Uint16 contreshigh_on;

	Uint16 resolution_block_size;

	Uint16 center_pattern_right_edge;
	Uint16 center_pattern_top_edge;

	Uint16 squares_height;
	Uint16 squares_width;

	Uint16 squares_left_edge;
	Uint16 squares_top_edge;

	Uint16 strip_height;
	Uint16 inset_height;

	Uint16 strip_left_edge;
	Uint16 strip_right_edge;
	Uint16 top_strip_top_edge;
	Uint16 bottom_strip_top_edge;

	Uint16 inset_left_edge;
	Uint16 inset_right_edge;
	Uint16 top_inset_top_edge;
	Uint16 bottom_inset_top_edge;

	void write_left_justified_vertically_aligned_top_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 left_col, Uint16 top_row,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			// In an XBM
			// - the low bit is the first and the high bit the last
			// - rows are padded to byte boundaries

			if (col >= left_col && col < left_col+bitmap_cols && row >= top_row && row < top_row+bitmap_rows) {
				Assert(row >= top_row);
				Assert(col >= left_col);
				unsigned short index = (row-top_row)*((bitmap_cols-1)/8+1)*8+(col-left_col);
				//unsigned short byte_index = index/8;
				//unsigned short bit_index = index%8;
				//unsigned char bit = (bitmap[index/8] >> (index%8)) & 0x01;

//cerr << "col=" << dec << col << " index=" << index << " byte_index=" << byte_index << " bit_index=" << bit_index << " bit=" << (unsigned)bit << endl;

				if ((bitmap[index/8] >> (index%8)) & 0x01) *ptr=on;
			}
		}

	void write_left_justified_vertically_aligned_center_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 left_col, Uint16 vcenter,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			Assert(vcenter >= bitmap_rows/2);
			unsigned short top_row=vcenter-bitmap_rows/2;

			write_left_justified_vertically_aligned_top_string(bitmap,bitmap_cols,bitmap_rows,
				left_col,top_row,
				row,col,ptr,on);
		}

	void write_right_justified_vertically_aligned_center_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 right_col, Uint16 vcenter,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			Assert(vcenter >= bitmap_rows/2);
			unsigned short top_row=vcenter-bitmap_rows/2;
			Assert(right_col >= bitmap_cols);
			unsigned short left_col=right_col-bitmap_cols;

			write_left_justified_vertically_aligned_top_string(bitmap,bitmap_cols,bitmap_rows,
				left_col,top_row,
				row,col,ptr,on);
		}

	void write_centered_vertically_aligned_center_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 hcenter, Uint16 vcenter,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			Assert(vcenter >= bitmap_rows/2);
			unsigned short top_row=vcenter-bitmap_rows/2;
			Assert(hcenter >= bitmap_cols/2);
			unsigned short left_col=hcenter-bitmap_cols/2;

			write_left_justified_vertically_aligned_top_string(bitmap,bitmap_cols,bitmap_rows,
				left_col,top_row,
				row,col,ptr,on);
		}

	void write_centered_vertically_aligned_top_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 hcenter, Uint16 top_row,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			Assert(hcenter >= bitmap_cols/2);
			unsigned short left_col=hcenter-bitmap_cols/2;

			write_left_justified_vertically_aligned_top_string(bitmap,bitmap_cols,bitmap_rows,
				left_col,top_row,
				row,col,ptr,on);
		}

	void write_centered_vertically_aligned_bottom_string(
		const unsigned char *bitmap,Uint16 bitmap_cols,Uint16 bitmap_rows,
		Uint16 hcenter, Uint16 bottom_row,
		Uint16 row,Uint16 col,Uint16 *ptr,
		Uint16 on)
		{
			Assert(bottom_row >= bitmap_rows);
			unsigned short top_row=bottom_row-bitmap_rows;
			Assert(hcenter >= bitmap_cols/2);
			unsigned short left_col=hcenter-bitmap_cols/2;

			write_left_justified_vertically_aligned_top_string(bitmap,bitmap_cols,bitmap_rows,
				left_col,top_row,
				row,col,ptr,on);
		}

public:
	SMPTE_PixelDataSource(Uint16 r,Uint16 c,bool pixrep,bool inv,Uint16 minv,Uint16 maxv)
			: SourceBase<Uint16>()
		{
			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);

			signedpixrep=pixrep;
			inverted=inv;

			Assert (SMPTE_Data_maxval > SMPTE_Data_minval);
			src_range=SMPTE_Data_maxval-SMPTE_Data_minval;

			if (signedpixrep) {
				Assert ((Int16)(maxv) > (Int16)(minv));
				dst_range=(Int16)(maxv)-(Int16)(minv);
				minval=(Int16)(minv);
			}
			else {
				Assert (maxv > minv);
				dst_range=maxv-minv;
				minval=(Uint32)minv;
			}

//cerr << "minval     = 0x" << hex << minval       << " " << dec << minval     << " dec" << endl;
//cerr << "dst_range  = 0x" << hex << dst_range    << " " << dec << dst_range  << " dec" << endl;

			row=0;
			col=0;
			buffer=new Uint16[c];
			Assert(buffer);

			// 50% background
			// 75% border around edges
			// 75% cross-hatch pattern 2 pixels high and wide at 10% of height spacing
			// Note that the cross-hatch extends outside the borders

			background = (inverted ? 50l:50l)*dst_range/src_range+minval;
			linevalue  = (inverted ? 25l:75l)*dst_range/src_range+minval;

			crosshatch_size = rows / 10;		// Note: gives 51 not 50 for 512 cf. RP 133-1991
			crosshatch_offset_row = (rows % crosshatch_size) / 2;		// Centers the cross hatch
			crosshatch_offset_col = (columns % crosshatch_size) / 2;
			if ((columns/crosshatch_size) % 2) crosshatch_offset_col+=crosshatch_size/2;

//cerr << "crosshatch_size=" << dec << crosshatch_size << endl;
//cerr << "crosshatch_offset_row=" << dec << crosshatch_offset_row << endl;
//cerr << "crosshatch_offset_col=" << dec << crosshatch_offset_col << endl;

			border_thickness = (rows < 400) ? 2 : (rows/200);	// 0.5% of height, not less than 2
											// Note: gives 2 not 3 for 512 cf. RP 133-1991
			border_inset     = (rows < 200) ? 2 : (rows/100);	// 1.0% of height, not less than 2

//cerr << "border_thickness=" << dec << border_thickness << endl;
//cerr << "border_inset=" << dec << border_inset << endl;

			left_edge   = border_inset+border_thickness;		// first usable pixel
			right_edge  = columns-border_inset-border_thickness-1;	// last  usable pixel
			top_edge    = border_inset+border_thickness;		// first usable pixel
			bottom_edge = rows-border_inset-border_thickness-1;	// last  usable pixel

//cerr << "left_edge=" << dec << left_edge << endl;
//cerr << "right_edge=" << dec << right_edge << endl;
//cerr << "top_edge=" << dec << top_edge << endl;
//cerr << "bottom_edge=" << dec << bottom_edge << endl;

			// Precomputed pixel values ...

			percent100=(inverted ?   0l:100l)*dst_range/src_range+minval;
			percent95 =(inverted ?   5l: 95l)*dst_range/src_range+minval;
			percent90 =(inverted ?  10l: 90l)*dst_range/src_range+minval;
			percent80 =(inverted ?  20l: 80l)*dst_range/src_range+minval;
			percent70 =(inverted ?  30l: 70l)*dst_range/src_range+minval;
			percent60 =(inverted ?  40l: 60l)*dst_range/src_range+minval;
			percent50 =(inverted ?  50l: 50l)*dst_range/src_range+minval;
			percent40 =(inverted ?  60l: 40l)*dst_range/src_range+minval;
			percent30 =(inverted ?  70l: 30l)*dst_range/src_range+minval;
			percent20 =(inverted ?  80l: 20l)*dst_range/src_range+minval;
			percent10 =(inverted ?  90l: 10l)*dst_range/src_range+minval;
			percent5  =(inverted ?  95l:  5l)*dst_range/src_range+minval;
			percent0  =(inverted ? 100l:  0l)*dst_range/src_range+minval;

//cerr << "percent100 = 0x" << hex << percent100   << " " << dec << percent100 << " dec" << endl;
//cerr << "percent95  = 0x" << hex << percent95    << " " << dec << percent95  << " dec" << endl;
//cerr << "percent90  = 0x" << hex << percent90    << " " << dec << percent90  << " dec" << endl;
//cerr << "percent80  = 0x" << hex << percent80    << " " << dec << percent80  << " dec" << endl;
//cerr << "percent70  = 0x" << hex << percent70    << " " << dec << percent70  << " dec" << endl;
//cerr << "percent60  = 0x" << hex << percent60    << " " << dec << percent60  << " dec" << endl;
//cerr << "percent50  = 0x" << hex << percent50    << " " << dec << percent50  << " dec" << endl;
//cerr << "percent40  = 0x" << hex << percent40    << " " << dec << percent40  << " dec" << endl;
//cerr << "percent30  = 0x" << hex << percent30    << " " << dec << percent30  << " dec" << endl;
//cerr << "percent20  = 0x" << hex << percent20    << " " << dec << percent20  << " dec" << endl;
//cerr << "percent10  = 0x" << hex << percent10    << " " << dec << percent10  << " dec" << endl;
//cerr << "percent5   = 0x" << hex << percent5     << " " << dec << percent5   << " dec" << endl;
//cerr << "percent0   = 0x" << hex << percent0     << " " << dec << percent0   << " dec" << endl;

			// High contrast resolution patterns

			highcontres_off = percent0;
			highcontres_on  = percent100;

			// Low contrast resolution patterns

			contreslow_off  = (inverted ? 50l:50l)*dst_range/src_range+minval;
			contreslow_on   = (inverted ? 49l:51l)*dst_range/src_range+minval;
			contresmid_off  = (inverted ? 52l:48l)*dst_range/src_range+minval;
			contresmid_on   = (inverted ? 49l:51l)*dst_range/src_range+minval;
			contreshigh_off = (inverted ? 52l:48l)*dst_range/src_range+minval;
			contreshigh_on  = (inverted ? 47l:53l)*dst_range/src_range+minval;

			resolution_block_size = rows * 25 / 400;	// 6.25% of height
									// Note: gives 15 not 16 for 256 cf. RP 133-1991

			center_pattern_right_edge = columns/2+resolution_block_size*2;
			center_pattern_top_edge = crosshatch_size*5+crosshatch_offset_row - resolution_block_size*3/2;
									// center between 4th and 6th horizontal cross-hatch

			// Big grayscale squares and black/white strips are aligned with cross-hatch pattern

			// cross-hatch interval is 10% of vertical height
			// each square is 10%, 4 vertically (40% of vertical height) and 6 horizontally

			Assert (columns > 6*crosshatch_size);	// can't do patterns if too tall and narrow
			Assert (rows > 4*crosshatch_size);	// can't do patterns if too tall and narrow

			squares_height=crosshatch_size;
			squares_width =crosshatch_size;

			squares_left_edge = ((columns-6*crosshatch_size)/2)/crosshatch_size*crosshatch_size+crosshatch_offset_col;
			squares_top_edge  = ((rows-4*crosshatch_size)/2)/crosshatch_size*crosshatch_size+crosshatch_offset_row;

			// each strip is 8%, centered in the 3rd and 8th horizontal cross-hatch areas
			// and from the left of the 2nd to the right of the 2nd last vertical cross-hatch areas

			strip_height=8*rows/100;
			inset_height=strip_height/2;

			Assert (crosshatch_size > strip_height);

			strip_left_edge = crosshatch_size+crosshatch_offset_col;
			strip_right_edge = columns-strip_left_edge+1;
			top_strip_top_edge  = crosshatch_size*2+(crosshatch_size-strip_height)/2+crosshatch_offset_row+1;
			bottom_strip_top_edge  = crosshatch_size*7+(crosshatch_size-strip_height)/2+crosshatch_offset_row+1;

			Assert (strip_right_edge > strip_left_edge);

			inset_left_edge = strip_left_edge + (strip_right_edge-strip_left_edge)/4;
			inset_right_edge = strip_right_edge - (strip_right_edge-strip_left_edge)/4;
			top_inset_top_edge  = top_strip_top_edge + strip_height/4;
			bottom_inset_top_edge  = bottom_strip_top_edge + strip_height/4;


		}

	~SMPTE_PixelDataSource()
		{
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			col=0;
			if (row >= rows) return 0;
			Uint16 *ptr=buffer;
			while (col < columns) {

				// Do background, border and cross-hatch pattern

				if (col >= border_inset && col < left_edge && row >= border_inset && row < rows-border_inset		// left border
				 || col > right_edge && col < columns-border_inset && row >= border_inset && row < rows-border_inset	// right border
				 || row >= border_inset && row < top_edge && col >= border_inset && col < columns-border_inset		// top border
				 || row > bottom_edge && row < rows-border_inset && col >= border_inset && col < columns-border_inset	// bottom border
				 || col%crosshatch_size == crosshatch_offset_col		// 1st pixel of vertical crosshatch
				 || col%crosshatch_size == crosshatch_offset_col+1		// 2nd pixel of vertical crosshatch
				 || row%crosshatch_size == crosshatch_offset_row		// 1st pixel of horizontal crosshatch
				 || row%crosshatch_size == crosshatch_offset_row+1		// 2nd pixel of horizontal crosshatch
				) {
//cerr << "border at row=" << dec << row << " col=" << col << endl;
					*ptr=linevalue;
				}
				else {
					*ptr=background;
				}

				// Do long thin white or black strips ("windows" in RP 133-1991)

				if (col >= strip_left_edge && col <= strip_right_edge) {
					if (row >= top_strip_top_edge && row < top_strip_top_edge+strip_height) {
						if (row >= top_inset_top_edge && row < top_inset_top_edge+inset_height
				 		 && col >= inset_left_edge && col <= inset_right_edge) {
							*ptr=percent0;
						}
						else {
							*ptr=percent100;
						}
					}
					else if (row >= bottom_strip_top_edge && row < bottom_strip_top_edge+strip_height) {
						if (row >= bottom_inset_top_edge && row < bottom_inset_top_edge+inset_height
				 		 && col >= inset_left_edge && col <= inset_right_edge) {
							*ptr=percent100;
						}
						else {
							*ptr=percent0;
						}
					}
				}

				// Do big grayscale squares aligned with cross-hatch pattern

				// the choice of < or <= and > or >= and the occasional +1 are to cover 2 pixel wide cross-hatches

				if (row >= squares_top_edge && row <= squares_top_edge+squares_height+1) {
					if (col >= squares_left_edge && col <= squares_left_edge+squares_width+1) {
						*ptr=percent30;
					}
					else if (col >= squares_left_edge+squares_width && col <= squares_left_edge+2*squares_width) {
						*ptr=percent40;
					}
					else if (col >= squares_left_edge+2*squares_width && col <= squares_left_edge+4*squares_width) {
						// two squares
						*ptr=percent50;
					}
					else if (col >= squares_left_edge+4*squares_width && col <= squares_left_edge+5*squares_width) {
						*ptr=percent60;
					}
					else if (col >= squares_left_edge+5*squares_width && col <= squares_left_edge+6*squares_width+1) {
						*ptr=percent70;
					}
				}
				else if (row > squares_top_edge+squares_height && row <= squares_top_edge+2*squares_height) {
					if (col >= squares_left_edge && col <= squares_left_edge+squares_width+1) {
						*ptr=percent20;
					}
					else if (col >= squares_left_edge+5*squares_width && col <= squares_left_edge+6*squares_width+1) {
						*ptr=percent80;
					}
				}
				else if (row > squares_top_edge+2*squares_height && row < squares_top_edge+3*squares_height) {
					if (col >= squares_left_edge && col <= squares_left_edge+squares_width+1) {
						*ptr=percent10;
					}
					else if (col >= squares_left_edge+5*squares_width && col <= squares_left_edge+6*squares_width+1) {
						*ptr=percent90;
					}
				}
				else if (row > squares_top_edge+3*squares_height-1 && row <= squares_top_edge+4*squares_height+1) {
					if (col >= squares_left_edge && col <= squares_left_edge+squares_width) {
						*ptr=percent0;
					}
					else if (col > squares_left_edge+squares_width && col <= squares_left_edge+2*squares_width) {
						if (row > squares_top_edge+13*squares_height/4 && row <= squares_top_edge+15*squares_height/4
						 && col > squares_left_edge+5*squares_width/4 && col <= squares_left_edge+7*squares_width/4) {
							*ptr=percent5;
						}
						else {
							*ptr=percent0;
						}
					}
					else if (col > squares_left_edge+2*squares_width && col <= squares_left_edge+4*squares_width) {
						*ptr=percent0;
					}
					else if (col > squares_left_edge+4*squares_width && col <= squares_left_edge+5*squares_width) {
						if (row > squares_top_edge+13*squares_height/4 && row <= squares_top_edge+15*squares_height/4
						 && col > squares_left_edge+17*squares_width/4 && col <= squares_left_edge+19*squares_width/4) {
							*ptr=percent95;
						}
						else {
							*ptr=percent100;
						}
					}
					else if (col > squares_left_edge+5*squares_width && col <= squares_left_edge+6*squares_width+1) {
						*ptr=percent100;
					}
				}


				// Do high contrast resolution patterns in corners
				// highest is 1 on 1 off
				// middle  is 2 on 2 off
				// lowest  is 3 on 3 off

				
				// horizontal high contrast resolution

				if (col >= left_edge && col < left_edge+resolution_block_size
				 || col > right_edge-resolution_block_size && col <= right_edge) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=(row-top_edge)%2 ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((row-top_edge-resolution_block_size)%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((row-top_edge-2*resolution_block_size)%6 >= 3) ? highcontres_on : highcontres_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=(bottom_edge-row)%2 ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((bottom_edge-resolution_block_size-row)%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((bottom_edge-2*resolution_block_size-row)%6 >= 3) ? highcontres_on : highcontres_off;
					}
				}

				// vertical high contrast resolution

				// Left

				if (col >= left_edge+resolution_block_size && col < left_edge+2*resolution_block_size) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=(col-left_edge-resolution_block_size)%2 ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((col-left_edge-resolution_block_size)%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((col-left_edge-resolution_block_size)%6 >= 3) ? highcontres_on : highcontres_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=(col-left_edge-resolution_block_size)%2 ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((col-left_edge-resolution_block_size)%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((col-left_edge-resolution_block_size)%6 >= 3) ? highcontres_on : highcontres_off;
					}
				}

				// Right

				if (col > right_edge-2*resolution_block_size && col <= right_edge-resolution_block_size) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=(col-(right_edge-2*resolution_block_size))%2 ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((col-(right_edge-2*resolution_block_size))%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((col-(right_edge-2*resolution_block_size))%6 >= 3) ? highcontres_on : highcontres_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=(col-(right_edge-2*resolution_block_size))%2 ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((col-(right_edge-2*resolution_block_size))%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((col-(right_edge-2*resolution_block_size))%6 >= 3) ? highcontres_on : highcontres_off;
					}
				}


				// horizontal low contrast mid frequency resolution

				if (col >= left_edge+2*resolution_block_size && col < left_edge+3*resolution_block_size
				 || col > right_edge-3*resolution_block_size && col <= right_edge-2*resolution_block_size) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=((row-top_edge)%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((row-top_edge-resolution_block_size)%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((row-top_edge-2*resolution_block_size)%4 >= 2) ? contreshigh_on : contreshigh_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=((bottom_edge-row)%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((bottom_edge-resolution_block_size-row)%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((bottom_edge-2*resolution_block_size-row)%4 >= 2) ? contreshigh_on : contreshigh_off;
					}
				}

				// vertical low contrast mid frequency resolution

				// Left

				if (col >= left_edge+3*resolution_block_size && col < left_edge+4*resolution_block_size) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contreshigh_on : contreshigh_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((col-left_edge-3*resolution_block_size)%4 >= 2) ? contreshigh_on : contreshigh_off;
					}
				}

				// Right

				if (col > right_edge-4*resolution_block_size && col <= right_edge-3*resolution_block_size) {

					// Top ...

					if (row >= top_edge && row < top_edge+resolution_block_size) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row >= top_edge+resolution_block_size && row < top_edge+2*resolution_block_size) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row >= top_edge+2*resolution_block_size && row < top_edge+3*resolution_block_size) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contreshigh_on : contreshigh_off;
					}

					// Bottom ...

					if (row > bottom_edge-resolution_block_size && row <= bottom_edge) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row > bottom_edge-2*resolution_block_size && row <= bottom_edge-resolution_block_size) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row > bottom_edge-3*resolution_block_size && row <= bottom_edge-2*resolution_block_size) {
						*ptr=((col-(right_edge-4*resolution_block_size))%4 >= 2) ? contreshigh_on : contreshigh_off;
					}
				}

				// Do center resolution pattern with same layout at top right hand corner ...

				// horizontal high contrast resolution

				if (col > center_pattern_right_edge-resolution_block_size && col <= center_pattern_right_edge) {

					if (row >= center_pattern_top_edge && row < center_pattern_top_edge+resolution_block_size) {
						*ptr=(row-center_pattern_top_edge)%2 ? highcontres_on : highcontres_off;
					}
					if (row >= center_pattern_top_edge+resolution_block_size && row < center_pattern_top_edge+2*resolution_block_size) {
						*ptr=((row-center_pattern_top_edge-resolution_block_size)%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row >= center_pattern_top_edge+2*resolution_block_size && row < center_pattern_top_edge+3*resolution_block_size) {
						*ptr=((row-center_pattern_top_edge-2*resolution_block_size)%6 >= 3) ? highcontres_on : highcontres_off;
					}
				}

				// vertical high contrast resolution

				if (col > center_pattern_right_edge-2*resolution_block_size && col <= center_pattern_right_edge-resolution_block_size) {

					if (row >= center_pattern_top_edge && row < center_pattern_top_edge+resolution_block_size) {
						*ptr=(col-(center_pattern_right_edge-2*resolution_block_size))%2 ? highcontres_on : highcontres_off;
					}
					if (row >= center_pattern_top_edge+resolution_block_size && row < center_pattern_top_edge+2*resolution_block_size) {
						*ptr=((col-(center_pattern_right_edge-2*resolution_block_size))%4 >= 2) ? highcontres_on : highcontres_off;
					}
					if (row >= center_pattern_top_edge+2*resolution_block_size && row < center_pattern_top_edge+3*resolution_block_size) {
						*ptr=((col-(center_pattern_right_edge-2*resolution_block_size))%6 >= 3) ? highcontres_on : highcontres_off;
					}
				}

				// horizontal low contrast mid frequency resolution

				if (col > center_pattern_right_edge-3*resolution_block_size && col <= center_pattern_right_edge-2*resolution_block_size) {

					if (row >= center_pattern_top_edge && row < center_pattern_top_edge+resolution_block_size) {
						*ptr=((row-center_pattern_top_edge)%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row >= center_pattern_top_edge+resolution_block_size && row < center_pattern_top_edge+2*resolution_block_size) {
						*ptr=((row-center_pattern_top_edge-resolution_block_size)%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row >= center_pattern_top_edge+2*resolution_block_size && row < center_pattern_top_edge+3*resolution_block_size) {
						*ptr=((row-center_pattern_top_edge-2*resolution_block_size)%4 >= 2) ? contreshigh_on : contreshigh_off;
					}
				}

				// vertical low contrast mid frequency resolution

				if (col > center_pattern_right_edge-4*resolution_block_size && col <= center_pattern_right_edge-3*resolution_block_size) {

					// Top ...

					if (row >= center_pattern_top_edge && row < center_pattern_top_edge+resolution_block_size) {
						*ptr=((col-(center_pattern_right_edge-4*resolution_block_size))%4 >= 2) ? contreslow_on : contreslow_off;
					}
					if (row >= center_pattern_top_edge+resolution_block_size && row < center_pattern_top_edge+2*resolution_block_size) {
						*ptr=((col-(center_pattern_right_edge-4*resolution_block_size))%4 >= 2) ? contresmid_on : contresmid_off;
					}
					if (row >= center_pattern_top_edge+2*resolution_block_size && row < center_pattern_top_edge+3*resolution_block_size) {
						*ptr=((col-(center_pattern_right_edge-4*resolution_block_size))%4 >= 2) ? contreshigh_on : contreshigh_off;
					}

				}

				// write text

				write_left_justified_vertically_aligned_center_string(
					smpte_string_70percent_bits,smpte_string_70percent_width,smpte_string_70percent_height,
					squares_left_edge+6*squares_width+5,squares_top_edge+squares_height/2,
					row,col,ptr,
					percent100);

				write_left_justified_vertically_aligned_center_string(
					smpte_string_80percent_bits,smpte_string_80percent_width,smpte_string_80percent_height,
					squares_left_edge+6*squares_width+5,squares_top_edge+3*squares_height/2,
					row,col,ptr,
					percent100);

				write_left_justified_vertically_aligned_center_string(
					smpte_string_90percent_bits,smpte_string_90percent_width,smpte_string_90percent_height,
					squares_left_edge+6*squares_width+5,squares_top_edge+5*squares_height/2,
					row,col,ptr,
					percent100);

				write_left_justified_vertically_aligned_center_string(
					smpte_string_100percent_bits,smpte_string_100percent_width,smpte_string_100percent_height,
					squares_left_edge+6*squares_width+5,squares_top_edge+7*squares_height/2,
					row,col,ptr,
					percent100);

				write_right_justified_vertically_aligned_center_string(
					smpte_string_30percent_bits,smpte_string_30percent_width,smpte_string_30percent_height,
					squares_left_edge-5,squares_top_edge+squares_height/2,
					row,col,ptr,
					percent100);

				write_right_justified_vertically_aligned_center_string(
					smpte_string_20percent_bits,smpte_string_20percent_width,smpte_string_20percent_height,
					squares_left_edge-5,squares_top_edge+3*squares_height/2,
					row,col,ptr,
					percent100);

				write_right_justified_vertically_aligned_center_string(
					smpte_string_10percent_bits,smpte_string_10percent_width,smpte_string_10percent_height,
					squares_left_edge-5,squares_top_edge+5*squares_height/2,
					row,col,ptr,
					percent100);

				write_right_justified_vertically_aligned_center_string(
					smpte_string_0percent_bits,smpte_string_0percent_width,smpte_string_0percent_height,
					squares_left_edge-5,squares_top_edge+7*squares_height/2,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_top_string(
					smpte_string_40percent_bits,smpte_string_40percent_width,smpte_string_40percent_height,
					squares_left_edge+3*squares_width/2,squares_top_edge+5,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_top_string(
					smpte_string_50percent_bits,smpte_string_50percent_width,smpte_string_50percent_height,
					squares_left_edge+5*squares_width/2,squares_top_edge+5,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_top_string(
					smpte_string_50percent_bits,smpte_string_50percent_width,smpte_string_50percent_height,
					squares_left_edge+7*squares_width/2,squares_top_edge+5,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_top_string(
					smpte_string_60percent_bits,smpte_string_60percent_width,smpte_string_60percent_height,
					squares_left_edge+9*squares_width/2,squares_top_edge+5,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_center_string(
					smpte_string_0percent_bits,smpte_string_0percent_width,smpte_string_0percent_height,
					squares_left_edge+5*squares_width/2,squares_top_edge+7*squares_width/2-smpte_string_0percent_height,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_center_string(
					smpte_string_95percent_bits,smpte_string_95percent_width,smpte_string_95percent_height,
					squares_left_edge+7*squares_width/2,squares_top_edge+7*squares_width/2-smpte_string_95percent_height,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_center_string(
					smpte_string_5percent_bits,smpte_string_5percent_width,smpte_string_5percent_height,
					squares_left_edge+5*squares_width/2,squares_top_edge+7*squares_width/2+smpte_string_5percent_height,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_center_string(
					smpte_string_100percent_bits,smpte_string_100percent_width,smpte_string_100percent_height,
					squares_left_edge+7*squares_width/2,squares_top_edge+7*squares_width/2+smpte_string_100percent_height,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_bottom_string(
					smpte_string_smpte_bits,smpte_string_smpte_width,smpte_string_smpte_height,
					columns/2,bottom_edge-smpte_string_revision_height-4,
					row,col,ptr,
					percent100);

				write_centered_vertically_aligned_bottom_string(
					smpte_string_revision_bits,smpte_string_revision_width,smpte_string_revision_height,
					columns/2,bottom_edge-2,
					row,col,ptr,
					percent100);

				++ptr;
				++col;
			}
			++row;
			return columns;
		}

	const Uint16 *getBuffer(void)		{ return buffer; }

	size_t getBufferCount(void) const	{ return col; }

	int good(void) const	{ return row < rows; }
};


#endif // __Header_SMPTESrc__
