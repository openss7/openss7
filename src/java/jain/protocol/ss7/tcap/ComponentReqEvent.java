/*
 @(#) $RCSfile: ComponentReqEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:28 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:28 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * A ComponentReqEvent is the superclass of all Component request
  * primitives, which represent TCAP Component requests sent from a
  * JainTcapListener to a JainTcapProvider . It affords a method of
  * generic event passing, while enforcing tight control over event flow
  * between the Listener and the Provider. This type of Event will be
  * processed by the sendComponentReqEvent() method of the
  * JainTcapProvider . This class implements the java.lang.Cloneable
  * interface, therefore all Component Request primitives that extend
  * this Event can be cloned using the clone() method inherited from
  * java.lang.Object .
  * @version 1.2.2
  * @author Monavacon Limited
  */
public abstract class ComponentReqEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {
    /** Constructs a new ComponentReqEvent.
      * @param source
      * The new source object supplied to the constructor.  */
    public ComponentReqEvent(java.lang.Object source) {
        super(source);
    }
    /** Sets the source of this event. This method may be used to
      * override the setting of the event source through the
      * constructor, this provides the ability to clone the Event and
      * change the Event source at any time.
      * @param source
      * The new object source.  */
    public void setSource(java.lang.Object source) {
        this.source = source;
    }
    /** Sets the Dialogue Id parameter of this Component Request Event.
      * Dialogue Id is a reference identifier, which identifies a
      * specific dialogue or transaction and all associated components
      * within that dialogue or transaction. Update v1.1 :- There is no
      * isDialogueIdPresent method in this super class because the
      * dialogueId parameter is mandatory in all children.
      * @param dialogueId
      * The new Dialogue Identifier.  */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }
    /** Sets the Invoke Id parameter of the request component. Invoke Id
      * identifies the operation invocation and its result. The mapping
      * rules for the Invoke Identifier in each of the components in the
      * Jain Tcap Api are as follows:- <ul>
      * <li>INVOKE Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in both the ITU and ANSI recommendations
      * for the Invoke component.
      * <li>RESULT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Result component.
      * <li>REJECT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Reject component.
      * <li>ERROR Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Error component. </ul>
      * Update v1.1 :- There is no isInvokeIdPresent method in this
      * super class because not all children have an optional InvokeId,
      * namely TimerReset and UserCancel.
      * @param invokeId
      * The new Invoke Identifier.  */
    public void setInvokeId(int invokeId) {
        m_invokeId = invokeId;
        m_invokeIdPresent = true;
    }
    /** Gets the Dialogue Id parameter of the Component Request Event,
      * which is a reference identifier which identifies a specific
      * dialogue or transaction and all associated components within
      * that dialogue or transaction.
      * @return
      * The Dialogue Id of the request event.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory parameter has
      * not been set.  */
    public int getDialogueId()
        throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent)
            return m_dialogueId;
        throw new MandatoryParameterNotSetException("Dialogue Id: not set.");
    }
    /** Gets the Invoke Id parameter of the request component. Invoke Id
      * identifies the operation invocation and its result. The mapping
      * rules for the Invoke Identifier in each of the components in the
      * Jain Tcap Api are as follows:- <ul>
      * <li>INVOKE Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in both the ITU and ANSI recommendations
      * for the Invoke component.
      * <li>RESULT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Result component.
      * <li>REJECT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Reject component.
      * <li>ERROR Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Error component. </ul>
      * @return
      * The Invoke Id of the Component Request event.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getInvokeId()
        throws ParameterNotSetException {
        if (m_invokeIdPresent)
            return m_invokeId;
        throw new ParameterNotSetException("Invoke Id: not set.");
    }
    /** This abstract method defines the 'getPrimitiveType' method that
      * must be implemented by its subclasses in order to return the
      * type of that subclassed Component Request primitive.
      * @return
      * The Primitive Type of the Event.  */
    public abstract int getPrimitiveType();
    /** java.lang.String representation of class
      * jain.protocol.ss7.tcap.ComponentReqEvent.
      * @return
      * java.lang.String provides description of class ComponentReqEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\n\tm_dialogueId = " + m_dialogueId);
        b.append("\n\tm_dialogueIdPresent = " + m_dialogueIdPresent);
        b.append("\n\tm_invokeId = " + m_invokeId);
        b.append("\n\tm_invokeIdPresent = " + m_invokeIdPresent);
        return b.toString();
    }
    /** The Dialogue Id parameter of the Component Request Event.
      * @serial m_dialogueId
      * - a default serializable field.  */
    protected int m_dialogueId = 0;

    /** Whether the Dialogue Id is present.
      * @serial m_dialogueIdPresent
      * - a default serializable field.  */
    protected boolean m_dialogueIdPresent = false;

    /** The Invoke Id parameter for ITU and ANSI for an INVOKE
      * component. <br>
      * The Invoke Id for ITU and Correlation Id for ANSI in the RESULT,
      * REJECT and ERROR components.
      * @serial m_invokeId
      * - a default serializable field.  */
    protected int m_invokeId = 0;

    /** Whether the Invoke Id is present.
      * @serial m_invokeIdPresent
      * - a default serializable field.  */
    protected boolean m_invokeIdPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
