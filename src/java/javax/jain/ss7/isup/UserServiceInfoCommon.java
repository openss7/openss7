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
* DATE         : $Date: 2008/05/16 12:24:14 $
* 
* MODULE NAME  : $RCSfile: UserServiceInfoCommon.java,v $
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

public abstract class UserServiceInfoCommon extends java.lang.Object implements java.io.Serializable{

	
	public UserServiceInfoCommon(){

	}
public UserServiceInfoCommon(byte in_infoTransferCap,
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


		 m_ext1 = in_ext1;
		 m_ext2 = in_ext2;
		 m_establishment = in_establishment;
		 m_struct = in_struct;
		 m_ext2a = in_ext2a;
		 m_infoTransferRateDestToOrig = in_infoTransferRateDestToOrig;
		 m_ext2b = in_ext2b;
		 m_layer1Id = in_layer1Id;
		 m_ext3 = in_ext3;
		 m_layer2Id = in_layer2Id;
		 m_ext4 = in_ext4;
		 m_layer3Id = in_layer3Id;
		 m_ext5 = in_ext5;
		 m_ext21 = in_ext21;
		 m_negotiation = in_negotiation;
		 m_syncFlag = in_syncFlag;
		 m_ext3a = in_ext3a;
		 m_flowCntrlRx = in_flowCntrlRx;
		 m_flowCntrlTx = in_flowCntrlTx;
		 m_nicRx = in_nicRx;
		 m_nicTx = in_nicTx;
		 m_ext3b1 = in_ext3b1;
		 m_bandNegotiationInd = in_bandNegotiationInd;
		 m_msgOrigInd = in_msgOrigInd;
		 m_lliNegotiationInd = in_lliNegotiationInd;
		 m_modeInd = in_modeInd;
		 m_multiFrameInd = in_multiFrameInd;
		 m_rateAdaptationInd = in_rateAdaptationInd;
		 m_ext3b2 = in_ext3b2;
		 m_ext3c = in_ext3c;
		 m_modemType = in_modemType;
		 m_duplexModeInd = in_duplexModeInd;
		 m_ext3d = in_ext3d;

		 if ((in_codingStd >= CS_ITU_STANDARD) &&
		  (in_codingStd <= CS_NATIONAL_STANDARD)) {
			 m_codingStd = in_codingStd;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if (in_config == 0)
			 m_config = in_config;

		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_infoTransferCap >= ITC_SPEECH) &&
		  (in_infoTransferCap <= ITC_VIDEO)) {
	    		 m_infoTransferCap = in_infoTransferCap;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_infoTransferRate >= ITR_2_by_64_KBPS) &&
		  (in_infoTransferRate <= ITR_MULTI_RATE)) {
			 m_infoTransferRate = in_infoTransferRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_intermediateRate >= IR_NOT_USED) &&
		  (in_intermediateRate <= IR_32_KBPS)) {
			 m_intermediateRate = in_intermediateRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_numDataBits >= NDBEP_NOT_USED) &&
		  (in_numDataBits <= NDBEP_8_BIT)) {
			 m_numDataBits = in_numDataBits;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_numStopBits >= NSB_NOT_USED) &&
		  (in_numStopBits <= NSB_2_bit)) {
			 m_numStopBits = in_numStopBits;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_parity >= PI_ODD) &&
		  (in_parity <= PI_FORCED_TO_1)) {
			 m_parity = in_parity;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_rateMultiplier >= 0x01) &&
		  (in_rateMultiplier <= 0x18)) {
			 m_rateMultiplier = in_rateMultiplier;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if (in_symmetry == 0) {
			 m_symmetry = in_symmetry;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_transferMode == TM_CIRCUIT_MODE) ||
		  (in_transferMode == TM_PACKET_MODE)) {
			 m_transferMode = in_transferMode;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_userInfoLayer1 >= UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30) &&
		  (in_userInfoLayer1 <= UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING)) {
			 m_userInfoLayer1 = in_userInfoLayer1;

		 }
		 else 
			throw new ParameterRangeInvalidException();
		 if ((in_userInfoLayer2 >= UIL2P_Q921_I441) &&
		  (in_userInfoLayer2 <= UIL2P_X25)) {
			 m_userInfoLayer2 = in_userInfoLayer2;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_userInfoLayer3 >= UIL3P_Q931) &&
		  (in_userInfoLayer3 <= UIL3P_X25)) {
			m_userInfoLayer3 = in_userInfoLayer3;

		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_userRate >= UR_RATE_I460) &&
		  (in_userRate <= UR_12DOT0_KBPS)) {
			 m_userRate = in_userRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();

	}

	public boolean getBandNegotiationInd(){
		return m_bandNegotiationInd;
	}

	public void setBandNegotiationInd(boolean aInbandOutbandNegot){
		m_bandNegotiationInd = aInbandOutbandNegot;
	}

	public byte getCodingStd(){
		return m_codingStd;
	}

	public void setCodingStd(byte aCodingStd)
                  throws ParameterRangeInvalidException{
		 if ((aCodingStd >= CS_ITU_STANDARD) &&
		  (aCodingStd <= CS_NATIONAL_STANDARD)) {
			m_codingStd = aCodingStd;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getConfiguration(){
		return m_config;
	}

	public void setConfiguration(byte aConfiguration)
                      throws ParameterRangeInvalidException{
		 if (aConfiguration == 0)
			 m_config = aConfiguration;

		 else 
			throw new ParameterRangeInvalidException();
	}

	public boolean getDuplexModeInd(){
		return m_duplexModeInd;
	}
	
	public void setDuplexModeInd(boolean aDuplexMode){
			m_duplexModeInd = 	aDuplexMode;
	}

	public byte getEstablishment(){
		return m_establishment;
	}

	public void setEstablishment(byte aEstablishment)
                      throws ParameterRangeInvalidException{
		m_establishment = aEstablishment;
	}

	public boolean getext3b2(){
		return m_ext3b2;
	}

	public void setext3b2(boolean aExt3b2){
		m_ext3b2 = aExt3b2;
	}

	public boolean getExtension1(){
		return m_ext1;
	}

	public void setExtension1(boolean aExt1){
		m_ext1 = aExt1;
	}
	
	public boolean getExtension2_1(){
		return m_ext21;
	}

	public void setExtension2_1(boolean aExt2_1){
		m_ext21 =  aExt2_1;
	}

	public boolean getExtension2(){
		return m_ext2;
	}

	public void setExtension2(boolean aExt2){
		m_ext2 = aExt2;
	}

	public boolean getExtension2a(){
		return m_ext2a;
	}

	public void setExtension2a(boolean aExt2a){
		m_ext2a = aExt2a;
	}
		
	public boolean getExtension2b(){
		return m_ext2b;
	}
		
	public void setExtension2b(boolean aExt2b){
		m_ext2b = aExt2b;
	}

	public boolean getExtension3(){
		return m_ext3;
	}
	
	public void setExtension3(boolean aExt3){
		m_ext3 = aExt3;
	}

	public boolean getExtension3a(){
		return m_ext3a;
	}

	public void setExtension3a(boolean aExt3a){
		m_ext3a = aExt3a;
	}

	public boolean getExtension3b1(){
		return m_ext3b1;
	}

	public void setExtension3b1(boolean aExt3b1){
		m_ext3b1 = aExt3b1;
	}
	
	public boolean getExtension3b2(){
		return m_ext3b2;
	}

	public void setExtension3b2(boolean aExt3b2){
		m_ext3b2 = aExt3b2;
	}

	public boolean getExtension3c(){
		return m_ext3c;
	}
	
	public void setExtension3c(boolean aExt3c){
		m_ext3c = aExt3c;
	}

	public boolean getExtension3d(){
		return m_ext3d;
	}

	public void setExtension3d(boolean aExt3d){
		m_ext3d = aExt3d;
	}

	public boolean getExtension4(){
		return m_ext4;
	}

	public void setExtension4(boolean aExt4){
		m_ext4 =  aExt4;
	}

	public boolean getExtension5(){
		return m_ext5;
	}

	public void setExtension5(boolean aExt5){
		m_ext5 =  aExt5;
	}

	public boolean getFlowControlRx(){
		return m_flowCntrlRx;
	}

	public void setFlowControlRx(boolean aFlowControlRx){
		m_flowCntrlRx = aFlowControlRx;
	}
	
	public boolean getFlowControlTx(){
		return m_flowCntrlTx;
	}
	
	public void setFlowControlTx(boolean aFlowControlTx){
		m_flowCntrlTx = aFlowControlTx;
	}

	public byte getInfoTransferCap(){
		return m_infoTransferCap;
	}

	public void setInfoTransferCap(byte aInfoTransferCap)
                        throws ParameterRangeInvalidException{
		 if ((aInfoTransferCap >= ITC_SPEECH) &&
		  (aInfoTransferCap <= ITC_VIDEO)) {
			m_infoTransferCap = aInfoTransferCap;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getInfoTransferRate(){
		return m_infoTransferRate;
	}

	public void setInfoTransferRate(byte aInfoTransferRate)
                         throws ParameterRangeInvalidException{
		 if ((aInfoTransferRate >= ITR_2_by_64_KBPS) &&
		  (aInfoTransferRate <= ITR_MULTI_RATE)) {
			m_infoTransferRate = aInfoTransferRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getInfoTransferRateDestToOrig(){
		return m_infoTransferRateDestToOrig;
	}

	public void setInfoTransferRateDestToOrig(byte aInfoTransferRateDestToOrig)
                                   throws ParameterRangeInvalidException{
		m_infoTransferRateDestToOrig = aInfoTransferRateDestToOrig;
	}


	public byte getIntermediateRate(){
		return m_intermediateRate;
	}

	public void setIntermediateRate(byte aIntermediateRate)
                         throws ParameterRangeInvalidException{
		 if ((aIntermediateRate >= IR_NOT_USED) &&
		  (aIntermediateRate <= IR_32_KBPS)) {
			m_intermediateRate = aIntermediateRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getLayer1Id(){
		return m_layer1Id;
	}

	public void setLayer1Id(byte aLayer1Id)
                 throws ParameterRangeInvalidException{
		m_layer1Id = aLayer1Id;
	}

	public byte getLayer2Id(){
		return m_layer2Id;
	}

	public void setLayer2Id(byte aLayer2Id)
                 throws ParameterRangeInvalidException{
		m_layer2Id = aLayer2Id;
	}

	public byte getLayer3Id(){
		return m_layer3Id;
	}

	public void setLayer3Id(byte aLayer3Id)
                 throws ParameterRangeInvalidException{
		m_layer3Id = aLayer3Id;
	}

	public boolean getLogLinkIdNegotiationInd(){
		return m_lliNegotiationInd;
	}

	public void setLogLinkIdNegotiationInd(boolean aLLINegotiationInd){
		m_lliNegotiationInd = aLLINegotiationInd;
	}

	public boolean getMessageOriginatorInd(){
		return m_msgOrigInd;
	}

	public void setMessageOriginatorInd(boolean aAssignorAssignee){
		m_msgOrigInd = aAssignorAssignee;
	}

	public byte getModemType(){
		return m_modemType;
	}

	public void setModemType(byte aModemType)
                  throws ParameterRangeInvalidException{
		m_modemType = aModemType;
	}

	public boolean getModeOfOperation(){
		return m_msgOrigInd;
	}

	public void setModeOfOperation(boolean aModeOfOperation){
		m_msgOrigInd = aModeOfOperation;
	}

	public boolean getMultiFrameEstablishmentInd(){
		return m_multiFrameInd;
	}

	public void setMultiFrameEstablishmentInd(boolean aMultiFrameInd){
		m_multiFrameInd = aMultiFrameInd;
	}

	public boolean getNegotiation(){
		return m_negotiation;
	}

	public void setNegotiation(boolean aNegotiation){
		m_negotiation = aNegotiation;
	}

	public boolean getNICRx(){
		return m_nicRx;
	}

	public void setNICRx(boolean aNICRx){
		m_nicRx = aNICRx;
	}

	public boolean getNICTx(){
		return m_nicTx;
	}

	public void setNICTx(boolean aNICTx){
		m_nicTx = aNICTx;
	}

	public byte getNumberOfDataBits(){
		return m_numDataBits;
	}

	public void setNumberOfDataBits(byte aNumOfDataBits)
                         throws ParameterRangeInvalidException{
		 if ((aNumOfDataBits >= NDBEP_NOT_USED) &&
		  (aNumOfDataBits <= NDBEP_8_BIT)) {
			m_numDataBits = aNumOfDataBits;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getNumberOfStopBits(){
		return m_numStopBits;
	}

	public void setNumberOfStopBits(byte aNumOfStopBits)
                         throws ParameterRangeInvalidException{
		 if ((aNumOfStopBits >= NSB_NOT_USED) &&
		  (aNumOfStopBits <= NSB_2_bit)) {
			m_numStopBits = aNumOfStopBits;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getParity(){
		return m_parity;
	}
		
	public void setParity(byte aParity)
               throws ParameterRangeInvalidException{
		 if ((aParity >= PI_ODD) &&
		  (aParity <= PI_FORCED_TO_1)) {
			m_parity = aParity;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public boolean getRateAdaptationHdr(){
		return m_rateAdaptationInd;
	}

	public void setRateAdaptationHdr(boolean aRateAdaptationHdr){
		m_rateAdaptationInd = aRateAdaptationHdr;
	}

	public byte getRateMultiplier(){
		return m_rateMultiplier;
	}

	public void setRateMultiplier(byte aRateMultiplier)
                       throws ParameterRangeInvalidException{
		 if ((aRateMultiplier >= 0x01) &&
		  (aRateMultiplier <= 0x18)) {
			m_rateMultiplier = aRateMultiplier;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getStructure(){
		return m_struct;
	}

	public void setStructure(byte aStructure)
                  throws ParameterRangeInvalidException{
		m_struct = aStructure;
	}

	public byte getSymmetry(){
		return m_symmetry;
	}

	public void setSymmetry(byte aSymmetry)
                 throws ParameterRangeInvalidException{
		 if (aSymmetry == 0) {
			m_symmetry = aSymmetry;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}
	
	public boolean getSyncAsyncInd(){
		return m_syncFlag;
	}

	public void setSyncAsyncInd(boolean aSyncAsync){
		m_syncFlag = aSyncAsync;
	}

	public byte getTransferMode(){
		return m_transferMode;
	}

	public void setTransferMode(byte aTransferMode)
                     throws ParameterRangeInvalidException{
		 if ((aTransferMode == TM_CIRCUIT_MODE) ||
		  (aTransferMode == TM_PACKET_MODE)) {
			m_transferMode = aTransferMode;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getUserInfoLayer1Protocol(){
		return m_userInfoLayer1;
	}
		
	public void setUserInfoLayer1Protocol(byte aUserInfoLayer1Protocol)
                               throws ParameterRangeInvalidException{
		 if ((aUserInfoLayer1Protocol >= UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30) &&
		  (aUserInfoLayer1Protocol <= UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING)) {
			m_userInfoLayer1 = aUserInfoLayer1Protocol;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}
		
	public byte getUserInfoLayer2Protocol(){
		return m_userInfoLayer2;
	}

	public void setUserInfoLayer2Protocol(byte aUserInfoLayer2Protocol)
                               throws ParameterRangeInvalidException{
		 if ((aUserInfoLayer2Protocol >= UIL2P_Q921_I441) &&
		  (aUserInfoLayer2Protocol <= UIL2P_X25)) {
			m_userInfoLayer2 = aUserInfoLayer2Protocol;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getUserInfoLayer3Protocol(){
		return m_userInfoLayer3;
	}
	
	public void setUserInfoLayer3Protocol(byte aUserInfoLayer3Protocol)
                               throws ParameterRangeInvalidException{
		 if ((aUserInfoLayer3Protocol >= UIL3P_Q931) &&
		  (aUserInfoLayer3Protocol <= UIL3P_X25)) {
			m_userInfoLayer3 = aUserInfoLayer3Protocol;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}
	
	public byte getUserRate(){
		return m_userRate;
	}

	public void setUserRate(byte aUserRate)
                 throws ParameterRangeInvalidException{
		 if ((aUserRate >= UR_RATE_I460) &&
		  (aUserRate <= UR_12DOT0_KBPS)) {
			m_userRate = aUserRate;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte[] flatUserServiceInfoCommon()
	{

		byte[] rc = ByteArray.getByteArray(UserServiceInfoCommon.PARAM_LENGTH);
		
		byte bandneg=0,duplex=0, ext1=1, ext2=1, ext21=1,ext2a=1;
		byte ext2b=1,ext3=1, ext3a=1,ext3b1=1,ext3b2=1,ext3c=1,ext3d=1,ext4=1,ext5=1;
		byte flowRx=0,flowTx=0,lli=0,mode=0,msgorig=0,multifr=0,neg=0,nicRx=0;
		byte nicTx=0,ratead=0,ratemul=0,syncflg=0;
		
		if(m_bandNegotiationInd == true)
			bandneg = 1;
		if(m_duplexModeInd == true)
			duplex = 1;
		if(m_ext1 == true)
			ext1 = 0;
		if(m_ext2 == true)
			ext2 = 0;
		if(m_ext21 == true)
			ext21 = 0;
		if(m_ext2a == true)
			ext2a = 0;
		if(m_ext2b == true)
			ext2b = 0;
		if(m_ext3 == true)
			ext3 = 0;
		if(m_ext3a == true)
			ext3a = 0;
		if(m_ext3b1 == true)
			ext3b1 = 0;
		if(m_ext3b2 == true)
			ext3b2 = 0;
		if(m_ext3c == true)
			ext3c = 0;
		if(m_ext3d == true)
			ext3d = 0;
		if(m_ext4 == true)
			ext4 = 0;
		if(m_ext5 == true)
			ext5 = 0;
		if(m_flowCntrlRx == true)
			flowRx = 1;
		if(m_flowCntrlTx == true)
			flowTx = 1;
		if(m_lliNegotiationInd == true)
			lli=1;
		if(m_modeInd == true)
			mode = 1;
		if(m_msgOrigInd == true)
			msgorig = 1;
		if(m_multiFrameInd == true)
			multifr = 1;
		if(m_negotiation == true)
			neg = 1;
		if(m_nicRx == true)
			nicRx = 1;
		if(m_nicTx == true)
			nicTx = 1;
		if(m_rateAdaptationInd == true)
			ratead = 1;
		if(m_syncFlag == true)
			syncflg = 1;
		
		
		rc[0] = (byte)(((ext1 << 7) & IsupMacros.L_BIT8_MASK) | 
				((m_codingStd << 5)	& IsupMacros.L_bits76_MASK)|
				((m_infoTransferCap & IsupMacros.L_bits51_MASK)));
		
		rc[1] = (byte)(((ext2 << 7) & IsupMacros.L_BIT8_MASK) |
				((m_transferMode << 5)	& IsupMacros.L_bits76_MASK)|
				((m_infoTransferRate & IsupMacros.L_bits51_MASK)));

		rc[2] = (byte)(((ext2a << 7) & IsupMacros.L_BIT8_MASK) |
				((m_struct << 5) & IsupMacros.L_bits75_MASK)|
				((m_config << 2) & IsupMacros.L_bits43_MASK) |
				((m_establishment & IsupMacros.L_bits21_MASK)));

		rc[3] = (byte)(((ext2b << 7) & IsupMacros.L_BIT8_MASK) |
				((m_symmetry << 5)	& IsupMacros.L_bits76_MASK)|			
				((m_infoTransferRateDestToOrig & IsupMacros.L_bits51_MASK)));
		
		rc[4] = (byte)(((ext3 << 7) & IsupMacros.L_BIT8_MASK) |
				((m_layer1Id << 5)	& IsupMacros.L_bits76_MASK)|	
				((m_userInfoLayer1 & IsupMacros.L_bits51_MASK)));

		rc[5] = (byte)(((ext3a << 7) & IsupMacros.L_BIT8_MASK) |
				((syncflg << 6) & IsupMacros.L_BIT7_MASK) |
				((neg << 5) & IsupMacros.L_BIT6_MASK) |				
				(m_userRate 	& IsupMacros.L_bits51_MASK));

		rc[6] = (byte)(((ext3b1 << 7) & IsupMacros.L_BIT8_MASK) |
				(m_rateMultiplier & IsupMacros.L_bits71_MASK));

		rc[7] = (byte)(((ext3c << 7) & IsupMacros.L_BIT8_MASK) |
				((m_numStopBits << 5) & IsupMacros.L_bits76_MASK) |
				((m_numDataBits << 3) & IsupMacros.L_bits54_MASK) |
				(m_parity & IsupMacros.L_bits31_MASK));
		
		rc[8] = (byte)(((ext3d << 7) & IsupMacros.L_BIT8_MASK) |
				((duplex << 6) & IsupMacros.L_BIT7_MASK) |
				((m_modemType << 5) & IsupMacros.L_BIT6_MASK));

		rc[9] = (byte)(((ext4 << 7) & IsupMacros.L_BIT8_MASK) |
				((m_layer2Id << 5)	& IsupMacros.L_bits76_MASK)|	
				((m_userInfoLayer2 & IsupMacros.L_bits51_MASK)));

		rc[10] = (byte)(((ext5 << 7) & IsupMacros.L_BIT8_MASK) |
				((m_layer3Id << 5)	& IsupMacros.L_bits76_MASK)|	
				((m_userInfoLayer3 & IsupMacros.L_bits51_MASK)));
		
		rc[11] = 0x07;
 
		return rc;

	}

	public void  putUserServiceInfoCommon(byte[] arr, int index, byte par_len){

		byte oct3 = arr[index];
		byte lyr  = arr[index+1];
		byte oct4 = arr[index+2];
		byte oct5 = arr[index+3];
		
		m_infoTransferCap = (byte)(arr[index+4] & IsupMacros.L_bits51_MASK);
		m_codingStd       = (byte)((arr[index+4] >> 5) & IsupMacros.L_bits21_MASK);
		if((byte)((arr[index+4] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
			m_ext1 = false;
		m_infoTransferRate = (byte)(arr[index+5] & IsupMacros.L_bits51_MASK);
		m_transferMode = 	(byte)((arr[index+5] >> 5) & IsupMacros.L_bits21_MASK);
		if((byte)((arr[index+5] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
			m_ext2 = false;
		if(m_ext2 == true){
			m_establishment = (byte)(arr[index+6] & IsupMacros.L_bits21_MASK);
			m_config        = (byte)((arr[index+6]>>2) & IsupMacros.L_bits21_MASK);
			m_struct        = (byte)((arr[index+6]>>4) & IsupMacros.L_bits31_MASK);
			if((byte)((arr[index+6] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
				m_ext2a = false;
				if(m_ext2a == true){
					m_infoTransferRateDestToOrig = (byte)(arr[index+7] & IsupMacros.L_bits51_MASK);
					m_symmetry                   = (byte)((arr[index+7] >> 5) & IsupMacros.L_bits21_MASK);
					if((byte)((arr[index+7] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
						m_ext2b = false;
				}
		}
		
		if(oct3 == 1){
			m_userInfoLayer1 = (byte)(arr[index+8] & IsupMacros.L_bits51_MASK);
			m_layer1Id       = (byte)((arr[index+8] >> 5) & IsupMacros.L_bits21_MASK);
			if((byte)((arr[index+8] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
				m_ext3 = false;
				if(lyr == 1){
					if(m_ext3 == true){
						m_userRate    = (byte)(arr[index+9]&IsupMacros.L_bits51_MASK);
						if((byte)((arr[index+9] >> 5)& IsupMacros.L_BIT1_MASK) == 1 )
							m_negotiation = true;
						if((byte)((arr[index+9] >> 6)& IsupMacros.L_BIT1_MASK) == 1 )
							m_syncFlag = true;
						if((byte)((arr[index+9] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
							m_ext3a = false;
						if(m_ext3a == true){
							m_rateMultiplier = (byte)(arr[index+10] & IsupMacros.L_bits71_MASK); 
							if((byte)((arr[index+10] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
								m_ext3b1 = false;
								if(m_ext3b1 == true){
									m_parity = (byte)(arr[index+11] & IsupMacros.L_bits31_MASK);
									m_numDataBits =
											(byte)((arr[index+11] >> 3)& IsupMacros.L_bits21_MASK);
									m_numStopBits =
											(byte)((arr[index+11] >> 5)& IsupMacros.L_bits21_MASK);
									if((byte)((arr[index+11] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
										m_ext3c = false;
										if(m_ext3c == true){
											m_modemType =  (byte)(arr[index+12] & IsupMacros.L_bits61_MASK);
											if((byte)((arr[index+12] >> 6)& IsupMacros.L_BIT1_MASK) == 1 )
												m_duplexModeInd = true;
											if((byte)((arr[index+12] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
												m_ext3d = false;
										}
								}
						}
				}
			}
		}
		
		if(oct4 == 1){
			m_userInfoLayer2 = (byte)(arr[index+13] &IsupMacros.L_bits51_MASK);
			
			m_layer2Id       = (byte)((arr[index+13] >> 5) &IsupMacros.L_bits21_MASK);
			if((byte)((arr[index+13] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
				m_ext4 = false;
		}
			
		if(oct5 == 1){
			m_userInfoLayer3 = (byte)(arr[index+14] &IsupMacros.L_bits51_MASK);
			
			m_layer3Id       = (byte)((arr[index+14] >> 5) &IsupMacros.L_bits21_MASK);
			
			if((byte)((arr[index+14] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )	
				m_ext5 = false;
			
		}	

	}

	/**
    * String representation of class UserServiceInfoCommon
    *
    * @return    String provides description of class UserServiceInfoCommon
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nbandNegotiationInd = ");
				buffer.append(m_bandNegotiationInd);
				buffer.append("\ncodingStd = ");
				buffer.append(m_codingStd);
				buffer.append("\nconfig = ");
				buffer.append(m_config);
				buffer.append("\nduplexModeInd = ");
				buffer.append(m_duplexModeInd);
				buffer.append("\nestablishment = ");
				buffer.append(m_establishment);
				buffer.append("\next1 = ");
				buffer.append(m_ext1);
				buffer.append("\next2 = ");
				buffer.append(m_ext2);
				buffer.append("\next21 = ");
				buffer.append(m_ext21);
				buffer.append("\next2a = ");
				buffer.append(m_ext2a);
				buffer.append("\next2b = ");
				buffer.append(m_ext2b);
				buffer.append("\next3 = ");
				buffer.append(m_ext3);
				buffer.append("\next3a = ");
				buffer.append(m_ext3a);
				buffer.append("\next3b1 = ");
				buffer.append(m_ext3b1);
				buffer.append("\next3b2 = ");
				buffer.append(m_ext3b2);
				buffer.append("\next3c = ");
				buffer.append(m_ext3c);
				buffer.append("\next3d = ");
				buffer.append(m_ext3d);
				buffer.append("\next4 = ");
				buffer.append(m_ext4);
				buffer.append("\next5 = ");
				buffer.append(m_ext5);
				buffer.append("\nflowCntrlRx = ");
				buffer.append(m_flowCntrlRx);
				buffer.append("\nflowCntrlTx = ");
				buffer.append(m_flowCntrlTx);
				buffer.append("\ninfoTransferCap = ");
				buffer.append(m_infoTransferCap);
				buffer.append("\ninfoTransferRate = ");
				buffer.append(m_infoTransferRate);
				buffer.append("\ninfoTransferRateDestToOrig = ");
				buffer.append(m_infoTransferRateDestToOrig);
				buffer.append("\nintermediateRate = ");
				buffer.append(m_intermediateRate);
				buffer.append("\nlayer1Id = ");
				buffer.append(m_layer1Id);
				buffer.append("\nlayer2Id = ");
				buffer.append(m_layer2Id);
				buffer.append("\nlayer3Idn = ");
				buffer.append(m_layer3Id);
				buffer.append("\nlliNegotiationInd = ");
				buffer.append(m_lliNegotiationInd);
				buffer.append("\nmodeInd = ");
				buffer.append(m_modeInd);
				buffer.append("\nmodemType = ");
				buffer.append(m_modemType);
				buffer.append("\nmsgOrigInd = ");
				buffer.append(m_msgOrigInd);
				buffer.append("\nmultiFrameInd = ");
				buffer.append(m_multiFrameInd);
				buffer.append("\nnegotiation = ");
				buffer.append(m_negotiation);
				buffer.append("\nnicRx = ");
				buffer.append(m_nicRx);
				buffer.append("\nnicTx = ");
				buffer.append(m_nicTx);
				buffer.append("\nnumDataBits = ");
				buffer.append(m_numDataBits);
				buffer.append("\nnumStopBits = ");
				buffer.append(m_numStopBits);
				buffer.append("\nparity = ");
				buffer.append(m_parity);
				buffer.append("\nrateAdaptationInd = ");
				buffer.append(m_rateAdaptationInd);
				buffer.append("\nrateMultiplier = ");
				buffer.append(m_rateMultiplier);
				buffer.append("\nstruct = ");
				buffer.append(m_struct);
				buffer.append("\nsymmetry = ");
				buffer.append(m_symmetry);
				buffer.append("\nsyncFlag = ");
				buffer.append(m_syncFlag);
				buffer.append("\ntransferMode = ");
				buffer.append(m_transferMode);
				buffer.append("\nuserInfoLayer1 = ");
				buffer.append(m_userInfoLayer1);
				buffer.append("\nuserInfoLayer1 = ");
				buffer.append(m_userInfoLayer1);
				buffer.append("\nuserInfoLayer2 = ");
				buffer.append(m_userInfoLayer2);
				buffer.append("\nuserInfoLayer3 = ");
				buffer.append(m_userInfoLayer3);
				buffer.append("\nuserRate = ");
				buffer.append(m_userRate);
				return buffer.toString();
		
		}		
	
	
	boolean m_bandNegotiationInd;
	byte    m_codingStd;
	byte    m_config;
	boolean m_duplexModeInd;
	byte    m_establishment;
	boolean m_ext1;
	boolean m_ext2;
	boolean m_ext21;
	boolean m_ext2a;
	boolean m_ext2b;
	boolean m_ext3;
	boolean m_ext3a;
	boolean m_ext3b1;
	boolean m_ext3b2;
	boolean m_ext3c;
	boolean m_ext3d;
	boolean m_ext4;
	boolean m_ext5;
	boolean m_flowCntrlRx;
	boolean m_flowCntrlTx;
	byte    m_infoTransferCap;
	byte    m_infoTransferRate;
	byte    m_infoTransferRateDestToOrig;
	byte    m_intermediateRate;
	byte    m_layer1Id;
	byte    m_layer2Id;
	byte    m_layer3Id;
	boolean m_lliNegotiationInd;
	boolean m_modeInd;
	byte    m_modemType;
	boolean m_msgOrigInd;
	boolean m_multiFrameInd;
	boolean m_negotiation;
	boolean m_nicRx;
	boolean m_nicTx;
	byte    m_numDataBits;
	byte    m_numStopBits;
	byte    m_parity;
	boolean m_rateAdaptationInd;
	byte    m_rateMultiplier;
	byte    m_struct;
	byte    m_symmetry;
	boolean m_syncFlag;
	byte    m_transferMode;
	byte    m_userInfoLayer1;
	byte    m_userInfoLayer2;
	byte    m_userInfoLayer3;
	byte    m_userRate;

	public static final byte PARAM_LENGTH = 12;

	public static final boolean EI_NEXT_OCTET_EXIST = false; 
	public static final boolean EI_LAST_OCTET = true; 
	public static final byte CS_ITU_STANDARD = 0x00; 
	public static final byte CS_ISO_IEC_STANDARD = 0x01; 
	public static final byte CS_NATIONAL_STANDARD = 0x02; 
	public static final byte CS_STANDARD_FOR_NETWORK = 0; /* Value not found in the specs */
	public static final byte ITC_SPEECH = 0x00; 
	public static final byte ITC_UNRESTRICTED_DIGITAL_INFORMATION = 0x08; 
	public static final byte ITC_RESTRICTED_DIGITAL_INFORMATION = 0x09; 
	public static final byte ITC_3DOT1_KHZ_AUDIO = 0x10; 
	public static final byte ITC_UNRESTRICTED_DIGITAL_INFORMATION_WITH_TONES_ANNOUNCEMENTS = 0x11; 
	public static final byte ITC_VIDEO = 0x18; 
	public static final byte TM_CIRCUIT_MODE = 0x00; 
	public static final byte TM_PACKET_MODE = 0x02; 
	public static final byte ITR_64_KBPS = 0x10; 
	public static final byte ITR_2_by_64_KBPS = 0x07; 
	public static final byte ITR_384_KBPS = 0x13; 
	public static final byte ITR_1472_KBPS = 0x14; 
	public static final byte ITR_1536_KBPS = 0x15; 
	public static final byte ITR_1920_KBPS = 0x17; 
	public static final byte ITR_MULTI_RATE = 0x18; 
	public static final byte STRUCTURE_DEFAULT = 0x00; 
	public static final byte STRUCTURE_8_KHZ_INTEGRITY = 0x01; 
	public static final byte STRUCTURE_SERVICE_DATA_UNIT_INTEGRITY = 0x04; 
	public static final byte STRUCTURE_UNSTRUCTURED = 0x07; 
	public static final byte CONFIGURATION_POINT_TO_POINT = 0x00; 
	public static final byte ESTABLISHMENT_DEMAND = 0x00; 
	public static final byte SYMMETRY_BIDIRECTIONAL_SYMMETRIC = 0x00; 
	public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30 = 0x01; 
	public static final byte UIL1P_RECOMMENDATION_G711_U_LA = 0x02; 
	public static final byte UIL1P_RECOMMENDATION_G711_A_LAW = 0; /* Value not found in the specs */
	public static final byte UIL1P_RECOMMENDATION_G721_I460 = 0; /* Value not found in the specs */
	public static final byte UIL1P_RECOMMENDATION_H221_H242 = 0; /* Value not found in the specs */
	public static final byte UIL1P_RECOMMENDATION_G722_G725_7KHZ_AUDIO = 0x05; /* Value not found in the specs */
	public static final byte UIL1P_RECOMMENDATION_H223_H245 = 0; /* Value not found in the specs */
	public static final byte UIL1P_NON_ITU_STANDARDIZED_RATE_ADAPTION = 0x07; 
	public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V120 = 0x08; 
	public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING = 0x09; 
	public static final boolean SYN_SYNCHRONOUS = false; 
	public static final boolean SYN_ASYNCHRONOUS = true; 
	public static final boolean NEGOT_INBAND_NEGOTIATION_IMPOSSIBLE = false; 
	public static final boolean NEGOT_INBAND_NEGOTIATION_POSSIBLE = true;
	public static final byte UR_RATE_I460 = 0x00; 
	public static final byte UR_0DOT6_KBPS = 0x01; 
	public static final byte UR_1DOT2_KBPS = 0x02; 
	public static final byte UR_2DOT4_KBPS = 0x03; 
	public static final byte UR_3DOT6_KBPS = 0x04; 
	public static final byte UR_4DOT8_KBPS = 0x05; 
	public static final byte UR_7DOT2_KBPS = 0x06; 
	public static final byte UR_8DOT0_KBPS = 0x07; 
	public static final byte UR_9DOT6_KBPS = 0x08; 
	public static final byte UR_14DOT4_KBPS = 0x09; 
	public static final byte UR_16DOT0_KBPS = 0x0A; 
	public static final byte UR_19DOT2_KBPS = 0x0B; 
	public static final byte UR_32DOT0_KBPS = 0x0C; 
	public static final byte UR_38DOT4_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_48DOT0_KBPS = 0x0E; 
	public static final byte UR_56DOT0_KBPS = 0x0F; 
	public static final byte UR_64DOT0_KBPS = 0x10; 
	public static final byte UR_57DOT6_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_28DOT8_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_24DOT0_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_0DOT1345_KBPS = 0x15; 
	public static final byte UR_0DOT100_KBPS = 0x16; 
	public static final byte UR_FWD_0DOT075_KBPS = 0x17; 
	public static final byte UR_BWD_1DOT2_KBPS = 0x18; 
	public static final byte UR_BWD_0DOT075_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_FWD_1DOT2_KBPS = 0; /* Value not found in the specs */
	public static final byte UR_0DOT050_KBPS = 0x19; 
	public static final byte UR_0DOT075_KBPS = 0x20; 
	public static final byte UR_0DOT110_KBPS = 0x1B; 
	public static final byte UR_0DOT150_KBPS = 0x1C; 
	public static final byte UR_0DOT200_KBPS = 0x1D; 
	public static final byte UR_0DOT300_KBPS = 0x1E; 
	public static final byte UR_12DOT0_KBPS = 0x1F; 
	public static final byte IR_NOT_USED = 0x00; 
	public static final byte IR_8_KBPS = 0x20; 
	public static final byte IR_16_KBPS = 0x40; 
	public static final byte IR_32_KBPS = 0x60; 
	public static final boolean NICTX_NOT_REQUIRED = false; 
	public static final boolean NICTX_REQUIRED = true; 
	public static final boolean NICRX_NOT_ACCEPT = false; 
	public static final boolean NICRX_ACCEPT = true;
	public static final boolean FCTX_NOT_REQUIRED = false; 
	public static final boolean FCTX_REQUIRED = true; 
	public static final boolean FCRX_NOT_ACCEPT = false; 
	public static final boolean FCRX_ACCEPT = true; 
	public static final boolean RAH_NOT_INCLUDED = false; 
	public static final boolean RAH_INCLUDED = true; 
	public static final boolean MFES_NOT_SUPPORTED = false; 
	public static final boolean MFES_SUPPORTED = true; 
	public static final boolean MO_BIT_TRANSPARENT = false; 
	public static final boolean MO_PROTOCOL_SENSITIVE = true; 
	public static final boolean LLIN_DEFAULT = false; 
	public static final boolean LLIN_NEGOTIATION = true; 
	public static final boolean AA_DEFAULT_ASSIGNEE = false; 
	public static final boolean AA_ASSIGNOR_ONLY = true; 
	public static final boolean ION_UNACCEPTABLE = false; 
	public static final boolean ION_NEGOTIATION_DONE_INBAND_USING_LOGICAL_LINK_ZERO = true; 
	public static final byte NSB_NOT_USED = 0x00; 
	public static final byte NSB_1_bit = 0x01; 
	public static final byte NSB_1DOT5_bit = 0x02; 
	public static final byte NSB_2_bit = 0x03; 
	public static final byte NDBEP_NOT_USED = 0x00; 
	public static final byte NDBEP_5_BIT = 0x01; 
	public static final byte NDBEP_7_BIT = 0x02; 
	public static final byte NDBEP_8_BIT = 0x03; 
	public static final byte PI_ODD = 0x00; 
	public static final byte PI_EVEN = 0x02; 
	public static final byte PI_NONE = 0x03; 
	public static final byte PI_FORCED_TO_0 = 0x04; 
	public static final byte PI_FORCED_TO_1 = 0x05; 
	public static final boolean DM_HALF_DUPLEX = false; 
	public static final boolean DM_FULL_DUPLEX = true; 
	public static final byte UIL2P_Q921_I441 = 0x01; 
	public static final byte UIL2P_T1DOT602 = 0; /*value not available */
	public static final byte UIL2P_X25 = 0x06; 
	public static final byte UIL2P_LAN_LOGIC_CONTROL = 0; /* Value not found in the specs */
	public static final byte UIL3P_Q931 = 0x02; 
	public static final byte UIL3P_T1DOT607 = 0x02; 
	public static final byte UIL3P_X25 = 0x06; 
	public static final byte UIL3P_ISO_IEC_TR_9577 = 0; /* Value not found in the specs */
}




