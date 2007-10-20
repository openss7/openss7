/* dap.h - */

/* 
 * $Header: /xtel/isode/isode/h/quipu/RCS/dap.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: dap.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef QUIPUDAP
#define QUIPUDAP

extern	int	  dsap_ad;	/* Association descriptor in simple DUA mode */
extern	int	  dsap_id;	/* Invocation identifier in simple DUA mode */

#define ds_read(arg,err,res) dap_read(dsap_ad,&(dsap_id),arg,err,res)
#define ds_compare(arg,err,res) dap_compare(dsap_ad,&(dsap_id),arg,err,res)
#define ds_abandon(arg,err) dap_abandon(dsap_ad,&(dsap_id),arg,err)
#define ds_list(arg,err,res) dap_list(dsap_ad,&(dsap_id),arg,err,res)
#define ds_search(arg,err,res) dap_search(dsap_ad,&(dsap_id),arg,err,res)
#define ds_addentry(arg,err) dap_addentry(dsap_ad,&(dsap_id),arg,err)
#define ds_removeentry(arg,err) dap_removeentry(dsap_ad,&(dsap_id),arg,err)
#define ds_modifyentry(arg,err) dap_modifyentry(dsap_ad,&(dsap_id),arg,err)
#define ds_modifyrdn(arg,err) dap_modifyrdn(dsap_ad,&(dsap_id),arg,err)

#endif
