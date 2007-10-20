#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/file_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/file_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: file_print.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/malloc.h"

char * parse_file = NULLCP;
RDN parse_rdn = NULLRDN;
extern LLog * log_dsap;
extern AttributeType last_at;
extern int avs_count;
extern char dsa_mode;

static char used_temp;

#ifndef SYS5
/* ARGSUSED */
#endif

int dflt_attr_file (at,x,full)
AttributeType  at;
AttributeValue x;
char full;
{
	/* make default file name */
char buffer [LINESIZE];
PS sps;
char *pathend = NULLCP;
char val, *_isodefile();
char *path = NULLCP;
struct file_syntax * fs;
unsigned last_heap;
int save_count;

	used_temp = FALSE;

	fs = (struct file_syntax *) x->av_struct;

	if ((parse_file != NULLCP) && ((pathend = rindex (parse_file,'/')) != NULLCP)) {
		val = *++pathend;
		*pathend = 0;
		path = parse_file;
	}

	last_heap = mem_heap;	/* put PS on general heap */
	mem_heap = 0;

	bzero (buffer,LINESIZE);

	sps = ps_alloc (str_open);
	if (str_setup (sps,buffer,LINESIZE,1) == NOTOK) {
		mem_heap = last_heap;
		ps_free (sps);
		return (NOTOK);
	}
	mem_heap = last_heap;

	if (parse_rdn == NULLRDN) {
		ps_printf (sps,"/tmp/%s_XXXXXX",at->oa_ot.ot_name);
		used_temp = TRUE;
		fs->fs_name = mktemp(strdup (buffer));
		if (pathend != NULLCP)
			*pathend = val;
		return (OK);
	} else {
		save_count = avs_count;
		rdn_print (sps,parse_rdn,EDBOUT);
		if ((avs_count = save_count) > 1) {
			ps_printf (sps,".%d.%s",avs_count,at->oa_ot.ot_name);
			used_temp = TRUE;
		} else
			ps_printf (sps,".%s",at->oa_ot.ot_name);
	}
	*sps->ps_ptr = 0;
	ps_free (sps);

#if	defined(SYS5) && !defined(SVR4)

	   if ((full) && ((int)strlen(buffer) > MAXFILENAMELEN)) {
		char *nptr, *mptr;
		char nbuf [LINESIZE];
		int i;

		nptr = buffer;
		mptr = nbuf;		
		used_temp = TRUE;

		for (i=0 ; (*nptr!=0) && (i < MAXFILENAMELEN-6) ; nptr++)
			if (isalpha(*nptr))
				*mptr++ = *nptr, i++;

		(void) strcpy (mptr,"XXXXXX");
		if (path != NULLCP)
			fs->fs_name = mktemp(strdup (_isodefile(path,nbuf)));
		else
			fs->fs_name = mktemp(strdup (nbuf));
		if (pathend != NULLCP)
			*pathend = val;

		fs->fs_mode = 0;
		return (OK);
	} 
#endif

	if (path != NULLCP)
		fs->fs_name = strdup (_isodefile(path,buffer));
	else
		fs->fs_name = strdup (buffer);
	if (pathend != NULLCP)
		*pathend = val;

	return (OK);
}



fileattr_print (ps,y,format)
PS ps;
AttributeValue y;
int format;
{
struct file_syntax * fs;
int um;
AttributeType save_at;

	fs = (struct file_syntax *) y->av_struct;

	if (format != EDBOUT) {
		if (fs->fs_attr) 
			AttrV_print (ps,fs->fs_attr,format);
		else 
			ps_print (ps,"Internal error, need to load the file!!!");
	} else {
		FILE * fptr;
		PS fps;

		ps_print (ps,"{FILE}");

		if (fs->fs_name == NULLCP) {
			save_at = last_at;
			if (dflt_attr_file (last_at,y,0) == NOTOK) {
				LLOG (log_dsap,LLOG_EXCEPTIONS,("Could not make default attribute file"));
				last_at = save_at;
				return;
			}
			last_at = save_at;
			if (used_temp) {
				ps_print (ps,fs->fs_name);
				fs->fs_mode &= ~FS_DEFAULT;
			}
		} else if (! (fs->fs_mode & FS_DEFAULT))
			ps_print (ps,fs->fs_name);

		if (fs->fs_attr == NULLAttrV)	
			/* already exists */
			return;

		if (fs->fs_mode & FS_CREATE)	/* already written */
			return;	

	        um = umask (0177);
		if ((fptr = fopen (fs->fs_name,"w")) != NULL) {
		        (void) umask (um);
			if ((fps = ps_alloc (std_open)) == NULLPS) {
				(void) fclose (fptr);
				LLOG (log_dsap,LLOG_EXCEPTIONS,("Could not alloc PS file '%s'",fs->fs_name));
				return;
			}				
			if ((std_setup (fps,fptr)) == NOTOK) {
				(void) fclose (fptr);
				ps_free (fps);
				LLOG (log_dsap,LLOG_EXCEPTIONS,("Could not open PS file '%s'",fs->fs_name));
				return;
			}
		} else {
		        (void) umask (um);
			LLOG ( log_dsap,LLOG_EXCEPTIONS,("Could not open attribute file '%s'",fs->fs_name));
			return;
		}
		AttrV_print (fps,fs->fs_attr,FILEOUT);
		if (dsa_mode) {
			AttrV_free (fs->fs_attr);
			fs->fs_attr = NULLAttrV;
		}
		(void) fclose (fptr);
		ps_free (fps);
		fs->fs_mode |= FS_CREATE;

		DLOG (log_dsap,LLOG_DEBUG,("Written photo file '%s'",fs->fs_name));
	}
	return;

}

void as_write_files (as,where)
Attr_Sequence as;
char *where;
{
struct file_syntax * fs;
int um;
AV_Sequence avs;
FILE * fptr;
PS fps;
char buffer[LINESIZE];
static unsigned long loopcount = 0;

	for ( ; as != NULLATTR; as = as -> attr_link ) 

	  for ( avs = as -> attr_value ; avs != NULLAV ; 
		  avs = avs -> avseq_next )

	    if ( avs -> avseq_av.av_syntax == AV_FILE ) {

		fs = (struct file_syntax *) avs->avseq_av.av_struct;

		if (fs->fs_name != NULLCP || fs->fs_attr == NULLAttrV)
		        continue ;

		(void) sprintf (buffer,"%s%D_%s",where,loopcount++,
				as -> attr_type -> oa_ot.ot_name);

		fs->fs_name = strdup(buffer);

	        um = umask (0177);
		if ((fptr = fopen (fs->fs_name,"w")) != NULL) {
		        (void) umask (um);
			if ((fps = ps_alloc (std_open)) == NULLPS) {
				(void) fclose (fptr);
				LLOG (log_dsap,LLOG_EXCEPTIONS,
				      ("Could not alloc PS file (tmp) '%s'",
				       fs->fs_name));
				free (fs->fs_name);
				fs->fs_name = NULLCP;
				return;
			}				
			if ((std_setup (fps,fptr)) == NOTOK) {
				(void) fclose (fptr);
				ps_free (fps);
				LLOG (log_dsap,LLOG_EXCEPTIONS,
				      ("Could not open PS file (tmp2) '%s'",
				       fs->fs_name));
				free (fs->fs_name);
				fs->fs_name = NULLCP;
				return;
			}
		} else {
		        (void) umask (um);
			LLOG ( log_dsap,LLOG_EXCEPTIONS,
			      ("Could not open attribute file (tmp) '%s'",
			       fs->fs_name));
			return;
		}
		AttrV_print (fps,fs->fs_attr,FILEOUT);

		fs->fs_mode &= ~FS_DEFAULT;
		fs->fs_mode |= FS_CREATE;
		fs->fs_mode |= FS_TMP;

		AttrV_free (fs->fs_attr);
		fs->fs_attr = NULLAttrV;

		(void) fclose (fptr);
		ps_free (fps);

		DLOG (log_dsap,LLOG_DEBUG,
		      ("Written photo file (tmp) '%s'",fs->fs_name));
	    }
}

