/*
 @(#) src/java/jain/protocol/ss7/tcap/ComponentIndEvent.java <p>
 
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

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * A ComponentIndEvent is the super class of all Component Indication
  * primitives, which represent TCAP Component Indications sent from a
  * JainTcapProvider to a JainTcapListener. It affords a method of
  * generic event passing, while enforcing tight control over event flow
  * between the Listener and the Provider. This type of Event will be
  * processed by the processComponentIndEvent method of the
  * JainTcapListener. This class implements the java.lang.Cloneable
  * interface, therefore all Component Indication primitives that extend
  * this Event can be cloned using the clone() method inherited from
  * java.lang.Object.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public abstract class ComponentIndEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {
    /** Constructs a new ComponentIndEvent.
      * @param source
      * The new object source supplied to the constructor.  */
    public ComponentIndEvent(java.lang.Object source) {
        super(source);
    }
    /** Sets the source of this event. This method may be used to
      * override the setting of the event source through the
      * constructor, this provides the ability to clone the Event and
      * change the Event source at any time.
      * @param source
      * The new Object Source value.  */
    public void setSource(java.lang.Object source) {
        this.source = source;
    }
    /** Sets the Dialogue Id parameter of this Component Indication
      * Event. Dialogue Id is a reference identifier, which identifies a
      * specific dialogue or transaction and all associated components
      * within that dialogue or transaction.
      * @param dialogueId
      * The new Dialogue Identifier.  */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }
    /** Sets the Invoke Id parameter of the indication component. The
      * mapping rules for the Invoke Identifier in each of the
      * components in the Jain Tcap Api are as follows:- <ul>
      *
      * <li>INVOKE Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in both the ITU and ANSI recommendations
      * for the Invoke component.
      *
      * <li>RESULT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Result component.
      *
      * <li>REJECT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Reject component.
      *
      * <li>ERROR Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Error component. </ul>
      *
      * @param invokeId
      * The new Invoke Identifier.  */
    public void setInvokeId(int invokeId) {
        m_invokeId = invokeId;
        m_invokeIdPresent = true;
    }
    /** Sets this indication Component as the last component in a
      * Transaction message for example BEGIN or CONTINUE.
      * <h4>Note to Developers:-</h4>
      * This should not be confused with the lastInvokeEvent parameter
      * in an INVOKE Component which is a flag for an ANSI
      * implementation of the JAIN TCAP API to determine the type of
      * component, either INVOKE_NL or INVOKE_L.  For example in an ANSI
      * INVOKE Indicaton the last component in the transaction message
      * could be an INVOKE_NL, were an INVOKE_L will follow in the next
      * message segment, therefore the lastComponent parameter is true,
      * while the lastInvokeEvent is false. <p>
      *
      * By default this component will not be the last Component, that
      * is the lastComponent is set to false by default.
      * @param lastComponent
      * The new LastComponent value.  */
    public void setLastComponent(boolean lastComponent) {
        m_lastComponent = lastComponent;
    }
    /** Gets the Dialogue Id parameter of the indication component,
      * which is a reference identifier which identifies a specific
      * dialogue or transaction and all associated components within
      * that dialogue or transaction.
      * @return
      * The Dialogue Id of the Component indication.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter is a JAIN optional
      * parameter and has not yet been set. The Dialogue Id is JAIN
      * Optional for the Invoke Indication.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if the Dialogue Id is a JAIN mandatory
      * parameter for the specific Component Indication. The Dialogue Id
      * is JAIN Mandatory for all component indications except for the
      * Invoke Indication.  */
    public abstract int getDialogueId()
        throws ParameterNotSetException, MandatoryParameterNotSetException;
    /** Indicates if the Invoke Id is present in this Indication event.
      * @return
      * True if Invoke Id has been set, false otherwise.  */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }
    /** Gets the Invoke Id parameter of the component indication.
      * Invoke Id identifies the operation invocation and its result.
      * The mapping rules for the Invoke Identifier in each of the
      * components in the Jain Tcap Api are as follows:- <ul>
      *
      * <li>INVOKE Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in both the ITU and ANSI recommendations
      * for the Invoke component.
      *
      * <li>RESULT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Result component.
      *
      * <li>REJECT Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Reject component.
      *
      * <li>ERROR Component :- The Invoke Identifier is the Invoke
      * Identifier as specified in the ITU recommendation and the
      * Correlation Identifier as specified in the ANSI recommendation
      * for the Error component. </ul>
      *
      * @return
      * The Invoke Id of the Indication event.
      * @exception ParameterNotSetException
      * This exception is thrown if this is a JAIN Optional parameter in
      * the specific component indication and has not been set.
      * @exception MandatoryParameterNotSetException
      * Thrown when this is a JAIN Mandatory parameter in the specific
      * component indication and has not been set.  */
    public abstract int getInvokeId()
        throws ParameterNotSetException, MandatoryParameterNotSetException;
    /** Indicates if this indication Component is the last component in
      * a Transaction message i.e. BEGIN or CONTINUE.
      *
      * <h4>Note to Developers:-</h4>
      * This should not be confused with the lastInvokeEvent parameter
      * in an INVOKE Component which is a flag for an ANSI
      * implementation of the JAIN TCAP API to determine the type of
      * component, either INVOKE_NL or INVOKE_L.  For example in an ANSI
      * INVOKE Indicaton the last component in the transaction message
      * could be an INVOKE_NL, were an INVOKE_L will follow in the next
      * message segment, therefore the lastComponent parameter is true,
      * while the lastInvokeEvent is false. <p>
      *
      * By default the lastComponent parameter is set to false.
      * @return
      * True if this is the last component, false otherwise.  */
    public boolean isLastComponent() {
        return m_lastComponent;
    }
    /** This abstract method defines the 'getPrimitiveType' method that
      * must be implemented by its subclasses in order to return the
      * type of that subclassed Component Indication primitive.
      * @return
      * The PrimitiveType value.  */
    public abstract int getPrimitiveType();
    /** java.lang.String representation of class ComponentIndEvent.
      * @return
      * java.lang.String provides description of class ComponentIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\n\tm_dialogueId = " + m_dialogueId);
        b.append("\n\tm_invokeId = " + m_invokeId);
        b.append("\n\tm_lastComponent = " + m_lastComponent);
        b.append("\n\tm_invokeIdPresent = " + m_invokeIdPresent);
        b.append("\n\tm_dialogueIdPresent = " + m_dialogueIdPresent);
        b.append("\n\tm_lastComponentPresent = " + m_lastComponentPresent);
        return b.toString();
    }
    /** The Dialogue Id parameter of the indication component.
      * @serial m_dialogueId
      * a default serializable field.  */
    protected int m_dialogueId = 0;
    /** The Invoke Id parameter in ITU and ANSI for an INVOKE component.
      * The Invoke Id for ITU and Correlation Id for ANSI in the RESULT,
      * REJECT and ERROR components.
      * @serial m_invokeId 
      * a default serializable field.  */
    protected int m_invokeId = 0;
    /** Indicates if this Indication Component is the last component of
      * a transaction message.
      * @serial m_lastComponent 
      * a default serializable field.  */
    protected boolean m_lastComponent = false;
    /** Whether the Invoke ID is present.
      * @serial m_invokeIdPresent 
      * a default serializable field.  */
    protected boolean m_invokeIdPresent = false;
    /** Whether the Dialogue ID is present.
      * @serial m_dialogueIdPresent 
      * a default serializable field.  */
    protected boolean m_dialogueIdPresent = false;
    /** Whether the Last Component is present.
      * @serial m_lastComponentPresent 
      * a default serializable field.  */
    protected boolean m_lastComponentPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
