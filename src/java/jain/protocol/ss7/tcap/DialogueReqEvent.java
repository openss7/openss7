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
 * File Name     : DialogueReqEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     01/11/2000  Phelim O'Doherty    Updated javadoc to clarify purpose
 *                                         of the Dialogue Id/Portion and QOS
 *                                         fields.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import jain.protocol.ss7.tcap.dialogue.DialoguePortion;

/**
 * A DialogueReqEvent is the superclass of all Dialogue Request primitives,
 * which represent TCAP Dialogue Requests sent from a <a
 * href="JainTcapProvider.html">JainTcapProvider</a> to a <a
 * href="JainTcapListener.html">JainTcapListener</a> . It affords a method of
 * generic event passing, while enforcing tight control over event flow between
 * the Listener and the Provider. This type of Event will be processed by the
 * <a href="JainTcapListener.html#processDialogueIndEvent(jain.protocol.ss7.tcap.DialogueIndEvent)">
 * processDialogueIndEvent method</a> of the <a href="JainTcapListener.html">
 * JainTcapListener</a> . This class implements the <B>java.lang.Cloneable</B>
 * interface, therefore all Dialogue Request primitives that extend this Event
 * can be cloned using the <B>clone()</B> method inherited from <B>
 * java.lang.Object</B> .
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public abstract class DialogueReqEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {

    /**
    * Constructs a new DialogueReqEvent.
    *
    * @param  source the Object Source supplied to the constructor
    */
    public DialogueReqEvent(Object source) {
        super(source);
    }

    /**
    * Sets the source of this event. This method may be used to overirde the
    * setting of the event source through the constructor, this provides the
    * ability to clone the Event and change the Event source at any time.
    *
    * @param  source The new Object Source
    */
    public void setSource(Object source) {
        this.source = source;
    }

    /**
    * Sets the Dialogue Id parameter, of this Dialogue Request Event, which is a
    * reference identifier that is used to uniquely identify a dialogue and all
    * associated transaction messages and components with that Dialogue. It is
    * determined by the local TCAP layer and given to the application for all
    * transactions received or initiated by the local application. <p>
    *
    * <b>Note to developers</b> :- If this is the first BEGIN transaction message
    * of a new dialogue a call should be made to the <a
    * href="JainTcapProvider.html#getNewDialogueId()">getNewDialogueID method
    * </a>of the <a href="JainTcapProvider.html">JainTcapProvider</a> in order to
    * get returned a free Dialogue Id from the underlying stack implementation.
    *
    * @param  dialogueId The new Dialogue Identifier
    */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }

    /**
    * Sets the Dialogue Portion of this Dialogue handling request primitive. The
    * Dialogue Portion provides a mechanism to determine the interpretation of
    * TCAP message information, security and confidentiality at a remote TCAP. A
    * Dialogue Portion is forbidden in an ANSI 1992 implementation of the JAIN
    * TCAP API.
    *
    * @param  dialoguePortion The new Dialogue Portion
    */
    public void setDialoguePortion(DialoguePortion dialoguePortion) {
        m_dialoguePortion = dialoguePortion;
        m_dialoguePortionPresent = true;
    }

    /**
    * Sets the Quality of Service of this Dialogue Request Event, the Quality of
    * Service parameter is initialized to zero, therefore by default can be
    * interpreted as "Not Used".
    *
    * @param  qualityOfService  The new QualityOfService value
    */
    public void setQualityOfService(byte qualityOfService) {
        m_qualityOfService = qualityOfService;
        m_qualityOfServicePresent = true;
    }

    /**
    * Gets the Dialogue Id parameter, which is a reference identifier that is
    * used to uniquely identify a dialogue and all associated transaction
    * messages and components with that Dialogue. It is determined by the local
    * TCAP layer and given to the application for all transactions received or
    * initiated by the local application.
    *
    * @return the <var>dialogueId</var> that uniquely identifies the Dialogue
    * of this Dialogue Request Event.
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * this JAIN Mandatory parameter has not been set.
    */
    public int getDialogueId() throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent == true) {
            return (m_dialogueId);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Indicates if the Dialogue Portion parameter is present in this Event. <p>
    *
    * <b>Note to developers</b> :- A Dialogue Portion can only be present in an
    * ITU 1993, ITU 1997 and ANSI 1996 implementation of the JAIN TCAP API.
    *
    * @return <var>true</var> if Dialogue Portion has been set, <var>false
    * </var>otherwise.
    */
    public boolean isDialoguePortionPresent() {
        return m_dialoguePortionPresent;
    }

    /**
    * Gets the Dialogue Portion of this Dialogue handling request primitive. The
    * Dialogue Portion provides a mechanism to determine the interpretation of
    * TCAP message information, security and confidentiality at a remote TCAP. A
    * Dialogue Portion is forbidden in a ANSI 1992 implementaion of the JAIN TCAP
    * API.
    *
    * @return  the <var>dialoguePortion</var> of this Dialogue handling
    * request primitive.
    * @exception  ParameterNotSetException  this exception is thrown if this JAIN
    * Optional parameter has not yet been set
    */
    public DialoguePortion getDialoguePortion() throws ParameterNotSetException {
        if (m_dialoguePortionPresent == true) {
            return (m_dialoguePortion);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Indicates if the Quality of Service is present in this Event.
    *
    * @return    true if Quality of Service has been set.
    */
    public boolean isQualityOfServicePresent() {
        return m_qualityOfServicePresent;
    }

    /**
    * Gets the Quality of Service parameter of this Dialogue Requeat event.
    * Quality of Service is an SCCP parameter that is required from the
    * application.
    *
    * @return the <var>qualityOfService</var> parameter of the Event.
    * @exception  ParameterNotSetException  this exception is thrown if this
    * parameter has not yet been set
    */
    public byte getQualityOfService() throws ParameterNotSetException {
        if (m_qualityOfServicePresent == true) {
            return (m_qualityOfService);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This abstract method defines the 'getPrimitiveType' method that must be
    * implemented by its subclasses in order to return the type of that
    * subclassed Dialogue Request primitive.
    *
    * @return    The Primitive Type of the Event
    */
    public abstract int getPrimitiveType();

    /**
    * String representation of class DialogueReqEvent
    *
    * @return    String provides description of class DialogueReqEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\n\ndialogueId = ");
        buffer.append(m_dialogueId);
        buffer.append("\n\ndialogueIdPresent = ");
        buffer.append(m_dialogueIdPresent);
        buffer.append("\n\nqualityOfService = ");
        buffer.append(m_qualityOfService);
        buffer.append("\n\nqualityOfServicePresent = ");
        buffer.append(m_qualityOfServicePresent);
        buffer.append("\n\ndialoguePortion = ");
        if (m_dialoguePortion != null) {
            buffer.append(m_dialoguePortion.toString());
        } else {
            buffer.append("value is null");
        }
        buffer.append("\n\ndialoguePortionPresent = ");
        buffer.append(m_dialoguePortionPresent);

        return buffer.toString();
    }

    /**
    * The Dialogue Id parameter.
    *
    * @serial    m_dialogueId - a default serializable field
    */
    protected int m_dialogueId = 0;

    /**
    * @serial    m_dialogueIdPresent - a default serializable field
    */
    protected boolean m_dialogueIdPresent = false;

    /**
    * The Quality of Service parameter for Dialogue Request Events.
    *
    * @serial    m_qualityOfService - a default serializable field We code this
    * parameter as: 0 - Not used 1 - Sequence guaranteed 2 - Return message
    * on error: 3 - Both 'Sequence guaranteed' and 'Return message on error'
    * Therefore a value of 3(byte equivalent is used) = Both 'Sequence
    * guaranteed' and 'Return message on error' for a connectionless SCCP
    * network service.
    */
    protected byte m_qualityOfService = 0;

    /**
    * @serial    m_qualityOfServicePresent - a default serializable field
    */
    protected boolean m_qualityOfServicePresent = false;

    /**
    * The Dialogue Portion of this Dialogue handling request primitive.
    *
    * @serial    m_dialoguePortion - a default serializable field
    */
    protected DialoguePortion m_dialoguePortion = null;

    /**
    * @serial    m_dialoguePortionPresent - a default serializable field
    */
    protected boolean m_dialoguePortionPresent = false;
}

