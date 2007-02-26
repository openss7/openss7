/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srmap.h
 * Description                  : SRAM protocol - mapping definitions
 *
 *
 **********************************************************************/

#ifndef __SRMAP_H__
#define __SRMAP_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "typedefs.h"
#include "gndrv.h"
#endif


/*
 * The following defines are used by a PM to permit/deny
 * overlapping of shared ram segments between boards.
 */
#define SR_NOSHARESEG            0
#define SR_SHARESEG              1


typedef struct sr_ramlst         SR_RAMLST;
typedef struct sr_bdsram         SR_BDSRAM;

/*
 * A SR_SEG table describes all the memory areas where boards may be
 * located. It is created at PM initialization time.
 */
typedef struct sr_seg {
   UCHAR    *sg_phys;            /* Physical address */
   UCHAR    *sg_virt;            /* Base address in kernel memory map */
#ifdef PCI_SPAN
   UCHAR    *sg_cfgvirt;         /* Base address in kernel memory map for cfg*/
#endif /* PCI_SPAN */
   ULONG    sg_size;             /* Segment size in bytes */
#ifdef SOLARIS_24
   ULONG    sg_npages;           /* Number of pages */
   unsigned long    sg_pageaddr;         /* Pages starting address */
   unsigned long    sg_mapped_addr;      /* mapped address in the kernel */
#endif /* SOLARIS_24 */
   ULONG    sg_rfu;
} SR_SEG;


/*
 * SR_RAMLST: This forms a linked list of board RAM segments. It is
 * maintained in ascending order, to detect holes and overlaps.
 */
struct sr_ramlst {
   SR_RAMLST   *rl_next;         /* Must be 1st for q routns to work */
   GN_BOARD    *rl_bdp;          /* Generic board pointer */
   SR_BDSRAM   *rl_srp;          /* Ptr to board's SR_BDSRAM struct */
   UCHAR       *rl_phys;         /* Physical address of the segment */
   ULONG       rl_size;          /* Size of the segment */
   ULONG       rl_part;          /* For boards that cross segments */
   ULONG       rl_rfu[2];
};


/*
 * SR_BDSRAM: Holds information for mapping board addresses into the
 * kernel's address space.
 */
struct sr_bdsram {
   UCHAR    *br_physaddr;        /* Base addr of shared RAM (phys) */
   UCHAR    *br_sramaddr;        /* Base addr of shared RAM (virt; part 1) */
   UCHAR    *br_sramaddr2;       /* Base addr or shared RAM (virt; part 2) */
#ifndef VME_SPAN
   USHORT   br_totmem;           /* Size of shared RAM area in bytes */
   USHORT   br_ioport;           /* Board IO port address */
   USHORT   br_segid;
   USHORT   br_offset;
   USHORT   br_memlen;
   USHORT   br_segid2;
   USHORT   br_offset2;
   USHORT   br_memlen2;
#ifdef PCI_SPAN
   USHORT   br_pcibrdid;
#endif /* PCI_SPAN */
#else
   ULONG    br_totmem;           /* Size of shared RAM area in bytes */
   ULONG    br_ioport;           /* Board IO port address */
   ULONG    br_segid;
   ULONG    br_offset;
   ULONG    br_memlen;
   ULONG    br_segid2;
   ULONG    br_offset2;
   ULONG    br_memlen2;
#endif /* VME_SPAN */
   ULONG    br_rfu[1];
};


/*
 * SR_MAPLST: Control structure for mapping functions. The call to 
 * sr_mapinit() returns a pointer to a structure of this type which
 * must be saved by the PM and used whenever calling any of the
 * mapping functions. THE PM MUST **NOT** MODIFY THIS STRUCTURE!
 */
typedef struct sr_maplst {
   SR_SEG      *sm_segp;         /* Ptr to SR_SEG table */
   SR_SEG      *sm_esegp;        /* Ptr to end of SR_SEG table */
   SR_RAMLST   *sm_rlbasep;      /* Ptr to base of SR_RAMLST table */
   SR_RAMLST   *sm_rlstartp;     /* Ptr to 1st SR_RAMLST table entry */
   ULONG       sm_memsz;         /* # bytes used by mapping tables */
   ULONG       sm_numents;       /* Current # entries in the tables */
   ULONG       sm_rfu[2];
} SR_MAPLST;


/*
 * Function prototypes
 */
#ifdef LFS
extern SR_MAPLST *sr_mapinit(ULONG);
extern int sr_mapadd(SR_MAPLST *, GN_BOARD *, SR_BDSRAM *, int);
extern int sr_map2mem(SR_MAPLST *);
extern int sr_unmap(SR_MAPLST *);
#ifndef VME_SPAN
extern int sr_checkbd(register UCHAR *, USHORT);
extern int sr_clearbd(register UCHAR *, USHORT);
#else
extern int sr_checkbd(register UCHAR *, ULONG);
extern int sr_clearbd(register UCHAR *, ULONG);
#endif
#else
extern SR_MAPLST *sr_mapinit();
extern int sr_mapadd();
extern int sr_map2mem();
extern int sr_unmap();
extern int sr_checkbd();
extern int sr_clearbd();
#endif


#endif

