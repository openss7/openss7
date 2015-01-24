/*
 @(#) src/java/jain/protocol/ss7/tcap/dialogue/EndReqEvent.java <p>
 
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

package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP End request dialogue primitive. This
  * event will be passed from the Listener (the TC User) to the Provider
  * (TCAP) to end a transaction with the destination node. No component
  * can be sent or received for a transaction once the end request has
  * been issued. If the Termination parameter is set to TC_BASIC_END
  * this Event indicates a basic dialogue termination ending and any
  * pending components will be sent to the destination node. If the
  * Termination parameter is set to TC_PRE_ARRANGED_END this Event
  * indicates a pre-arranged dialogue termination ending no pending
  * components will be sent to the destination node. The mandatory
  * parameters of this primitive are supplied to the constructor.
  * Optional parameters may then be set using the set methods. <p>
  * The optional parameters 'Application Context Name' and 'User
  * Information' are centrally located in the Dialogue Portion class,
  * therefore to manipulate them it is necessary to instantiate the
  * Dialogue Portion Object and use the accessors method for the two
  * parameters in that Dialogue Portion Object.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see DialogueReqEvent
  */
public final class EndReqEvent extends DialogueReqEvent {
    /** Constructs a new EndReqEvent, with only the Event Source and the
      * JAIN TCAP Mandatory parameters being supplied to the
      * constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.  */
    public EndReqEvent(java.lang.Object source, int dialogueId) {
        super(source);
        setDialogueId(dialogueId);
    }
    /** Sets the Termination parameter of the End request primitive.
      * @param termination
      * One of the following: <ul>
      * <li>TC_BASIC_END
      * <li>TC_PRE_ARRANGED_END </ul>
      * @see DialogueConstants */
    public void setTermination(int termination) {
        m_termination = termination;
        m_terminationPresent = true;
    }
    /** Indicates if the Termination parameter is present in this Event.
      * @return
      * True if Termination has been set, false otherwise.  */
    public boolean isTerminationPresent() {
        return m_terminationPresent;
    }
    /** Gets the termination parameter of the End request primitive.
      * The Termination parameter indicates which scenario is chosen by
      * the application for the end of the dialogue
      * @return
      * The Termination of the EndReqEvent, one of the following: <ul>
      * <li>TC_BASIC_END
      * <li>TC_PREARRANGED_END </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.
      * @see DialogueConstants */
    public int getTermination()
        throws ParameterNotSetException {
        if (m_terminationPresent)
            return m_termination;
        throw new ParameterNotSetException("Termination: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this Event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_END;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialoguePortionPresent = false;
        m_dialogueIdPresent = false;
        m_qualityOfServicePresent = false;
        m_terminationPresent = false;
    }
    /** java.lang.String representation of class EndReqEvent.
      * @return
      * java.lang.String provides description of class EndReqEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nEndReqEvent");
        b.append(super.toString());
        b.append("\n\tm_termination = " + m_termination);
        b.append("\n\tm_terminationPresent = " + m_terminationPresent);
        return b.toString();
    }
    /** The Termination parameter of the End request dialogue
      * primitive.
      * @serial m_termination
      * - a default serializable field.  */
    private int m_termination = 0;
    /** Whether Termination is present.
      * @serial m_terminationPresent
      * - a default serializable field.  */
    private boolean m_terminationPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
