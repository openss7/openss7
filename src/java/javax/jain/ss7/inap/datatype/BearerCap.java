/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : BearerCap.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This Class defines the BearerCap Datatype. (Encoded as in Q.931)
*/


public class BearerCap implements java.io.Serializable 
{

    private int codingStandard;
    
    private int informationTransferCapability;
    
    private int transferMode ;
    
    private int informationTransferRate ;
    
    private int rateMultiplier ;
    
    private int userInformationLayer1Protocol;
    
    private boolean synchronousAsynchronousIndicator;
    
    private boolean negotiationIndicator;
    
    private int userRate;
    
    private int intermediateRate;
    
    private boolean networkIndependentClockOnTransmissionIndicator;
    
    private boolean networkIndependentClockOnReceptionIndicator;
    
    private boolean flowControlOnTransmissionIndicator;
    
    private boolean flowControlOnReceptionIndicator;
    
    private boolean rateAdaptionHeaderIndicator;
    
    private boolean multipleFrameEstablishmentSupportInDataLinkIndicator;
    
    private boolean modeOfOperationIndicator;
    
    private boolean logicalLinkIdentifierNegotiationIndicator;
    
    private boolean assignorAssigneeIndicator;
    
    private boolean inBandOutBandNegotiationIndicator;
    
    private int numberOfStopBits;
    
    private int numberOfDataBits;
    
    private int parityInformation;
    
    private int modeDuplex;
    
    private int modemType;
    
    private int userInformationLayer2Protocol;
    
    private int userInformationLayer3Protocol;
    
    private boolean isRateMultiplier=false;

	 private boolean isSynchronousAsynchronousIndicator =false ;
    
    private boolean isNegotiationIndicator =false ;
    
    private boolean isUserRate =false ;
    
	 private boolean isNumberOfStopBits =false ;
    
    private boolean isNumberOfDataBits =false ;
    
    private boolean isParityInformation =false ;
    
    private boolean isModeDuplex =false ;
    
    private boolean isModemType =false ;
  
    

/**
Gets Coding Standard
*/

    public int getCodingStandard()
    {
        return codingStandard ;
    }

/**
Sets Coding Standard
*/

    public void  setCodingStandard (int codingStandard)
    {
        this.codingStandard = codingStandard ;
    }

//--------------------------------------------------------------------------

/**
Gets Information Transfer Capability
*/

    public int getInformationTransferCapability()
    {
        return informationTransferCapability ;
    }

/**
Sets Information Transfer Capability
*/

    public void  setInformationTransferCapability (int informationTransferCapability)
    {
        this.informationTransferCapability = informationTransferCapability ;
    }

//--------------------------------------------------------------------------

/**
Gets Transfer Mode
*/

    public int getTransferMode()
    {
        return transferMode ;
    }

/**
Sets Transfer Mode
*/

    public void  setTransferMode (int transferMode)
    {
         this.transferMode = transferMode ;
    }

//--------------------------------------------------------------------------

/**
Gets informationTransferRate
*/

    public int getInformationTransferRate()
    {
        return informationTransferRate;
    }

/**
Sets Information Transfer Rate
*/

    public void  setInformationTransferRate (int informationTransferRate)
    {
        this.informationTransferRate = informationTransferRate;
    }

//--------------------------------------------------------------------------

/**
Gets Rate Multiplier
*/

    public int getRateMultiplier() throws ParameterNotSetException
    {
        if(isRateMultiplierPresent())
		{
         	return rateMultiplier ;
        }
		else
		{
               throw new ParameterNotSetException();
        }                  
    }

/**
Sets Rate Multiplier
*/

    public void  setRateMultiplier (int rateMultiplier)
    {
        this.rateMultiplier = rateMultiplier ;
        isRateMultiplier=true;
    }



/**
Indicates if the Rate Multiplier optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isRateMultiplierPresent()
    {
        return isRateMultiplier;
    }



//--------------------------------------------------------------------------

/**
Gets User Information Layer 1 Protocol
*/

    public int getUserInformationLayer1Protocol()
    {
        return userInformationLayer1Protocol;
    }

/**
Sets User Information Layer 1 Protocol
*/

    public void  setUserInformationLayer1Protocol (int userInformationLayer1Protocol)
    {
    	this.userInformationLayer1Protocol = userInformationLayer1Protocol ;
    }

//--------------------------------------------------------------------------

/**
Gets Synchronous Asynchronous Indicator
*/

    public boolean getSynchronousAsynchronousIndicator()
    {
        return synchronousAsynchronousIndicator;
    }

/**
Sets Synchronous Asynchronous Indicator
*/

    public void  setSynchronousAsynchronousIndicator (boolean synchronousAsynchronousIndicator )
    {
        this.synchronousAsynchronousIndicator = synchronousAsynchronousIndicator ;
        isSynchronousAsynchronousIndicator=true;
    }
/**
Indicates if the Synchronous Asynchronous Indicator optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isSynchronousAsynchronousIndicatorPresent()
    {
        return isSynchronousAsynchronousIndicator;
    }


//--------------------------------------------------------------------------

/**
Gets Negotiation Indicator
*/

    public boolean getNegotiationIndicator()
    {
        return negotiationIndicator;
    }

/**
Sets Negotiation Indicator
*/

    public void  setNegotiationIndicator (boolean negotiationIndicator)
    {
        this.negotiationIndicator = negotiationIndicator ;
        isNegotiationIndicator=true;

    }

/**
Indicates if the Negotiation Indicator optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isNegotiationIndicatorPresent()
    {
        return isNegotiationIndicator;
    }

//--------------------------------------------------------------------

/**
Gets User Rate
*/

    public int getUserRate()
    {
        return userRate;
    }

/**
Sets User Rate
*/

    public void  setUserRate (int userRate)
    {
        this.userRate = userRate ;
        isUserRate=true;
    }

/**
Indicates if the User Rate optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isUserRatePresent()
    {
        return isUserRate;
    }


//--------------------------------------------------------------------

/**
Gets Intermediate Rate
*/

    public int getIntermediateRate()
    {
        return intermediateRate;
    }

/**
Sets Intermediate Rate
*/

    public void  setIntermediateRate (int intermediateRate)
    {
        this.intermediateRate = intermediateRate ;
    }

//--------------------------------------------------------------------

/**
Gets Network Independent Clock On Transmission Indicator
*/

    public boolean getNetworkIndependentClockOnTransmissionIndicator()
    {
        return networkIndependentClockOnTransmissionIndicator;
    }

/**
Sets Network Independent Clock On Transmission Indicator
*/

    public void  setNetworkIndependentClockOnTransmissionIndicator (boolean networkIndependentClockOnTransmissionIndicator)
    {
        this.networkIndependentClockOnTransmissionIndicator = networkIndependentClockOnTransmissionIndicator ;
    }

//--------------------------------------------------------------------

/**
Gets Network Independent Clock On Reception Indicator
*/

    public boolean getNetworkIndependentClockOnReceptionIndicator()
    {
        return networkIndependentClockOnReceptionIndicator;
    }

/**
Sets Network Independent Clock On Reception Indicator
*/

    public void  setNetworkIndependentClockOnReceptionIndicator (boolean networkIndependentClockOnReceptionIndicator)
    {
        this.networkIndependentClockOnReceptionIndicator = networkIndependentClockOnReceptionIndicator ;
    }
 
//--------------------------------------------------------------------

/**
Gets Flow Control On Transmission Indicator
*/

    public boolean getFlowControlOnTransmissionIndicator()
    {
        return flowControlOnTransmissionIndicator;
    }

/**
Sets Flow Control On Transmission Indicator
*/

    public void  setFlowControlOnTransmissionIndicator (boolean flowControlOnTransmissionIndicator)
    {
        this.flowControlOnTransmissionIndicator = flowControlOnTransmissionIndicator ;
    }

//--------------------------------------------------------------------

/**
Gets Flow Control On Reception Indicator
*/

    public boolean getFlowControlOnReceptionIndicator()
    {
        return flowControlOnReceptionIndicator;
    }

/**
Sets Flow Control On Reception Indicator
*/

    public void  setFlowControlOnReceptionIndicator (boolean flowControlOnReceptionIndicator)
    {
        this.flowControlOnReceptionIndicator = flowControlOnReceptionIndicator ;
    }

//--------------------------------------------------------------------

/**
Gets Rate Adaption Header Indicator
*/

    public boolean getRateAdaptionHeaderIndicator()
    {
        return rateAdaptionHeaderIndicator;
    }

/**
Sets Rate Adaption Header Indicator
*/

    public void  setRateAdaptionHeaderIndicator (boolean rateAdaptionHeaderIndicator)
    {
        this.rateAdaptionHeaderIndicator = rateAdaptionHeaderIndicator ;
    }
//--------------------------------------------------------------------

/**
Gets Multiple Frame Establishment Support In Data Link Indicator
*/

    public boolean getMultipleFrameEstablishmentSupportInDataLinkIndicator()
    {
        return multipleFrameEstablishmentSupportInDataLinkIndicator;
    }

/**
Sets Multiple Frame Establishment Support In Data Link Indicator
*/

    public void  setMultipleFrameEstablishmentSupportInDataLinkIndicator (boolean multipleFrameEstablishmentSupportInDataLinkIndicator)
    {
        this.multipleFrameEstablishmentSupportInDataLinkIndicator = multipleFrameEstablishmentSupportInDataLinkIndicator ;
    }


//--------------------------------------------------------------------

/**
Gets Mode Of Operation Indicator
*/

    public boolean getModeOfOperationIndicator()
    {
        return modeOfOperationIndicator;
    }

/**
Sets Mode Of Operation Indicator
*/

    public void  setModeOfOperationIndicator (boolean modeOfOperationIndicator)
    {
        this.modeOfOperationIndicator = modeOfOperationIndicator ;
    }
//--------------------------------------------------------------------

/**
Gets Logical Link Identifier Negotiation Indicator
*/

    public boolean getLogicalLinkIdentifierNegotiationIndicator()
    {
        return logicalLinkIdentifierNegotiationIndicator;
    }

/**
Sets Logical Link Identifier Negotiation Indicator
*/

    public void  setLogicalLinkIdentifierNegotiationIndicator (boolean logicalLinkIdentifierNegotiationIndicator)
    {
        this.logicalLinkIdentifierNegotiationIndicator = logicalLinkIdentifierNegotiationIndicator ;
    }
//--------------------------------------------------------------------

/**
Gets Assignor Assignee Indicator
*/

    public boolean getAssignorAssigneeIndicator()
    {
        return assignorAssigneeIndicator;
    }

/**
Sets Assignor Assignee Indicator
*/

    public void  setAssignorAssigneeIndicator (boolean assignorAssigneeIndicator)
    {
        this.assignorAssigneeIndicator = assignorAssigneeIndicator ;
    }
//--------------------------------------------------------------------

/**
Gets In-Band / Out-Band Negotiation Indicator
*/

    public boolean getInBandOutBandNegotiationIndicator()
    {
        return inBandOutBandNegotiationIndicator;
    }

/**
Sets In-Band / Out-Band Negotiation Indicator
*/

    public void  setInBandOutBandNegotiationIndicator (boolean inBandOutBandNegotiationIndicator)
    {
        this.inBandOutBandNegotiationIndicator = inBandOutBandNegotiationIndicator ;
    }

//--------------------------------------------------------------------

/**
Gets Number Of Stop Bits
*/

    public int getNumberofstopbits()
    {
        return numberOfStopBits;
    }

/**
Sets Number Of Stop Bits
*/

    public void  setNumberOfStopBits(int numberOfStopBits)
    {
        this.numberOfStopBits = numberOfStopBits;
        isNumberOfStopBits=true;
    }

/**
Indicates if the Number Of Stop Bits optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isNumberOfStopBitsPresent()
    {
        return isNumberOfStopBits;
    }


//--------------------------------------------------------------------

/**
Gets Number Of Data Bits Excluding Parity Bit If Present
*/

    public int getNumberofDataBits()
    {
        return numberOfDataBits;
    }

/**
Sets Number Of Data Bits Excluding Parity Bit If Present
*/

    public void  setNumberOfDataBits(int numberOfDataBits)
    {
        this.numberOfDataBits = numberOfDataBits;
        isNumberOfDataBits=true;

    }

/**
Indicates if the Number Of Data Bits optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isNumberOfDataBitsPresent()
    {
        return isNumberOfDataBits;
    }

//--------------------------------------------------------------------

/**
Gets Parity Information
*/

    public int getParityInformation()
    {
        return parityInformation;
    }

/**
Sets Parity Information
*/

    public void  setParityInformation(int parityInformation)
    {
        this.parityInformation = parityInformation;
        isParityInformation=true;

    }

/**
Indicates if the Parity Information optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isParityInformationPresent()
    {
        return isParityInformation;
    }



//--------------------------------------------------------------------

/**
Gets Mode Duplex
*/

    public int getModeDuplex()
    {
        return modeDuplex;
    }

/**
Sets Mode Duplex
*/

    public void  setModeDuplex(int modeDuplex)
    {
        this.modeDuplex = modeDuplex ;
        isModeDuplex=true;

    }

/**
Indicates if the Mode Duplex optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isModeDuplexPresent()
    {
        return isModeDuplex;
    }



//--------------------------------------------------------------------

/**
Gets Modem Type
*/

    public int getModemType()
    {
        return modemType;
    }

/**
Sets Modem Type
*/

    public void  setModemType(int modemType)
    {
        this.modemType = modemType ;
        isModemType=true;
    }

/**
Indicates if the Modem Type optional  parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isModemTypePresent()
    {
        return isModemType;
    }


//--------------------------------------------------------------------

/**
Gets User Information Layer 2 Protocol
*/

    public int getUserInformationLayer2Protocol()
    {
        return userInformationLayer2Protocol;
    }

/**
Sets User Information Layer 2 Protocol
*/

    public void  setUserInformationLayer2Protocol (int userInformationLayer2Protocol)
    {
        this.userInformationLayer2Protocol = userInformationLayer2Protocol ;
    }

//--------------------------------------------------------------------

/**
Gets User Information Layer 3 Protocol
*/

    public int getUserInformationLayer3Protocol()
    {
        return  userInformationLayer3Protocol;
    }

/**
Sets User Information Layer 3 Protocol
*/

    public void  setUserInformationLayer3Protocol (int userInformationLayer3Protocol)
    {
         this.userInformationLayer3Protocol = userInformationLayer3Protocol ;
    }
//--------------------------------------------------------------------


}

