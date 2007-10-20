/* imagesbr.h - include file for image subroutines */

/* 
 * $Header: /xtel/isode/isode/others/quipu/image/RCS/imagesbr.h,v 9.0 1992/06/16 12:43:24 isode Rel $
 *
 *
 * $Log: imagesbr.h,v $
 * Revision 9.0  1992/06/16  12:43:24  isode
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


#include "psap.h"
#include "isoaddrs.h"
#include "logger.h"

/*  */

extern int   debug;
extern int   errsw;

/* GENERAL */

extern int   recording;
extern LLog *pgm_log;


/* AKA */

void	init_aka ();

/* DIRECTORY */


/* IMAGE */

struct type_IMAGE_Image {
    int     width;

    int     height;

    struct qbuf *data;
};

struct type_IMAGE_Image *fetch_image ();


/* ERRORS */

void	adios (), advise ();


/* MISC */

char   *strdup ();
