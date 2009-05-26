package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
    The Event interface is the parent of all JCC  Event interfaces.
    Event interfaces within each package are organized in a hierarchical fashion. 
    <p>
    Event objects correspond to the object which is undergoing a state change; the specific
    state change is conveyed to the application in two ways.
    <p>
    First, the implementation reports the event to a particular method in a particular Listener
    interface to a listening object; generally the method corresponds to a particular state change. 
    <p>
    Second, the event that is presented to the method has an identification integer which indicates
    the specific state change. The {@link #getID()} method returns this identification number for 
    each event. The actual event identification integer values that may be conveyed by the individual
    event object are defined in each of the specific event interfaces. 
    <p>
    Each event caries a cause or a reason why the event happened. The {@link #getCause()} method 
    returns this cause value. 
    @since 1.0b
    */
   
public interface JccEvent {
    /**
        Returns the cause associated with this event. Every event has a cause. The various cause
        values are defined as public static final variables in this interface. 
        @return the cause of the event. 
    */
    public int getCause();
    /**
        Returns the id of event. Every event has an id.
        @return the id of the event.
    */
    public int getID();
    /**
        Returns the event source of the event. 
        @return The object sending the event. 
    */
    public Object getSource();
    /**
    Cause code indicating a normal operation.     
    */
    public static final int CAUSE_NORMAL=100;
    /**
    Cause code indicating the cause was unknown.     
    */
    public static final int CAUSE_UNKNOWN=101;
    /**
    Cause code indicating the user has terminated call. This can be either because
    of the user going 
    on-hook or because the call has been terminated by the service logic.     
    */
    public static final int CAUSE_CALL_CANCELLED=102;
    /**
    Cause code indicating the destination is not available.     
    */
    public static final int CAUSE_DEST_NOT_OBTAINABLE=103;
    /**
    Cause code indicating that a call has encountered an incompatible destination.     
    */
    public static final int CAUSE_INCOMPATIBLE_DESTINATION=104;
    /**
    Cause code indicating that a call has encountered an inter-digit timeout while dialing.     
    */
    public static final int CAUSE_LOCKOUT=105;
    /**
    Cause code indicating a new call.     
    */
    public static final int CAUSE_NEW_CALL=106;
    /**
    Cause code indicating that resources were not available.     
    */
    public static final int CAUSE_RESOURCES_NOT_AVAILABLE=107;
    /**
    Cause code indicating that a call has encountered network congestion.     
    */
    public static final int CAUSE_NETWORK_CONGESTION=108;
    /**
    Cause code indicating that a call could not reach a destination network.     
    */
    public static final int CAUSE_NETWORK_NOT_OBTAINABLE=109;
    /**
    Cause code indicating that the event is part of a snapshot of the current state of the call.     
    */
    public static final int CAUSE_SNAPSHOT=110;
    /**
    Cause code indicating the cause was because of call being redirected.  
    If this cause code occurs, the event id. is always 
    {@link JccConnectionEvent#CONNECTION_DISCONNECTED} and the connection 
    returned by {@link JccConnectionEvent#getConnection()} is a terminating 
    connection.
    */
    public static final int CAUSE_REDIRECTED=111;
    /**
    Cause code indicating that the network needs more addressing information to complete the call.
    */
    public static final int CAUSE_MORE_DIGITS_NEEDED=112;
    /**
    The user is busy. 
    This value may be returned by {@link JccEvent#getCause()}.
    */
    public static final int CAUSE_BUSY=113;
    /** 
    No answer was received. 
    This value may be returned by {@link JccEvent#getCause()}.
    */
	public static final int CAUSE_NO_ANSWER=114;
	/** 
	A disconnect was received. 
	This value may be returned by {@link JccEvent#getCause()}.
	*/
	public static final int CAUSE_CALL_RESTRICTED=115; 
	/** 
	A general network failure occurred. 
	This value may be returned by {@link JccEvent#getCause()}.
	*/
	public static final int CAUSE_GENERAL_FAILURE=116;
	/** 
	The connection was released because an activity timer expired. 
	This value may be returned by {@link JccEvent#getCause()}.
	*/
	public static final int CAUSE_TIMER_EXPIRY=117;
	/** 
	The user isn't available in the network. This means that the number isn't allocated or that the user isn't registered.
	This value may be returned by {@link JccEvent#getCause()}.
	*/
	public static final int CAUSE_USER_NOT_AVAILABLE=118;
}

