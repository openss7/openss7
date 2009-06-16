/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class defines the MessageId DataType.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class MessageID implements java.io.Serializable {
    /** Constructors For MessageId.  */
    public MessageID(int elementaryMessageID) {
        setElementaryMessageID(elementaryMessageID);
    }
    /** Constructors For MessageId.  */
    public MessageID(int elementaryMessageIDs[]) {
        setElementaryMessageIDs(elementaryMessageIDs);
    }
    /** Constructors For MessageId.  */
    public MessageID(Text text) {
        setText(text);
    }
    /** Constructors For MessageId.  */
    public MessageID(VariableMessage variableMessage) {
        setVariableMessage(variableMessage);
    }
    /** Gets Message ID Choice.  */
    public MessageIDChoice getMessageIDChoice() {
        return messageIDChoice;
    }
    /** Gets Elementary Message Id.  */
    public int getElementaryMessageID()
        throws ParameterNotSetException {
        if (messageIDChoice == MessageIDChoice.ELEMENTARY_MESSAGE_ID)
            return elementaryMessageID;
        throw new ParameterNotSetException("Elementary Message: not set.");
    }
    /** Sets Elementary Message Id.  */
    public void setElementaryMessageID(int elementaryMessageID) {
        this.elementaryMessageID = elementaryMessageID;
        messageIDChoice=MessageIDChoice.ELEMENTARY_MESSAGE_ID;
    }
    /** Gets Elementary Message Ids.  */
    public int[] getElementaryMessageIDs()
        throws ParameterNotSetException {
        if (messageIDChoice == MessageIDChoice.ELEMENTARY_MESSAGE_IDS)
            return elementaryMessageIDs;
        throw new ParameterNotSetException("Elmentary Message Ids: not set.");
    }
    /** Gets a particular Elementary Message Id.  */
    public int getElementaryMessageIDs(int index) {
        return elementaryMessageIDs[index];
    }
    /** Sets Elementary Message Ids.  */
    public void setElementaryMessageIDs(int elementaryMessageIDs[]) {
        this.elementaryMessageIDs = elementaryMessageIDs;
        messageIDChoice = MessageIDChoice.ELEMENTARY_MESSAGE_IDS;
    }
    /** Sets a particular Elementary Message Id.  */
    public void  setElementaryMessageIDs(int index, int elementaryMessageIDs) {
        this.elementaryMessageIDs[index] = elementaryMessageIDs;
    }
    /** Gets Text.  */
    public Text getText()
        throws ParameterNotSetException {
        if (messageIDChoice == MessageIDChoice.TEXT)
            return text;
        throw new ParameterNotSetException("Text: not set");
    }
    /** Sets Text.  */
    public void setText(Text text) {
        this.text = text;
        messageIDChoice = MessageIDChoice.TEXT;
    }
    /** Gets Variable Message.  */
    public VariableMessage getVariableMessage()
        throws ParameterNotSetException {
        if (messageIDChoice == MessageIDChoice.VARIABLE_MESSAGE)
            return variableMessage;
        throw new ParameterNotSetException("Variable Message: not set.");
    }
    /** Sets Variable Message.  */
    public void setVariableMessage(VariableMessage variableMessage) {
        this.variableMessage = variableMessage;
        messageIDChoice = MessageIDChoice.VARIABLE_MESSAGE;
    }
    private int elementaryMessageID;
    private int elementaryMessageIDs[];
    private Text text;
    private VariableMessage variableMessage;
    private MessageIDChoice messageIDChoice;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
