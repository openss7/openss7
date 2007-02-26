/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srmap.c
 * Description                  : SRAM protocol - segment mapping 
 *
 *
 **********************************************************************/

#ifdef LFS
#define _LIS_SOURCE	1
#include <sys/os7/compat.h>
#else
#include <sys/LiS/config.h>
#if (CLONE__CMAJOR_0 > 220)
#define DLGC_LIS_FLAG
#include <sys/LiS/module.h>
#endif
#endif
#include "dlgclinux.h"
#include <linux/mm.h>

#include "typedefs.h"

#include "gndefs.h"
#include "gndrv.h"

#include "d40defs.h"
#include "dxdevdef.h"
#include "srmap.h"

#include "srdefs.h"
#include "drvdebug.h"
#ifdef LINUX
#include "dlgcos.h"
#endif /* LINUX */

#ifdef PCI_SPAN
#include "dpci.h"
#endif /* PCI_SPAN */

#if (defined(SVR4) || defined(UNISYS_MP) || defined(UNISYS_UP))
   #ifndef SE_NOSLP
      #ifdef SVR4
         #ifdef sun
         #else
	    #if defined(SVR5) && DDI8
            #else /* ! DDI8 */
               #include <sys/strsubr.h>
            #endif /* ! DDI8 */
         #endif /* sun */
      #else
         #define SE_NOSLP 1
      #endif
   #endif
#endif

#if defined(SVR4_MOT) || (defined(SVR5) && DDI8)
#include <sys/ddi.h>
#endif /* SVR4_MOT */

/*
 * Define macros for segment/offset manipulation
 */
#ifdef VME_SPAN
#else
#define seg(addr)       (((ULONG)(addr)) & 0xFFFF0000)
#define offset(addr)    (((ULONG)(addr)) & 0x0000FFFF)
#endif /* VME_SPAN */

#ifdef LINUX
int order = 5;
extern int Single_Board_SS;
#endif /* LINUX */

/*
 * Private function prototypes
 */
#ifdef LFS
PRIVATE int overlap(SR_RAMLST *, SR_RAMLST *);
PRIVATE int rladd(SR_MAPLST *, UCHAR *, ULONG, GN_BOARD *, SR_BDSRAM *, ULONG, int);
PRIVATE int buildsegs(SR_MAPLST *);
PRIVATE void unmapsegs(SR_MAPLST *);
#if (defined(PCI_SPAN) && !defined(sun))
PRIVATE UCHAR *sram_getvirtmem(ULONG, ULONG);
#endif /* SVR3 */
#else
PRIVATE int overlap();
PRIVATE int rladd();
PRIVATE int buildsegs();
PRIVATE void unmapsegs();
#if (defined(PCI_SPAN) && !defined(sun))
PRIVATE UCHAR *sram_getvirtmem();
#endif /* SVR3 */
#endif
#ifdef VME_SPAN /*  */
extern int dlgn_FORCEvme_5v;
extern caddr_t dlgn_map_regs(int, int);
extern void dlgn_unmap_regs(int, int, char **);
#if defined(SVR5) && DDI8
extern rm_key_t dlgn_getrmkey(unsigned short);
extern void *dlgn_physmap(UCHAR *, ULONG);
#endif	/* DDI8 */

static sr_map_init = 0;
char *sr_map_regs(int, int);
int sr_unmap_regs(int, int);
#endif /* VME_SPAN */

#ifdef DLGN_DEBUG
extern unsigned short dlgn_debug;
#endif /* DLGN_DEBUG */

#ifdef PCI_SPAN
unsigned char *IsaGlobalRearmAddress = 0;
extern PGP_CONTROL_BLOCK GpControlBlock;
extern unsigned char NumOfVoiceBoards;
#endif /* PCI_SPAN */

#ifdef SVR4_MOT
int
sr_mapmem(SR_BDSRAM *srp)
{
   /* 
   ** Code for ISA/UNIX SVR4 
   **
   srp->br_sramaddr = (UCHAR *)(
         ((ULONG)srp->br_physaddr | KVBASE) + srp->br_offset);
   srp->br_memlen = srp->br_totmem;
   **
   */

   srp->br_sramaddr = (UCHAR *)niomapin(
         (ULONG)srp->br_physaddr + srp->br_offset, srp->br_totmem);

   srp->br_memlen = srp->br_totmem;

   srp->br_sramaddr2 = 0;
   srp->br_memlen2 = 0;

   STRACE3("sr_mapmem: physaddr=0x%lx, offset=0x%lx, MAPPED addr=0x%lx", srp->br_physaddr, srp->br_offset, srp->br_sramaddr);

   return(0);
}
#endif /* SVR4_MOT */


#ifndef SVR4_MOT
/***********************************************************************
 *        NAME: sr_mapinit
 * DESCRIPTION: Initializes shared RAM mapping functions for PM use.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: SR_MAPLST ptr on success, NULL on failure.
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
SR_MAPLST *sr_mapinit(nbds)
ULONG nbds;
{
   int         segsz;
   int         rlsz;
   int         memsz;
   SR_MAPLST   *mlp;

#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0004)
      cmn_err(CE_NOTE,"sr_mapinit(%d)", nbds);
#endif /* DLGN_DEBUG */

   /* Compute the amount of memory required */
   segsz = (2 * nbds + 1) * sizeof(SR_SEG);
   rlsz  = nbds * 2 * sizeof(SR_RAMLST);
   memsz = segsz + rlsz + sizeof(SR_MAPLST);

   /* Allocate the memory */
#if (defined(sun) || defined(SVR5))
   if ((mlp = (SR_MAPLST *)kmem_zalloc(memsz, KM_NOSLEEP)) == NULL) {
#elif LINUX
#ifdef LFS
   if ((mlp = (SR_MAPLST *)kmem_zalloc(memsz, KM_NOSLEEP)) == NULL) { 
#else
   /* This is an error.  sr_mapinit can be called from a write put procedure and
    * must not sleep. GFP_ATOMIC must be used here instead of GFP_KERNEL. --bb */
   if ((mlp = (SR_MAPLST *)lis_kmalloc(memsz, GFP_KERNEL)) == NULL) { 
#endif
#else
   if ((mlp = (SR_MAPLST *)kseg(btoc(memsz))) == NULL) {
#endif /* sun */
      return ((SR_MAPLST *)NULL);
   }

   /* Clear the allocated block */
#if (defined(sun) || defined(SVR5))
#elif LINUX
   bzero(mlp,memsz);
#else
   bzero(mlp,ctob(btoc(memsz)));
#endif /* sun */

   /* Set up the control structure info */
   mlp->sm_memsz     = memsz;
   mlp->sm_segp      = (SR_SEG *)(mlp + 1);
   mlp->sm_esegp     = mlp->sm_segp;
   mlp->sm_rlbasep   = (SR_RAMLST *)(mlp->sm_segp + 2 * nbds + 1);
   mlp->sm_rlstartp  = NULL;

   return (mlp);
}


/***********************************************************************
 *        NAME: sr_mapadd
 * DESCRIPTION: Adds a board to the SR_RAMLST for the calling PM.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 0 on success, -1 on failure.
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sr_mapadd(mlp,bdp,srp,ovrflag)
SR_MAPLST   *mlp;
GN_BOARD    *bdp;
SR_BDSRAM   *srp;
int         ovrflag;
{
   ULONG numbds;
   UCHAR *memaddr;
   UCHAR *segg;


#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0004)
      cmn_err(CE_NOTE,"sr_mapadd(0x%x, 0x%x, 0x%x, %d)",
	    mlp, bdp, srp, ovrflag);
#endif /* DLGN_DEBUG */

   memaddr = srp->br_physaddr;
   numbds  = srp->br_totmem / SH_RAM_SIZE;


   /* Total size must be a multiple of shared RAM size */
   if ((srp->br_totmem % SH_RAM_SIZE) != 0) {
      return (-1);
   }

   /* Must have 1 - 4 boards */
   if ((numbds < 1) || (numbds > 4)) {
      return (-1);
   }

#ifdef VME_SPAN 
   STRACE2("sr_mapadd: memaddr=0x%x, br_totmem=0x%x\n", 
      memaddr, srp->br_totmem);
   srp->br_memlen = srp->br_totmem;
   if (rladd(mlp,memaddr,srp->br_memlen,bdp,srp,0,ovrflag) == -1) {
      return (-1);
   }
#else
   if (seg(memaddr) != seg(memaddr + srp->br_totmem - 1)) {
      srp->br_memlen = 0x10000 - offset(memaddr);
      if (rladd(mlp,memaddr,srp->br_memlen,bdp,srp,0,ovrflag) == -1) {
         return (-1);
      }

      srp->br_memlen2 = offset(memaddr + srp->br_totmem);
#ifdef LFS
      segg = (UCHAR *) (ulong) (seg(memaddr) == 0xD0000 ? 0xA0000 : 0xD0000);
#else
      segg = (UCHAR *) (seg(memaddr) == 0xD0000 ? 0xA0000 : 0xD0000);
#endif
      if (rladd(mlp,segg,srp->br_memlen2,bdp,srp,1,ovrflag) == -1) {
         return (-1);
      }

   } else {
      srp->br_memlen = srp->br_totmem;
      if (rladd(mlp,memaddr,srp->br_memlen,bdp,srp,0,ovrflag) == -1) {
         return (-1);
      }
   }
#endif /* VME_SPAN */

   return (0);
}


/***********************************************************************
 *        NAME: sr_map2mem
 * DESCRIPTION: Performs mapping on list built up by the calling PM.
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 0 for success or -1 for failure.
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sr_map2mem(mlp)
SR_MAPLST   *mlp;
{
   SR_BDSRAM *srp;
   SR_RAMLST *rlp;
   SR_SEG    *segp;

#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x0004)
      cmn_err(CE_NOTE,"sr_map2mem(0x%x)", mlp);
#endif /* DLGN_DEBUG */

   /* Build the segment tables and map the memory to kernel addresses */
   if (buildsegs(mlp) != 0) {
      return (-1);
   }

   /*
    * Go through the sorted linked list of SR_RAMLST entries and compute
    * the mapped virtual address for each entry and save it.
    */
   for (rlp = mlp->sm_rlstartp; rlp != NULL; rlp = rlp->rl_next) {

      srp = rlp->rl_srp;

      if (rlp->rl_part == 0) {
         segp = mlp->sm_segp + srp->br_segid;
         srp->br_sramaddr = segp->sg_virt + srp->br_offset;

         STRACE3("sr_map2mem: PART 0 virt=0x%lx, offset=0x%lx, sramaddr=0x%lx", segp->sg_virt, srp->br_offset, srp->br_sramaddr);
      } else {
         segp = mlp->sm_segp + srp->br_segid2;
         srp->br_sramaddr2 = segp->sg_virt + srp->br_offset2;

         STRACE3("sr_map2mem: PART 1 virt=0x%lx, offset2=0x%lx, sramaddr=0x%lx", segp->sg_virt, srp->br_offset2, srp->br_sramaddr2);

      }
   }

   return (0);
}


/***********************************************************************
 *        NAME: sr_unmap
 * DESCRIPTION: 
 *      INPUTS: 
 *     OUTPUTS: 
 *     RETURNS: 0 on success, -1 on failure
 *       CALLS: 
 *    CAUTIONS: 
 **********************************************************************/
int sr_unmap(mlp)
SR_MAPLST   *mlp;
{
   /* Unmap and free the memory if ptr is legit */
   if (mlp) {
      unmapsegs(mlp);
#if (defined(sun) || defined(SVR5))
      kmem_free((void *)mlp, mlp->sm_memsz);
#elif LINUX
#ifdef LFS
      kmem_free((void *)mlp, mlp->sm_memsz);
#else
      lis_kfree((void *)mlp);
#endif
#else
      unkseg((char *)mlp);
#endif /* sun */
      return (0);
   }
   return (-1);
}


/***********************************************************************
 *        NAME: rladd
 * DESCRIPTION: Creates an SR_RAMLST entry and insetrs it at the proper
 *              point in the linked list of SR_RAMLST entries for the 
 *              givem SR_MAPLST. The linkes list is sorted in order of
 *              ascending memory addresses.
 *      INPUTS: mlp - ptr to SR_MAPLST control structure
 *              addr - start physical address
 *              size - continous size in bytes
 *              bp - pointer to board structure associated with memory
 *              part - which half for boards that have 2 discontinuous
 *                ranges of memory
 *              ovrflag - overlap flag: set to SR_NOSHARESEG to check
 *                the entry for overlap with other segments; set to
 *                SR_SHARESEG to disable overlap checking.
 *     OUTPUTS: new entry inserted in linked list if no overlap error.
 *     RETURNS: 0 if no overlap detected; -1 if overlap detected.
 *       CALLS: overlap()
 *    CAUTIONS: none
 ***********************************************************************/
PRIVATE int rladd(mlp, addr, size, bdp, srp, part, ovrflag)
SR_MAPLST   *mlp;
UCHAR       *addr;
ULONG       size;
GN_BOARD    *bdp;
SR_BDSRAM   *srp;
ULONG       part;
int         ovrflag;
{
   register SR_RAMLST *rlp;
   register SR_RAMLST *hdp = ((SR_RAMLST *) &(mlp->sm_rlstartp));
   register SR_RAMLST *el;

   /*
    * First save the new entry into an SR_RAMLST structure
    */
   el = mlp->sm_rlbasep + mlp->sm_numents;
   mlp->sm_numents++;

   el->rl_next = NULL;
   el->rl_phys = addr;
   el->rl_size = size;
   el->rl_bdp  = bdp;
   el->rl_srp  = srp;
   el->rl_part = part;


   /*
    * Now go through the linked list of SR_RAMLST structures and
    * insert the new entry into the list.
    */
   for (rlp = hdp; rlp != NULL; rlp = rlp->rl_next) {
      if (((rlp == hdp) || (el->rl_phys >= rlp->rl_phys)) && 
          ((rlp->rl_next == NULL) || 
          (el->rl_phys <= rlp->rl_next->rl_phys))) {
         break;
      }
   }

   /* Check lower overlap if not at list head */
   if ((ovrflag == SR_NOSHARESEG) && (rlp != hdp)) {
      if (overlap(rlp, el)) {
         cmn_err(CE_WARN,"rladd: memory overlap on %s and %s",
             rlp->rl_bdp->bd_name, el->rl_bdp->bd_name);
         cmn_err(CE_WARN,"rladd: board %s will be ignored",
             el->rl_bdp->bd_name);
         return (-1);
      }
   }

   /* And upper overlap if rlp->rl_next is not NULL */
   if ((ovrflag == SR_NOSHARESEG) && (rlp->rl_next != NULL)) {
      if (overlap(el, rlp->rl_next)) {
         cmn_err(CE_WARN,"rladd: memory overlap on %s and %s",
             el->rl_bdp->bd_name, rlp->rl_bdp->bd_name);
         cmn_err(CE_WARN,"rladd: board %s will be ignored",
             el->rl_bdp->bd_name);
         return (-1);
      }
   }

   /* Insert at rlp */
   el->rl_next = rlp->rl_next;
   rlp->rl_next = el;

   return 0;
}


/***********************************************************************
 *        NAME: overlap
 * DESCRIPTION: Compares 2 SR_RAMLST pointers to check for memory
 *              overlap. Called by rladd().
 *      INPUTS: lp - lower SR_RAMLST pointer
 *              up - upper SR_RAMLST pointer
 *     OUTPUTS: Generates console message in case of overlap.
 *     RETURNS: 0 = success; 1 = overlap.
 *       CALLS: cmn_err()
 *    CAUTIONS: none
 **********************************************************************/
PRIVATE int overlap(lp, up)
SR_RAMLST *lp;
SR_RAMLST *up;
{
   /*
    * If BOTH SR_RAMLST entries have a valid IO port, then the
    * boards are using FBAM and may overlap.
    */
/* #ifdef sun  */
#ifdef VME_SPAN /*  */
#else
   if ((lp->rl_srp->br_ioport != DX_NOPORT) &&
       (up->rl_srp->br_ioport != DX_NOPORT)) {
      return (0);
   }
#endif /* VME_SPAN */
/* #endif sun  */

   /* Check for overlap */
   if (lp->rl_phys + lp->rl_size > up->rl_phys) {
      return (1);
   }

   return (0);
}


/**********************************************************************
 *        NAME: buildsegs
 * DESCRIPTION: This function builds the SR_SEG table based on the
 *              linked list of SR_RAMLST entries. A segment is a
 *              contiguous range of physical memory which will be
 *              subsequently mapped into the kernel address space
 *      INPUTS: mlp - ptr to SR_MAPLST control structure
 *     OUTPUTS: SR_SEG table built and SR_BDSRAM structure updated.
 *     RETURNS: 0 = success; -1 - fail
 *       CALLS: none
 *    CAUTIONS: none
 **********************************************************************/
#ifdef Linux_BuildSegs
PRIVATE int buildsegs(mlp)
SR_MAPLST   *mlp;
{
   int rc               = 0;
   int PFlag            = 0;
   int index            = 0;
   int newseg           = 1;
   ULONG pci_cfgaddr    = 0;
   ULONG pci_cfgsize    = 0x80;
   register SR_RAMLST   *rlp;
   register SR_BDSRAM   *srp;
   register SR_SEG      *segp  = mlp->sm_segp;
   ULONG                cfg_addr;

   /* Linux_BuildSegs: Macros defined in dpci.h */
   
   /*
    * Go through the sorted linked list of SR_RAMLST entries and
    * build an array of SR_SEG structures having one SR_SEG structure
    * for each contiguous segment of memory.
    * If this is the start of a new segment, then just save
    * the starting address and the length. If not, recompute the
    * length of the segment based upon the location and size of
    * new SR_RAMLST entry.
    * If the end of the new SR_RAMLST entry goes beyond the
    * current end of the segment, then expand the segment size
    * so that it can contain the entire new entry.
    */
   for(rlp = mlp->sm_rlstartp; rlp != NULL; rlp = rlp->rl_next) {
       if (newseg) {
           newseg = 0;
           segp->sg_phys = rlp->rl_phys;
           segp->sg_size = rlp->rl_size;
       } else if((rlp->rl_phys + rlp->rl_size) > (segp->sg_phys + segp->sg_size)) {
           segp->sg_size = rlp->rl_phys - segp->sg_phys + rlp->rl_size;
       }
 
       srp = rlp->rl_srp;
 
       /*
        * Save the SR_SEG id (an array index) and the offset so that
        * we will later be able to calculate the virtual address
        * corresponding to the physical address given by this SR_RAMLST
        * entry.
        */
       if(rlp->rl_part == 0) {
          srp->br_segid = segp - mlp->sm_segp;
          srp->br_offset = rlp->rl_phys - segp->sg_phys;
       } else {
          srp->br_segid2 = segp - mlp->sm_segp;
          srp->br_offset2 = rlp->rl_phys - segp->sg_phys;
       }
 
       /*
        * If we are at the last element, or we are crossing segment
        * boundaries, or have a hole, then save the current segment
        * and start another.
        */
       if(CHK_PHY_ADDR((int)segp->sg_phys, 0xfff00000)) {
          for(index =0; index <= NumOfVoiceBoards; index++) {
              if((CHK_BRD_PHY_ADDR((int)segp->sg_phys, 0xffffe000, index)) && PCI_2K(index)) {
                 newseg = 1;
                 mlp->sm_esegp++;
                 segp++;
                 PFlag++;
                 break;
              }
          }
       }
 
       if(!PFlag) {
          if(rlp->rl_next == NULL ||
            ((segp->sg_phys + segp->sg_size) < rlp->rl_next->rl_phys) ||
            ((segp->sg_phys + 0x10000) == rlp->rl_next->rl_phys)) {
             newseg = 1;
             mlp->sm_esegp++;
             segp++;
          }
       }
       PFlag = 0;
   }
 
 
   /* Map each segment into kernel space */
   for (segp = mlp->sm_segp; segp < mlp->sm_esegp; segp++) {
       if ((long)segp->sg_phys) {
          /* ISA and PCI Board Map for Linux */
          segp->sg_virt = (UCHAR*)MAP_ADDR((ULONG)segp->sg_phys, segp->sg_size);
          STRACE3("buildsegs: sg_phys=0x%lx, size=0x%lx ==>virt=0x%lx\n",
                                segp->sg_phys, segp->sg_size, segp->sg_virt);
 
          if (segp->sg_virt == NULL) {
              cmn_err(CE_WARN, "srmap: cannot map 0x%x bytes at segment 0x%x\n",
                                                segp->sg_size, segp->sg_phys);
             rc = -1;
          } else if ((long)segp->sg_phys & 0xfff00000) {
             for(index = 0; index <= NumOfVoiceBoards; index++) {
                if (segp->sg_phys == GET_PHY_ADDR(index) )
                    SET_VIRT_ADDR(index, segp->sg_virt);
             }
          }
       }
   } /* for each segment */

   for(index = 0; index <= NumOfVoiceBoards; index++) {
     cfg_addr= MAP_ADDR(GET_CFG_PHY_ADDR(index), 0x80);
     PciIntEnb((UCHAR*)cfg_addr, index);
     SET_CFG_VIRT(index, cfg_addr) ;
   }
 
   return rc;
}

#else
#ifdef LFS
int PciIntEnb(char *, int);
#endif
PRIVATE int buildsegs(mlp)
SR_MAPLST   *mlp;
{
   register SR_SEG      *segp  = mlp->sm_segp;
   register SR_RAMLST   *rlp;
   register SR_BDSRAM   *srp;
   int newseg  = 1;
   int rc      = 0;
   int PFlag = 0;
#ifdef PCI_SPAN
#ifndef LFS
   int reg;
#endif
   int index = 0;
#ifndef LFS
   unsigned long pci_cfgaddr = 0;
   ULONG pci_cfgsize = 0x80;
#endif
#endif /* PCI_SPAN */

#ifdef VME_SPAN 
   unsigned int npages, pfn;
   unsigned long a, cvaddr, kva;
#endif /* VME_SPAN */

#ifdef DDI8
   rm_key_t rmkey = 0;
   size_t psize;
   USHORT segid;
#endif /* DDI8 */

#ifdef DLGN_DEBUG
   if (dlgn_debug & 0x000c)
      cmn_err(CE_NOTE,"buildsegs(0x%x)", mlp);
#endif /* DLGN_DEBUG */

   /*
    * Go through the sorted linked list of SR_RAMLST entries and
    * build an array of SR_SEG structures having one SR_SEG structure
    * for each contiguous segment of memory.
    */
   for (rlp = mlp->sm_rlstartp; rlp != NULL; rlp = rlp->rl_next) {
      /*
       * If this is the start of a new segment, then just save
       * the starting address and the length. If not, recompute the 
       * length of the segment based upon the location and size of
       * new SR_RAMLST entry.
       */
      if (newseg) {
         newseg = 0;
         segp->sg_phys = rlp->rl_phys;
         segp->sg_size = rlp->rl_size;
      } else {
         /*
          * If the end of the new SR_RAMLST entry goes beyond the 
          * current end of the segment, then expand the segment size
          * so that it can contain the entire new entry.
          */
         if ((rlp->rl_phys + rlp->rl_size) >
             (segp->sg_phys + segp->sg_size)) {
            segp->sg_size = rlp->rl_phys - segp->sg_phys + rlp->rl_size;
         }
      }

      srp = rlp->rl_srp;

      /*
       * Save the SR_SEG id (an array index) and the offset so that
       * we will later be able to calculate the virtual address
       * corresponding to the physical address given by this SR_RAMLST
       * entry.
       */
      if (rlp->rl_part == 0) {
         srp->br_segid = segp - mlp->sm_segp;
         srp->br_offset = rlp->rl_phys - segp->sg_phys;
      } else {
         srp->br_segid2 = segp - mlp->sm_segp;
         srp->br_offset2 = rlp->rl_phys - segp->sg_phys;
      }

      /*
       * If we are at the last element, or we are crossing segment
       * boundaries, or have a hole, then save the current segment
       * and start another.
       */
     
#if (defined(SVR5) || defined(LINUX))
#ifdef LFS
   if (((int)(long)segp->sg_phys & 0xfff00000) != 0) 
#else
   if (((int)segp->sg_phys & 0xfff00000) != 0) 
#endif
   {
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
#ifdef LFS
          if ((((int)(long)segp->sg_phys & 0xffffe000) == 
	      GpControlBlock->GpVirtMapBlock[index].PhysAddr) && 
	      ((GpControlBlock->BoardAttrs[index].BoardType == PCI_MSI_ID)  ||
 	      (GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID)||
//  *for CSP* (GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID)||
 	      (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID)))
#else
          if ((((int)segp->sg_phys & 0xffffe000) == 
	      GpControlBlock->GpVirtMapBlock[index].PhysAddr) && 
	      ((GpControlBlock->BoardAttrs[index].BoardType == PCI_MSI_ID)  ||
 	      (GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID)||
//  *for CSP* (GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID)||
 	      (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID)))
#endif
	  {
              newseg = 1;
              mlp->sm_esegp++;
              segp++;
	      PFlag++;
              break;
          }
      }   
   } 

   if (!PFlag)
#endif /* SVR5 */
     {
      if (rlp->rl_next == NULL || 
          ((segp->sg_phys + segp->sg_size) < rlp->rl_next->rl_phys) || 
          ((segp->sg_phys + 0x10000 ) == rlp->rl_next->rl_phys) ) {
         newseg = 1;
         mlp->sm_esegp++;
         segp++;
      }
     }
     PFlag = 0;
   }


   /* Map each segment into kernel space */
   for (segp = mlp->sm_segp; segp < mlp->sm_esegp; segp++) {
#ifdef VME_SPAN
      segp->sg_size = sr_make_sram_multiple(segp->sg_size);
#endif /* VME_SPAN */

if ((long)segp->sg_phys) {
#ifdef sun
#ifdef PCI_SPAN
#ifdef SPARC_PCI
    for (index =0; index <= NumOfVoiceBoards; index++)
    {
     if (((long)segp->sg_phys)== GpControlBlock->GpVirtMapBlock[index].VirtAddr)
        { 
          segp->sg_virt = (UCHAR *)GpControlBlock->GpVirtMapBlock[index].VirtAddr;
          segp->sg_cfgvirt = (UCHAR *)GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr; 
   	  PciIntEnb(segp->sg_cfgvirt, index);
          break;
        }
      }    
#else
   if ((int)segp->sg_phys & 0xfff00000) 
   {
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
         if (((int)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr)
          break;
      }   
   }

   segp->sg_virt = (UCHAR *)dlgn_map_regs((int)segp->sg_phys, segp->sg_size,
                                index, GP_PCI_MEM_PROP_REG);
   segp->sg_cfgvirt = (UCHAR *)dlgn_map_regs(pci_cfgaddr, pci_cfgsize,
                                index, GP_PCI_PLX_PROP_REG);
   GpControlBlock->GpVirtMapBlock[index].VirtAddr =(ULONG)segp->sg_virt ;
   GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr =(ULONG)segp->sg_cfgvirt ;
#endif /* SPARC_PCI */

#elif ISA_SPAN 
   segp->sg_virt = (UCHAR *)dlgn_map_regs((int)segp->sg_phys, segp->sg_size,0,0);
#else
   segp->sg_virt = (UCHAR *)sr_map_regs((int)segp->sg_phys, segp->sg_size);
#endif /* PCI_SPAN */

    /*
     * ioremap doesn't really map ISA addresses but it does check
     * for valid ISA addresses.  There is no need to free the address,
     * later on.
     */
#else 

#ifdef SVR3
 if ((segp->sg_virt = (UCHAR *)sptalloc(btoc(segp->sg_size),
            PG_P | PG_LOCK, btoc(segp->sg_phys), SE_NOSLP)) == NULL)

#else
#ifdef LFS
 if (!((int)(long)segp->sg_phys & 0xfff00000)) {  /* Check for ISA */
#else
 if (!((int)segp->sg_phys & 0xfff00000)) {  /* Check for ISA */
#endif
#ifdef DDI8
    segp->sg_virt = (UCHAR *)dlgn_physmap(segp->sg_phys, segp->sg_size);
#elif LINUX
#ifdef LFS
    segp->sg_virt = (UCHAR *)ioremap_nocache((ULONG) segp->sg_phys, segp->sg_size);    
#else
    segp->sg_virt = (UCHAR *)lis_ioremap_nocache((ULONG) segp->sg_phys, segp->sg_size);    
#endif
#else 
    segp->sg_virt = (UCHAR *)physmap(segp->sg_phys, segp->sg_size, KM_NOSLEEP);
#endif  /* DDI8  */
} else {
#if defined (DDI8) || defined(LINUX)
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
#if 0	/* some address pass both cases - removing first case */
          if ((((int)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) || (((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr)){
#else
#ifdef LFS
          if (((int)(long)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr){
#else
          if (((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr){
#endif
#endif
           if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID)|| 
               (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID)|| 
//  *for CSP*  (GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID)|| 
               (GpControlBlock->BoardAttrs[index].BoardType == PCI_MSI_ID) )  { 
#ifdef DDI8
           if (GpControlBlock->BoardAttrs[index].BoardType==PCI_D41JCT_ID ) {
                 segp->sg_virt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 0, (size_t) 0, 0x2000);
	   } else {
                 segp->sg_virt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 1, (size_t) 0, 0x2000);
		 GpControlBlock->GpVirtMapBlock[index].VirtAddr=(ULONG)segp->sg_virt;
          }
#else
#ifdef LFS
                 segp->sg_virt = (UCHAR *)ioremap_nocache((ULONG) GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x2000);
#else
                 segp->sg_virt = (UCHAR *)lis_ioremap_nocache((ULONG) GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x2000);
#endif
		 GpControlBlock->GpVirtMapBlock[index].VirtAddr=(ULONG)segp->sg_virt;
#endif /* DDI8 */
             } else if (GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID){ 
#ifdef DDI8
                 segp->sg_virt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 1, (size_t) 0, 0x4000);
#else
		 /* for CSP changed from 0x2000 to 0x4000 */
#ifdef LFS
	         segp->sg_virt = (UCHAR *)ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x4000);
#else
	         segp->sg_virt = (UCHAR *)lis_ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x4000);
#endif
#endif /* DDI8 */
             } else if (GpControlBlock->BoardAttrs[index].BoardType == PCI_D82U_ID){ 
#ifdef DDI8
                 segp->sg_virt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 1, (size_t) 0, 0x8000);
#else
		 /* for CSP changed from 0x4000 to 0x8000 */
#ifdef LFS
	         segp->sg_virt = (UCHAR *)ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x8000);
#else
	         segp->sg_virt = (UCHAR *)lis_ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, 0x8000);
#endif
#endif /* DDI8 */
             } else {
#ifdef DDI8
                 segp->sg_virt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 1, (size_t) 0, segp->sg_size);
#else
#ifdef LFS
                 segp->sg_virt = (UCHAR *)ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, segp->sg_size);
#else
                 segp->sg_virt = (UCHAR *)lis_ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr, segp->sg_size);
#endif
#endif /* DDI8 */
             }
      }
   }
}
#endif /* DDI8 */
#endif /* SVR3 */
   if (segp->sg_virt == NULL) {
#ifdef LFS
      cmn_err(CE_WARN, "buildsegs: cannot map 0x%lx bytes at segment: 0x%p",
            				segp->sg_size, segp->sg_phys);
#else
      dlgn_msg(CE_WARN, "buildsegs: cannot map 0x%x bytes at segment: 0x%x",
            				segp->sg_size, segp->sg_phys);
#endif
      rc = -1;
   }
#endif /* sun */
}

#if defined (PCI_SPAN) && !defined(sun)
#ifdef LFS
 if ((int)(long)segp->sg_phys & 0xfff00000)
#else
 if ((int)segp->sg_phys & 0xfff00000)
#endif
 {  
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
#if 0	/* some address pass both cases - removing first case */
          if ( ((((int)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) || 
	       (((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr)) &&  
#else
#ifdef LFS
	  if (((((int)(long)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr)) &&  
#else
	  if (((((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr)) &&  
#endif
#endif
	      (((GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID) &&
	       (GpControlBlock->BoardAttrs[index].BoardType != PCI_COYOTE_ID) &&
	       (GpControlBlock->BoardAttrs[index].BoardType != PCI_D41JCT_ID))||
              ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))))) {
#ifdef DDI8
      if ((segp->sg_cfgvirt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 0, (size_t) 0, 0x80)) == NULL)
#elif LINUX
#ifdef LFS
      if ((segp->sg_cfgvirt = (UCHAR *)ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)    
#else
      if ((segp->sg_cfgvirt = (UCHAR *)lis_ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)    
#endif
#else
      if ((segp->sg_cfgvirt = sram_getvirtmem(GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)
#endif /* DDI8 */
         {
#ifdef LFS
           cmn_err(CE_WARN, "buildsegs: cannot map at segment: 0x%lx",
                            GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr);
#else
           dlgn_msg(CE_WARN, "buildsegs: cannot map at segment: 0x%x",
                            GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr);
#endif
           rc = -1;
           }
   	   PciIntEnb(segp->sg_cfgvirt, index);
	   if (GpControlBlock->BoardAttrs[index].BoardType != PCI_MSI_ID) { 
	       GpControlBlock->GpVirtMapBlock[index].VirtAddr =(ULONG)segp->sg_virt ;
	   }
           GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr =(ULONG)segp->sg_cfgvirt ;
           break;

	 }
      }  

  }
#endif /* PCI_SPAN */

      STRACE3("buildsegs: sg_phys=0x%lx, size=0x%lx ==>virt=0x%lx", segp->sg_phys, segp->sg_size, segp->sg_virt);
   }

#if (defined(PCI_SPAN) && !defined(sun)) 
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
          if (GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID) 
          { 
#ifdef DDI8
      	     if ((segp->sg_cfgvirt = (UCHAR *)devmem_mapin((rm_key_t) GpControlBlock->GpVirtMapBlock[index].RmKey, (uint_t) 0, (size_t) 0, 0x80)) == NULL)
#elif LINUX
#ifdef LFS
             if ((segp->sg_cfgvirt = (UCHAR *)ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)
#else
             if ((segp->sg_cfgvirt = (UCHAR *)lis_ioremap_nocache((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)
#endif

#else
     	     if ((segp->sg_cfgvirt = sram_getvirtmem(GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr, 0x80)) == NULL)
#endif /* DDI8 */
             {
#ifdef LFS
                cmn_err(CE_WARN, "buildsegs: cannot map at segment: 0x%lx",
                            GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr);
#else
                dlgn_msg(CE_WARN, "buildsegs: cannot map at segment: 0x%x",
                            GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr);
#endif
                rc = -1;
             }
   	     PciIntEnb(segp->sg_cfgvirt, index);
   	     GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr =(ULONG)segp->sg_cfgvirt ;
	   } 
      }  
#endif /* PCI_SPAN */

   return rc;
}
#endif

/***********************************************************************
 *        NAME: unmapsegs
 * DESCRIPTION: Unmap the segments described by the segment table
 *      INPUTS: mlp - ptr to SR_MAPLST control structure
 *     OUTPUTS: Segments are unmapped.
 *     RETURNS: none
 *       CALLS: sptfree()
 *    CAUTIONS: 
 **********************************************************************/
#ifdef LFS
int PciIntDis(char *, int);
#endif
PRIVATE void unmapsegs(mlp)
SR_MAPLST *mlp;
{
   register SR_SEG *segp;
#ifdef PCI_SPAN
   int  index = 0;
#ifndef LFS
   int  cfg_size = 0x80;
#endif
#endif /* PCI_SPAN */
#ifndef LFS
   UCHAR *cfgAddr;
#endif

   /* Unmap each segment on the list */
   for (segp = mlp->sm_segp; segp < mlp->sm_esegp; segp++) {

#ifdef sun
#ifdef PCI_SPAN
#ifdef SPARC_PCI

/* Memory remains mapped forever in Solaris ; Mapping is done when the
generic driver loaded  not in protocol module like other drivers */

   if ((int)segp->sg_phys) 
   {
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
         if (((int)segp->sg_phys)==GpControlBlock->GpVirtMapBlock[index].VirtAddr) 
         {
           segp->sg_cfgvirt = (UCHAR *)GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr;
           PciIntDis(GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr, index);
           break;
         }
      }   
   }    
#else
   if ((int)segp->sg_phys & 0xfff00000) 
   {
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
          if  ((((int)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) || (((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) )
          break;
      }
   }

   dlgn_unmap_regs((int)segp->sg_phys, segp->sg_size,
          		(char **)&(segp->sg_virt), index, GP_PCI_MEM_PROP_REG);
   PciIntDis(GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr, index);
   dlgn_unmap_regs(0, 0x80, (char **)&(GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr), index, GP_PCI_PLX_PROP_REG);
#endif /* SPARC_PCI */
#elif ISA_SPAN 
      dlgn_unmap_regs((int)segp->sg_phys, segp->sg_size,
          (char **)&(segp->sg_virt),0,0);
#else
      sr_unmap_regs((int)segp->sg_phys, segp->sg_size);
#endif /* PCI_SPAN */

#else  /* sun */
#ifdef LINUX
#ifdef LFS
      if ((int)(long)segp->sg_phys & 0xfff00000)  /* Only PCI was mapped*/
#else
      if ((int)segp->sg_phys & 0xfff00000)  /* Only PCI was mapped*/
#endif
#ifdef LFS
         (void)vfree(segp->sg_virt);
#else
         (void)lis_vfree(segp->sg_virt);
#endif
#elif SVR3
      (void)sptfree(segp->sg_virt, btoc(segp->sg_size), 0);
#elif DDI8
      devmem_mapout(segp->sg_virt, segp->sg_size);
#else 
      physmap_free(segp->sg_virt, segp->sg_size, 0);
#endif /* LINUX */

#ifdef PCI_SPAN
#ifdef LFS
  if ((int)(long)segp->sg_phys & 0xfff00000) 
#else
  if ((int)segp->sg_phys & 0xfff00000) 
#endif
  {
      for (index =0; index <= PCI_MAX_BOARDS; index++)
      {
#ifdef LFS
          if ( (((int)(long)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) ||
	       ((((int)(long)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) &&  (GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID)) ||
	       ((((int)(long)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) &&  (GpControlBlock->BoardAttrs[index].BoardType != PCI_COYOTE_ID)) ) {
#else
          if ( (((int)segp->sg_phys & 0xffff0000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) ||
	       ((((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) &&  (GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID)) ||
	       ((((int)segp->sg_phys & 0xffffe000) == GpControlBlock->GpVirtMapBlock[index].PhysAddr) &&  (GpControlBlock->BoardAttrs[index].BoardType != PCI_COYOTE_ID)) ) {
#endif

          	PciIntDis(segp->sg_cfgvirt, index);
#ifdef LINUX
        /*  
         * lis_vfree won't accept an address not on a page boundary, but ioremap
         * will! Just duplicate what ioremap did to get to the page
         * boundary.
         */
#ifdef LFS
        (void)vfree((UCHAR *)((unsigned long)segp->sg_cfgvirt & PAGE_MASK));
#else
        (void)lis_vfree((UCHAR *)((unsigned long)segp->sg_cfgvirt & PAGE_MASK));
#endif
#elif SVR3
      	(void)sptfree(segp->sg_cfgvirt, btoc(cfg_size), 0);
#elif DDI8
        devmem_mapout(segp->sg_cfgvirt, cfg_size);
#else 
        physmap_free(segp->sg_cfgvirt, cfg_size, 0);
#endif /* LINUX */
	  	break;
	  }
      }
  }
#endif /* PCI_SPAN */

#endif /* sun */

   }

#ifdef Linux_BuildSegs
   for (index = 0; index <= NumOfVoiceBoards; index++) {   
       cfgAddr= GET_CFG_VIRT_ADDR(index);
       PciIntDis((UCHAR*)cfgAddr, index);
#ifdef LFS
       (void)vfree((UCHAR *)((ULONG)cfgAddr & PAGE_MASK));
#else
       (void)lis_vfree((UCHAR *)((ULONG)cfgAddr & PAGE_MASK));
#endif
   }
#endif
   
#if defined(SVR3) && defined(PCI_SPAN)
      for (index =0; index <= NumOfVoiceBoards; index++)
      {
        if (GpControlBlock->BoardAttrs[index].BoardType == PCI_PYTHON_ID)
        { 
          PciIntDis(GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr, index);
      	  (void)sptfree(GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr, btoc(cfg_size), 0);
	} 
      }  
#endif /* SVR3 */

}

#endif /* SVR4_MOT */

/***********************************************************************
 *        NAME: sr_checkbd
 * DESCRIPTION: Check for the existence of a board's shared ram. This
 *              function checks each SH_RAMSIZEd section of memory
 *              individually.
 *      INPUTS: bdaddr - board start virtual address
 *              memlen - number of bytes of shared RAM
 *     OUTPUTS: Board ram checked.
 *     RETURNS: 0 if none of the memory is present;
 *              1 if some of the memory is present;
 *              2 if all of the memory is present.
 *       CALLS: none.
 *    CAUTIONS: none.
 **********************************************************************/
int sr_checkbd(bdaddr, memlen)
register UCHAR *bdaddr;

#ifndef VME_SPAN
USHORT memlen;
#else
ULONG memlen;
#endif /* VME_SPAN */

{
#ifdef sun /*  */
#ifdef ISA_SPAN /*  */
   volatile UCHAR    *addr;
   volatile UCHAR    *limit;
   volatile UCHAR    first;
   volatile UCHAR    last;
   volatile int      true = 0;
   volatile int      nchecks = 0;
#endif
#else /* sun */
   UCHAR    *addr;
   UCHAR    *limit;
   UCHAR    first;
   UCHAR    last;
   int      true = 0;
   int      nchecks = 0;
#endif /* sun */

   STRACE2("sr_checkbd: bdaddr = 0x%lx, memlen = 0x%x\n", bdaddr, memlen);

   /* Check to see if there's really a board here */
#ifndef VME_SPAN
   for (addr = bdaddr; addr < bdaddr + memlen; addr += SH_RAM_SIZE) {
      limit = addr + SH_RAM_SIZE - 1; /*  removed "- 0x200" */
#else
   /* Skip the VME/CR area.
   */
   for (addr = bdaddr + 0x200; addr < bdaddr + memlen; addr += SH_RAM_SIZE) {
      limit = addr + SH_RAM_SIZE - 1 - 0x200;
#endif /* VME_SPAN */
#ifdef LINUX
      /*
       * The gcc compiler optimzes out the reads, above.  Use
       * the linux macros which take care of the problem.
       */
      first = readb(addr);
      last = readb(limit);

      writeb(0x55, addr);
      writeb(0xAA, limit);
      true += (readb(addr) == 0x55);
      true += (readb(limit) == 0xAA);
      nchecks += 2;

      writeb(0xAA,addr);
      writeb(0x55,limit);
      true += (readb(addr) == 0xAA);
      true += (readb(limit) == 0x55);
      nchecks += 2;

      writeb(first, addr);
      writeb(last, limit);
#else

      first = *addr;
      last = *limit;

      *addr = 0x55;
      *limit = 0xAA;
      true += (*addr == 0x55);
      true += (*limit == 0xAA);
      nchecks += 2;

      *addr = 0xAA;
      *limit = 0x55;
      true += (*addr == 0xAA);
      true += (*limit == 0x55);
      nchecks += 2;

      *addr = first;
      *limit = last;
#endif /* LINUX */
   }

   if (true == nchecks) {
      return 2;
   } else if (true == 0) {
      return (0);
   }

   return (1);
}


/***********************************************************************
 *        NAME: sr_clearbd
 * DESCRIPTION: Clear a board's memory if it exists, and also clear all
 *              interrupt latches in the specified range.
 *      INPUTS: bdaddr - board start virtual address
 *              memlen - number of bytes of shared RAM
 *     OUTPUTS: Board memory cleared and interrupt line reset
 *     RETURNS: 0 if board present; -1 if not.
 *       CALLS: sr_checkbd()
 *    CAUTIONS: none.
 **********************************************************************/
int sr_clearbd(bdaddr, memlen)
register UCHAR *bdaddr;

#ifndef VME_SPAN
USHORT memlen;
#else
ULONG memlen;
#endif /* VME_SPAN */

{
   register UCHAR *addr;
#ifdef sun /*  */
#ifdef ISA_SPAN /*  */
   volatile UCHAR dummy;
#endif /* ISA_SPAN */ /* TOCM */
#else  /* sun */
#ifndef LFS
   UCHAR dummy;
#endif
#endif /* sun */

   /* Check to see if there's really a board here */
   if (sr_checkbd(bdaddr, memlen) != 2) {
      return (-1);
   }

   for (addr = bdaddr; addr < bdaddr + memlen; addr += SH_RAM_SIZE) {

      STRACE2("sr_clearbd: addr=0x%lx, memlen=0x%lx\n", addr, memlen);
#ifdef LINUX
      readb(&(((D4XSRAM *)addr)->dl_msgrdy));    /* Clear IRQ line */
#else
      dummy = ((D4XSRAM *)addr)->dl_msgrdy;    /* Clear IRQ line */
#endif /* LINUX */
      /*
       * Zero out the shared RAM area except for the byte at offset:
       * D4XE_LOCATOR. Writing to that location could cause a D4XE to
       * map itself out.
       */
#ifndef VME_SPAN
#ifndef BRI_SUPPORT /* Eliminate clearing 0xD0 */
      bzero((char *)addr, D4XE_LOCATOR);
#endif /* BRI_SUPPORT */
/*      bzero((char *)addr + D4XE_LOCATOR + 1, ENDRAM - D4XE_LOCATOR - 1);*/
      bzero((char *)addr + 0x100, ENDRAM - 0x100);
#else
      /*
      ** DON'T zero the configuration/status area which is replicated
      ** even in the lower 32K of the virtual board which is made up
      ** of (2 * 32K) = 64K.
      */
      /*
      ** Doing this on the first virtual board will panic the system 
      ** bacause from 0 to 0xBF are the VME/CR only which is read-only
      ** at every 4th byte.
      **
      ** bzero(addr, D4XE_LOCATOR);
      */
      sr_bzero((char *)(addr + 0x100), 0x7A40 - 0x100); 
      sr_bzero((char *)(addr + 0x8000), 0x7E80 - 1); 
#endif /* VME_SPAN */
   }

   return (0);
}

#ifdef sun
#ifdef VME_SPAN /*  */

typedef struct span_sram {
   unsigned long physaddr_start; /* VMEbus addr */
   unsigned long virtaddr_start; /* virtual addr */
   unsigned long size;           /* size of the largest map;
                                 ** minimum is the entire SPAN.
                                 */
   unsigned long reference;      /* reference count */

   unsigned long npages;         /* for 5CE */
   unsigned long cvaddr;         /* for 5CE */
   unsigned long pageaddr;       /* for 5CE */
} SPAN_SRAM_T;

/*
** The following structure is used to maintain the VMEbus address mapping
** into the kernel virtual address.  If a VMEbus address is already mapped,
** its kernel virtual address will be re-used.  This table is created
** to work around the limitation of the FORCE Solaris 2.4 ddi_map_regs()
** and ddi_unmap_regs().  Apparently, if one does this:
**
**   ddi_map_regs: VMEbus=0x180000, size=0x40000 (1)
**   ddi_map_regs: VMEbus=0x180000, size=0x20000 (2)
** then:
**   ddi_map_regs: VMEbus=0x180000, size=0x20000 
**   ddi_map_regs: VMEbus=0x180000, size=0x40000 
**
** The unmappping of the 20000 bytes is treated as a partial unmapping
** of the 0x40000 mapping.  According to the man page, partial unmapping
** is not allowed.  However, this is not really the partial unmapping
** of the 0x40000 mapping; it is the unmapping of the 0x20000.  The
** ddi_unmap_regs() probably does not track how many mappings to a
** particular VMEbus address.  What is even more confusing in the man 
** page is that there is no explicit statement against multiple mapping
** of the same VMEbus address.
*/
SPAN_SRAM_T SPAN_SRAM[GN_MAXSLOTS];

sr_span_sram_init()
{
   int i;
   SPAN_SRAM_T *ssramp; 

   for (i = 0, ssramp = &SPAN_SRAM[0]; i < GN_MAXSLOTS; i++, ssramp++) {
      ssramp->physaddr_start = 0;
      ssramp->virtaddr_start = 0;
      ssramp->size = 0;
      ssramp->reference = 0;
      ssramp->npages = 0;
      ssramp->cvaddr = 0;
      ssramp->pageaddr = 0;
   }
}

char *
sr_map_regs(int physaddr, int size)
{
   /* Traverse the SPAN_SRAM mapping table.  If the address
   ** is already mapped, use it and increment the reference
   ** count.  If the reference count is 0, call dlgn_map_regs.
   */

   SPAN_SRAM_T *ssramp;
   int i, first_unused_entry;
   unsigned int npages, pfn;
   unsigned long a, cvaddr, kva;
   unsigned long virtaddr_start, physaddr_start, physaddr_end;
   
   if (sr_map_init == 0) {
      sr_map_init = 1;
      sr_span_sram_init();
   }

   physaddr &= 0xFFFFFF;   /* enforce A24 */

   STRACE2("sr_map_regs: physaddr=0x%x, size=0x%x\n", physaddr, size);

   for (i = 0, ssramp = &SPAN_SRAM[0]; i < GN_MAXSLOTS; i++, ssramp++) {

      if (ssramp->size == 0) {
         first_unused_entry = i;
         continue;
      }

      physaddr_start = ssramp->physaddr_start;
      physaddr_end = ssramp->physaddr_start + ssramp->size;

      if (physaddr >= physaddr_start && physaddr <= physaddr_end) {

         if (physaddr_start + size > physaddr_end) {
            cmn_err(CE_NOTE, "sr_map_regs: size 0x%x to map exceeds the last mapped byte 0x%x. (starting addr=0x%x)\n", size, physaddr_end, physaddr_start);
         }

         ssramp->reference++;

         STRACE3("sr_map_regs: physaddr=0x%x, virt=0x%x, ref=0x%x\n",
            physaddr, ssramp->virtaddr_start, ssramp->reference);

         return((char *)(ssramp->virtaddr_start + 
                     (physaddr - ssramp->physaddr_start)));

      }
   }

   ssramp = &SPAN_SRAM[first_unused_entry];

   /* If we get here, it means no mapping existed.  A new
   ** mapping is needed.
   */
   if (dlgn_FORCEvme_5v) {

      if ((virtaddr_start = (int)dlgn_map_regs(((int)physaddr & 0xffffff), size)) 
            == NULL) {
         STRACE2("buildsegs: failed mapping vmeaddr=0x%x, size=0x%x",
            physaddr, size);
         return NULL;
      }

      STRACE3("buildsegs: vmeaddr=0x%x, size=0x%x, kva=0x%x\n", 
         physaddr, size, virtaddr_start);

   }
   else {

      npages = mmu_btopr(size);

      pfn = btop(SBUS_VME_WIN + 
         (((unsigned long)physaddr | VME24_BASE) & 0xfffffff));

      a = rmalloc(kernelmap, (long)npages);
      if (a == 0) {
         cmn_err(CE_WARN, "buildsegs: out of kernelmap resources");
         return(NULL);
      }

      cvaddr = (unsigned long)kmxtob(a);

      dlgn_flush_PTE();

      segkmem_mapin(&kvseg, (caddr_t)cvaddr, (unsigned int)mmu_ptob(npages),
         PROT_READ | PROT_WRITE, pfn, 0);

      virtaddr_start = 
         (int)(cvaddr + (((unsigned long)physaddr | VME24_BASE)
            & MMU_PAGEOFFSET));

      STRACE2(
         "buildsegs: vmeaddr=0x%x, size=0x%x\n", physaddr, size);
      STRACE2(
         "buildsegs: npages=0x%x, pfn=0x%x\n", npages, pfn);
      STRACE3("buildsegs: a=0x%x, cvaddr=0x%x, kva=0x%x\n",
         a, cvaddr, virtaddr_start);
   }

   ssramp->size = size;
   ssramp->reference++;
   ssramp->physaddr_start = physaddr;
   ssramp->virtaddr_start = virtaddr_start;

   ssramp->npages = npages;
   ssramp->pageaddr = a;
   ssramp->cvaddr = cvaddr;

   STRACE3("sr_map_regs: physaddr=0x%x, virt=0x%x, ref=0x%x\n",
      physaddr, ssramp->virtaddr_start, ssramp->reference);

   return((char *)ssramp->virtaddr_start); 
}

int
sr_unmap_regs(int physaddr, int size)
{
   /* Traverse the SPAN_SRAM mapping table.  If the address
   ** is mapped, decrement the reference count.  If the reference
   ** count reaches 0, call dlgn_unmap_regs.  Upon unmapping,
   ** clear the entries in the SPAN_SRAM array;
   */

   SPAN_SRAM_T *ssramp;
   unsigned int i, npages, pfn;
   unsigned long a, cvaddr, kva;
   unsigned long virtaddr_start, physaddr_start, physaddr_end;

   physaddr &= 0xFFFFFF;

   STRACE2("sr_unmap_regs: physaddr=0x%x, size=0x%x\n", physaddr, size);

   if (sr_map_init == 0) {
      cmn_err(CE_CONT, "sr_unmap_regs: map table not initialized\n");
      return;
   }

   for (i = 0, ssramp = &SPAN_SRAM[0]; i < GN_MAXSLOTS; i++, ssramp++) {

      if (ssramp->size == 0) {
         continue;
      }

      physaddr_start = ssramp->physaddr_start;
      physaddr_end = ssramp->physaddr_start + ssramp->size;


      if (physaddr >= physaddr_start && physaddr <= physaddr_end) {

         if (physaddr_start + size > physaddr_end) {
            cmn_err(CE_NOTE, 
               "sr_unmap_regs: size to unmap exceeds mapped size\n");
         }

         ssramp->reference--;

         STRACE3("sr_unmap_regs: physaddr=0x%x, virt=0x%x, ref=0x%x\n",
            physaddr, ssramp->virtaddr_start, ssramp->reference);

         break;
      }
   }

   if (i == GN_MAXSLOTS) {
      STRACE0("sr_unmap_regs: nothing to unmap.\n");
      return;
   }

   /* Unmap the address if the reference count is 0
   */
   if (ssramp->reference == 0) {

      if (dlgn_FORCEvme_5v) {

         STRACE3("unmapsegs: vmeaddr=0x%x, size=0x%x, kva=0x%x\n", 
            ssramp->physaddr_start, ssramp->size, ssramp->virtaddr_start);

         dlgn_unmap_regs(((int)ssramp->physaddr_start & 0xffffff), ssramp->size,
            (char **)&(ssramp->virtaddr_start)); 

      }
      else {

         STRACE3("unmapsegs: pageaddr=0x%x, npages=0x%x, kva=0x%x\n",
            ssramp->pageaddr, ssramp->npages, ssramp->virtaddr_start);

         segkmem_mapout(&kvseg, (caddr_t)ssramp->cvaddr,
            (unsigned int)mmu_ptob(ssramp->npages));
         rmfree(kernelmap, ssramp->npages, ssramp->pageaddr);

      }

      ssramp->physaddr_start = 0;
      ssramp->virtaddr_start = 0;
      ssramp->size = 0;
      ssramp->reference = 0;
      ssramp->npages = 0;
      ssramp->pageaddr = 0;
      ssramp->cvaddr = 0;
   }
}

#endif /* VME_SPAN */
#endif /* sun */

#if (defined(PCI_SPAN) && defined(SVR3))
/***************************************************************************
 *        NAME: PRIVATE UCHAR *sram_getvirtmem(dip)
 * DESCRIPTION: This function maps the physical address to the virtual
 *            : address.
 *      INPUTS: paddr - Physical Address.
 *            : size  - Amount of physical memory.
 *     OUTPUTS: Nothing.
 *     RETURNS: Virtual address or NULL, if map fails.
 *       CALLS: sptalloc() or physmap()
 *    CAUTIONS: None.
 ***************************************************************************/
#ifndef SVR3
UCHAR *sram_getvirtmem(ULONG paddr, ULONG size)
#else
UCHAR *sram_getvirtmem(      paddr,       size)
   ULONG paddr;
   ULONG size;
#endif
{
   UCHAR *vaddr;
   ULONG sram_offset;

#ifdef SVR3
   /*
    * Save the offset of the address before calculating the segment.
    * The virtual address will be obtained at the segment boundary
    * since sptalloc operates at page boundaries (4K).  We will add
    * the offset value to size to obtain memory upto the last possible
    * address to be used.
    */
   sram_offset = paddr & 0x00001FFF;
 
   paddr &= 0xFFFFE000;
   size += sram_offset;
   if ((vaddr = (UCHAR *)sptalloc(btoc(size), PG_P | PG_LOCK, btoc(paddr),
                                                            SE_NOSLP)) == NULL)
#else
   /*
    * Get virtual memory of required size starting at the given physical address
    */
   if ((vaddr = (UCHAR *)physmap(paddr, size, KM_NOSLEEP)) == NULL)
#endif
   {
      cmn_err(CE_WARN, "GPIO: Cannot map 0x%x bytes at segment 0x%x",
                                                                  size, paddr);
   }
#ifdef SVR3
   vaddr += sram_offset;
#endif
 
   return vaddr;
}
#endif /* SVR3 */

/***********************************************************************
 *        NAME: PciIntEnb()
 * DESCRIPTION: Write to PCI board's Plx interrupt control/status reg
 *              to enable PCI interrupts.
 *      INPUTS: Virtual Config Address
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS:
 * CALLED FROM:
 *  LOGIC FLOW:
 *              - read plx interrupt control/status reg
 *              - write plx interrupt control/status reg or'ing enb bits
 *    CAUTIONS:
 **********************************************************************/
int
PciIntEnb(PlxVirtAddr, index)
char    *PlxVirtAddr;
int 	index;
{
        char *IntrRegAddr, *FfRegAddr;
        char IntrReg, MaskReg;
        unsigned long IntrReg1;
#ifndef LFS
	UCHAR *cfgAddr;
#endif

	if (PlxVirtAddr == 0)
           return (0);

        if (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID) {
	   if (Single_Board_SS) {
               /* New Enable Interrupt for Coyote Boards */
#ifdef Linux_BuildSegs
               cfgAddr = (UCHAR*)GET_CFG_VIRT_ADDR(index);
               cfgAddr += 0x4C;
#ifdef LFS
               cfgAddr = (UCHAR *)(long)0x1443;
#else
              (long)cfgAddr = 0x1443;
#endif
#else
	       PlxVirtAddr += 0x4C;
#ifdef LFS
	       PlxVirtAddr = (char *)(long)0x1443;
#else
	       (long)PlxVirtAddr = 0x1443;
#endif
#endif
	   }
	   return (0);
	}
	
 	if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
            IntrRegAddr = PlxVirtAddr + PCIe_PLX_INTR_REG;
	} else {
            IntrRegAddr = PlxVirtAddr + PCI_PLX_INTR_REG;
	}
	
        if (IS_SPANPLUS_TYPE(GpControlBlock->BoardAttrs[index].BoardType)) {
           IntrReg1 = *(volatile unsigned long *)IntrRegAddr;
	   IntrReg1 = IntrReg1 & PLX_DISABLE; 
           *(volatile unsigned long *)IntrRegAddr = IntrReg1 ; 
           IntrReg1 = *(volatile unsigned long *)IntrRegAddr;
	   IntrReg1 = IntrReg1 | PLX_CLEAR; 
           *(volatile unsigned long *)IntrRegAddr = IntrReg1 ; 
           IntrReg1 = *(volatile unsigned long *)IntrRegAddr;
	   IntrReg1 = IntrReg1 | PLX_ENABLE; 
           *(volatile unsigned long *)IntrRegAddr = IntrReg1 ; 

        } else {
	    // For D41JCT PCIe boards, need to enable interrupts differently
	    if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
		IntrReg1 = PCIe_PLX_INTR_ENABLE_BIT;
		*(volatile unsigned long *)IntrRegAddr = IntrReg1;
	    } else {
                IntrReg = *IntrRegAddr;

                /* Setting bit7 to 1 panics the system = 0x81 */
               if (!IntrReg) {
                   IntrReg |= PCI_PLX_INTR_ENABLE_BIT ;
	       } else {
                   IntrReg = 0x41;
               }

               *IntrRegAddr = IntrReg;
	    }

#if 0
        cmn_err(CE_NOTE, "PlxVirtAddr = 0x%x BoardType = 0x%x\n", PlxVirtAddr, GpControlBlock->BoardAttrs[index].BoardType );
#endif 

	if (GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID) {
	        if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
        	    FfRegAddr = PlxVirtAddr + PCIe_PLX_FF_REG;
	        } else {
        	    FfRegAddr = PlxVirtAddr + PCI_PLX_FF_REG;
		}
        	MaskReg = *(volatile ULONG * const)FfRegAddr;
        	if (!MaskReg) {
                   *FfRegAddr = BYTERESETFLIPFLOP;
        	} else {
                   *FfRegAddr = MaskReg & 0xFB;
                }
 
        	if (!MaskReg) {
                   *FfRegAddr = BYTESETFLIPFLOP;
                } else {
                   *FfRegAddr = MaskReg | 0x04;
                }
        }
     }
     return(0);
}

/***********************************************************************
 *        NAME: PciIntDis
 * DESCRIPTION: Write to PCI board's Plx interrupt control/status reg
 *              to disable PCI interrupts.
 *      INPUTS:
 *     OUTPUTS: none
 *     RETURNS: none
 *       CALLS:
 * CALLED FROM:
 *  LOGIC FLOW:
 *      - read plx interrupt control/status reg
 *      - write plx interrupt control/status reg and'ing disable bits
 *    CAUTIONS:
 **********************************************************************/
int
PciIntDis(PlxVirtAddr, index)
char    *PlxVirtAddr;
int 	index;
{
 
        char *IntrRegAddr, *FfRegAddr;
        char IntrReg, MaskReg, DisReg;
        unsigned long IntrReg1;
#ifndef LFS
	UCHAR *cfgAddr;
#endif
 
	if (PlxVirtAddr == 0)
           return (0);

        if (GpControlBlock->BoardAttrs[index].BoardType == PCI_COYOTE_ID) {
	   if (Single_Board_SS) {
               /* New Disable Interrupt for Coyote Boards */
#ifdef Linux_BuildSegs
               cfgAddr = (UCHAR*)GET_CFG_VIRT_ADDR(index);
               cfgAddr += 0x4C;
               *cfgAddr &= 0xffbe;
#else
	       PlxVirtAddr += 0x4C;
	       *PlxVirtAddr &= 0xffbe;
#endif
	   }
	   return (0);
	}

 	if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
            IntrRegAddr = PlxVirtAddr + PCIe_PLX_INTR_REG;
	} else {
            IntrRegAddr = PlxVirtAddr + PCI_PLX_INTR_REG;
	}

        if (IS_SPANPLUS_TYPE(GpControlBlock->BoardAttrs[index].BoardType))
        {
 
           IntrReg1 = *(volatile unsigned long *)IntrRegAddr;
           IntrReg1 = IntrReg1 & PLX_DISABLE;
           *(volatile unsigned long *)IntrRegAddr = IntrReg1 ;

        } else {

        IntrReg = *IntrRegAddr;

	if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
            DisReg = 0;
	} else {
            DisReg = 0x40;
	}

        if (!IntrReg) {
                *IntrRegAddr = DisReg;
        } else {
                *IntrRegAddr = IntrReg & DisReg;
        }

	if (GpControlBlock->BoardAttrs[index].BoardType != PCI_PYTHON_ID) {
	        if ((GpControlBlock->BoardAttrs[index].BoardType == PCI_D41JCT_ID) && (GET_TYPE_PCIe(index))) {
        	    FfRegAddr = PlxVirtAddr + PCIe_PLX_FF_REG;
		} else {
        	    FfRegAddr = PlxVirtAddr + PCI_PLX_FF_REG;
		}
        	MaskReg = *(volatile ULONG * const)FfRegAddr;
        
		if (!MaskReg) {
                   *FfRegAddr = BYTERESETFLIPFLOP;
        	} else {
                   *FfRegAddr = MaskReg & 0xFB;
        	}
	}
    }
    return(0);
}

