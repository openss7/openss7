/*
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
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

package javax.telephony.mobile;
import  javax.telephony.*;


/**
 * The <CODE>MobileRadio</CODE> interface is used for
 * simple radio management capabilities (turning on and off), if
 * the platform supports it.
 * and getting information about the current signal strength.
 *
 * <H4>Network States</H4>
 * 
 * The <CODE>MobileRadio</CODE> state can either be on  (setState(true)) or off (setState(false)).
 *
 * If state management is not user controllable, then the <CODE>setState</CODE> method
 * will throw a <CODE>MethodNotSupportedException</CODE>
 *
 * If the device has been secured via password protection (e.g. PIN-code),
 * the <CODE>setState</CODE> method may also throw the 
 * <CODE>InvalidStateException</CODE>, if an attempt is made to manipulate the 
 * radio while the device is secured.
 *
 * <H4>Mobile Radio Listeners </H4>
 *
 * All changes in a <CODE>MobileRadio</CODE> object are reported via the 
 * <CODE>MobileRadioListener</CODE> interface. 
 *
 * Applications instantiate an object which implements this
 * interface and begins this delivery of events to this object using the
 * <CODE>MobileRadio.addRadioListener()</CODE> method. 
 *
 * Applications receive events on a listener until the listener
 * is removed via the <CODE>MobileRadio.removeRadioListener()</CODE> method.
 *
 * @see javax.telephony.mobile.MobileRadioEvent
 * @see javax.telephony.mobile.MobileRadioListener
 * @version 02/26/99 1.4
 */

public interface MobileRadio {

    /**
     * Returns the current MobileRadio state. 
     * 
     * @return  true if the radio is on, otherwise false.
     * @exception MethodNotSupportedException  can not access the current radio state.
     */
    public boolean getRadioState() throws MethodNotSupportedException;

    /**
     * Set the MobileRadio state.
	*
	* The <CODE>MobileProvider</CODE> must be OUT_OF_SERVICE before the MobileRadio object
	* may turn the physical radio off.
	* After the radio is successfully turned off the <CODE>MobileRadio</CODE> state 
	* must be changed to reflect the radio off status.
	*
	* When the radio is turned off, no further network events will be generated
	* and the radio signal level is set to zero, until the radio is reenabled.
     * <p>
     * When  the radio is turned on,
     * the <CODE>MobileProvider</CODE> can be in any valid state, depending
     * on whether there are networks detected and subscription details
     * are OK. 
	*
     * @param state The desired radio state.
     * @exception MethodNotSupportedException Indicates the radio state can not
	* be manipulated on this implementation.
     * @exception InvalidStateException Indicates the network service
     * state cannot be changed due to current <CODE>MobileProvider</CODE> state.
     */
    public void setRadioState(boolean state)
        throws MethodNotSupportedException, InvalidStateException;

    /**
     * Returns the <CODE>MobileRadio</CODE> startup state at boot. 
     * @return The current radio startup state.
     * @exception MethodNotSupportedException Indicates the radio
     * state startup management capability is not supported.
     */
    public boolean getRadioStartState()
        throws MethodNotSupportedException;

    /**
	* Sets the <CODE>MobileRadio</CODE> startup state at boot. 
	*
	* A possible required password  will not be 
     * given here for security reasons! 
	*
     * The password required should 
     * be given some other way, determined by native implementation, 
     * or at next time when a <CODE>MobileProvider</CODE> object is requested 
     * from the JtapiPeer. 
	*
     * @param state The new radio startup state. True, if radio 
     * is turned on. Otherwise, false.
     * @exception MethodNotSupportedException Indicates the radio
     * state startup management is not supported.
     * @exception InvalidStateException Indicates the network service
     * startup state cannot be changed due to current platform state.
     */
    public void setRadioStartState(boolean state)
        throws MethodNotSupportedException,InvalidStateException;

    /**
     * Get the current radio signal level.
     * <p>
     * When the radio is turned off the signal level is set to zero.
	*
	* The radio signal is mapped to positive integer values from 1 to 
	* <CODE>getMaxSignalLevel</CODE>. In GSM networks the signal strength
	* is mapped to the range 1-5. (e.g. Level 1 = 20%, Level 5 = 100%)
	*
	* @see #getMaxSignalLevel
     * @return The current radio signal level.
     */
    public int getSignalLevel();

    /**
     * Get the maximum radio signal level.
	* @see #getSignalLevel
     * @return The maximum radio signal level.
     */
    public int getMaxSignalLevel();

    /**
     * Add a listener for <CODE>MobileRadio</CODE> events.
     * 
     * @param listener The listener object to add.
     * @exception MethodNotSupportedException This implementation does not generate events.
     */
    public void addRadioListener(MobileRadioListener listener)
        throws MethodNotSupportedException;

    /**
     * Remove a listener from the <CODE>MobileRadio</CODE>.
     * 
     * @param listener The listener to remove.
     */
    public void removeRadioListener(MobileRadioListener listener);

}

