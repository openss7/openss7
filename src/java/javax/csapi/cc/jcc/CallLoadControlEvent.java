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
 *  Version       : 1.0b
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


/**
    This is the base interface for all Load Control related Events. All events 
    which pertain to Load control must extend this interface. Events
    which extend this interface are reported via the {@link CallLoadControlListener} interface. 
    @since 1.0
*/

public interface CallLoadControlEvent extends JccEvent
{

/**
        indicates that the network has detected overload.
        This constant 
        indicates a specific event passed via a CallLoadControlEvent event and is reported on the
        CallLoadControlListener interface.
*/
    public static final int PROVIDER_CALL_OVERLOAD_ENCOUNTERED = 118;

/**
        indicates that the network has detected that the overload has ceased.
        This constant 
        indicates a specific event passed via a CallLoadControlEvent event and is reported on the
        CallLoadControlListener interface.

*/
    public static final int PROVIDER_CALL_OVERLOAD_CEASED = 119;

}
