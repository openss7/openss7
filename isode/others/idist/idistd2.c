/* idistd2.c -- the non remote operations parts of the protocol */

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/idistd2.c,v 9.0 1992/06/16 14:38:53 isode Rel $
 *
 * Parts of the idist server which are not mixed up with remote
 * operations but depend on the defined types.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 *
 *
 * $Log: idistd2.c,v $
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 * 
 */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/idistd2.c,v 9.0 1992/06/16 14:38:53 isode Rel $";
#endif

#include "defs.h"
#include "Idist-types.h"

char	*tp, *stp[128];

extern	int catname;
extern	char	target[];
extern	FILE	*cfile;
extern	struct type_Idist_FileSpec *cfiletype;
extern	int oumask;
extern	char    utmpfile[];
extern	char    *tmpname;
extern	struct type_Idist_IA5List *ia5list;

static	char	*cannon ();
static	struct type_Idist_IA5List *str2ia5list ();
static	int compare ();
extern	struct type_Idist_FileSpec *makefs ();
extern	struct type_Idist_QueryResult *query ();
extern	struct type_Idist_FileList *do_listcdir ();

doexec (cmd)
char	*cmd;
{
	int fd[2], status, pid, i;
	char	buf[BUFSIZ];

	if (pipe(fd) < 0)
		return NOTOK;

	if ((pid = vfork ()) == 0) {
		/*
		 * Return everything the shell commands print.
		 */
		(void) close(0);
		(void) close(1);
		(void) close(2);
		(void) open("/dev/null", 0);
		(void) dup(fd[1]);
		(void) dup(fd[1]);
		(void) close(fd[0]);
		(void) close(fd[1]);
		(void) execl("/bin/sh", "sh", "-c", cmd, 0);
		_exit(127);
	}
	if (pid == -1)
		return NOTOK;

	(void) close(fd[1]);

	while ((i = read(fd[0], buf, sizeof(buf))) > 0) {
		addtoia5 (buf, i);
	}

	while ((i = wait(&status)) != pid && i != -1)
		;
	if (i == -1)
		status = -1;
	(void) close(fd[0]);

	return OK;
}

do_symlink (fs)
struct type_Idist_FileSpec *fs;
{
	char	*new, old[BUFSIZ], *linkname;
	int	i;

	new = qb2str (fs -> filename);
	linkname = cannon (new);
	free (new);

	new = qb2str (fs -> linkname);
	(void) strcpy (old, new);
	free (new);

	
	if (symlink (old, linkname) < 0) {
		if (errno != ENOENT || chkparent (linkname) < 0 ||
		    symlink (old, linkname) < 0) {
			nadvise (linkname, "Can't symlink %s to", old);
			return NOTOK;
		}
	}

	if (bit_on (fs -> fileopts, bit_Idist_Options_compare)) {
		char	tbuf[BUFSIZ];

		if ((i = readlink (target, tbuf, BUFSIZ)) >= 0 &&
		    i == fs -> filesize &&
		    strncmp (old, tbuf, (int) fs -> filesize) == 0) {
			(void) unlink (linkname);
			return OK;
		}
		if (bit_on (fs -> fileopts, bit_Idist_Options_verify)) {
			(void) unlink (linkname);
			note ("need to update: %s", target);
			return OK;
		}
	}

	if (rename (linkname, target) < 0) {
		nadvise (target, "can't rename %s to", linkname);
		(void) unlink (linkname);
		return NOTOK;
	}
	if (bit_on (fs -> fileopts, bit_Idist_Options_compare))
		note ("updated %s\n", target);
	return OK;
}

static	char	*cannon (name)
char	*name;
{
	static char	nname[BUFSIZ];
	char	*cp;
	extern	char	*tmpname;

	if (catname)
		(void) sprintf (tp, "/%s", name);
	if((cp = rindex (target, '/')) == NULL)
		(void) strcpy (nname, tmpname);
	else if (cp == target)
		(void) sprintf (nname, "/%s", tmpname);
	else {
		*cp = '\0';
		(void) sprintf (nname, "%s/%s", target, tmpname);
		*cp = '/';
	}
	return nname;
}

/*
 * Check to see if parent directory exists and create one if not.
 */
chkparent(name)
	char *name;
{
	register char *cp;
	struct stat stb;

	cp = rindex(name, '/');
	if (cp == NULL || cp == name)
		return(0);
	*cp = '\0';
	if (lstat(name, &stb) < 0) {
		if (errno == ENOENT && chkparent(name) >= 0 &&
		    mkdir(name, 0777 & ~oumask) >= 0) {
			*cp = '/';
			return(0);
		}
	} else if (ISDIR(stb.st_mode)) {
		*cp = '/';
		return(0);
	}
	*cp = '/';
	return(-1);
}

do_rfile (fs)
struct type_Idist_FileSpec *fs;
{
	char	*name, *p;

	p = qb2str (fs -> filename);
	name = cannon (p);
	free (p);

	if ((cfile = fopen (name, "w")) == NULL) {
		if (errno != ENOENT || chkparent (name) < 0 ||
		    (cfile = fopen (name, "w")) == NULL) {
			nadvise (name, "Can't create file");
			return NOTOK;
		}
	}
	(void) fchmod (fileno (cfile), (int)fs -> filemode);

	return OK;
}

do_hardlink (fs)
struct type_Idist_FileSpec *fs;
{
	char 	*new;
	char	*cp;
	char	old[BUFSIZ];
	struct stat stb;
	int	exists = 0;

	new = qb2str (fs -> filename);
	cp = qb2str (fs -> linkname);
	if (exptilde (old, cp) == NULL) {
		free (cp);
		free (new);
		return NOTOK;
	}
	free (cp);

	if (catname)
		(void) sprintf (tp, "/%s", new);
	free (new);

	if (lstat(target, &stb) == 0) {
		int mode = stb.st_mode & S_IFMT;
		if (mode != S_IFREG && mode != S_IFLNK) {
			nadvise (NULLCP, "%s: not a regular file",
				 host, target);
			return NOTOK;
		}
		exists = 1;
	}
	if (chkparent(target) < 0 ) {
		nadvise("chkparent", "%s (no parent)", target);
		return NOTOK;
	}
	if (exists && (unlink(target) < 0)) {
		nadvise ("unlink", "%s", target);
		return NOTOK;
	}
	if (link(old, target) < 0) {
		nadvise (old,  "can't link %s to", target);
		return NOTOK;
	}
	return OK;
}

do_direct (fs)
struct type_Idist_FileSpec *fs;
{
	char	*cp, *name;
	struct stat stb;

	cp = name = qb2str (fs -> filename);

	if (catname >= sizeof(stp)) {
		nadvise (NULLCP, "Too many directory levels");
		free (name);
		return NOTOK;
	}

	stp[catname] = tp;
	if (catname++) {
		*tp ++ = '/';
		while (*tp++ = *cp ++)
			;
		tp --;
	}
	
	if (bit_on (fs -> fileopts, bit_Idist_Options_verify)) {
		free (name);
		return OK;
	}

	if (lstat (target, &stb) == 0) {
		if ((stb.st_mode & S_IFMT) == S_IFDIR) {
			if ((stb.st_mode & 07777) == fs -> filemode) {
				free (name);
				return OK;
			}
			note ("%s remote node %o != local mode %o",
			      target, stb.st_mode & 07777, fs -> filemode);
			free (name);
			return OK;
		}
		errno = ENOTDIR;
	}
	else if (errno == ENOENT && (mkdir(target, (int)fs -> filemode) == 0 ||
			     chkparent (target) == 0 &&
			     mkdir (target, (int)fs -> filemode) == 0)) {
		char *owner = qb2str (fs -> fileowner);
		char *group = qb2str (fs -> filegroup);

		if (chog (target, owner, group, fs -> filemode) == 0) {
			free (owner);
			free (group);
			free (name);
			return OK;
		}
		free (owner);
		free (group);
	}
	free (name);
	nadvise (target, "Can't install directory");
	tp = stp[--catname];
	*tp = '\0';
	return NOTOK;
}

/*
 * Remove a file or directory (recursively) and send back an acknowledge
 * or an error message.
 */
i_remove(str)
char	*str;
{
	DIR *d;
	struct dirent *dp;
	struct stat stb;
	char	buf[BUFSIZ];
	int	result = OK;

	if (lstat (str, &stb) < 0) {
		nadvise (str, "Can't stat file");
		return NOTOK;
	}
	
	switch (stb.st_mode & S_IFMT) {
	case S_IFREG:
	case S_IFLNK:
		if (unlink(str) < 0) {
			nadvise (str, "Can't unlink");
			return NOTOK;
		}
		note ("removed: %s", str);
		return OK;

	case S_IFDIR:
		break;

	default:
		nadvise (NULLCP, "%s: not a plain file", target);
		return NOTOK;
	}

	if ((d = opendir(str)) == NULL) {
		nadvise (str, "Can't open directory");
		return NOTOK;
	}

	while (dp = readdir(d)) {
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		(void) sprintf (buf, "%s/%s", str, dp -> d_name);
		result = i_remove(buf) == OK ? result : NOTOK;
	}
	(void) closedir(d);
	if (rmdir(str) < 0) {
		nadvise (str, "Can't remove directory", str);
		return NOTOK;
	}
	note ("removed: %s", str);
	return result;
}


addtoia5 (str, len)
char	*str;
int	len;
{
	struct type_Idist_IA5List **ia5p;

	for (ia5p = &ia5list; *ia5p; ia5p = &(*ia5p) -> next)
		continue;

	*ia5p = str2ia5list (str, len);
}

struct type_Idist_QueryResult *query (str)
char	*str;
{
	struct type_Idist_QueryResult *qr;
	struct stat stb;

	qr = (struct type_Idist_QueryResult *) malloc (sizeof *qr);
	if (qr == NULL)
		adios ("memory", "out of");
	
	if (catname)
		(void) sprintf (tp, "/%s", str);

	if (lstat (target, &stb) < 0) {
		if (errno == ENOENT) {
			qr -> offset = type_Idist_QueryResult_doesntExist;
		}
		else {
			nadvise ("failed", "lstat");
			free ((	char *) qr);
			qr = NULL;
		}
		*tp = '\0';
		return qr;
	}
	qr -> offset = type_Idist_QueryResult_doesExist;
	switch (stb.st_mode & S_IFMT) {
	    case S_IFREG:
	    case S_IFDIR:
	    case S_IFLNK:
		qr -> un.doesExist = makefs (stb.st_mode & S_IFMT, 0,
					     stb.st_mode & 07777, stb.st_size,
					     stb.st_mtime, "", "", str, "");
		break;

	    default:
		nadvise (NULLCP, "%s: not a file or directory", str);
		free ((char *)qr);
		qr = NULL;
	}
	*tp = '\0';
	return qr;
}

static struct type_Idist_IA5List *str2ia5list (s, len)
char   *s;
int	len;
{
	register struct type_Idist_IA5List *ia5;

	if ((ia5 = (struct type_Idist_IA5List  *) calloc (1, sizeof *ia5))
	    == NULL)
		return NULL;

	if ((ia5 -> IA5String = str2qb (s, len, 1)) == NULL) {
		free ((char *) ia5);
		return NULL;
	}

	return ia5;
}

struct type_Idist_FileList *do_listcdir ()
{
	DIR	*d;
	register struct dirent *dp;
	struct type_Idist_FileList *base, **flp;
	char	buf[BUFSIZ];
	struct stat stb;

	base = NULL;
	flp = &base;
	
	if ((d = opendir (target)) == NULL) {
		nadvise (target, "Can't open directory");
		return NULL;
	}

	while (dp = readdir (d)) {
		if (strcmp (dp -> d_name, ".") == 0 ||
		    strcmp (dp -> d_name, "..") == 0)
			continue;
		(void) sprintf (buf, "%s/%s", target, dp -> d_name);
		if (lstat (buf, &stb) < 0) {
			nadvise (buf, "Can't stat");
			continue;
		}
		switch (stb.st_mode & S_IFMT) {
		    case S_IFDIR:
		    case S_IFLNK:
		    case S_IFREG:
			break;
		    default:	/* skip sockets, fifos et al... */
			continue;
		}
		if ((*flp = (struct type_Idist_FileList *)
		     malloc (sizeof **flp)) == NULL)
			adios ("memory", "out of");

		(*flp) -> FileSpec = makefs (stb.st_mode & S_IFMT, 0,
					     stb.st_mode & 07777, stb.st_size,
					     stb.st_mtime, "", "",
					     dp->d_name, "");
		(*flp) -> next = NULL;
		flp = &(*flp) -> next;
	}
	(void) closedir (d);
	return base;
}

fixup ()
{
	struct timeval tvp[2];
	char	*new, *p;
	char	*owner, *group;
	long	convtime ();

	p = qb2str (cfiletype -> filename);
	new = cannon (p);
	free (p);
	
	if (bit_on (cfiletype -> fileopts, bit_Idist_Options_compare)) {
		if (compare (target, new) == OK) {
			(void) unlink (new);
			return OK;
		}
		if (bit_on (cfiletype -> fileopts, bit_Idist_Options_verify)) {
			(void) unlink (new);
			note ("need to update: %s", target);
			return OK;
		}
	}

	tvp[1].tv_sec =
		tvp[0].tv_sec =
			convtime (cfiletype -> filemtime);
	tvp[0].tv_usec = tvp[1].tv_usec = 0;
	if (utimes (new, tvp) < 0)
		nadvise (new, "utimes failed on");

	owner = qb2str (cfiletype -> fileowner);
	group = qb2str (cfiletype -> filegroup);

	if (chog (new, owner, group, cfiletype -> filemode) < 0) {
		free (owner);
		free (group);
		(void) unlink (new);
		return NOTOK;
	}
	free (owner);
	free (group);
	
	if (rename (new, target) < 0) {
		nadvise (target, "Can't rename %s to", new);
		return NOTOK;
	}
	if (bit_on (cfiletype -> fileopts, bit_Idist_Options_compare))
		note ("updated %s", target);
	free_Idist_FileSpec (cfiletype);
	cfiletype = NULL;
	return OK;
}

static int	compare (f1, f2)
char	*f1, *f2;
{
	FILE	*fp1, *fp2;
	char	buf1[BUFSIZ], buf2[BUFSIZ];	/* these two had
						   better be identical */
	int	n1, n2;

	if ((fp1 = fopen (f1, "r")) == NULL) {
		nadvise (f1, "Can't reopen file");
		return NOTOK;
	}
	if ((fp2 = fopen (f2, "r")) == NULL) {
		nadvise (f2, "Can't reopend file");
		(void) fclose (fp1);
		return NOTOK;
	}
	for (;;) {
		n1 = fread (buf1, sizeof buf1[0], sizeof buf1, fp1);
		n2 = fread (buf2, sizeof buf2[0], sizeof buf2, fp2);
		if (n1 != n2 || n1 == 0)
			break;
		if (bcmp (buf1, buf2, n1) != 0)
			break;
	}

	(void) fclose (fp1);
	(void) fclose (fp2);
	return n1 == 0 ? OK : NOTOK;
}


/*
 * Change owner, group and mode of file.
 */
chog(file, owner, group, imode)
	char *file, *owner, *group;
	integer imode;
{
	register int i;
	int uid, gid;
	extern char user[];
	extern int userid;
	int	mode;

	mode = imode;	/* ? lint ? */

	uid = userid;
	if (userid == 0) {
		if (*owner == ':') {
			uid = atoi(owner + 1);
		} else if (pw == NULL || strcmp(owner, pw->pw_name) != 0) {
			if ((pw = getpwnam(owner)) == NULL) {
				if (mode & 04000) {
					note("%s: unknown login name, clearing setuid",
						host, owner);
					mode &= ~04000;
					uid = 0;
				}
			} else
				uid = pw->pw_uid;
		} else
			uid = pw->pw_uid;
		if (*group == ':') {
			gid = atoi(group + 1);
			goto ok;
		}
	} else if ((mode & 04000) && strcmp(user, owner) != 0)
		mode &= ~04000;
	gid = -1;
	if (gr == NULL || strcmp(group, gr->gr_name) != 0) {
		if ((*group == ':' && (getgrgid(gid = atoi(group + 1)) == NULL))
		   || ((gr = getgrnam(group)) == NULL)) {
			if (mode & 02000) {
				note("%s: unknown group", group);
				mode &= ~02000;
			}
		} else
			gid = gr->gr_gid;
	} else
		gid = gr->gr_gid;
	if (userid && gid >= 0) {
		if (gr) for (i = 0; gr->gr_mem[i] != NULL; i++)
			if (!(strcmp(user, gr->gr_mem[i])))
				goto ok;
		mode &= ~02000;
		gid = -1;
	}
ok:
	if (userid == 0 && chown(file, uid, gid) < 0 )
		nadvise (file, "chown failed on");
	if ((mode & 06000) && chmod(file, mode) < 0) {
		nadvise (file, "chmod to 0%o failed on", mode);
	}
	return(0);
}

SFD cleanup ()
{
	char	*p, *temp;

	if (cfiletype) {
		p = qb2str (cfiletype -> filename);
		temp = cannon (p);
		(void) unlink (temp);
	}
}
