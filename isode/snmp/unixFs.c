/* unixFs.c - MIB realization of the filesystem group

 	donated by Michael Davidson/UTK  <davidson@cs.utk.edu> */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/unixFs.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/unixFs.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: unixFs.c,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
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


/*    PRINT */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#include "smux.h"
#include "objects.h"
#include "logger.h"

#if !(defined(ultrix) && defined(mips))
#include <mntent.h>
#else
#include <sys/types.h>
#include <sys/fs_types.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif
#include <sys/stat.h>
#if !(defined(ultrix) && defined(mips))
#include <sys/vfs.h>
#else
/* 
 * forge setmntent() and getmntent() calls by use of the
 * appropriate calls onto getmountent(start, buffer, nentries) 
 * see man (3) getmountent() on Ultrix for details.
 */

static	int mnt_cnt = 0;	/* global count of offset in getmountent() */

 /*
  * mntent struct snarfed from *nix elsewhere
  */
static struct mntent{
                 char  *mnt_fsname;  /* name of mounted file system */
                 char  *mnt_dir;     /* file system path prefix */
                 char  *mnt_type;    /* MNTTYPE_* */
                 char  *mnt_opts;    /* MNTOPT* */
                 int   mnt_freq;     /* dump frequency, in days */
                 int   mnt_passno;   /* pass number on parallel fsck */
       };
FILE 		*setmntent();
FILE 		*endmntent();
struct mntent	*getmntent();
#endif

#ifdef HPUX
#define MNT_FILE	"/etc/mnttab"
#else
#define MNT_FILE	"/etc/mtab"
#endif

#define fsIdentifier		0
#define fsName			1
#define fsMountPoint		2
#define fsMountType 		3
#define fsMountOptions		4
#define fsBlockSize		5
#define fsBlockCount		6
#define fsBlocksFree		7
#define fsBlocksAvailable 	8
#define fsInodeCount		9
#define fsInodesAvailable 	10
#define fsMAX			fsInodesAvailable

#define NONAME			int_SNMP_error__status_noSuchName
#define GENERR			int_SNMP_error__status_genErr

/*
 * given an object identifier and the associated object type,
 *     PTR: return the adress of the instance portion of the oid
 *     SZ:  return the length ot the instance portion of the oid
 */
#define INST_PTR(oid,ot)	(oid->oid_elements + ot->ot_name->oid_nelem)
#define INST_SZ(oid,ot)		(oid->oid_nelem - ot->ot_name->oid_nelem)


#define MAX_INST	3
struct fs {
	unsigned fs_instance[MAX_INST];
	int	fs_insize;

	struct fs *next;

	int	fs_Identifier;
	char	*fs_Name;
	char	*fs_MountPoint;
	char	*fs_MountType;
	char	*fs_MountOptions;
	int	fs_BlockSize;	
	int	fs_BlockCount;
	int	fs_BlocksFree;
	int	fs_BlocksAvailable;
	int	fs_InodeCount;
	int	fs_InodesAvailable;
};
struct fs *fs_tbl = NULL ;


extern int quantum;

int init_unix_fs(); 
int sync_unix_fs();

static struct fs *get_fsent();
static int  get_fs_table();
static void free_fs_table();
static int  o_unix_fs();
static void refresh_entry();


/*-----------------------------------------------------------------
 * Come here to retreive any variable from this mib group
 *-----------------------------------------------------------------*/
static int o_unix_fs(oi, v, offset)
OI  oi;
struct type_SNMP_VarBind *v;
int offset;
{
	OID  new;
	OID  oid   = oi->oi_name;
	OT   ot    = oi->oi_type;
	int  i;
	unsigned *ip, *jp;
	struct fs *fs;

	/*** fetch the correct row.  ***/

	switch( offset ){
		case type_SNMP_SMUX__PDUs_get__request:
			fs = get_fsent(INST_PTR(oid,ot),INST_SZ(oid,ot), 0);
			if( fs == NULL )
				return(NONAME);

			break;

		case type_SNMP_SMUX__PDUs_get__next__request:
try_again:
			fs = get_fsent(INST_PTR(oid,ot),INST_SZ(oid,ot), 1);
			if( fs == NULL ){
				if( ((int)ot->ot_info + 1) > fsMAX )
					return(NONAME);

				oid->oid_nelem -= INST_SZ(oid,ot);	
				oid->oid_elements[(oid->oid_nelem - 1)]++;

				if( (ot = name2obj(oid)) == NULLOT )
					return(GENERR);

				goto try_again;
			}

			/** change instance suffix of oid **/
	
			new = oid_extend(oid, fs->fs_insize - INST_SZ(oid,ot) );
			if( new == NULLOID)
				return( NOTOK );

			ip = new->oid_elements + new->oid_nelem - fs->fs_insize;
			jp = fs->fs_instance;
			for( i = fs->fs_insize; i > 0; i-- )
				*ip++ = *jp++;

			if( v->name )
				free_SNMP_ObjectName( v->name );
			v->name = new;

			oid = new;
			oi->oi_type = ot;
			break;

		default:
			return( int_SNMP_error__status_genErr );
	}

	/*** Now, return the particular variable. ***/

	switch( (int)ot->ot_info ){
		case fsIdentifier:
			return( o_integer(oi, v, fs->fs_Identifier) );
		case fsName:
			return( o_string(oi, v, fs->fs_Name, 
			    strlen(fs->fs_Name)) );
		case fsMountPoint:
			return( o_string(oi, v, fs->fs_MountPoint,
			    strlen(fs->fs_MountPoint)) );
		case fsMountType:
			return( o_string(oi, v, fs->fs_MountType,
			    strlen(fs->fs_MountType)) );
		case fsMountOptions:
			return( o_string(oi, v, fs->fs_MountOptions, 
			    strlen(fs->fs_MountOptions)) );
		case fsBlockSize:
			return( o_integer(oi, v, fs->fs_BlockSize) );
		case fsBlockCount:
			return( o_integer(oi, v, fs->fs_BlockCount) );
		case fsBlocksFree:
			return( o_integer(oi, v, fs->fs_BlocksFree) );
		case fsBlocksAvailable:
			return( o_integer(oi, v, fs->fs_BlocksAvailable) );
		case fsInodeCount:
			return( o_integer(oi, v, fs->fs_InodeCount) );
		case fsInodesAvailable:
			return( o_integer(oi, v, fs->fs_InodesAvailable) );
		default:
			return( int_SNMP_error__status_noSuchName );
	}
}


/*-----------------------------------------------------------------
 * Initialize each node in the object identifier tree.
 *-----------------------------------------------------------------*/
int init_unix_fs() 
{
	OT ot;

	if (ot = text2obj("fsIdentifier"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsIdentifier;

	if (ot = text2obj("fsName"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsName;

	if (ot = text2obj("fsMountPoint"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsMountPoint;

	if (ot = text2obj("fsMountType"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsMountType;

	if (ot = text2obj("fsMountOptions"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsMountOptions;

	if (ot = text2obj("fsBlockSize"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsBlockSize;

	if (ot = text2obj("fsBlockCount"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info = (caddr_t)fsBlockCount;

	if (ot = text2obj("fsBlocksFree"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsBlocksFree;

	if (ot = text2obj("fsBlocksAvailable"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsBlocksAvailable;

	if (ot = text2obj("fsInodeCount"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsInodeCount;

	if (ot = text2obj("fsInodesAvailable"))
		ot->ot_getfnx = o_unix_fs,
		ot->ot_info   = (caddr_t)fsInodesAvailable;

	return(1);
}


/*-----------------------------------------------------------------
 * Perform commit/rollback operations. (this mib group has no 
 * set operations so there are not commit/rollback operations)
 *-----------------------------------------------------------------*/
int sync_unix_fs( cor )
integer	cor;
{
	switch (cor) {
		case int_SNMP_SOutPDU_commit:
			return(1);

		case int_SNMP_SOutPDU_rollback:
			return(1);
	}
}


/*-----------------------------------------------------------------
 * Find an entry in the file system table.
 *-----------------------------------------------------------------*/
static struct fs *get_fsent( ip, len, isnext )
unsigned *ip;
int len;
int isnext;
{
	static long last = 0;
	static int lastq = -1;
	int refresh = 1;
	struct fs *fsp;
	struct stat st;

	if( lastq == quantum ){
		refresh = 0;
	}
	else {
		if( stat( MNT_FILE, &st) == -1 || last != st.st_mtime ){
			last = st.st_mtime;
			refresh = 0;
			free_fs_table();
			get_fs_table();
		}
	}
	lastq = quantum;

	for(fsp = fs_tbl; fsp != NULL; fsp = fsp->next ){
		switch(elem_cmp(fsp->fs_instance, fsp->fs_insize, ip, len) ){
			case 0:
				if( !isnext ){
					if( refresh ) 
						refresh_entry(fsp);
					return(fsp);
				}

				fsp = fsp->next;
				if( fsp == NULL )
					return(NULL);

				/* else fall */
			case 1:
				if( isnext ){
					if( refresh )
						refresh_entry(fsp);
					return(fsp);
				}

				return(NULL);
		}
	}

	return(NULL);
}


/*-----------------------------------------------------------------
 * Simple insertion sort of file system table.
 *-----------------------------------------------------------------*/
static void insert_entry(fsp)
struct fs *fsp;
{
	struct fs *cur   = fs_tbl;
	struct fs **last = &fs_tbl;

	for(;;){
		if( cur == NULL ){
			*last = fsp;
			fsp->next = NULL;
			return;
		}

		switch( elem_cmp(cur->fs_instance, cur->fs_insize, 
				 fsp->fs_instance, fsp->fs_insize) ){
			case 0:
				/* WARNING: duplicate entry */
				fsp->fs_instance[fsp->fs_insize - 1] += 100;
				insert_entry(fsp);
				return;
			case 1:
				*last = fsp;
				fsp->next = cur;
				return;
		}

		last = &(cur->next);
		cur  = cur->next;
	}
}


#ifdef BSDSTRS
/*-----------------------------------------------------------------
 * some systems (sun386) do not have this funtion 
 *-----------------------------------------------------------------*/
char * strstr(s1, s2)
register char *s1, *s2;
{
	int s2len = strlen(s2); /* length of the second string */

	if (s2len == 0)
		return (s1);

	while (strlen(s1) >= s2len) {
		if (strncmp(s1, s2, s2len) == 0)
			return (s1);
		s1++;
	}
	return (0);
}
#endif


/*-----------------------------------------------------------------
 * Fill the entire file system table. (call with fs_tbl == NULL)
 *-----------------------------------------------------------------*/
static int get_fs_table()
{
	static int fake_dev  = 0;
	FILE *mfile;
	struct mntent *mp;
	struct fs *fsp, *lfsp=NULL;
	char *p;


	if( (mfile = setmntent(MNT_FILE, "r")) == NULL )
		return(0);

	while( (mp = getmntent(mfile)) != NULL ){
		fsp = (struct fs *)malloc( sizeof(struct fs) );
		if( fsp == NULL ){
			endmntent(mfile);
			return(0);
		}

		fsp->fs_Name = (char *)malloc( strlen(mp->mnt_fsname)+1 );
		if( fsp->fs_Name == NULL ){
			endmntent(mfile);
			free(fsp);
			return(0);
		}

		fsp->fs_MountPoint = (char *)malloc( strlen(mp->mnt_dir)+1 );
		if( fsp->fs_MountPoint == NULL ){
			endmntent(mfile);
			free(fsp->fs_Name);
			free(fsp);
			return(0);
		}

		fsp->fs_MountType = (char *)malloc( strlen(mp->mnt_type)+1 );
		if( fsp->fs_MountType == NULL ){
			endmntent(mfile);
			free(fsp->fs_Name);
			free(fsp->fs_MountPoint);
			free(fsp);
			return(0);
		}

		fsp->fs_MountOptions = (char *)malloc( strlen(mp->mnt_opts)+1 );
		if( fsp->fs_MountOptions == NULL ){
			endmntent(mfile);
			free(fsp->fs_Name);
			free(fsp->fs_MountPoint);
			free(fsp->fs_MountType);
			free(fsp);
			return(0);
		}

		strcpy(fsp->fs_Name, mp->mnt_fsname);
		strcpy(fsp->fs_MountPoint, mp->mnt_dir);
		strcpy(fsp->fs_MountType, mp->mnt_type);
		strcpy(fsp->fs_MountOptions, mp->mnt_opts);

		p = strstr(mp->mnt_opts, "dev=");
		fsp->fs_Identifier = (p == NULL) ? ++fake_dev%100 : atoi(p+4);

		fsp->fs_instance[0] = fsp->fs_Identifier;
		fsp->fs_insize =  1;

		refresh_entry(fsp);
		insert_entry(fsp);
	}

	endmntent(mfile);
	return(1);
}


/*-----------------------------------------------------------------
 * Get current statistics for this filesystem.
 *-----------------------------------------------------------------*/
static void refresh_entry(fsp)
struct fs *fsp;
{

#if	!(defined(ultrix) && defined(mips))
	struct statfs  fss;

	if( statfs(fsp->fs_MountPoint, &fss) == -1 ){
		fsp->fs_BlockSize	= -1;
		fsp->fs_BlockCount	= -1;
		fsp->fs_BlocksFree	= -1;
		fsp->fs_BlocksAvailable	= -1;
		fsp->fs_InodeCount	= -1;
		fsp->fs_InodesAvailable	= -1;
	}
	else {
		fsp->fs_BlockSize	= fss.f_bsize;
		fsp->fs_BlockCount	= fss.f_blocks;
		fsp->fs_BlocksFree	= fss.f_bfree;
		fsp->fs_BlocksAvailable	= fss.f_bavail;
		fsp->fs_InodeCount	= fss.f_files;
		fsp->fs_InodesAvailable	= fss.f_ffree;
	}
#else
	struct fs_data  fss;

	if( statfs(fsp->fs_MountPoint, &fss) == -1 ){
		fsp->fs_BlockSize	= -1;
		fsp->fs_BlockCount	= -1;
		fsp->fs_BlocksFree	= -1;
		fsp->fs_BlocksAvailable	= -1;
		fsp->fs_InodeCount	= -1;
		fsp->fs_InodesAvailable	= -1;
	}
	else {
		fsp->fs_BlockSize	= fss.fd_req.bsize;
		fsp->fs_BlockCount	= fss.fd_req.btot;
		fsp->fs_BlocksFree	= fss.fd_req.bfree;
		fsp->fs_BlocksAvailable	= fss.fd_req.bfreen;
		fsp->fs_InodeCount	= fss.fd_req.gtot;
		fsp->fs_InodesAvailable	= fss.fd_req.gfree;
	}
#endif
}


/*-----------------------------------------------------------------
 * Delete the file system table.
 *-----------------------------------------------------------------*/
static void free_fs_table()
{
	struct fs *fs = fs_tbl, *bye;

	while( fs != NULL ){
		bye = fs;
		fs = bye->next;

		free(bye->fs_Name);
		free(bye->fs_MountPoint);
		free(bye->fs_MountType);
		free(bye->fs_MountOptions);
		free(bye);
	}

	fs_tbl = NULL;
}

#if defined(ultrix) && defined(mips)

FILE *
setmntent(mntfile, mode)
char	*mntfile;
char	*mode;
{
	/*
	 * initialize global static counter to zero
	 */
	mnt_cnt = 0;
	return (FILE *)1;
}

struct mntent *
getmntent(mfile)
FILE	*mfile;
{
	static struct fs_data	mnt_buf;
	static struct mntent	fake;
	int	stat;

	switch (stat = getmountent(mnt_cnt, &mnt_buf, 1)) {
		case 1:
                 	fake.mnt_fsname	= mnt_buf.fd_req.devname;	
                 	fake.mnt_dir	= mnt_buf.fd_req.path;     
                 	fake.mnt_type	= gt_names[mnt_buf.fd_req.fstype];    
                 	fake.mnt_opts	= NULL;    
                 	fake.mnt_freq	= -1;    
                 	fake.mnt_passno	= -1;  

		case 0:		/* end of list */
		default:	/* uh-oh... */
			return	(struct mntent *)0;
	}
}

FILE *
endmntent(mntfile)
FILE	*mntfile;
{
	/* nullop */
}
#endif
