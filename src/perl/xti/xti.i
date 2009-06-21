/*****************************************************************************

 @(#) $RCSfile: xti.i,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:41:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2009-06-21 11:41:59 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti.i,v $
 Revision 1.1.2.1  2009-06-21 11:41:59  brian
 - added files to new distro

 *****************************************************************************/

%module Xti
%{
#include <sys/types.h>
#include <stdint.h>
%}

%include "typemaps.i"
//%include "arrays_java.i"
//%include "various.i"

//%javaconst(1);

typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int u_int32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned short u_int16_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef unsigned char u_int8_t;

//%apply char *BYTE { void * }
//%apply char *BYTE { void *iov_base }
//%apply char *BYTE { char *iov_base }
//%apply char *BYTE { char *buf }
//%apply char *BYTE { char value[0] }

%ignore _t_errno;
%ignore opthdr;

%ignore t_iovec_;
#define t_iovec t_iovec_
%immutable iov_base;

#define _XOPEN_SOURCE 600

%include "sys/xti.h"
%include "sys/xti_inet.h"
// %include "sys/xti_atm.h"
%include "sys/xti_osi.h"
%include "sys/xti_mosi.h"

#undef t_iovec

%inline %{
struct t_iovec {
        %immutable;
        char *iov_base;
        size_t iov_len;
        %mutable;
};
//union t_struct_p {
//        struct t_bind *bind;
//        struct t_optmgmt *optmgmt;
//        struct t_call *call;
//        struct t_discon *dis;
//        struct t_unitdata *unitdata;
//        struct t_uderr *uderror;
//        struct t_info *info;
//};
int xti_errno(void) {
        return t_errno;
}
%}

%rename(xti_accept) t_accept;
%rename(xti_addleaf) t_addleaf;
%rename(xti_alloc) t_alloc;
%rename(xti_bind) t_bind;
%rename(xti_close) t_close;
%rename(xti_connect) t_connect;
%rename(xti_error) t_error;
%rename(xti_free) t_free;
%rename(xti_getinfo) t_getinfo;
%rename(xti_getprotaddr) t_getprotaddr;
%rename(xti_getstate) t_getstate;
%rename(xti_listen) t_listen;
%rename(xti_look) t_look;
%rename(xti_open) t_open;
%rename(xti_optmgmt) t_optmgmt;
%rename(xti_rcv) t_rcv;
%rename(xti_rcvconnect) t_rcvconnect;
%rename(xti_rcvdis) t_rcvdis;
%rename(xti_rcvleafchange) t_rcvleafchange;
%rename(xti_rcvopt) t_rcvopt;
%rename(xti_rcvrel) t_rcvrel;
%rename(xti_rcvreldata) t_rcvreldata;
%rename(xti_rcvudata) t_rcvudata;
%rename(xti_rcvuderr) t_rcvuderr;
%rename(xti_rcvv) t_rcvv;
%rename(xti_rcvvopt) t_rcvvopt;
%rename(xti_rcvvudata) t_rcvvudata;
%rename(xti_removeleaf) t_removeleaf;
%rename(xti_snd) t_snd;
%rename(xti_snddis) t_snddis;
%rename(xti_sndopt) t_sndopt;
%rename(xti_sndrel) t_sndrel;
%rename(xti_sndreldata) t_sndreldata;
%rename(xti_sndudata) t_sndudata;
%rename(xti_sndv) t_sndv;
%rename(xti_sndvopt) t_sndvopt;
%rename(xti_sndvudata) t_sndvudata;
%rename(xti_strerror) t_strerror;
%rename(xti_sync) t_sync;
%rename(xti_sysconf) t_sysconf;
%rename(xti_unbind) t_unbind;

extern int t_accept(int fd, int resfd, const struct t_call *INPUT);
// extern int t_addleaf(int fd, int leafid, const struct netbuf *INPUT);
// extern union t_struct_p t_alloc(int fd, int type, int fields);
extern int t_bind(int fd, const struct t_bind *INPUT, struct t_bind *OUTPUT);
extern int t_close(int fd);
extern int t_connect(int fd, const struct t_call *INPUT, struct t_call *OUTPUT);
extern int t_error(const char *INPUT);
// extern int t_free(union t_struct_p INPUT, int type);
extern int t_getinfo(int fd, struct t_info *OUTPUT);
extern int t_getprotaddr(int fd, struct t_bind *OUTPUT, struct t_bind *OUTPUT);
extern int t_getstate(int fd);
extern int t_listen(int fd, struct t_call *OUTPUT);
extern int t_look(int fd);
extern int t_open(const char *INPUT, int oflag, struct t_info *OUTPUT);
extern int t_optmgmt(int fd, const struct t_optmgmt *INPUT, struct t_optmgmt *OUTPUT);
extern int t_rcv(int fd, char *BYTE, unsigned int nbytes, int *OUTPUT);
extern int t_rcvconnect(int fd, struct t_call *OUTPUT);
extern int t_rcvdis(int fd, struct t_discon *OUTPUT);
// extern int t_rcvleafchange(int fd, struct t_leaf_status *OUTPUT);
extern int t_rcvopt(int fd, struct t_unitdata *OUTPUT, int *OUTPUT);
extern int t_rcvrel(int fd);
extern int t_rcvreldata(int fd, struct t_discon *OUTPUT);
extern int t_rcvudata(int fd, struct t_unitdata *OUTPUT, int *OUTPUT);
extern int t_rcvuderr(int fd, struct t_uderr *OUTPUT);
extern int t_rcvv(int fd, const struct t_iovec *INPUT, unsigned int, int *OUTPUT);
extern int t_rcvvopt(int fd, struct t_unitdata *OUTPUT, const struct t_iovec *INPUT, unsigned int iovcount, int flags);
extern int t_rcvvudata(int fd, struct t_unitdata *OUTPUT, const struct t_iovec *INPUT, unsigned int iovcount, int *OUTPUT);
// extern int t_removeleaf(int fd, int, int);
extern int t_snd(int fd, char *BYTE, unsigned int nbytes, int flags);
extern int t_snddis(int fd, const struct t_call *INPUT);
extern int t_sndopt(int fd, const struct t_unitdata *INPUT, int flags);
extern int t_sndrel(int fd);
extern int t_sndreldata(int fd, const struct t_discon *INPUT);
extern int t_sndudata(int fd, const struct t_unitdata *INPUT);
extern int t_sndv(int fd, const struct t_iovec *INPUT, unsigned int iovcount, int flags);
extern int t_sndvopt(int fd, const struct t_unitdata *INPUT, const struct t_iovec *INPUT, unsigned int iovcount, int flags);
extern int t_sndvudata(int fd, const struct t_unitdata *INPUT, struct t_iovec *INPUT, unsigned int flags);
extern const char *t_strerror(int fd);
extern int t_sync(int fd);
extern int t_sysconf(int fd);
extern int t_unbind(int fd);

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("openss7XtiJNI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}

/*
   vim: ft=c
 */
