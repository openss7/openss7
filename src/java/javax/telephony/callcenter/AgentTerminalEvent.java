package javax.telephony.callcenter;

import javax.telephony.TerminalEvent;

/**
 * The <CODE>AgentTerminalEvent</CODE> interface is the interface exposed for all events
 * reported for the <CODE>AgentTerminal</CODE> interface.
 * This interface extends
 * the <CODE>CallCenterEvent</CODE> interface and the core <CODE>TerminalEvent</CODE>
 * interface.
 * <p>
 * All <CODE>AgentTerminalEvents</CODE> are reported via
 * the <CODE>AgentTerminalListener</CODE> interface,
 * to objects registered as Listeners for the <CODE>AgentTerminal</CODE> associated with the Agent.
 * Objects register as Listeners of <CODE>AgentTerminalEvents</CODE> by implementing
 * the <CODE>AgentTerminalListener</CODE> interface and invoking <CODE>Terminal.addListener</CODE>.
 * <p>
 * The call center package reports state changes
 * in <CODE>Agents</CODE> which are associated
 * with <CODE>AgentTerminal</CODE>.
 * <p>
 * Each state change is reported via 
 * an <CODE>AgentTerminalEvent</CODE>.
 * An <CODE>AgentTerminalEVent</CODE> carries an event ID which indicates 
 * which specific <CODE>Agent</CODE> state change occurred.
 * <p>
 * The state change indicated by a particular <CODE>AgentTerminalEvent</CODE>,
 * indicated in turn by the ID value,
 * may be determined by calling
 * the <CODE>Event.getID</CODE> method on the event.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalEvent
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.AgentTerminal
 * @since 1.4
 */
public interface AgentTerminalEvent extends CallCenterEvent, TerminalEvent 
{
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.BUSY</CODE> state.
    * @see Agent#BUSY
    */
    public static final int AGENT_TERMINAL_BUSY            = 308;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.LOG_OFF</CODE> state.
    * @see Agent#LOG_OUT
    */
    public static final int AGENT_TERMINAL_LOGGED_OFF      = 309;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.LOG_ON</CODE> state.
    * @see Agent#LOG_IN
    */
    public static final int AGENT_TERMINAL_LOGGED_ON       = 310;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.NOT_READY</CODE> state.
    * @see Agent#NOT_READY
    */
    public static final int AGENT_TERMINAL_NOT_READY       = 311;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.READY</CODE> state.
    * @see Agent#READY
    */
    public static final int AGENT_TERMINAL_READY           = 312;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.UNKNOWN</CODE> state.
    * @see Agent#UNKNOWN
    */
    public static final int AGENT_TERMINAL_UNKNOWN         = 313;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.WORK_NOT_READY</CODE> state.
    * @see Agent#WORK_NOT_READY
    */
    public static final int AGENT_TERMINAL_WORK_NOT_READY  = 314;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.WORK_READY</CODE> state.
    * @see Agent#WORK_READY
    */
    public static final int AGENT_TERMINAL_WORK_READY      = 315;
   
    /**
     * Returns the <CODE>Agent</CODE> associated with the
     * <CODE>AgentTerminal</CODE>.
     * <p>
     * @return The associated Agent.
     */
    public Agent getAgent();

 
}
