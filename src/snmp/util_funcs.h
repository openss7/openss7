/*****************************************************************************

 @(#) src/snmp/util_funcs.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 *****************************************************************************/

#ifndef __LOCAL_UTIL_FUNCS_H__
#define __LOCAL_UTIL_FUNCS_H__

/*
 *  util_funcs.h:  utilitiy functions for extensible groups.
 */

//#ifdef IN_UCD_SNMP_SOURCE
#include "struct.h"
//#else
//#include <ucd-snmp/struct.h>
//#endif

void Exit(int);
int shell_command(struct extensible *);
int exec_command(struct extensible *);
int get_exec_output(struct extensible *);
int get_exec_pipes(char *cmd, int *fdIn, int *fdOut, int *pid);
WriteMethod clear_cache;
RETSIGTYPE restart_doit(int);
WriteMethod restart_hook;
void print_mib_oid(oid *, size_t);
void sprint_mib_oid(char *, oid *, size_t);
int header_simple_table(struct variable *, oid *, size_t *, int, size_t *,
			WriteMethod ** write_method, int);
int header_generic(struct variable *, oid *, size_t *, int, size_t *, WriteMethod **);
int checkmib(struct variable *, oid *, size_t *, int, size_t *, WriteMethod ** write_method, int);
char *find_field(char *, int);
int parse_miboid(const char *, oid *);
void string_append_int(char *, int);
void wait_on_exec(struct extensible *);

#define     satosin(x)      ((struct sockaddr_in *) &(x))
#define     SOCKADDR(x)     (satosin(x)->sin_addr.s_addr)
#ifndef MIB_STATS_CACHE_TIMEOUT
#define MIB_STATS_CACHE_TIMEOUT 5
#endif

typedef void *mib_table_t;
typedef int (RELOAD) (mib_table_t);
typedef int (COMPARE) (const void *, const void *);
mib_table_t Initialise_Table(int, int, RELOAD, COMPARE);
int Search_Table(mib_table_t, void *, int);
int Add_Entry(mib_table_t, void *);
void *Retrieve_Table_Data(mib_table_t, int *);

#if 0
int marker_uptime(marker_t pm);
int marker_tticks(marker_t pm);
int timeval_uptime(struct timeval *tv);
int timeval_tticks(struct timeval *tv);
#endif

#endif				/* __LOCAL_UTIL_FUNCS_H__ */
