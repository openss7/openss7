/*
 *  sad.h  - declarations for Streams Administrative Driver and autopush
 */

#ident "@(#) LiS sad.h 2.4 11/22/00 17:27:29 "

#ifndef _SYS_SAD_H
#define _SYS_SAD_H

#ifndef _SYS_STROPTS_H
#include <sys/stropts.h>	/* for FMNAMESZ */
#endif

/*
 *  In LiS, the device is a simple cloneable node.
 *  Only root can do admin.
 */
#define USERDEV "/dev/sad"
#define ADMINDEV "/dev/sad"

#define MAXAPUSH        8	/* max # of autopushed mods */

#define SADIOC	('D' << 8)
#define SAD_SAP	(SADIOC + 1)	/* Set autopush	configuration		*/
#define SAD_GAP	(SADIOC + 2)	/* Get autopush	configuration		*/
#define SAD_VML	(SADIOC + 3)	/* Validate list of modules		*/

struct apcommon {
	unsigned apc_cmd;	/* Command				*/
	major_t apc_major;	/* Major device number			*/
	minor_t apc_minor;	/* First minor device number		*/
	minor_t apc_lastminor;	/* Last minor device number		*/
	unsigned int apc_npush;	/* Number of modules to autopush	*/
};

/* Values for apcommon.apc_cmd field above */
#define SAP_CLEAR	0	/* Clear an entry			*/
#define SAP_ONE		1	/* Add an entry for one minor		*/
#define SAP_RANGE	2	/* Add an entry for range of minors	*/
#define SAP_ALL		3	/* Add an entry for all minors		*/

/* SAD_SAP and SAD_GAP use this structure */
struct strapush {
	struct apcommon sap_common;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
};

/* Short names for strapush.sap_common fields above */
#define sap_cmd		sap_common.apc_cmd
#define sap_major	sap_common.apc_major
#define sap_minor	sap_common.apc_minor
#define sap_lastminor	sap_common.apc_lastminor
#define sap_npush	sap_common.apc_npush

#endif
