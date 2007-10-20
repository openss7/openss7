/* fred.h - definitions for fred */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/fred.h,v 9.0 1992/06/16 12:44:30 isode Rel $
 *
 *
 * $Log: fred.h,v $
 * Revision 9.0  1992/06/16  12:44:30  isode
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
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*    MAIN */

extern int  interrupted;
extern int  oneshot;

extern	SFP	istat;
extern	SFP	qstat;

extern LLog _fred_log, *fred_log;


void	adios (), advise ();
int	ask (), getline ();
char   *strdup ();

/*    DATA */

extern int	bflag;
extern int	boundP;
extern int	debug;
extern int	fflag;
extern int	kflag;
extern int	mail;
extern int	nametype;
extern int	network;
extern int	phone;
extern int	query;
extern int	readonly;
extern int	soundex;
extern int	timelimit;
extern int	ufn_options;
extern int	verbose;
extern int	watch;

extern int	didbind;
extern int	dontpage;
extern int	rcmode;
extern int	runcom;
extern int	runsys;

extern char    *manager;
extern char    *pager;
extern char    *server;

extern char    *myarea;
extern char    *mydn;
extern char    *myhome;
extern char    *myuser;


extern FILE    *stdfp;
extern FILE    *errfp;


#define	EOLN	(network && !mail ? "\r\n" : "\n")

/*    DISPATCH */

struct dispatch {
    char   *ds_name;
    IFP	    ds_fnx;

    int	    ds_flags;
#define	DS_NULL	0x00
#define	DS_USER	0x01
#define	DS_SYOK	0x02

    char   *ds_help;
};

/*    MISCELLANY */

struct area_guide {
    int	    ag_record;
#define	W_ORGANIZATION	0x01
#define	W_UNIT		0x02
#define	W_LOCALITY	0x03
#define	W_PERSON	0x04
#define	W_DSA		0x05
#define	W_ROLE		0x06
    char   *ag_key;

    char   *ag_search;

    char   *ag_class;
    char   *ag_rdn;

    char   *ag_area;
};

extern int  area_quantum;
extern struct area_guide areas[];

/*    WHOIS */

extern char *whois_help[];

/*  */

extern int  errno;

