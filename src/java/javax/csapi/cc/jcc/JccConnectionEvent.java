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
This is the base interface for all {@link JccConnection} related events.  
@since 1.0
*/
public interface JccConnectionEvent extends JccCallEvent {
    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#AUTHORIZE_CALL_ATTEMPT}. 
    */
    public static final int CONNECTION_AUTHORIZE_CALL_ATTEMPT     = 113;

    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#ADDRESS_COLLECT}. 
    */
    public static final int CONNECTION_ADDRESS_COLLECT          = 114;

    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#ADDRESS_ANALYZE}. 

    */
    public static final int CONNECTION_ADDRESS_ANALYZE           = 115;

    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#CALL_DELIVERY}. 

    */
    public static final int CONNECTION_CALL_DELIVERY            = 116;

    /**
        This event indicates that a mid call event has occurred in the 
        {@link JccConnection#CONNECTED} state.
		@since 1.0b
    */
    public static final int CONNECTION_MID_CALL                  = 123;
//---------------------------------------------------------------------------------------------
// FROM JCP

    /**
        Returns the JccConnection associated with this event.
    @return JccConnection associated with this JccConnection Event. The result of this call might be a cast of the result of {@link JccEvent#getSource()}.
	@since 1.0b
    */
    public JccConnection getConnection( );
    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#ALERTING}. 
		@since 1.0b
    */
    public static int CONNECTION_ALERTING                   = 104;
    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#CONNECTED}.
		@since 1.0b
    */
    public static int CONNECTION_CONNECTED                  = 105;
    /**
        This event indicates that a new JccConnection object has been created
        in the {@link JccConnection#IDLE} state.
		@since 1.0b
    */
    public static int CONNECTION_CREATED     = 106;
    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#DISCONNECTED}.
		@since 1.0b
    */
    public static int CONNECTION_DISCONNECTED               = 107;
    /**
        This event indicates that the state of the JccConnection object has changed to 
        {@link JccConnection#FAILED}. 
		@since 1.0b
    */
    public static int CONNECTION_FAILED                     = 108;
//    /**
//    This event indicates that the state of the JcpConnection object has changed
//    to {@link JccConnection#INPROGRESS}.
//       
//    */
//    public static int CONNECTION_INPROGRESS                 = 109;
//    /**
//        This event indicates that the state of the JccConnection object has changed to 
//        {@link JccConnection#UNKNOWN}. 
//		@since 1.0b
//    */
//    public static int CONNECTION_UNKNOWN                     = 110;

   
}

