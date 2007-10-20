/* hexphoto.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/hexphoto.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/hexphoto.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: hexphoto.c,v $
 * Revision 9.0  1992/06/16  12:43:35  isode
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


#include "quipu/util.h"
#include "quipu/photo.h"
#include "psap.h"

decode_t4 (picture,persons_name, len)
char    *picture;
char    *persons_name;
{
int i;

	if (len == 0) len = photolen (picture);
	
	for (i=0; i<len; i++)
		(void) fprintf (stderr,"%02x",*picture++ & 255);


}
static	photolen (s1)
char * s1;
{
int length=0,cnt,i;
char * temp;

       if (*s1 == 0x03) {
       /* we have a coded picture */

	    temp = s1;
	    temp++;
	    cnt = *temp++ & 0x7f;  /*assume len > 127 for now */
	    for (i=0; i<cnt; i++) 
		length = (length << 8) | (*temp++ & 0xff) ;

	    length += 2 + cnt;
	    return (length);

       } else 
	    return (-1);

}
