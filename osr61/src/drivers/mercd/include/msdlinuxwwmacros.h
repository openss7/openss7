/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdlinuxwwmacros.h
 * Description                  : WW macro definitions
 *
 *
 **********************************************************************/

#ifndef _MSDLINUXWWMACROS_H
#define _MSDLINUXWWMACROS_H

//Macros For WW support

////////////////////////////////////////////////////////////////////////////
//Macros for Plx 9054
////////////////////////////////////////////////////////////////////////////

#define MsdWWPutinBoundQReg(padapter,val) \
            writel(val, padapter->pww_info->inBoundQRegAddress);

#define MsdWWGetinBoundQReg(padapter) \
            readl(padapter->pww_info->inBoundQRegAddress);

#define MsdWWPutoutBoundQReg(padapter,val) \
            writel(val, padapter->pww_info->outBoundQRegAddress);

#define MsdWWGetoutBoundQReg(padapter) \
            readl(padapter->pww_info->outBoundQRegAddress);

#define MsdWWPlxGetRemapRegister(padapter) \
            readl((((pmercd_virt_map_sT)(padapter->phw_info->\
                    virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+\
                    MERCD_WW_PLX_REMAP_REG));

#define MsdWWPlxPutRemapRegister(padapter, val) \
            writel(val, (((pmercd_virt_map_sT)(padapter->phw_info->\
                         virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+\
                         MERCD_WW_PLX_REMAP_REG));

#ifdef LFS
#define MsdWWPlxGenerateDoorBell(padapter, val) \
          writel(val, (((pmercd_virt_map_sT)(padapter->phw_info->\
		        virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+\
			MERCD_WW_PLX_H2B_INTR_REG));
#else
#define MsdWWPlxGenerateDoorBell(padapter, val) \
          loc = (volatile pmerc_ulong_t)(((pmercd_virt_map_sT)(padapter->phw_info->\
                virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MERCD_WW_PLX_H2B_INTR_REG); \
          writel(val, loc);
#endif

#define MsdWWPlxGetIntrReg(IntrReg){ \
        if (padapter->pww_info->state == MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING) \
          { \
	    IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                      virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MD_PCI_VENID_LOCATION));\
	    if ((IntrReg == MD_PCI_DLGCID) || (IntrReg == MD_PCI_NEWBID))\
		IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                      virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MSD_PCI_PLX_INTR_REG)); \
            else {\
                IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                      virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MD_PCI_VENID_LOCATION));\
		if ((IntrReg == MD_PCI_DLGCID) || (IntrReg == MD_PCI_NEWBID))\
	             IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                        virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MSD_PCI_PLX_INTR_REG)); \
                else { \
                    IntrReg = 0; \
                    cmn_err(CE_WARN, "Invaild BAR0 and BAR3.\n");\
                }\
            }\
          }  else { \
            if (padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr) {\
                IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                      virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MSD_PCI_PLX_INTR_REG)); \
            } else { \
		IntrReg = 0; \
                cmn_err(CE_WARN, "MsdWWPlxGetIntrReg: null virtual address\n");\
            }\
          }\
        }

#define MsdWWPlxEnableBigEndian(Value)                \
         writel(Value, padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->\
                       virt_addr+MERCD_WW_PLX_BIGEND_REG);

#define MsdWWPlxGetIntrRequest(reason) { \
         if (padapter->pww_info->state == MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING) \
         {  \
           reason = readl((((pmercd_virt_map_sT)(padapter->phw_info->\
		    virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MERCD_WW_PLX_B2H_INTR_REG)); \
         } else { \
           reason = readl((((pmercd_virt_map_sT)(padapter->phw_info->\
                    virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MERCD_WW_PLX_B2H_INTR_REG)); \
         } \
      }

#define MsdWWPlxClearIntrRequest(reason) { \
     if (padapter->pww_info->state == MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING) {   \
        writel(reason, ((((pmercd_virt_map_sT)(padapter->phw_info->\
             virt_ww_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr)+MERCD_WW_PLX_B2H_INTR_REG)); \
         } else { \
        writel(reason, ((((pmercd_virt_map_sT)\
                        (padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr)+\
                        MERCD_WW_PLX_B2H_INTR_REG)); \
         } \
       }

//////////////////////////////////////////////////////////////////////////
//Macros for 21554
//Reading a non-zero from MSD_PCI_21554_B2H_INTR_REG tells it is our
//interrupt. If we read a zero, then it is not from our board. To clear
//the interrupt, we write back what we just read to
//MSD_PCI_21554_B2H_INTR_CLEAR_REG
//////////////////////////////////////////////////////////////////////////
#define MsdWW21554GetIntrReg(IntrReg) \
   IntrReg = readl((((pmercd_virt_map_sT)( padapter->phw_info->\
                     virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+\
                     MSD_PCI_21554_B2H_INTR_REG));

#define MsdWW21554ClearIntrRequest(reason) \
    writew(reason, (((pmercd_virt_map_sT)(padapter->phw_info->\
                   virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+\
                   MSD_PCI_21554_B2H_INTR_CLEAR_REG));

#define MsdWW21554GenerateDoorBell(padapter, val) \
    writew(val, (caddr_t)(((size_t)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr))+\
                         MSD_PCI_21554_H2B_INTR_REG));

//Reset Register is in the PCI config space
#define MsdWW21554WriteToResetReg(data) \
          pci_write_config_dword(Dip, MSD_PCI_21554_SOFT_RESET_REG, data)

//////////////////////////////////////////////////////////////////////////////////
//Macros for 80321
//We read the MSD_PCI_80321_B2H_INTR_CS_REG, and if the 2nd
//bit is set, then it is ours. We read the interrupt type from
//MSD_PCI_80321_B2H_INTR_REG. We write back what we just read to
//MSD_PCI_80321_B2H_INTR_REG to clear the interrupt.
//////////////////////////////////////////////////////////////////////////////////
#define MsdWW80321GetIntrReg(IntrReg) \
            IntrReg = *((volatile pmerc_ulong_t)(((pmercd_virt_map_sT)( padapter->phw_info->\
            virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr+MSD_PCI_80321_B2H_INTR_CS_REG));

#define MsdWW80321GetIntrRequest(reason) \
         reason = *((pmerc_ulong_t)(((pmercd_virt_map_sT)(padapter->phw_info->\
         virt_map_q[MERCD_PCI_BRIDGE_MAP]))->\
         virt_addr+MSD_PCI_80321_B2H_INTR_REG));

#define MsdWW80321ClearIntrRequest(reason)  \
         *((pmerc_uint_t)(((size_t)((pmercd_virt_map_sT)\
         (padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr)+\
         MSD_PCI_80321_B2H_INTR_REG)) = reason;

#define MsdWW80321GenerateDoorBell(padapter, val) \
          *((pmerc_uint_t)(((size_t)(padapter->phw_info->\
          virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr))+\
	  MSD_PCI_80321_H2B_INTR_REG)) = val;

#endif //_MSDLINUXWWMACROS_H
