#include <fstream.h>

#include "attrmxls.h"
#include "attrtype.h"
#include "attrothr.h"
#include "attrval.h"
#include "elmdict.h"
#include "elmconst.h"
#include "sopclu.h"
#include "mesgtext.h"
#include "dcopt.h"

static Attribute *
isValuePresentElseAbort(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a && a->getVM())
		return a;
	else {
		log << filename << ": " 
		    << AMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		exit(1);
	}
}

static void
rotate( double dst_row_dircos_x,double dst_row_dircos_y,double dst_row_dircos_z,
	double dst_col_dircos_x,double dst_col_dircos_y,double dst_col_dircos_z,
	double dst_nrm_dircos_x,double dst_nrm_dircos_y,double dst_nrm_dircos_z,
	double  src_pos_x,double  src_pos_y,double  src_pos_z,
	double &dst_pos_x,double &dst_pos_y,double &dst_pos_z)
{
	dst_pos_x = dst_row_dircos_x * src_pos_x
		  + dst_row_dircos_y * src_pos_y
		  + dst_row_dircos_z * src_pos_z;

	dst_pos_y = dst_col_dircos_x * src_pos_x
		  + dst_col_dircos_y * src_pos_y
		  + dst_col_dircos_z * src_pos_z;

	dst_pos_z = dst_nrm_dircos_x * src_pos_x
		  + dst_nrm_dircos_y * src_pos_y
		  + dst_nrm_dircos_z * src_pos_z;
}

static bool
getPositionOrientationSpacingAndSize(AttributeList &list,const char *filename,TextOutputStream &log,bool verbose,
	double &row_dircos_x,double &row_dircos_y,double &row_dircos_z,
	double &col_dircos_x,double &col_dircos_y,double &col_dircos_z,
	double &nrm_dircos_x,double &nrm_dircos_y,double &nrm_dircos_z,
	double &pos_x,double &pos_y,double &pos_z,
	unsigned long &rows,unsigned long &cols,
	double &row_spacing,double &col_spacing,
	double &row_length,double &col_length)
{

	// the localizer is the "destination" for the posting operation

	Attribute *aImageOrientationPatient = isValuePresentElseAbort(list,
		TagFromName(ImageOrientationPatient),"ImageOrientationPatient",filename,log);

	if (aImageOrientationPatient->getVM() != 6) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValueMultiplicity)
		    << " - ImageOrientationPatient"
		    << endl;
		return false;
	}

	if (!aImageOrientationPatient->getValue(0,row_dircos_x)
	 || !aImageOrientationPatient->getValue(1,row_dircos_y)
	 || !aImageOrientationPatient->getValue(2,row_dircos_z)
	 || !aImageOrientationPatient->getValue(3,col_dircos_x)
	 || !aImageOrientationPatient->getValue(4,col_dircos_y)
	 || !aImageOrientationPatient->getValue(5,col_dircos_z)
	) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValue)
		    << " - ImageOrientationPatient"
		    << endl;
		return false;
	}

//cerr << "row_dircos_x = " << row_dircos_x << endl;
//cerr << "row_dircos_y = " << row_dircos_y << endl;
//cerr << "row_dircos_z = " << row_dircos_z << endl;

//cerr << "col_dircos_x = " << col_dircos_x << endl;
//cerr << "col_dircos_y = " << col_dircos_y << endl;
//cerr << "col_dircos_z = " << col_dircos_z << endl;

	// compute nrm to row and col (i.e. cross product of row and col unit vectors)

	nrm_dircos_x = row_dircos_y * col_dircos_z - row_dircos_z * col_dircos_y;
	nrm_dircos_y = row_dircos_z * col_dircos_x - row_dircos_x * col_dircos_z;
	nrm_dircos_z = row_dircos_x * col_dircos_y - row_dircos_y * col_dircos_x;

//cerr << "nrm_dircos_x = " << nrm_dircos_x << endl;
//cerr << "nrm_dircos_y = " << nrm_dircos_y << endl;
//cerr << "nrm_dircos_z = " << nrm_dircos_z << endl;

	// check are unit vectors ...

	if (!(fabs(row_dircos_x*row_dircos_x+row_dircos_y*row_dircos_y+row_dircos_z*row_dircos_z - 1.0) < 0.000001)
	 || !(fabs(col_dircos_x*col_dircos_x+col_dircos_y*col_dircos_y+col_dircos_z*col_dircos_z - 1.0) < 0.000001)
	 || !(fabs(nrm_dircos_x*nrm_dircos_x+nrm_dircos_y*nrm_dircos_y+nrm_dircos_z*nrm_dircos_z - 1.0) < 0.000001)
	) {
		log << filename << ": "
		    << "Abort - row, column or normal are not unit vectors"
		    << " - ImageOrientationPatient"
		    << endl;
		return false;
	}

	// check are orthogonal (dot product is zero, i.e. cos 90)

	if (!(fabs(row_dircos_x*col_dircos_x+row_dircos_y*col_dircos_y+row_dircos_z*col_dircos_z) < 0.000001)
	 || !(fabs(row_dircos_x*nrm_dircos_x+row_dircos_y*nrm_dircos_y+row_dircos_z*nrm_dircos_z) < 0.000001)
	 || !(fabs(col_dircos_x*nrm_dircos_x+col_dircos_y*nrm_dircos_y+col_dircos_z*nrm_dircos_z) < 0.000001)
	) {
		log << filename << ": "
		    << "Abort - row, column and normal are not orthogonal"
		    << " - ImageOrientationPatient"
		    << endl;
		return false;
	}

	Attribute *aImagePositionPatient = isValuePresentElseAbort(list,
		TagFromName(ImagePositionPatient),"ImagePositionPatient",filename,log);

	if (aImagePositionPatient->getVM() != 3) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValueMultiplicity)
		    << " - ImagePositionPatient"
		    << endl;
		return false;
	}

	if (!aImagePositionPatient->getValue(0,pos_x)
	 || !aImagePositionPatient->getValue(1,pos_y)
	 || !aImagePositionPatient->getValue(2,pos_z)
	) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValue)
		    << " - ImagePositionPatient"
		    << endl;
		return false;
	}

//cerr << "pos_x = " << pos_x << endl;
//cerr << "pos_y = " << pos_y << endl;
//cerr << "pos_z = " << pos_z << endl;

	Attribute *aPixelSpacing = isValuePresentElseAbort(list,
		TagFromName(PixelSpacing),"PixelSpacing",filename,log);

	if (aPixelSpacing->getVM() != 2) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValueMultiplicity)
		    << " - PixelSpacing"
		    << endl;
		return false;
	}

	if (!aPixelSpacing->getValue(0,row_spacing)
	 || !aPixelSpacing->getValue(1,col_spacing)
	) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValue)
		    << " - PixelSpacing"
		    << endl;
		return false;
	}

//cerr << "row_spacing = " << row_spacing << endl;
//cerr << "col_spacing = " << col_spacing << endl;

	Attribute *aRows = isValuePresentElseAbort(list,
		TagFromName(Rows),"Rows",filename,log);

	if (aRows->getVM() != 1) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValueMultiplicity)
		    << " - Rows"
		    << endl;
		return false;
	}

	if (!aRows->getValue(0,rows)) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValue)
		    << " - Rows"
		    << endl;
		return false;
	}

//cerr << "rows = " << rows << endl;

	Attribute *aColumns = isValuePresentElseAbort(list,
		TagFromName(Columns),"Columns",filename,log);

	if (aColumns->getVM() != 1) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValueMultiplicity)
		    << " - Columns"
		    << endl;
		return false;
	}

	if (!aColumns->getValue(0,cols)) {
		log << filename << ": "
		    << AMsgDC(BadAttributeValue)
		    << " - Columns"
		    << endl;
		return false;
	}

//cerr << "cols = " << cols << endl;

	row_length=cols*row_spacing;
	col_length=rows*col_spacing;

//cerr << "row_length = " << row_length << endl;
//cerr << "col_length = " << col_length << endl;

	return true;
}

static inline void
drawPixel(Uint16 *graphics_buffer,unsigned long graphics_buffer_rows,unsigned long graphics_buffer_cols,
	int row_pixel,int col_pixel)
{
//cerr << "drawPixel: row=" << row_pixel << " col=" << col_pixel << endl;
	if (row_pixel >= 1 && row_pixel <= graphics_buffer_rows
	 && col_pixel >= 1 && col_pixel <= graphics_buffer_cols) {
		unsigned long which_bit=(row_pixel-1)*graphics_buffer_cols+(col_pixel-1);
		graphics_buffer[which_bit/16] |= Uint16(1)<<(which_bit%16);
//cerr << "drawPixel: which_bit=" << which_bit << " %16=" << (which_bit%16) << " word=" << (which_bit/16) << endl;
	}
	else {
//cerr << "clipped" << endl;
	}
}

static void
drawLine(Uint16 *graphics_buffer,unsigned long graphics_buffer_rows,unsigned long graphics_buffer_cols,
	int from_row_pixel,int from_col_pixel,int to_row_pixel,int to_col_pixel)
{
	// start and end pixels are not yet clipped to boundaries
	// pixels are DICOM style ... TLHC is 1,1, BRHC is cols,rows

	if (abs(to_col_pixel-from_col_pixel) > abs(to_row_pixel-from_row_pixel)) {
		// integer increment along column axis
//cerr << "doing integer increment along column axis" << endl;
		if (from_col_pixel > to_col_pixel) {
			// swap drawing direction to keep column increment positive
//cerr << "swap drawing direction to keep column increment positive" << endl;
			int tmp;
			tmp=from_col_pixel; from_col_pixel=to_col_pixel; to_col_pixel=tmp;
			tmp=from_row_pixel; from_row_pixel=to_row_pixel; to_row_pixel=tmp;
		}
		double drow=to_row_pixel-from_row_pixel;
		double dcol=to_col_pixel-from_col_pixel;
//cerr << "drow = " << drow << endl;
//cerr << "dcol = " << dcol << endl;

		Assert(fabs(dcol) > 0.00001);
		double slope=drow/dcol;
//cerr << "slope drow/dcol = " << slope << endl;
		Assert(-1.0 <= slope && slope <= 1.0);		// otherwise would be doing rows
		int col;
		double row=from_row_pixel+0.5;	// the 0.5 is a constant throughout for the rounding operation
		for (col=from_col_pixel; col <= to_col_pixel; ++col) {
			drawPixel(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,int(row),col);
			row+=slope;
		}
	}
	else if (to_row_pixel != from_row_pixel) {
		// integer increment along row axis
//cerr << "doing integer increment along row axis" << endl;
		if (from_row_pixel > to_row_pixel) {
			// swap drawing direction to keep row increment positive
//cerr << "swap drawing direction to keep row increment positive" << endl;
			int tmp;
			tmp=from_col_pixel; from_col_pixel=to_col_pixel; to_col_pixel=tmp;
			tmp=from_row_pixel; from_row_pixel=to_row_pixel; to_row_pixel=tmp;
		}
		double drow=to_row_pixel-from_row_pixel;
		double dcol=to_col_pixel-from_col_pixel;
//cerr << "drow = " << drow << endl;
//cerr << "dcol = " << dcol << endl;

		Assert(fabs(drow) > 0.00001);
		double slope=dcol/drow;
//cerr << "slope dcol/drow = " << slope << endl;
		int row;
		double col=from_col_pixel+0.5;	// the 0.5 is a constant throughout for the rounding operation
		for (row=from_row_pixel; row <= to_row_pixel; ++row) {
			drawPixel(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,row,int(col));
			col+=slope;
		}
	}
	else {
		// to_col_pixel == from_col_pixel && to_row_pixel == from_row_pixel
//cerr << "just draw a point" << endl;
		drawPixel(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,to_row_pixel,to_col_pixel);
	}
}


int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool veryveryverbose=options.get("veryveryverbose") || options.get("vvv");
	if (veryveryverbose) veryverbose=true;
	if (veryverbose) verbose=true;

	const char *localizer_filename;
	if (!options.get("localizer",localizer_filename)) {
		cerr << "Must specify a localizer image" << endl;
		bad=true;
	}

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	int numberofinputfiles=!options;

	const char **filenamestable = new const char *[numberofinputfiles];
	Assert(filenamestable);
	const char **ptr;
	for (ptr=filenamestable; !options && (*ptr++=options()); ++options);

	options.done();

	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose|-vv|-veryverbose|-vvv|-veryveryverbose]"
			<< " [-localizer filename]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (!dicom_output_options.transfersyntaxuid)
		dicom_output_options.transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;

	bool success=true;
	TextOutputStream log(cerr);

	DicomOutputStream dout(*(ostream *)output_opener,
		dicom_output_options.transfersyntaxuid,
		dicom_output_options.usemetaheader,
		dicom_output_options.useimplicitmetaheader,
		dicom_output_options.addtiff);

	// Open the localizer image ...

	if (veryverbose) log << "Reading \"" << localizer_filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
	ifstream *fstr=new ifstream(localizer_filename,ios::in|ios::binary);
#else
	ifstream *fstr=new ifstream(localizer_filename);
#endif
	if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
		log << AMsgDC(FileReadOpenFailed);
		if (localizer_filename) log <<" - \"" << localizer_filename << "\"";
		exit(1);
	}

	DicomInputStream din(*(istream *)fstr,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList localizer_list;
		
	if (veryverbose) log << "******** While reading ... " << localizer_filename << " ... ********" << endl; 
	localizer_list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags);

	if (!localizer_list.good()) {
		log << localizer_list.errors()
			<< EMsgDC(DatasetReadFailed) << endl;
		exit(1);
	}

	double dst_row_dircos_x;
	double dst_row_dircos_y;
	double dst_row_dircos_z;
	double dst_col_dircos_x;
	double dst_col_dircos_y;
	double dst_col_dircos_z;
	double dst_nrm_dircos_x;
	double dst_nrm_dircos_y;
	double dst_nrm_dircos_z;
	double dst_pos_x;
	double dst_pos_y;
	double dst_pos_z;
	double dst_row_spacing;
	double dst_col_spacing;
	double dst_row_length;
	double dst_col_length;
	unsigned long dst_rows;
	unsigned long dst_cols;

	if (!getPositionOrientationSpacingAndSize(localizer_list,localizer_filename,log,verbose,
		dst_row_dircos_x,dst_row_dircos_y,dst_row_dircos_z,
		dst_col_dircos_x,dst_col_dircos_y,dst_col_dircos_z,
		dst_nrm_dircos_x,dst_nrm_dircos_y,dst_nrm_dircos_z,
		dst_pos_x,dst_pos_y,dst_pos_z,
		dst_rows,dst_cols,
		dst_row_spacing,dst_col_spacing,
		dst_row_length,dst_col_length)
	) exit(1);

	unsigned long graphics_buffer_rows=dst_rows;
	unsigned long graphics_buffer_cols=dst_cols;
	unsigned long graphics_buffer_size=(graphics_buffer_rows*graphics_buffer_cols-1)/16+1;
	Uint16 *graphics_buffer=new Uint16[graphics_buffer_size];
	Assert(graphics_buffer);
	{ unsigned long i; Uint16 *ptr; for (i=0,ptr=graphics_buffer; i<graphics_buffer_size; ++i) *ptr++=0; }

	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		const char *filename=filenamestable[i];
		Assert(filename);
		if (veryverbose) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			log << AMsgDC(FileReadOpenFailed);
			if (filename) log <<" - \"" << filename << "\"";
			exit(1);
		}

		DicomInputStream din(*(istream *)fstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;
		
		if (veryverbose) log << "******** While reading ... " << filename << " ... ********" << endl; 
		list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags);

		if (!list.good()) {
			log << list.errors()
				<< EMsgDC(DatasetReadFailed) << endl;
			exit(1);
		}

		double src_row_dircos_x;
		double src_row_dircos_y;
		double src_row_dircos_z;
		double src_col_dircos_x;
		double src_col_dircos_y;
		double src_col_dircos_z;
		double src_nrm_dircos_x;
		double src_nrm_dircos_y;
		double src_nrm_dircos_z;
		double src_pos_x;
		double src_pos_y;
		double src_pos_z;
		double src_row_spacing;
		double src_col_spacing;
		double src_row_length;
		double src_col_length;
		unsigned long src_rows;
		unsigned long src_cols;


		if (!getPositionOrientationSpacingAndSize(list,filename,log,verbose,
			src_row_dircos_x,src_row_dircos_y,src_row_dircos_z,
			src_col_dircos_x,src_col_dircos_y,src_col_dircos_z,
			src_nrm_dircos_x,src_nrm_dircos_y,src_nrm_dircos_z,
			src_pos_x,src_pos_y,src_pos_z,
			src_rows,src_cols,
			src_row_spacing,src_col_spacing,
			src_row_length,src_col_length)
		) exit(1);

		// Build a square to project with 4 corners TLHC, TRHC, BRHC, BLHC ...

		double pos_x[4];
		double pos_y[4];
		double pos_z[4];

		// TLHC is what is in ImagePositionPatient

		pos_x[0]=src_pos_x;
		pos_y[0]=src_pos_y;
		pos_z[0]=src_pos_z;

		// TRHC

		pos_x[1]=src_pos_x + src_row_dircos_x*(src_row_length-1);
		pos_y[1]=src_pos_y + src_row_dircos_y*(src_row_length-1);
		pos_z[1]=src_pos_z + src_row_dircos_z*(src_row_length-1);

		// BRHC

		pos_x[2]=src_pos_x + src_row_dircos_x*(src_row_length-1) + src_col_dircos_x*(src_col_length-1);
		pos_y[2]=src_pos_y + src_row_dircos_y*(src_row_length-1) + src_col_dircos_y*(src_col_length-1);
		pos_z[2]=src_pos_z + src_row_dircos_z*(src_row_length-1) + src_col_dircos_z*(src_col_length-1);

		// BLHC

		pos_x[3]=src_pos_x + src_col_dircos_x*(src_col_length-1);
		pos_y[3]=src_pos_y + src_col_dircos_y*(src_col_length-1);
		pos_z[3]=src_pos_z + src_col_dircos_z*(src_col_length-1);

		int row_pixel[4];
		int col_pixel[4];

		int i;
		for (i=0; i<4; ++i) {

			// we want to view the source slice from the "point of view" of
			// the target localizer, i.e. a parallel projection of the source
			// onto the target

			// do this by imagining that the target localizer is a view port
			// into a relocated and rotated co-ordinate space, where the
			// viewport has a row vector of +X, col vector of +Y and normal +Z,
			// then the X and Y values of the projected target correspond to
			// col and row offsets in mm from the TLHC of the localizer image !

			// move everything to origin of target

//cerr << "original pos_x = " << pos_x[i] << endl;
//cerr << "original pos_y = " << pos_y[i] << endl;
//cerr << "original pos_z = " << pos_z[i] << endl;

			pos_x[i] -= dst_pos_x;
			pos_y[i] -= dst_pos_y;
			pos_z[i] -= dst_pos_z;

//cerr << "reoriginate src_pos_x = " << pos_x[i] << endl;
//cerr << "reoriginate src_pos_y = " << pos_y[i] << endl;
//cerr << "reoriginate src_pos_z = " << pos_z[i] << endl;

			// The rotation is easy ... just rotate by the row, col and normal
			// vectors ...

			rotate( dst_row_dircos_x,dst_row_dircos_y,dst_row_dircos_z,
				dst_col_dircos_x,dst_col_dircos_y,dst_col_dircos_z,
				dst_nrm_dircos_x,dst_nrm_dircos_y,dst_nrm_dircos_z,
				pos_x[i],pos_y[i],pos_z[i],
				pos_x[i],pos_y[i],pos_z[i]);

//cerr << "\t\t After rotate, pos_x = " << pos_x[i] << endl;
//cerr << "\t\t After rotate, pos_y = " << pos_y[i] << endl;
//cerr << "\t\t After rotate, pos_z = " << pos_z[i] << endl;

			// DICOM coordinates are center of pixel 1\1

			col_pixel[i] = int(pos_x[i]/dst_col_spacing + 0.5);
			row_pixel[i] = int(pos_y[i]/dst_row_spacing + 0.5);

//cerr << "Col offset = " << pos_x[i] << " (mm)\t " << col_pixel[i] << " (pixels)" << endl;
//cerr << "Row offset = " << pos_y[i] << " (mm)\t " << row_pixel[i] << " (pixels)" << endl;
		}

		// draw the trapezoid (will repeatedly draw the same line if orthogonal) ...

		drawLine(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,
			row_pixel[0],col_pixel[0],row_pixel[1],col_pixel[1]);
		drawLine(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,
			row_pixel[1],col_pixel[1],row_pixel[2],col_pixel[2]);
		drawLine(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,
			row_pixel[2],col_pixel[2],row_pixel[3],col_pixel[3]);
		drawLine(graphics_buffer,graphics_buffer_rows,graphics_buffer_cols,
			row_pixel[3],col_pixel[3],row_pixel[0],col_pixel[0]);
	}

	unsigned u=0x0000;	// Use Overlay Group 0x6000 ...

	localizer_list+=new UnsignedShortAttribute(Tag(OverlayRows_GROUP+u,OverlayRows_ELEMENT),graphics_buffer_rows);
	localizer_list+=new UnsignedShortAttribute(Tag(OverlayColumns_GROUP+u,OverlayColumns_ELEMENT),graphics_buffer_cols);
	localizer_list+=new CodeStringAttribute(Tag(OverlayType_GROUP+u,OverlayType_ELEMENT),"G");
	localizer_list+=new UnsignedShortAttribute(Tag(OverlayBitsAllocated_GROUP+u,OverlayBitsAllocated_ELEMENT),1);
	localizer_list+=new UnsignedShortAttribute(Tag(OverlayBitPosition_GROUP+u,OverlayBitPosition_ELEMENT),0);

	SignedShortAttribute *aOverlayOrigin = new SignedShortAttribute(Tag(OverlayOrigin_GROUP+u,OverlayOrigin_ELEMENT));
	Assert(aOverlayOrigin);
	aOverlayOrigin->addValue(Int16(1));
	aOverlayOrigin->addValue(Int16(1));
	localizer_list+=aOverlayOrigin;

	OtherWordSmallNonPixelAttribute *aOverlayData =
		new OtherWordSmallNonPixelAttribute(Tag(OverlayData_GROUP+u,OverlayData_ELEMENT));
	Assert(aOverlayData);
	aOverlayData->setValue(graphics_buffer,graphics_buffer_size);
	localizer_list+=aOverlayData;

	if (!usualManagedAttributeListWrite(localizer_list,dout,
		dicom_output_options,log,veryverbose)) success=false;

	//if (numberofinputfiles && filenamestable) delete[] filenamestable;
	//if (localizer_din) delete localizer_din;

	return success ? 0 : 1;
}

	
