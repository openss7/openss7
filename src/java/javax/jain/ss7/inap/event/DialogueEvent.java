/*
 @(#) $RCSfile: DialogueEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:28:36 $ <p>
 
 Copyright &copy; 2008-2010  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 
 Last Modified $Date: 2010-11-28 14:28:36 $ by $Author: brian $
 */

package javax.jain.ss7.inap.event;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This is an base class for Dialogue Events: both indications
  * and requests.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class DialogueEvent extends InapEvent {
    private DialoguePrimitiveType dialoguePrimitiveType;
    private TransportAddress transportAddress;
    private int callID;
    private byte[] applicationContext;
    private java.util.Vector<java.lang.Object> inapMessages = null;
    /** Constructs a new DialogueEvent.  */
    protected DialogueEvent(java.lang.Object source) {
        super(source);
    }
    /** Constructs a new DialogueEvent with primitive type. */
    protected DialogueEvent(java.lang.Object source, DialoguePrimitiveType dialoguePrimitiveType) {
        super(source);
        setDialoguePrimitiveType(dialoguePrimitiveType);
    }
    /** Protected constructor with all fields. */
    protected DialogueEvent(java.lang.Object source,
            TransportAddress transportAddress,
            int callID, byte[] applicationContext,
            DialoguePrimitiveType dialoguePrimitiveType) {
        this(source,dialoguePrimitiveType);
        setCallID(callID);
        setTransportAddress(transportAddress);
        setApplicationContext(applicationContext);
    }
    /** Protected constructor with all fields and INAP messages. */
    protected DialogueEvent(java.lang.Object source,
            TransportAddress transportAddress,
            int callID, byte[] applicationContext,
            java.util.Vector<java.lang.Object> inapMessages,
            DialoguePrimitiveType dialoguePrimitiveType) {
        this(source, transportAddress, callID, applicationContext, dialoguePrimitiveType);
        setInapMessages(inapMessages);
    }
    /** Sets the type of dialogue primitive.  */
    private void setDialoguePrimitiveType(DialoguePrimitiveType dialoguePrimitiveType) {
        this.dialoguePrimitiveType = dialoguePrimitiveType;
    }
    /** Gets the type of dialogue primitive.  */
    public DialoguePrimitiveType getDialoguePrimitiveType() {
        return dialoguePrimitiveType;
    }
    /** Gets the Transport Address.  */
    protected TransportAddress getTransportAddress() {
        return transportAddress;
    }
    /** Sets the Transport Alias Address.  */
    protected void setTransportAddress(TransportAddress transportAddress) {
        this.transportAddress = transportAddress;
    }
    /** Gets the Call Identifier of the Call.  */
    public int getCallID() {
        return callID;
    }
    /** Sets the Call Identifier.  */
    public void setCallID(int callID) {
        this.callID = callID;
    }
    /** Gets the Application Context.  */
    public byte[] getApplicationContext() {
        return applicationContext;
    }
    /** Sets the Application Context.  */
    public void setApplicationContext(byte[] applicationContext) {
        this.applicationContext = applicationContext;
    }
    /** Sets a reference of the message objects for the event.  */
    public void setInapMessages(java.util.Vector<java.lang.Object> inapMessages) {
        this.inapMessages = inapMessages;
    }
    /** Adds a message object for the event.  */
    public void addInapMessage(java.lang.Object inapMessage) {
        if (inapMessages == null)
            inapMessages = new java.util.Vector<java.lang.Object>();
        inapMessages.addElement(inapMessage);
    }
    /** Gets the number of message objects for the event.  */
    public int getNoOfMessageObjects() {
        return inapMessages.size();
    }
    /** Gets a specific message object for the event.  */
    public java.lang.Object getMessageObjectAt(int index) {
        return inapMessages.elementAt(index);
    }
    /** Gets a reference of the message objects for the event.  */
    public java.util.Vector<java.lang.Object> getInapMessages() {
        return inapMessages;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

