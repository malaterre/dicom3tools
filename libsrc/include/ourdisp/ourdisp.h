#ifndef __Header_ourdisp__
#define __Header_ourdisp__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#if USEXMITSHMEXTENSION == 1
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

class OurDisplay : public ErrorsInClass {
	friend class OurWindow;
	friend class OurColorMap;

	char *		name;
	Display *	display;
	int 		screen;
	Window 		rootwindow;
	Visual *	visual;
	unsigned 	depth;
	int 		bitmappad;

	char * 		getName(void)		{ return name; }
	Display *	getDisplay(void)	{ return display; }
	int 		getScreen(void)		{ return screen; }
	Window 		getRootwindow(void)	{ return rootwindow; }
	Visual *	getVisual(void)		{ return visual; }
	unsigned 	getDepth(void)		{ return depth; }
	int 		getBitmapPad(void)	{ return bitmappad; }
public:
	OurDisplay(void);
	~OurDisplay();

	XEvent nextEvent(void);
};

class OurColorMap : public ErrorsInClass {
	friend class OurWindow;

	OurDisplay *	display;
	Colormap 	colormap;

	unsigned long *	cells;
	unsigned length;

	Colormap getColormap(void)	{ return colormap; }
public:
	OurColorMap(OurDisplay *d);
	~OurColorMap(void);

	bool setColorCellsWanted(unsigned nwanted,unsigned nminimum);
	bool getColorCellsAvailable(unsigned &n,unsigned long *&table);
	bool setColorCellValues(unsigned n,
		unsigned short *red,
		unsigned short *green,
		unsigned short *blue);
};

class OurWindow : public ErrorsInClass {
private:
	friend class OurWindowImage;

	OurDisplay *	ourdisplay;

	Window 		window;
	int 		x;
	int 		y;
	unsigned 	width;
	unsigned	imageheight;
	unsigned	totalheight;
	unsigned	headerheight;
	unsigned	footerheight;
	unsigned 	border;

	unsigned long	foreground;
	unsigned long	background;

	GC		reservedareagc;
	XFontStruct *	reservedareafont;

	Window 		getWindow(void)		{ return window; }
	int 		getX(void)		{ return x; }
	int 		getY(void)		{ return y; }
	unsigned 	getImageStartX(void)	{ return 0; }
	unsigned	getImageStartY(void)	{ return headerheight; }
	unsigned 	getImageWidth(void)	{ return width; }
	unsigned	getImageHeight(void)	{ return imageheight; }
	unsigned	getReservedRowsTop(void)
						{ return headerheight; }
	unsigned	getReservedRowsBottom(void)
						{ return footerheight; }
	unsigned 	getBorder(void)		{ return border; }

	unsigned long 	getForeground(void)	{ return foreground; }
	unsigned long 	getBackground(void)	{ return background; }

	char * 		getName(void)
				{ return ourdisplay->getName(); }
	Display *	getDisplay(void)
				{ return ourdisplay->getDisplay(); }
	int 		getScreen(void)
				{ return ourdisplay->getScreen(); }
	Window 		getRootwindow(void)
				{ return ourdisplay->getRootwindow(); }
	Visual *	getVisual(void)
				{ return ourdisplay->getVisual(); }
	unsigned 	getDepth(void)
				{ return ourdisplay->getDepth(); }
	int 		getBitmapPad(void)
				{ return ourdisplay->getBitmapPad(); }

	void writeTextSomewhere(int baseline,
			const char *left,const char *center,const char *right);
public:
	OurWindow(OurDisplay *d,
		unsigned w,unsigned h,
		unsigned headerh=0,unsigned footerh=0);
	~OurWindow(void);

	void writeHeaderText(
		const char *left,const char *center=0,const char *right=0);
	void writeFooterText(
		const char *left,const char *center=0,const char *right=0);
	void clearHeader(void);
	void clearFooter(void);

	void reportPosition(
			int x,int y,
			unsigned short &row,unsigned short &col) const;

	void useCursorLevelWidth(void);
	void useCursorCrossHair(void);
	void useCursorWatch(void);

	void start(void);
	void clear(void);
	void alarm(void);
	void setColormap(OurColorMap *colormap);

	void redraw(void);
};

class OurWindowImage : public ErrorsInClass {
	OurWindow *		window;
	XImage *		ximage;
	GC			imagegc;
#if USEXMITSHMEXTENSION == 1
	bool			shm_flag;
	XShmSegmentInfo *	xshminfo;
#endif
public:
	OurWindowImage(OurWindow *w);
	~OurWindowImage();

	void put(void);

	char * get8BitDataAddress(void);
	bool write8BitDataToStream(ostream &o);
	bool write8BitDataToPGMStream(ostream &o);

	void putPoint(unsigned x,unsigned y);

	unsigned getHeight(void)	{ return window->getImageHeight(); }
	unsigned getWidth(void)		{ return window->getImageWidth(); }
	unsigned getDepth(void)		{ return window->getDepth(); }
	unsigned getBytesPerRow(void)	{ return ximage->bytes_per_line; }
};

// NB. The OurWindowLevelWidthUpdator deals with an "internal" scale
// of window level and width, ie. disregarding the intercept and slope
// entirely and using the unsigned representation of what is in the
// pixel data.

// The one exception to this is displaying the values of the current
// width and level in the rserved area, in which case the sign,
// slop and intercept are used to translate into "real world" values 

class OurWindowLevelWidthUpdator {
private:
	long maplevelmin;
	long maplevelmax;
	long mapwidthmin;
	long mapwidthmax;
	long maplevel;
	long mapwidth;
	long signxor;
	long intercept;
	long slopeby1024;
	long lastx;
	long lasty;
	OurWindow *window;
public:
	OurWindowLevelWidthUpdator(OurWindow &w,Uint16 bits,bool issigned,
		double rescaleintercept,double rescaleslope);
	~OurWindowLevelWidthUpdator();

	void set(Uint16 level,Uint16 width);
	void get(Uint16 &level,Uint16 &width);

	Uint16 getStoredLevelFromDisplayedLevel(long level);
	Uint16 getStoredWidthFromDisplayedWidth(long width);
	long getDisplayedLevelFromStoredLevel(long level);
	long getDisplayedWidthFromStoredWidth(long width);

	void put(void);
	void press(long x,long y);
	void release(long x,long y);
	void move(long x,long y,long multiplier);
	void update(long x,long y,long multiplier);
};

#endif /* __Header_ourdisp__ */
