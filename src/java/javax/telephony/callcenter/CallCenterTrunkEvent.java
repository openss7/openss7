package javax.telephony.callcenter;

/**
 * This interface is exposed for all events
 * reported for <CODE>CallCenterTrunk</CODE>-related state change events in the call center package.
 * This interface extends
 * the <CODE>CallCenterCallEvent</CODE> interface.
 * <p>
 * This interface contains a single method, <CODE>getTrunk</CODE>, which
 * returns the <CODE>CallCenterTrunk</CODE> associated with this event.
 * <p>
 * The call center package returns two possible CallCenterTrunkEvent event IDs -
 * they are <CODE>CALL_CENTER_TRUNK_VALID_EVENT</CODE> and
 * <CODE>CALL_CENTER_TRUNK_INVALID_EVENT</CODE>.
 * These IDs indicate
 * state changes in the <CODE>CallCenterTrunk</CODE> with respect to this particular {@link CallCenterCall}.
 * <p>
 * All <CODE>CallCenterTrunkEvents</CODE> are reported via
 * the <CODE>CallCenterCallListener</CODE> interface,
 * to objects registered as Listeners.
 * Objects register as Listeners of <CODE>CallCenterTrunkEvents</CODE> by implementing
 * the <CODE>CallCenterCallListener</CODE> interface and invoking <CODE>Call.addListener</CODE>.
 * <p>
 * @see javax.telephony.callcenter.CallCenterCall
 * @since 1.4
 */
public interface CallCenterTrunkEvent extends CallCenterCallEvent
{
    
    /**
     * Returns the <CODE>CallCenterTrunk</CODE> associated with this event.
     * @return The associated CallCenterTrunk.
     */
    public CallCenterTrunk getTrunk();
    
   /**
    * This event ID
    * indicates that <CODE>CallCentTrunkValidEv</CODE> has moved into the 
    * <CODE>CallCenterTrunk.VALID_TRUNK</CODE> state.
    */
    public static final int CALL_CENTER_TRUNK_VALID_EVENT   = 317;
    
   /**
    * This event ID
    * indicates that <CODE>CallCentTrunkValidEv</CODE> has moved into the 
    * <CODE>CallCenterTrunk.VALID_TRUNK_INVALID</CODE> state.
    */
    public static final int CALL_CENTER_TRUNK_INVALID_EVENT = 318;
}

