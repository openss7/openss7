/*
 @(#) src/java/org/openss7/ss7/mtp/MtpTransferEvent.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MtpTransferEvent extends MtpEvent {
    /** Construct an MtpTransferEvent object.  */
    public MtpTransferEvent() {
        super();
    }
    /**
      * Construct an MtpTransferEvent object will mandatory fields.
      *
      * @param source
      * The source object generating this event object.
      * @param destinationPointCode
      * The destination point code for the message transfer.
      * @param originatingPointCode
      * The origination point code for the message transfer.
      * @param signalingLinkSelection
      * The signalling link selection code that is used to determine the
      * normal signalling link in a signalling link set that will be
      * used to transfer the message.  Messages that must have a high
      * assurance of in-order deliver must set this parameter to the
      * same value for each transmission. <p> When the value is null, a
      * new signalling link selection value will be assigned
      * automatically.
      * @param messagePriority
      * The message priority.  This can logically be a value between 0
      * and 3 includsive, however, the value 3 is reserved for MTP
      * management messages and must not be specified to this
      * constructor.
      * @param transferData
      * The message data to transfer to the MTP peer.
      */
    public MtpTransferEvent(
            java.lang.Object source,
            javax.jain.ss7.NetworkIndicator networkIndicator,
            javax.jain.ss7.SignalingPointCode destinationPointCode,
            javax.jain.ss7.SignalingPointCode originatingPointCode,
            SignalingLinkSelection signalingLinkSelection,
            MessagePriority messagePriority,
            SignalingInformationField transferData) {
        super(source);
        setNetworkIndicator(networkIndicator);
        setDestinationPointCode(destinationPointCode);
        setOriginatingPointCode(originatingPointCode);
        setSignalingLinkSelection(signalingLinkSelection);
        setMessagePriority(messagePriority);
        setTransferData(transferData);
    }
    public javax.jain.ss7.NetworkIndicator getNetworkIndicator() {
        return networkIndicator;
    }
    /** Gets the destination point code.
      * @return
      * The destination point code set, or null if no destination point
      * code is set. */
    public javax.jain.ss7.SignalingPointCode getDestinationPointCode() {
        return destinationPointCode;
    }
    /** Gets the originating point code.
      * @return
      * The originating point code set, or null if no originating point
      * code is set. */
    public javax.jain.ss7.SignalingPointCode getOriginatingPointCode() {
        return originatingPointCode;
    }
    /** Gets the signaling link selection.
      * The signalling link selection code is used to determine the
      * normal signalling link in a signalling link set that will be
      * used to transfer the message.  Messages that must have a high
      * assurance of in-order delivery must set this parameter to the
      * same value for each transmission.
      * @return
      * The signalling link selection value.  */
    public SignalingLinkSelection getSignalingLinkSelection() {
        return signalingLinkSelection;
    }
    /** Gets the message priority.
      * For national systems and by bilateral agreement in international
      * systems, the message priority indicates the priority of the
      * message during SS7 signalling network congestion.  Messages of a
      * lower priority will be discarded before messages of a higher
      * priority under heavy loads.  This value can be between 0 and 3
      * (inclusive); however, the value 3 is reserved for MTP
      * management messages and will not be return from this method.
      * @return
      * The message priority (0 &le; messagePrioirty &le; 3).
      */
    public MessagePriority getMessagePriority() {
        return messagePriority;
    }
    public SignalingInformationField getTransferData() {
        return transferData;
    }
    /** Set the network indicator associated with the MTP Transfer.
      * @param networkIndicator
      * The Network Indicator - one of the following values; <ul>
      * <li>{@link javax.jain.ss7.NetworkIndicator#NI_INTERNATIONAL_00 NI_INTERNATIONAL_00}
      * <li>{@link javax.jain.ss7.NetworkIndicator#NI_INTERNATIONAL_01 NI_INTERNATIONAL_01}
      * <li>{@link javax.jain.ss7.NetworkIndicator#NI_NATIONAL_10 NI_NATIONAL_10}
      * <li>{@link javax.jain.ss7.NetworkIndicator#NI_NATIONAL_11 NI_NATIONAL_11}. </ul>
      */
    public void setNetworkIndicator(javax.jain.ss7.NetworkIndicator networkIndicator) {
        this.networkIndicator = networkIndicator;
    }
    public void setDestinationPointCode(javax.jain.ss7.SignalingPointCode destinationPointCode) {
        this.destinationPointCode = destinationPointCode;
    }
    public void setOriginatingPointCode(javax.jain.ss7.SignalingPointCode originatingPointCode) {
        this.originatingPointCode = originatingPointCode;
    }
    public void setSignalingLinkSelection(SignalingLinkSelection signalingLinkSelection) {
        if (signalingLinkSelection == null)
            signalingLinkSelection = new SignalingLinkSelection();
        this.signalingLinkSelection = signalingLinkSelection;
    }
    public void setMessagePriority(MessagePriority messagePriority) {
        this.messagePriority = messagePriority;
    }
    public void setTransferData(SignalingInformationField transferData) {
        this.transferData = transferData;
    }
    private javax.jain.ss7.NetworkIndicator networkIndicator;
    private javax.jain.ss7.SignalingPointCode destinationPointCode;
    private javax.jain.ss7.SignalingPointCode originatingPointCode;
    private SignalingLinkSelection signalingLinkSelection;
    private MessagePriority messagePriority;
    private SignalingInformationField transferData;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
