/**
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

package javax.telephony.callcontrol;
import  javax.telephony.*;


/**
 * The <CODE>CallControlForwarding</CODE> class represents a <EM>forwarding
 * instruction</EM>. This instruction tells how the platform should forward
 * incoming telephone calls to a specific address. There are several attributes
 * to a forwarding instruction. 
 * <p>
 * The first attribute is its <EM>type</EM>. The forwarding instruction's type
 * tells the platform when to forward the call. There are currently three types
 * of instructions: telling the platform to always forward incoming calls,
 * telling the platform to forward incoming calls when the address is busy, and
 * telling the platform to forward incoming calls when no one answers.
 * <p>
 * The second attribute of a forwarding instruction is its <EM>filter</EM>.
 * The filter indicates which type of incoming calls this forwarding 
 * instruction should apply to. This forwarding instruction can apply to all
 * calls, to external calls only, to internal calls only, or to a specific
 * calling address.
 */

public class CallControlForwarding {
 
  private String destAddress;
  private String caller;
  private int    type;
  private int    whichCalls;
  
  /**
   * Forwarding filter: apply instruction to all incoming calls.
   */
  public final static int ALL_CALLS = 0x1;


  /**
   * Forwarding filter: apply instruction to calls originating from the
   * provider domain. */
  public final static int INTERNAL_CALLS = 0x2;


  /**
   * Forwarding filter: apply instruction to calls originating from outside
   * the provider domain.
   */
  public final static int EXTERNAL_CALLS = 0x3;


  /**
   * Forwarding filter: apply instruction to calls originating from a specific
   * address.
   */
  public final static int SPECIFIC_ADDRESS = 0x4;


  /**
   * Forwarding type: forward calls unconditionally.
   */
  public final static int FORWARD_UNCONDITIONALLY = 0x1;

  
  /**
   * Forwarding type: forward calls on busy.
   */
  public final static int FORWARD_ON_BUSY = 0x2;

  
  /**
   * Forwarding type: forward calls on no answer.
   */
  public final static int FORWARD_ON_NOANSWER = 0x3;
  

  /**
   * This constructor is the default constructor, which only takes the
   * address to apply this forwarding instruction. The forwarding instruction
   * forwards all calls unconditionally.
   *
   * @param destAddress destination address for the call forwarding operation
   */
  public CallControlForwarding(String destAddress) {
    this.destAddress = destAddress;
    type             = FORWARD_UNCONDITIONALLY;
    caller           = null;
    whichCalls       = ALL_CALLS;
  }
  

  /**
   * This constructor takes the address to apply this forwarding instruction
   * and the type of forwarding for all incoming calls.
   *
   * @param destAddress destination address for the call forwarding operation
   * @param type the type of the forwarding operation
   */
  public CallControlForwarding(String destAddress, int type) {
    this.destAddress = destAddress;
    this.type        = type;
    caller           = null;
    whichCalls       = ALL_CALLS;
  }


  /**
   * This constructor takes the address to apply this forwarding instruction,
   * the type of forwarding desired for this address, and a boolean flag
   * indicating whether this instruction should apply to internal (true) or 
   * external (false) calls.
   *
   * @param destAddress destination address for the call forwarding operation
   * @param type the type of the forwarding operation
   * @param internalCalls flag for internal call forwarding
   */
  public CallControlForwarding(String destAddress, int type, boolean
	  		       internalCalls) {
    this.destAddress = destAddress;
    this.type        = type;
    caller           = null;
    if (internalCalls) {
      whichCalls = INTERNAL_CALLS;
    }
    else {
      whichCalls = EXTERNAL_CALLS;
    }
  }
  

  /**
   * This constructor takes an address to apply the forwarding instruction
   * for a specific incoming telephone call, identified by a string address.
   * It also takes the type of forwarding desired for this specific address.
   *
   * @param destAddress destination address for the call forwarding operation
   * @param type the type of the forwarding operation
   * @param caller the address of the incoming caller
   */
  public CallControlForwarding(String destAddress, int type, String caller) {
    this.destAddress = destAddress;
    this.type        = type;
    this.caller      = caller;
    whichCalls       = SPECIFIC_ADDRESS;
  }


  /**
   * Returns the destination address of this forwarding instruction.
   * <p>
   * @return The destination address of this forwarding instruction.
   */
  public String getDestinationAddress() {
    return this.destAddress;
  }


  /**
   * Returns the type of this forwarding instruction, either unconditionally,
   * upon no answer, or upon busy.
   * <p>
   * @return The type of this forwarding instruction.
   */
  public int getType() {
    return this.type;
  }


  /**
   * Returns the filter of this forwarding instruction. The filter indicates
   * which calls should trigger this forwarding instruction. Filters include:
   * applying this instruction to all calls, to only internal calls, to only
   * external call, or for calls from a specific address.
   * <p>
   * @return The filter for this forwarding instruction. 
   */
  public int getFilter() {
    return this.whichCalls;
  }


  /**
   * If the filter for this forwarding instruction is SPECIFIC_ADDRESS, then
   * this method returns that calling address to which this filter applies.
   * If the filter is something other than SPECIFIC_ADDRESS, this method
   * returns null.
   * <p>
   * @return The specific address for this forwarding instruction.
   */
  public String getSpecificCaller() {
    return this.caller;
  }
}
