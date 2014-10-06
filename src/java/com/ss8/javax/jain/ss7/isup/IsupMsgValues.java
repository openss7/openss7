/******************************************************************************
*                                                                             *
*                                                                             *
* Copyright (c) SS8 Networks, Inc.                                            *
* All rights reserved.                                                        *
*                                                                             *
* This document contains confidential and proprietary information in which    *
* any reproduction, disclosure, or use in whole or in part is expressly       *
* prohibited, except as may be specifically authorized by prior written       *
* agreement or permission of SS8 Networks, Inc.                               *
*                                                                             *
*******************************************************************************
* VERSION      : $Revision: 1.1 $
* DATE         : $Date: 2008/05/16 12:23:17 $
* 
* MODULE NAME  : $RCSfile: IsupMsgValues.java,v $
* AUTHOR       : Nilgun Baykal [SS8]
* DESCRIPTION  : 
* DATE 1st REL : 
* REV.HIST.    : 
* 
* Date      Owner  Description
* ========  =====  ===========================================================
* 
* 
*******************************************************************************
*                                                                             *
*                     RESTRICTED RIGHTS LEGEND                                *
* Use, duplication, or disclosure by Government Is Subject to restrictions as *
* set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and    *
* Computer Software clause at DFARS 252.227-7013                              *
*                                                                             *
******************************************************************************/
package com.ss8.javax.jain.ss7.isup;


public class IsupMsgValues{

	public IsupMsgValues(){

	}

        /*****************************************************
        *     definitions for message type codes ITU/ANSI    *
        ******************************************************/
public static final byte ISUP_MSG_NULL		     =  0x00;   /* NULL message*/
public static final byte ISUP_MSG_ACM            =  0x06;   /* Address complete ITU/ANSI             */
public static final byte ISUP_MSG_ANM            =  0x09;   /* Answer ITU/ANSI                       */
public static final byte ISUP_MSG_BLO            =  0x13;   /* Blocking ITU/ANSI                     */
public static final byte ISUP_MSG_BLA            =  0x15;   /* Blocking acknowledgment ITU/ANSI      */
public static final byte ISUP_MSG_CMC            =  0x1D;   /* Call modification completed ITU only  */
public static final byte ISUP_MSG_CMR            =  0x1C;   /* Call modification request ITU only    */
public static final byte ISUP_MSG_CMJ            =  0x1E;   /* Call modification reject ITU only     */
public static final byte ISUP_MSG_CPG            =  0x2C;   /* Call progress ITU/ANSI                */
public static final byte ISUP_MSG_CGB            =  0x18;   /* Circuit group blocking ITU/ANSI       */
public static final byte ISUP_MSG_CGBA           =  0x1A;   /* Circuit group blocking ack ITU/ANSI   */
public static final byte ISUP_MSG_CQM            =  0x2A;   /* Circuit group query ITU/ANSI          */
public static final byte ISUP_MSG_CQR            =  0x2B;   /*Circuitgroup query response ITU/ANSI  */
public static final byte ISUP_MSG_GRS            =  0x17;   /* Circuit group reset ITU/ANSI          */
public static final byte ISUP_MSG_GRA            =  0x29;   /* Circuit group reset ack ITU/ANSI      */
public static final byte ISUP_MSG_CGU            =  0x19;   /* Circuit group unblocking ITU/ANSI     */
public static final byte ISUP_MSG_CGUA           =  0x1B;   /* Circuit group unblocking ack ITU/ANSI */
public static final byte ISUP_MSG_CHI            =  0x31;   /* Charge information ITU only           */
public static final byte ISUP_MSG_CFN            =  0x2F;   /* Confusion ITU/ANSI                    */
public static final byte ISUP_MSG_CON            =  0x07;   /* Connect ITU only                      */
public static final byte ISUP_MSG_COT            =  0x05;   /* Continuity ITU/ANSI                   */
public static final byte ISUP_MSG_CCR            =  0x11;   /* Continuity check request ITU/ANSI     */
public static final byte ISUP_MSG_DRS            =  0x27;   /* Delayed release ITU only              */
public static final byte ISUP_MSG_FCA            =  0x20;   /* Facility accepted ITU/ANSI            */
public static final byte ISUP_MSG_FCJ            =  0x21;   /* Facility reject ITU/ANSI              */
public static final byte ISUP_MSG_FCR            =  0x1F;   /* Facility request ITU/ANSI             */
public static final byte ISUP_MSG_FOT            =  0x08;   /* Forward transfer ITU/ANSI             */
public static final byte ISUP_MSG_INF            =  0x04;   /* Information ITU/ANSI                  */
public static final byte ISUP_MSG_INR            =  0x03;   /* Information request ITU/ANSI          */
public static final byte ISUP_MSG_IAM            =  0x01;   /* Initial address ITU/ANSI              */
public static final byte ISUP_MSG_LBA            =  0x24;   /* Loop back acknowledgment ITU/ANSI     */
public static final byte ISUP_MSG_OVL            =  0x30;   /* Overload ITU only                     */
public static final byte ISUP_MSG_PSA            =  0x28;   /* Pass-along ITU/ANSI                   */
public static final byte ISUP_MSG_REL            =  0x0C;   /* Release ITU/ANSI                      */
public static final byte ISUP_MSG_RLC            =  0x10;   /* Release complete ITU/ANSI             */
public static final byte ISUP_MSG_RSC            =  0x12;   /* Reset circuit ITU/ANSI                */
public static final byte ISUP_MSG_RES            =  0x0E;   /* Resume ITU/ANSI                       */
public static final byte ISUP_MSG_SAM            =  0x02;   /*Subsequent address message ITU only  */
public static final byte ISUP_MSG_SUS            =  0x0D;   /* Suspend ITU/ANSI                      */
public static final byte ISUP_MSG_UBL            =  0x14;   /* Unblocking ITU/ANSI                   */
public static final byte ISUP_MSG_UBA            =  0x16;   /* Unblocking acknowledgment ITU/ANSI    */
public static final byte ISUP_MSG_UCIC           =  0x2E;   /* Unequipped CIC ITU/ANSI               */
public static final byte ISUP_MSG_UUI            =  0x2D;   /* User-to-user information ITU/ANSI     */
public static final byte ISUP_MSG_CRM            =  (byte)0xEA;   /* Circuit reservation ANSI only         */
public static final byte ISUP_MSG_CRA            =  (byte)0xE9;   /* Circuit reservation ack ANSI only     */
public static final byte ISUP_MSG_CVR            =  (byte)0xEB;   /* Circuitvalidation response ANSI only */
public static final byte ISUP_MSG_CVT            =  (byte)0xEC;   /* Circuit validation test ANSI only     */
public static final byte ISUP_MSG_EXM            =  (byte)0xED;   /* Exit message ANSI only                */
public static final byte ISUP_MSG_NRM            =  0x32;   /* Network Resource Management ITU-WB    */
public static final byte ISUP_MSG_FAC            =  0x33;   /* Facility ITU-WB/ANSI-96               */
public static final byte ISUP_MSG_UPT            =  0x34;   /* User Part Test ITU-WB                 */
public static final byte ISUP_MSG_UPA            =  0x35;   /* User Part Available ITU-WB            */
public static final byte ISUP_MSG_IDR            =  0x36;   /* Identification Request ITU-WB         */
public static final byte ISUP_MSG_IRS            =  0x37;   /* Identification Response ITU-WB        */
public static final byte ISUP_MSG_SGM            =  0x38;   /* Segmentation ITU-WB                   */

    

	

}
