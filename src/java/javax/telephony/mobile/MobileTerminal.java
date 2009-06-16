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
 * A <CODE>MobileTerminal</CODE> represents a physical hardware endpoint 
 * connected to a wireless telephony domain.
 *
 * An example of a 
 * MobileTerminal is a mobile phone connected to a laptop 
 * via serial port. For example, computer workstations and handheld 
 * (PDA) devices are modeled as MobileTerminals if they act 
 * as physical endpoints in a cellular telephony network. 
 * This interface extends the core Terminal with capabilities 
 * to get the manufacturer name, the terminal identification and 
 * the type approval number.
 * 	
 * <p>
 * DTMF tone generation is handled in this interface to allow for 
 * communication with legacy server applications which expect tone inputs
 * to be sent from a dumb telephony terminal. 
 *
 * <P>
 * Note: in mobile networks the tones are not typically generated from the 
 * terminal node, but are handled by the network itself. An implementation
 * of this interface is responsible for propagating the tones 
 * appropriately.
 * <ul>
 * <li> GSM 04.08 section 9.3.24/25/26/28/29 </li>
 * <li> IS-136.2 section 2.7.3.1.3 </li>
 * <li> IS-95B section 6.7.2.3</li>
 * </ul>
 *
 * @see javax.telephony.Terminal
 * @see javax.telephony.mobile.MobileProvider
 * @version 03/03/99 1.5
 */

public interface MobileTerminal extends Terminal{
	
	/**
	 * Returns the Terminal Identification String.
	 * This methods returns an equipment ID string such as IMEI or ESN.
	 * (International Mobile Equipment Identification) or
	 * (Electronic Serial Number)
	 * <p>
	 * @return an equipment ID String, NULL if no such ID is implemented.
	 */
	public String getTerminalId();

	/**
	 * Returns the Mobile's Manufacturer Name.
	 * <p>
	 * @return an manufacturer name String, NULL if not implemented.
	 */
	public String getManufacturerName();


	/**
	 * Returns the Software Version String.
	 * This methods returns a software version string.
	 * <p>
	 * @return a software version String, NULL if not implemented.
	 */
	public String getSoftwareVersion();

	/**
	 * Returns the type approval String.
	 * This methods returns a type approval string.
	 * <p>
	 * @return a type approval String, NULL if not implemented.
	 */
	public String getTypeApproval();


	/**
	 * Generates one continuous DTMF tone. The character parameter must consist of
	 * one  of the following characters: the numbers zero through nine
	 * (0 - 9), the letters A through D, asterisk (*), or pound(#). 
	 * 
	 * @param digit The  DTMF-tone digit to generate on the telephone
	 * line.
	 * @exception MethodNotSupportedException The implementation does not
	 * support generating DTMF tones.
	 * @return true if a tone was successfully delivered.
	 */
	
	public boolean startDTMF(char digit) 		
		throws MethodNotSupportedException;

	/**
	 * Stop the continuous DTMF tone being generated on the telephone 
	 * line.
	 */	
	public void stopDTMF();				

	/**
	 * Generates one or more DTMF tones. The string parameter must consists of
	 * one or more of the following characters: the numbers zero through nine
	 * (0 - 9), the letters A through D, asterisk (*), or pound(#). Any other
	 * characters in the string parameter are ignored.
	 * 
	 * @param digits The string of DTMF-tone digits to generate on the telephone
	 * line.
	 * @exception MethodNotSupportedException The implementation does not
	 * support generating DTMF tones.
	 * @return true if a tone was successfully delivered.
	 */

	public boolean generateDTMF(String digits)	
		throws MethodNotSupportedException;
}

