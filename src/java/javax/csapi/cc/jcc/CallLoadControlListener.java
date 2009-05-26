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
    Interface for notifying load control related changes happening in a {@link JccProvider} event. These changes are
    reported as events to the {@link CallLoadControlListener} method corresponding to the type of event.
    Applications must instantiate an object which implements this interface and then use
    the {@link JccProvider#addCallLoadControlListener(CallLoadControlListener)} method to register the 
    object to receive all future
    events associated with the JccProvider object.
    @since 1.0
*/
public interface CallLoadControlListener extends java.util.EventListener
{
    /**
        This method indicates that the network has detected overload and may have automatically
        imposed load control on calls requested to a particular address range or calls 
        made to a particular destination. 
        @param loadcontrolevent CallLoadControlEvent with event ID {@link CallLoadControlEvent#PROVIDER_CALL_OVERLOAD_ENCOUNTERED}.
    */
    
    public void providerCallOverloadEncountered(CallLoadControlEvent loadcontrolevent);
    
    /**
    
        This method indicates that the network has detected that the  overload has ceased  and has automatically
        removed load control on calls requested to a particular address range or calls 
        made to a particular destination. 
        @param loadcontrolevent CallLoadControlEvent with event ID {@link CallLoadControlEvent#PROVIDER_CALL_OVERLOAD_CEASED}.
    */

    public void providerCallOverloadCeased(CallLoadControlEvent loadcontrolevent);
}


