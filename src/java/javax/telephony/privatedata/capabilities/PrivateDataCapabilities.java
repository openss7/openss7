/*
#pragma ident "@(#)PrivateDataCapabilities.java	1.9      99/02/23     SMI"

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

package javax.telephony.privatedata.capabilities;

/**
 * The <CODE>PrivateDataCapabilities</CODE> interface is the capabilities
 * interface for the <CODE>PrivateData</CODE> interface. Additional packages
 * which want to extend the private data package should extend this interface
 * for its capabilities.
 * <p>
 * Since the <CODE>PrivateData</CODE> interface is always implemented on some
 * existing JTAPI object (e.g. Provider, Call, etc), this interface should be
 * implemented along with the corresponding object's capabilities interface.
 * For example, if the implementation's Call object supports private data, the
 * <CODE>Provider.getCallCapabilities()</CODE> and
 * <CODE>Call.getCapabilities()</CODE> methods should return objects which
 * implement <CODE>PrivateDataCapabilities</CODE> in addition to the
 * <CODE>CallCapabilities</CODE> interface.
 * <p>
 * @see javax.telephony.privatedata.PrivateData
 */

public interface PrivateDataCapabilities {

  /**
   * This method returns true if the <CODE>PrivateData&#46setPrivateData()</CODE>
   * method is supported, false otherwise.
   * <p>
   * @since JTAPI v1.2
   * @return True if the setting of private data is supported, false otherwise.
   */
  public boolean canSetPrivateData();


  /**
   * This method returns true if the <CODE>PrivateData&#46getPrivateData()</CODE>
   * method is supported, false otherwise.
   * <p>
   * @since JTAPI v1.2
   * @return True if obtaining the private data is supported, false otherwise.
   */
  public boolean canGetPrivateData();


  /**
   * This method returns true if the <CODE>PrivateData&#46sendPrivateData()</CODE>
   * method is supported, false otherwise.
   * <p>
   * @since JTAPI v1.2
   * @return True if the sending of private data is supported, false otherwise.
   */
  public boolean canSendPrivateData();
}
