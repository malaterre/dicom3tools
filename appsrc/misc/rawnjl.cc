#include "bnstream.h"
#include "bnopt.h"
#include "mesgtext.h"

// Test of new lossless JPEG proposal ISO/IEC WD 14495

static inline Uint32 Maximum(Uint32 a,Uint32 b) { return (a > b) ? a : b; }
static inline Uint16 Maximum(Uint16 a,Uint16 b) { return (a > b) ? a : b; }
static inline Uint32 Minimum(Uint32 a,Uint32 b) { return (a > b) ? b : a; }
static inline Uint16 Minimum(Uint16 a,Uint16 b) { return (a > b) ? b : a; }
static inline Uint32 Abs(Int32 x) { return (Uint32)((x < 0) ? -x : x); }
static inline Uint16 Abs(Int16 x) { return (Uint16)((x < 0) ? -x : x); }

static Uint32
readRow(BinaryInputStream &in, Uint16 *buffer, Uint32 n,Uint16 bpp)
{
	Uint32 count=0;
	if (bpp <= 8) {
		while (n-- && in) {
			unsigned char value;
			in >> value;
			*buffer++=Uint16(value);
			if (in || in.eof()) ++count;
		}
	}
	else {
		while (n-- && in) {
			in >> *buffer++;
			if (in || in.eof()) ++count;
		}
	}
	return (in || in.eof()) ? count : 0;
}

static BinaryOutputStream &
writeRow(BinaryOutputStream &out, Uint16 *buffer, Uint32 n,Uint16 bpp)
{
	if (bpp <= 8) {
		while (n-- && out) out << (unsigned char)(*buffer++);
	}
	else {
		while (n-- && out) out << *buffer++;
	}
	return out;
}

static Uint32 		readBitByteOffset=0;
static Int16 		readBitCount=0;
static unsigned char 	readBitByte=0;

static BinaryInputStream &
readBit(BinaryInputStream &in,Uint32 &bit)
{
	// first bits are read from msb of byte
	Assert(readBitCount >= 0);
	if (readBitCount < 1) {
		++readBitByteOffset;
		in.read((char *)&readBitByte,1);
		readBitCount=8;
	}
	bit=(readBitByte>>(--readBitCount)) & 1;
//cerr << (bit ? "1" : "0");
	return in;
}

static void
dumpReadBitPosition(void)
{
	cerr << "dumpReadBitPosition: " << (readBitByteOffset-1+(readBitCount ? 0 : 1)) << "." << (readBitCount ? (8-readBitCount) : 0) << endl;
}

static Uint32 		writeBitByteOffset=0;
static Uint16 		writeBitCount=0;
static unsigned char 	writeBitByte=0;

static BinaryOutputStream &
writeBit(BinaryOutputStream &out,Uint32 bit)
{
	// first bits are written into msb of byte
	Assert(writeBitCount<8);
	writeBitByte=writeBitByte<<1;
	if (bit) writeBitByte|=1;
	if (++writeBitCount >= 8) {
		++writeBitByteOffset;
		out.write((char *)&writeBitByte,1);
		writeBitByte=0;
		writeBitCount=0;
	}
//cerr << (bit ? "1" : "0");
	return out;
}

static BinaryOutputStream &
writeBitFlush(BinaryOutputStream &out)
{
	Assert(writeBitCount<8);
	writeBitByte=writeBitByte<<(8-writeBitCount);
	out.write((char *)&writeBitByte,1);
	writeBitByte=0;
	writeBitCount=0;
	return out;
}

static void
dumpWriteBitPosition(void)
{
	cerr << "dumpWriteBitPosition: " << writeBitByteOffset << "." << writeBitCount << endl;
}

int
main(int argc,char **argv)
{
	bool bad=false;

	GetNamedOptions				options(argc,argv);
	BinaryInputOptionsWithByteOrder		input_options(options);
	BinaryOutputOptionsWithByteOrder	output_options(options);

	unsigned rows=0;
	if (!options.get("rows",rows) && !options.get("height",rows) && !options.get("h",rows)) {
		//cerr << EMsgDC(NeedOption) << " - rows" << endl;
		bad=true;
	}
	unsigned cols=0;
	if (!options.get("columns",cols) && !options.get("width",cols) && !options.get("w",cols)) {
		//cerr << EMsgDC(NeedOption) << " - columns" << endl;
		bad=true;
	}
	unsigned bits=0;
	if (!options.get("bits",bits) && !options.get("depth",bits)) {
		//cerr << EMsgDC(NeedOption) << " - bits" << endl;
		bad=true;
	}

	Assert(bits <= 16);

	bool decompressing=options.get("d") || options.get("decompress");
	bool sendFirstValue=options.get("first");
	bool useRunMode=options.get("run");

	input_options.done();
	output_options.done();
	options.done();

	BinaryInputOpenerFromOptions input_opener(
		options,input_options.filename,cin);
	BinaryOutputOpenerFromOptions output_opener(
		options,output_options.filename,cout);

	//cerr << input_options.errors();
	//cerr << output_options.errors();
	//cerr << options.errors();
	//cerr << input_opener.errors();
	//cerr << output_opener.errors();

	if (!input_options.good()
	 || !output_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< input_options.usage()
			<< output_options.usage()
			<< " [-d|decompress]"
			<< " [-first]"
			<< " [-run]"
			<< " -rows|height|h n"
			<< " -columns|width|w n"
			<< " -bits|depth n"
			<< " [" << MMsgDC(InputFile)
				<< "[" << MMsgDC(OutputFile) << "]]"
			<< " <" << MMsgDC(InputFile)
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	BinaryInputStream in(*(istream *)input_opener,input_options.byteorder);
	BinaryOutputStream out(*(ostream *)output_opener,output_options.byteorder);

	bool success=true;

	// Fixed things

	const Uint16 nContexts = 1215;	// Supposed to be 1092 but set quantization :(

	// Set specified parameters (would be in syntax)

	Uint32 ROWS = rows;
	Uint32 COLUMNS = cols;
	Uint16 bpp = bits;

	Uint32 Max = (1l<<bpp)-1;	// maximum possible value

//cerr << "Max = " << Max << endl;

	Uint16 backGroundValue = 0;

//cerr << "backGroundValue = " << backGroundValue << endl;

	Uint16 RESET = 64;	// The value at which A, B, and N are halved
	Uint16 near = 0;	// Lossless

	// Thresholds for context gradients ...

	Uint16 T1 = 3+3*near;				// eg. 3  for lossless
	Uint16 T2 = 7+5*near;				// eg. 7  for lossless
	Uint16 T3 = 21+7*near;				// eg. 21 for lossless
	Uint16 T4 = Maximum(Uint16(5),(near+1));	// eg. 5  for lossless

	if (bits != 8) {
		Uint16 multipler8=1<<(bpp-8);		// eg. 256  for 16 bit
		Uint16 multipler9=1<<(bpp-9);		// eg. 128  for 16 bit
		T1=multipler8*(T1-2)+2;			// eg. 258  for 16 bit
		T2=multipler8*(T2-3)+3;			// eg. 1027 for 16 bit
		T3=multipler8*(T3-4)+4;			// eg. 4356 for 16 bit
		T4=Maximum(5u,multipler9);		// eg. 128  for 16 bit
	}

//cerr << "T1 = " << T1 << endl;
//cerr << "T2 = " << T2 << endl;
//cerr << "T3 = " << T3 << endl;
//cerr << "T4 = " << T4 << endl;

	Int32 MIN_C = -128;	// Limits on values in bias correction array C
	Int32 MAX_C =  127;


	// Initialization of variables ...

	Uint32	*A = new Uint32[nContexts];	// accumulated prediction error magnitude [0..nContexts-1]
	Int32	*B = new Int32[nContexts];	// auxilliary counters for bias cancellation [0..nContexts-1]
	Int32	*C = new Int32[nContexts];	// counters indicating bias correction value [0..nContexts-1]
	Int32	*N = new Int32[nContexts];	// counters for context type occurence [0..nContexts-1]
						// (never -ve but often used as -N[Q] so int not unsigned saves cast)

	Assert(A);
	Assert(B);
	Assert(C);
	Assert(N);
	{
		unsigned i;
		for (i=0; i<nContexts; ++i) {
			A[i]=4;
			B[i]=C[i]=0;
			N[i]=1;
		}
	}

	// Range of prediction error representation ...
	Int32 MODULO = (Max+2*near)/(2*near+1)+1;	// for lossless, is just Max+1
							// int not unsigned to avoid need for cast when used

//cerr << "MODULO = " << MODULO << endl;

	if (decompressing) {
		if (sendFirstValue) {
			in >> backGroundValue;
//cerr << "backGroundValue = " << backGroundValue << endl;
		}
	}

	// The run variables seem to need to live beyond a single run or row !!!

	unsigned RUNIndex = 0;

	static Uint16 J[32] = {		// Order of run length codes
		0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,6,6,7,7,8,9,10,11,12,13,14,15 };

	Int32	EA[2]  = { 4,4 };	// Accummulated prediction error at end of run
	Uint32	EN[2]  = { 1,1 };	// Occurrence counters for end of run
	Int32	ENn[2] = { 0,0 };	// Negative prediction error for end of run

//dumpReadBitPosition();
//dumpWriteBitPosition();

	Uint16 *rowA = new Uint16[COLUMNS];
	Uint16 *rowB = new Uint16[COLUMNS];
	Assert(rowA);
	Assert(rowB);

	Uint32 row=0;
	Uint16 *thisRow=rowA;
	Uint16 *prevRow=rowB;
	for (row=0; row<ROWS; ++row) {
//cerr << "Row " << row << endl;
//bool traceOn = (row >= 84 && row <= 86);
//bool traceOn = true;
//if (decompressing)
//	dumpReadBitPosition();
//else
//	dumpWriteBitPosition();
		if (!decompressing)  {
			Uint32 n=readRow(in,thisRow,COLUMNS,bpp);
			Assert (n==COLUMNS);
//cerr << "Row " << row << " read returns " << n << endl;
			if (sendFirstValue && row == 0) {
				backGroundValue=thisRow[0];
				out << backGroundValue;
//cerr << "backGroundValue = " << backGroundValue << endl;
			}
		}

		Uint32 col=0;
		for (col=0; col<COLUMNS; ++col) {

//cerr << "\tcol = " << col << endl;

			//	. c a d .
			//	e b x . .
			//	. . . . .

			Int32 Rx;	// not Uint16 to allow overrange before clamping
			if (!decompressing) {
				Rx = thisRow[col];
//cerr << "\t\tRx = " << Rx << endl;
			}
			Uint16 Rb = (col > 0)                    ? thisRow[col-1] : backGroundValue;
			Uint16 Re = (col > 1)                    ? thisRow[col-2] : backGroundValue;
			Uint16 Rc = (col > 0 && row > 0)         ? prevRow[col-1] : backGroundValue;
			Uint16 Ra = (row > 0)                    ? prevRow[col]   : backGroundValue;
			Uint16 Rd = (col+1 < COLUMNS && row > 0) ? prevRow[col+1] : backGroundValue;

//cerr << "\t\tRa = " << Ra << endl;
//cerr << "\t\tRb = " << Rb << endl;
//cerr << "\t\tRc = " << Rc << endl;
//cerr << "\t\tRd = " << Rd << endl;
//cerr << "\t\tRe = " << Re << endl;

			// NB. We want the Reconstructed values, which are the same
			// in lossless mode, but if near != 0 take care to write back
			// reconstructed values into the row buffers in previous positions

			// Compute local gradient ...

			Int32 D1=(Int32)Rd-Ra;
			Int32 D2=(Int32)Ra-Rc;
			Int32 D3=(Int32)Rc-Rb;
			Int32 D4=(Int32)Rb-Re;

//cerr << "\t\tD1 = " << D1 << endl;
//cerr << "\t\tD2 = " << D2 << endl;
//cerr << "\t\tD3 = " << D3 << endl;
//cerr << "\t\tD4 = " << D4 << endl;

			// Check for run mode ... (should check Abs() works ok for Int32)

			if (useRunMode && Abs(D1) <= near && Abs(D2) <= near && Abs(D3) <= near) {
				// Run mode

//cerr << "Row at run start " << row << endl;
//cerr << "\tcol at run start " << col << endl;
				if (decompressing) {
//dumpReadBitPosition();
					// Why is RUNIndex not reset to 0 here ?
					Uint32 R;
					while (readBit(in,R)) {
//cerr << "\tcol " << col << endl;
						if (R == 0) {
//cerr << "0" << endl;
							Int32 rm=1<<J[RUNIndex];
//cerr << "\tRUNIndex " << RUNIndex << endl;
//cerr << "\trm " << rm << endl;
							while (rm-- && col < COLUMNS) thisRow[col++]=Rb;
							// This will match when exact count coincides with end of row ...
							if (rm == -1 && RUNIndex < 32) {
								++RUNIndex;
//cerr << "\tRUNIndex incremented to " << RUNIndex << endl;
							}
							if (col >= COLUMNS) {
//cerr << "\tFilled to end of row" << endl;
//cerr << "\tAfter having found end of row " << endl;
//dumpReadBitPosition();
								break;
							}
						}
						else {
//cerr << "1" << endl;
							// Read J[RUNIndex] bits
							Uint16 bits=J[RUNIndex];
//cerr << "\tRUNIndex " << RUNIndex << endl;
//cerr << "\tReading bits " << bits << endl;
							Uint32 nfill=0;
							Uint32 bit;
							// msb bit is read first
							while (bits-- && readBit(in,bit)) {
//cerr << (bit ? "1" : "0") << endl;
								nfill=(nfill<<1) | bit;
							}
//cerr << "\tFill with " << nfill << endl;
							// Fill with nfill values of Rb
							while (nfill--) {
//if (!(col<(COLUMNS-1))) {
//	cerr << "Fail at line 367 ... !(col<(COLUMNS-1))" << endl;
//	cerr << "\tstill to fill " << nfill+1 << endl;
//	cerr << "\trow is " << row << endl;
//	cerr << "\tcol is " << col << endl;
//}
								Assert(col<(COLUMNS-1));
								thisRow[col++]=Rb;
							}
							if (RUNIndex > 0) {
								--RUNIndex;
//cerr << "\tRUNIndex decremented to " << RUNIndex << endl;
							}

							// Decode the end of run value
//cerr << "\tcol at end of run " << col << endl;
//cerr << "\tBefore decoding value that ends run " << endl;
//dumpReadBitPosition();
							{
								Uint16 bits=16;
								Uint16 value=0;
								Uint32 bit;
								// msb bit is read first
								while (bits-- && readBit(in,bit)) value=(value<<1) | bit;
//cerr << "\tValue that ends run " << value << endl;
								Assert(col<COLUMNS);
								thisRow[col]=value;
							}
//cerr << "\tBefore decoding value that ends run " << endl;
//dumpReadBitPosition();
							break;
						}
					}
				}
				else {
//dumpWriteBitPosition();
					// Scan to determine length of run ...
//cerr << "\tRb is " << Rb << endl;
					Int32 RUNCnt=0;
					while (col < COLUMNS
					    && (thisRow[col] == Rb || (near > 0 && Abs(Int32(thisRow[col])-Int32(Rb)) <= near) )
					) {
						++RUNCnt;
						if (near > 0) thisRow[col]=Rb;
						++col;
					}
//cerr << "\tRUNCnt " << RUNCnt << endl;
					// Encode length of run ...
					// NB. Could really precompute so-called "rm" rather than "rk" values in table :(
					Uint32 rm;
					// Why is RUNIndex not reset to 0 here ?
					while (RUNCnt >= (rm=1<<J[RUNIndex])) {
//cerr << "\tRUNIndex " << RUNIndex << endl;
//cerr << "\trm " << rm << endl;
						writeBit(out,0);
//cerr << "0" << endl;
						RUNCnt-=rm;
//cerr << "\tRUNCnt decremented to " << RUNCnt << endl;
						Assert(RUNCnt >= 0);	// is why int not unsigned
						if (RUNIndex < 32) {
							++RUNIndex;
//cerr << "\tRUNIndex incremented to " << RUNIndex << endl;
						}
					}

					if (col < COLUMNS) {	// Must have been terminated by different value
								// the draft text and figure seem to conflict on how to
								// test for end of row ... let's use col instead
//cerr << "\tcol " << col << endl;
//cerr << "\tDifferent value is " << thisRow[col] << endl;
						writeBit(out,1);
//cerr << "1" << endl;
						// Append least significant J[RUNIndex] bits
						Uint16 bits=J[RUNIndex];
						Uint32 value=RUNCnt;
						Assert(value < (1<<J[RUNIndex]));	// Does it really fit in this ? why should it ??
//cerr << "\tRemaining RUNCnt " << RUNCnt << endl;
//cerr << "\tEncoding in bits " << bits << endl;
						// msb bit is written first
						while (bits--) {
							Uint32 bit=(value>>bits)&1;
							writeBit(out,bit);  // use the decremented bits as shift
//cerr << (bit ? "1" : "0") << endl;
						}

						if (RUNIndex > 0) {
							--RUNIndex;
//cerr << "\tRUNIndex decremented to " << RUNIndex << endl;
						}

						// Encode the end of run value
//cerr << "\tcol at end of run " << col << endl;
						Assert(col<COLUMNS);
//cerr << "\tBefore encoding value that ends run " << endl;
//dumpWriteBitPosition();
//cerr << "\tValue that ends run " << thisRow[col] << endl;
						{
							Uint16 bits=16;
							// msb bit is written first
							while (bits--) {
								writeBit(out,(thisRow[col]>>bits)&1);
							}
						}
//cerr << "\tAfter encoding value that ends run " << endl;
//dumpWriteBitPosition();
					}
					else {						// Aborted at end of row
//cerr << "\tEnd of row" << endl;
						if (RUNCnt > 0) {
							writeBit(out,0);		// Append an extra 0
											// decoder knows to stop at end of row
											// though remainder can't be > 1<<J[RUNIndex]
//cerr << "0 for end of row" << endl;
//cerr << "\tAfter indicating end of row " << endl;
//dumpWriteBitPosition();
						}
					}
				}

			}
			else
				{

				// Regular mode

				// Gradient quantization ...

				Int16 Q1,Q2,Q3,Q4;

				if      (D1 <= -T3)   Q1=-4;
				else if (D1 <= -T2)   Q1=-3;
				else if (D1 <= -T1)   Q1=-2;
				else if (D1 <  -near) Q1=-1;
				else if (D1 <=  near) Q1= 0;
				else if (D1 <   T1)   Q1= 1;
				else if (D1 <   T2)   Q1= 2;
				else if (D1 <   T3)   Q1= 3;
				else                  Q1= 4;

				if      (D2 <= -T3)   Q2=-4;
				else if (D2 <= -T2)   Q2=-3;
				else if (D2 <= -T1)   Q2=-2;
				else if (D2 <  -near) Q2=-1;
				else if (D2 <=  near) Q2= 0;
				else if (D2 <   T1)   Q2= 1;
				else if (D2 <   T2)   Q2= 2;
				else if (D2 <   T3)   Q2= 3;
				else                  Q2= 4;

				if      (D3 <= -T3)   Q3=-4;
				else if (D3 <= -T2)   Q3=-3;
				else if (D3 <= -T1)   Q3=-2;
				else if (D3 <  -near) Q3=-1;
				else if (D3 <=  near) Q3= 0;
				else if (D3 <   T1)   Q3= 1;
				else if (D3 <   T2)   Q3= 2;
				else if (D3 <   T3)   Q3= 3;
				else                  Q3= 4;

				if      (D4 <= -T4)   Q4=-1;
				else if (D4 <   T4)   Q4= 0;
				else                  Q4= 1;

//cerr << "\t\tQ1 = " << Q1 << endl;
//cerr << "\t\tQ2 = " << Q2 << endl;
//cerr << "\t\tQ3 = " << Q3 << endl;
//cerr << "\t\tQ4 = " << Q4 << endl;

				// Context merging and determination of sign ...

				// Note that there are 3 missings contexts because of Run mode,
				// ie. Q1 == 0 && Q2 == 0 && Q3 == 0 and Q4 == anything (-1,0,+1)

				Int16 sign;

				// "If the 1st non-zero component of vector (Q1,Q2,Q3) is negative" ...

				if ( Q1 < 0
				 || (Q1 == 0 && Q2 < 0)
				 || (Q1 == 0 && Q2 == 0 && Q3 < 0) ) {
					Q1=-Q1;
					Q2=-Q2;
					Q3=-Q3;
					Q4=-Q4;
					sign=-1;	// signifies -ve
				}
				else {
					sign=1;		// signifies +ve
				}

//cerr << "\t\tsign= " << sign << endl;

//cerr << "\t\tQ1 after sign = " << Q1 << endl;
//cerr << "\t\tQ2 after sign = " << Q2 << endl;
//cerr << "\t\tQ3 after sign = " << Q3 << endl;
//cerr << "\t\tQ4 after sign = " << Q4 << endl;

				// Q1 therefore always becomes +ve or is already 0

				// The derivation of Q is not specified in the current draft :(

				Uint16 Q = Q1*243 + (Q2+4)*27 + (Q3+4)*3 + (Q4+1);	// 0..1091 supposed to be
				//         0..4      0..8        0..8       0..2	// 0..1214 ?

//cerr << "\t\tQ itself = " << Q << endl;

if (Q >= nContexts) {
	//cerr << "\t\tQ1 after sign = " << Q1 << endl;
	//cerr << "\t\tQ2 after sign = " << Q2 << endl;
	//cerr << "\t\tQ3 after sign = " << Q3 << endl;
	//cerr << "\t\tQ4 after sign = " << Q4 << endl;
	//cerr << "\t\tQ itself = " << Q << endl;
}
				Assert(Q<nContexts);	// Just in case

				// let's just forget the run contexts and go with 1215 for now :(

				// Edge detecting predictor ...

				Int32 Px;	// not Uint16 to allow overrange before clamping

				if      (Rc >= Maximum(Ra,Rb)) Px = Minimum(Ra,Rb);
				else if (Rc <= Minimum(Ra,Rb)) Px = Maximum(Ra,Rb);
				else			   Px = Ra+Rb-Rc;

//cerr << "\t\tPx = " << Px << endl;

				// Prediction correction and clamping ...

				Px = Px + ((sign > 0) ? C[Q] : -C[Q]);

//cerr << "\t\tC[Q] = " << C[Q] << endl;
//cerr << "\t\tPx corrected = " << Px << endl;

				if      (Px > (Int32)Max) Px=Max;	// wanted signed comparison
				else if (Px < 0)   Px=0;

				// Compute prediction error ...
//cerr << "\t\tPx clamped = " << Px << endl;


				// Prediction error encoding and decoding...

				// Golomb encoding ...

				// Determine Golomb parameter k

				Uint16 k;

//cerr << "\t\tN[Q] = " << N[Q] << endl;
//cerr << "\t\tA[Q] = " << A[Q] << endl;

				Assert(N[Q]);				// Make sure we don't get out of control
				for (k=0;(N[Q]<<k) < A[Q]; ++k)		// Number of occurrences vs accumulated error magnitude
					Assert(k<32);	// not in draft, but need limit else possible infinite loop if A[Q] really big

//cerr << "\t\tk = " << k << endl;

				Uint32 Merror;
				Int32 error;

				if (decompressing) {
					// Decode Golomb mapped error from input...

					{
						// Read least significant k bits
						Uint16 bits=k;
						Uint32 lsvalue=0;
						Uint32 bit;
						// msb bit is read first
//cerr << "\t\t" << bits << " k bits" << endl;
						while (bits-- && readBit(in,bit)) lsvalue=(lsvalue<<1) | bit;
//cerr << ",";
						// Read unary representation of remaining most significant bits
						Uint32 msvalue=0;
						while (readBit(in,bit) && !bit) ++msvalue;	// Stops after 1 bit
//cerr << "\t\t" << msvalue << " msb bits" << endl;
						Merror=msvalue<<k | lsvalue;
					}

//cerr << "\t\tMerror = " << Merror << endl;

					// Unmap error from non-negative ...

					// "In the near-lossless mode ... is independent of k." text of A.8.2
					// conflicts with figure A.2 ... go with the figure.

					if (near > 0 && k == 0 && B[Q] <= -N[Q]/2) {
						if (Merror%2 != 0)
							error=((Int32)Merror-1)/2;	//  1 becomes  0,  3 becomes  1,  5 becomes  2
						else
							error=-(Int32)Merror/2 - 1;	//  0 becomes -1,  2 becomes -2,  4 becomes -3
					}
					else {
						if (Merror%2 == 0)
							error=(Int32)Merror/2;		//  0 becomes  0, 2 becomes  1,  4 becomes  2
						else
							error=-((Int32)Merror + 1)/2;	//  1 becomes -1, 3 becomes -2
					}

					// Dequantize the error if in near-lossless mode ...

//cerr << "\t\terror undequantized = " << error << endl;

					if (near) error=error*(2*near+1);

//cerr << "\t\terror sign uncorrected = " << error << endl;

					if (sign < 0) error=-error;		// if "context type" was negative

//cerr << "\t\terror result = " << error << endl;

					Rx=(Px+error)%(MODULO*(2*near+1));	// watch this for bad unsigned->signed conversion :(
										// ? should be sign * error ??? :(
					if      (Rx > Max) Rx=Max;
					else if (Rx < 0)   Rx=0;

					thisRow[col]=(Uint16)Rx;
				}
				else {
					error = Rx - Px;		// watch this for bad unsigned->signed conversion :(

//cerr << "\t\terror start = " << error << endl;

					if (sign < 0) error=-error;	// if "context type" was negative

//cerr << "\t\terror sign corrected = " << error << endl;

					// Quantize the error if in near-lossless mode ...

					if (near) {
						if (error > 0)
							error=(error+near)/(2*near+1);
						else
							// error=(near-error)/(2*near+1);	// This must be wrong in A.7.4
												// as it doesn't match decompress
							error=(error-near)/(2*near+1);

						// Replace with the reconstructed value the decoder will have
						// (obviously if in lossless mode there will be no difference

						Int32 newRx=Px+sign*error*(2*near+1);
						if      (newRx > Max) newRx=Max;
						else if (newRx < 0)   newRx=0;
					
						thisRow[col]=Rx=(Uint16)newRx;
					}

//cerr << "\t\terror quantized = " << error << endl;

					// Modulo reduction of the prediction error

					error=error%MODULO;	// make very sure everything is signed !!
								// else generates huge errors :)

//cerr << "\t\terror modulo " << MODULO << " = " << error << endl;

					// Map error to non-negative ...

					// "In the near-lossless mode ... is independent of k." text of A.8.2
					// conflicts with figure A.2 ... go with the figure.

					if (near > 0 && k == 0 && B[Q] <= -N[Q]/2) {
						if (error >= 0)
							Merror =  2*error + 1;	//  0 becomes 1,  1 becomes 3,  2 becomes 5
						else
							Merror = -2*(error+1);	// -1 becomes 0, -2 becomes 2, -3 becomes 4
					}
					else {
						if (error >= 0)
							Merror =  2*error;	//  0 becomes 0,  1 becomes 2,  2 becomes 4
						else
							Merror = -2*error - 1;	// -1 becomes 1, -2 becomes 3
					}

//cerr << "\t\tMerror = " << Merror << endl;

					// Encode Golomb mapped error ...

					{
						// Append least significant k bits
						Uint16 bits=k;
//cerr << "\t\t" << bits << " k bits" << endl;
						Uint32 value=Merror;
						// msb bit is written first
						while (bits--) { writeBit(out,(value>>bits)&1); }  // use the decremented bits as shift
					}
//cerr << ",";
					{
						// Append unary representation of remaining most significant bits
						Uint32 value=Merror>>k;
//cerr << "\t\t" << value << " msb bits" << endl;
						while (value--) writeBit(out,0);
					}
					writeBit(out,1);	// Note that a 1 is always the last thing written
								// so for k=0 and e=0 nothing is written but 1
				}

//cerr << "\t\tMerror = " << Merror << endl;
//cerr << "\t\tRx = " << Rx << endl;

				// Update parameters ...

//cerr << "\t\tUse error (Rx - Px) = " << (Rx - Px) << endl;
//cerr << "\t\tA[Q] old = " << A[Q] << endl;
//cerr << "\t\tB[Q] old = " << B[Q] << endl;
//cerr << "\t\tC[Q] old = " << C[Q] << endl;
//cerr << "\t\tN[Q] old = " << N[Q] << endl;

				// don't use the value of error because it is different at
				// this point because of all the massaging ... recompute it
				// based on the reconstructed value that is shared between
				// both encoder and decoder

				B[Q]=B[Q]+sign*(Rx - Px)*(2*near+1);
				A[Q]=A[Q]+Abs(error);
				if (N[Q] == RESET) {
					A[Q]=A[Q]>>1;
					B[Q]=B[Q]>>1;
					N[Q]=N[Q]>>1;	// This isn't in the text at A.9.1 but seems necessary :(
				}
				++N[Q];

//cerr << "\t\tA[Q] updated = " << A[Q] << endl;
//cerr << "\t\tB[Q] updated = " << B[Q] << endl;
//cerr << "\t\tC[Q] updated = " << C[Q] << endl;
//cerr << "\t\tN[Q] updated = " << N[Q] << endl;

				// Context dependent bias cancellation ...

				if (B[Q] <= -N[Q]) {
					if (C[Q] > MIN_C) --C[Q];
					B[Q]+=N[Q];
					if (B[Q] <= -N[Q]) B[Q]=-N[Q]+1;
				}
				else if (B[Q] > 0) {
					if (C[Q] < MAX_C) ++C[Q];
					B[Q]-=N[Q];
					if (B[Q] > 0) B[Q]=0;
				}

//cerr << "\t\tA[Q] bias cancelled = " << A[Q] << endl;
//cerr << "\t\tB[Q] bias cancelled = " << B[Q] << endl;
//cerr << "\t\tC[Q] bias cancelled = " << C[Q] << endl;
//cerr << "\t\tN[Q] bias cancelled = " << N[Q] << endl;

			}
		}
		if (decompressing) {
			if (!writeRow(out,thisRow,COLUMNS,bpp)) Assert(0);
		}
		Uint16 *tmpRow=thisRow;
		thisRow=prevRow;
		prevRow=tmpRow;
	}

	if (!decompressing) writeBitFlush(out);

	if (rowA) delete[] rowA;
	if (rowB) delete[] rowB;
	if (A) delete[] A;
	if (B) delete[] B;
	if (C) delete[] C;
	if (N) delete[] N;

	return success ? 0 : 1;
}

