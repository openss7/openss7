/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_linux_osal.c
 * Description                  : Operating System Abstraction Layer
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MERCD_OSAL_LINUX_C
#include "msdextern.h"
#undef _MERCD_OSAL_LINUX_C

/*
 * OSAL Functional Definition for OS Specific Driver Layer to Core DM3
 * Driver Architecture Layer
 */

int initialize_linux_osal()
{
        /*
         * Initializing the CDDA component of the OSAL
         */
        MSD_FUNCTION_TRACE("initialize_linux_osal", NO_PARAMETERS);
        mercd_osal_mid_func[MID_STREAM_SEND] = &mid_strmmgr_send;
        mercd_osal_mid_func[MID_PAM_SEND_MERCURY] = &mid_msgmgr_pam_send_mercury;
        mercd_osal_mid_func[MID_DISPATCHER_BIND] = &mid_bndmgr_bind;
        mercd_osal_mid_func[MID_DISPATCHER_UNBIND] = &mid_bndmgr_unbind;
        mercd_osal_mid_func[MID_STREAM_OPEN] = &mid_strmmgr_strm_open;
        mercd_osal_mid_func[MID_STREAM_CLOSE] = &mid_strmmgr_strm_close;
        mercd_osal_mid_func[MID_STREAM_SET_PARAMETER] = &mid_strmmgr_set_param;
        mercd_osal_mid_func[MID_STREAM_GET_PARAMETER] = &mid_strmmgr_get_param;
        mercd_osal_mid_func[MID_STREAM_FLUSH] = &mid_strmmgr_strm_flush;
        mercd_osal_mid_func[MID_STREAM_CLOSE_INTERNAL] = &mid_strmmgr_strm_close_internal;
        mercd_osal_mid_func[MID_CFG_DRIVER_START] = &mid_drvrmgr_cfg_drvr_start;
        mercd_osal_mid_func[MID_CFG_DRIVER_SHUTDOWN] = &mid_drvrmgr_cfg_drvr_shutdown;
        mercd_osal_mid_func[MID_CFG_PAM_START] = &mid_drvrmgr_cfg_pam_start;
        mercd_osal_mid_func[MID_CFG_PAM_SHUTDOWN] = &mid_drvrmgr_cfg_pam_shutdown;
        mercd_osal_mid_func[MID_CFG_BOARD_CONFIG] = &mid_brdmgr_cfg_brd_config;
        mercd_osal_mid_func[MID_CFG_BOARD_SHUTDOWN] = &mid_brdmgr_cfg_brd_shutdown;
        mercd_osal_mid_func[MID_CFG_GET_PARAMETER] = &mid_brdmgr_get_param;
        mercd_osal_mid_func[MID_GET_BOARD_CONFIG] = &mid_brdmgr_cfg_get_brd_config;
        mercd_osal_mid_func[MID_CFG_MAP_BOARD] = &mid_brdmgr_cfg_map_brd;
        mercd_osal_mid_func[MID_CFG_GET_PHYS_BOARD_ATTR] = &mid_brdmgr_cfg_get_phys_brd_attrib;
        mercd_osal_mid_func[MID_CFG_GET_BOARD_STATE] = &mid_brdmgr_cfg_get_brd_state;
        mercd_osal_mid_func[MID_CFG_GET_CONFIG_ROM] = &mid_brdmgr_cfg_get_config_rom;
        mercd_osal_mid_func[MID_CFG_SCAN_BOARD] = &mid_brdmgr_cfg_scan_brd;
        mercd_osal_mid_func[MID_PAM_GET_PANIC_DUMP] = &mid_drvrmgr_pam_get_panic_dump ;
        mercd_osal_mid_func[MID_PAM_SET_BOOT_HOST_RAM_BIT] = &mid_getset_boot_host_ram_bit;
        mercd_osal_mid_func[MID_PAM_GET_BOOT_HOST_RAM_BIT] = &mid_getset_boot_host_ram_bit;
        mercd_osal_mid_func[MID_CFG_GET_BRD_DIAG_STATE] = &mid_brdmgr_cfg_get_brd_diag_state;
        mercd_osal_mid_func[MID_CFG_SET_BRD_STATE_DNLD] = &mid_brdmgr_cfg_set_brd_state_dnld;
        mercd_osal_mid_func[MID_ENABLE_DRV_TRACE] = &mid_drvrmgr_enable_drv_trace;
        mercd_osal_mid_func[MID_DISABLE_DRV_TRACE] = &mid_drvrmgr_disable_drv_trace;
        mercd_osal_mid_func[MID_GET_DRV_TRACE] = &mid_drvr_get_drv_trace;
        mercd_osal_mid_func[MID_STREAM_MBLK_SEND] = &mid_strmmgr_mblk_send;



        mercd_osal_mid_func[MID_PAM_BOARD_START] = &mid_brdmgr_pam_brd_start ;
        mercd_osal_mid_func[MID_EXIT_NOTIFY_BIND] = &mid_bndmgr_bind_exit_markup;
	mercd_osal_mid_func[MID_BRDID_TO_CFGID] = &mid_brdmgr_pam_brd_to_cfg;
	mercd_osal_mid_func[MID_GET_SRAM] = &mid_brdmgr_cfg_get_sram;
	mercd_osal_mid_func[MID_STREAM_SEND_TERMINATE] = &mid_strmmgr_strm_terminate;
	mercd_osal_mid_func[MID_GET_POWER_STATUS] = &mid_brdmgr_get_power_status;

        // WW Support
        mercd_osal_mid_func[MID_SET_WW_PARAM] = &mid_wwmgr_set_param;
        mercd_osal_mid_func[MID_GET_WW_PARAM] = &mid_wwmgr_get_param;
        mercd_osal_mid_func[MID_SET_WW_MODE] = &mid_wwmgr_set_mode;
        mercd_osal_mid_func[MID_GET_WW_MODE_OPERATIONAL] = &mid_wwmgr_get_mode_operational;
        mercd_osal_mid_func[MID_GET_WW_MODE] = &mid_wwmgr_get_mode;
	mercd_osal_mid_func[MID_START_WW_STREAM_READ] = &mid_wwmgr_start_streams;
        mercd_osal_mid_func[MID_START_WW_STRM_DEBUG] = &mid_wwmgr_debug_streams;

	// Post State (Third Rock Only)
	mercd_osal_mid_func[MID_CFG_GET_POST_STATE] = &mid_brdmgr_get_post_state;
	mercd_osal_mid_func[MID_CFG_SET_POST_STATE] = &mid_brdmgr_set_post_state;
	
        // Driver Statistics
        mercd_osal_mid_func[MID_GET_STATISTICS] = &mid_drvrmgr_get_statistics;

        /*
         * Initializing the OSSDL Components for each OS
         */

        mercd_osal_func[MERCD_OSAL_DPC_REGISTER] = &mercd_return_success;
        mercd_osal_func[MERCD_OSAL_DPC_DEREGISTER] = &mercd_return_success;
        mercd_osal_func[MERCD_OSAL_INTR_REGISTER] = &linux_intr_register_intr_srvc;
        mercd_osal_func[MERCD_OSAL_INTR_DEREGISTER] = &linux_intr_deregister_intr_srvc;
        mercd_osal_func[MERCD_OSAL_MEM_PHYS_VIRT] = &linux_phys2virt;
        mercd_osal_func[MERCD_OSAL_MEM_VIRT_FREE] = &linux_free_virt_addr;
        mercd_osal_func[MERCD_OSAL_TIMEOUT_START] = &linux_timeout;
        mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP] = &linux_untimeout;

        return(0);
}
