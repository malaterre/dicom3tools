static char *CopyrightIdentifier(void) { return "@(#)ntstream.cc 1.00 95/09/21 Copyright (C) 1995. David A. Clunie. All rights reserved."; }
#include "ntstream.h"

#include <unistd.h>
#include <stdlib.h>	// for atoi()
#include <string.h>	// for memxxx()
#include <sys/types.h>

#include <sys/socket.h>	// NB. g++ on sun doesn't have prototypes

#ifdef NEEDMISSINGPROTOTYPES
extern "C" {
int accept(int, struct sockaddr *, int *);
int bind(int, struct sockaddr *, int);
int connect(int, const struct sockaddr *, int);
int listen(int, int);
int socket(int, int, int);
}
#endif /* NEEDMISSINGPROTOTYPES */

#include <netinet/in.h>	// for htons()

#include <arpa/inet.h>	// for inet_addr()
			// NB. g++ on sun doesn't have prototypes

#ifdef NEEDMISSINGPROTOTYPES
extern "C" {
unsigned long    inet_addr(const char*);
}
#endif /* NEEDMISSINGPROTOTYPES */

#include <netdb.h>	// NB. g++ on sun doesn't have prototypes

#ifdef NEEDMISSINGPROTOTYPES
extern "C" {
struct hostent *gethostbyname(const char *);
struct servent *getservbyname(char *, char *);
struct protoent *getprotobyname(char *);
}
#endif /* NEEDMISSINGPROTOTYPES */

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff
#endif /* INADDR_NONE */

// ********************* netbuf member functions ******************

netbuf::netbuf()
{
	open_flag=0;
}

netbuf::~netbuf()
{
	(void)close();
}

netbuf*
netbuf::open(const char *h,const char *s,const char *p,int md)
{
	struct sockaddr_in 	address;
	struct servent *	service;
	struct hostent *	host;
	struct protoent *	protocol;

	memset((char *)&address,0,sizeof(address));
	address.sin_family = AF_INET;

	if ((service=::getservbyname((char *)s,(char *)p)) == 0) {
		if ((address.sin_port=htons((unsigned short)atoi(s))) == 0) {
			return 0;
		}
	}
	else {
		address.sin_port=htons(ntohs((unsigned short)service->s_port));
	}

	if ((host=::gethostbyname(h)) == 0) {
  		if ((address.sin_addr.s_addr=inet_addr(h)) == INADDR_NONE) {
			return 0;
		}
	}
	else {
		memcpy((char *)&address.sin_addr,host->h_addr,host->h_length);
	}

	if ((protocol=::getprotobyname((char *)p)) == 0) return 0;

	int type = strcmp(p,"udp") ? SOCK_STREAM : SOCK_DGRAM;

	if ((socket=::socket(PF_INET,type,protocol->p_proto)) < 0) return 0;

	if (::connect(socket,(struct sockaddr *)&address,sizeof(address)) < 0) return 0;

	open_flag=1;
	//mode=md;
	(void)md;
	mode=ios::in|ios::out;

	//cerr << "netbuf::open: success\n" << flush;

	return this;
}

netbuf*
netbuf::close(void)
{
	int err=0;
	if (open_flag) {
		err+=(mode&ios::out && overflow(EOF) == EOF);
		err+=(::close(socket) == -1);
		open_flag=0;
	}
	return err ? 0 : this;
}

// The default supplied by libg++ for this won't work, so do it ourselves ...

int
netbuf::xsputn(const char *p,int n)
{
	//cerr << "netbuf::xsputn: start with p=<" << p << ">\n" << flush;
	if (sync() == EOF) return EOF;
	return ::write(socket,p,n);
}

int
netbuf::overflow(int c)
{
	//cerr << "netbuf::overflow: start with c=" << c << "\n" << flush;

	if (!(mode&ios::out)			// can't write
	 || allocate() == EOF			// no holding area
	 || (gptr() && gptr() < egptr())	// are get chars left
	) return EOF;

	//cerr << "netbuf::overflow: ok\n" << flush;

	setg(0,0,0);
	if (!pptr()) setp(base(),base());

	int unconsummed = pptr()-pbase();

	if (c != EOF) {		// consume argument
		*pptr()=c;	// space is always there
		++unconsummed;
	}

	//cerr << "netbuf::overflow: unconsummed=" << unconsummed << "\n" << flush;

	if (unconsummed == 0) return 0;	// nothing to do, but not error

	if (::write(socket,pbase(),unconsummed) == unconsummed) {
		setp(base(),ebuf()-1);	// space for c next time

		//cerr << "netbuf::overflow: success\n" << flush;

		return 1;		// not EOF
	}
	else {
		setp(0,0);

		//cerr << "netbuf::overflow: fail\n" << flush;

		return EOF;	// error
	}
}

int
netbuf::underflow(void)
{
	//cerr << "netbuf::underflow: start\n" << flush;

	if (!(mode&ios::in)		// can't read
	 || allocate() == EOF		// no holding area
	) return EOF;

	//cerr << "netbuf::underflow: ok\n" << flush;

	if (gptr() && gptr() < egptr())	// still chars in get area
		return *gptr();

	//cerr << "netbuf::underflow: need to produce more\n" << flush;

	if (pptr() && pptr() < pbase()
	 && overflow(EOF) == EOF)	// can't put remaining chars
		return EOF;

	setp(0,0);

	//cerr << "netbuf::underflow: put remaining ok\n" << flush;

	int putbacklng = (blen() > 1) ? 1 : 0;

	int nread=::read(socket,base()+putbacklng,blen()-putbacklng);

	//cerr << "netbuf::underflow: read=" << nread << "\n" << flush;

	if (nread > 0) {
		setg(base(),base()+putbacklng,base()+putbacklng+nread);
		return *gptr();
	}
	else {
		setg(0,0,0);
		return EOF;	// error
	}
}

int
netbuf::sync(void)
{
	//cerr << "netbuf::sync: start\n" << flush;

	if (gptr() && gptr() < egptr())	// still chars in get area
		return EOF;		// can't return them to producer

	if (pptr() && pptr() < pbase()
	 && overflow(EOF) == EOF)	// can't put remaining chars
		return EOF;

	//cerr << "netbuf::sync: success\n" << flush;

	return 0;			// all chars flushed
}

// ********************* netstreambase member functions ******************

netstreambase::netstreambase()
{
}

netstreambase::netstreambase(const char *host,const char *service,const char *protocol,int mode)
{
	init(&buf);			// supply ios with the streambuf
	open(host,service,protocol,mode);
}

netstreambase::~netstreambase()
{
	close();
}

void
netstreambase::open(const char *host,const char *service,const char *protocol,int mode)
{
	(void)buf.open(host,service,protocol,mode);
}

void
netstreambase::close(void)
{
	(void)buf.close();
}

void
netstreambase::setbuf(char *p,int l)
{
	(void)buf.setbuf(p,l);
}

// ********************* inetstream member functions ******************

inetstream::inetstream()
{
}

inetstream::inetstream(const char *host,const char *service,const char *protocol,int mode)
	: netstreambase(host,service,protocol,mode)
{
}

inetstream::~inetstream()
{
}

// ********************* onetstream member functions ******************

onetstream::onetstream()
{
}

onetstream::onetstream(const char *host,const char *service,const char *protocol,int mode)
	: netstreambase(host,service,protocol,mode)
{
}

onetstream::~onetstream()
{
}

#ifdef TEST_ntstream

int
main(int argc,char **argv)
{
	(void)argc; (void)argv;

	inetstream in("localhost","echo","tcp");

	//char inbuffer[1024];
	//in.rdbuf()->setbuf(inbuffer,1024);

	ostream out(in.rdbuf());

	in.tie(&out);

	char buffer[80];
	while (cin.getline(buffer,80,'\n')) {
		out.write(buffer,strlen(buffer));
		out << "\n";
		if (!out.good()) {
			cerr << "Write failed\n" << flush;
		}
		//out << flush;
		//if (!out.good()) {
		//	cerr << "Flush failed\n" << flush;
		//}

		in.getline(buffer,80,'\n');
		if (!in.good()) {
			cerr << "Read failed\n" << flush;
		}
		cout.write(buffer,in.gcount());
		cout << "\n";
	}
}

#endif /* TEST_ntstream */
