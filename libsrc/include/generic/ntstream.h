/* ntstream.h 1.00 95/09/21 Copyright (C) 1995. David A. Clunie. All rights reserved. */
#ifndef __Header_nstream__
#define __Header_nstream__

#include <iostream.h>

class netbuf: public streambuf {
protected:
	int	mode;
	int	open_flag;
	int	socket;

	virtual int	overflow(int=EOF);
	virtual int	underflow(void);
	virtual int	sync(void);
	virtual int	xsputn(const char *p,int n);
public:
	netbuf();
	virtual ~netbuf();

	int	is_open(void)	{ return open_flag; }
	netbuf*	open(const char *host,const char *service,const char *protocol,int md);
	netbuf*	close(void);
};

class netstreambase : virtual public ios {
private:
	netbuf buf;
public:
	netstreambase();
	netstreambase(const char *host,const char *service,const char *protocol,int mode);
	virtual ~netstreambase();

	void	open(const char *host,const char *service,const char *protocol,int mode);
	void	close(void);
	void	setbuf(char *p,int l);

	netbuf*	rdbuf() { return &buf; }
};

class inetstream : public netstreambase, public istream {
public:
	inetstream();
	inetstream(const char *host,const char *service,const char *protocol,int mode=ios::in);
	virtual ~inetstream();

	netbuf*	rdbuf() { return netstreambase::rdbuf(); }
};

class onetstream : public netstreambase, public ostream {
public:
	onetstream();
	onetstream(const char *host,const char *service,const char *protocol,int mode=ios::out);
	virtual ~onetstream();

	netbuf*	rdbuf() { return netstreambase::rdbuf(); }
};

// There is no netstream class that does bidirectional i/o a la fstream
// The en vogue way to do this is sharing the rdbuf() with another ostream
// eg.
//
//	inetstream in("localhost","echo","tcp");
//	ostream out(in.rdbuf());
//	in.tie(&out);		// this is necessary if interactive

#endif /* __Header_nstream__ */
