
%module Streams

%include "typemaps.i"

%inline %{

    extern "C" {
        struct strbuf {
            int maxlen;
            int len;
            char *buf;
        };

        extern int open(const char *pathname, int flags);
        extern int close(int fd);
        extern int *__errno_location(void) throw();
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

%}

// vim: ft=cpp sw=4 et
