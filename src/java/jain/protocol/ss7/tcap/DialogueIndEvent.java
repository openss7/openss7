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

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.tcap.dialogue.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * A DialogueIndEvent is the superclass of all Dialogue Indication
  * primitives, which represent TCAP Dialogue Indications sent from a
  * JainTcapProvider to a JainTcapListener . It affords a method of
  * generic event passing, while enforcing tight control over event flow
  * between the Listener and the Provider. This type of Event will be
  * processed by the processDialogueIndEvent method of the
  * JainTcapListener . This class implements the java.lang.Cloneable
  * interface, therefore all Dialogue Indication primitives that extend
  * this Event can be cloned using the clone() method inherited from
  * java.lang.Object .
  * @version 1.2.2
  * @author Monavacon Limited
  */
public abstract class DialogueIndEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {
    /** Constructs a new DialogueIndEvent.
      * @param source
      * The object source supplied to the constructor.  */
    public DialogueIndEvent(java.lang.Object source) {
        super(source);
    }
    /** Sets the source of this event. This method may be used to
      * override the setting the event source through the constructor,
      * this provides the ability to clone the Event and change the
      * Event source at any time.
      * @param source
      * The new Object source.  */
    public void setSource(java.lang.Object source) {
        this.source = source;
    }
    /** Sets the Dialogue Id parameter of this Dialogue Indication
      * Event, which is a reference identifier that is used to uniquely
      * identify a dialogue and all associated transaction messages and
      * components with that Dialogue. It is determined by the local
      * TCAP layer and given to the application for all transactions
      * received or initiated by the local application. <p>
      * <em>Note to developers:-</em> A Dialogue Id is forbidden in a
      * UNI Indication message, therefore this method should not be used
      * in a UnidirectionalInd Event.
      * @param dialogueId
      * The new Dialogue Identifier.  */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }
    /** Sets the Dialogue Portion of this Dialogue handling indication
      * primitive. The Dialogue Portion provides a mechanism to
      * determine the interpretation of TCAP message information,
      * security and confidentiality at a remote TCAP. A Dialogue
      * Portion is forbidden in an ANSI 1992 implementation of the JAIN
      * TCAP API.
      * @param dialoguePortion
      * The new Dialogue Portion.  */
    public void setDialoguePortion(DialoguePortion dialoguePortion) {
        m_dialoguePortion = dialoguePortion;
        m_dialoguePortionPresent = true;
    }
    /** Gets the Dialogue Id parameter, which is a reference identifier
      * that is used to uniquely identify a dialogue and all associated
      * transaction messages and components with that Dialogue. It is
      * determined by the local TCAP layer and given to the application
      * for all transactions received or initiated by the local
      * application. <p>
      * <em>Note to developers:-</em> A Dialogue Id is forbidden in a
      * UNI Indication message, therefore this method should not be used
      * in a UnidirectionalInd Event.
      * @return
      * The dialogueId that uniquely identifies the Dialogue of this
      * Dialogue Indication Event.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory Parameter has
      * not been set.  */
    public int getDialogueId()
        throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent)
            return m_dialogueId;
        throw new MandatoryParameterNotSetException("Dialogue Id: not set.");
    }
    /** Indicates if the Dialogue Portion parameter is present in this
      * Event. <p>
      * <em>Note to developers:-</em> A Dialogue Portion can only be
      * present in an ITU 1993, ITU 1997 and ANSI 1996 implementation of
      * the JAIN TCAP API.
      * @return
      * True if Dialogue Portion has been set, false otherwise.  */
    public boolean isDialoguePortionPresent() {
        return m_dialoguePortionPresent;
    }
    /** Gets the Dialogue Portion of this Dialogue handling indication
      * primitive. The Dialogue Portion provides a mechanism to
      * determine the interpretation of TCAP message information,
      * security and confidentiality at a remote TCAP. A Dialogue
      * Portion is forbidden in a ANSI 1992 implementaion of the JAIN
      * TCAP API.
      * @return
      * The dialoguePortion of this Dialogue handling indication
      * primitive.
      * @exception ParameterNotSetException
      * This exception is thrown if this Optional JAIN parameter has not
      * been set.  */
    public DialoguePortion getDialoguePortion()
        throws ParameterNotSetException {
        if (m_dialoguePortionPresent)
            return m_dialoguePortion;
        throw new ParameterNotSetException("Dialogue Portion: not set.");
    }
    /** This abstract method defines the 'getPrimitiveType' method that
      * must be implemented by its subclasses in order to return the
      * type of that subclassed Dialogue Indication primitive.
      * @return
      * The Primitive Type of the Event.  */
    public abstract int getPrimitiveType();
    /** String representation of class DialogueIndEvent.
      * @return
      * String provides description of class DialogueIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\n\tm_dialogueId = " + m_dialogueId);
        b.append("\n\tm_dialogueIdPresent = " + m_dialogueIdPresent);
        b.append("\n\tm_dialoguePortion = ");
        if (m_dialoguePortion != null)
            b.append(m_dialoguePortion.toString());
        b.append("\n\tm_dialoguePortionPresent = " + m_dialoguePortionPresent);
        return b.toString();
    }
    /** The Dialogue Id parameter, which uniquely identifies this Dialogue Indication Event to a specific Dialogue between two TCAP Users.
      * @serial m_dialogueId
      * - a default serializable field.  */
    protected int m_dialogueId = 0;
    /** Whether Dialogue Id is present.
      * @serial m_dialogueIdPresent
      * - a default serializable field.  */
    protected boolean m_dialogueIdPresent = false;
    /**
      * The Dialogue Portion of this Dialogue handling indication
      * primitive, which is forbidden in an ANSI 1992 implementation of
      * the JAIN TCAP API.
      * @serial m_dialoguePortion
      * - a default serializable field.  */
    protected DialoguePortion m_dialoguePortion = null;
    /**
      * Should always be false in an ANSI 1992 implementation of the
      * JAIN TCAP API.
      * @serial m_dialoguePortionPresent
      * - a default serializable field.  */
    protected boolean m_dialoguePortionPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
