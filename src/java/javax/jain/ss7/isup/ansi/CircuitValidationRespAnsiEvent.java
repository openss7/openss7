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
* DATE         : $Date: 2008/05/16 12:24:09 $
* 
* MODULE NAME  : $RCSfile: CircuitValidationRespAnsiEvent.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class CircuitValidationRespAnsiEvent extends IsupEvent{

	public CircuitValidationRespAnsiEvent(java.lang.Object source,
                                      SignalingPointCode dpc,
                                      SignalingPointCode opc,
                                      byte sls,
                                      int cic,
                                      byte congestionPriority,
                                      short in_circuitValResp,
                                      CircuitGrpCharacteristicsIndAnsi in_circuitGrpCharacteristicsIndAnsi)
                               throws ParameterRangeInvalidException{


		super(source,dpc,opc,sls,cic,congestionPriority);
		cktGrpCharacteristicsIndAnsi = in_circuitGrpCharacteristicsIndAnsi;
		cktValResp                   = in_circuitValResp;
		m_cktValRespPresent          = true;
		m_cktGrpCharacteristicsIndAnsiPresent     = true;

	}

	public void checkMandatoryParameters()
                              throws MandatoryParameterNotSetException{
		if((m_cktValRespPresent == true) &&
			(m_cktGrpCharacteristicsIndAnsiPresent == true))	
			return;
		else throw new MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_RESP;
	}
		
	public CircuitGrpCharacteristicsIndAnsi getCircuitGrpCharacteristicsInd()
                                                                 throws MandatoryParameterNotSetException{
		if(m_cktGrpCharacteristicsIndAnsiPresent == true)
			return cktGrpCharacteristicsIndAnsi;
		else throw new MandatoryParameterNotSetException();
	}

	public void setCircuitGrpCharacteristicsInd(CircuitGrpCharacteristicsIndAnsi in_circuitGrpCharacteristicsIndAnsi){
		cktGrpCharacteristicsIndAnsi = in_circuitGrpCharacteristicsIndAnsi;
		m_cktGrpCharacteristicsIndAnsiPresent     = true;
	}

	public CircuitIdNameAnsi getCircuitIdName()
                                   throws ParameterNotSetException{
		if(isCktIdNameAnsi	== true)
			return cktIdNameAnsi;
		else throw new ParameterNotSetException();
	}

	public void setCircuitIdName(CircuitIdNameAnsi in_circuitIdName){
		cktIdNameAnsi   = in_circuitIdName;
		isCktIdNameAnsi	= true;
	}

	public boolean isCircuitIdNamePresent(){
		return isCktIdNameAnsi;
	}

	public short getCircuitValidationResp(){
		return cktValResp;
	}

	public void setCircuitValidationResp(short in_cktValResp)
                              throws ParameterRangeInvalidException{
		cktValResp = in_cktValResp;
	}
		
	public CommonLangLocationIdAnsi getCommonLangLocationId()
                                                 throws ParameterNotSetException{
		if(isCommonLangLocationIdAnsi == true)
			return commonLangLocationIdAnsi;
		else throw new ParameterNotSetException();
	}

	public void setCommonLangLocationId
		(CommonLangLocationIdAnsi in_commonLangLocationIdAnsi){
		commonLangLocationIdAnsi   = in_commonLangLocationIdAnsi;
		isCommonLangLocationIdAnsi = true;
	}

	public boolean isCommonLangLocationIdPresent(){
		return isCommonLangLocationIdAnsi;
	}


	/**
    * String representation of class CircuitValidationRespAnsiEvent
    *
    * @return    String provides description of class CircuitValidationRespAnsiEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncktGrpCharacteristicsIndAnsi = ");
				buffer.append(cktGrpCharacteristicsIndAnsi);
				buffer.append("\n\nisCktIdNameAnsi = ");
				buffer.append(isCktIdNameAnsi);
				buffer.append("\ncktIdNameAnsi = ");
				buffer.append(cktIdNameAnsi);
				buffer.append("\n\ncktValResp = ");
				buffer.append(cktValResp);
				buffer.append("\n\nisCommonLangLocationIdAnsi = ");
				buffer.append(isCommonLangLocationIdAnsi);
				buffer.append("\n\ncommonLangLocationIdAnsi = ");
				buffer.append(commonLangLocationIdAnsi);
				return buffer.toString();
		
		}



	
	CircuitGrpCharacteristicsIndAnsi cktGrpCharacteristicsIndAnsi;
	CircuitIdNameAnsi                cktIdNameAnsi;
	short                            cktValResp;
	CommonLangLocationIdAnsi         commonLangLocationIdAnsi;
	boolean                          isCktIdNameAnsi;
	boolean                          isCommonLangLocationIdAnsi;

	protected boolean m_cktValRespPresent     = false;
	protected boolean m_cktGrpCharacteristicsIndAnsiPresent     = false;
}	


