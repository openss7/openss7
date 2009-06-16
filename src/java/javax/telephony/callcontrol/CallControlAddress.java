/*
#pragma ident "@(#)CallControlAddress.java	1.25      97/12/12     SMI"

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

package javax.telephony.callcontrol;
import  javax.telephony.*;

/**
 * <H4>Introduction</H4>
 *
 * The <CODE>CallControlAddress</CODE> interface extends the core
 * <CODE>Address</CODE> interface. It provides additional features on the
 * Address. Applications may query an Address object using the
 * <CODE>instanceof</CODE> operator to see whether it supports this interface.
 *
 * <H4>Address Forwarding</H4>
 *
 * This interface supports methods which permit applications to modify and
 * query the forwarding characteristics of an Address. The forwarding
 * characteristics determine how incoming telephone calls to this Address
 * should be handled, if any special handling is desired.  A switching domain
 * will honor those instructions to the extent that other possibly higher 
 * priority instructions allow.
 * <p>
 * Each Address may have zero or more <EM>forwarding instructions</EM>. Each
 * instruction describes how the switching domain should handle incoming
 * telephone calls to an Address under different circumstances. Examples of
 * forwarding instructions are "forward all calls to x9999" or "forward all
 * calls to x7777 when no one answers." Each forwarding instruction is
 * represented by an instance of the <CODE>CallControlForwarding</CODE> class.
 * A switching domain will honor forwarding instructions to the extent that 
 * other (possibly higher priority) instructions allow.
 * <p>
 * Applications assign a list of forwarding instructions via the
 * the <CODE>CallControlAddress.setForwarding()</CODE> method. To obtain the
 * current, effective forwarding instructions, applications invoke the
 * <CODE>CallControlAddress.getForwarding()</CODE> method. To cancel all
 * forwarding instructions, applications use the
 * <CODE>CallControlAddress.cancelForwarding()</CODE> method.
 *
 * <H4>Do Not Disturb and Message Waiting</H4>
 *
 * The <CODE>CallControlAddress</CODE> interface defines two additional
 * attributes: <EM>do-not-disturb</EM> and <EM>message-waiting</EM>.
 * <p>
 * The do-not-disturb feature gives the means to notify the telephony platform 
 * that an Address does not want to receive incoming telephone calls. That
 * is, if this feature is activated, the underlying telephony platform will not
 * alert this Address to incoming telephone calls. Applications use the
 * <CODE>CallControlAddress.setDoNotDisturb()</CODE> method to activate or
 * deactivate this feature and the
 * <CODE>CallControlAddress.getDoNotDisturb()</CODE> method to return the
 * current state of this attribute.
 * <p>
 * Note that the <CODE>CallControlTerminal</CODE> interface also has a
 * do-not-disturb attribute. This gives the ability to control the do not
 * disturb property at either the Address level (e.g. a phone number) or
 * at the Terminal level (e.g. an individual phone.)
 * <p>
 * The message-waiting attribute indicates whether there are messages waiting
 * for a human user of the Address. These messages may either be maintained
 * by an application or some telephony platform. Applications inform the
 * telephony hardware of the message waiting status, and typically the hardware
 * displays a visible indicator (e.g. an LED) to users. Applications use the
 * <CODE>CallControlAddress.setMessageWaiting()</CODE> method to activate or
 * deactivate this feature and the
 * <CODE>CallControlAddress.getMessageWaiting()</CODE> method to return the
 * current state of this attribute.
 *
 * <H4>Observers and Events</H4>
 *
 * All events pertaining to the <CODE>CallControlAddress</CODE> interface are
 * reported via the <CODE>AddressObserver.addressChangedEvent()</CODE>
 * method. The application observer object must also implement the
 * <CODE>CallControlCallObserver</CODE> interface to express interest in the
 * call control package events.
 * <p>
 * The following are the events associated with this interface:
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrDoNotDisturbEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the do-not-disturb feature of this Address has changed.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrForwardEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the forwarding characteristics of this Address has changed.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrMessageWaitingEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the message waiting characteristics of this Address has changed.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.callcontrol.CallControlTerminal
 * @see javax.telephony.callcontrol.CallControlForwarding
 * @see javax.telephony.callcontrol.CallControlAddressObserver
 * @see javax.telephony.callcontrol.events.CallCtlAddrDoNotDisturbEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrForwardEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrMessageWaitingEv
 */

public interface CallControlAddress extends Address {

  /**
   * Sets the forwarding characteristics for this Address. This forwarding
   * command supplants all previous forwarding instructions if it succedes,
   * otherwise the forwarding state at the time of the command remains unchanged. 
   * This method takes an array of <CODE>CallControlForwarding</CODE> objects. 
   * Each object describes a different forwarding. This method waits until all 
   * forwarding instructions have been set or until an error occurs and an exception 
   * is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getForwarding() == instructions
   * <LI>CallCtlAddrForwardEv is delivered for this Address
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlAddrForwardEv
   * @param instructions An array of Address forwarding instructions
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   * @exception InvalidStateException The Provider is not "in service".
   * @exception InvalidArgumentException An invalid set of forwarding
   * instructions were given as a parameter.
   */
  public void setForwarding(CallControlForwarding[] instructions)
    throws MethodNotSupportedException, InvalidStateException,
      InvalidArgumentException;


  /**
   * Returns an array of forwarding instructions currently effective for this 
   * Address. If there are no effective forwarding instructions, this method
   * returns null.
   * <p>
   * @return An array of Address forwarding instructions, null if there are 
   * none.
   * @exception MethodNotSupportedException This method is not supported by
   * the given implementation.
   */
  public CallControlForwarding[] getForwarding()
    throws MethodNotSupportedException;


  /**
   * Cancels all of the forwarding instructions on this Address. When this
   * method completes, the <CODE>CallControlAddress.getForwarding()</CODE>
   * method will return null. This method waits until all forwarding
   * instructions have been cancelled or until an error occurs and an exception
   * is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getForwarding == null
   * <LI>CallCtlAddrForwardEv is delivered for this Address.
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlAddrForwardEv
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   * @exception InvalidStateException The Provider is not "in service".
   */
  public void cancelForwarding()
    throws MethodNotSupportedException, InvalidStateException;


  /**
   * Returns true if the do-not-disturb feature is activated, false otherwise.
   * <p>
   * @return True if do-not-disturb feature is activated, false if it is deactivated.
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   */
  public boolean getDoNotDisturb()
    throws MethodNotSupportedException;


  /**
   * Specifies whether the do-not-disturb feature should be activated or
   * deactivated for this Address. This feature only affects whether or not
   * calls will be accepted at this Address. Note that the do-not-disturb feature
   * on all Terminals associated with this Address are independent of this
   * Terminal's do-not-disturb setting. If 'enable' is true, do-not-disturb is
   * activated if not already activated. If 'enable' is false, do-not-disturb is
   * deactivated if not already deactivated.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getDoNotDisturb() == enable
   * <LI>CallCtlAddrDoNotDisturbEv is delivered for this Address
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlAddrDoNotDisturbEv
   * @param enable True to activate do-not-disturb, false to deactivate.
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   * @exception InvalidStateException The Provider is not "in service".
   */
  public void setDoNotDisturb(boolean enable)
    throws MethodNotSupportedException, InvalidStateException;


  /**
   * Returns true if the message waiting is activated, false otherwise.
   * <p>
   * @return True if message-waiting is activated, false if it is deactivated.
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   */
  public boolean getMessageWaiting()
    throws MethodNotSupportedException;


  /**
   * Specifies whether the message-waiting indicator should be activated or
   * deactivated for this Address. If 'enable' is true, message-waiting is
   * activated if not already activated. If 'enable' is false, message-waiting is
   * deactivated if not already deactivated.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getMessageWaiting() == enable
   * <LI>CallCtlAddrMessageWaitingEv is delivered for this Address
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlAddrMessageWaitingEv
   * @param enable True to activate message-waiting, false to deactivate.
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   * @exception InvalidStateException The Provider is not "in service".
   */
  public void setMessageWaiting(boolean enable)
    throws MethodNotSupportedException, InvalidStateException;
}
