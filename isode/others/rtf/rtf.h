/* rtf.h - definitions for RT-file transfer utility */

/* 
 * $Header: /xtel/isode/isode/others/rtf/RCS/rtf.h,v 9.0 1992/06/16 12:48:07 isode Rel $
 *
 *
 * $Log: rtf.h,v $
 * Revision 9.0  1992/06/16  12:48:07  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include "rtsap.h"
#include "logger.h"
#include "RTF-types.h"
#ifdef	NULL
#undef	NULL
#endif
#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif
#include "sys.file.h"
#include <sys/stat.h>


extern LLog *pgm_log;

/*  */

char   *SReportString ();

void	rts_adios (), rts_advise ();
void	adios (), advise (), ryr_advise ();
