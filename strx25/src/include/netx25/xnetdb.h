/*****************************************************************************

 @(#) $Id: xnetdb.h,v 0.9.2.2 2008-05-03 21:22:37 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-05-03 21:22:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xnetdb.h,v $
 Revision 0.9.2.2  2008-05-03 21:22:37  brian
 - updates for release

 Revision 0.9.2.1  2008-05-03 10:46:38  brian
 - added package files

 *****************************************************************************/

#ifndef __NETX25_XNETDB_H__
#define __NETX25_XNETDB_H__

#ident "@(#) $RCSfile: xnetdb.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed wtih doxygen(1). */

#include <stdint.h>
#include <sys/types.h>

/** @weakgroup sx25
  * @{ */

/** @file src/include/netx25/xnetdb.h xnetdb.h
  * User level X.25 network database header file.
  * This file contains declarations for the X.25 database library subroutines,
  * most of which utility the struct xhostent structure. */

struct link_data;
struct config_ident;
struct LINK_config_data;
struct X25_config_data;
struct MLP_config_data;
struct LAPB_config_data;
struct LLC2_config_data;
struct WAN_config_data;

/* Addressing is defined by the xaddrf structure.  The members of the xaddrf
 * structure are:
 *
 * link_id:	Holds the link number as a uint32_t.  By default, link_id has
 *		a value of 0xFF.  When link_id is 0xFF, X.25 attempts to match
 *		the called address with an entry in a routing configuration
 *		file.  If it cannot find a match, it routes the call over the
 *		lowest numbered WAN link.
 *
 * aflags:	Specifies the options required or used by the subnetwork to
 *		encode and interpret addresses.  These take on of these values:
 *
 *		NSAP_ADDR   0x00    NSAP field contains OSI-encoded NSAP
 *				    address.
 *		EXT_ADDR    0x01    NSAP field contains non-OSI-encoded
 *				    extended address.
 *		PVC_LCI	    0x02    NSAP field contains a PVC number.
 *
 * DTE_MAC:	The DTE address or LSAP as two BCD digits per byte, right
 *		justified, or the PVC_LCI as three BCD digits with two digits
 *		per byte, right justified.
 *
 * nsap_len:	The length in semi-octets of the NSAP as two BCD digits per
 *		byte, right justified.
 *
 * NSAP:	The NSAP or address extension (see aflags) as two BCD digits
 *		per byte, right justified.
 */
#define NSAPMAXSIZE 20
struct xaddrf {
	uint32_t link_id;
	unsigned char aflags;
	struct lsapformat DTE_MAC;
	unsigned char nsap_len;
	unsigned char NSAP[NSAPMAXSIZE];
};

/* The padent structure is defined in netx25/xnetdb.h.  The structure contains
 * a single entry from the /etc/x25/padmapconf file.  This contains
 * information about facilities and so on to be used when making PAD calls to
 * a particular adress.  The members of the padent structure are:
 *
 * xaddr:	The host X.25 address.
 *
 * x29:		The X.29 version specified.  Possible values are:
 *
 *		0 - use the configured default X.29 address
 *		1 - use the yellow book (1980) X.29 address
 *		2 - use the red book (1984) X.29 address
 *		3 - use the blue book (1988) X.29 address
 *
 * xtras:	Any facilities and QOS parameters defined for this entry.
 *
 * cud:		Any Call User Data defined for this entry.
 */
#if 0
#define MAX_HXCUDFSIZE MAxnetdb.hXCUDFSIZE
#else
#define MAX_HXCUDFSIZE 63
#endif
struct padent {
	struct xaddrf xaddr;
	unsigned char x29;
	struct extraformat xtras;
	unsigned char cud[MAX_HXCUDFSIZE + 1];
};

/* The xhostent structure is defined in the netx25/xnetdb.h file.  The
 * structure contains a single entry from the xhosts file.  This contains
 * information mapping host names to X.25 addresses and is used when making PAD
 * calls.  By default, this file is in the /etc/x25/ directory.  The members of
 * the structure are:
 *
 * h_name:	A pointer to the name of the X.25 host, as defined in the
 *		xhosts file.
 * h_aliases:	A pointer to an array of character pointers that point to
 *		aliases for the X.25 host.
 * h_addrtype:	The type of address being returned.  This is always CCITT_X25.
 * h_length:	The length in bytes of the structure that contains the X.25
 *		address.
 * h_addr:	A pointer to an xaddrf structure containing the network address
 *		of the X.25 host.
 */
struct xhostent {
	char *h_name;
	char **h_aliases;
	int h_addrtype;
	int h_length;
	char *h_addr;
};

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/** @name X.25 Network Database API Functions
  * API functions for the X.25 Network Database.
  * @{ */
/**
  * a weak alias of __sx25_padtos(). */
extern int padtos(struct padent *p, unsigned char *strp);

/**
  * a weak alias of __sx25_linkidtox25(). */
extern unsigned long linkidtox25(unsigned char *str_linkid);

/**
  * a weak alias of __sx25_x25tolinkid(). */
extern int x25tolinkid(unsigned long linkid, unsigned char *str_linkid);

/**
  * a weak alias of __sx25_getnettype(). */
extern int getnettype(unsigned char *linkid);

/**
  * a weak alias of __sx25_setpadent(). */
extern void setpadent(int stayopen);

/**
  * a weak alias of __sx25_getpadbyaddr(). */
extern struct padent *getpadbyaddr(char *addr);

/**
  * a weak alias of __sx25_getpadent(). */
extern struct padent *getpadent(void);

/**
  * a weak alias of __sx25_endpadent(). */
extern void endpadent(void);

/** Compare two X.25 addresses:
  * a weak alias of __sx25_equalx25(). */
extern int equalx25(struct xaddrf *x1, struct xaddrf *x2);

/** Convert X.25 address to xaddrf structure:
  * a weak alias of __sx25_stox25(). */
extern int stox25(unsigned char *cp, struct xaddrf *xad, int lookup);

/** Convert xaddrf structure to X.25 address:
  * a weak alias of __sx25_x25tos(). */
extern int x25tos(struct xaddrf *xad, unsigned char *cp, int lookup);

/**
  * a weak alias of __sx25_setxhostent(). */
extern void setxhostent(int stayopen);

/** Get X.25 host name by address:
  * a weak alias of __sx25_getxhostbyaddr(). */
extern struct xhostent *getxhostbyaddr(char *addr, int len, int type);

/** Get X.25 address by name:
  * a weak alias of __sx25_getxhostbyname(). */
extern struct xhostent *getxhostbyname(char *name);

/**
  * a weak alias of __sx25_getxhostent(). */
extern struct xhostent *getxhostent(void);

/**
  * a weak alias of __sx25_endxhostent(). */
extern void endxhostent(void);

/**
  * a weak alias of __sx25_find_link_parameters(). */
extern int x25_find_link_parameters(struct link_data **lptr);

/**
  * a weak alias of __sx25_read_config_parameters(). */
extern int x25_read_config_parameters(int linkid,
				      struct config_ident *ipt,
				      struct LINK_config_data *lpt,
				      struct X25_config_data *xpt,
				      struct MLP_config_data *mpt,
				      struct LAPB_config_data *lbp,
				      struct LLC2_config_data *l2p,
				      struct WAN_config_data *wpt, int *flags);
/**
  * a weak alias of __sx25_read_config_parameters_file(). */
extern int x25_read_config_parameters_file(char *filename,
					   struct config_ident *ipt,
					   struct LINK_config_data *lpt,
					   struct X25_config_data *xpt,
					   struct MLP_config_data *mpt,
					   struct LAPB_config_data *lbp,
					   struct LLC2_config_data *l2p,
					   struct WAN_config_data *wpt, int *flags);
/**
  * a weak alias of __sx25_save_link_parameters(). */
extern int x25_save_link_parameters(struct link_data *linkid);

/**
  * a weak alias of __sx25_set_parse_error_function(). */
extern int (*x25_set_parse_error_function(int (*func)(char *))) (char *) ;

/**
  * a weak alias of __sx25_write_config_parameters(). */
extern int x25_write_config_parameters(int linkid,
				       struct config_ident *ipt,
				       struct LINK_config_data *lpt,
				       struct X25_config_data *xpt,
				       struct MLP_config_data *mpt,
				       struct LAPB_config_data *lbp,
				       struct LLC2_config_data *l2p, struct WAN_config_data *wpt);
/**
  * a weak alias of __sx25_write_config_parameters_file(). */
extern int x25_write_config_parameters_file(char *filename,
					    struct config_ident *ipt,
					    struct LINK_config_data *lpt,
					    struct X25_config_data *xpt,
					    struct MLP_config_data *mpt,
					    struct LAPB_config_data *lbp,
					    struct LLC2_config_data *l2p,
					    struct WAN_config_data *wpt);
/** @} */

/** @name X.25 Network Database API Functions
  * These are the internal implementations of the functions.  The formal
  * functions from <netx25/xnetdb.h> are weak aliases to these.
  * @{ */
extern int __sx25_padtos(struct padent *p, unsigned char *strp);
extern unsigned long __sx25_linkidtox25(unsigned char *str_linkid);
extern int __sx25_x25tolinkid(unsigned long linkid, unsigned char *str_linkid);
extern int __sx25_getnettype(unsigned char *linkid);

extern void __sx25_setpadent(int stayopen);
extern struct padent *__sx25_getpadbyaddr(char *addr);
extern struct padent *__sx25_getpadent(void);
extern void __sx25_endpadent(void);

extern int __sx25_equalx25(struct xaddrf *x1, struct xaddrf *x2);
extern int __sx25_stox25(unsigned char *cp, struct xaddrf *xad, int lookup);
extern int __sx25_x25tos(struct xaddrf *xad, unsigned char *cp, int lookup);

extern void __sx25_setxhostent(int stayopen);
extern struct xhostent *__sx25_getxhostbyaddr(char *addr, int len, int type);
extern struct xhostent *__sx25_getxhostbyname(char *name);
extern struct xhostent *__sx25_getxhostent(void);
extern void __sx25_endxhostent(void);

extern int __sx25_find_link_parameters(struct link_data **lptr);
extern int __sx25_read_config_parameters(int linkid,
					 struct config_ident *ipt,
					 struct LINK_config_data *lpt,
					 struct X25_config_data *xpt,
					 struct MLP_config_data *mpt,
					 struct LAPB_config_data *lbp,
					 struct LLC2_config_data *l2p,
					 struct WAN_config_data *wpt, int *flags);
extern int __sx25_read_config_parameters_file(char *filename,
					      struct config_ident *ipt,
					      struct LINK_config_data *lpt,
					      struct X25_config_data *xpt,
					      struct MLP_config_data *mpt,
					      struct LAPB_config_data *lbp,
					      struct LLC2_config_data *l2p,
					      struct WAN_config_data *wpt, int *flags);
extern int __sx25_save_link_parameters(struct link_data *linkid);
extern int (*__sx25_set_parse_error_function(int (*func)(char *))) (char *) ;
extern int __sx25_write_config_parameters(int linkid,
					  struct config_ident *ipt,
					  struct LINK_config_data *lpt,
					  struct X25_config_data *xpt,
					  struct MLP_config_data *mpt,
					  struct LAPB_config_data *lbp,
					  struct LLC2_config_data *l2p,
					  struct WAN_config_data *wpt);
extern int __sx25_write_config_parameters_file(char *filename,
					       struct config_ident *ipt,
					       struct LINK_config_data *lpt,
					       struct X25_config_data *xpt,
					       struct MLP_config_data *mpt,
					       struct LAPB_config_data *lbp,
					       struct LLC2_config_data *l2p,
					       struct WAN_config_data *wpt);
/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

/** @} */

#endif				/* __NETX25_XNETDB_H__ */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
