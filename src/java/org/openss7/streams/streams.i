
%module Streams
%{
extern "C" {
#define _XOPEN_SOURCE 600
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS 1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/limits.h>
#include <values.h>
}
%}

%include "typemaps.i"
%include "various.i"
%include "stl.i"

%javaconst(1);

%ignore strbuf;

%ignore ::open;
%ignore ::close;
%ignore ::__errno_location;
%ignore ::ioctl;
%ignore ::isastream;
%ignore ::getmsg;
%ignore ::getpmsg;
%ignore ::putmsg;
%ignore ::putpmsg;
%ignore ::fattach;
%ignore ::fdetach;
%ignore ::pipe;

%apply int &INOUT { int &flagsp };
%apply int &INOUT { int &bandp };

extern "C" {
    struct strbuf {
        int maxlen;
        int len;
        char *buf;
    };
    extern int open(const char *pathname, int flags);
    extern int close(int fd);
    extern int *__errno_location(void);
    extern int ioctl(int fd, unsigned long int request, ...);
}

%ignore getStrbuf;

%inline %{

#ifndef __THROW
#define __THROW
#endif
    extern "C" {
        extern int isastream(int fd);
        extern int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);
        extern int getpmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp);
        extern int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags);
        extern int putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags);
        extern int fattach(int fd, const char *path);
        extern int fdetach(const char *path);
        extern int pipe(int fds[2]) __THROW;
    }

    namespace streams {

        class Error {
            private:
                int errnum;
                const char *msg;
            public:
                virtual const char *strerror(void) { return (msg != ((const char *)0)) ? msg : ::strerror(errnum); }
                virtual int error(void) { return errnum; }
                virtual ~Error(void) { }
                Error(void) : errnum(errno), msg(((const char *)0)) { }
                Error(int err) : errnum(err), msg(((const char *)0)) { }
                Error(const char *string) : errnum(errno), msg(string) { }
                Error(int err, const char *string) : errnum(err), msg(string) { }
        };

        class Strbuf {
            private:
                bool allocated;
                struct strbuf buffer;
                Strbuf(void) { }
            public:
                struct strbuf * getStrbuf(void) { return &buffer; }
                virtual ~Strbuf(void) {
                    if (allocated && (buffer.buf != ((char *)0))) {
                        free((void *)buffer.buf);
                    }
                }
                Strbuf(char *buf, int len, int maxlen=-1) {
                    allocated = false;
                    buffer.buf = buf;
                    buffer.len = len;
                    buffer.maxlen = maxlen;
                }
                Strbuf(int len) {
                    allocated = true;
                    buffer.maxlen = len;
                    buffer.len = len;
                    if (len > 0) {
                        buffer.buf = (char *)malloc(len);
                    }
                }
        };

        class Stream {
            private:
                int fd;
                Stream(int filedes) : fd(filedes) { }
            public:
                virtual void ioctl(unsigned long int request, unsigned long int arg=0)
                { if (::ioctl(fd, request, arg) == -1) throw new Error(); }
                virtual void isastream(void)
                { if (::isastream(fd) == -1) throw new Error(); }
                virtual void getmsg(Strbuf &ctlptr, Strbuf &datptr, int &flagsp)
                { if (::getmsg(fd, ctlptr.getStrbuf(), datptr.getStrbuf(), &flagsp) == -1) throw new Error(); }
                virtual void getpmsg(Strbuf &ctlptr, Strbuf &datptr, int &bandp, int &flagsp)
                { if (::getpmsg(fd, ctlptr.getStrbuf(), datptr.getStrbuf(), &bandp, &flagsp) == -1) throw new Error(); }
                virtual void putmsg(Strbuf &ctlptr, Strbuf &datptr, int flags)
                { if (::putmsg(fd, ctlptr.getStrbuf(), datptr.getStrbuf(), flags) == -1) throw new Error(); }
                virtual void putpmsg(Strbuf &ctlptr, Strbuf &datptr, int band, int flags)
                { if (::putpmsg(fd, ctlptr.getStrbuf(), datptr.getStrbuf(), band, flags) == -1) throw new Error(); }
                virtual void fattach(const char *path)
                { if (::fattach(fd, path) == -1) throw new Error(); }
                static void fdetach(const char *path)
                { if (::fdetach(path) == -1) throw new Error(); }
                static void pipe(Stream *strs[2]) {
                    int fds[2] = { 0, 0 };
                    if (::pipe(fds) == -1)
                        throw new Error();
                    strs[0] = new Stream(fds[0]);
                    strs[1] = new Stream(fds[1]);
                }
            public:
                virtual ~Stream(void) { if (fd != 0) ::close(fd); }
                Stream(const char *devname, int flags) { if ((fd = ::open(devname, flags)) == -1) throw new Error(); }
        };

    };

%}

// vim: ft=cpp sw=4 et
