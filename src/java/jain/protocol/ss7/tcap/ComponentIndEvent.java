/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : ComponentIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     16/10/2000  Phelim O'Doherty    Clarifed function of lastComponet
 *                                         parameter and invoke Id mapping in
 *                                         comment.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
/**
 * A ComponentIndEvent is the super class of all Component Indication
 * primitives, which represent TCAP Component Indications sent from a <a
 * href="JainTcapProvider.html">JainTcapProvider</a> to a <a
 * href="JainTcapListener.html">JainTcapListener</a> . It affords a method of
 * generic event passing, while enforcing tight control over event flow between
 * the Listener and the Provider. This type of Event will be processed by the
 * <a href="JainTcapListener.html#processComponentIndEvent(jain.protocol.ss7.tcap.ComponentIndEvent)">
 * processComponentIndEvent method</a> of the <a href="JainTcapListener.html">
 * JainTcapListener</a> . This class implements the <B>java.lang.Cloneable</B>
 * interface, therefore all Component Indication primitives that extend this
 * Event can be cloned using the <B>clone()</B> method inherited from <B>
 * java.lang.Object</B> .
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public abstract class ComponentIndEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {

    /**
    * Constructs a new ComponentIndEvent.
    *
    * @param  source the new object source supplied to the constructor
    */
    public ComponentIndEvent(Object source) {
        super(source);
    }

    /**
    * Sets the source of this event. This method may be used to override the
    * setting of the event source through the constructor, this provides the
    * ability to clone the Event and change the Event source at any time.
    *
    * @param  source The new Object Source value
    */
    public void setSource(Object source) {
        this.source = source;
    }

    /**
    * Sets the Dialogue Id parameter of this Component Indication Event. Dialogue
    * Id is a reference identifier, which identifies a specific dialogue or
    * transaction and all associated components within that dialogue or
    * transaction.
    *
    * @param  dialogueId  The new Dialogue Identifier
    */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }

    /**
    * Sets the Invoke Id parameter of the indication component. The mapping rules
    * for the Invoke Identifier in each of the components in the Jain Tcap Api
    * are as follows:-
    *  <UL>
    *    <LI> INVOKE Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in both the ITU and ANSI recommendations for the Invoke
    *    component.
    *    <LI> RESULT Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Result component.
    *    <LI> REJECT Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Reject component.
    *    <LI> ERROR Component :- The Invoke Identifier is the Invoke Identifier as
    *    specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Error component.
    *  </UL>
    *
    * @param  invokeId The new Invoke Identifier
    */
    public void setInvokeId(int invokeId) {
        m_invokeId = invokeId;
        m_invokeIdPresent = true;
    }

    /**
    * Sets this indication Component as the last component in a Transaction
    * message for example BEGIN or CONTINUE. <p>
    *
    * <b>Note to Developers:-</b> This should not be confused with the <var>
    * lastInvokeEvent</var> parameter in an INVOKE Component which is a flag for
    * an ANSI implementation of the JAIN TCAP API to determine the type of
    * component, either INVOKE_NL or INVOKE_L. For example in an ANSI INVOKE
    * Indicaton the last component in the transaction message could be an
    * INVOKE_NL, were an INVOKE_L will follow in the next message segment,
    * therefore the <var>lastComponent</var> parameter is true, while the <var>
    * lastInvokeEvent</var> is false. <p>
    *
    * By default this component will <B>not </B>be the last Component, that is the
    * lastComponent is set to <b>false</b> by default.
    *
    * @param  lastComponent  The new LastComponent value
    */
    public void setLastComponent(boolean lastComponent) {
        m_lastComponent = lastComponent;
    }

    /**
    * Gets the Dialogue Id parameter of the indication component, which is a
    * reference identifier which identifies a specific dialogue or transaction
    * and all associated components within that dialogue or transaction.
    *
    * @return the Dialogue Id of the Component indication
    * @exception  ParameterNotSetException this exception is thrown if
    * this parameter is a JAIN optional parameter and has not yet been set.
    * The Dialogue Id is JAIN Optional for the <a href="InvokeIndEvent.html">
    * Invoke Indication</a> .
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * the Dialogue Id is a JAIN mandatory parameter for the specific
    * Component Indication. The Dialogue Id is JAIN Mandatory for all
    * component indications except for the <a href="InvokeIndEvent.html">
    * Invoke Indication</a> .
    */
    public abstract int getDialogueId() throws ParameterNotSetException, MandatoryParameterNotSetException;

    /**
    * Indicates if the Invoke Id is present in this Indication event.
    *
    * @return <var>true</var> if Invoke Id has been set, <var>false</var>
    * otherwise.
    */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }

    /**
    * Gets the Invoke Id parameter of the component indication. Invoke Id
    * identifies the operation invocation and its result. The mapping rules for
    * the Invoke Identifier in each of the components in the Jain Tcap Api are as
    * follows:-
    *  <UL>
    *    <LI> INVOKE Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in both the ITU and ANSI recommendations for the Invoke
    *    component.
    *    <LI> RESULT Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Result component.
    *    <LI> REJECT Component :- The Invoke Identifier is the Invoke Identifier
    *    as specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Reject component.
    *    <LI> ERROR Component :- The Invoke Identifier is the Invoke Identifier as
    *    specified in the ITU recommendation and the Correlation Identifier as
    *    specified in the ANSI recommendation for the Error component.
    *  </UL>
    *
    *
    * @return the Invoke Id of the Indication event.
    * @exception  ParameterNotSetException this exception is thrown if
    * this is a JAIN Optional parameter in the specific component indication
    * and has not been set.
    * @exception  MandatoryParameterNotSetException if this is a JAIN Mandatory
    * parameter in the specific component indication and has not been set.
    */
    public abstract int getInvokeId() throws ParameterNotSetException, MandatoryParameterNotSetException;

    /**
    * Indicates if this indication Component is the last component in a
    * Transaction message i.e. BEGIN or CONTINUE. <p>
    *
    * <b>Note to Developers:-</b> This should not be confused with the <var>
    * lastInvokeEvent</var> parameter in an INVOKE Component which is a flag for
    * an ANSI implementation of the JAIN TCAP API to determine the type of
    * component, either INVOKE_NL or INVOKE_L. For example in an ANSI INVOKE
    * Indicaton the last component in the transaction message could be an
    * INVOKE_NL, were an INVOKE_L will follow in the next message segment,
    * therefore the <var>lastComponent</var> parameter is true, while the <var>
    * lastInvokeEvent</var> is false. <p>
    *
    * By default the <var>lastComponent</var> parameter is set to <b>false</b> .
    *
    *  @return    <var>true</var> if this is the last component, <var>false</var>
    *      otherwise.
    */
    public boolean isLastComponent() {
        return (m_lastComponent);
    }

    /**
    * This abstract method defines the 'getPrimitiveType' method that must be
    * implemented by its subclasses in order to return the type of that
    * subclassed Component Indication primitive.
    *
    * @return    The PrimitiveType value
    */
    public abstract int getPrimitiveType();

    /**
    * String representation of class ComponentIndEvent
    *
    * @return    String provides description of class ComponentIndEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\n\ndialogueId = ");
        buffer.append(m_dialogueId);
        buffer.append("\n\ninvokeId = ");
        buffer.append(m_invokeId);
        buffer.append("\n\nlastComponent = ");
        buffer.append(m_lastComponent);
        buffer.append("\n\ninvokeIdPresent = ");
        buffer.append(m_invokeIdPresent);
        buffer.append("\n\ndialogueIdPresent = ");
        buffer.append(m_dialogueIdPresent);
        buffer.append("\n\nlastComponentPresent = ");
        buffer.append(m_lastComponentPresent);

        return buffer.toString();
    }

    /**
    * The Dialogue Id parameter of the indication component
    *
    * @serial    m_dialogueId - a default serializable field
    */
    protected int m_dialogueId = 0;

    /**
    * The Invoke Id parameter in ITU and ANSI for an INVOKE component. <BR>
    * The Invoke Id for ITU and Correlation Id for ANSI in the RESULT, REJECT and
    * ERROR components.
    *
    * @serial    m_invokeId - a default serializable field
    */
    protected int m_invokeId = 0;

    /**
    *  Indicates if this Indication Component is the last component of a
    *  transaction message.
    *
    * @serial    m_lastComponent - a default serializable field
    */
    protected boolean m_lastComponent = false;

    /**
    * @serial    m_invokeIdPresent - a default serializable field
    */
    protected boolean m_invokeIdPresent = false;

    /**
    * @serial    m_dialogueIdPresent - a default serializable field
    */
    protected boolean m_dialogueIdPresent = false;

    /**
    * @serial    m_lastComponentPresent - a default serializable field
    */
    protected boolean m_lastComponentPresent = false;
}

