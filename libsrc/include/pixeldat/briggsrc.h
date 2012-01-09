#ifndef __Header_BriggsSrc__
#define __Header_BriggsSrc__

#include "srcsink.h"

// The following is the standard (unrotated) target).
//
// Checkerboard TLHC origin is defined relative to 0\0 origin
//
// Other targets are derived by rotating by 90,180,270 degrees
// bearing in mind that the actual TLHC checker of each checkerboard
// should be light (ie. because of even number of checkers B-80 and
// B-90 need special attention)


static const int bpt4_width  = 128;
static const int bpt4_height = 256;
static const int bpt4_checkerboards_per_target = 17;

struct bpt4_parameter_entry {
	unsigned short score;
	unsigned short checkers_per_board;
	unsigned short pixels_per_checker;
	unsigned short tlhc_col;
	unsigned short tlhc_row;
} bpt4_parameter_table[bpt4_checkerboards_per_target] = {
	90,	2,	1,	51,	188,
	85,	3,	1,	38,	187,
	80,	4,	1,	38,	200,
	75,	5,	1,	52,	202,
	70,	7,	1,	67,	200,
	65,	11,	1,	63,	180,
	60,	5,	2,	60,	160,
	55,	7,	2,	36,	160,
	50,	5,	3,	11,	160,
	45,	5,	4,	8,	185,
	40,	3,	7,	19,	215,
	35,	3,	8,	50,	217,
	30,	3,	10,	84,	209,
	25,	3,	13,	84,	160,
	20,	3,	16,	75,	102,
	15,	3,	20,	4,	90,
	10,	3,	25,	4,	4
};

static void
generate_bpt4(Uint16 *matrix,unsigned width, unsigned height,
			unsigned put_at_col,unsigned put_at_row,
			unsigned short rotation,
			Uint16 light_value,Uint16 dark_value,Uint16 surround_value)
{
	// Prepare coordinates of rotated target pattern from standard target ...

	unsigned short target_width;
	unsigned short target_height;
	unsigned short target_tlhc_row[bpt4_checkerboards_per_target];
	unsigned short target_tlhc_col[bpt4_checkerboards_per_target];

	if (rotation == 0) {
//cerr << "rotation = " << dec << rotation << endl;
		target_width=bpt4_width;
		target_height=bpt4_height;
		unsigned short checkerboard;
		for (checkerboard=0; checkerboard<bpt4_checkerboards_per_target; ++checkerboard) {
//cerr << "checkerboard = " << dec << checkerboard << endl;
			target_tlhc_row[checkerboard]=bpt4_parameter_table[checkerboard].tlhc_row;
			target_tlhc_col[checkerboard]=bpt4_parameter_table[checkerboard].tlhc_col;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_row = " << dec << bpt4_parameter_table[checkerboard].tlhc_row << endl;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_col = " << dec << bpt4_parameter_table[checkerboard].tlhc_col << endl;
//cerr << "\t target_tlhc_row[checkerboard] = " << dec << target_tlhc_row[checkerboard] << endl;
//cerr << "\t target_tlhc_col[checkerboard] = " << dec << target_tlhc_col[checkerboard] << endl;
			Assert(target_tlhc_row[checkerboard]<target_height);
			Assert(target_tlhc_col[checkerboard]<target_width);
		}
	}
	else if (rotation == 90) {
//cerr << "rotation = " << dec << rotation << endl;
		target_width=bpt4_height;	// swap height and width
		target_height=bpt4_width;
		unsigned short checkerboard;
		for (checkerboard=0; checkerboard<bpt4_checkerboards_per_target; ++checkerboard) {
//cerr << "checkerboard = " << dec << checkerboard << endl;
			target_tlhc_col[checkerboard]=bpt4_height-bpt4_parameter_table[checkerboard].tlhc_row
				-bpt4_parameter_table[checkerboard].pixels_per_checker*bpt4_parameter_table[checkerboard].checkers_per_board;
			target_tlhc_row[checkerboard]=bpt4_parameter_table[checkerboard].tlhc_col;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_row = " << dec << bpt4_parameter_table[checkerboard].tlhc_row << endl;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_col = " << dec << bpt4_parameter_table[checkerboard].tlhc_col << endl;
//cerr << "\t target_tlhc_row[checkerboard] = " << dec << target_tlhc_row[checkerboard] << endl;
//cerr << "\t target_tlhc_col[checkerboard] = " << dec << target_tlhc_col[checkerboard] << endl;
			Assert(target_tlhc_row[checkerboard]<target_height);
			Assert(target_tlhc_col[checkerboard]<target_width);
		}
	}
	else if (rotation == 180) {
//cerr << "rotation = " << dec << rotation << endl;
		target_width=bpt4_width;
		target_height=bpt4_height;
		unsigned short checkerboard;
		for (checkerboard=0; checkerboard<bpt4_checkerboards_per_target; ++checkerboard) {
//cerr << "checkerboard = " << dec << checkerboard << endl;
			target_tlhc_row[checkerboard]=bpt4_height-bpt4_parameter_table[checkerboard].tlhc_row
				-bpt4_parameter_table[checkerboard].pixels_per_checker*bpt4_parameter_table[checkerboard].checkers_per_board;
			target_tlhc_col[checkerboard]=bpt4_width-bpt4_parameter_table[checkerboard].tlhc_col
				-bpt4_parameter_table[checkerboard].pixels_per_checker*bpt4_parameter_table[checkerboard].checkers_per_board;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_row = " << dec << bpt4_parameter_table[checkerboard].tlhc_row << endl;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_col = " << dec << bpt4_parameter_table[checkerboard].tlhc_col << endl;
//cerr << "\t target_tlhc_row[checkerboard] = " << dec << target_tlhc_row[checkerboard] << endl;
//cerr << "\t target_tlhc_col[checkerboard] = " << dec << target_tlhc_col[checkerboard] << endl;
			Assert(target_tlhc_row[checkerboard]<target_height);
			Assert(target_tlhc_col[checkerboard]<target_width);
		}
	}
	else if (rotation == 270) {
//cerr << "rotation = " << dec << rotation << endl;
		target_width=bpt4_height;	// swap height and width
		target_height=bpt4_width;
		unsigned short checkerboard;
		for (checkerboard=0; checkerboard<bpt4_checkerboards_per_target; ++checkerboard) {
//cerr << "checkerboard = " << dec << checkerboard << endl;
			target_tlhc_col[checkerboard]=bpt4_parameter_table[checkerboard].tlhc_row;
			target_tlhc_row[checkerboard]=bpt4_width-bpt4_parameter_table[checkerboard].tlhc_col
				-bpt4_parameter_table[checkerboard].pixels_per_checker*bpt4_parameter_table[checkerboard].checkers_per_board;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_row = " << dec << bpt4_parameter_table[checkerboard].tlhc_row << endl;
//cerr << "\t bpt4_parameter_table[checkerboard].tlhc_col = " << dec << bpt4_parameter_table[checkerboard].tlhc_col << endl;
//cerr << "\t target_tlhc_row[checkerboard] = " << dec << target_tlhc_row[checkerboard] << endl;
//cerr << "\t target_tlhc_col[checkerboard] = " << dec << target_tlhc_col[checkerboard] << endl;
			Assert(target_tlhc_row[checkerboard]<target_height);
			Assert(target_tlhc_col[checkerboard]<target_width);
		}
	}
	else {
		Assert(0);
	}

	// Verify space in pixel matrix and fill with surround value ...

	Assert(matrix);
	Assert(put_at_col+target_width <= width);
	Assert(put_at_row+target_height <= height);

//cerr << "filling surround" << endl;

	unsigned row;
	unsigned col;
	for (row=put_at_row; row < put_at_row+target_height; ++row) {
//cerr << "row=" << dec << row << endl;
		 for (col=put_at_col; col < put_at_col+target_width; ++col) {
			Assert(col<width);
			Assert(row<height);
			*(matrix+row*width+col)=surround_value;
		}
	}

	// Generate the checkerboards that comprise the target
	// by iterating through checkboard rows and columns,
	// alternating light and dark, starting with light at TLHC ...

	unsigned short checkerboard;
	for (checkerboard=0; checkerboard<bpt4_checkerboards_per_target; ++checkerboard) {
//cerr << "checkerboard = " << dec << checkerboard << endl;
		unsigned ncheckers=bpt4_parameter_table[checkerboard].checkers_per_board;
		unsigned start_row=put_at_row+target_tlhc_row[checkerboard];
		unsigned start_col=put_at_col+target_tlhc_col[checkerboard];
		unsigned checker_height=bpt4_parameter_table[checkerboard].pixels_per_checker;
		unsigned checker_width =bpt4_parameter_table[checkerboard].pixels_per_checker;
		bool first_checker_in_row_is_light=true;
		unsigned row_checker_count;
		for (row_checker_count=0; row_checker_count<ncheckers; ++row_checker_count) {
			bool next_checker_in_row_is_light=first_checker_in_row_is_light;
			unsigned col_checker_count;
			for (col_checker_count=0; col_checker_count<ncheckers; ++col_checker_count) {
				// make an entire checker ...
//cerr << "\t row_checker_count = " << dec << row_checker_count << " col_checker_count = " << col_checker_count << endl;
				Uint16 value = next_checker_in_row_is_light ? light_value : dark_value;
				unsigned first_row=start_row+checker_height*row_checker_count;
				unsigned last_row=start_row+checker_height*(row_checker_count+1)-1;
				unsigned first_col=start_col+checker_width*col_checker_count;
				unsigned last_col=start_col+checker_width*(col_checker_count+1)-1;
				for (row=first_row; row <= last_row; ++row) {
		 			for (col=first_col; col <= last_col; ++col) {
//cerr << "\t\t row = " << dec << row << " col = " << col << endl;
						Assert(col<width);
						Assert(row<height);
						*(matrix+row*width+col)=value;
					}
				}
				next_checker_in_row_is_light=!next_checker_in_row_is_light;
			}
			first_checker_in_row_is_light=!first_checker_in_row_is_light;
		}
	
	}
}


class Briggs_PixelDataSource : public SourceBase<Uint16> {
protected:
	Uint16 rows;
	Uint16 columns;

	bool signedpixrep;
	bool inverted;

	Int32  minval;
	Int32  maxval;

	Uint16 row;
	Uint16 *buffer;
	Uint16 *bufptr;

public:
	Briggs_PixelDataSource(Uint16 r,Uint16 c,bool pixrep,bool inv,Uint16 minv,Uint16 maxv,Uint16 contrast,Uint16 surround_percent,Uint16 background_percent)
			: SourceBase<Uint16>()
		{
cerr << "Briggs_PixelDataSource: contrast = " << dec << contrast << endl;
cerr << "Briggs_PixelDataSource: surround_percent = " << dec << surround_percent << endl;

			rows=r;
			Assert(rows);
			columns=c;
			Assert(columns);

cerr << "Briggs_PixelDataSource: rows=" << rows << endl;
cerr << "Briggs_PixelDataSource: columns=" << columns << endl;

			Assert(surround_percent<=100);
			Assert(background_percent<=100);

			signedpixrep=pixrep;
			inverted=inv;

			Uint32 range;
			if (signedpixrep) {
				Assert ((Int16)(maxv) > (Int16)(minv));
				range=(Int16)(maxv)-(Int16)(minv);
				minval=(Int16)minv;
				maxval=(Int16)maxv;
			}
			else {
				Assert (maxv > minv);
				range=maxv-minv;
				minval=(Uint32)minv;
				maxval=(Uint32)maxv;
			}

			//minval=(Uint32)minv;
			//maxval=(Uint32)maxv;
			//Assert (maxv > minv);
			//Uint32 range=maxv-minv;

cerr << "Briggs_PixelDataSource: minval = " << dec << minval << endl;
cerr << "Briggs_PixelDataSource: maxval = " << dec << maxval << endl;
cerr << "Briggs_PixelDataSource: range = " << dec << range << endl;

			Assert(contrast <= range);

			// Allocate and buffer the entire array, not just one row ...

			buffer=new Uint16[columns*rows];
			Assert(buffer);
			bufptr=buffer-columns;		// position BEFORE buffer, since 1st "read" increments it
			row=0;


			// Determine how many 256*256 paired targets will fit in
			// available rows and columns ...

			unsigned short pairsperrow = columns/256;
			unsigned short pairspercolumn = rows/256;
			unsigned short npairs = pairsperrow * pairspercolumn;

			Assert(npairs);			// caller should have checked > 256*256

cerr << "Briggs_PixelDataSource: pairsperrow=" << dec << pairsperrow << endl;
cerr << "Briggs_PixelDataSource: pairspercolumn=" << dec << pairspercolumn << endl;
cerr << "Briggs_PixelDataSource: npairs=" << npairs << dec << endl;

			// Determine left over spacing ...

			unsigned short col_offset=columns-pairsperrow*256;
			unsigned short col_spacing=pairsperrow > 1 ? col_offset/(pairsperrow-1) : 0;
			col_offset=(col_offset-col_spacing*(pairsperrow-1))/2;

			unsigned short row_offset=rows-pairspercolumn*256;
			unsigned short row_spacing=pairspercolumn > 1 ? row_offset/(pairspercolumn-1) : 0;
			row_offset=(row_offset-row_spacing*(pairspercolumn-1))/2;

cerr << "Briggs_PixelDataSource: col_offset=" << dec << col_offset << endl;
cerr << "Briggs_PixelDataSource: col_spacing=" << dec << col_spacing << endl;
cerr << "Briggs_PixelDataSource: row_offset=" << dec << row_offset << endl;
cerr << "Briggs_PixelDataSource: row_spacing=" << dec << row_spacing << endl;

			// Fill whole array with background value

			Uint16 background_value = (Uint32)range*background_percent/100+minval;
			if (inverted) {
				background_value=range-(background_value-minval)+minval;
			}

			if (signedpixrep) {
cerr << "Briggs_PixelDataSource: \t background_value = " << dec << (Int16)background_value << endl;
				Assert((Int16)background_value >= minval);
				Assert((Int16)background_value <= maxval);
			}
			else {
cerr << "Briggs_PixelDataSource: \t background_value = " << dec << background_value << endl;
				Assert(background_value >= minval);
				Assert(background_value <= maxval);
			}

			{
				Uint16 *ptr=buffer;
				unsigned row;
				for (row=0; row<rows;++row) {
					unsigned col;
					for (col=0; col<columns; ++col) {
						*ptr++=background_value;
					}
				}
			}

			// Pairs are ordered by contrast between targets in the
			// pair from 1 to npairs
			//
			// ie. P-1 has targets with ranks 1 and n
			// and P-2 has targets with ranks 2 and n-1, etc.
			//
			// Goal is to spread pairs around so that average contrast
			// between pairs is evenly distributed, ie. average along
			// rows == average along columns == average along diagonals
			//
			// For a square matrix greater than 2*2, this is a "magic
			// square" problem ... here we use the Siamese algorithm
			// to find a magic square for squares of odd order ... for
			// even order squares, a 2*2 square, or a rectangle it does
			// close enough for the purpose ...

			// Note also that for the 2*2 case , the Siamese algorithm will put
			// P-3 top right and P-4 bottom left, swapped relative to Briggs'
			// example, but this is OK, as Briggs points out.

			unsigned short pair_table[npairs];
			#define index_pair_table(pcol,prow)	(pair_table[(prow)*pairsperrow+(pcol)])

			// Fill pair_table with zero to indicate unfilled ...

			short prow;
			short pcol;
			for (prow=0; prow<pairspercolumn; prow++) {
				for (pcol=0; pcol<pairsperrow; pcol++) {
					index_pair_table(pcol,prow)=0;
				}
			}

			// First pair goes in middle of top row (if odd number of pairsperrow)
			// or left of middle (if even number of pairsperrow) ...

			prow=0;
			pcol=(pairsperrow+1)/2 - 1;	// 0->error,1->0,2->0,3->1,4->1,5->2, etc.
			Assert(pcol != -1);

			unsigned short whichpair=1;
			while (1) {
//cerr << "Briggs_PixelDataSource: Putting whichpair=" << whichpair << " at pcol=" << pcol << " prow=" << prow << endl;
				index_pair_table(pcol,prow)=whichpair;

				if (++whichpair > npairs) break;

				// where to put next pair determined by Siamese algorithm
				// "ordinary vector" (1,-1) with wraparound ...
				short trycol = (pcol+1 >= pairsperrow) ? 0 : pcol+1;
				short tryrow = (prow <= 0) ? pairspercolumn-1 : prow-1;
//cerr << "Briggs_PixelDataSource: Trying trycol=" << trycol << " tryrow=" << tryrow << endl;
				if (index_pair_table(trycol,tryrow)) {	// non-zero == filled
//cerr << "Briggs_PixelDataSource: Filled" << endl;
					// "break vector" (0,1) with wraparound ...
					trycol = pcol;
					tryrow = (prow+1 >= pairspercolumn) ? 0 : prow+1;
//cerr << "Briggs_PixelDataSource: Trying trycol=" << trycol << " tryrow=" << tryrow << endl;
					if (index_pair_table(trycol,tryrow)) {	// non-zero == filled
//cerr << "Briggs_PixelDataSource: Filled" << endl;
						// shit :(
						//Assert(0);
						break;
					}
				}
				pcol=trycol;
				prow=tryrow;
			}

			cerr << "Briggs_PixelDataSource: Pair table ..." << endl;
			for (prow=0; prow<pairspercolumn; prow++) {
				unsigned short rowtotal=0;
				for (pcol=0; pcol<pairsperrow; pcol++) {
					cerr << "\t" << index_pair_table(pcol,prow);
					rowtotal+=index_pair_table(pcol,prow);
				}
				cerr << "\t| " << (float)rowtotal/pairsperrow << endl;
			}
			cerr << "\t";
			for (pcol=0; pcol<pairsperrow; pcol++) {
				cerr << "--------";
			}
			cerr << endl;
			if (pairspercolumn == pairsperrow) {
				unsigned short diagonaltotal=0;
				unsigned short diagonalcount=0;
				for (pcol=pairsperrow-1,prow=0; pcol>=0 && prow<pairspercolumn; pcol--, prow++) {
					diagonaltotal+=index_pair_table(pcol,prow);
					++diagonalcount;
				}
				cerr << (float)diagonaltotal/diagonalcount;
			}
			for (pcol=0; pcol<pairsperrow; pcol++) {
				unsigned short coltotal=0;
				for (prow=0; prow<pairspercolumn; prow++) {
					coltotal+=index_pair_table(pcol,prow);
				}
				cerr << "\t" << (float)coltotal/pairspercolumn;
			}
			if (pairspercolumn == pairsperrow) {
				unsigned short diagonaltotal=0;
				unsigned short diagonalcount=0;
				for (pcol=0,prow=0; pcol<pairsperrow && prow<pairspercolumn; pcol++, prow++) {
					diagonaltotal+=index_pair_table(pcol,prow);
					++diagonalcount;
				}
				cerr << "\t  " << (float)diagonaltotal/diagonalcount;
			}
			cerr << endl;

			// Build target table by iterating through targets and indexing pair table ...
			// - locations assigned by location in pair table
			// - two ranks assigned by P-n and ntargets+1-n
			// - alternating horizontal and vertical pairs

			unsigned short ntargets=npairs*2;
cerr << "Briggs_PixelDataSource: ntargets = " << dec << ntargets << endl;

			// NB. this target table is NOT in Briggs target number (T-n) order ...

			struct target_table_entry {
				unsigned short rank;
				unsigned short rotation;
				unsigned short tlhc_col;
				unsigned short tlhc_row;
			} target_table[ntargets];

			unsigned short target=0;
			for (pcol=0; pcol<pairsperrow; pcol++) {
				for (prow=0; prow<pairspercolumn; prow++) {
					Assert(target<ntargets);

					unsigned tlhc_pair_col=col_offset+pcol*(256+col_spacing);
					unsigned tlhc_pair_row=row_offset+prow*(256+row_spacing);

					// Horizontal or vertical orientation ?
					// Also alternate hi/lo contrast rank order based on odd/even col
					// (Briggs just says "balance" ranks ... this seems as good a way as any)

					if (prow%2) {		// odd rows  (1,3,5 ...)
						if (pcol%2) {	// odd cols  (1,3,5 ...)
							// vertical ...
							target_table[target].rotation=0;
							target_table[target+1].rotation=180;
							target_table[target].tlhc_col=tlhc_pair_col;
							target_table[target].tlhc_row=tlhc_pair_row;
							target_table[target+1].tlhc_col=tlhc_pair_col+128;
							target_table[target+1].tlhc_row=tlhc_pair_row;
							target_table[target].rank=index_pair_table(pcol,prow);
							target_table[target+1].rank=ntargets+1-index_pair_table(pcol,prow);
						}
						else {		// even cols (0,2,4 ...)
							// horizontal ...
							target_table[target].rotation=270;
							target_table[target+1].rotation=90;
							target_table[target].tlhc_col=tlhc_pair_col;
							target_table[target].tlhc_row=tlhc_pair_row;
							target_table[target+1].tlhc_col=tlhc_pair_col;
							target_table[target+1].tlhc_row=tlhc_pair_row+128;
							target_table[target].rank=ntargets+1-index_pair_table(pcol,prow);
							target_table[target+1].rank=index_pair_table(pcol,prow);
						}
					}
					else {			// even rows (0,2,4 ...)
						if (pcol%2) {	// odd cols  (1,3,5 ...)
							// horizontal ...
							target_table[target].rotation=270;
							target_table[target+1].rotation=90;
							target_table[target].tlhc_col=tlhc_pair_col;
							target_table[target].tlhc_row=tlhc_pair_row;
							target_table[target+1].tlhc_col=tlhc_pair_col;
							target_table[target+1].tlhc_row=tlhc_pair_row+128;
							target_table[target].rank=index_pair_table(pcol,prow);
							target_table[target+1].rank=ntargets+1-index_pair_table(pcol,prow);
						}
						else {		// even cols (0,2,4 ...)
							// vertical ...
							target_table[target].rotation=0;
							target_table[target+1].rotation=180;
							target_table[target].tlhc_col=tlhc_pair_col;
							target_table[target].tlhc_row=tlhc_pair_row;
							target_table[target+1].tlhc_col=tlhc_pair_col+128;
							target_table[target+1].tlhc_row=tlhc_pair_row;
							target_table[target].rank=ntargets+1-index_pair_table(pcol,prow);
							target_table[target+1].rank=index_pair_table(pcol,prow);
						}
					}

					target+=2;
				}
			}
									
cerr << "Briggs_PixelDataSource: target table ..." << endl;
			for (target=0; target < ntargets; ++target) {
				cerr << "\trank=" << dec << target_table[target].rank
				     << "\trotation=" << target_table[target].rotation
				     << "\ttlhc_col=" << target_table[target].tlhc_col
				     << "\ttlhc_row=" << target_table[target].tlhc_row
				     << endl;
			}


			for (target=0; target < ntargets; ++target) {
cerr << "Briggs_PixelDataSource: target = " << dec << (target+1) << endl;
				unsigned short rank=target_table[target].rank;
				Uint16 light_value= (rank-1)*(range-contrast)/(ntargets > 1 ? ntargets-1 : 1)+contrast+minval;
				Uint16 dark_value = (rank-1)*(range-contrast)/(ntargets > 1 ? ntargets-1 : 1)+minval;	// ie. light_value-contrast

				Uint16 average_command_level=(light_value+dark_value)/2;
				Uint16 median_command_level=range/2+minval;
				Uint16 surround_value;

				if (signedpixrep) {
					Int32 difference=(Int16)average_command_level-(Int16)median_command_level;
//cerr << "Briggs_PixelDataSource: \t difference = " << dec << difference << endl;
					surround_value=surround_percent*difference/100 + median_command_level;
//cerr << "Briggs_PixelDataSource: \t average_command_level = " << dec << (Int16)average_command_level << endl;
//cerr << "Briggs_PixelDataSource: \t median_command_level = " << dec << (Int16)median_command_level << endl;
//cerr << "Briggs_PixelDataSource: \t surround_value = " << dec << (Int16)surround_value << endl;
				}
				else {
					Int32 difference=average_command_level-median_command_level;
//cerr << "Briggs_PixelDataSource: \t difference = " << dec << difference << endl;
					surround_value=surround_percent*difference/100 + median_command_level;
//cerr << "Briggs_PixelDataSource: \t average_command_level = " << dec << average_command_level << endl;
//cerr << "Briggs_PixelDataSource: \t median_command_level = " << dec << median_command_level << endl;
//cerr << "Briggs_PixelDataSource: \t surround_value = " << dec << surround_value << endl;
				}

				if (inverted) {
					light_value=range-(light_value-minval)+minval;
					dark_value=range-(dark_value-minval)+minval;
					surround_value=range-(dark_value-minval)+minval;
				}

cerr << "Briggs_PixelDataSource: \t rank = " << dec << rank << endl;

				if (signedpixrep) {
cerr << "Briggs_PixelDataSource: \t light_value = " << dec << (Int16)light_value << endl;
cerr << "Briggs_PixelDataSource: \t dark_value = " << dec << (Int16)dark_value << endl;
cerr << "Briggs_PixelDataSource: \t surround_value = " << dec << (Int16)surround_value << endl;
					Assert((Int16)light_value >= minval);
					Assert((Int16)light_value <= maxval);
					Assert((Int16)dark_value >= minval);
					Assert((Int16)dark_value <= maxval);
					Assert((Int16)surround_value >= minval);
					Assert((Int16)surround_value <= maxval);
				}
				else {
cerr << "Briggs_PixelDataSource: \t light_value = " << dec << light_value << endl;
cerr << "Briggs_PixelDataSource: \t dark_value = " << dec << dark_value << endl;
cerr << "Briggs_PixelDataSource: \t surround_value = " << dec << surround_value << endl;
					Assert(light_value >= minval);
					Assert(light_value <= maxval);
					Assert(dark_value >= minval);
					Assert(dark_value <= maxval);
					Assert(surround_value >= minval);
					Assert(surround_value <= maxval);
				}

				generate_bpt4(buffer,columns,rows,
					target_table[target].tlhc_col,target_table[target].tlhc_row,target_table[target].rotation,
					light_value,dark_value,surround_value);
			}
		}

	~Briggs_PixelDataSource()
		{
			if (buffer) delete[] buffer;
		}

	size_t read(void)
		{
			bufptr+=columns;
			++row;
			return columns;
		}

	const Uint16 *getBuffer(void)
		{
			Assert (bufptr >= buffer);	// Just in case someone calls getBuffer() without read()
			return bufptr;
		}

	size_t getBufferCount(void) const	{ return columns; }

	int good(void) const	{ return row < rows; }
};


#endif // __Header_BriggsSrc__
