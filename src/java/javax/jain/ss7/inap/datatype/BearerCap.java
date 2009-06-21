/*
 @(#) $RCSfile: BearerCap.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:51 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:34:51 $ by $Author: brian $
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This Class defines the BearerCap Datatype. (Encoded as in Q.931)
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class BearerCap implements java.io.Serializable 
{
    /** Gets Coding Standard.  */
    public int getCodingStandard() {
        return codingStandard;
    }
    /** Sets Coding Standard.  */
    public void setCodingStandard(int codingStandard) {
        this.codingStandard = codingStandard;
    }
    /** Gets Information Transfer Capability.  */
    public int getInformationTransferCapability() {
        return informationTransferCapability;
    }
    /** Sets Information Transfer Capability.  */
    public void setInformationTransferCapability(int informationTransferCapability) {
        this.informationTransferCapability = informationTransferCapability;
    }
    /** Gets Transfer Mode.  */
    public int getTransferMode() {
        return transferMode;
    }
    /** Sets Transfer Mode.  */
    public void setTransferMode(int transferMode) {
        this.transferMode = transferMode;
    }
    /** Gets informationTransferRate.  */
    public int getInformationTransferRate() {
        return informationTransferRate;
    }
    /** Sets Information Transfer Rate.  */
    public void setInformationTransferRate(int informationTransferRate) {
        this.informationTransferRate = informationTransferRate;
    }
    /** Gets Rate Multiplier.  */
    public int getRateMultiplier() throws ParameterNotSetException {
        if (isRateMultiplier)
            return rateMultiplier;
        throw new ParameterNotSetException("Rate Multiplier: not set.");
    }
    /** Sets Rate Multiplier.  */
    public void setRateMultiplier(int rateMultiplier) {
        this.rateMultiplier = rateMultiplier;
        isRateMultiplier = true;
    }
    /** Indicates when the Rate Multiplier optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isRateMultiplierPresent() {
        return isRateMultiplier;
    }
    /** Gets User Information Layer 1 Protocol.  */
    public int getUserInformationLayer1Protocol() {
        return userInformationLayer1Protocol;
    }
    /** Sets User Information Layer 1 Protocol.  */
    public void setUserInformationLayer1Protocol(int userInformationLayer1Protocol) {
        this.userInformationLayer1Protocol = userInformationLayer1Protocol;
    }
    /** Gets Synchronous Asynchronous Indicator.  */
    public boolean getSynchronousAsynchronousIndicator() {
        return synchronousAsynchronousIndicator;
    }
    /** Sets Synchronous Asynchronous Indicator.  */
    public void setSynchronousAsynchronousIndicator(boolean synchronousAsynchronousIndicator ) {
        this.synchronousAsynchronousIndicator = synchronousAsynchronousIndicator;
        isSynchronousAsynchronousIndicator = true;
    }
    /** Indicates when the Synchronous Asynchronous Indicator optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isSynchronousAsynchronousIndicatorPresent() {
        return isSynchronousAsynchronousIndicator;
    }
    /** Gets Negotiation Indicator.  */
    public boolean getNegotiationIndicator() {
        return negotiationIndicator;
    }
    /** Sets Negotiation Indicator.  */
    public void setNegotiationIndicator(boolean negotiationIndicator) {
        this.negotiationIndicator = negotiationIndicator;
        isNegotiationIndicator = true;
    }
    /** Indicates when the Negotiation Indicator optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isNegotiationIndicatorPresent() {
        return isNegotiationIndicator;
    }
    /** Gets User Rate.  */
    public int getUserRate() {
        return userRate;
    }
    /** Sets User Rate.  */
    public void setUserRate(int userRate) {
        this.userRate = userRate;
        isUserRate = true;
    }
    /** Indicates when the User Rate optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isUserRatePresent() {
        return isUserRate;
    }
    /** Gets Intermediate Rate.  */
    public int getIntermediateRate() {
        return intermediateRate;
    }
    /** Sets Intermediate Rate.  */
    public void setIntermediateRate(int intermediateRate) {
        this.intermediateRate = intermediateRate;
    }
    /** Gets Network Independent Clock On Transmission Indicator.  */
    public boolean getNetworkIndependentClockOnTransmissionIndicator() {
        return networkIndependentClockOnTransmissionIndicator;
    }
    /** Sets Network Independent Clock On Transmission Indicator.  */
    public void setNetworkIndependentClockOnTransmissionIndicator(boolean networkIndependentClockOnTransmissionIndicator) {
        this.networkIndependentClockOnTransmissionIndicator = networkIndependentClockOnTransmissionIndicator;
    }
    /** Gets Network Independent Clock On Reception Indicator.  */
    public boolean getNetworkIndependentClockOnReceptionIndicator() {
        return networkIndependentClockOnReceptionIndicator;
    }
    /** Sets Network Independent Clock On Reception Indicator.  */
    public void setNetworkIndependentClockOnReceptionIndicator(boolean networkIndependentClockOnReceptionIndicator) {
        this.networkIndependentClockOnReceptionIndicator = networkIndependentClockOnReceptionIndicator;
    }
    /** Gets Flow Control On Transmission Indicator.  */
    public boolean getFlowControlOnTransmissionIndicator() {
        return flowControlOnTransmissionIndicator;
    }
    /** Sets Flow Control On Transmission Indicator.  */
    public void setFlowControlOnTransmissionIndicator(boolean flowControlOnTransmissionIndicator) {
        this.flowControlOnTransmissionIndicator = flowControlOnTransmissionIndicator;
    }
    /** Gets Flow Control On Reception Indicator.  */
    public boolean getFlowControlOnReceptionIndicator() {
        return flowControlOnReceptionIndicator;
    }
    /** Sets Flow Control On Reception Indicator.  */
    public void setFlowControlOnReceptionIndicator(boolean flowControlOnReceptionIndicator) {
        this.flowControlOnReceptionIndicator = flowControlOnReceptionIndicator;
    }
    /** Gets Rate Adaption Header Indicator.  */
    public boolean getRateAdaptionHeaderIndicator() {
        return rateAdaptionHeaderIndicator;
    }
    /** Sets Rate Adaption Header Indicator.  */
    public void setRateAdaptionHeaderIndicator(boolean rateAdaptionHeaderIndicator) {
        this.rateAdaptionHeaderIndicator = rateAdaptionHeaderIndicator;
    }
    /** Gets Multiple Frame Establishment Support In Data Link
      * Indicator.  */
    public boolean getMultipleFrameEstablishmentSupportInDataLinkIndicator() {
        return multipleFrameEstablishmentSupportInDataLinkIndicator;
    }
    /** Sets Multiple Frame Establishment Support In Data Link
      * Indicator.  */
    public void setMultipleFrameEstablishmentSupportInDataLinkIndicator(boolean multipleFrameEstablishmentSupportInDataLinkIndicator) {
        this.multipleFrameEstablishmentSupportInDataLinkIndicator = multipleFrameEstablishmentSupportInDataLinkIndicator;
    }
    /** Gets Mode Of Operation Indicator.  */
    public boolean getModeOfOperationIndicator() {
        return modeOfOperationIndicator;
    }
    /** Sets Mode Of Operation Indicator.  */
    public void setModeOfOperationIndicator(boolean modeOfOperationIndicator) {
        this.modeOfOperationIndicator = modeOfOperationIndicator;
    }
    /** Gets Logical Link Identifier Negotiation Indicator.  */
    public boolean getLogicalLinkIdentifierNegotiationIndicator() {
        return logicalLinkIdentifierNegotiationIndicator;
    }
    /** Sets Logical Link Identifier Negotiation Indicator.  */
    public void setLogicalLinkIdentifierNegotiationIndicator(boolean logicalLinkIdentifierNegotiationIndicator) {
        this.logicalLinkIdentifierNegotiationIndicator = logicalLinkIdentifierNegotiationIndicator;
    }
    /** Gets Assignor Assignee Indicator.  */
    public boolean getAssignorAssigneeIndicator() {
        return assignorAssigneeIndicator;
    }
    /** Sets Assignor Assignee Indicator.  */
    public void setAssignorAssigneeIndicator(boolean assignorAssigneeIndicator) {
        this.assignorAssigneeIndicator = assignorAssigneeIndicator;
    }
    /** Gets In-Band / Out-Band Negotiation Indicator.  */
    public boolean getInBandOutBandNegotiationIndicator() {
        return inBandOutBandNegotiationIndicator;
    }
    /** Sets In-Band / Out-Band Negotiation Indicator.  */
    public void setInBandOutBandNegotiationIndicator(boolean inBandOutBandNegotiationIndicator) {
        this.inBandOutBandNegotiationIndicator = inBandOutBandNegotiationIndicator;
    }
    /** Gets Number Of Stop Bits.  */
    public int getNumberofstopbits() {
        return numberOfStopBits;
    }
    /** Sets Number Of Stop Bits.  */
    public void setNumberOfStopBits(int numberOfStopBits) {
        this.numberOfStopBits = numberOfStopBits;
        isNumberOfStopBits = true;
    }
    /** Indicates when the Number Of Stop Bits optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isNumberOfStopBitsPresent() {
        return isNumberOfStopBits;
    }
    /** Gets Number Of Data Bits Excluding Parity Bit If Present.  */
    public int getNumberofDataBits() {
        return numberOfDataBits;
    }
    /** Sets Number Of Data Bits Excluding Parity Bit If Present.  */
    public void setNumberOfDataBits(int numberOfDataBits) {
        this.numberOfDataBits = numberOfDataBits;
        isNumberOfDataBits = true;
    }
    /** Indicates when the Number Of Data Bits optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isNumberOfDataBitsPresent() {
        return isNumberOfDataBits;
    }
    /** Gets Parity Information.  */
    public int getParityInformation() {
        return parityInformation;
    }
    /** Sets Parity Information.  */
    public void setParityInformation(int parityInformation) {
        this.parityInformation = parityInformation;
        isParityInformation = true;
    }
    /** Indicates when the Parity Information optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isParityInformationPresent() {
        return isParityInformation;
    }
    /** Gets Mode Duplex.  */
    public int getModeDuplex() {
        return modeDuplex;
    }
    /** Sets Mode Duplex.  */
    public void setModeDuplex(int modeDuplex) {
        this.modeDuplex = modeDuplex;
        isModeDuplex = true;
    }
    /** Indicates when the Mode Duplex optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isModeDuplexPresent() {
        return isModeDuplex;
    }
    /** Gets Modem Type.  */
    public int getModemType() {
        return modemType;
    }
    /** Sets Modem Type.  */
    public void setModemType(int modemType) {
        this.modemType = modemType;
        isModemType = true;
    }
    /** Indicates when the Modem Type optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isModemTypePresent() {
        return isModemType;
    }
    /** Gets User Information Layer 2 Protocol.  */
    public int getUserInformationLayer2Protocol() {
        return userInformationLayer2Protocol;
    }
    /** Sets User Information Layer 2 Protocol.  */
    public void setUserInformationLayer2Protocol(int userInformationLayer2Protocol) {
        this.userInformationLayer2Protocol = userInformationLayer2Protocol;
    }
    /** Gets User Information Layer 3 Protocol.  */
    public int getUserInformationLayer3Protocol() {
        return userInformationLayer3Protocol;
    }
    /** Sets User Information Layer 3 Protocol.  */
    public void setUserInformationLayer3Protocol(int userInformationLayer3Protocol) {
        this.userInformationLayer3Protocol = userInformationLayer3Protocol;
    }
    private int codingStandard;
    private int informationTransferCapability;
    private int transferMode;
    private int informationTransferRate;
    private int rateMultiplier;
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
    private boolean isRateMultiplier = false;
    private boolean isSynchronousAsynchronousIndicator = false;
    private boolean isNegotiationIndicator = false;
    private boolean isUserRate = false;
    private boolean isNumberOfStopBits = false;
    private boolean isNumberOfDataBits = false;
    private boolean isParityInformation = false;
    private boolean isModeDuplex = false;
    private boolean isModemType = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
