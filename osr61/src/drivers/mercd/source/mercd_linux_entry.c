/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_linux_entry.c
 * Description                  : Driver Entry Points Module
 *
 *
 **********************************************************************/

#ifndef LINUX24
#include <linux/moduleparam.h>
#endif
#ifdef LFS
#include <sys/kmem.h>
#endif

#include <linux/module.h>
#include <linux/version.h>
#include "msd.h"
#include "mercd.h"

#define MERCD_LINUX_ENTRY
#include "msdextern.h"
#undef MERCD_LINUX_ENTRY

static int mercdMajor = 0; /* Till 2.4.2 there is minor no. limit - 256 */

#ifdef LFS
static char *mercd_string       = "mercd";
#else
static char mercd_string[]       = "mercd";
#endif

static int dm3_feature = 2; /* Enable additional features at command line */
int NewCanTakeProtocol = 0; /* Optimizes CanTake protocol in play */
int No_Send_From_DPC = 0;   /* Do not send messages from DPC */
int ReceiveDMA = 0;
int SendDMA = 0;
int SendExitNotify = 0;
int NonDefaultId = 0;
int DPC_Disabled = 0;
int RH_Enabled = 0;
int HCS_Flag = 1;

MODULE_AUTHOR("Intel Corp. (c) 2005");
MODULE_DESCRIPTION("Intel Mercury Driver");
#ifdef LINUX24
MODULE_PARM(dm3_feature,"i");
#else
module_param(dm3_feature,int,0444);
module_param(mercd_string,charp,0444);
MODULE_PARM_DESC(mercd_string, "Mercd driver name");
MODULE_LICENSE("GPL v2");
#endif
#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("mercd");
MODULE_ALIAS("streams-mercd");
MODULE_ALIAS("streams-mercdDriver");
#endif
#endif
static int mercd_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void mercd_release(struct pci_dev *pdev);
int mercd_resume(struct pci_dev *pdev);
#ifdef LFS
#ifndef HAVE_KTYPE_PM_MESSAGE_T
typedef u32 pm_message_t;
#endif
#ifdef LINUX24
int mercd_suspend(struct pci_dev *pdev, u32 state);
#else
int mercd_suspend(struct pci_dev *pdev, pm_message_t state);
#endif
#else
int mercd_suspend(struct pci_dev *pdev);
#endif

//========================================================================
//
// Set the low/high water marks to allow only one message to be queued
// for the write-side service routine at a time.
//
//========================================================================
#define LO_WATER        1
#define HI_WATER        (48*4*1024)
#define MIN_PSZ         0
#define MAX_PSZ         (16*4*1024)
#define MSD_ID          401

#ifndef LFS
static struct file_operations mercd_fops =
{
        .owner   =    THIS_MODULE,
        .read    =    mercd_read,
        .write   =    mercd_write,
        .poll    =    mercd_poll,
        .open    =    mercd_open,
        .release =    mercd_close,
        .ioctl   =    mercd_ioctl,
        .mmap    =    mercd_mmap
};
#else
static struct module_info mercd_minfo = {
	.mi_idnum = 0,
	.mi_idname = "mercd",
	.mi_hiwat = HI_WATER,
	.mi_lowat = LO_WATER,
	.mi_minpsz = MIN_PSZ,
	.mi_maxpsz = MAX_PSZ,
};
static struct qinit mercdrinit = {
	.qi_srvp = mercd_rsrv,
	.qi_qopen = mercd_open,
	.qi_qclose = mercd_close,
	.qi_minfo = &mercd_minfo,
};
static struct qinit mercdwinit = {
	.qi_putp = mercd_writeput,
	.qi_minfo = &mercd_minfo,
};
static struct streamtab mercd_fops = {
	.st_rdinit = &mercdrinit,
	.st_wrinit = &mercdwinit,
};
#define NATIVE_MAX_POOL_SIZE     3000
#define NATIVE_SEQ_POOL_SIZE     100 
#define NATIVE_MAX_BANK_SIZE     1000
#endif

/*************************************************************************
*
*      Global Allocation
*
*************************************************************************/
pmercd_control_block_sT MsdControlBlock = NULL;

PMSD_MUTEX_STATS pmutex_stats = NULL;


static struct pci_device_id mercd_pci_tbl[] __devinitdata = {
        { PCI_VENDOR_ID_DLGC, PCI_DEVICE_ID_DLGC_906D,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DM3, },
        { PCI_VENDOR_ID_DLGC, PCI_DEVICE_ID_DLGC_9054,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DM3, },
        { PCI_VENDOR_ID_DLGC_NEW, PCI_DEVICE_ID_DLGC_0046,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DTI16, },
        { PCI_VENDOR_ID_DLGC_NEW, PCI_DEVICE_ID_DLGC_530D,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_ROZETTA, },
        { PCI_VENDOR_ID_DLGC_NEW, PCI_DEVICE_ID_DLGC_0046,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_ROZETTA, },
        { PCI_VENDOR_ID_DLGC_21555, PCI_DEVICE_ID_DLGC_21555,
                PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DTI16, },
	{ PCI_SUBVENDOR_ID_DLGC, PCI_DEVICE_ID_DLGC_4143,
		PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DM3, },
	{ PCI_VENDOR_ID_DLGC,PCI_DEVICE_ID_DLGC_9054,
		PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DMVB, },
        { PCI_SUBVENDOR_ID_DLGC, PCI_DEVICE_ID_DLGC_5356,
                 PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_DISI, },
        { PCI_SUBVENDOR_ID_DLGC, PCI_DEVICE_ID_DLGC_5356,
                 PCI_SUBVENDOR_ID_DLGC, PCI_SUBDEVICE_ID_NEWB, },
        { 0,}
};

MODULE_DEVICE_TABLE(pci, mercd_pci_tbl);

extern int ctimod_check_rh(void);

static struct pci_driver mercd_driver = {
#ifdef LFS
        .name     =       "mercd",
#else
        .name     =       mercd_string,
#endif
        .id_table =       mercd_pci_tbl,
        .probe    =       mercd_probe,
        .remove   =       mercd_release,
        .resume   =       mercd_resume,
        .suspend  =       mercd_suspend,
};


static int __init mercd_init_module(void)
{
#ifndef LFS
        int i;
#endif
        int ret;

        // Initialize Control Block
        mercd_zalloc(MsdControlBlock, pmercd_control_block_sT, MERCD_CONTROL_BLOCK_STRUCT);

        mercd_zalloc(pmutex_stats, PMSD_MUTEX_STATS, sizeof(MSD_MUTEX_STATS));

        if (MsdControlBlock == NULL) {
            printk("mercd_init_module: Control Block Allocation Failure");
#ifdef LFS
	    return (-ENOMEM);
#else
            return;
#endif
        }

        MSD_INIT_MUTEX(&MsdControlBlock->mercd_ctrl_block_mutex, "Control Block Mutex", NULL);

        MsdControlBlock->maxbind = MSD_ABSOLUTE_MAX_BIND;
        MsdControlBlock->MsdState = MERCD_CTRL_BLOCK_PRE_INI;
        MsdControlBlock->maxstreams = MSD_INITIAL_MAX_STREAMS;

        /* Initialize the interface function table */
        INITIALIZE_OSAL();
        INITIALIZE_DHAL();

        /* This will give high major node to low */
	mercdMajor = abstract_register_dev(0, &mercd_fops, MSD_MAX_OPEN_ALLOWED, mercd_string);

	if (mercdMajor < 0) {
	    printk("mercd_init_module: Unable to register driver.\n");
	    return mercdMajor;
	}

	if (dm3_feature & 1) { NewCanTakeProtocol = 1; printk("NewCanTake Protocol Enabled \n"); }
        if (dm3_feature & 2) { ReceiveDMA = 1; printk("Receive DMA Enabled \n"); }
        if (dm3_feature & 4) { SendDMA = 1; printk("Send DMA Enabled \n"); }
        if (dm3_feature & 8) { No_Send_From_DPC  = 1; printk("30 mSec Send Only From Timer Enabled \n"); }
        if (dm3_feature & 16) { SendExitNotify = 1; printk("Exit Notification Enabled\n"); }
        if (dm3_feature & 32) {
           NonDefaultId = 1;
           printk("NonDefault mode of Driver attach order based on Configuration id\n");
        }
        if (dm3_feature & 64) { DPC_Disabled = 1; printk("DPC Disabled\n"); }
        if (dm3_feature & 128) { HCS_Flag = 0; printk("HCS Disabled\n"); }
        if (ctimod_check_rh()) { RH_Enabled = 1; printk("RH Enabled \n"); }

#ifndef LFS
        qDrvMsgPoolAllocate(NATIVE_MAX_POOL_SIZE);	
#endif

        ret = pci_module_init(&mercd_driver);

        if (ret < 0)  {
#ifndef LFS
            qDrvMsgPoolFree();
#endif
	    mercd_free(MsdControlBlock,MERCD_CONTROL_BLOCK_STRUCT, MERCD_FORCE);

	    if (abstract_unregister_dev(mercdMajor, mercd_string) < 0) {
		printk("mercd_init_module: Unable to remove driver err=%d.\n", ret);
 	    } else {
		printk("mercd_init_module: Driver removed\n");
            }

	    printk("mercd_init_module: No DM3 Cards found!\n");
        }

        return ret;
}

static void __exit mercd_cleanup_module(void)
{
#ifndef LFS
	int i;
#endif


	if (pmutex_stats) {
            mercd_free(pmutex_stats, sizeof(MSD_MUTEX_STATS), MERCD_FORCE);
	}

	if (abstract_unregister_dev(mercdMajor, mercd_string) < 0) {
	    printk( "mercd_cleanup_module: Unable to remove driver.\n");
	} else {
 	    printk("mercd_cleanup_module: Driver removed.\n");
	}

        pci_unregister_driver(&mercd_driver);
#ifndef LFS
        qDrvMsgPoolFree();
#endif
}

module_init(mercd_init_module);
module_exit(mercd_cleanup_module);

//========================================================================
//
//NAME			:	mercd_release
//DESCRIPTION		:	Streams Driver Release Routine. 
//				Called for each board 
//				Clean up board structures
// INPUTS		:       device info	
// OUTPUTS		:	none
// RETURNS		:	none
// CALLS			:	
// CAUTIONS		:	Called by OS.
//
//========================================================================
static void __devexit mercd_release(struct pci_dev *pdev) {
        mercd_dhal_drvr_free_sT drvr_free = { 0 };
        mercd_hs_t* hsd = NULL;

	hsd = (mercd_hs_t*)pci_get_drvdata(pdev);
        if(hsd == NULL) {
           printk("mercd_release: error retreiving hsd object\n");
           return ;
        }

        MSD_ENTER_CONTROL_BLOCK_MUTEX();

        if (hsd->state != MERCD_SUSPENDED ) {
#ifdef LFS
#ifdef LINUX24
           mercd_suspend(pdev, 0);
#else
           mercd_suspend(pdev, (pm_message_t){ PM_SUSPEND } );
#endif
#else
           mercd_suspend(pdev);
#endif
        }

        drvr_free.pdevi = pdev;
        (*mercd_dhal_func[MERCD_DHAL_DRVR_FREE])((pmerc_void_t)&drvr_free);
        if (drvr_free.ret != MD_SUCCESS) {
           printk("mercd_release: mercd_dhal_drvr_free().. failed\n");
        }

        if (hsd) {
            MSD_FREE_KERNEL_MEMORY(hsd, sizeof(mercd_hs_t));
	}

        MSD_EXIT_CONTROL_BLOCK_MUTEX();

	return;
}

//========================================================================
//
//NAME			:	mercd_probe
//DESCRIPTION		:	Streams Driver probe Routine. 
//				Called for each board 
//				Clean up board structures
// INPUTS		:       device info	
// OUTPUTS		:	none
// RETURNS		:	none
// CALLS			:	
// CAUTIONS		:	Called by OS.
//
//========================================================================
static int __devinit mercd_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
        mercd_dhal_drvr_probe_sT drvr_probeinfo = { 0 };
#ifdef LFS
        mercd_hs_t* hsd = NULL;
        drvr_probeinfo.pdevi = pdev;
#else
        drvr_probeinfo.pdevi = pdev;
        mercd_hs_t* hsd = NULL;
#endif

        hsd = kmalloc(sizeof(mercd_hs_t), GFP_KERNEL);
        if (hsd == NULL) {
          printk("mercd_probe: unable to allocate memory for hsd\n");
          return (0);
        }
        memset(hsd, 0x00, sizeof(mercd_hs_t));

        // Store a pointer to the device object in the kernels pci_dev object 
        pci_set_drvdata(pdev, hsd);

        // Indicate successful probe completition 
        hsd->state = MERCD_PROBED;

        (*mercd_dhal_func[MERCD_DHAL_DRVR_PROBE])((pmerc_void_t)&drvr_probeinfo);

        if (drvr_probeinfo.ret != MD_SUCCESS) {
            printk("mercd_probe: No DM3 boards detected.\n");
            MSD_FREE_KERNEL_MEMORY(hsd, sizeof(mercd_hs_t));
            return (0);
        }

        if (!RH_Enabled) {
            linux_pci_brd_probe((pmerc_void_t)&drvr_probeinfo);
        }

        return (0);
}

//========================================================================
//
//NAME                  :       mercd_resume
//DESCRIPTION           :       Streams Driver probe Routine.
//                              Called for each board
//                              Clean up board structures
// INPUTS               :       device info
// OUTPUTS              :       none
// RETURNS              :       none
// CALLS                        :
// CAUTIONS             :       Called by OS.
//
//========================================================================
int mercd_resume(struct pci_dev *pdev)
{
        mercd_dhal_drvr_probe_sT drvr_probeinfo = { 0 };
#ifdef LFS
        mercd_hs_t* hsd = NULL;

        drvr_probeinfo.pdevi = pdev;
#else
        drvr_probeinfo.pdevi = pdev;

        mercd_hs_t* hsd = NULL;
#endif

        hsd = (mercd_hs_t*)pci_get_drvdata(pdev);
        if(hsd == NULL) {
           printk("mercd_resume: error retreiving hsd object\n");
           return (0);
        }
      
        if (( hsd->state != MERCD_PROBED ) &&  ( hsd->state != MERCD_SUSPENDED )) {
           (*mercd_dhal_func[MERCD_DHAL_DRVR_PROBE])((pmerc_void_t)&drvr_probeinfo);
        }

        linux_pci_brd_probe((pmerc_void_t)&drvr_probeinfo);

	if (drvr_probeinfo.ret == MD_FAILURE) {
	    printk("mercd_resume: board probe failed to resume\n");
            return (0);
	}

        // Indicate successful probe completition 
        hsd->state = MERCD_RESUMED;

        return (0);
}


//========================================================================
//
//NAME                  :       mercd_suspend
//DESCRIPTION           :       Streams Driver probe Routine.
//                              Called for each board
//                              Clean up board structures
// INPUTS               :       device info
// OUTPUTS              :       none
// RETURNS              :       none
// CALLS                        :
// CAUTIONS             :       Called by OS.
//
//========================================================================
#ifdef LFS
#ifdef LINUX24
int mercd_suspend(struct pci_dev *pdev, u32 state)
#else
int mercd_suspend(struct pci_dev *pdev, pm_message_t state)
#endif
#else
int mercd_suspend(struct pci_dev *pdev)
#endif
{
        pmercd_adapter_block_sT padapter;
        mercd_osal_timeout_stop_sT timeoutinfo = { 0 };
        mercd_dhal_intr_disable_sT intr_disableinfo = { 0 };
#ifndef LFS
        mercd_dhal_drvr_free_sT drvr_free = { 0 };
#endif
        mercd_dhal_dma_free_sT   dma_free = { 0 };
        mercd_hs_t* hsd = NULL;
        int j = 0;

        hsd = (mercd_hs_t*)pci_get_drvdata(pdev);
        if(hsd == NULL) {
           printk("mercd_suspend:  error retreiving hsd object\n");
           return (0);
        }

        if (RH_Enabled && (hsd->state != MERCD_RESUMED)) {
           printk("mercd_suspend:  Incorrect state %#x\n", hsd->state);
           return (0);
        }

        hsd->state = MERCD_SUSPENDED;

        if ((padapter = hsd->padapter) != NULL ) {
           if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) || 
	       (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
               // first do a soft reset
               linux_ww_generate_soft_reset(padapter);

               // now a hard reset as well as free up things
               if ((padapter->pww_info) &&
                   (padapter->pww_info->state != MERCD_ADAPTER_WW_IN_SHUTDOWN)) {
                   mid_wwmgr_process_cancel_request_intr_ack_from_board(padapter->pww_info);
               }
           }

           // remove timeout routine timer 
           if (padapter->flags.SendTimeoutPending  & MERC_BOARD_FLAG_SEND_TIMEOUT_PEND) {
#ifdef LFS
               timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id;
#else
               timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id;
#endif
               (*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);

               if (timeoutinfo.ret != MD_SUCCESS) {
                   printk("mercd_suspend: Untimeout failed on board %d\n", padapter->adapternumber);
               }
           }

           // remove host ram semaphore timeout timer 
           if (padapter->flags.SendTimeoutPending  & MERC_ADAPTER_FLAG_HRAM_SEM_TIMEOUT_PEND) {
#ifdef LFS
               timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id_sem;
#else
               timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id_sem;
#endif
               (*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);

               if (timeoutinfo.ret != MD_SUCCESS){
                   printk("mercd_suspend: Untimeout failed for host Sem on board %d\n", padapter->adapternumber);
               }
           }

           // remove interrupt lines and PLX interrupt
           intr_disableinfo.ConfigId = padapter->adapternumber;

           // disable PLX interrupt
           (*mercd_dhal_func[MERCD_DHAL_INTR_DISABLE])((void *) &intr_disableinfo);

           // remove interrupt lines
           if (!( padapter->flags.LaunchIntr & MERC_ADAPTER_FLAG_LAUNCH_INTR)) {
               free_irq(pdev->irq, (void *)padapter);
               padapter->flags.LaunchIntr &= ~MERC_ADAPTER_FLAG_LAUNCH_INTR;
           }

           if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
               dma_free.ConfigId = padapter->adapternumber;
               (*mercd_dhal_func[MERCD_DHAL_DMA_FREE_HANDLE])((void *) &dma_free);
           }
       }

       for (j = 0; j < MSD_MAX_BOARD_ID_COUNT ; j++) {
            if (mercd_adapter_log_to_phy_map_table[j] == mercd_adapter_map[padapter->adapternumber]  ) {
                if (mercd_adapter_map[padapter->adapternumber] != 0xFF )
                    mercd_adapter_log_to_phy_map_table[j] = 0xFF;
                break;
            }
       }

       if (padapter->flags.WWFlags & MERCD_ADAPTER_WW_I20_MESSAGING_READY) {
           padapter->flags.WWFlags &= ~(MERCD_ADAPTER_WW_I20_MESSAGING_READY);
       }
	
       if (padapter->pww_info) {
           if (padapter->pww_info->pww_param)
               mercd_free(padapter->pww_info->pww_param, MERCD_WW_PARAM, MERCD_FORCE);
           mercd_free(padapter->pww_info, MERCD_WW_DEV_INFO, MERCD_FORCE);
	   padapter->pww_info = NULL;
       }

       padapter->state = MERCD_ADAPTER_STATE_SUSPENDED;

       return (0);
}
