/************************************************************************
*                       STREAMS Configuration                           *
*************************************************************************
*									*
* This program processes an ASCII configuration file and builds the	*
* files needed for LiS to to compile with this configuration.		*
*									*
* The documentation on this utility can be found in the LiS html docs.  *
*									*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Jan 1999:	Ole Husgaard <sparre@login.dknet.dk>			*
*		Added configuration support for loadable object files	*
*		and autopush.						*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS strconf.c 2.15 09/13/04"

#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>
#ifdef QNX
#include <mem.h>
#else
#include <memory.h>
#include <string.h>
#endif
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define MAXAPUSH 8 /* From <sys/sad.h> */


/************************************************************************
*                          Data Structures                              *
************************************************************************/

typedef struct oi
{
    struct oi	  *link ;
    char	   name[50] ;
    int		   loadable ;	/* "true" if loadable object code file	*/
    int		   printed ;	/* "true" if printed to drvrconf.mk	*/

} object_info_t ;

typedef struct mi
{
    struct mi	    *link ;
    char	     name[50] ;
    char	     prefix[50] ;
    int              id ;
    int              units ;
    object_info_t   *obj ;
    int		     qlock_option ;

} module_info_t ;

typedef struct ai
{
    struct ai	    *link ;
    char	     name[50] ;
    int		     cmd ;
    int		     minor ;
    int		     lastminor ;
    int		     npush ;
    module_info_t   *mods[MAXAPUSH] ;

} autopush_info_t ;

typedef struct di
{
    struct di	    *link ;
    char	     name[50] ;
    char	     prefix[50] ;
    int              id ;
    int              nmajors ;
    int		     *major ;
    int		     nminors ;
    int		     init ;		/* "true" if has init routine */
    int		     term ;		/* has a terminate routine */
    int		     intr ;		/* has an interrupt routine */
    object_info_t   *obj ;
    autopush_info_t *ap ;
    int		     ndev ;		/* # of devices declared */
    int		     inst_nr ;		/* instance nr for this prefix */
    int		     qlock_option ;	/* qlock option -1 or 0 - 3 */

} driver_info_t ;

typedef struct ni
{
    struct ni	    *link ;
    char	     name[100] ;
    int		     type ;		/* I_IFCHR, etc */
    int		     major;
    int		     minor ;

} node_info_t ;

typedef struct vi
{
    struct vi		*link ;
    char		 name[50] ;
    driver_info_t	*drvr ;
    int			 unit ;
    long		 port ;
    int			 nports ;
    int			 irq_share ;
    int			 irq ;
    long		 mem ;
    long		 mem_size ;
    int			 dma1 ;
    int			 dma2 ;

} device_info_t ;

#define	IRQ_SHARE_DC	0		/* x */
#define	IRQ_NO_SHARE	1		/* n */
#define	IRQ_SHARE	2		/* s */
#define	IRQ_SHARE_DEV	3		/* S */

typedef struct lk
{
    struct lk	*link ;
    char	 name[100] ;

} lookup_struct_t ;


/************************************************************************
*                         Storage Declarations                          *
************************************************************************/

int             majors[256] ;		/* table of used majors */
int		nmajors = 0 ;		/* length of table */
int		next_major = 50 ;	/* larger than dedicated majors */

driver_info_t	*driver_head = NULL ;
module_info_t	*module_head = NULL ;
node_info_t	*node_head   = NULL ;
device_info_t	*device_head = NULL ;
object_info_t	*object_head = NULL ;
int		 ndevices ;

char		*outfile_name = "modconf.c" ;
char		*mkfile_name  = "makenodes.c" ;
char		*mknod_name   = "mknod" ;
char		*func_name    = "main" ;
char		*infile_name ;
char		*config_name  = "config.h" ;
char		*drvr_name    = "drvrconf.mk" ;
char		*confmod_name = "conf.modules" ;

FILE		*infile ;
FILE		*outfile ;
FILE		*mkfile ;
FILE		*configfile ;

char		 inbuf[600] ;
int		 line_nr ;
char		*nextp ;			/* nxt chr in buffer */
char		 token[200] ;			/* current input token */
long		 value ;			/* converted integer */
int		 class ;			/* token classification */
#define	NUMBER	1				/* numeric (converted) */
#define	IDENT	2				/* identifier */
#define	EOL	3				/* end of line */

int		 dflt_perm = 0666 ;		/* default permissions */

/************************************************************************
*                            major_in_use                               *
*************************************************************************
*									*
* Returns non-zero iff the argument is in use.				*
*									*
************************************************************************/
int major_in_use(int major)
{
	int i;

	for (i = 0; i < nmajors; ++i)
		if (majors[i] == major)
			return 1;
	return 0;
}

/************************************************************************
*                            get_free_major                             *
*************************************************************************
*									*
* Return a device major number that is not in use.			*
*									*
************************************************************************/
int get_free_major(void)
{
	int major = next_major++;

	while (major_in_use(major))
		major = next_major++;

	return major;
}

/************************************************************************
*                            reserve_major                              *
*************************************************************************
*									*
* Reserve a major a device major number that is not in use.		*
*									*
************************************************************************/
void reserve_major(int major)
{
	if (major_in_use(major)) {
		fprintf(stderr, "Internal error: Major in use.\n");
		exit(1);
	}
	if (nmajors >= sizeof(majors)/sizeof(majors[0])) {
		fprintf(stderr, "Internal error: Majors table overflow.\n");
		exit(1);
	}
	majors[nmajors++] = major;
}



/************************************************************************
*                            link_on_end                                *
*************************************************************************
*									*
* link item onto the end of the list whose head is pointed to by 'head'	*
*									*
************************************************************************/
void	link_on_end(lookup_struct_t **head, lookup_struct_t *item)
{
    lookup_struct_t	*p ;

    item->link = NULL ;
    if (*head == NULL)
    {
	*head = item ;
	return ;
    }

    for (p = *head; p->link != NULL; p = p->link) ;

    p->link = item ;

} /* link_on_end */

/************************************************************************
*                          driver_instance                              *
*************************************************************************
*                                                                       *
* A driver can be declared multiple times with the same prefix in order *
* to allocate multiple majors to a single driver.  Each time this is   	*
* done we allocate a new instance number for the driver.               	*
*                                                                      	*
* This routine runs down the driver list and counts instances of the   	*
* drivers with the given prefix.  It returns the number found.  Instance*
* numbers begin with zero.                                             	*
*                                                                      	*
************************************************************************/
int    driver_instance(char *prefix)
{
    driver_info_t      *dp ;
    int                         inst_cnt = 0 ;

    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
       if (strcmp(dp->prefix, prefix) == 0)
           inst_cnt += dp->nmajors ;
    }

    return(inst_cnt) ;

} /* driver_instance */


/************************************************************************
*                           mk_driver                                   *
*************************************************************************
*									*
* Make an entry for a driver.						*
*									*
************************************************************************/
driver_info_t	*mk_driver(char *name, char *prefix,
			   int *maj, int nmaj, int nmin)
{
    driver_info_t	*p ;

    p = (driver_info_t *) malloc(sizeof(*p)) ;
    if (p == NULL)
    {
	fprintf(stderr, "Cannot allocate entry for \"%s\"\n", name) ;
	exit(1) ;
    }

    strncpy(p->name, name, sizeof(p->name)) ;
    strncpy(p->prefix, prefix, sizeof(p->prefix)) ;
    p->major = maj ;
    p->nmajors = nmaj ;
    p->nminors = nmin ;
    p->init = 0 ;		/* default is no init routine */
    p->obj = NULL ;		/* default object */
    p->inst_nr = driver_instance(prefix) ;
    p->qlock_option = -1 ;	/* none specified */

    link_on_end((lookup_struct_t **) &driver_head, (lookup_struct_t *) p) ;

    return(p) ;

} /* mk_driver */

/************************************************************************
*                           mk_module                                   *
*************************************************************************
*									*
* Make an entry for a module.						*
*									*
************************************************************************/
module_info_t	*mk_module(char *name, char *prefix, int units)
{
    module_info_t	*p ;

    p = (module_info_t *) malloc(sizeof(*p)) ;
    if (p == NULL)
    {
	fprintf(stderr, "Cannot allocate entry for \"%s\"\n", name) ;
	exit(1) ;
    }

    strncpy(p->name, name, sizeof(p->name)) ;
    strncpy(p->prefix, prefix, sizeof(p->prefix)) ;
    p->units = units;
    p->obj = NULL ;		/* default object */
    p->id = 0 ;			/* not yet known */
    p->qlock_option = -1 ;	/* none specified */

    link_on_end((lookup_struct_t **) &module_head, (lookup_struct_t *) p) ;

    return(p) ;

} /* mk_module */

/************************************************************************
*                           mk_object                                   *
*************************************************************************
*									*
* Make an entry for a object.						*
*									*
************************************************************************/
object_info_t	*mk_object(char *name)
{
    object_info_t	*p ;

    p = (object_info_t *) malloc(sizeof(*p)) ;
    if (p == NULL)
    {
	fprintf(stderr, "Cannot allocate entry for \"%s\"\n", name) ;
	exit(1) ;
    }

    strncpy(p->name, name, sizeof(p->name)) ;
    p->loadable = 0 ;		/* default is not loadable */
    p->printed = 0 ;		/* not printed yet */

    link_on_end((lookup_struct_t **) &object_head, (lookup_struct_t *) p) ;

    return(p) ;

} /* mk_object */

/************************************************************************
*                             mk_node                                   *
*************************************************************************
*									*
* Make an entry for a node.						*
*									*
************************************************************************/
node_info_t	*mk_node(char *name, int type, int maj, int min)
{
    node_info_t	*p ;

    p = (node_info_t *) malloc(sizeof(*p)) ;
    if (p == NULL)
    {
	fprintf(stderr, "Cannot allocate entry for \"%s\"\n", name) ;
	exit(1) ;
    }

    strncpy(p->name, name, sizeof(p->name)) ;
    p->type  = type ;
    p->major = maj ;
    p->minor = min ;

    link_on_end((lookup_struct_t **) &node_head, (lookup_struct_t *) p) ;

    return(p) ;

} /* mk_node */

/************************************************************************
*                             mk_device                                 *
*************************************************************************
*									*
* Make an entry for a device.						*
*									*
************************************************************************/
device_info_t	*mk_device(device_info_t *devp)
{
    device_info_t	*p ;

    p = (device_info_t *) malloc(sizeof(*p)) ;
    if (p == NULL)
    {
	fprintf(stderr, "Cannot allocate device entry for \"%s\"\n", devp->name) ;
	exit(1) ;
    }

    *p = *devp ;

    link_on_end((lookup_struct_t **) &device_head, (lookup_struct_t *) p) ;
    ndevices++ ;

    return(p) ;

} /* mk_device */

/************************************************************************
*                            lookup_name                                *
*************************************************************************
*									*
* This routine takes advantage of the fact that all of the table	*
* entries have the same first two fields:  a link and a name.		*
*									*
* It searches any of the tables for the named item.			*
*									*
************************************************************************/
void	*lookup_name(void *list_head, char *name)
{
    lookup_struct_t	*p ;

    for (p = (lookup_struct_t *) list_head; p != NULL; p = p->link)
	if (strcmp(name, p->name) == 0) return((void *) p) ;

    return(NULL) ;

} /* lookup_name */

/************************************************************************
*                           find_driver                                 *
*************************************************************************
*									*
* Find the driver by name.						*
*									*
************************************************************************/
driver_info_t	*find_driver(char *name)
{
    return((driver_info_t *) lookup_name(driver_head, name)) ;

} /* find_driver */

/************************************************************************
*                        find_driver_by_major                           *
*************************************************************************
*									*
* Find the driver by major device number.				*
*									*
************************************************************************/
driver_info_t	*find_driver_by_major(int major)
{
    driver_info_t *dp ;
    int i;

    for (dp = driver_head; dp != NULL; dp = dp->link) {
	for (i = 0;  i < dp->nmajors;  i++)
	    if (dp->major[i] == major)  return dp;
    }

    return NULL; 
} /* find_driver_by_major */

/************************************************************************
*                           find_module                                 *
*************************************************************************
*									*
* Find the module by name.						*
*									*
************************************************************************/
module_info_t	*find_module(char *name)
{
    return((module_info_t *) lookup_name(module_head, name)) ;

} /* find_module */

/************************************************************************
*                           find_node                                 *
*************************************************************************
*									*
* Find the node by name.						*
*									*
************************************************************************/
node_info_t	*find_node(char *name)
{
    return((node_info_t *) lookup_name(node_head, name)) ;

} /* find_node */

/************************************************************************
*                           find_object                                 *
*************************************************************************
*									*
* Find the object code file by name.					*
*									*
************************************************************************/
object_info_t	*find_object(char *name)
{
    return((object_info_t *) lookup_name(object_head, name)) ;

} /* find_object */

/************************************************************************
*                             err                                       *
*************************************************************************
*									*
* Print an error message.						*
*									*
************************************************************************/
void	err(char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "\"%s\" line %d: ", infile_name, line_nr) ;
    va_start (args, fmt);
    vfprintf (stderr, fmt, args);
    va_end (args);

} /* err */

/************************************************************************
*                           read_line                                   *
*************************************************************************
*									*
* Read a line from infile.  Skip comment lines and blank lines.		*
* Return negative for EOF, return zero for success.			*
*									*
************************************************************************/
int	read_line(void)
{
    do
    {
	line_nr++ ;
	if (fgets(inbuf, sizeof(inbuf), infile) == NULL)
	{
	    strcpy(inbuf, "End-of-file") ;
	    return(-1) ;
	}

    } while (inbuf[0] == '#' || inbuf[0] == '\n') ;

    nextp = inbuf ;			/* set global ptr */
    return(0) ;

} /* read_line */

/************************************************************************
*                           get_next                                    *
*************************************************************************
*									*
* Get the next blank-delimited token and leave in the global token	*
* buffer.  Advance global input pointers.				*
*									*
* Return the class of the next thing, EOL if end of line.		*
*									*
************************************************************************/
int	get_next(void)
{
    char	*p, *endp ;
    int		 n ;
    long	 val;

    p = token ;
    n = 0 ;

    while ( *nextp && *nextp <= ' ' ) nextp++ ;	/* skip white space */

    while ( *nextp && *nextp > ' ' && n < sizeof(token)-1 )
    {						/* copy token */
	*p++ = *nextp++ ;
	n++ ;
    }

    while ( *nextp && *nextp > ' ' ) nextp++ ;	/* skip excess chars */

    *p = 0 ;					/* NUL terminate the token */

#if 0
    if (isdigit(token[0]))			/* numeric value */
    {
	class = NUMBER ;
	value = (int) strtol(token, NULL, 0) ;	/* input convert */
    }
#else
    endp = token;				/* non-NULL pointer */
    val = strtol(token, &endp, 0);		/* input convert */
    if (token[0] != '\0' && *endp == '\0'
        && val < LONG_MAX && val > LONG_MIN)	/* valid number */
    {
	class = NUMBER ;
	value = val ;
    }
#endif
    else
    if (token[0] == 0)				/* end of line */
    {
	strcpy(token, "End-of-line") ;
	class = EOL ;
    }
    else					/* treat as identifier */
	class = IDENT ;

    return(class) ;				/* return its class */

} /* get_next */

/************************************************************************
*                           process_line                                *
*************************************************************************
*									*
* Process one line of input.						*
*									*
************************************************************************/
void	process_line(void)
{
    if (get_next() == EOL)
    {
	err("Internal error:  empty line\n") ;
	return ;
    }

    if (strcmp(token, "driver") == 0)
    {
	char	 name[50] ;
	char	 prefix[50] ;
	int	 *major, major0 ;
	int      nmajors = 0 ;
	int	 nminors ;

	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(name, token, sizeof(name)-1) ;
	name[sizeof(name)-1] = 0 ;

	if (get_next() != IDENT)
	{
	    err("Driver prefix must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(prefix, token, sizeof(prefix)-1) ;
	name[sizeof(prefix)-1] = 0 ;

	switch (get_next())
	{
	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
	    	err("Invalid driver major %ld  specifier: %s\n", value, token) ;
	    	return ;
	    }
	    major0 = (int) value ;
	    break ;
	case IDENT:
	    if (strcmp(token, "*") == 0)
	    {
		major0 = -1 ;
		break ;
	    }
	    /*
	     * Fall into error case 
	     */
	default:
	    err("Invalid driver major specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
	        err("Invalid driver n-minors specifier: %ld\n", value) ;
	        return ;
	    }
	    nminors = (int) value ;
	    break ;
	case EOL:
	    nminors = 1 ;
	    break ;
	default:
	    err("Invalid driver n-minors specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
	        err("Invalid driver n-majors specifier: %ld\n", value) ;
	        return ;
	    }
	    nmajors = (int) value ;
	    break ;
	case EOL:
	    nmajors = 0 ;
	    break ;
	}

	/*
	 *  NB - the following only works with a major/minor scheme
	 *  for which the minor number occupies <x> low bits and the
	 *  major occupies the next <y> high bits (in terms of
	 *  significance.  This is known to be true on Linux; it should
	 *  be checked on other systems (also true on Solaris and
	 *  HP-UX, and maybe many others).
	 */
	if (nmajors == 0)
	    nmajors = major(nminors-1)+1;
	else
	    if (nmajors <= major(nminors-1)) {
		err("Too few majors (%d) specified for %d minors.\n",
		    nmajors, nminors );
	    }

	if (nmajors == 1) {
	    if (major0 < 0)  major0 = get_free_major();

	    if (major_in_use(major0))
	    {
		err("Major %d already in use.\n", major0 ) ;
		return ;
	    }
	    reserve_major(major0) ;

	    major = malloc( sizeof(int) );
	    *major = major0;
	} else {
	    int i;

	    if (major0 >= 0)
	    {
		err("Multiple majors - major spec must be '*'.\n" );
		return ;
	    }

	    major = malloc( nmajors * sizeof(int) );
	    for (i = 0;  i < nmajors;  i++) {
		major[i] = get_free_major();
		reserve_major(major[i]);
	    }
	}
	mk_driver(name, prefix, major, nmajors, nminors) ; /* make the entry */
    }
    else
    if (strcmp(token, "module") == 0)
    {
	char	 name[50] ;
	char	 prefix[50] ;
	int      units;

	if (get_next() != IDENT)
	{
	    err("Module name must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(name, token, sizeof(name)-1) ;
	name[sizeof(name)-1] = 0 ;

	if (get_next() != IDENT)
	{
	    err("Module prefix must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(prefix, token, sizeof(prefix)-1) ;
	name[sizeof(prefix)-1] = 0 ;

	switch (get_next())
	{
	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
		err("Invalid module units specifier: %ld\n", value) ;
		return ;
	    }
	    units = (int) value ;
	    break ;
	case EOL:
	    units = 1;
	    break ;
	default:
	    err("Invalid module units specifier: %s\n", token) ;
	    return ;
	}
    
	mk_module(name, prefix, units) ;
    }
    else
    if (strcmp(token, "node") == 0)
    {
	char	 	 name[100] ;
	int	 	 type ;			/* I_IFCHR, etc */
	int	 	 major;
	int	 	 minor ;
	driver_info_t	*dp, *major_dp = NULL ;

	if (get_next() != IDENT)
	{
	    err("Node name must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(name, token, sizeof(name)-1) ;
	name[sizeof(name)-1] = 0 ;

	switch (get_next())
	{
	case IDENT:
	    if (strcmp(token, "c") == 0)
	    {
		type = S_IFCHR ;
		break ;
	    }
	    else
	    if (strcmp(token, "b") == 0)
	    {
		type = S_IFBLK ;
		break ;
	    }
	    else
	    if (strcmp(token, "p") == 0)
	    {
		type = S_IFIFO ;
		break ;
	    }
	    /*
	     * Fall into error case 
	     */
	case NUMBER:
	case EOL:
	default:
	    err("Invalid device type specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case NUMBER:
	    if (value < 0 || value > 0777)
	    {
	        err("Invalid permissions specifier: %s\n", token) ;
	        return ;
	    }
	    type |= value ;			/* add permissions */
	    break ;
	case IDENT:
	    if (strcmp(token, "*") == 0)
	    {
		type |= dflt_perm ;
		break ;
	    }
	    /*
	     * Fall through to error
	     */
	case EOL:
	default:
	    err("Invalid permissions specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case IDENT:
	    major_dp = dp = find_driver(token) ;
	    if (dp == NULL)
	    {
		err("Invalid node driver name: %s\n", token) ;
		return ;
	    }

	    major = dp->major[0] ;
	    break ;

	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
	    	err("Invalid node major specifier: %s\n", token) ;
	    	return ;
	    }
	    major = (int) value ;
	    break ;
	case EOL:
	default:
	    err("Invalid node major specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case IDENT:
	    dp = find_driver(token) ;
	    if (dp == NULL)
	    {
		err("Invalid driver name: %s\n", token) ;
		return ;
	    }

	    minor = dp->major[0] ;
	    break ;

	case NUMBER:
	    if (value < 0 || value > INT_MAX)
	    {
	    	err("Invalid node minor specifier: %s\n", token) ;
	    	return ;
	    }
	    minor = (int) value ;
	    break ;
	case EOL:
	default:
	    err("Invalid node minor specifier: %s\n", token) ;
	    return ;
	}

	switch (get_next())
	{
	case NUMBER:
	    if (!major_dp)
	    {
		err("Use of major index requires driver name\n");
		return ;
	    }
	    else if (value < 0 || value >= major_dp->nmajors)
	    {
	    	err("Invalid node major index specifier: %s\n", token) ;
	    	return ;
	    }
	    major = major_dp->major[value] ;
	    break ;
	case EOL:
	default:
	    break ;
	}

	mk_node(name, type, major, minor) ;
    }
    else
    if (strcmp(token, "device") == 0)
    {
	device_info_t	 dev ;

	memset(&dev, 0, sizeof(dev)) ;
	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	strncpy(dev.name, token, sizeof(dev.name)-1) ;
	dev.name[sizeof(dev.name)-1] = 0 ;
	dev.drvr = find_driver(token) ;
	if (dev.drvr == NULL)
	{
	    err("Driver name \"%s\" not found\n", token) ;
	    return ;
	}

	if (get_next() == NUMBER)
	{
	    if (value < INT_MIN || value > INT_MAX)
	    {
	        err("Invalid unit number: %s\n", token) ;
	        return ;
	    }
	    dev.unit = (int) value ;
	}
	else
	if (class == EOL)
	    dev.unit = -1 ;
	else
	{
	    err("Unit number must be a number: %s\n", token) ;
	    return ;
	}

	if (get_next() == NUMBER)
	    dev.port = value ;
	else
	if (class == EOL)
	    dev.port = -1 ;
	else
	{
	    err("Port address must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	{
	    if (value < 0 || value > INT_MAX)
	    {
	        err("Invalid port number: %s\n", token) ;
	        return ;
	    }
	    dev.nports = (int) value ;
	}
	else
	if (class == EOL)
	    dev.nports = 0 ;
	else
	{
	    err("Number of port addresses must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == IDENT)
	    switch (token[0])
	    {
	    case 'n':
		dev.irq_share =	IRQ_NO_SHARE ;
		break ;
	    case 's':
		dev.irq_share =	IRQ_SHARE ;
		break ;
	    case 'S':
		dev.irq_share =	IRQ_SHARE_DEV ;
		break ;
	    case 'x':
		dev.irq_share =	IRQ_SHARE_DC ;
		break ;
	    default:
		err("Invalid IRQ sharing code: \"%s\"\n", token) ;
		return ;
	    }
	else
	if (class == EOL)
	    dev.irq_share = 0 ;
	else
	{
	    err("IRQ sharing code must be identifier: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	{
	    if (value != -1 && (value < 0 || value > INT_MAX))
	    {
	        err("Invalid IRQ number: %s\n", token) ;
	        return ;
	    }
	    dev.irq = (int) value ;
	}
	else
	if (class == EOL)
	    dev.irq = -1 ;
	else
	{
	    err("IRQ must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	    dev.mem = value ;
	else
	if (class == EOL)
	    dev.mem = 0 ;
	else
	{
	    err("Memory address must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	    dev.mem_size = value ;
	else
	if (class == EOL)
	    dev.mem_size = 0 ;
	else
	{
	    err("Memory size must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	{
	    if (value != -1 && (value < 0 || value > INT_MAX))
	    {
	        err("Invalid DMA 1 channel number: %s\n", token) ;
	        return ;
	    }
	    dev.dma1 = (int) value ;
	}
	else
	if (class == EOL)
	    dev.dma1 = -1 ;
	else
	{
	    err("DMA 1 channel must be a number: %s\n", token) ;
	    return ;
	}


	if (get_next() == NUMBER)
	{
	    if (value != -1 && (value < 0 || value > INT_MAX))
	    {
	        err("Invalid DMA 2 channel number: %s\n", token) ;
	        return ;
	    }
	    dev.dma2 = (int) value ;
	}
	else
	if (class == EOL)
	    dev.dma2 = -1 ;
	else
	{
	    err("DMA 2 channel must be a number: %s\n", token) ;
	    return ;
	}

	mk_device(&dev) ;
	dev.drvr->ndev++ ;		/* count devices for driver */
    }
    else
    if (strcmp(token, "qlock") == 0)
    {
	if (get_next() != IDENT)
	{
	    err("Qlock type must be \"driver\" or \"module\": %s\n", token) ;
	    return ;
	}

	if (strcmp(token, "driver") == 0)
	{
	    driver_info_t *dp ;

	    if (get_next() != IDENT)
	    {
	        err("Driver name must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((dp = find_driver(token)) == NULL)
	    {
	        err("Driver name \"%s\" not found\n", token) ;
	        return ;
	    }

	    if (get_next() != NUMBER)
	    {
	        err("Qlock option must be a number: %s\n", token) ;
	        return ;
	    }

	    if (value < 0 || value > 3)
	    {
	        err("Qlock option must be in range 0 - 3: %s\n", token) ;
	        return ;
	    }

	    dp->qlock_option = value ;
	}
	else
	if (strcmp(token, "module") == 0)
	{
	    module_info_t *mp ;

	    if (get_next() != IDENT)
	    {
	        err("Module name must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((mp = find_module(token)) == NULL)
	    {
	        err("Module name \"%s\" not found\n", token) ;
	        return ;
	    }

	    if (get_next() != NUMBER)
	    {
	        err("Qlock option must be a number: %s\n", token) ;
	        return ;
	    }

	    if (value < 0 || value > 3)
	    {
	        err("Qlock option must be in range 0 - 3: %s\n", token) ;
	        return ;
	    }

	    mp->qlock_option = value ;
	}
	else
	{
	    err("Qlock type must be \"driver\" or \"module\": %s\n", token) ;
	    return ;
	}
    }
    else
    if (strcmp(token, "initialize") == 0)
    {
	driver_info_t	 *dp ;

	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	dp = find_driver(token) ;
	if (dp == NULL)
	{
	    err("Driver name \"%s\" not found\n", token) ;
	    return ;
	}

	dp->init = 1 ;			/* driver has init flag */
    }
    else
    if (strcmp(token, "terminate") == 0)
    {
	driver_info_t	 *dp ;

	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	dp = find_driver(token) ;
	if (dp == NULL)
	{
	    err("Driver name \"%s\" not found\n", token) ;
	    return ;
	}

	dp->term = 1 ;			/* driver has term flag */
    }
    else
    if (strcmp(token, "interrupt") == 0)
    {
	driver_info_t	 *dp ;

	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	dp = find_driver(token) ;
	if (dp == NULL)
	{
	    err("Driver name \"%s\" not found\n", token) ;
	    return ;
	}

	dp->intr = 1 ;			/* driver has intr flag */
    }
    else
    if (strcmp(token, "define") == 0)
    {
	fprintf(configfile, "#define %s\n", nextp) ;
    }
    else
    if (strcmp(token, "loadable") == 0)
    {
	object_info_t *op ;

	if (get_next() != IDENT)
	{
	    err("Loadable object name must be identifier: %s\n", token) ;
	    return ;
	}

	if ((op = find_object(token)) == NULL)
	{
	    err("Object name \"%s\" not found\n", token) ;
	    return ;
	}

	op->loadable = 1 ;		/* object code is loadable */
    }
    else
    if (strcmp(token, "objname") == 0)
    {
	object_info_t *op ;

	if (get_next() != IDENT)
	{
	    err("Objname type must be \"driver\" or \"module\": %s\n", token) ;
	    return ;
	}

	if (strcmp(token, "driver") == 0)
	{
	    driver_info_t *dp ;

	    if (get_next() != IDENT)
	    {
	        err("Driver name must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((dp = find_driver(token)) == NULL)
	    {
	        err("Driver name \"%s\" not found\n", token) ;
	        return ;
	    }

	    if (dp->obj != NULL)
	    {
	        err("Object name already declared for driver \"%s\"\n",
		    token) ;
	        return ;
	    }

	    if (get_next() != IDENT)
	    {
	        err("Driver objname must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((op = find_object(token)) != NULL)
		dp->obj = op ;
	    else
	    {
		char	 name[50] ;

	        strncpy(name, token, sizeof(name)-1) ;
	        name[sizeof(name)-1] = 0 ;

	        dp->obj = mk_object(name) ;
	    }
	}
	else
	if (strcmp(token, "module") == 0)
	{
	    module_info_t *mp ;

	    if (get_next() != IDENT)
	    {
	        err("Module name must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((mp = find_module(token)) == NULL)
	    {
	        err("Module name \"%s\" not found\n", token) ;
	        return ;
	    }

	    if (mp->obj != NULL)
	    {
	        err("Object name already declared for driver \"%s\"\n", token) ;
	        return ;
	    }

	    if (get_next() != IDENT)
	    {
	        err("Module objname must be identifier: %s\n", token) ;
	        return ;
	    }

	    if ((op = find_object(token)) != NULL)
		mp->obj = op ;
	    else
	    {
		char name[50] ;

	        strncpy(name, token, sizeof(name)-1) ;
	        name[sizeof(name)-1] = 0 ;

	        mp->obj = mk_object(name) ;
	    }
	}
	else
	{
	    err("Objname type must be \"driver\" or \"module\": %s\n", token) ;
	    return ;
	}
    }
    else
    if (strcmp(token, "autopush") == 0)
    {
        autopush_info_t	*ap ;
	driver_info_t *dp ;

	if (get_next() != IDENT)
	{
	    err("Driver name must be identifier: %s\n", token) ;
	    return ;
	}

	if ((dp = find_driver(token)) == NULL)
	{
	    err("Driver name \"%s\" not found\n", token) ;
	    return ;
	}

        ap = (autopush_info_t *) malloc(sizeof(*ap)) ;
        if (ap == NULL)
        {
	    fprintf(stderr,
		   "Cannot allocate autopush entry for device \"%s\"\n",
		    dp->name) ;
	    return ;
        }

	if (get_next() != NUMBER || value < -1 || value > INT_MAX)
	{
	    fprintf(stderr,
		    "Invalid autopush entry (minor) for device \"%s\"\n",
		    dp->name) ;
	    free(ap) ;
	    return ;
	}
	ap->minor = value ;

	if (get_next() != NUMBER || value < 0 || value > INT_MAX)
	{
	    fprintf(stderr,
		    "Invalid autopush entry (lastminor) for device \"%s\"\n",
		    dp->name) ;
	    free(ap) ;
	    return ;
	}
	ap->lastminor = (int) value ;

	if (ap->minor == -1)
	    ap->lastminor = 0; /* ignore lastminor */
	else if (ap->lastminor != 0 && ap->lastminor <= ap->minor)
	{
	    fprintf(stderr,
		    "Invalid autopush entry (lastminor <= minor) "
		    "for device \"%s\"\n",
		    dp->name) ;
	    free(ap) ;
	    return ;
	}

	for (ap->npush = 0; ap->npush < MAXAPUSH; ++ap->npush)
	{
	    int sym = get_next() ;

	    if (sym == EOL)
		break ;

	    if (sym != IDENT)
	    {
	        fprintf(stderr, "Invalid autopush entry for device \"%s\": %s\n",
		        dp->name, token) ;
	        free(ap) ;
	        return ;
	    }

	    if ((ap->mods[ap->npush] = find_module(token)) == NULL)
	    {
	        err("Module name \"%s\" not found\n", token) ;
	        free(ap) ;
	        return ;
	    }
	}

	/*
	 *  Check for minor number collisions
	 */
#define INRANGE(x, lo, hi) ( (x) >= (lo) && (x) <= (hi) )
	if (ap->minor == -1)			/* SAP_ALL */
        {
	    if (dp->ap != NULL)
	    {
	        fprintf(stderr,
			"Conflicting autopush entries for device \"%s\"\n",
		        dp->name) ;
	        free(ap) ;
	        return ;
	    }
	}
	else if (ap->lastminor == 0)		/* SAP_ONE */
        {
	    autopush_info_t *ar;

	    for (ar = dp->ap; ar != NULL; ar = ar->link)
	    {
	        if (ar->minor == -1
	            || (ar->lastminor != 0
			&& INRANGE(ar->minor, ap->minor, ap->lastminor))
		    || (ar->lastminor == 0 && ar->minor == ap->minor))
	        {
	            fprintf(stderr,
			    "Conflicting autopush entries for device \"%s\"\n",
		            dp->name) ;
	            free(ap) ;
	            return ;
	        }
	    }
	}
	else if (ap->lastminor > ap->minor)	/* SAP_RANGE */
        {
	    autopush_info_t *ar;

	    for (ar = dp->ap; ar != NULL; ar = ar->link)
	    {
	        if (ar->minor == -1
	          || (ar->lastminor != 0
		      && (   INRANGE(ar->minor,     ap->minor, ap->lastminor)
			  || INRANGE(ar->lastminor, ap->minor, ap->lastminor)
			  || (ar->minor < ap->minor
			      && ar->lastminor > ap->lastminor)))
		  || (ar->lastminor == 0
		      && INRANGE(ar->minor, ap->minor, ap->lastminor)))
	        {
	            fprintf(stderr,
			    "Conflicting autopush entries for device \"%s\"\n",
		            dp->name) ;
	            free(ap) ;
	            return ;
	        }
	    }
	}
	else
	{
	    fprintf(stderr, "---Invalid autopush entry for device \"%s\"\n",
		    dp->name) ;
	    free(ap) ;
	    return ;
	}
#undef INRANGE

	ap->link = dp->ap;
	dp->ap = ap;
    }
    else
	err("Invalid keyword \"%s\"\n", token) ;

} /* process_line */

/************************************************************************
*                           process_file                                *
*************************************************************************
*									*
* Process the input file.						*
*									*
************************************************************************/
void	process_file(void)
{
    int			 next_id, done ;
    driver_info_t	*dp ;
    module_info_t	*mp ;
    object_info_t       *op ;


    /*
     *  Process input
     */
    while (read_line() == 0)
    {
	process_line() ;
    }

    /*
     *   Do missing/default object code files
     */
    for (mp = module_head; mp != NULL; mp = mp->link)
    {
	if (mp->obj == NULL)
	{
	    if ((op = find_object(mp->name)) == NULL)
		op = mk_object(mp->name) ;
	    mp->obj = op ;
	}
    }
    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	if (dp->obj == NULL)
	{
	    if ((op = find_object(dp->name)) == NULL)
		op = mk_object(dp->name) ;
	    dp->obj = op ;
	}
    }

    /*
     *  Sort drivers by major
     */
    do {
    	driver_info_t **dpp;

    	done = 1;
    	for (dpp = &driver_head; *dpp != NULL; dpp = &(*dpp)->link) {
    	    driver_info_t *dp1, *dp2;

    	    dp1 = *dpp;
    	    dp2 = dp1->link;

	    if (dp2 != NULL && dp1->major[0] > dp2->major[0]) {
		done = 0;
		dp1->link = dp2->link;
		dp2->link = dp1;
		*dpp = dp2;
            }
    	}
    } while (!done);

    /*
     *  Sort modules by name
     */
    do {
	module_info_t **mpp;

	done = 1;
	for (mpp = &module_head; *mpp != NULL; mpp = &(*mpp)->link) {
	    module_info_t *mp1, *mp2;

	    mp1 = *mpp;
	    mp2 = mp1->link;

	    if (mp2 != NULL && strcmp(mp1->name, mp2->name) > 0) {
		done = 0;
		mp1->link = mp2->link;
		mp2->link = mp1;
		*mpp = mp2;
	    }
	}
    } while (!done);


    /*
     *   Allocate IDs
     */
    next_id = 1 ;
    /* first loadable modules */
    for (mp = module_head; mp != NULL; mp = mp->link)
	if (mp->obj->loadable)
	    mp->id = next_id++ ;
    /* then linked modules (all other) */
    for (mp = module_head; mp != NULL; mp = mp->link)
	if (mp->id == 0)
	    mp->id = next_id++ ;
    /* then drivers */
    for (dp = driver_head; dp != NULL; dp = dp->link)
	    dp->id = next_id++ ;

} /* process_file */

/************************************************************************
*                           build_modconf                               *
*************************************************************************
*									*
* Build the modconf.c file.						*
*									*
************************************************************************/
void	build_modconf(void)
{
    driver_info_t	*dp ;
    module_info_t	*mp ;
    device_info_t	*devptr;

#define	p0(f)		fprintf(outfile, f)
#define	p1(f,y)		fprintf(outfile, f,y)
#define	p2(f,y,z)	fprintf(outfile, f,y,z)

    p0("/* WARNING:  THIS FILE WAS GENERATED. "
       " MANUAL CHANGES MAY BE LOST. */\n");
    p0("\n") ;
    p0("\n") ;

    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	if (dp->obj->loadable)
	    continue ;

	p1("extern struct streamtab %sinfo ;\n", dp->prefix) ;
	if (dp->init)
	    p1("extern void _RP"
	       " %sinit(void) ;\n", dp->prefix) ;
	if (dp->term)
	    p1("extern void _RP"
	       " %sterm(void) ;\n", dp->prefix) ;
	if (dp->intr)
	{
	    p1("extern void _RP"
	       " %sintr(void) ;\n", dp->prefix) ;
	    p1("\nint _RP"
	       "__%sintr(int vect,void *dev,struct pt_regs *reg)\n",
	       dp->prefix);
	    p0("{\n");
	    p0("#if defined(KERNEL_2_5)\n");
	    p1("    return(%sintr\(vect,dev,reg)); \n", dp->prefix);
	    p0("#else\n");
	    p1("    %sintr\(vect,dev,reg); \n", dp->prefix);
	    p0("    return(lis_irqreturn_handled);\n");
	    p0("#endif\n");
	    p0("}\n");
	}

    }
    p0("\n") ;

    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	int j;
#if 0
	if (dp->obj->loadable)
	    continue ;
#endif
	p2("int %s_%u_majors[] = {", dp->prefix, dp->inst_nr) ;
	for (j = 0;  j < dp->nmajors;  j++) {
	    p1("%d", dp->major[j]);
	    if (j < dp->nmajors-1)
		p0(", ");
	    else
		p0("};\n");
	}
    }
    p0("\n") ;

    p0("driver_config_t		lis_driver_config[] =\n") ;
    p0("{\n") ;
    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	if (dp->obj->loadable)
	    continue ;

	p1("	{\"%s\",", dp->name) ;
	p1(" &%sinfo,", dp->prefix) ;
	p2(" %s_%u_majors,", dp->prefix, dp->inst_nr) ;
	p1(" %d,", dp->nmajors) ;
	p1(" %d,", dp->nminors) ;
	if (dp->init)
	    p1(" %sinit,", dp->prefix) ;
	else
	    p0(" NULL,") ;
	if (dp->term)
	    p1(" %sterm,", dp->prefix) ;
	else
	    p0(" NULL,") ;
	p1(" %d},\n", dp->qlock_option) ;
    }
    p0("} ;\n") ;
    p0("\n") ;

    p0("device_config_t		lis_device_config[] =\n");
    p0("{\n");
    for (devptr = device_head;devptr !=NULL;devptr = devptr->link)
    {
    	if (devptr->drvr->obj->loadable || !devptr->drvr->intr)
	   continue;

	p1("    {\"%s\",",devptr->drvr->name);
	p1(" \"%s\",",devptr->drvr->prefix);
	p1(" &%sinfo,",devptr->drvr->prefix);
	p1(" __%sintr,",devptr->drvr->prefix);
	p1(" %d,",devptr->unit);
	p1(" 0x%lx,",devptr->port);
	p1(" %d,",devptr->nports);
	p1(" %d,",devptr->irq_share);
	p1(" %d,",devptr->irq);
	p1(" 0x%lx,",devptr->mem);
	p1(" 0x%lx,",devptr->mem_size);
	p1(" %d,",devptr->dma1);
	p1(" %d,",devptr->dma2);
	p1(" %d,",devptr->drvr->major[0]);
	p1(" %d",devptr->drvr->nminors);
	p0(" },\n");
    }

    p0("} ;\n") ;
    p0("\n") ;

    for (mp = module_head; mp != NULL; mp = mp->link)
    {
	if (mp->obj->loadable)
	    continue ;

	p1("extern struct streamtab		%sinfo ;\n", mp->prefix) ;
    }
    p0("\n") ;

    p0("module_config_t		lis_module_config[] =\n") ;
    p0("{\n") ;
    p0("	{\"\",NULL,\"\"}, /* empty 0th entry */") ;
    for (mp = module_head; mp != NULL; mp = mp->link)
    {
	p1("	{\"%s\",", mp->name) ;
	if (mp->obj->loadable)
	{				/* no streamtab, incl obj name */
	    p1(" NULL, \"%s\", ", mp->obj->name) ;
	    p1("%d},\n", mp->qlock_option) ;
	}
	else
	{				/* streamtab, no obj name */
	    p1(" &%sinfo,", mp->prefix) ;
	    p1(" \"\", %d},\n", mp->qlock_option) ;
	}
    }
    p0("} ;\n") ;
    p0("\n") ;

    p0("\n") ;
    p0("#ifdef LIS_LOADABLE_SUPPORT\n") ;
    p0("\n") ;

    p0("driver_obj_name_t		lis_drv_objnames[] =\n") ;
    p0("{\n") ;
    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	if (!dp->obj->loadable)
	    continue ;

	p2("    {%s_%u_majors,", dp->prefix, dp->inst_nr) ;
	p1(" %d,", dp->nmajors) ;
	if (dp->init)
	    p1(" \"%sinit\",", dp->prefix) ;
	else
	    p0(" NULL,") ;
	p1(" \"%s\"},\n", dp->obj->name) ;
    }
    p0("} ;\n") ;
    p0("\n") ;

    p0("\n") ;
    p0("#endif /* LIS_LOADABLE_SUPPORT */\n") ;
    p0("\n") ;

    p0("autopush_init_t		lis_apush_init[] =\n") ;
    p0("{\n") ;
    for (dp = driver_head; dp != NULL; dp = dp->link)
    {
	autopush_info_t *ap ;

	for (ap = dp->ap; ap != NULL; ap = ap->link)
	{
	    int i ;

	    p1("	{%3d,", dp->major[0]) ;
	    p1("%3d,", ap->minor) ;
	    p1("%3d,", ap->lastminor) ;
	    p1("%2d, {", ap->npush) ;
	    for (i = 0; i < ap->npush; ++i)
	    {
		p1("\"%s\", ", ap->mods[i]->name) ;
	    }
	    p0("} },\n") ;
	}
    }
    p0("} ;\n") ;

    fclose(outfile) ;

#undef p0
#undef p1

} /* build_modconf */

/************************************************************************
*                           build_mknods				*
*************************************************************************
*									*
* Build the makenodes.c file.						*
*									*
************************************************************************/
void	build_mknods(void)
{
    node_info_t	*np ;

#define	p0(f)	fprintf(mkfile, f)
#define	p1(f,y)	fprintf(mkfile, f,y)

    p0("/* WARNING:  THIS FILE WAS GENERATED. "
       " MANUAL CHANGES MAY BE LOST. */\n");
    p0("\n") ;
    p0("#if defined(LINUX)\n") ;
    p0("#  include <sys/types.h>\n") ;
    p0("#  include <sys/stat.h>\n") ;
    p0("#  include <sys/sysmacros.h>\n") ;
    p0("#  define makedevice(maj,min) makedev(maj,min)\n") ;
    p0("#elif defined(QNX)\n") ;
    p0("#  include <sys/types.h>\n") ;
    p0("#  include <sys/stat.h>\n") ;
    p0("#  define makedevice(maj,min) makedev(1,maj,min)\n") ;
    p0("#elif defined(USER)\n") ;
    p0("#  include <sys/stropts.h>\n") ;
    p0("#  include <sys/LiS/usrio.h>\n") ;
    p0("#else\n") ;
    p0("#  include <sys/types.h>\n") ;
    p0("#  include <sys/stat.h>\n") ;
    p0("#  define makedevice(maj,min) makedev(maj,min)\n") ;
    p0("#endif\n") ;
    p0("\n") ;
    p0("#include <stdio.h>\n") ;
    p0("#include <unistd.h>\n") ;
    p0("#include <stdlib.h>\n") ;
    p0("\n") ;
    p0("#if !defined(LINUX)\n") ;
    p0("#  include <fcntl.h>\n") ;
    p0("#endif\n") ;
    p0("\n") ;

    p1("int %s( int argc, char *argv[] )\n", func_name) ;
    p0("{\n") ;

    p0("	int	rslt ;\n") ;
    p0("	int	rmopt = 0 ;\n") ;
    p0("	char	*strerror(int) ;\n") ;
    p0("\n") ;
    p0("#if !defined(USER)\n") ;
    p0("	(void)umask(0);\n");
    p0("	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'r')\n");
    p0("	    rmopt = 1 ;\n");
    p0("\n") ;
    p0("#endif\n") ;
    for (np = node_head; np != NULL; np = np->link)
    {
        p0("#if !defined(USER)\n") ;
	p1("	(void)unlink(\"%s\");\n", np->name) ;
        p0("#endif\n") ;
	p0("	if (!rmopt)\n") ;
	p0("	{\n") ;
	p1("	    rslt = %s(", mknod_name) ;
	p1("\"%s\", ", np->name) ;
	p1("0%o, ", np->type) ;
	p1("makedevice(%d,", np->major) ;
	p1("%d)) ;\n", np->minor) ;
	p1("	    if (rslt < 0) printf(\"%s: %%s\\n\", strerror(-rslt));\n",
			    np->name) ;
	p0("	}\n") ;
	p0("\n") ;
    }

    p0("#if !defined(USER)\n\texit(0);\n#else\n\treturn(0);\n#endif\n");

    p0("}\n") ;

    fclose(mkfile) ;

#undef p0
#undef p1

} /* build_mknods */

/************************************************************************
*                            raise                                      *
*************************************************************************
*									*
* Return the upper-case equivalent of the passed-in string.		*
*									*
* Map non-alphameric characters to underscore so that even a name	*
* containing punctuation comes out obeying C language identifier	*
* syntax.								*
*									*
************************************************************************/
char	*raise(char *p)
{
    static char		 uc[200] ;
    char		*ucp ;


    for (ucp = uc; *p; p++, ucp++)
    {
	*ucp = toupper(*p) ;
	if (   (*ucp >= 'A' && *ucp <= 'Z')
	    || (*ucp >= '0' && *ucp <= '9')
	    || *ucp == '_'
	   )
	    ;
	else
	    *ucp = '_' ;
    }
    *ucp = 0 ;

    return(uc) ;

} /* raise */

/************************************************************************
*                             build_config                              *
*************************************************************************
*									*
* Build the file config.h.						*
*									*
************************************************************************/

#undef	p0
#undef	p1
#undef	p2

#define	p0(f)		fprintf(configfile, f)
#define	p1(f,y)		fprintf(configfile, f,y)
#define	p2(f,y,z)	fprintf(configfile, f,y,z)
#define p3(f,y,z,w)     fprintf(configfile, f,y,z,w)

void	start_config(void)
{
    p0("/* WARNING:  THIS FILE WAS GENERATED. "
       " MANUAL CHANGES MAY BE LOST. */\n");
    p0("\n") ;

} /* start_config */

void	build_config(void)
{
    driver_info_t	*drp ;
    device_info_t	*dp ;
    module_info_t       *mp ;
    int			 i, devnr = 0 ;

    for (drp = driver_head; drp != NULL; drp = drp->link)
    {
	if (strcmp(drp->prefix, "-") == 0)	/* no name */
	    continue ;				/* no defines */

	p1("#define\t%s\t\t1\n", raise(drp->prefix));
	p2("#define\t%s_ID\t%d\n", raise(drp->name), drp->id);
	p2("#define\t%s_CNTLS\t%d\n", raise(drp->prefix), drp->ndev);
	p2("#define\t%s_UNITS\t%d\n", raise(drp->prefix), drp->nminors);
	p2("#define\t%s_CMAJORS\t%d\n", raise(drp->prefix), drp->nmajors);
	for (i = 0;  i < drp->nmajors;  i++)
	    p3("#define\t%s_CMAJOR_%d\t%d\n",
	       raise(drp->prefix), i + drp->inst_nr, drp->major[i] );
	/* TBD: calculate SVR4 compatible type */
	p2("#define\t%s_TYPE\t%d\n", raise(drp->name), -1);
	p0("\n") ;
    }

    for (dp = device_head; dp != NULL; dp = dp->link)
    {
	if (strcmp(dp->drvr->prefix, "-") == 0)	/* no name */
	    continue ;				/* no defines */

	devnr = dp->unit ;
	p2("#define\t%s_%u\t1\n", raise(dp->drvr->prefix), devnr);

	if (dp->port > 0)
	{
	    p2("#define\t%s_%u_SIOA\t", raise(dp->drvr->prefix), devnr);
	    p1("0x%lx\n", dp->port) ;
	}
	else
	    p2("#define\t%s_%u_SIOA\t0\n", raise(dp->drvr->prefix), devnr);

	if (dp->nports > 0)
	{
	    p2("#define\t%s_%u_EIOA\t", raise(dp->drvr->prefix), devnr);
	    p1("0x%lx\n", dp->port+dp->nports-1) ;
	}
	else
	{
	    p2("#define\t%s_%u_EIOA\t", raise(dp->drvr->prefix), devnr);
	    p1("0x%lx\n", dp->port) ;
	}

	p2("#define\t%s_%u_VECT\t", raise(dp->drvr->prefix), devnr);
	p1("%d\n", dp->irq) ;

	if (dp->mem > 0)
	{
	    p2("#define\t%s_%u_SCMA\t", raise(dp->drvr->prefix), devnr);
	    p1("0x%lx\n", dp->mem) ;
	    p2("#define\t%s_%u_ECMA\t", raise(dp->drvr->prefix), devnr);
	    p1("0x%lx\n", dp->mem + dp->mem_size - 1) ;
	}
	else
	{
	    p2("#define\t%s_%u_SCMA\t0\n", raise(dp->drvr->prefix), devnr);
	    p2("#define\t%s_%u_ECMA\t0\n", raise(dp->drvr->prefix), devnr);
	}

	p2("#define\t%s_%u_CHAN\t", raise(dp->drvr->prefix), devnr);
	p1("%d\n", dp->dma1) ;
	p2("#define\t%s_%u_CHAN2\t", raise(dp->drvr->prefix), devnr);
	p1("%d\n", dp->dma2) ;

	/* TBD:  come up with something for driver "type" */
	p0("\n") ;
    }

    for (mp = module_head; mp != NULL; mp = mp->link)
    {
	if (strcmp(mp->prefix, "-") == 0)	/* no name */
	    continue ;				/* no defines */

	p1("#define\t%s\t\t1\n", raise(mp->prefix));
	p2("#define\t%s_ID\t%d\n", raise(mp->prefix), mp->id);
	p2("#define\t%s_UNITS\t%d\n", raise(mp->prefix), mp->units);
	/* TBD: calculate SVR4 compatible type */
	p2("#define\t%s_TYPE\t%d\n", raise(mp->prefix), -1);
	p0("\n") ;
    }

   fclose(configfile) ;

#undef p0
#undef p1
#undef p2

} /* build_config */

/************************************************************************
*                             build_drvrconf                            *
*************************************************************************
*									*
* Build the file drvrconf.mk.						*
*									*
************************************************************************/

void build_drvrconf(void)
{
	FILE *f;
	driver_info_t *dp;
	module_info_t *mp;

	if ((f = fopen(drvr_name, "w")) == NULL)
	{
		perror(drvr_name);
		exit(1) ;
	}
	fprintf(f, "# WARNING:  THIS FILE WAS GENERATED. "
		" MANUAL CHANGES MAY BE LOST.\n\n");

	fprintf(f, "MODCONF_LINKS =");
	for (mp = module_head; mp != NULL; mp = mp->link)
	{
		if (!mp->obj->loadable && !mp->obj->printed)
		{
			mp->obj->printed = 1;
			fprintf(f, " %s", mp->obj->name);
		}
	}
	for (dp = driver_head; dp != NULL; dp = dp->link)
	{
		if (!dp->obj->loadable && !dp->obj->printed)
		{
			dp->obj->printed = 1;
			fprintf(f, " %s", dp->obj->name);
		}
	}
	fprintf(f, "\n");

	fprintf(f, "MODCONF_LOADS =");
	for (mp = module_head; mp != NULL; mp = mp->link)
	{
		if (mp->obj->loadable && !mp->obj->printed)
		{
			mp->obj->printed = 1;
			fprintf(f, " %s", mp->obj->name);
		}
	}
	for (dp = driver_head; dp != NULL; dp = dp->link)
	{
		if (dp->obj->loadable && !dp->obj->printed)
		{
			dp->obj->printed = 1;
			fprintf(f, " %s", dp->obj->name);
		}
	}
	fprintf(f, "\n");

	fclose(f);
}

/************************************************************************
*                             build_confmod                             *
*************************************************************************
*									*
* Build the file conf.modules.						*
*									*
************************************************************************/

void build_confmod(void)
{
	FILE *f;
	driver_info_t *dp;

	if ((f = fopen(confmod_name, "w")) == NULL)
	{
		perror(confmod_name);
		exit(1) ;
	}
	fprintf(f, "# WARNING:  THIS FILE WAS GENERATED. "
		" MANUAL CHANGES MAY BE LOST.\n\n");

	for (dp = driver_head; dp != NULL; dp = dp->link)
	{
	    int i;

	    for (i = 0;  i < dp->nmajors;  i++) {
		fprintf(f, "alias char-major-%-4d ", dp->major[i]);
		if (dp->obj->loadable)
			fprintf(f, "streams-%-20s", dp->obj->name);
		else
			fprintf(f, "streams %20s", "");
		fprintf(f, "# driver %s\n", dp->name);
	    }
	}

	fclose(f);
}

/************************************************************************
*                            print_options                              *
************************************************************************/
void	print_options(void)
{

#define	p	printf

p("Usage:  strconf <options> <input-file>\n");
p("\n");
p("<options>\n") ;
p("\n");
p("-b<number>   Set the base major number for automatic assignment\n");
p("             of major device numbers.\n");
p("-h<file>     Set the output header file name to something other than\n");
p("             the default \"config.h\".\n");
p("-o<file>     Set the output file name rather than the default\n");
p("             name \"modconf.c\".  This is the file that must be\n");
p("             linked in with STREAMS.\n");
p("-m<file>     Set the output file name for the program that will\n");
p("             make the nodes to something other than \"makenodes.c\".\n");
p("             This program is compiled and then run at boot time\n");
p("             to create all the /dev entries for the STREAMS drivers.\n");
p("-M<name>     The name of the mknod() routine to call.  The default\n");
p("             is \"mknod\".\n");
p("-p<perm>     Set default permissions to something other than 0666.\n");
p("-r<name>     The name of the routine to build in makenodes.c.\n");
p("             The default is \"main\".\n") ;

#undef p

} /* print_options */

/************************************************************************
*                          get_options                                  *
*************************************************************************
*									*
* Get command line options.						*
*									*
************************************************************************/
void	get_options(int argc, char **argv)
{
    char	*p = NULL ;

    for (argc--, argv++; argc > 0; argc--, argv++)
    {
	p = *argv ;
	if (*p == '-')
	    switch (*++p)
	    {
	    case 'b':			/* -b<number> */
		next_major = (int) strtol(p+1, NULL, 0) ;
		break ;
	    case 'h':			/* -h<file> */
		config_name = p+1 ;
		break ;
	    case 'o':			/* -o<file> */
		outfile_name = p+1 ;
		break ;
	    case 'm':			/* -m<file> */
		mkfile_name = p+1 ;
		break ;
	    case 'M':			/* -M<name> */
		mknod_name = p+1 ;
		break ;
	    case 'p':
		dflt_perm = (int) strtol(p+1, NULL, 0) ;
		break ;
	    case 'r':
	    	func_name = p+1 ;	/* -p<name> */
		break ;
	    case 'l':			/* -l<file> */
		drvr_name = p+1 ;
		break ;
	    case 'L':			/* -L<file> */
		confmod_name = p+1 ;
		break ;
	    default:
		print_options() ;
		exit(1) ;
	    }
	else
	    infile_name = p ;
    }

    if (infile_name == NULL || outfile_name == NULL || mkfile_name == NULL ||
        mknod_name == NULL || config_name == NULL)
    {
	print_options() ;
	exit(1) ;
    }

} /* get_options */

/************************************************************************
*                              main                                     *
************************************************************************/
int	main(int argc, char **argv)
{
    get_options(argc, argv) ;

    /*
     * Open all the files
     */
    infile = fopen(infile_name, "r") ;
    if (infile == NULL)
    {
	perror(infile_name) ;
	exit(1) ;
    }

    outfile = fopen(outfile_name, "w") ;
    if (outfile == NULL)
    {
	perror(outfile_name) ;
	exit(1) ;
    }

    mkfile = fopen(mkfile_name, "w") ;
    if (mkfile == NULL)
    {
	perror(mkfile_name) ;
	exit(1) ;
    }

    configfile = fopen(config_name, "w") ;
    if (configfile == NULL)
    {
	perror(config_name) ;
	exit(1) ;
    }
    start_config() ;
    process_file() ;
    build_modconf() ;
    build_mknods() ;
    build_config() ;

    build_drvrconf() ;
    build_confmod() ;
    exit(0) ;
    return(0) ;

} /* main */
