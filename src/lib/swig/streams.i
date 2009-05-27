/* vim: ft=c
 */
%module streams
%{

#include "../streams.h"

%}

int getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp);
int getpmsg(intfd, struct strbuf *ctlptr, struct strbuf *datptr, int *bandp, int *flagsp);
int putmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags);
int putpmsg(int fd, const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags);
