/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_osal.h
 * Description                  : OS abstraction layer definitions
 *
 *
 **********************************************************************/

#define MERCD_OSAL_DPC_REGISTER			0x0001
#define MERCD_OSAL_DPC_DEREGISTER		0x0002
#define MERCD_OSAL_INTR_REGISTER		0x0003
#define MERCD_OSAL_INTR_DEREGISTER		0x0004
#define MERCD_OSAL_MEM_PHYS_VIRT		0x0005
#define MERCD_OSAL_MEM_VIRT_FREE		0x0006
#define MERCD_OSAL_TIMEOUT_START		0x0007
#define MERCD_OSAL_TIMEOUT_STOP			0x0008


typedef struct _mercd_osal_mem_phys_virt{
	mercd_dev_info_T *dip;
	mercd_dev_acc_handle_T *DevAccHandlePtr;
	merc_int_t	RegSet;
	merc_uint_t	PhysAddr;
	merc_uint_t	ByteCount;
	merc_int_t	ConfigId;
	pmerc_void_t	ret;
}mercd_osal_mem_phys_virt_sT, *pmercd_osal_mem_phys_virt_sT;


typedef struct _mercd_osal_mem_virt_free{
	mercd_dev_acc_handle_T	*DevAccHandlePtr;
	size_t                  VirtAddr; /* Was merc_uint_t VirtAddr; */
	merc_uint_t             ByteCount;
	md_status_t             ret;
}mercd_osal_mem_virt_free_sT, *pmercd_osal_mem_virt_free_sT;

typedef struct _mercd_osal_timeout_start{
	PMD_FUNCTION	Function;
	pmerc_void_t	Context;
#if defined LiS || defined LIS || defined LFS
#ifdef LFS
	toid_t		*Handle; /* Was -> INT32 *  Handle;  */
#else
	ssize_t		*Handle; /* Was -> INT32 *  Handle;  */
#endif
#else
	struct timer_list *Handle;
#endif /* LiS */
	merc_uint_t	Interval;
	md_status_t	ret;
}mercd_osal_timeout_start_sT, *pmercd_osal_timeout_start_sT;

typedef struct _mercd_osal_timeout_stop{
#if defined LiS || defined LIS || defined LFS
#ifdef LFS
	toid_t		*Handle;	/* Was -> merc_uint_t Handle; */
#else
	ssize_t		Handle;	/* Was -> merc_uint_t Handle; */
#endif
#else
	struct timer_list *Handle;
#endif /* LiS */
	md_status_t	ret;
}mercd_osal_timeout_stop_sT, *pmercd_osal_timeout_stop_sT;


typedef struct _mercd_osal_intr_register{
/* Why are we using all these; these are all defined in hw_info structure */
	mercd_dev_info_T   *dip;
	pmercd_hw_info_sT  phw_info;
	PMSD_FUNCTION      Isr;
	size_t             IsrArg;     /* Was -> ULONG   IsrArg; */
	size_t             *IsrHandle; /* Was -> PULONG  IsrHandle; */
	merc_uint_t        Level;
	merc_uint_t        IntrVector;
	merc_int_t         ConfigId;
	md_status_t        ret;
}mercd_osal_intr_register_sT, *pmercd_osal_intr_register_sT;

typedef struct _mercd_osal_intr_deregister{
	mercd_dev_info_T    *dip;  /* ASA */
	pmercd_hw_info_sT   phw_info;
	size_t              IsrArg;        /* Added for Linux  */
	size_t              IsrHandle;     /* Was -> ULONG IsrHandle; */
	merc_int_t          ConfigId;
	md_status_t         ret;
}mercd_osal_intr_deregister_sT, *pmercd_osal_intr_deregister_sT;

typedef struct _mercd_osal_dpc_register{
	PMD_FUNCTION        Function;
	pmercd_hw_info_sT   phw_info;
	pmerc_void_t        Context;
	size_t              *DpcHandle;	/* Was -> PULONG DpcHandle; */
	merc_uint_t         ConfigId;
	md_status_t         ret;
}mercd_osal_dpc_register_sT, *pmercd_osal_dpc_register_sT;


typedef struct _mercd_osal_dpc_deregister{
	size_t		DpcHandle;	/* Was -> merc_uint_t DpcHandle; */
	merc_uint_t	ConfigId;
	md_status_t	ret;
}mercd_osal_dpc_deregister_sT, *pmercd_osal_dpc_deregister_sT;

