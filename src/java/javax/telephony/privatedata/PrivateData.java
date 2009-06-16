/*
#pragma ident "@(#)PrivateData.java	1.12      97/09/24     SMI"

 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
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

package javax.telephony.privatedata;
import  javax.telephony.*;

/**
 * <H4>Introduction</H4>
 *
 * The <EM>private data</EM> mechanism in JTAPI is a means by which
 * applications can send platform-specific messages to the underlying telephone
 * platform. The <CODE>PrivateData</CODE> interface may be implemented on any
 * JTAPI object. Applications may query an object to see if it supports this
 * interface via the <CODE>instanceof</CODE> operator. This interface makes no
 * attempt to interpret the data sent to the underlying platform.
 * <p>
 * <STRONG>Note:</STRONG> Use of this interface interferes with application
 * portability across different JTAPI implementations. Applications which make
 * use of this interface may not work properly with other JTAPI-compliant
 * implementations.
 *
 * <H4>Setting vs. Sending Private Data</H4>
 *
 * There are two ways in which information is sent to the platform.
 * Applications can <EM>set</EM> a piece of data to be associated with the next
 * method invocation on the object. The data is only valid for the next method
 * invocation on the same object. This data is not transmitted to the
 * underlying platform until the next method is invoked. Also, applications
 * may immediately <EM>send</EM> a piece of data to the underlying platform.
 * This data is not associated with any future method invocation.
 *
 * <H4>Private Data Events</H4>
 *
 * Implementations may also send platform-specific events to the application.
 * Each individual object carries its own private data event. The data carried
 * in these objects are specific to the implementation. The private data event
 * interfaces defined are: <CODE>PrivateAddrEv</CODE>,
 * <CODE>PrivateCallEv</CODE>, <CODE>PrivateProvEv</CODE>, and
 * <CODE>PrivateTermEv</CODE>
 * <p>
 * @see javax.telephony.privatedata.capabilities.PrivateDataCapabilities
 * @see javax.telephony.privatedata.events.PrivateAddrEv
 * @see javax.telephony.privatedata.events.PrivateCallEv
 * @see javax.telephony.privatedata.events.PrivateProvEv
 * @see javax.telephony.privatedata.events.PrivateTermEv
 */

public interface PrivateData {

 /**
  * Associates some platform-specific data with the next method that is
  * invoked on the object for which this interface is implemented. The format
  * of this data and the manner in which it modifies the method invocation is
  * platform-dependent. This data applies to the next method invocation ONLY
  * and does not affect any future method invocations.
  * <p>
  * @param data The platform-dependent data.
  */
  public void setPrivateData(Object data);


  /**
   * Returns some platform-specific data associated with the last method that
   * was invoked on the object for which this PrivateData is implemented. The
   * format of this data is platform-dependent. This data pertains to the last
   * method invocation ONLY.
   * <p>
   * @since JTAPI v1.2 
   * @return Object The platform-dependent data.
   */
  public Object getPrivateData();


  /**
   * Immediately performs some platform-specific action. The effect of this
   * methods invocation is immediate and does not directly relate to any
   * future object method invocations. The action taken upon receipt of this
   * data is platform-dependent as is the format of the data itself. This
   * method returns the platform-dependent data actually sent.
   * <p>
   * @param data The platform-dependent data.
   * @return The platform-dependent data sent.
   */
  public Object sendPrivateData(Object data);
}
