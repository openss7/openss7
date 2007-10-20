/* config.h - compile time configuration parameters */

/* 
 * $Header: /xtel/isode/isode/h/quipu/RCS/config.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: config.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
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
 * OPTIONS:-
 *
 * #define PDU_DUMP
 * 		If this is defined, and "dish" is invoke with
 *		dish -pdu foobar
 *		Then a directory "foobar" will be created, and 
 *		it will contain logs of all the X500 PDUs sent
 *
 * #define NO_STATS
 * 		If defined, the QUIPU will NOT produce statistical
 *		logs of both the DSA and DUA.
 *
 * #define CHECK_FILE_ATTRIBUTES
 * 		If and EDB entry contains a FILE attribute, check that
 *		the corresponding file exists
 *
 * #define QUIPU_MALLOC	
 * 		Use a version of malloc optimised for the memory
 * 		resident Quipu DSA database.
 *
 * #define TURBO_DISK
 *		Store EDB files in gdbm files instead of plain text files.
 *		This makes modifies of entries in large EDB files much
 *		faster.  See the ../../quipu/turbo directory for tools to
 *		help in converting your EDB files.
 *
 * #define TURBO_INDEX
 *		Enable code to build and search database indexes for
 *		selected attributes (see tailor file options optimize_attr,
 *		index_subtree, and index_siblings).
 *		This can cut the search	time for very large databases.
 *
 * #define SOUNDEX_PREFIX
 *		Consider soundex prefixes as matches.  For example, make
 *		"fred" match "frederick".  #defining this option gives
 *		approximate matching behavior the same as in version 6.0.
 *
 * #define STRICT_X500
 *		Enforce X.500 more strictly than "normal".
 *		Useful for conformance testing, but not "real users".
 *
 * #define HAVE_PROTECTED
 *		If defined, enable use of protectedPassword attribute.
 *
 * #define QUIPU_CONSOLE
 *		Undocumented pre-alpha test feature.
 *
 */

 
# ifndef QUIPU_CONFIG
# define QUIPU_CONFIG

#ifndef	USE_BUILTIN_OIDS
#define	USE_BUILTIN_OIDS	1
#endif

# define PDU_DUMP   
# define QUIPU_MALLOC
# define TURBO_INDEX
# define SOUNDEX_PREFIX
# define HAVE_PROTECTED

# endif 
