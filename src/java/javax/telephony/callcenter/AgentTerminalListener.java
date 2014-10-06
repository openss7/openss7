package javax.telephony.callcenter;

import javax.telephony.TerminalListener;

/**
 * The <CODE>AgentTerminalListener</CODE> interface reports all changes 
 * that happen to the <CODE>AgentTerminal</CODE> object. This interface 
 * extends the <CODE>TerminalListener</CODE> interfaces.
 *
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.callcenter.AgentTerminal
 * @since 1.4
 */
public interface AgentTerminalListener extends TerminalListener
{
    /**
     * The <CODE>agentTerminalBusy</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.BUSY</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_BUSY</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_BUSY
     */
    public void agentTerminalBusy(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalLoggedOff</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.LOG_OUT</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_LOGGED_OFF</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_LOGGED_OFF
     */
    public void agentTerminalLoggedOff(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalLoggedOn</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.LOG_IN</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_LOGGED_ON</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_LOGGED_ON
     */
    public void agentTerminalLoggedOn(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalNotReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.NOT_READY</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_NOT_READY</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_NOT_READY
     */
    public void agentTerminalNotReady(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.READY</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_READY</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_READY
     */
    public void agentTerminalReady(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalUnknown</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.UNKNOWN</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_UNKNOWN</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_UNKNOWN
     */
    public void agentTerminalUnknown(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalWorkNotReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.WORK_NOT_READY</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_WORK_NOT_READY</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_WORK_NOT_READY
     */
    public void agentTerminalWorkNotReady(AgentTerminalEvent event);
    
    /**
     * The <CODE>agentTerminalWorkReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> object 
     * has changed to <CODE>Agent.WORK_READY</CODE>.
     * <p>
     * @param event An <CODE>AgentTerminalEvent</CODE> with eventID 
     * <CODE>AgentTerminalEvent.AGENT_TERMINAL_WORK_READY</CODE>.
     *
     * @see AgentTerminalEvent#AGENT_TERMINAL_WORK_READY
     */
    public void agentTerminalWorkReady(AgentTerminalEvent event);
}

