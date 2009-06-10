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
 * File Name     : ComponentReqEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     31/10/2000  Phelim O'Doherty    Clarified invokeId mappings in comment.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import java.util.EventObject;
/**
 * A ComponentReqEvent is the superclass of all Component request primitives,
 * which represent TCAP Component requests sent from a <a
 * href="JainTcapListener.html">JainTcapListener</a> to a <a
 * href="JainTcapProvider.html">JainTcapProvider</a> . It affords a method of
 * generic event passing, while enforcing tight control over event flow between
 * the Listener and the Provider. This type of Event will be processed by the
 * <a href="JainTcapProvider.html#sendComponentReqEvent(jain.protocol.ss7.tcap.ComponentReqEvent)">
 * sendComponentReqEvent() method</a> of the <a href="JainTcapProvider.html">
 * JainTcapProvider</a> . This class implements the <B>Cloneable</B>
 * interface, therefore all Component Request primitives that extend this Event
 * can be cloned using the <B>clone()</B> method inherited from <B>
 * Object</B> .
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public abstract class ComponentReqEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {

    /**
    * Constructs a new ComponentReqEvent.
    *
    * @param  source  the new source object supplied to the constructor
    */
    public ComponentReqEvent(Object source) {
        super(source);
    }

    /**
    * Sets the source of this event. This method may be used to override the
    * setting of the event source through the constructor, this provides the
    * ability to clone the Event and change the Event source at any time.
    *
    * @param  source  The new object source
    */
    public void setSource(Object source) {
        this.source = source;
    }

    /**
    * Sets the Dialogue Id parameter of this Component Request Event. Dialogue Id
    * is a reference identifier, which identifies a specific dialogue or
    * transaction and all associated components within that dialogue or
    * transaction.
    *
    * Update v1.1 :- There is no isDialogueIdPresent method in this super class
    * because the dialogueId parameter is mandatory in all children.
    *
    * @param  dialogueId  The new Dialogue Identifier
    */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }

    /**
    * Sets the Invoke Id parameter of the request component. Invoke Id identifies
    * the operation invocation and its result. The mapping rules for the Invoke
    * Identifier in each of the components in the Jain Tcap Api are as follows:-
    *
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
    * Update v1.1 :- There is no isInvokeIdPresent method in this super class
    * because not all children have an optional InvokeId, namely TimerReset and
    * UserCancel.
    *
    * @param  invokeId  The new Invoke Identifier
    */
    public void setInvokeId(int invokeId) {
        m_invokeId = invokeId;
        m_invokeIdPresent = true;
    }

    /**
    * Gets the Dialogue Id parameter of the Component Request Event, which is a
    * reference identifier which identifies a specific dialogue or transaction
    * and all associated components within that dialogue or transaction.
    *
    * @return the Dialogue Id of the request event
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * this JAIN Mandatory parameter has not been set
    */
    public int getDialogueId() throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent == true) {
            return (m_dialogueId);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Invoke Id parameter of the request component. Invoke Id identifies
    * the operation invocation and its result. The mapping rules for the Invoke
    * Identifier in each of the components in the Jain Tcap Api are as follows:-
    *
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
    * @return the Invoke Id of the Component Request event.
    * @exception  ParameterNotSetException  this exception is thrown if this
    *      parameter has not yet been set.
    */
    public int getInvokeId() throws ParameterNotSetException {
        if (m_invokeIdPresent == true) {
            return (m_invokeId);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This abstract method defines the 'getPrimitiveType' method that must be
    * implemented by its subclasses in order to return the type of that
    * subclassed Component Request primitive.
    *
    * @return    The Primitive Type of the Event
    */
    public abstract int getPrimitiveType();

    /**
    * String representation of class jain.protocol.ss7.tcap.ComponentReqEvent
    *
    * @return    String provides description of class ComponentReqEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\n\ndialogueId = ");
        buffer.append(m_dialogueId);
        buffer.append("\n\ndialogueIdPresent = ");
        buffer.append(m_dialogueIdPresent);
        buffer.append("\n\ninvokeId = ");
        buffer.append(m_invokeId);
        buffer.append("\n\ninvokeIdPresent = ");
        buffer.append(m_invokeIdPresent);

        return buffer.toString();
    }

    /**
    * The Dialogue Id parameter of the Component Request Event.
    *
    * @serial    m_dialogueId - a default serializable field
    */
    protected int m_dialogueId = 0;

    /**
    * @serial    m_dialogueIdPresent - a default serializable field
    */
    protected boolean m_dialogueIdPresent = false;

    /**
    * The Invoke Id parameter for ITU and ANSI for an INVOKE component. <BR>
    * The Invoke Id for ITU and Correlation Id for ANSI in the RESULT, REJECT and
    * ERROR components.
    *
    * @serial    m_invokeId - a default serializable field
    */
    protected int m_invokeId = 0;

    /**
    * @serial    m_invokeIdPresent - a default serializable field
    */
    protected boolean m_invokeIdPresent = false;
}

