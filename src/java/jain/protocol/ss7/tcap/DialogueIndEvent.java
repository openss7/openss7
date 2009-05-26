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
 * File Name     : DialogueIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     16/10/2000  Phelim O'Doherty    Clarified Dialogue Id/Portion use in
 *                                         comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import jain.protocol.ss7.tcap.dialogue.DialoguePortion;

/**
 * A DialogueIndEvent is the superclass of all Dialogue Indication primitives,
 * which represent TCAP Dialogue Indications sent from a <a
 * href="JainTcapProvider.html">JainTcapProvider</a> to a <a
 * href="JainTcapListener.html">JainTcapListener</a> . It affords a method of
 * generic event passing, while enforcing tight control over event flow between
 * the Listener and the Provider. This type of Event will be processed by the
 * <a href="JainTcapListener.html#processDialogueIndEvent(jain.protocol.ss7.tcap.DialogueIndEvent)">
 * processDialogueIndEvent method</a> of the <a href="JainTcapListener.html">
 * JainTcapListener</a> . This class implements the <B>java.lang.Cloneable</B>
 * interface, therefore all Dialogue Indication primitives that extend this
 * Event can be cloned using the <B>clone()</B> method inherited from <B>
 * java.lang.Object</B> .
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public abstract class DialogueIndEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {

    /**
    * Constructs a new DialogueIndEvent.
    *
    * @param  source the object source supplied to the constructor
    */
    public DialogueIndEvent(Object source) {
        super(source);
    }

    /**
    * Sets the source of this event. This method may be used to override the
    * setting the event source through the constructor, this provides the ability
    * to clone the Event and change the Event source at any time.
    *
    * @param  source The new Object source
    */
    public void setSource(Object source) {
        this.source = source;
    }

    /**
    * Sets the Dialogue Id parameter of this Dialogue Indication Event, which is
    * a reference identifier that is used to uniquely identify a dialogue and all
    * associated transaction messages and components with that Dialogue. It is
    * determined by the local TCAP layer and given to the application for all
    * transactions received or initiated by the local application. <p>
    *
    * <b>Note to developers</b> :- A Dialogue Id is forbidden in a UNI Indication
    * message, therefore this method should not be used in a UnidirectionalInd
    * Event.
    *
    * @param  dialogueId The new Dialogue Identifier
    */
    public void setDialogueId(int dialogueId) {
        m_dialogueId = dialogueId;
        m_dialogueIdPresent = true;
    }

    /**
    * Sets the Dialogue Portion of this Dialogue handling indication primitive.
    * The Dialogue Portion provides a mechanism to determine the interpretation
    * of TCAP message information, security and confidentiality at a remote TCAP.
    * A Dialogue Portion is forbidden in an ANSI 1992 implementation of the JAIN
    * TCAP API.
    *
    * @param  dialoguePortion The new Dialogue Portion
    */
    public void setDialoguePortion(DialoguePortion dialoguePortion) {
        m_dialoguePortion = dialoguePortion;
        m_dialoguePortionPresent = true;
    }

    /**
    * Gets the Dialogue Id parameter, which is a reference identifier that is
    * used to uniquely identify a dialogue and all associated transaction
    * messages and components with that Dialogue. It is determined by the local
    * TCAP layer and given to the application for all transactions received or
    * initiated by the local application. <p>
    *
    * <b>Note to developers</b> :- A Dialogue Id is forbidden in a UNI Indication
    * message, therefore this method should not be used in a UnidirectionalInd
    * Event.
    *
    * @return the <var>dialogueId</var> that uniquely identifies the Dialogue
    * of this Dialogue Indication Event.
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * this JAIN Mandatory Parameter has not been set.
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
    * Gets the Dialogue Portion of this Dialogue handling indication primitive.
    * The Dialogue Portion provides a mechanism to determine the interpretation
    * of TCAP message information, security and confidentiality at a remote TCAP.
    * A Dialogue Portion is forbidden in a ANSI 1992 implementaion of the JAIN
    * TCAP API.
    *
    * @return the <var>dialoguePortion</var> of this Dialogue handling
    * indication primitive.
    * @exception  ParameterNotSetException  this exception is thrown if this
    * Optional JAIN parameter has not been set.
    */
    public DialoguePortion getDialoguePortion() throws ParameterNotSetException {
        if (m_dialoguePortionPresent == true) {
            return (m_dialoguePortion);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This abstract method defines the 'getPrimitiveType' method that must be
    * implemented by its subclasses in order to return the type of that
    * subclassed Dialogue Indication primitive.
    *
    * @return    The Primitive Type of the Event
    */
    public abstract int getPrimitiveType();

    /**
    * String representation of class DialogueIndEvent
    *
    * @return    String provides description of class DialogueIndEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\n\ndialogueId = ");
        buffer.append(m_dialogueId);
        buffer.append("\n\ndialogueIdPresent = ");
        buffer.append(m_dialogueIdPresent);
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
    * The Dialogue Id parameter, which uniquely identifies this Dialogue
    * Indication Event to a specific Dialogue between two TCAP Users.
    *
    * @serial    m_dialogueId - a default serializable field
    */
    protected int m_dialogueId = 0;

    /**
    * @serial    m_dialogueIdPresent - a default serializable field
    */
    protected boolean m_dialogueIdPresent = false;

    /**
    * The Dialogue Portion of this Dialogue handling indication primitive, which
    * is forbidden in an ANSI 1992 implementation of the JAIN TCAP API.
    *
    * @serial    m_dialoguePortion - a default serializable field
    */
    protected DialoguePortion m_dialoguePortion = null;

    /**
    * Should always be false in an ANSI 1992 implementation of the JAIN TCAP API.
    *
    * @serial    m_dialoguePortionPresent - a default serializable field
    */
    protected boolean m_dialoguePortionPresent = false;
}

