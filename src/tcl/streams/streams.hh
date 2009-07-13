//  ==========================================================================
//  
//  @(#) $Id: streams.hh,v 1.1.2.1 2009-07-13 07:13:36 brian Exp $
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date: 2009-07-13 07:13:36 $ by $Author: brian $
//  
//  --------------------------------------------------------------------------
//  
//  $Log: streams.hh,v $
//  Revision 1.1.2.1  2009-07-13 07:13:36  brian
//  - changes for multiple distro build
//
//  ==========================================================================

#ident "@(#) $Id: streams.hh,v 1.1.2.1 2009-07-13 07:13:36 brian Exp $"

#ifndef __LOCAL_STREAMS_HH__
#define __LOCAL_STREAMS_HH__

#pragma interface

//
// These are the definitions of a brief class library for interfacing with a
// Stream implementing the STREAMS system call interface.  Its purpose is to
// provide an object-oriented interface to SWIG primarily for generation of a
// TCL API for use with TCAP.
//

extern "C" {
    struct strbuf {
	int maxlen;
	int len;
	char *buf;
    };

    extern int open(const char *pathname, int flags);
    extern int close(int fd);
    extern int *_errno(void);
#define errno (*_errno())
    extern int ioctl(int fd, unsigned long int request, ...);
    extern int isastream(int fd);
    extern int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);
    extern int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp);
    extern int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags);
    extern int putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags);
    extern int fattach(int fd, const char *path);
    extern int fdetach(const char *path);
    extern int pipe(int fds[2]);
}

namespace streams {

    class Error {
	private:
	    int error;
	    const char *msg;
	public:
	    virtual const char *strerror(void) { return (msg != ((const char *)0)) ? msg : ::strerror(error); }
	    virtual int errno(void) { return error; }
	    Error(void) : error(errno), msg(((const char *)0)) { }
	    Error(int errnum) : error(errnum), msg(((const char *)0)) { }
	    Error(const char *string) : error(errno), msg(string) { }
	    Error(int errnum, const char *string) : error(errnum), msg(string) { }
    };

    class Stream {
	private:
	    int fd;
	    Stream(int filedes) : fd(filedes) { }
	protected:
	    virtual void ioctl(unsigned long int request, unsigned long int arg=0)
	    { if (::ioctl(fd, request, arg) == -1) throw new ::streams::Error(); }
	    virtual void isastream(void)
	    { if (::isastream(fd) == -1) throw new ::streams::Error(); }
	    virtual void getmsg(struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
	    { if (::getmsg(fd, ctlptr, datptr, flagsp) == -1) throw new ::streams::Error(); }
	    virtual void getpmsg(struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp)
	    { if (::getpmsg(fd, ctlptr, datptr, bandp, flagsp) == -1) throw new ::streams::Error(); }
	    virtual void putmsg(const struct strbuf *ctlptr, const struct strbuf *datptr, int flags)
	    { if (::putmsg(fd, ctlptr, datptr, flags) == -1) throw new ::streams::Error(); }
	    virtual void putpmsg(const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags)
	    { if (::putpmsg(fd, ctlptr, datptr, band, flags) == -1) throw new ::streams::Error(); }
	    virtual void fattach(const char *path)
	    { if (::fattach(fd, path) == -1) throw new ::streams::Error(); }
	    static void fdetach(const char *path)
	    { if (::fdetach(path) == -1) throw new ::streams::Error(); }
	    static Stream *pipe(Stream **other) {
		int fds[2] = { 0, 0 };
		if (::pipe(fds) == -1)
		    throw new ::streams::Error();
		if (other != ((::streams::Stream **)0))
		    *other = new ::streams::Stream(fds[1]);
		return new ::streams::Stream(fds[0]);
	    }
	public:
	    virtual ~Stream(void) { if (fd != 0) ::close(fd); }
	    Stream(const char *devname, int flags) { if ((fd = ::open(devname, flags)) == -1) throw new ::streams::Error(); }
    };

};


#endif				/* __LOCAL_STREAMS_HH__ */

