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
* DATE         : $Date: 2008/05/16 12:24:03 $
* 
* MODULE NAME  : $RCSfile: IsupMacros.java,v $
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


package javax.jain.ss7.isup;


public class IsupMacros{

	public IsupMacros(){

	}

	public static final byte L_BIT1_MASK = 0x01;
	public static final byte L_BIT2_MASK = 0x02;
	public static final byte L_BIT3_MASK = 0x04;
	public static final byte L_BIT4_MASK = 0x08;
	public static final byte L_BIT5_MASK = 0x10;
	public static final byte L_BIT6_MASK = 0x20;
	public static final byte L_BIT7_MASK = 0x40;
	public static final byte L_BIT8_MASK = (byte)0x80;

	public static final byte L_bits18_MASK = (byte)0xFF;
	public static final byte L_bits21_MASK = 0x03;
	public static final byte L_bits31_MASK = 0x07;	
	public static final byte L_bits32_MASK = 0x06;
	public static final byte L_bits41_MASK = 0x0F;
	public static final byte L_bits43_MASK = 0x0C;
	public static final byte L_bits51_MASK = 0x1F;
	public static final byte L_bits54_MASK = 0x18;
	public static final byte L_bits61_MASK = 0x3F;
	public static final byte L_bits65_MASK = 0x30;
	public static final byte L_bits71_MASK = 0x7F;
	public static final byte L_bits75_MASK = 0x70;
	public static final byte L_bits76_MASK = 0x60;
	public static final byte L_bits83_MASK = (byte)0xFC;
	public static final byte L_bits85_MASK = (byte)0xF0;
	public static final byte L_bits86_MASK = (byte)0xE0;
	public static final byte L_bits87_MASK = (byte)0xC0;

}
