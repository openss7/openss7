/*
 * BaseSymbol.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Vendor modifiable base for Symbols.
 * This abstract class supplies getSymbol(String) and loadSymbols(String).
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
// package
public abstract
class BaseSymbol { 
    /*
     * <b>Implementation note:</b>
     * A Symbol wraps its internal int value exactly the way
     * an Integer wraps an int value. The runtime/performance impact 
     * is the same. 
     * Also, Symbols derive their hashcode from the internal int value.
     * <p>
     * There are two internal formats defined for Symbols. 
     * <p>
     * Provider/implementors may need to define new Symbols.
     * The protected static method Symbol.getSymbol(int value) 
     * should be used to create the Symbol for the given value.
     * <p>
     * The internal numeric value of a Symbol is partitioned into fields:
     * Format (2 LSBs), and for format = 00:<br>
     * Reserved (next  2 LSBs), Object (next 12 LSBs), ItemName (top 16 bits)
     * and for format = 01: <br>
     * Vendor   (next 10 LSBs), Object (next 10 LSBs), ItemName (top 10 bits)
     * <p>
     * There are protected methods for parsing a Symbol into its
     * internal fields: format, vendor, object, and item.
     * <p>
     * This is part of S.200 or S.300 level definitions.
     * It is currently published as "package" not "public".
     */
    /** The standard constructor. */
    protected BaseSymbol(int value) {this.value = value;} 

    /** unused no arg constructor. */
    //protected Symbol() {this(0);}

    /**
     * The unique identifing value of this Symbol.
     * this value is final, once it is set in the constructor.
     * <p>
     * int=32 bits, could be long=64 bits in next release. 
     */
    final private int value;

    /**
     * Return the internal identifing value, as an int.
     * This value may be of interest to provider implementors
     * and debuggers, but generally should not be used by applications.
     * <p>
     * Symbol is like an Integer but is restricted to <b>nominal</b> 
     * not <b>scalar</b> operations: 
     * it is ligitimate to use <tt>equals</tt> and <tt>hashCode</tt>, 
     * but one should <b>not</b> do arithmetic operations on a Symbol.
     * <p>
     * <b>Note:</b>
     * <tt>intValue()</tt> <i>may</i> be of use to compare against <tt>int</tt> 
     * suberror values; but such usage is so rare we don't mind telling
     * the motivated developer to use <tt>hashCode()</tt> instead.
     * 
     * @return the internal value as an int.
     * @see Symbol.equals
     */
    public int intValue() { return value; }
    
    /**
     * Compares the internal value of two Symbols.
     * <p>
     * The result is <tt>true</tt> if and only if the argument 
     * is an instance of Symbol and
     * contains the same internal value as this Symbol. 
     * <p>
     * <b>Note:</b>
     * The "<tt>==</tt>" operator just compares
     * the identity (object handle) of two Symbols. 
     * <p>
     * @param other a Symbol to compare to this Symbol.
     * @return TRUE iff the Symbols have the same value.
     */
    public boolean equals(Object other) {
 	return ((other instanceof BaseSymbol)
	&& (this.value == ((BaseSymbol)other).value));
    }

    /**
     * Returns a hashcode for this Symbol.
     * <p>
     * @return a hashcode value for this Symbol.
     */
    public int hashCode() {
	return value;
    }


    /** 
     * Return a String representation of this Symbol.
     * <p>
     * Attempt to parse the symbol value to identify the 
     * Vendor, Object and ItemName.
     * @return a String representing this Symbol.
     */
    public String toString() {
	return Stringifier.toString(this);
    }

    /** 
     * Returns a <code>Symbol</code> with the given name.
     * <p>
     * Returns a Symbol with the given name, if the name is recognized.
     * <p>
     * The default implementation works for Symbols that are
     * <tt>public static Symbol</tt> fields of public classes.
     * The name should be formatted as: (<b>class.FieldName</b>)
     * The package name "javax.telephony.media" is prepended if necessary.
     * <p>
     * For example: 
     * <br><code>getSymbol("ESymbol.Any_NULL");</code>
     * <br><code>getSymbol("PlayerEvent.q_RTC");</code>
     * <br><code>getSymbol("ErrSym.e_OK");</code>
     * <br><code>getSymbol("com.vendor.package.VendorSyms.Item_Name");</code>
     * <p>
     * A JTAPI Media vendor may extend this implementation; 
     * for example, to prepend other package names.
     * 
     * @param stringName a String that names a Symbol
     * @return the named Symbol, or null if stringName is not registered.
     */
    public static Symbol getSymbol(String stringName) {
	int dot = stringName.lastIndexOf('.');
	String clsName;
	String itmName;
	if (dot < 1) {
	    // first default: stringName = "Player_Play"
	    clsName = "javax.telephony.media.ESymbol";
	    itmName = stringName;
	} else {
	    clsName = stringName.substring(0,dot);
	    itmName = stringName.substring(dot+1);
	}
	try {
	    // itmName must be a "public static Symbol" of clsName
	    return (Symbol)Class.forName(clsName).getField(itmName).get(null);
	} catch (Exception ex) {}
	// Alternative default:  stringName = "Player.ev_Play" or
	// Alternative default:  stringName = "ESymbol.Player_Play"
	clsName = "javax.telephony.media." + clsName;
	try {
	    // itmName must be a "public static Symbol" of clsName
	    return (Symbol)Class.forName(clsName).getField(itmName).get(null);
	} catch (Exception ex) {}
	return null;
    }
    
    /**
     * Assign a name to a Symbol.
     * A Symbol may be given multiple names.
     * <code>Symbol.toString()</code> uses the last assigned name.
     *
     * @param symbol the Symbol to be named 
     * @param stringName the name to be given to that Symbol
     */
    public void setName(String stringName) {
	Stringifier.setSymbolName(this, stringName);
    }

    /**
     * Store Symbol name information to someplace.
     * @param pathName a String that indicates where to store the symbol table.
     */
    public static void saveSymbolNames(String pathName) {
	// dump nameSymbol = Symbol.value
	//
	// There should be a standard for this in the next release.
	// basically dump records of the form: 
	// Object_VENDOR_ItemName = 0xFFFFFF;
	//
	// or if using the Java class approach, files in the format of: 
	//    symbols/R2Symbols.java PLUS symbols/R2SymbolNames.java
	//    [and then invoke the java compiler!]
    }

    /**
     * Load Symbol name information from somewhere.
     * 
     * @param pathName a String that indicates which symbol table to load.
     *     if <tt>pathName</tt> is null, load standard/default symbol table.
     * <p>
     * <b>Note:</b>
     * The format of the <tt>pathName</tt> String is not yet standardized.<br>
     * The implementation supplied in the reference code accepts strings
     * like: <tt>"class:javax.telephony.media.symbols.R2SymbolNames"</tt>.
     * The indicated class file is loaded, which runs its <tt>static</tt>
     * block and invokes <tt>Symbol.setName(String)</tt> for each Symbol.
     */ 
    public static void loadSymbolNames(String pathName) {
	// load into nameSymbol (and symbolName)
	//
	// There should be a standard for this in the next release.
	// read file in the format dumped, 
	// and for each nameSymbol and intValue:
	// ((getSymbol(intValue)).setName(nameSymbol));
	//
	// There may be better formats later, but for now one can use:
	// loadSymbolNames("class:javax.telephony.media.symbols.R2SymbolNames");
	// Which loads that class file, and runs its static{} block,
	// which includes a Symbol.setName(String) for each R2Symbol.
	if (pathName == null) 
	    pathName = "class:javax.telephony.media.symbols.R2SymbolNames";
	if (pathName.startsWith("class:")) {
	    try {Class.forName(pathName.substring(6));}
	    catch (ClassNotFoundException ex) {
		throw new RuntimeException("Could not load symbols from: "+pathName);
	    }
	}
    }
}
