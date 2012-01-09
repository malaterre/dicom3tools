#include <iostream.h>
#include <iomanip.h>

#include "basetype.h"
#include "errclass.h"
#include "ourdisp.h"
#include "mesgtext.h"

OurDisplay::OurDisplay(void)
{
	name = NULL;
	if ((display=XOpenDisplay(name)) == NULL) {
		errorstream << EMsgDC(DisplayOpenFailed) << endl;
		good_flag=false;
	}
	else {
		screen     = DefaultScreen(display);
		rootwindow = RootWindow(display,screen);
		visual     = DefaultVisual(display,screen);
		depth      = DefaultDepth(display,screen);
		bitmappad  = XBitmapPad(display);
	}
}

OurDisplay::~OurDisplay()
{
	XCloseDisplay(display);
}

XEvent OurDisplay::nextEvent(void)
{
	XEvent report;
	XNextEvent(display,&report);
	return report;
}

OurColorMap::OurColorMap(OurDisplay *d)
{
	display=d;
	Assert(display);

	cells=0;
	length=0;
	colormap=(Colormap)None;
}

OurColorMap::~OurColorMap(void)
{
	if (colormap != (Colormap) None)
		XFreeColormap(
			display->getDisplay(),
			colormap
		);
	if (cells) delete[] cells;
}

bool
OurColorMap::setColorCellsWanted(unsigned nwanted,unsigned nminimum)
{
//cerr << "OurColorMap::setColorCellsWanted nwanted=" << dec << nwanted << endl;
//cerr << "OurColorMap::setColorCellsWanted nminimum=" << dec << nminimum << endl;
	Assert(!cells);
	cells = new unsigned long [nwanted];
	Assert(cells);

	// will always tried to get nwanted, but
	// will use default if can get nminimum
	// else will try creating private map

	// using the default is much preferrable because
	// of reduced flashing

	// not using XCopyColormapAndFree() from default
	// to private because assuming application has
	// not yet allocated any colors

	bool trieddefault=false;
	bool triedcreate=false;
	bool triedwithoutbw=false;

	XColor whitecolor;
	whitecolor.red=0xffff;
	whitecolor.green=0xffff;
	whitecolor.blue=0xffff;
	XColor blackcolor;
	blackcolor.red=0;
	blackcolor.green=0;
	blackcolor.blue=0;

	unsigned n=0;
	while (n < nminimum) {
//cerr << "OurColorMap::setColorCellsWanted loop start"
//     << " n=" << dec << n
//     << " nminimum=" << dec << nminimum
//     << endl;
		if (!trieddefault) {
//cerr << "OurColorMap::setColorCellsWanted try DefaultColormap" << endl;
			colormap = DefaultColormap(
				display->getDisplay(),
				display->getScreen()
			);
			trieddefault=true;
		}
		else if (!triedcreate) {
//cerr << "OurColorMap::setColorCellsWanted try XCreateColormap" << endl;
			colormap = XCreateColormap(
				display->getDisplay(),
				display->getRootwindow(),
				display->getVisual(),
				AllocNone
			);
			triedcreate=true;

			// make sure white & black are present in correct order,
			// otherwise writes in the reserved area won't work
			// (these will already be present in the default map

			if (!XAllocColor(display->getDisplay(),
				colormap,&whitecolor)
			 || !XAllocColor(display->getDisplay(),
				colormap,&blackcolor)
			) Assert(0);
		}
		else if (!triedwithoutbw) {
//cerr << "OurColorMap::setColorCellsWanted try XCreateColormap no bw" << endl;
			colormap = XCreateColormap(
				display->getDisplay(),
				display->getRootwindow(),
				display->getVisual(),
				AllocNone
			);
			triedwithoutbw=true;
			// Hope for the best re. black and white :(
		}
		else {
//cerr << "OurColorMap::setColorCellsWanted tries failed" << endl;
			break;
		}
		if (colormap == (Colormap) None) {
			errorstream << EMsgDC(ColorMapCreationFailed) << endl;
			continue;
		}

		n=nwanted;	// always optimistic
		while (n && !XAllocColorCells(
			display->getDisplay(),
			colormap,
			False,			// contiguous planes
			(unsigned long *) 0,	// plane masks
			0,			// # of planes
			cells,
			n)
		) --n;
//cerr << "OurColorMap::setColorCellsWanted loop done"
//     << " n=" << dec << n
//     << " nminimum=" << dec << nminimum
//     << endl;
	}

//cerr << "OurColorMap::setColorCellsWanted got=" << dec << n << endl;

	length=n;

	if (!n) {
		errorstream << EMsgDC(ColorCellAllocationFailedNone) << endl;
		good_flag=false;
	}
	else if (n < nminimum) {
		errorstream << EMsgDC(ColorCellAllocationFailedTooFew)
			    << " - " << n << " " << MMsgDC(Needed) << " "
			    << nminimum << "/" << nwanted << endl;
		good_flag=false;
	}
	return good_flag;
}

bool
OurColorMap::getColorCellsAvailable(unsigned &n,unsigned long *&table)
{
//cerr << "OurColorMap::getColorCellsAvailable length=" << dec << length << endl;
	if (length && cells) {
		n=length;
		table=cells;
	}
	else {
		n=0;
		table=0;
		good_flag=false;
	}
	return good_flag;
}

bool
OurColorMap::setColorCellValues(unsigned n,
	unsigned short *red,
	unsigned short *green,
	unsigned short *blue)
{
//cerr << "OurColorMap::setColorCellValues n=" << dec << n << endl;
	Assert(n == length);
	XColor c;
	c.flags=DoRed | DoGreen | DoBlue;
	unsigned i;
	for (i=0; i<length; ++i) {
		c.pixel	= cells[i];
		c.red	= red[i];
		c.green	= green[i];
		c.blue	= blue[i];
//cerr << "i=" << dec << i
//     << " pixel=" << dec << c.pixel
//     << " red=" << hex << c.red
//     << " green=" << hex << c.green
//     << " blue=" << hex << c.blue
//     << dec << endl;
		XStoreColor(
			display->getDisplay(),
			colormap,
			&c);
	}
	return true;
}

OurWindow::OurWindow(OurDisplay *d,
	unsigned w,unsigned h,
	unsigned headerh,unsigned footerh)
{
	ourdisplay=d;

	x=0;
	y=0;
	width=w;
	border=4;

	headerheight=headerh;
	footerheight=footerh;

	imageheight=h;
	totalheight=imageheight+headerheight+footerheight;

	foreground=WhitePixel(getDisplay(),getScreen());
	background=BlackPixel(getDisplay(),getScreen());

	window = XCreateSimpleWindow(
		getDisplay(),
		getRootwindow(),
		x,y,width,totalheight,
		border,
		foreground,
		background
	);

	reservedareagc = XCreateGC(getDisplay(),window,0,NULL);
	XSetForeground(getDisplay(),reservedareagc,foreground);
	XSetBackground(getDisplay(),reservedareagc,background);
	reservedareafont=XQueryFont(getDisplay(),XGContextFromGC(reservedareagc));
	Assert(reservedareafont);

	clearHeader();
	clearFooter();
}

OurWindow::~OurWindow(void)
{
	XDestroyWindow(
		getDisplay(),
		window
	);
}

void
OurWindow::start(void)
{
	XSelectInput(getDisplay(),window,
		ExposureMask
		|ButtonPressMask|ButtonReleaseMask
		|Button1MotionMask
		|PointerMotionMask
		|KeyPressMask);
	XMapWindow(getDisplay(),window);
}

void
OurWindow::clear(void)
{
	XClearWindow(getDisplay(),window);
}

void
OurWindow::alarm(void)
{
	XBell(getDisplay(),100);
	XFlush(getDisplay());
}

void
OurWindow::setColormap(OurColorMap *ourcolormap)
{
	XSetWindowColormap(
		getDisplay(),
		getWindow(),
		ourcolormap->getColormap()
	);

	// these may have changed ... it may be worth searching
	// the new colormap to find the best choices ...
	// (the default choices here will probably not work if
	// the default colormap has been replaced :( )

	foreground=WhitePixel(getDisplay(),getScreen());
	background=BlackPixel(getDisplay(),getScreen());

	XSetForeground(getDisplay(),reservedareagc,foreground);
	XSetBackground(getDisplay(),reservedareagc,background);
}

void
OurWindow::writeTextSomewhere(int baseline,
	const char *left,const char *center,const char *right)
{
//cerr << "write at =" << baseline << endl;

	int length;
	if (left && (length=strlen(left))) {
		XDrawImageString(getDisplay(),window,reservedareagc,
			width/20,baseline,left,length);
	}
	if (center && (length=strlen(center))) {
		int textwidth=XTextWidth(reservedareafont,center,length);
		int offset=textwidth < width ? (width-textwidth)/2 : 0;
		XDrawImageString(getDisplay(),window,reservedareagc,
			offset,baseline,center,length);
	}
	if (right && (length=strlen(right))) {
		int textwidth=XTextWidth(reservedareafont,right,length);
		int offset=textwidth < width-width/20 ? width-width/20-textwidth : 0;
		XDrawImageString(getDisplay(),window,reservedareagc,
			offset,baseline,right,length);
	}
	XFlush(getDisplay());
}

void
OurWindow::writeHeaderText(
	const char *left,const char *center,const char *right)
{
//cerr << "fid=" << reservedareafont->fid << endl;
//cerr << "ascent=" << reservedareafont->ascent << endl;
//cerr << "descent=" << reservedareafont->descent << endl;

	int baseline=(headerheight
			+reservedareafont->ascent
			+reservedareafont->descent)/2+1;

	//int baseline=headerheight-reservedareafont->descent+1;

	writeTextSomewhere(baseline,left,center,right);
}

void
OurWindow::writeFooterText(
	const char *left,const char *center,const char *right)
{
//cerr << "fid=" << reservedareafont->fid << endl;
//cerr << "ascent=" << reservedareafont->ascent << endl;
//cerr << "descent=" << reservedareafont->descent << endl;

	int baseline=headerheight+imageheight
	            +(footerheight
			+reservedareafont->ascent
			+reservedareafont->descent)/2
		    +1;

	//int baseline=totalheight-reservedareafont->descent+1;

	writeTextSomewhere(baseline,left,center,right);
}

void
OurWindow::clearHeader(void)
{
	if (headerheight) {
		XClearArea(getDisplay(),window,
			0,0,
			width,headerheight,
			False // No exposure event generated
		);
		XFlush(getDisplay());
	}
}

void
OurWindow::clearFooter(void)
{
	if (footerheight) {
		XClearArea(getDisplay(),window,
			0,imageheight+headerheight,
			width,footerheight,
			False // No exposure event generated
		);
		XFlush(getDisplay());
	}
}

void
OurWindow::reportPosition(
	int x,int y,
	unsigned short &row,unsigned short &col) const
{
	int r=y-headerheight;
	if (r < 0) r=0;
	else if (r>=imageheight) r=imageheight-1;

	row=(unsigned short)r;
	col=(unsigned short)x;
}

#include "wlcursor.cbm"
#include "wlmask.cbm"

void
OurWindow::useCursorLevelWidth(void)
{
	static bool initialized;
	static Cursor cursor;

	if (!initialized) {
		initialized=true;
//cerr << "OurWindow::useCursorLevelWidth:: creating" << endl;
		Pixmap bitmap=XCreateBitmapFromData(getDisplay(),window,
			(const char *)wlcursor_xbm_bits,
			wlcursor_xbm_width,
			wlcursor_xbm_height);
		Pixmap maskbitmap=XCreateBitmapFromData(getDisplay(),window,
			(const char *)wlmask_xbm_bits,
			wlmask_xbm_width,
			wlmask_xbm_width);

		// color really doesn't matter ...

		XColor foregroundcolor;
		XColor backgroundcolor;
		foregroundcolor.pixel=WhitePixel(getDisplay(),getScreen());
		backgroundcolor.pixel=BlackPixel(getDisplay(),getScreen());
		XQueryColor(getDisplay(),
			DefaultColormap(getDisplay(),getScreen()),
			&foregroundcolor);
		XQueryColor(getDisplay(),
			DefaultColormap(getDisplay(),getScreen()),
			&backgroundcolor);

		cursor=XCreatePixmapCursor(getDisplay(),
			bitmap,
			maskbitmap,
			&foregroundcolor,
			&backgroundcolor,
			wlcursor_xbm_width/2-1,
			wlcursor_xbm_height/2-1);
		XFreePixmap(getDisplay(),bitmap);
		XFreePixmap(getDisplay(),maskbitmap);
	}
	XDefineCursor(getDisplay(),window,cursor);
	XFlush(getDisplay());
}

void
OurWindow::useCursorCrossHair(void)
{
	static bool initialized;
	static Cursor cursor;

	if (!initialized) {
		initialized=true;
//cerr << "OurWindow::useCursorCrossHair:: creating" << endl;
		cursor=XCreateFontCursor(getDisplay(),34);
	}
	XDefineCursor(getDisplay(),window,cursor);
	XFlush(getDisplay());
}

void
OurWindow::useCursorWatch(void)
{
	static bool initialized;
	static Cursor cursor;

	if (!initialized) {
		initialized=true;
//cerr << "OurWindow::useCursorCrossHair:: creating" << endl;
		cursor=XCreateFontCursor(getDisplay(),150);
	}
	XDefineCursor(getDisplay(),window,cursor);
	XFlush(getDisplay());
}

void
OurWindow::redraw(void)
{
	_XEvent event;
	event.type=Expose;
	event.xexpose.count = 0;

	// should probably set a bunch of other stuff, but works for our app 

	XSendEvent(getDisplay(),window,
		False,	// propagate - the X not C++ false
		ExposureMask,&event
	);							
}

OurWindowImage::OurWindowImage(OurWindow *w)
{
	good_flag=true;
	window=w;
	ximage=0;

#if USEXMITSHMEXTENSION == 1

	bool shm_flag=true;
	xshminfo=0;

	int vmajor;
	int vminor;
	Bool vpixmap;
	if (XShmQueryVersion(window->getDisplay(),&vmajor,&vminor,&vpixmap) != True) {	// the X style True !
		errorstream << WMsgDC(XSharedMemoryExtensionUnsupported) << endl;
		shm_flag=false;
	}
	else if (vpixmap != True) {	// the X style True !
		errorstream << WMsgDC(XSharedMemoryPixmapsUnsupported) << endl;
		shm_flag=false;
	}
	else {
		xshminfo=(XShmSegmentInfo *) new char[sizeof(XShmSegmentInfo)];
		if (xshminfo) {
			ximage = XShmCreateImage(window->getDisplay(),
					window->getVisual(),
					window->getDepth(),
					ZPixmap,
					(char *) 0,	// image data
					xshminfo,
					window->getImageWidth(),
					window->getImageHeight()
			);
			if (ximage) {
				Assert(xshminfo);
				xshminfo->shmid=shmget(IPC_PRIVATE,ximage->bytes_per_line*ximage->height,IPC_CREAT|0777);
				if (xshminfo->shmid < 0) {
					errorstream << WMsgDC(SharedMemoryAllocationFailed) << " - "
					    << (ximage->bytes_per_line*ximage->height)
					    << " " << MMsgDC(Bytes) << endl;
					shm_flag=false;
					XDestroyImage(ximage);
					ximage=0;
					delete[] xshminfo;	// otherwise destructor or fall back get messed up
					xshminfo=0;
//cerr << "OurWindowImage::OurWindowImage: SharedMemoryAllocationFailed exit" << endl;
				}
				else {
					Assert(xshminfo);
					xshminfo->shmaddr=(char *)shmat(xshminfo->shmid,0,0);
					if (xshminfo->shmaddr == (char *)-1) {
						errorstream << WMsgDC(SharedMemoryAttachFailed) << endl;
						shm_flag=false;
						shmctl(xshminfo->shmid,IPC_RMID,0);
						XDestroyImage(ximage);
						ximage=0;
						delete[] xshminfo;	// otherwise destructor or fall back get messed up
						xshminfo=0;
					}
					else {
						Assert(ximage);
						ximage->data=xshminfo->shmaddr;
						Assert(xshminfo);
						xshminfo->readOnly=False;	// the X style False !
						XShmAttach(window->getDisplay(),xshminfo);
//cerr << "OurWindowImage::OurWindowImage: shared memory successfully allocated and attached" << endl;
						// Good to go ...
					}
				}
			}
			else {
				errorstream << WMsgDC(XImageCreationFailed) << endl;
				shm_flag=false;
				delete[] xshminfo;	// otherwise destructor or fall back get messed up
				xshminfo=0;
			}
		}
		else {
			errorstream << WMsgDC(XSharedMemorySegmentCreationFailed) << endl;
			shm_flag=false;
		}
	}

	if (!shm_flag) {	// do it without shared memory ...
//cerr << "OurWindowImage::OurWindowImage: fall through to do it without shared memory" << endl;
		Assert(!xshminfo);
		Assert(!ximage);
#endif
//cerr << "OurWindowImage::OurWindowImage: creating non-shared X Image" << endl;
		ximage = XCreateImage(window->getDisplay(),
				window->getVisual(),
				window->getDepth(),
				ZPixmap,
				0,		// offset
				(char *) 0,	// image data
				window->getImageWidth(),
				window->getImageHeight(),
				window->getBitmapPad(),
				0		// bytes per line
		);
		if (ximage) {
			ximage->data = new char [ximage->bytes_per_line*ximage->height];
			if (!ximage->data) {
				errorstream << EMsgDC(XImageDataAllocationFailed) << " - "
					    << (ximage->bytes_per_line*ximage->height)
					    << " " << MMsgDC(Bytes) << endl;
				good_flag=false;
				XDestroyImage(ximage);
				ximage=0;
			}
		}
		else {
			errorstream << EMsgDC(XImageCreationFailed) << endl;
			good_flag=false;
		}
#if USEXMITSHMEXTENSION == 1
	}
#endif
//cerr << "OurWindowImage::OurWindowImage: creating GC" << endl;
	if (good_flag && ximage) {
#ifdef USEDEFAULTGCFORIMAGE
		imagegc = DefaultGC(
				window->getDisplay(),
				window->getScreen()
		);
#else
		imagegc = XCreateGC(window->getDisplay(),window->getWindow(),0,NULL);
		XSetForeground(window->getDisplay(),imagegc,WhitePixel(window->getDisplay(),window->getScreen()));
		XSetBackground(window->getDisplay(),imagegc,BlackPixel(window->getDisplay(),window->getScreen()));
#endif
	}
//cerr << "OurWindowImage::OurWindowImage: at end, shm_flag=" << (shm_flag ? "T":"F") << endl;
//cerr << "OurWindowImage::OurWindowImage: end" << endl;
}

OurWindowImage::~OurWindowImage()
{
//cerr << "OurWindowImage::~OurWindowImage: start" << endl;
#if USEXMITSHMEXTENSION == 1
	if (xshminfo) {
//cerr << "OurWindowImage::~OurWindowImage: destroying xshminfo" << endl;
		XShmDetach(window->getDisplay(),xshminfo);
		ximage->data=0;
		shmdt(xshminfo->shmaddr);
		shmctl(xshminfo->shmid,IPC_RMID,0);
		delete[] xshminfo;
	}
#endif
	if (ximage) {
//cerr << "OurWindowImage::~OurWindowImage: destroying ximage" << endl;
		if (ximage->data) delete[] ximage->data;
		XDestroyImage(ximage);
		ximage=0;
	}
//cerr << "OurWindowImage::~OurWindowImage: end" << endl;
}

void
OurWindowImage::put(void)
{
//cerr << "OurWindowImage::put: start" << endl;
	Assert(ximage);
	Assert(ximage->data);
#if USEXMITSHMEXTENSION == 1
//cerr << "OurWindowImage::put: shm_flag=" << (shm_flag ? "T":"F") << endl;
	//if (shm_flag) {
	if (xshminfo) {
//cerr << "OurWindowImage::put: XShmPutImage" << endl;
		XShmPutImage(
			window->getDisplay(),
			window->getWindow(),
			imagegc,ximage,
			0,0,
			window->getImageStartX(),window->getImageStartY(),
			window->getImageWidth(),window->getImageHeight(),
			False	// send event
		);
	}
	else {
#endif
//cerr << "OurWindowImage::put: XPutImage" << endl;
		XPutImage(
			window->getDisplay(),
			window->getWindow(),
			imagegc,ximage,
			0,0,
			window->getImageStartX(),window->getImageStartY(),
			window->getImageWidth(),window->getImageHeight()
		);
#if USEXMITSHMEXTENSION == 1
	}
#endif
}

char *
OurWindowImage::get8BitDataAddress(void)
{
	return ximage->depth == 8
		? ximage->data
		: 0;
}

bool
OurWindowImage::write8BitDataToStream(ostream &o)
{
	Assert(getDepth() == 8);

	char *pixels=get8BitDataAddress();
	Assert(pixels);

	unsigned row=0;
	char *p = pixels;
	while (row++ < getHeight() && o) {
		o.write(p,getWidth());
		p+=getBytesPerRow();
	}
	return o ? true : false;
}

bool
OurWindowImage::write8BitDataToPGMStream(ostream &o)
{
	o << "P5\n";
	o << getWidth() << " " << getHeight() << " 255\n";	// check this doesn't translate to \n on non-unix :(

	return write8BitDataToStream(o);
}

void
OurWindowImage::putPoint(unsigned x,unsigned y)
{
//cerr << "OurWindowImage::putPoint: start" << endl;

	Assert(x < window->getImageWidth());
	Assert(y < window->getImageHeight());

//if (window->getImageStartY()+y < window->getImageHeight()) {
//	cerr << "OurWindowImage::putPoint:" << dec 
//	     << " window->getImageStartY()=" << window->getImageStartY()
//	     << " y=" << y
//	     << " window->getImageHeight()=" << window->getImageHeight()
//	     << endl;
//}

	XDrawPoint(
		window->getDisplay(),
		window->getWindow(),
		imagegc,
		window->getImageStartX()+x,
		window->getImageStartY()+y
	);
}

OurWindowLevelWidthUpdator::OurWindowLevelWidthUpdator(
	OurWindow &w,Uint16 bits,bool issigned,
	double rescaleintercept,double rescaleslope)
{
	window=&w;
	maplevelmin=0;
	maplevelmax=(1<<bits)-1;
	mapwidthmin=0;
	mapwidthmax=(1<<bits)-1;
	maplevel=maplevelmax/2;
	mapwidth=mapwidthmax/4;
//cerr << "width min" << mapwidthmin  << " level min " << maplevelmin << endl;
//cerr << "width max" << mapwidthmax  << " level max " << maplevelmax << endl;
//cerr << "width " << mapwidth  << " level " << maplevel << endl;
	signxor=issigned ? 1l<<(bits-1) : 0;
	Assert(rescaleintercept<=LONG_MAX);
	Assert(rescaleintercept>=LONG_MIN);
	intercept=long(rescaleintercept);
	Assert(rescaleslope<=LONG_MAX/1024);
	Assert(rescaleslope>=LONG_MIN/1024);
	slopeby1024=long(rescaleslope*1024);
}

OurWindowLevelWidthUpdator::~OurWindowLevelWidthUpdator()
{}

void
OurWindowLevelWidthUpdator::set(Uint16 level,Uint16 width)
{
	maplevel=level;
	mapwidth=width;
	Assert(mapwidth>=mapwidthmin);
	Assert(mapwidth<=mapwidthmax);
	Assert(maplevel>=maplevelmin);
	Assert(maplevel<=maplevelmax);
}

void
OurWindowLevelWidthUpdator::get(Uint16 &level,Uint16 &width)
{
	Assert(maplevel<=Uint16_MAX);
	Assert(mapwidth<=Uint16_MAX);
	level=Uint16(maplevel);
	width=Uint16(mapwidth);
}

Uint16
OurWindowLevelWidthUpdator::getStoredLevelFromDisplayedLevel(long l)
{
	return Uint16((l-intercept)*1024/slopeby1024+signxor);
}

Uint16
OurWindowLevelWidthUpdator::getStoredWidthFromDisplayedWidth(long w)
{
	return Uint16(w*1024/slopeby1024);
}

long
OurWindowLevelWidthUpdator::getDisplayedLevelFromStoredLevel(long l)
{
	return (l-signxor)*slopeby1024/1024+intercept;
}

long
OurWindowLevelWidthUpdator::getDisplayedWidthFromStoredWidth(long w)
{
	return w*slopeby1024/1024;
}

void
OurWindowLevelWidthUpdator::put(void)
{
	ostrstream levelostr;
	levelostr << "level " << setw(6) << getDisplayedLevelFromStoredLevel(maplevel) << ends;
	char *levelstr = levelostr.str();

	ostrstream widthostr;
	widthostr << "width " << setw(6) << getDisplayedWidthFromStoredWidth(mapwidth) << ends;
	char *widthstr = widthostr.str();

	window->clearFooter();
	window->writeFooterText(levelstr,0,widthstr);

	if (levelstr) delete[] levelstr;
	if (widthstr) delete[] widthstr;
}

void
OurWindowLevelWidthUpdator::press(long x,long y)
{
	lastx=x;
	lasty=y;
}

void
OurWindowLevelWidthUpdator::release(long x,long y)
{
	// Don't update ... it is irritating to have
	// the values change slightly on button release
	(void)x; (void)y;
}

void
OurWindowLevelWidthUpdator::move(long deltax,long deltay,long multiplier)
{
	update(lastx+deltax,lasty+deltay,multiplier);
}

void
OurWindowLevelWidthUpdator::update(long x,long y,long multiplier)
{
//cerr << "position " << x << "," << y << endl;
	long deltax=(x-lastx)*multiplier;	// +ve to right
	long deltay=(lasty-y)*multiplier;	// +ve upwards
//cerr << "delta " << deltax << "," << deltay << endl;
	lastx=x;
	lasty=y;
	mapwidth+=deltax;
	if (mapwidth<mapwidthmin) mapwidth=mapwidthmin;
	if (mapwidth>mapwidthmax) mapwidth=mapwidthmax;
	maplevel+=deltay;
	if (maplevel<maplevelmin) maplevel=maplevelmin;
	if (maplevel>maplevelmax) maplevel=maplevelmax;
//cerr << "width " << mapwidth  << " level " << maplevel << endl;
	put();
}

