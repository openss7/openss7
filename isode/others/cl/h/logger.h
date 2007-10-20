/* logger.h - logging routines */

/* 
 * $Header: /f/iso/h/RCS/logger.h,v 5.0 88/07/21 14:39:04 mrose Rel $
 *
 *
 * $Log$
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

/*  */

#ifndef	_LOGGER_
#define	_LOGGER_

#ifndef	BSD42

#define	LOG_PID		0x01	/* log process-id */
#define	LOG_TIME	0x02	/* log time-of-day */

#define	LOG_ALERT	1	/* alert */
#define	LOG_SALERT	2	/* subalert */
#define	LOG_ERR		4	/* error */
#define	LOG_WARNING	6	/* warning */
#define	LOG_INFO	8	/* information */
#define	LOG_DEBUG	9	/* debugging */

#else
#include <syslog.h>
#endif

void	setlog (), openlog (), closelog (), syslog (), _syslog ();

#endif
