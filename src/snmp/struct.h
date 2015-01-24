/*****************************************************************************

 @(#) src/snmp/struct.h

 -----------------------------------------------------------------------------

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

#ifndef __LOCAL_STRUCT_H__
#define __LOCAL_STRUCT_H__

#define STRMAX			1024

#define SHPROC			1
#define EXECPROC		2
#define PASSTHRU		3
#define PASSTHRU_PERSIST	4

#define MIBMAX			30

struct extensible {
	char name[STRMAX];
	char command[STRMAX];
	char fixcmd[STRMAX];
	int type;
	int result;
	char output[STRMAX];
	struct extensible *next;
	unsigned long miboid[MIBMAX];
	size_t miblen;
	int pid;
};

struct myproc {
	char name[STRMAX];
	char fixcmd[STRMAX];
	int min;
	int max;
	struct myproc *next;
};

/*
struct mibinfo 
{
   int numid;
   unsigned long mibid[10];
   char *name;
   void (*handle) ();
};
*/

#endif				/* __LOCAL_STRUCT_H__ */
