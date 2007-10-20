/* jpeg2asn.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/jpeg2asn.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/jpeg2asn.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: jpeg2asn.c,v $
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


/*
 *
 * jpeg2asn - takes a file, and adds the ASN-1 stuff to the beginning
 *
 * Russ Wright - Lawrence Berkeley Laboratory-  Oct 1991
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "general.h"

#include "jpeg.h"

#define ASN_LONG_LEN    (0x80)

void            GotError();

main(argc, argv)
int             argc;
char           *argv[];
{
    int             i;
    unsigned long   length,
                    nLength;
    FILE           *infile,
                   *outfile;
    struct stat     buf;



    if (argc != 2)
    {
        (void) printf("Usage: jpeg2asn <input file>\n");
        (void) printf("Takes a normal JPEG file and adds ASN-1 tag and length\n");
        (void) printf("and prints it to standard output\n");
        exit(0);
    }

    if (!(infile = fopen(argv[1], "r")))
    {
        GotError("Can not open input file");
    }

    outfile = stdout;

    /* first get the length of the source file */

    if (lstat(argv[1], &buf))
        GotError("lstat failed");

    length = buf.st_size;

    /* First thing, add the JPEG tag */

    (void) putc(NEW_JPEG_TAG, outfile);

    /* Has not been tested on a machine were net and host ordering is
     * different - e.g. a VAX */


/* time to put ASN-1 length down */

    nLength = length;
    if (length < 0x80)          /* fits in 7 bits */
        (void) putc((unsigned char) length, outfile);
    else if (length <= 0xFF)    /* 1 byte of length */
    {
        (void) putc((unsigned char) (0x01 | ASN_LONG_LEN), outfile);
        (void) putc((unsigned char) length, outfile);
    }
    else if (length <= 0xFFFF)  /* 2 bytes of length */
    {
        (void) putc((unsigned char) (0x02 | ASN_LONG_LEN), outfile);
        (void) putc((unsigned char) ((nLength >> 8) & 0xFF), outfile);
        (void) putc((unsigned char) (nLength & 0xFF), outfile);
    }
    else if (length <= 0xFFFFFF)/* 3 bytes of length */
    {
        (void) putc((unsigned char) (0x03 | ASN_LONG_LEN), outfile);
        (void) putc((unsigned char) ((nLength >> 16) & 0xFF), outfile);
        (void) putc((unsigned char) ((nLength >> 8) & 0xFF), outfile);
        (void) putc((unsigned char) (nLength & 0xFF), outfile);
    }
    else /* 4 bytes of length */
    {
        (void) putc((unsigned char) (0x04 | ASN_LONG_LEN), outfile);
        (void) putc((unsigned char) ((nLength >> 24) & 0xFF), outfile);
        (void) putc((unsigned char) ((nLength >> 16) & 0xFF), outfile);
        (void) putc((unsigned char) ((nLength >> 8) & 0xFF), outfile);
        (void) putc((unsigned char) (nLength & 0xFF), outfile);
    }

    /* Excellent, all we have to do now is to copy the file over ! */

    while ((i = getc(infile)) != EOF)
        (void) putc(i, outfile);

    (void) fclose(infile);
    (void) fclose(outfile);

    return 0;
}

void
GotError(theStr)
char           *theStr;

{
    (void) perror(theStr);
    exit(1);
}
