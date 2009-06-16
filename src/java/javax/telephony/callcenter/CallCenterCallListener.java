package javax.telephony.callcenter;

import javax.telephony.CallListener;

/**
 * The <CODE>CallCenterCallListener</CODE> interface extends the event
 * reporting of of the core <CODE>CallListener</CODE> to include call center
 * related events. Applications instantiate an object which implements this
 * interface and use the <CODE>Call.addListener()</CODE> to request delivery
 * of events to this Listener object. Events will be delivered to this
 * interface only if the Provider is in the <CODE>Provider.IN_SERVICE</CODE>
 * state. All events which are reported via this interface must extend the
 * <CODE>CallCenterCallEvent</CODE> interface.
 * <p>
 * With JTAPI 1.2, events were reported to Observers who registered to receive events.
 * With JTAPI 1.3, the Listener style of event reporting was introduced to the JTAPI Core, Media and Mobile packages.
 * With JTAPI 1.4, Listener style event reporting is included in the remaining JTAPI packages,
 * and the Observer style interfaces are deprecated.
 * <p>
 * Changes to call-associated information, specifically, trunks active
 * on the call and application data, are reported on this interface.
 * <p>
 * For interfaces and comments on the JTAPI 1.4 world, see
 * {@link javax.telephony.Call}, 
 * {@link javax.telephony.CallListener}, and
 * {@link javax.telephony.callcenter.CallCenterCallEvent}.
 * <p>
 * For interfaces and comments on the pre-JTAPI 1.4 world, see:
 * {@link javax.telephony.CallObserver},
 * {@link javax.telephony.callcenter.events.CallCentCallEv},
 * {@link javax.telephony.callcenter.events.CallCentTrunkValidEv},
 * {@link javax.telephony.callcenter.events.CallCentTrunkInvalidEv},
 * {@link javax.telephony.callcenter.events.CallCentCallAppDataEv} and
 * {@link javax.telephony.callcenter.events.CallCentConnInProgressEv}.
 * @since 1.4
 */
public interface CallCenterCallListener extends CallListener
{

    	/**
	 * This method is called to report that the application data
	 * associated with the {@link javax.telephony.callcenter.CallCenterCall} has changed.
	 */
    public void applicationData(CallCenterCallEvent event);

	/** 
	 * This method is called to report that the state of a
	 * {@link javax.telephony.callcenter.CallCenterTrunk} object associated
	 * with this {@link javax.telephony.callcenter.CallCenterCall}, has
	 * changed to the <CODE>CallCenterTrunk.VALID_TRUNK</CODE> state.
	 */
    public void trunkInvalid(CallCenterTrunkEvent event);
    
	/** 
	 * This method is called to report that the state of a
	 * {@link javax.telephony.callcenter.CallCenterTrunk} object associated
	 * with this {@link javax.telephony.callcenter.CallCenterCall}, has
	 * changed to the <CODE>CallCenterTrunk.INVALID_TRUNK</CODE> state.
	 */
    public void trunkValid(CallCenterTrunkEvent event);
}
