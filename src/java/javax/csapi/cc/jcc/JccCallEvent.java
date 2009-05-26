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
    This is the base interface for all {@link JccCall}-related events.   
    @since 1.0
    */
public interface JccCallEvent extends JccEvent {
    
    /**
    The CALL_SUPERVISE_START event indicates that the supervision of the call has started.
    The policy followed is that given in {@link JccCall#superviseCall(JccCallListener,double,int)}.
    */
    public static final int CALL_SUPERVISE_START =111;
    /**
    The CALL_SUPERVISE_END event indicates that the supervision of the call has ended.
    The policy followed is that given in {@link JccCall#superviseCall(JccCallListener,double,int)}.
    */        
    public static final int CALL_SUPERVISE_END=112;
	
	//--------------------------------------------------------------------
	
    /**
        Returns the JccCall object associated with this event.

    @return JccCall represents the JccCall object associated with this JccCall event. The result of this call might be a cast of the result of {@link JccEvent#getSource()}.
    		@since 1.0b
    */
    public JccCall getCall(  );

    /**
        The CALL_ACTIVE event 
        indicates that the state of the Call object has changed to {@link JccCall#ACTIVE}.
        <br>
        This constant corresponds to a specific call state change, it is passed via a 
        JccCallEvent event and is reported to the {@link JccCallListener#callActive(JccCallEvent)} method.
        		@since 1.0b
    */
    public static int CALL_ACTIVE         = 101;

    /**
        The CALL_INVALID event 
        indicates that the state of the JccCall object has changed to {@link JccCall#INVALID}.
        <br>
        This constant corresponds to a specific call state change, it is passed via a 
        JccCallEvent event and is reported to the {@link JccCallListener#callInvalid(JccCallEvent)} method.
        @since 1.0b
    */
    public static int CALL_INVALID        = 102;
    /**
         The CALL_EVENT_TRANSMISSION_ENDED event indicates that the application will no 
         longer receive JccCall events from this call on 
         the instance of the JccCallListener. 
        <br>
        This constant is passed via a 
        JccCallEvent event and is reported to the {@link JccCallListener#callEventTransmissionEnded(JccCallEvent)} method.
        @since 1.0b
    */
    public static int CALL_EVENT_TRANSMISSION_ENDED = 103;
    /**
        The CALL_CREATED event 
        indicates that the JccCall object has been created and is in the 
        {@link JccCall#IDLE} state.
        <br>
        This constant corresponds to a specific call state change, it is passed via a 
        JccCallEvent event and is reported to the {@link JccCallListener#callCreated(JccCallEvent)} method.
        @since 1.0b
    */
    public static int CALL_CREATED        = 118;
    	
	
}

