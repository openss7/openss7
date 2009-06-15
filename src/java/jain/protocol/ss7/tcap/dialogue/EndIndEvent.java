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

package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP End indication dialogue primitive. This
  * event will be passed from the Provider (TCAP) to the Listener(the TC
  * User) to indicate the termination of a structured dialogue with the
  * originating node. <br>
  * The mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods. <p>
  * The optional parameters 'Application Context Name' and 'User
  * Information' are centrally located in the Dialogue Portion class,
  * therefore to manipulate them it is necessary to instantiate the
  * Dialogue Portion Object and use the accessors method for the two
  * parameters in that Dialogue Portion Object.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see DialogueIndEvent
  */
public final class EndIndEvent extends DialogueIndEvent {
    /** Constructs a new EndIndEvent, with only the Event Source and the
      * JAIN TCAP Mandatory parameters being supplied to the
      * constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.
      * @param componentsPresent
      * The Components Present Flag supplied to the constructor.  */
    public EndIndEvent(java.lang.Object source, int dialogueId,
            boolean componentsPresent) {
        super(source);
        setDialogueId(dialogueId);
        setComponentsPresent(componentsPresent);
    }
    /** Sets the Quality of Service parameter of the End indication
      * primitive.
      * @param qualityOfService
      * The new Quality Of Service value.  */
    public void setQualityOfService(byte qualityOfService) {
        m_qualityOfService = qualityOfService;
        m_qualityOfServicePresent = true;
    }
    /** Sets the Components present parameter of this End indication
      * primitive. This flag is used to determine if their are any
      * components associated with this primitive. This flag will be
      * reset to false when the clearAllParameters() method is invoked.
      * @param componentsPresent
      * The new Components Present Flag.  */
    public void setComponentsPresent(boolean componentsPresent) {
        m_componentsPresent = componentsPresent;
    }
    /** Indicates if the Quality of Service parameter is present in this
      * Event.
      * @return
      * True if Quality of Service has been set, false otherwise.  */
    public boolean isQualityOfServicePresent() {
        return m_qualityOfServicePresent;
    }
    /** Gets the Quality of Service parameter of the End indication
      * primitive. Quality of Service is an SCCP parameter that is
      * required from the application.
      * @return
      * The Quality of Service parameter of the EndEvent.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public byte getQualityOfService()
        throws ParameterNotSetException {
        if (m_qualityOfServicePresent)
            return m_qualityOfService;
        throw new ParameterNotSetException("Quality of Service: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this Event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_END;
    }
    /** Returns the Components present flag of this End indication
      * primitive. This flag is used to determine if their are any
      * components associated with this primitive. This flag will be
      * reset to false when the clearAllParameters() method is invoked.
      * @return
      * The Components Present of the EndEvent.  */
    public boolean isComponentsPresent() {
        return m_componentsPresent;
    }
    /** Clears all previously set parameters and resets the 'Components
      * Present' flag to false.  */
    public void clearAllParameters() {
        m_dialoguePortionPresent = false;
        m_dialogueIdPresent = false;
        m_qualityOfServicePresent = false;
        m_componentsPresent = false;
    }
    /** String representation of class EndIndEvent.
      * @return
      * String provides description of class EndIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nEndIndEvent");
        b.append(super.toString());
        b.append("\n\tm_qualityOfService = " + m_qualityOfService);
        b.append("\n\tm_qualityOfServicePresent = " + m_qualityOfServicePresent);
        b.append("\n\tm_componentsPresent = " + m_componentsPresent);
        return b.toString();
    }
    /** The Quality of Service parameter of the End indication dialogue
      * primitive.
      * @serial m_qualityOfService
      * - a default serializable field.  */
    private byte m_qualityOfService = 0;
    /** Whether Quality of Service is present.
      * @serial m_qualityOfServicePresent
      * - a default serializable field.  */
    private boolean m_qualityOfServicePresent = false;
    /** The Components Present parameter of the Dialogue Indication
      * primitive.
      * @serial m_componentsPresent
      * - a default serializable field.  */
    private boolean m_componentsPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
