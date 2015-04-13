/*****************************************************************************

 @(#) src/include/sys/xti_osi.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef _SYS_XTI_OSI_H
#define _SYS_XTI_OSI_H

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI ISO-Specific header file.
  *
  * This file contains specific ISO option and management parameters.  */

/*
 * XTI ISO-Specific Header File.
 *
 * This file contains specific ISO option and management parameters.
 */

/**
  * @name ISO Transport Classes
  * Definition of the ISO transport classes.
  *
  * @{ */
#define T_CLASS0	0
#define T_CLASS1	1
#define T_CLASS2	2
#define T_CLASS3	3
#define T_CLASS4	4
/** @} */

/**
  * @name Priorities
  * Definition of the priorities.
  *
  * @{ */
#define T_PRITOP	0
#define T_PRIHIGH	1
#define T_PRIMID	2
#define T_PRILOW	3
#define T_PRIDFLT	4
/** @} */

/**
  * @name Protection Levels
  * Definitions of the protection levels.
  *
  * @{ */
#define T_NOPROTECT		1
#define T_PASSIVEPROTECT	2
#define T_ACTIVEPROTECT		4
/** @} */

/**
  * Rate structure.
  */
struct rate {
	t_scalar_t targetvalue;		/**< Target value. */
	t_scalar_t minacceptvalue;	/**< Value of minimum acceptable quality. */
};

/**
  * Request value structure.
  */
struct reqvalue {
	struct rate called;		/**< Called rate. */
	struct rate calling;		/**< Calling rate. */
};

/**
  * Throughput structure.
  */
struct thrpt {
	struct reqvalue maxthrpt;	/**< Maximum throughput. */
	struct reqvalue avgthrpt;	/**< Average throughput. */
};

/**
  * Transit delay structure.
  */
struct transdel {
	struct reqvalue maxdel;		/**< Maximum transit delay. */
	struct reqvalue avgdel;		/**< Average transit delay. */
};

/**
  * ISO Transport Option Level.
  */
#define T_ISO_TP	0x0100

/**
  * @name QOS Options
  * Options for Quality of Service and Expedited Data (ISO 8072:1994).
  *
  * @{ */
#define T_TCO_THROUGHPUT	0x0001	/**< struct thrpt, octets per second. */
#define T_TCO_TRANSDEL		0x0002	/**< struct transdel, milliseconds. */
#define T_TCO_RESERRORRATE	0x0003	/**< struct rate, -log10(ratio). */
#define T_TCO_TRANSFFAILPROB	0x0004	/**< struct rate, -log10(ratio). */
#define T_TCO_ESTFAILPROB	0x0005	/**< struct rate, -log10(ratio). */
#define T_TCO_RELFAILPROB	0x0006	/**< struct rate, -log10(ratio). */
#define T_TCO_ESTDELAY		0x0007	/**< struct rate, milliseconds. */
#define T_TCO_RELDELAY		0x0008	/**< struct rate, milliseconds. */
#define T_TCO_CONNRESIL		0x0009	/**< struct rate, -log10(ratio). */
#define T_TCO_PROTECTION	0x000a	/**< t_uscalar_t, values above. */
#define T_TCO_PRIORITY		0x000b	/**< t_uscalar_t, values above. */
#define T_TCO_EXPD		0x000c	/**< t_uscalar_t T_YES/T_NO. */
/** @} */

/**
  * @name Management Options
  *
  * @{ */
#define T_TCO_LTPDU		0x0100	/**< t_uscalar_t, octets. */
#define T_TCO_ACKTIME		0x0200	/**< t_uscalar_t, milliseconds. */
#define T_TCO_REASTIME		0x0300	/**< t_uscalar_t, seconds. */
#define T_TCO_PREFCLASS		0x0900	/**< t_uscalar_t, see above. */
#define T_TCO_ALTCLASS1		0x0a00	/**< t_uscalar_t, see above. */
#define T_TCO_ALTCLASS2		0x0b00	/**< t_uscalar_t, see above. */
#define T_TCO_ALTCLASS3		0x0c00	/**< t_uscalar_t, see above. */
#define T_TCO_ALTCLASS4		0x0d00	/**< t_uscalar_t, see above. */
#define T_TCO_EXTFORM		0x0400	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_FLOWCTRL		0x0500	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_CHECKSUM		0x0600	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_NETEXP		0x0700	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_NETRECPTCF	0x0800	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_SELECTACK		0x0e00	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_REQUESTACK	0x0f00	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */
#define T_TCO_NBLKEXPDATA	0x1000	/**< t_uscalar_t, T_YES/T_NO/T_UNSPEC. */

#define T_TCL_TRANSDEL		0x000d
#define T_TCL_RESERRORRATE	T_TCO_RESERRORRATE
#define T_TCL_PROTECTION	T_TCO_PROTECTION
#define T_TCL_PRIORITY		T_TCO_PRIORITY
#define T_TCL_CHECKSUM		T_TCO_CHECKSUM
/** @} */

/**
  * @name Address/Selector Sizes
  *
  * @{ */
#define NODSIZ		 6	/**< fixed length of 802.[345] address */
#define MACSIZ		20	/**< max length of MAC address */
#define NSAPSZ		20	/**< max length of NSAP address */
#define SUBNSZ		20	/**< max length of subnetwork address */
#define TSAPSZ		32	/**< max length of TSAP selector */
#define SSAPSZ		16	/**< max length of SSAP selector */
#define PSAPSZ		16	/**< max length of PSAP selector */
/** @} */

/**
  * Node structure
  */
typedef struct {
	unsigned char addr[NODSIZ];	/* node address */
} node_address;

/**
  * MAC structure
  */
typedef struct {
	unsigned len;			/* MAC address length */
	unsigned char addr[MACSIZ];	/* MAC address */
} mac_address;

/**
  * Subnet structure
  */
typedef struct {
	unsigned len;			/* subnet address length */
	unsigned char addr[SUBNSZ];	/* subnet address */
} subnet_address;

/**
  * LSAP selector
  */
typedef unsigned char lsap_selector;	/* DLSAP selector */

/**
  * NSAP address structure
  */
typedef struct {
	unsigned len;			/* NSAP address length */
	unsigned char addr[NSAPSZ];	/* NSAP address (binary) */
} nsap_address;

/**
  * NSAP selector
  */
typedef unsigned char nsap_selector;	/* NSAP selector */

/**
  * NSAP service user identifier
  */
typedef unsigned char nsu_id;		/* NSAP service user TLID */

/**
  * TSAP selector structure
  */
typedef struct {
	unsigned len;			/* TSAP selector length */
	unsigned char addr[TSAPSZ];	/* TSAP selector */
} tsap_selector;

/**
  * TSAP address structure
  */
typedef struct {
	tsap_selector tsap;		/* TSAP selector */
	nsap_address nsap;		/* NSAP address */
} tsap_address;

/**
  * SSAP selector structure
  */
typedef struct {
	unsigned len;			/* SSAP selector length */
	unsigned char addr[SSAPSZ];	/* SSAP selector */
} ssap_selector;

/**
  * SSAP address structure
  */
typedef struct {
	ssap_selector ssap;		/* SSAP id */
	tsap_selector tsap;		/* TSAP id */
	nsap_address nsap;		/* NSAP address */
} ssap_address;

/**
  * PSAP selector structure
  */
typedef struct {
	unsigned len;			/* PSAP selector length */
	unsigned char addr[PSAPSZ];	/* PSAP selector */
} psap_selector;

/**
  * PSAP address structure
  */
typedef struct {
	psap_selector psap;		/* PSAP id */
	ssap_selector ssap;		/* SSAP id */
	tsap_selector tsap;		/* TSAP id */
	nsap_address nsap;		/* NSAP address */
} psap_address;

#endif				/* _SYS_XTI_OSI_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
