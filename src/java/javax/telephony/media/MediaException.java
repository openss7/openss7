/*
 * MediaException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

/*
 * <pre>
 *
 *        CException                          CRuntimeException
 *            |                                   |
 *           CMediaException*                 CMediaRuntimeException
 *            |  EventObject + getEvent           |
 *            |					  |
 *	      |					  |
 *	      |->CMediaCallException		  |->CDisconnectedException
 *            |                         	  |->CNoResourceException
 *	      |         	    	     	  |->CNotBoundException
 *	      |         	    	     	  +->CNotOwnerException
 *            |->CMediaResourceException
 *	      |   ResEvent  + getResourceEvent
 *	      |                                   
 *            |->CContainerException              
 *	      |   ContEvent + getContainerEvent   
 *	      |
 *	      +->CMediaServiceException*
 *	          |
 *		  |->CMediaBindException*
 *	          |   |     + getBindAndReleaseEvent [proposed addition]
 *		  |   |
 *		  |   |->CAlreadyBoundException
 *		  |   |->CBindCancelledException
 *		  |   |->CNoServiceAssignedException
 *		  |   |   -->CBadServiceNameException
 *		  |   +->CNoServiceReadyException
 *		  |
 *		  |->CMediaGroupException        [proposed addition]
 *	              |     + getMediaGroupEvent [proposed addition]
 *		      |
 *                    +->CMediaConfigException*
 *                        | + getResourceSpec
 *                        |
 *		          |->CBadConfigSpecException
 *                        |   -->CBadResourceSpecException
 *		          |->CConfigFailedBusyException
 *		          |->CResourceBusyException
 *		          +->CResourceNotSupportedException
 *
 * </pre>
 */
/**
 * MediaException is the parent for [non-Runtime] Exceptions
 * in JTAPI Media.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public abstract class MediaException extends Exception {
    /** The MediaEvent associated with this Exception.
     * Typically a MediaServiceEvent or ContainerEvent, etc.
     * <!-- If using the provider package, it is a Base.Event -->
     */
     private MediaEvent event = null;
    
    /**
     * Constructs a <code>MediaException</code> 
     * with no specified detail message. 
     */
    public MediaException() {super();}

    /**
     * Constructs a <code>MediaException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public MediaException(java.lang.String s) {super(s);}

    /**
     * Construct a <code>MediaException</code>
     * with the specified detail message and Event.
     *
     * Wraps the given Event in a MediaException.
     * This exception is thrown if the Event indicates
     * an error or other exceptional status.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public MediaException(java.lang.String s, MediaEvent event) {
	super(s);
	this.event = event;
    }

    /** 
     * Return the MediaEvent associated with this MediaException.
     * <p>
     * <b>Note:</b> for <i>pre-condition</i> errors,
     * this may return <tt>null</tt>.
     *
     * @return the MediaEvent associated with this Exception.
     */
    public MediaEvent getEvent() {
	return event;
    }

    /**
     * Set the MediaEvent for this MediaException.
     * <p>
     * <b>Note:</b>
     * This method is for package implementors and not normally used
     * by application developers. This avoids the need to define a
     * MediaEvent constructor for all derived classes.
     */
    // withdrawn: define "MediaEvent" constructors for all Exceptions 
    // public void setEvent(MediaEvent event) { this.event = event; }
}
