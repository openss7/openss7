/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * Contributed by Oliver Wenzel 1990
 *
 * config.h - installation dependant configuration
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/config.h,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: config.h,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/*--- default root for RFA tree (can be changed at runtime) ---*/
#define FS_BASE		"/usr/something"

/*--- dir where isotailor and .rfarc files are expected ---*/
#define RFA_TAILDIR     "/usr/local/isode/etc"

/*--- timeout in sec. for lock operation of .rfainfo file  ---*/
#define LOCK_TIMEOUT    30

/*--- number of secs. for sending the time during the timeSync operation ---*/
#define SENDTIME_DELAY	1

/*--- default limit for filesize when compression of file shall occur. 
      (can be changed at runtime) ---*/
#define COMPRESSLIMIT	4500	
/*--- Will be calculated as follows:

	     C0 : time required to start compress process
	     U0 : time required to start compress process
	     C	: time required to compress 1K data
	     U	: time required to compress 1K data
	     T	: time required to transfer 1K data
	     FC : factor by which the data size is reduced by compression
	     L  : Limit when data should be transfered compressed

	           FC * (C0 + U0) 
	     L = ---------------------------
		  (FC - 1) * T - FC * (U + C)

	     With:

		  C0 = 1s
		  CU = 1s
		  C  = 1/50s (1/100 on Sparc with load = 1)
		  U  = 1/70s (1/140 on Sparc with load = 1)
		  T  = 8/10 s (X.25)
		  FC = 2.5

		  then  L =  4.48 Kbytes 
---*/
