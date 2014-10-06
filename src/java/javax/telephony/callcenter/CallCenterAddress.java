/*
#pragma ident "@(#)CallCenterAddress.java	1.9      97/07/10     SMI"

 * Copyright (c) 1996,1997 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony.callcenter;
import  javax.telephony.*;

/**
 * The <CODE>CallCenterAddress</CODE> interface is the base Address interface
 * for the call center package. This package defines two additional Address
 * interfaces which both extend this interface: <CODE>ACDAddress</CODE> and
 * <CODE>ACDManagerAddress</CODE>.
 *
 * <H4>The <CODE>addCallObserver()</CODE> Method</H4>
 *
 * This interface defines a version of the <CODE>addCallObserver()</CODE>
 * method which overloaded the definition in the <CODE>Address</CODE>
 * interface. This overloaded version accepts an additional boolean parameter
 * which allows applications to monitor Calls which come to this Address for
 * the lifetime of the Call, and not just while it is at this Address.
 * <p>
 * @see javax.telephony.Address
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.ACDManagerAddress
 */

public interface CallCenterAddress extends Address {

  /**
   * This method behaves similarly to <CODE>Address.addCallObserver()</CODE>,
   * with the following exceptions:
   * <p>
   * <UL>
   * <LI>If <EM>remain</EM> is true, the observer remains on all Calls which
   * come to this Address, for the lifetime of the Call.
   * <LI>If <EM>remain</EM> is false, this method behaves exactly the same
   * as <CODE>Address.addCallObserver()</CODE>
   * </UL>
   * <p>
   * If an application attempts to add an instance of an observer to the
   * Address more than once, whether via the method or via the
   * <CODE>Address.addCallObserver()</CODE> method, only a single instance of
   * an observer will be added. Repeated attempts to add an observer will
   * silently fail, i.e. no exception is thrown.
   * <p>
   * <H4>Post-Conditions:</H4>
   * <OL>
   * <LI>observer is an element of this.getCallObservers()
   * <LI>observer is an element of Call.getObservers() for each Call
   * associated with the Connections from this.getConnections()
   * <LI>An array of snapshot events is reported to the observer for existing
   * calls associated with this Address.
   * </OL>
   * <p>
   * @param observer The observer being added.
   * @param remain If true, the observer remains on the Call for the lifetime
   * of the Call. If false, the observer uses the default behavior.
   * @exception MethodNotSupportedException The Address is currently not
   * observable.
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to perform this type of observation.
   * @see javax.telephony.Address
   */
  public void addCallObserver(CallObserver observer, boolean remain)
    throws ResourceUnavailableException, PrivilegeViolationException,
      MethodNotSupportedException;
}
