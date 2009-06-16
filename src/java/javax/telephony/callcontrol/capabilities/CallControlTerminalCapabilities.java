/*
#pragma ident "%W%      %E%     SMI"

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

package javax.telephony.callcontrol.capabilities;
import  javax.telephony.*;
import  javax.telephony.capabilities.TerminalCapabilities;

/**
 * The <CODE>CallControlTerminalCapabilities</CODE> interface extends the core
 * <CODE>TerminalCapabilities</CODE> interface. This interface provides
 * methods to reflect the capabilities of the methods on the
 * <CODE>CallControlTerminal</CODE> interface.
 * <p>
 * The <CODE>Provider.getTerminalCapabilities()</CODE> method returns the
 * static Terminal capabilities, and the
 * <CODE>Terminal.getCapabilities()</CODE> method returns the dynamic Terminal
 * capabilities. The object returned from each of these methods can be queried
 * with the <CODE>instanceof</CODE> operator to check if it supports this
 * interface. This same interface is used to reflect both static and dynamic
 * Terminal capabilities.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Terminal
 * @see javax.telephony.capabilities.TerminalCapabilities
 */

public interface CallControlTerminalCapabilities extends TerminalCapabilities {

  /**
   * Returns true if the application can obtain the do not disturb state,
   * false otherwise.
   * <p>
   * @return True if the application can obtain the do not disturb state,
   * false otherwise.
   */
  public boolean canGetDoNotDisturb();


  /**
   * Returns true if the application can set the do not disturb state, false
   * otherwise.
   * <p>
   * @return True if the application can set the do not disturb state, false
   * otherwise.
   */
  public boolean canSetDoNotDisturb();


  /**
   * Returns true if the application can invoke the pickup feature, false
   * otherwise.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI 1.2 with
   * overloaded versions. These versions permit applications to give typed
   * argument to obtain the capabilities for a particular overloaded version
   * of the <CODE>CallControlTerminal.pickup()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. The default behavior of this method in
   * JTAPI v1.2 and later should invoke the canPickup(Connection, Address)
   * method.
   * @return True if the application can invoke the pickup feature, false
   * otherwise.
   */
  public boolean canPickup();



  /**
   * Returns true if the application can invoke the overloaded pickup feature
   * which takes a Connection and an Address as arguments, false otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param connection This argument is used for typing information to
   * determine the overloaded version of the pickup() method.
   * @param address This argument is used for typing information to determine
   * the overloaded version of the pickup() method.
   * @return True if the application can invoke the pickup feature which takes
   * a Connection and an Address as arguments, false otherwise.
   */
  public boolean canPickup(Connection connection, Address address);


  /**
   * Returns true if the application can invoke the overloaded pickup feature
   * which takes a TerminalConnection and an Address as arguments, false
   * otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param tc This argument is used for typing information to determine the
   * overloaded version of the pickup() method.
   * @param address This argument is used for typing information to determine
   * the overloaded version of the pickup() method.
   * @return True if the application can invoke the pickup feature which takes
   * a TerminalConnection and an Address as arguments, false otherwise.
   */
  public boolean canPickup(TerminalConnection tc, Address address);


  /**
   * Returns true if the application can invoke the overloaded pickup feature
   * which takes two Addresses as arguments, false otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param address1 This argument is used for typing information to determine
   * the overloaded version of the pickup() method.
   * @param address2 This argument is used for typing information to determine
   * the overloaded version of the pickup() method.
   * @return True if the application can invoke the pickup feature which takes
   * two Addresses as arguments, false otherwise.
   */
  public boolean canPickup(Address address1, Address address2);


  /**
   * Returns true if the application can invoke the pickup from group feature,
   * false otherwise.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI 1.2 with
   * overloaded versions. These versions permit applications to give typed
   * argument to obtain the capabilities for a particular overloaded version
   * of the <CODE>CallControlTerminal.pickupFromGroup()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. The default behavior of this method in
   * JTAPI v1.2 and later should invoke the canPickupFromGroup(String,
   * Address) method.
    * @return True if the application can invoke the pickup from group feature,
   * false otherwise.
   */
  public boolean canPickupFromGroup();


  /**
   * Returns true if the application can invoke the pickup from group feature
   * which takes a string pickup group code and an Address as arguments, false
   * otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param group This argument is used for typing information to determine
   * the overloaded version of the pickupFromGroup() method.
   * @param address This argument is used for typing information to determine
   * the overloaded version of the pickupFromGroup() method.
   * @return True if the application can invoke the pickup from group feature
   * which takes a string pickup group code and an Address as arguments, false
   * otherwise.
   */
  public boolean canPickupFromGroup(String group, Address address);


  /**
   * Returns true if the application can invoke the pickup from group feature
   * which takes an Address as an argument, false otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param address This argument is used for typing information to determine
   * the overloaded version of the pickupFromGroup() method.
   * @return True if the application can invoke the pickup from group feature
   * which takes an Address as an argument, false otherwise.
   */
  public boolean canPickupFromGroup(Address address);
}
