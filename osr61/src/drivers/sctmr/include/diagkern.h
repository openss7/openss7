/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : diagkern.h
 * Description                  : kernel level diagnostic
 *
 *
 **********************************************************************/

#ifndef __DIAG_H__
#define __DIAG_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 |
 |			DIAGNOSTIC SYSTEM
 |			-----------------
 |
 |	DIAGNOSTIC TYPES (selectable on a module basis)
 |	----------------
 |
 |	o  Bug Check		DIAGBUGCHECK( STRING, parms... )
 |	o  Error		DIAGERR(    STRING, parms... )
 |	o  Warning		DIAGWARN(   STRING, parms... )
 |	o  Information		DIAGINFO(   LEVEL, STRING, parms... )
 |	o  Function Entry	DIAGENTER(  FuncName, parms... )
 |	o  Function Exit	DIAGEXIT(   ReturnVal )
 |	o  Fail			DIAGFAIL(   STRING, parms... )
 |
 |	o  Start		DIAGSTART()
 |	o  Dispatch		DIAGDISPATCH()
 |	o  Finish		DIAGFINISH()
 |
 |	ADVANCED FEATURES
 |
 |	o  Diagnostic enable		DIAGENABLE( diag-type, level )
 |	o  Diagnostic disable		DIAGDISABLE(diag-type )
 |	o  Diagnostic destination	DIAGDEST( diag-type, destination, parm )
 |
 |
 |   USAGE
 |   -----
 |
 |	All file that wish to use this diagnostic system must include
 |	this file ('diagkern.h').  And just before this file is included,
 |	MODULE_NAME and DIAG_FLAG_VAR must be defined... ie
 |
 |		#define MODULE_NAME	"TEST"
 |		#define DIAG_FLAG_VAR	_tstDIAGFlag
 |		#include "diagkern.h"
 |
 |	MODULE_NAME is a character constant of the module name.  Ideally
 |	each different module should have a uniq module name.
 |
 |	DIAG_FLAG_VAR is the name of a global varible that can be used
 |	to dynamically control diagnostic output at runtime.  You will
 |	need either a kernal debugger or a kernal level function to assign
 |	new values to this varibles.  If you do not define this varible it
 |	will default to _diagInfo_Level.
 |
 |	Each entery to a function must start with DIAGENTER() as the very
 |	first executable statement.  At every point of return within the
 |	function DIAGEXIT() must be called.  A conveniance function DIAGFAIL()
 |	exist to do both DIAGERR() and DIAGEXIT().
 |
 |	There are several different types of diagnostic.  They should be
 |	used as follows:
 |
 |		BUGCHECK   should only be used when an error,  internal to  
 |			   the current module has been detected.  For example
 |			internal buffers full or received a message that the
 |			code doesn't know how to deal with.  Diagnostics of
 |			this type would be the sort of things that would
 |			require technical support and changes to the code of
 |			the current module.  It is possible that this diag
 |			type may be used to automatically generate Software
 |			Bug Reports.
 |
 |		ERROR	used for another error (ie unable to alloc memory).
 |
 |		WARNING	used for conditions that are suspect,  but are valid.
 |			ie playing a zero length message.
 |
 |		INFORMATION	additional information about the internal 
 |				state of code.  This can be used to dump 
 |			internal varibles at strategic points.  As the 
 |			information becomes more detailed a higher information
 |			level number should be used.  Valid numbers are 1-9.
 |
 |	DIAGFINISH() should be the last DIAGxxx statement to execute.  For 
 |	most module it will not be possible to include this,  becuase an
 |	executable normally finishes in one place.  The purpose of DBGFINISH()
 |	is allow each destination meathod to clean up (ie close files, send
 |	diagnostic session result to mail/printer/what_ever.
 |
 |	DIAGDISPATCH() can be used to send diagnostics uptill the current
 |	time.  This is what modules should do if they are not sure a
 |	DIAGFINISH() exists.  DIAGDISPATCH() should be done when the modules
 |	thinks it has been called the last time,  or... if it wants to
 |	dispatch the current diagnostic report immediately.
 |	
 |
 |
 |
 |   COMPILE TIME OPTIONS
 |   --------------------
 |
 |	-DDIAGBUGCHECK_OFF Removes all DIAGBUGCHECK() calls
 |	-DDIAGERR_OFF	Removes all DIAGERR() calls
 |	-DDIAGWARN_OFF	Removes all DIAGWARN() calls
 |
 |	-DDIAGTRACE_ON	Adds DIAGTRACE() code
 |	-DDIAGINFO_ON	Adds DIAGINFO() code
 |
 |	-DDIAGALLOUT	Totally remove ALL diagnostic code 
 |
 |
 |
 |   RUN TIME CONFIGURATION
 |   ----------------------
 |
 |	By changing the value of DIAG_FLAG_VAR varible.
 |
 |
 |
 |
 |
 */


#define DIAGEXIT_BIT		0x20
#define DIAGENTER_BIT		0x10
#define DIAGTRACE_BITS		(DIAGEXIT_BIT | DIAGENTER_BIT)
#define DIAGINFO_BITS		0x0f

#ifndef DIAG_FLAG_VAR
#define DIAG_FLAG_VAR	_diagInfo_Level
#endif

#ifdef DIAGALLOUT
#define	DIAGBUGCHECK_OFF
#define DIAGERR_OFF
#define DIAGWARN_OFF
#undef	DIAGTRACE_ON
#undef	DIAGINFO_ON
#define	DIAGSTART()
#define	DIAGDISPATCH()
#define	DIAGFINISH()
#define	DIAGENABLE( type, level )
#define	DIAGDISABLE( type )
#else
#define	DIAGSTART()
#define	DIAGDISPATCH()
#define	DIAGFINISH()
#define	DIAGENABLE( type, level ) 
#define	DIAGDISABLE( type )	 
#endif



#ifdef DIAGERR_OFF
#define DIAGERR(m)			/* deActivate all DIAGERR's */
#define DIAGERR0(m)			/* deActivate all DIAGERR's */
#define DIAGERR1(m,p1)
#define DIAGERR2(m,p1,p2)
#define DIAGERR3(m,p1,p2,p3)
#define DIAGERR4(m,p1,p2,p3,p4)
#define DIAGERR5(m,p1,p2,p3,p4,p5)
#define DIAGERR6(m,p1,p2,p3,p4,p5,p6)
#else
#define DIAGERR(m)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR0(m)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR1(m,p1)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG3(CE_CONT,m,p1);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR2(m,p1,p2)		DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG4(CE_CONT,m,p1,p2);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR3(m,p1,p2,p3)		DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DEBUG_DLGC_MSG5(CE_CONT,m,p1,p2,p3);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR4(m,p1,p2,p3,p4)		DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR5(m,p1,p2,p3,p4,p5)	DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);     \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGERR6(m,p1,p2,p3,p4,p5,p6)	DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);  \
					DLGC_MSG2(CE_CONT,"\n");

#endif

#ifdef DIAGERR_OFF
#define DIAGFAIL(m)			DIAGEXIT()
#define DIAGFAIL0(m)			DIAGEXIT()
#define DIAGFAIL1(m,p1)			DIAGEXIT()
#define DIAGFAIL2(m,p1,p2)		DIAGEXIT()
#define DIAGFAIL3(m,p1,p2,p3)		DIAGEXIT()
#define DIAGFAIL4(m,p1,p2,p3,p4)	DIAGEXIT()
#define DIAGFAIL5(m,p1,p2,p3,p4,p5)	DIAGEXIT()
#define DIAGFAIL6(m,p1,p2,p3,p4,p5,p6)	DIAGEXIT()
#else
#define DIAGFAIL(m)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL0(m)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL1(m,p1)			DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG3(CE_CONT,m,p1);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL2(m,p1,p2)		DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG4(CE_CONT,m,p1,p2);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL3(m,p1,p2,p3)		DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG5(CE_CONT,m,p1,p2,p3);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL4(m,p1,p2,p3,p4)	DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL5(m,p1,p2,p3,p4,p5)	DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);     \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#define DIAGFAIL6(m,p1,p2,p3,p4,p5,p6)	DLGC_MSG4(CE_CONT,"_ERROR  : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);  \
					DLGC_MSG2(CE_CONT,"\n");		\
					DIAGEXIT()
#endif

#ifdef DIAGBUGCHECK_OFF
#define DIAGBUGCHECK(m)			/* deActivate all DIAGBUGCHECK's */
#define DIAGBUGCHECK0(m)
#define DIAGBUGCHECK1(m,p1)
#define DIAGBUGCHECK2(m,p1,p2)
#define DIAGBUGCHECK3(m,p1,p2,p3)
#define DIAGBUGCHECK4(m,p1,p2,p3,p4)
#define DIAGBUGCHECK5(m,p1,p2,p3,p4,p5)
#define DIAGBUGCHECK6(m,p1,p2,p3,p4,p5,p6)
#else
#define DIAGBUGCHECK(m)			DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK0(m)		DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK1(m,p1)		DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG3(CE_CONT,m,p1);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK2(m,p1,p2)		DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG4(CE_CONT,m,p1,p2);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK3(m,p1,p2,p3)	DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG5(CE_CONT,m,p1,p2,p3);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK4(m,p1,p2,p3,p4)	DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK5(m,p1,p2,p3,p4,p5)	DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);     \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGBUGCHECK6(m,p1,p2,p3,p4,p5,p6) DLGC_MSG4(CE_CONT,"_BUGchk : %s::%s() ",   \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);  \
					DLGC_MSG2(CE_CONT,"\n");

#endif

#ifdef DIAGWARN_OFF
#define DIAGWARN(m)
#define DIAGWARN0(m)
#define DIAGWARN1(m,p1)
#define DIAGWARN2(m,p1,p2)
#define DIAGWARN3(m,p1,p2,p3)
#define DIAGWARN4(m,p1,p2,p3,p4)
#define DIAGWARN5(m,p1,p2,p3,p4,p5)
#define DIAGWARN6(m,p1,p2,p3,p4,p5,p6)
#else
#define DIAGWARN(m)			DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN0(m)			DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN1(m,p1)			DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG3(CE_CONT,m,p1);		       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN2(m,p1,p2)		DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG4(CE_CONT,m,p1,p2);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN3(m,p1,p2,p3)		DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG5(CE_CONT,m,p1,p2,p3);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN4(m,p1,p2,p3,p4)	DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	       \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN5(m,p1,p2,p3,p4,p5)	DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);     \
					DLGC_MSG2(CE_CONT,"\n");

#define DIAGWARN6(m,p1,p2,p3,p4,p5,p6)	DLGC_MSG4(CE_CONT,"_WARNING: %s::%s() ", \
						MODULE_NAME, _diag_FuncName ); \
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);  \
					DLGC_MSG2(CE_CONT,"\n");

#endif




#ifdef DIAGINFO_ON
#ifndef	__DIAGKGLOB_H__
extern int	DIAG_FLAG_VAR;
#endif

#if DIAGINFO_ON < 2
#define DIAGINFO_LEVEL 9
#endif

#if DIAGINFO_ON > 9
#define DIAGINFO_LEVEL 9
#endif

#ifndef DIAGINFO_LEVEL
#define DIAGINFO_LEVEL DIAGINFO_ON
#endif



#define DIAGINFO(l,m)			if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO0(l,m)			if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG2(CE_CONT,m);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO1(l,m,p1)		if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG3(CE_CONT,m,p1);		       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO2(l,m,p1,p2)		if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG4(CE_CONT,m,p1,p2);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO3(l,m,p1,p2,p3)		if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG5(CE_CONT,m,p1,p2,p3);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO4(l,m,p1,p2,p3,p4)	if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	       \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO5(l,m,p1,p2,p3,p4,p5)	if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);     \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#define DIAGINFO6(l,m,p1,p2,p3,p4,p5,p6) if (l <= (DIAGINFO_BITS & DIAG_FLAG_VAR)) {	\
					DLGC_MSG4(CE_CONT,"_INFO.%d : %s::%s() ",\
						l,MODULE_NAME,_diag_FuncName); \
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);  \
					DLGC_MSG2(CE_CONT,"\n");		\
					}

#else
#define DIAGINFO(l,m)			/* deActivated */
#define DIAGINFO0(l,m)
#define DIAGINFO1(l,m,p1)
#define DIAGINFO2(l,m,p1,p2)
#define DIAGINFO3(l,m,p1,p2,p3)
#define DIAGINFO4(l,m,p1,p2,p3,p4)
#define DIAGINFO5(l,m,p1,p2,p3,p4,p5)
#define DIAGINFO6(l,m,p1,p2,p3,p4,p5,p6)
#endif



#ifndef DIAGALLOUT
#ifdef DIAGTRACE_ON
#define DIAGENTER(f)			char	*_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, _diag_FuncName );	\
					DLGC_MSG2(CE_CONT,"");			\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER0(f)			char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG2(CE_CONT,"");			\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER1(f,m,p1)		char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG3(CE_CONT,m,p1);			\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER2(f,m,p1,p2)		char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG4(CE_CONT,m,p1,p2);		\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER3(f,m,p1,p2,p3)	char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG5(CE_CONT,m,p1,p2,p3);		\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER4(f,m,p1,p2,p3,p4)  char    *_diag_FuncName=f;			\
				     if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG6(CE_CONT,m,p1,p2,p3,p4);	\
					DLGC_MSG2(CE_CONT,"\n");			\
				     }

#define DIAGENTER5(f,m,p1,p2,p3,p4,p5)	char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG7(CE_CONT,m,p1,p2,p3,p4,p5);	\
					DLGC_MSG2(CE_CONT,"\n");			\
					}

#define DIAGENTER6(f,m,p1,p2,p3,p4,p5,p6) char    *_diag_FuncName=f;		\
					if (DIAGENTER_BIT & DIAG_FLAG_VAR) {	\
					DLGC_MSG4(CE_CONT,"_Enter  \\ %s::%s() ",\
						MODULE_NAME, f );		\
					DLGC_MSG8(CE_CONT,m,p1,p2,p3,p4,p5,p6);	\
					DLGC_MSG2(CE_CONT,"\n");			\
					}


#define DIAGEXIT()		if (DIAGEXIT_BIT & DIAG_FLAG_VAR) {		\
				   DLGC_MSG4(CE_CONT,"_Exit   / %s::%s() ",	\
						MODULE_NAME, _diag_FuncName );	\
				   DLGC_MSG2(CE_CONT,"");			\
				   DLGC_MSG2(CE_CONT,"\n");			\
				}

#define DIAGEXIT1(m,rtnval)	if (DIAGEXIT_BIT & DIAG_FLAG_VAR) {		\
				   DLGC_MSG4(CE_CONT,"_Exit   / %s::%s() ",	\
						MODULE_NAME, _diag_FuncName );	\
				   DLGC_MSG3(CE_CONT,m,rtnval);			\
				   DLGC_MSG2(CE_CONT,"\n");			\
				}

#else
#define DIAGENTER(f)			char    *_diag_FuncName=f;
#define DIAGENTER0(f)			char    *_diag_FuncName=f;
#define DIAGENTER1(f,m,p1)		char    *_diag_FuncName=f; 
#define DIAGENTER2(f,m,p1,p2)		char    *_diag_FuncName=f;
#define DIAGENTER3(f,m,p1,p2,p3)	char    *_diag_FuncName=f;
#define DIAGENTER4(f,m,p1,p2,p3,p4)	char    *_diag_FuncName=f;
#define DIAGENTER5(f,m,p1,p2,p3,p4,p5)	char    *_diag_FuncName=f;
#define DIAGENTER6(f,m,p1,p2,p3,p4,p5,p6) char    *_diag_FuncName=f;

#define DIAGEXIT()
#define DIAGEXIT1(m,rtnval)

#endif
#else		/* DIAGALLOUT */

#define DIAGENTER(f)
#define DIAGENTER0(f)
#define DIAGENTER1(f,m,p1)
#define DIAGENTER2(f,m,p1,p2)
#define DIAGENTER3(f,m,p1,p2,p3)
#define DIAGENTER4(f,m,p1,p2,p3,p4)
#define DIAGENTER5(f,m,p1,p2,p3,p4,p5)
#define DIAGENTER6(f,m,p1,p2,p3,p4,p5,p6)
#define DIAGEXIT()
#define DIAGEXIT1(m,rtnval)
#endif


#endif
