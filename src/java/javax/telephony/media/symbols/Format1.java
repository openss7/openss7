/*
 * Format1.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.symbols;
import javax.telephony.media.*;
import java.util.Hashtable;

/**
 * A Parser for S100-R2 compatible, Vendor defined Symbols with FORMAT==1.
 * <p>
 * The internal numeric value of Format1 is partitioned into fields:<br>
 * Format (2 LSBs), and for format = 01: <br>
 * Vendor   (next 10 LSBs), Object (next 10 LSBs), ItemName (top 10 bits)<br>
 * <p>
 * Server and Resource vendors can use this class, and their [ECTF] defined
 * vendor ID to create unique Symbols. Print strings for whole symbols
 * or Object/Vendor fields may be registered to provide readable print strings.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
// package
public
class Format1 implements Stringifier.Parser {
    /** IIIIIIIIIIOOOOOOOOOOVVVVVVVVVVFF */
    private static final int FORMAT = 01;
    private static final int maskFormat	= 0x00000003;	// format bits [==1]
    private static final int maskVendor	= 0x00000fff;	// format & vendor
    private static final int maskObject	= 0x003fffff;	// object bits 
    private static final int maskItem	= 0xffC00000;	// just the Item bits

    private static final int venShift = 2;
    private static final int objShift = 12;
    private static final int itemShift = 22;
    
    public boolean isFormat(int value) {return (value & maskFormat) == FORMAT;}
    public int getVendor(int value)    {return (value & maskVendor);}
    public int getVendorOnly(int value){return (value & maskVendor)>>venShift;}
    public int getObject(int value)    {return (value & maskObject);}
    public int getObjectOnly(int value){return (value & maskObject)>>objShift;}
    public int getItem(int value)      {return (value & maskItem)>>itemShift;}

    /** compute Symbol.value for the given attributes.
     * <p><b>Note:</b> This does not do any range checking.
     * If ven or obj or item is too large, it will corrupt adjacent fields.
     * For <tt>Format1</tt> each of these is restricted to 10 bits (< 1024).
     * @param ven a legal int VendorID value
     * @param obj a legal int ObjectID value
     * @param item a legal int ItemName value
     */
    static protected int f1value(int ven, int obj, int item) {
	return (FORMAT+
		(ven<<venShift)+
		(obj<<objShift)+
		(item<<itemShift));
    }

    /** create Symbol for given vendor, object and itemName fields.
     * <p><b>Note:</b> This does not do any range checking.
     * If ven or obj or item is too large, it will corrupt adjacent fields.
     * For <tt>Format1</tt> each of these is restricted to 10 bits (< 1024).
     * @param ven a legal int VendorID value
     * @param obj a legal int ObjectID value
     * @param item a legal int ItemName value
     */
    public static Symbol f1symbol(int ven, int obj, int item) {
	return Symbol.getSymbol((item<<itemShift)+
				(obj<<objShift)+
				(ven<<venShift)+
				FORMAT);
    }
    /** use Format1 to Stringify Symbols with value:(FORMAT==1) */
    static {Stringifier.addParser(new Format1());}
}
