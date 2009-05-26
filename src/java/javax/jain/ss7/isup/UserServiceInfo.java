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
* DATE         : $Date: 2008/05/16 12:24:13 $
* 
* MODULE NAME  : $RCSfile: UserServiceInfo.java,v $
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

import javax.jain.*;
import javax.jain.ss7.*;

public class UserServiceInfo extends UserServiceInfoCommon{

	public UserServiceInfo(){

	}

	public UserServiceInfo(byte in_infoTransferCap,
                       byte in_codingStd,
                       boolean in_ext1,
                       byte in_infoTransferRate,
                       byte in_transferMode,
                       boolean in_ext2,
                       byte in_establishment,
                       byte in_config,
                       byte in_struct,
                       boolean in_ext2a,
                       byte in_infoTransferRateDestToOrig,
                       byte in_symmetry,
                       boolean in_ext2b,
                       byte in_userInfoLayer1,
                       byte in_layer1Id,
                       boolean in_ext3,
                       byte in_userInfoLayer2,
                       byte in_layer2Id,
                       boolean in_ext4,
                       byte in_userInfoLayer3,
                       byte in_layer3Id,
                       boolean in_ext5,
                       byte in_rateMultiplier,
                       boolean in_ext21,
                       byte in_userRate,
                       boolean in_negotiation,
                       boolean in_syncFlag,
                       boolean in_ext3a,
                       boolean in_flowCntrlRx,
                       boolean in_flowCntrlTx,
                       boolean in_nicRx,
                       boolean in_nicTx,
                       byte in_intermediateRate,
                       boolean in_ext3b1,
                       boolean in_bandNegotiationInd,
                       boolean in_msgOrigInd,
                       boolean in_lliNegotiationInd,
                       boolean in_modeInd,
                       boolean in_multiFrameInd,
                       boolean in_rateAdaptationInd,
                       boolean in_ext3b2,
                       byte in_parity,
                       byte in_numDataBits,
                       byte in_numStopBits,
                       boolean in_ext3c,
                       byte in_modemType,
                       boolean in_duplexModeInd,
                       boolean in_ext3d)
                throws ParameterRangeInvalidException{

		super();

		m_infoTransferCap = in_infoTransferCap;
        m_codingStd = in_codingStd;
        m_ext1 = in_ext1;
        m_infoTransferRate = in_infoTransferRate;
        m_transferMode = in_transferMode;
        m_ext2 = in_ext2;
        m_establishment = in_establishment;
        m_config = in_config;
        m_struct = in_struct;
        m_ext2a = in_ext2a;
        m_infoTransferRateDestToOrig = in_infoTransferRateDestToOrig;
        m_symmetry = in_symmetry;
        m_ext2b = in_ext2b;
        m_userInfoLayer1 = in_userInfoLayer1;
        m_layer1Id = in_layer1Id;
        m_ext3 = in_ext3;
        m_userInfoLayer2 = in_userInfoLayer2;
        m_layer2Id = in_layer2Id;
        m_ext4 = in_ext4;
        m_userInfoLayer3 = in_userInfoLayer3;
        m_layer3Id = in_layer3Id;
        m_ext5 = in_ext5;
        m_rateMultiplier = in_rateMultiplier;
        m_ext21 = in_ext21;
        m_userRate = in_userRate;
        m_negotiation = in_negotiation;
        m_syncFlag = in_syncFlag;
        m_ext3a = in_ext3a;
        m_flowCntrlRx = in_flowCntrlRx;
        m_flowCntrlTx = in_flowCntrlTx;
        m_nicRx = in_nicRx;
        m_nicTx = in_nicTx;
        m_intermediateRate = in_intermediateRate;
        m_ext3b1 = in_ext3b1;
        m_bandNegotiationInd = in_bandNegotiationInd;
        m_msgOrigInd = in_msgOrigInd;
        m_lliNegotiationInd = in_lliNegotiationInd;
        m_modeInd = in_modeInd;
        m_multiFrameInd = in_multiFrameInd;
        m_rateAdaptationInd = in_rateAdaptationInd;
        m_ext3b2 = in_ext3b2;
        m_parity = in_parity;
        m_numDataBits = in_numDataBits;
        m_numStopBits = in_numStopBits;
        m_ext3c = in_ext3c;
        m_modemType = in_modemType;
        m_duplexModeInd = in_duplexModeInd;
        m_ext3d = in_ext3d;
	}

	public byte[] flatUserServiceInfo()
	{
		return super.flatUserServiceInfoCommon();
	}

	public void  putUserServiceInfo(byte[] arr, int index, byte par_len){
		super.putUserServiceInfoCommon(arr,index,par_len);
	}
	
	/**
    * String representation of class UserServiceInfo
    *
    * @return    String provides description of class UserServiceInfo
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
			    buffer.append(super.toString());
				return buffer.toString(); 
		
		}		
	
	

}




