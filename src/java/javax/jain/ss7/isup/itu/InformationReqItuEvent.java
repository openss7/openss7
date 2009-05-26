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
* DATE         : $Date: 2008/05/16 12:23:56 $
* 
* MODULE NAME  : $RCSfile: InformationReqItuEvent.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class InformationReqItuEvent extends InformationReqEvent{

	public InformationReqItuEvent(java.lang.Object source,
                              SignalingPointCode dpc,
                              SignalingPointCode opc,
                              byte sls,
                              int cic,
                              byte congestionPriority,
                              InfoReqInd in_infoReqInd)
                       throws ParameterRangeInvalidException{
	
		super(source,dpc,opc,sls,cic,congestionPriority,in_infoReqInd);

	}

	public NwSpecificFacItu getNwSpecificFac()
                                  throws ParameterNotSetException{
		if(isNwSpecificFacItu == true)
			return nwSpecificFacItu;
		else throw new ParameterNotSetException();
	}

	public void setNwSpecificFac(NwSpecificFacItu nsf){
		nwSpecificFacItu   = nsf; 
		isNwSpecificFacItu = true;
	}
		
	public boolean isNwSpecificFacPresent(){
		return isNwSpecificFacItu;
	}

	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                                    throws ParameterNotSetException{
		if(isParamCompatibilityInfoItu == true)
			return paramCompatibilityInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci){
		paramCompatibilityInfoItu   = pci;
		isParamCompatibilityInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompatibilityInfoItu;
	}

	/**
    * String representation of class InformationReqItuEvent
    *
    * @return    String provides description of class InformationReqItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisNwSpecificFacItu = ");
				buffer.append(isNwSpecificFacItu);
				buffer.append("\nnwSpecificFacItu = ");
				buffer.append(nwSpecificFacItu);
				buffer.append("\n\nisParamCompatibilityInfoItu = ");
				buffer.append(isParamCompatibilityInfoItu);
				buffer.append("\nparamCompatibilityInfoItu  = ");
				buffer.append(paramCompatibilityInfoItu);			
				return buffer.toString();
		
		}

	
	boolean                   isNwSpecificFacItu;
	boolean                   isParamCompatibilityInfoItu;
	NwSpecificFacItu          nwSpecificFacItu;
	ParamCompatibilityInfoItu paramCompatibilityInfoItu;


	


    
}


