/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwutl.c
 * Description                  : PLX specific Messaging and Utilities
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDWWUTL_C_
#include "msdextern.h"
#undef _MSDWWUTL_C_

/*
 * Local variables
 */

#ifndef LFS
static int i_bperline = 16;
#endif
#define RMARGIN 80
#define BPERLINE 16
#undef  isprint
#define isprint(C)      (((C) >= 0x20) && ((C) < 0x7F))
#ifdef LFS
const char i_bprintf_Id[] = "$Id: i_bprintf.c,v 1.8 1996/12/12 05:52:06 xxxxx Exp $";
#else
static char i_bprintf_Id[] = "$Id: i_bprintf.c,v 1.8 1996/12/12 05:52:06 xxxxx Exp $";
#endif

/******************************************************************************
 * Function Name                : msdwwutl_ww_read_free_inboundQ_for_mf_address
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : get the index of the next free MF from
 *                                free in-bound Q FIFO
 * Additional comments          :
 ****************************************************************************/
int msdwwutl_ww_read_free_inboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST)
{

   MD_STATUS Status;
   merc_ulong_t index;
#ifndef LFS
   merc_ulong_t mfAddress;
#endif
   pmercd_ww_dev_info_sT pwwDev;
   pmercd_adapter_block_sT padapter;

   MSD_FUNCTION_TRACE("msdwwutl_ww_read_free_inboundQ_for_mf_address 0x%x %s #%d",
                       (size_t) pmfAddressST, __FILE__, __LINE__);

   Status = MD_SUCCESS;
   pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddressST->pwwDevi));

   padapter = (pmercd_adapter_block_sT)(pwwDev->padapter);

   //Reading the in-bound Q register gives the index of the next free MF
   MSD_ENTER_MUTEX(&pwwDev->ww_iboundQ_mutex);
   index = MsdWWGetinBoundQReg(padapter);

   //Update the Perf counters
   //atomic_inc(&pwwDev->pww_counters->total_inboundq_reads);

   if (index == 0xFFFFFFFF)
    {
     MSD_LEVEL2_DBGPRINT(
           "msdwwutl_ww_read_free_inboundQ_for_mf_address: No MFAs! index: 0x%x\n %s #%d",
                        index, __FILE__, __LINE__
                         );
     pmfAddressST->pmfAddress = NULL;
     pmfAddressST->mfIndex = 0xffffffff;
     Status = MD_FAILURE;
     //atomic_inc(&pwwDev->pww_counters->nomf_inboundq_reads);
     MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);
     return(Status);
    }

   //Mask out the higher order bits
      index = index & 0x0000FFFF;

   //check whether the index is greater then the max num of MF for this queue
   if ( index > pwwDev->pww_param->numberMFAInboundQ)
    {
    cmn_err(CE_CONT, "index > max MFAs!!!\n");
    MSD_LEVEL2_DBGPRINT(
        "msdwwutl_ww_read_free_inboundQ_for_mf_address: index: %x max MFAs: %x %s #%d\n!", 
                   index, pwwDev->pww_param->numberMFAInboundQ, __FILE__, __LINE__
                   );
    pmfAddressST->pmfAddress = NULL;
    pmfAddressST->mfIndex = index;
    //atomic_inc(&pwwDev->pww_counters->outofrange_inboundq_reads);
    Status = MD_FAILURE;
    MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);
    return(Status);
    }


   //atomic_inc(&pwwDev->pww_counters->valid_inboundq_reads);
   pmfAddressST->mfIndex = index;


  if (pwwDev->inBoundQMfAddress) {
#ifdef LFS
     pmfAddressST->pmfAddress = (pmerc_uchar_t)((pwwDev->inBoundQMfAddress) +
                              (index * pwwDev->pww_param->sizeofMFAInboundQ));
#else
     pmfAddressST->pmfAddress = (pmerc_ulong_t)((pwwDev->inBoundQMfAddress) +
                              (index * pwwDev->pww_param->sizeofMFAInboundQ));
#endif

     MSD_ZERO_MEMORY(pmfAddressST->pmfAddress, pwwDev->pww_param->sizeofMFAInboundQ);
   }

   MSD_EXIT_MUTEX(&pwwDev->ww_iboundQ_mutex);

return (Status);
}


/*********************************************************************************
 * Function Name                : msdwwutl_ww_read_posted_outboundQ_for_mf_address
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : get the index of the next free MF from
 *                                free in-bound Q FIFO
 * Additional comments          :
 *********************************************************************************/
int msdwwutl_ww_read_posted_outboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST)
{

   MD_STATUS Status;
   merc_ulong_t index = 0;
#ifndef LFS
   merc_ulong_t mfAddress;
#endif
   pmercd_ww_dev_info_sT pwwDev;
   pmercd_adapter_block_sT padapter;

   MSD_FUNCTION_TRACE("msdwwutl_ww_read_posted_outboundQ_for_mf_address 0x%x %s #%d\n", 
                       (size_t) pmfAddressST, __FILE__, __LINE__);

   Status = MD_SUCCESS;
   pwwDev = ((pmercd_ww_dev_info_sT)(pmfAddressST->pwwDevi));
   padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

   //Reading the out-bound Q register gives the index of the next free MF
   MSD_ENTER_MUTEX(&pwwDev->ww_oboundQ_mutex);
   index = MsdWWGetoutBoundQReg(padapter);


   if (index == 0xFFFFFFFF)
    {
     MSD_LEVEL2_DBGPRINT(
       "msdwwutl_ww_read_posted_outboundQ_for_mf_address: No MFAs! index: 0x%x %s #%d\n",
                    index, __FILE__, __LINE__
                     );
       pmfAddressST->pmfAddress = NULL;
       pmfAddressST->mfIndex = index;
       MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);
       return(MD_FAILURE);
    }

   //Mask out the higher order bits
      index = index & 0x0000FFFF;


   //check whether the index is greater then the max num of MF for this queue
   if ( index >= pwwDev->pww_param->numberMFAOutboundQ)
    {
       cmn_err(CE_CONT, "msdwwutl_ww_read_posted_outboundQ_for_mf_address index > max MFAs %s #%d\n", __FILE__, __LINE__);
       MSD_LEVEL2_DBGPRINT(
    "msdwwutl_ww_read_posted_outboundQ_for_mf_address: index: 0x%x, maxMFAs: 0x%x %s #%d!\n", 
           index, pwwDev->pww_param->numberMFAOutboundQ, 
           __FILE__, __LINE__
                          );
       pmfAddressST->pmfAddress = NULL;
       pmfAddressST->mfIndex = index;
       //atomic_inc(&pwwDev->pww_counters->outofrange_outboundq_reads);
       MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);
       return(MD_FAILURE);
    }

   //Update the Perf counters
   //atomic_inc(&pwwDev->pww_counters->total_outboundq_reads);

   pmfAddressST->mfIndex = index;


   if (pwwDev->outBoundQMfAddress) {
#ifdef LFS
      pmfAddressST->pmfAddress = (pmerc_uchar_t)((pwwDev->outBoundQMfAddress) +
                                 (index * pwwDev->pww_param->sizeofMFAOutboundQ));
#else
      pmfAddressST->pmfAddress = (pmerc_ulong_t)((pwwDev->outBoundQMfAddress) +
                                 (index * pwwDev->pww_param->sizeofMFAOutboundQ));
#endif
    }

   MSD_EXIT_MUTEX(&pwwDev->ww_oboundQ_mutex);
return (Status);
}

/***************************************************************************
 * Function Name                : msgutl_ww_get_nextfree_dmadescr
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                Gets Next Free Snd BIG Msg DMA descriptor
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_get_nextfree_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt)
{
  merc_uint_t count, iBit;
  merc_uint_t useMask = 0x00000001;
  merc_uint_t numMasks;
  merc_uint_t found = 0;
  merc_uint_t bitPos = 0;
  merc_uint_t maxBits = 32;
  MD_STATUS Status=MD_SUCCESS;
  pmerc_uint_t bitmap = (pmerc_uint_t)pdmaDscrSt->pBitMap;

  MSD_FUNCTION_TRACE("msgutl_ww_get_nextfree_dmadescr 0x%x %s #%d",
               (size_t) pdmaDscrSt, __FILE__, __LINE__);

  if (!bitmap)
     return(MD_FAILURE);

  numMasks = ((pdmaDscrSt->szBitMap)/(8 * sizeof(merc_uint_t)));

  if ((pdmaDscrSt->szBitMap)%(8 * sizeof(merc_uint_t)))
     numMasks += 1;

  for (count = 0; count <= numMasks-1; count ++)
  {

    maxBits = msgutl_ww_get_numofbits_in_current_mask(bitPos, pdmaDscrSt->szBitMap);

     for (iBit = 0; iBit < maxBits; iBit++)
      {
         bitPos = (count * sizeof(merc_uint_t) * 8)+(iBit);

         if ( !( *bitmap & useMask) )
         {
             pdmaDscrSt->index = bitPos;
             found = 1;
             break;
         }
         else
         {
            useMask <<= 1;
         }
      } /* for iBit */
     if (found)
       {
        Status = MD_SUCCESS;
        break;
       }
     else
        {
        useMask = 0x00000001;
        bitmap += 1;
        }
  } /* for count */

 if (!found)
    Status = MD_FAILURE;


return ( Status );

}


/************************************************************************************
 * Function Name                : msgutl_ww_get_numofbits_in_current_mask
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                              : Gets the the active number of bits in the current
 *                              : 32 bit mask.
 * Additional comments          :
 *
 *************************************************************************************/
merc_uint_t msgutl_ww_get_numofbits_in_current_mask(merc_uint_t bitpos, merc_uint_t bmapsz)
{
 merc_uint_t maxBits;

  MSD_FUNCTION_TRACE("msgutl_ww_get_numofbits_in_current_mask 0x%x %s #%d\n",
               (size_t) bmapsz, __FILE__, __LINE__);

    if ( (bitpos+32) > ((bmapsz) - 1))
     {
       // Change me
       if (bmapsz == MERCD_WW_MAX_MFS_PER_STREAM) {
           maxBits = bmapsz;
       } else {
           maxBits  = ((bmapsz-1) - (bitpos));
       }
     }
    else
     {
       maxBits = 32;
     }

 return (maxBits);
}

/***************************************************************************
 * Function Name                : msgutl_ww_mark_dmadescr_free
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                Gets Next Free Snd BIG Msg DMA descriptor
 * Additional comments          :
 *
 ****************************************************************************/
merc_uint_t msgutl_ww_mark_dmadescr_free(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt)
{
  pmerc_uint_t bitmap;
  merc_uint_t numMask;
  merc_uint_t tobemarked;
  merc_uint_t bit_offset;
  MD_STATUS Status;
  merc_uint_t usedMask = 0x00000001;

 MSD_FUNCTION_TRACE("msgutl_ww_mark_dmadescr_free 0x%x %s #%d\n",
               (size_t) pdmaDscrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  bitmap = ((pmerc_uint_t)(pdmaDscrSt->pBitMap));
  if (!bitmap)
     return(MD_FAILURE);
  tobemarked = pdmaDscrSt->index;

  if ((tobemarked) > (pdmaDscrSt->szBitMap))
     {
       MSD_LEVEL2_DBGPRINT(
          "msgutl_ww_mark_dmadescr_free: tobemarked: %d bitmap size: %d %s #%d\n",
           tobemarked,
           pdmaDscrSt->szBitMap, __FILE__, __LINE__
              );
       Status = MD_FAILURE;
       return (Status);
     }

  numMask = ((tobemarked)/(8 * sizeof(merc_uint_t)));
  bit_offset = (tobemarked - ((numMask) * (8 * sizeof(int))));
  usedMask <<= (bit_offset);
  bitmap += (numMask);
  *bitmap &= ~(usedMask);

return(Status);

}


/******************************************************************************
 * Function Name                : msgutl_ww_get_mark_dmadescr_used
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                Gets Next Free Snd BIG Msg DMA descriptor
 * Additional comments          :
 *
 *******************************************************************************/
merc_uint_t msgutl_ww_mark_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt)
{

  pmerc_uint_t bitmap;
  merc_uint_t numMask;
#ifndef LFS
  merc_uint_t found = 0;
#endif
  merc_uint_t tobemarked;
  merc_uint_t bit_offset;
  MD_STATUS Status;
  merc_uint_t usedMask = 0x00000001;

  MSD_FUNCTION_TRACE("msgutl_ww_mark_dmadescr_used 0x%x %s #%d\n",
               (size_t) pdmaDscrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  bitmap = (pmerc_uint_t)pdmaDscrSt->pBitMap;

  if (!bitmap)
     return(MD_FAILURE);

  tobemarked = pdmaDscrSt->index;
  if ((tobemarked) > (pdmaDscrSt->szBitMap))
    {
       MSD_LEVEL2_DBGPRINT(
                 "msgutl_ww_mark_dmadescr_used: tobemarked: %d bitmap size: %d %s #%d\n",
                  tobemarked,
                  pdmaDscrSt->szBitMap, __FILE__, __LINE__
                  );
       Status = MD_FAILURE;
       return (Status);
    }
  numMask = ((tobemarked)/(8 * sizeof(merc_uint_t)));
  bit_offset = (tobemarked - ((numMask) * (8 * sizeof(int))));
  usedMask <<= (bit_offset);
  bitmap += (numMask);

  *bitmap |= (usedMask);

return(Status);

}

/******************************************************************************
 * Function Name                : msgutl_ww_check_if_dmadescr_used
 * Function Type                : Support Function for Stream Manager
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Streams Driver Routine.
 *                                Gets the Status of Snd BIG Msg DMA descriptor
 *                                Returns SUCCESS if it is used. Else FAILURE.
 * Additional comments          :
 *
 *******************************************************************************/
merc_uint_t
msgutl_ww_check_if_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt)
{

  pmerc_uint_t bitmap;
  merc_uint_t numMask;
#ifndef LFS
  merc_uint_t found = 0;
#endif
  merc_uint_t tobemarked;
  merc_uint_t bit_offset;
  MD_STATUS Status;
  merc_uint_t usedMask = 0x00000001;

  MSD_FUNCTION_TRACE("msgutl_ww_check_if_dmadescr_used 0x%x %s #%d\n",
               (size_t) pdmaDscrSt, __FILE__, __LINE__);

  Status = MD_SUCCESS;
  bitmap = (pmerc_uint_t)pdmaDscrSt->pBitMap;

  if (!bitmap)
   return(MD_FAILURE);


  tobemarked = pdmaDscrSt->index;

  if ((tobemarked) > (pdmaDscrSt->szBitMap))
    {
       MSD_LEVEL2_DBGPRINT(
                 "msgutl_ww_check_if_dmadescr_used: tobemarked: %d bitmap size: %d %s #%d\n",
                  tobemarked,
                  pdmaDscrSt->szBitMap, __FILE__, __LINE__
                  );
       Status = MD_FAILURE;
       return (Status);
    }
  numMask = ((tobemarked)/(8 * sizeof(merc_uint_t)));
  bit_offset = (tobemarked - ((numMask) * (8 * sizeof(int))));
  usedMask <<= (bit_offset);
  bitmap += (numMask);

  if ((*bitmap) & (usedMask))
  {
    Status = MD_SUCCESS;
  }
  else
  {
    Status = MD_FAILURE;

  }

return(Status);
}



merc_uint_t msgutl_ww_check_free_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt)
{
  merc_uint_t count, iBit;
  merc_uint_t useMask = 0x00000001;
  merc_uint_t numMasks;
  merc_uint_t found = 0;
  merc_uint_t bitPos = 0;
  merc_uint_t maxBits = 32;
  MD_STATUS Status=MD_SUCCESS;
  pmerc_uint_t bitmap = (pmerc_uint_t)pdmaDscrSt->pBitMap;

  MSD_FUNCTION_TRACE("msgutl_ww_get_nextfree_dmadescr 0x%x %s #%d",
               (size_t) pdmaDscrSt, __FILE__, __LINE__);

  if (!bitmap)
     return(MD_FAILURE);

  numMasks = ((pdmaDscrSt->szBitMap)/(8 * sizeof(merc_uint_t)));

  if ((pdmaDscrSt->szBitMap)%(8 * sizeof(merc_uint_t)))
     numMasks += 1;

  for (count = 0; count <= numMasks-1; count ++)
  {

    maxBits = msgutl_ww_get_numofbits_in_current_mask(bitPos, pdmaDscrSt->szBitMap);

     for (iBit = 0; iBit < maxBits; iBit++)
      {
         bitPos = (count * sizeof(merc_uint_t) * 8)+(iBit);

         if ( !( *bitmap & useMask) )
         {
             found = 1;
             break;
         }
         else
         {
            useMask <<= 1;
         }
      } /* for iBit */
     if (found)
       {
        Status = MD_SUCCESS;
        break;
       }
     else
        {
        useMask = 0x00000001;
        bitmap += 1;
        }
  } /* for count */

 if (!found)
    Status = MD_FAILURE;


return ( Status );

}
