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
    An instance of this MidCallData interface is returned by {@link JccConnection#getMidCallData()} after 
    an event with id {@link JccConnectionEvent#CONNECTION_MID_CALL} occured.  This interface provides access
    to the type of service code and (optionally) a string (with e.g. digits) associated with it.
    @since 1.0b
    */
public interface MidCallData {

   /**
    Returns the type of the mid call event.
    @return an int representing the type of the mid call event. Only {@link #SERVICE_CODE_UNDEFINED}, 
    {@link #SERVICE_CODE_DIGITS}, {@link #SERVICE_CODE_FACILITY}, {@link #SERVICE_CODE_U2U},
    {@link #SERVICE_CODE_HOOKFLASH} or {@link #SERVICE_CODE_RECALL} are valid return values.
    */
   public int getServiceCodeType();

   /**
    Returns the value of the mid call event.
    @return a string representing the value of the mid call event.  If no value is availabe (e.g. {@link #getServiceCodeType()}
    returns {@link #SERVICE_CODE_HOOKFLASH}) then null is returned.
    */
   public String getServiceCodeValue();

   /**
    {@link #getServiceCodeType()} return constant: the type of service code is unknown, 
    the corresponding value (returned by {@link #getServiceCodeValue()}) is operator specific.
    */
   public static final int SERVICE_CODE_UNDEFINED = 0;


   /**
    {@link #getServiceCodeType()} return constant: the user entered a digit sequence during the call, 
    the corresponding value (returned by {@link #getServiceCodeValue()}) is an ascii representation 
    of the received digits.
    */
   public static final int SERVICE_CODE_DIGITS = 1;


   /**
    {@link #getServiceCodeType()} return constant: a facility information element is received, the 
    corresponding value (returned by {@link #getServiceCodeValue()}) contains the facility 
    information element as defined in ITU Q.932.
    */
   public static final int SERVICE_CODE_FACILITY = 2;

   /**
    {@link #getServiceCodeType()} return constant: a user-to-user message was received. The associated 
    value (returned by {@link #getServiceCodeValue()}) contains the content of the user-to-user information element.
    
    value (returned by {@link #getServiceCodeValue()})
    */
   public static final int SERVICE_CODE_U2U = 3;

   /**
    {@link #getServiceCodeType()} return constant: the user performed a hookflash, optionally followed 
    by some digits. The corresponding value (returned by {@link #getServiceCodeValue()}) is an ascii 
    representation of the entered digits.
    */
   public static final int SERVICE_CODE_HOOKFLASH = 4;

   /**
    {@link #getServiceCodeType()} return constant: the user pressed the register recall button, optionally 
    followed by some digits. The corresponding value (returned by {@link #getServiceCodeValue()}) is an 
    ascii representation of the entered digits.
    */
   public static final int SERVICE_CODE_RECALL = 5;
}
