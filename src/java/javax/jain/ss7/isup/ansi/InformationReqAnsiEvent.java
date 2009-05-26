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
* DATE         : $Date: 2008/05/16 12:24:11 $
* 
* MODULE NAME  : $RCSfile: InformationReqAnsiEvent.java,v $
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

public class InformationReqAnsiEvent extends InformationReqEvent{

	public InformationReqAnsiEvent(java.lang.Object source,
                               SignalingPointCode dpc,
                               SignalingPointCode opc,
                               byte sls,
                               int cic,
                               byte congestionPriority,
                               InfoReqIndAnsi informationReqInd)
                        throws ParameterRangeInvalidException{
	


		super(source,dpc,opc,sls,cic,congestionPriority,informationReqInd);		
	}

	public ConReq getConReq() throws ParameterNotSetException{
		if(isConReq == true)
			return conReq;
		else throw new ParameterNotSetException();

	}

	public void setConReq(ConReq connectionReq){
		conReq   = connectionReq;
		isConReq = true;
	}

	public boolean isConReqPresent(){
		return isConReq;
	}

	public byte[] getNetworkTransport()
                           throws ParameterNotSetException{

		if(isNetworkTransport == true)
			return networkTransport;
		else throw new ParameterNotSetException();

	}

	public void setNetworkTransport(byte[] ntp){
		networkTransport   = ntp;
		isNetworkTransport = true;
	}

	public boolean isNetworkTransportPresent(){
		return isNetworkTransport;
	}

	/**
    * String representation of class InformationReqAnsiEvent
    *
    * @return    String provides description of class InformationReqAnsiEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisConReq = ");
				buffer.append(isConReq);
				buffer.append("\nconReq = ");
				buffer.append(conReq);
				buffer.append("\n\nisNetworkTransport = ");
				buffer.append(isNetworkTransport);
				if(isNetworkTransport == true){
					buffer.append("\nnetworkTransport = ");
					for(i=0;i<networkTransport.length;i++)
						buffer.append(" "+Integer.toHexString((int)(networkTransport[i] & 0xFF)));		
				}
				return buffer.toString();
		
		}


	ConReq  conReq;
	boolean isConReq;
	boolean isNetworkTransport;
	byte[]  networkTransport;
	
}	


