/*
#pragma ident "%W%      %E%     SMI"

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
 * <H4>Introduction</H4>
 *
 * The <CODE>CallCenterCall</CODE> interface extends the core <CODE>Call</CODE>
 * interface. This interface provides additional Call-related features for the
 * call center package. Applications may query an object with the
 * <CODE>instanceof</CODE> operator to check whether it supports this
 * interface.
 *
 * <H4>Predictive Dialing</H4>
 *
 * Predictive dialing is a special means to place a telephone call. In a
 * predictive telephone call, the destination end is created and placed on the
 * Call first. Only when the destination Connection reaches a certain state, as
 * designated by the application, the originating Connection is created and the
 * originating end is placed on the Call. Predictive dialing is used, for
 * example, when customers are called from a long list, and a customer service
 * representative is placed on the Call only when a customer can be reached.
 * Applications placed predictive telephone calls via the
 * <CODE>connectPredictive()</CODE> method on this interface.
 *
 * <H4>Application Data</H4>
 *
 * Applications may associated an arbitrary piece of data with a Call. This
 * data is seen by any application which has a handle to a Call. This mechanism
 * is often used to store information specific to a Call, such as customer
 * information. The <CODE>setApplicationData()</CODE> method defined on this
 * sets the current data and the <CODE>getApplicationData()</CODE> method
 * retrieves the current application-specific data.
 *
 * <H4>Trunks</H4>
 *
 * Zero or more trunks may be associated with the Call. Applications obtain the
 * trunks associated with a Call via the <CODE>getTrunks()</CODE> method on
 * this interface. A trunk is represented by the <CODE>CallCenterTrunk</CODE>
 * interface.
 *
 * <H4>Observers and Events</H4>
 *
 * Events pertaining to the <CODE>CallCenterCall</CODE> interface are reported
 * via the <CODE>CallCenterCallObserver</CODE> interface. The following are
 * those events reported via this interface:
 * <UL>
 * <LI>Connection "in progress" state event
 * <LI>Trunk state events
 * <LI>Application data change events
 * </UL>
 * <p>
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.CallCenterTrunk
 */

public interface CallCenterCall extends Call {

  /**
   * The <CODE>CallCenterCall.MIN_RINGS</CODE> constant defines the minimum
   * number of rings which the application may specify for the destination end
   * before a predictive telephone call is classified as "no answer".
   */
  public static final int MIN_RINGS = 2;


  /**
   * The <CODE>CallCenterCall.MIN_RINGS</CODE> constant defines the minimum
   * number of rings which the application may specify for the destination end
   * before a predictive telephone call is classified as "no answer".
   */
  public static final int MAX_RINGS = 15;


  /**
   * Answering endpoint treatment: follow the default treatment.
   * The answering endpoint treatment should follow the default treatment.
   */
  public static final int ANSWERING_TREATMENT_PROVIDER_DEFAULT = 1;


  /**
   * Answering endpoint treatment: drop the Call when the endpoint is detected.
   */
  public static final int ANSWERING_TREATMENT_DROP = 2;


  /**
   * Answering endpoint treatment: connect the Call when the endpoint is
   * detected.
   */
  public static final int ANSWERING_TREATMENT_CONNECT = 3;


  /**
   * Answering endpoint treatment: no treatment specified.
   */
  public static final int ANSWERING_TREATMENT_NONE = 4;


  /**
   * Answering endpoint type: answering endpoint may be an answering machine.
   */
  public static final int ENDPOINT_ANSWERING_MACHINE = 1;


  /**
   * Answering endpoint type: answering endpoint may be a fax machine.
   */
  public static final int ENDPOINT_FAX_MACHINE = 2;


  /**
   * Answering endpoint type: answering endpoint may be a human.
   */
  public static final int ENDPOINT_HUMAN_INTERVENTION = 3;


  /**
   * Answering endpoint type: answering endpoint may be anything.
   */
  public static final int ENDPOINT_ANY = 4;


  /**
   * Places a predictive telephone call. A predictive telephone call is a
   * telephone call placed to the destination end first, and connects the
   * originating end only if the destination end reaches either the
   * <CODE>Connection.CONNECTED</CODE> or <CODE>Connection.ALERTING</CODE>
   * state. The destination Connection is created first, and the originating
   * Connection is created only if the destination reaches the designated
   * state. This method returns successfully when both Connections are created
   * and returned.
   *
   * <H5>The originating and destination end arguments</H5>
   *
   * The first three arguments are identical to the arguments of the
   * <CODE>Call.connect()</CODE> method. They represent the desired originating
   * and destination ends of the Call. The originating Terminal may be
   * <CODE>null</CODE>, however, for certain types of Addresses, such as ACD
   * Addresses, this argument may be <CODE>null</CODE>. The destination address
   * string given must be complete and valid.
   *
   * <H5>The target destination Connection state</H5>
   *
   * The application designates when the originating end of the Call is created
   * and placed on the telephone Call, based upon the state of the destination
   * Connection. The desired target state for the destination Connection is
   * given as the <EM>connectionState</EM> argument to this method. The value
   * must be either <CODE>Connection.CONNECTED</CODE> or
   * <CODE>Connection.ALERTING</CODE>. If the destination Connection never
   * reaches this state, this method throws an appropriate exception.
   *
   * <H5>The maximum number of rings</H5>
   *
   * The application may also designate the maximum number of rings allowed on
   * the destination end before the Call is classified as a "no answer". The
   * value must be between <CODE>CallCenterCall.MIN_RINGS</CODE> (2) and
   * <CODE>CallCenterCall.MAX_RINGS</CODE> (15).
   *
   * <H5>Answering treatment and endpoint type</H5>
   *
   * The two final arguments specify how the Call is treated when the
   * destination Connection reaches its target destination, and the allowed
   * kinds of endpoints on the answering end. Each of these arguments must be
   * one of the designated constants defined by this interface.
   *
   * <H5>The returned Connections</H5>
   *
   * The Connections created and returned by this method behave similarly to
   * Connections which were returned from <CODE>Call.connect()</CODE>. The
   * originating Connection moves into the <CODE>Connection.CONNECTED</CODE>
   * state when an originating party is placed on the Call. If the target state
   * for the destination Connection is <CODE>Connection.ALERTING</CODE>, it
   * moves into the <CODE>Connection.CONNECTED</CODE> state when the called
   * party answers the Call.
   * <p>
   * <B>Pre-conditions</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>connectionState == Connection.CONNECTED or Connection.ALERTING
   * <LI>maxRings >= CallCenterCall.MIN_RINGS
   * <LI>maxRings <= CallCenterCall.MAX_RINGS
   * </OL>
   * <B>Post-conditions</B>
   * <OL>
   * <LI>Let connections[] = this.getConnections()
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>connections.length == 2
   * <LI>connections[0].getState() == connectionState
   * <LI>connections[1].getState() == Connection.IDLE
   * </OL>
   * @return An array of the originating and destination Connection
   * @param originatorTerminal The originating Terminal. If the originating
   * Address is an ACD Address, for example, this value may be null.
   * @param originatorAddress The originating Address of the telephone call.
   * @param destination The complete and valid telephone address string.
   * @param connectionState The target state for the destination Connection
   * before the originating Connection is created. This must either be
   * Connection.CONNECTED or Connection.ALERTING
   * @param maxRings The maximum number of rings before classifying the Call as
   * "no answer".
   * @param treatment The treatment of the Call when the endpoint is detected.
   * @param endpointType The permitted answering endpoint type.
   * @exception ResourceUnavailableException An internal resource necessary
   * for placing the phone call is unavailable.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to place a telephone call.
   * @exception InvalidPartyException Either the originator or the destination
   * does not represent a valid party required to place a telephone call.
   * @exception InvalidArgumentException An argument provided is not valid
   * either by not providing enough information or is inconsistent with another
   * argument.
   * @exception InvalidStateException Either the Provider is not in service or
   * the Call is not idle.
   * @exception MethodNotSupportedException The implementation does not support
   * this method.
   */
  public Connection[] connectPredictive(Terminal originatorTerminal,
					Address originatorAddress,
					String destination,
					int connectionState,
					int maxRings,
					int treatment,
					int endpointType)
    throws ResourceUnavailableException, PrivilegeViolationException,
      InvalidPartyException, InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException;


  /**
   * This method associates application specific data with a Call. The
   * format of the data is application-specific. The application-specific data
   * given in this method replaces any existing application data. If the
   * argument given is <CODE>null</CODE>, the current application data (if any)
   * is removed.
   * <p>
   * In the case that a Call is transfered or conferenced, the application data
   * from the Call from which the conference or transfer is invoked will be
   * retained.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE or Call.IDLE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE or Call.IDLE
   * <LI>this.getApplicationData() = data
   * <LI>A CallCentCallAppDataEv is delivered for this Call
   * </OL>
   * @param data The data to be associated with the call.
   * @exception ResourceUnavailableException An internal resource necessary
   * for adding the data was unavailable. For example, the size of the Object
   * was not supported by the implementation.
   * @exception InvalidArgumentException The argument provided is not valid.
   * For example, the implementation does not support the specific object type.
   * @exception InvalidStateException Either the Provider was not in service
   * or the Call was not active or idle.
   * @exception MethodNotSupportedException The implementation does not support
   * this method.
   * @see javax.telephony.callcenter.events.CallCentCallAppDataEv
   */
  public void setApplicationData(Object data)
      throws ResourceUnavailableException, InvalidArgumentException,
        InvalidStateException, MethodNotSupportedException;


  /**
   * Returns the application-specific data associated with the Call. This
   * method returns <CODE>null</CODE> is there is no associated data.
   * <p>
   * @return The application-specific data associated with this Call.
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   */
  public Object getApplicationData()
    throws MethodNotSupportedException;


  /**
   * Returns an array of all <CODE>CallCenterTrunks</CODE> currently being
   * used for this Call. If there are no trunks being used for this Call, this
   * method returns null. Each trunk returns by this method will be in the
   * <CODE>CallCenterTrunk.VALID</CODE> state.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let CallCenterTrunk[] trunks = this.getTrunks()
   * <LI>trunks == null or trunks.length >= 1
   * <LI>trunks[i].getState() == CallCenterTrunk.VALID_TRUNK, for all i
   * </OL>
   * @return An array of trunks currently associated with this Call.
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   */
  public CallCenterTrunk[] getTrunks()
    throws MethodNotSupportedException;
}
