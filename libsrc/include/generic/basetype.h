#ifndef __Header_basetype__
#define __Header_basetype__

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#ifdef __SC__
#include <types.h>           // has an enum for false and true
#define	NOBOOLEANTYPE
#define NOINCLUDESUBDIRECTORIES
#endif /* __SC__ */

#ifdef __MWERKS__
#define NOBOOLEANTYPE
#define NOINCLUDESUBDIRECTORIES
#define ANSILIBRARYPUBSEEK
#endif /* __MWERKS__ */

#ifdef __BORLANDC__
#define	NOBOOLEANTYPE
#define	NOBOOLEANVALUES
#endif /* __BORLANDC__ */

#ifdef NOBOOLEANTYPE
typedef short int bool;
#endif /* NOBOOLEANTYPE */

#ifdef NOBOOLEANVALUES
const bool false = 0;
const bool true = 1;
#endif /* NOBOOLEANVALUES */

#if HASGETHOSTID == 0
static inline long gethostid(void) { return 0; }
#else
#if HASGETHOSTIDPROTOTYPE == 0
extern long gethostid(void);
#endif /* HASGETHOSTIDPROTOTYPE */
#endif /* HASGETHOSTID */

#ifdef ANSILIBRARYPUBSEEK
#ifndef PubSeekPos
#define PubSeekPos	pubseekpos
#endif
#ifndef PubSeekOff
#define PubSeekOff	pubseekoff
#endif
#else
#ifndef PubSeekPos
#define PubSeekPos	seekpos
#endif
#ifndef PubSeekOff
#define PubSeekOff	seekoff
#endif
#endif /* ANSILIBRARYPUBSEEK */

#ifndef PubSeekBad
#define PubSeekBad	OurStreamPos(-1)
#endif

#if HASSTDNAMESPACEFORSTREAMPOS == 1
#define OurStreamPos	std::streampos
#define OurStreamOff	std::streamoff
#define OurSeekDir	std::ios::seekdir
#else
#define OurStreamPos	streampos
#define OurStreamOff	streamoff
#define OurSeekDir	seek_dir
#endif

#include <assert.h>

#define Assert(expr)	assert(expr)	// In case we get more creative later !

// The following basic types for internal use, and
// must be at least long enough to represent the
// expected value range without loss of accuracy
// but may be longer.

// They should be distinct else stream insertors
// will be ambiguous

typedef unsigned char   Uint8;
typedef unsigned short	Uint16;
typedef unsigned long	Uint32;
typedef unsigned long long Uint64;
typedef signed char     Int8;
typedef signed short	Int16;
typedef signed long	Int32;
typedef signed long long Int64;
typedef float		Float32;
typedef double		Float64;

// These limits may not actually be true, but are desirable ...

#define	Uint8_MAX	 0xFF
#define	Uint16_MAX	 0xFFFF
#define	Uint32_MAX	 0xFFFFFFFF
#define	Int8_MIN	-0x80
#define	Int8_MAX	 0x7F
#define	Int16_MIN	-0x8000
#define	Int16_MAX	 0x7FFF
#define	Int32_MIN	-0x80000000
#define	Int32_MAX	 0x7FFFFFFF

#if defined(__GNUC__)  && __GNUC__ >= 3
#include <backward/strstream>
using std::istrstream;
using std::ostrstream;
#else
#include <strstream.h>
#endif

#include <math.h>

static inline double ourlog2(double x) { return log(x)/log(2); }

#endif /* __Header_basetype__ */
