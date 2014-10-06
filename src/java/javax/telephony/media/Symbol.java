/*
 * Symbol.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.media.symbols.*;
import java.lang.reflect.Field;
import java.io.Serializable;

/**
 * Symbols are opaque, immutable, nominal tokens. 
 * They are used in applications only for assignment
 * and equality/identity testing. 
 * <p>
 * Symbol is a Java class to encapsulate/implement the ECTF defined Symbols.
 * Symbols are namespace and value space controlled ints
 * that allow extensibilty by vendors, interoperability
 * across languages, and other benefits.
 * <p>
 * Symbols are defined as "public final static" members of various
 * JTAPI Media interfaces. 
 * Symbol constants are used to identify Events, Event Qualifiers, 
 * MediaService and Resource Parameters, RTC triggers Conditions,
 * RTC Actions, and Keys or Values in Dictionaries (or Maps).
 * <p>
 * <a name="prefixconvention"><!-- --></a>
 * <H4>Symbol name convention:</H4>
 * A particular Symbol (and its associated value) 
 * may be used in multiple contexts.
 * Each expected usage of the Symbol is given a
 * distinct name, composed by a diffentiating prefix
 * that indicates the usage, 
 * and an <i>item name</i> that identifies the actual value.
 * <p>
 * The convention for Symbol name prefixes is as follows:
 * <table border="1" cellpadding="3">
 * <tr><td>With prefix:</td><td>the Symbol is used as:</td>
 * <td>for example:</td></tr>
 * <tr><td align=right><code>  a_</code></td><td>Attribute</td>
 * <td><code>&nbsp;&nbsp;&nbsp;a_LocalState</code></td></tr>
 * <tr><td align=right><code>  e_</code></td><td>Error</td>
 * <td><code>&nbsp;&nbsp;&nbsp;e_Disconnected</code></td></tr>
 * <tr><td align=right><code> ev_</code></td><td>Event</td>
 * <td><code>&nbsp;&nbsp;ev_SignalDetected</code></td></tr>
 * <tr><td align=right><code>  p_</code></td><td>Parameter</td>
 * <td><code>&nbsp;&nbsp;&nbsp;p_EnabledEvents</code></td></tr>
 * <tr><td align=right><code>  q_</code></td><td>Qualifier</td>
 * <td><code>&nbsp;&nbsp;&nbsp;q_Disconnected</code></td></tr>
 * <tr><td align=right><code>rtca_</code></td><td>RTC Action</td>
 * <td><code>rtca_Stop</code></td></tr>
 * <tr><td align=right><code>rtcc_</code></td><td>RTC Condition</td>
 * <td><code>rtcc_SignalDetected</code></td></tr>
 * <tr><td align=right><code>  v_</code></td><td>Dictionary value</td>
 * <td><code>&nbsp;&nbsp;&nbsp;v_Stop</code></td></tr>
 * </table>
 * <p>
 * Each (non-error) Symbol typically appears as a final static field
 * in each interface or class to which it is relevant. 
 * This is often accomplished by defining the Symbol in a "Constants"
 * interface that is inherited by the classes or interfaces
 * in which the Symbol is relevant.
 * For example, each <i>Resource</i> class has a <i>Resource</i>Constants
 * interface that is inherited by the <i>Resource</i> interface and 
 * the associated <i>Resource</i>Event interface.
 * <p>
 * <b>Note:</b>
 * Symbols used to identify errors, 
 * (that is, the return values from {@link ResourceEvent#getError getError()}
 * are used solely for errors and do not appear in other contexts.
 * Because error Symbols are numerous and may appear in many
 * contexts,
 * the error Symbols are defined in interface {@link ErrSym}
 * and are not always renamed or inherited in other interfaces.
 * Therefore, error Symbols are generally accessed 
 * as <code>ErrSym.e_ItemName</code>.
 * <p>
 * <b>Note:</b>
 * Pre-defined Symbol constants with the same internal value 
 * generally refer to the same java.lang.Object, but this cannot be relied upon.
 * As with Integer, to compare two Symbols for numerical/logical equivalence, 
 * the {@link #equals equals} method <b>must</b> be used.
 * <p>
 * <b>Note:</b>
 * The other methods on Symbol are useful when debugging,
 * and may be used by JTAPI Media implementors,
 * but are not usually needed (or useful) during production.
 * Symbols are defined by name in final static fields,
 * by the classes that assign some meaning to a Symbol.
 * Applications do not need to create Symbols at runtime.
 * <p>
 * <h4>Why no intValue()?</h4>
 * Symbol is like an Integer but is restricted to <b>nominal</b> 
 * not <b>scalar</b> operations: 
 * it is ligitimate to use <tt>equals</tt> and <tt>hashCode</tt>, 
 * but one should <b>not</b> do arithmetic operations on a Symbol.
 * <p>
 * <b>Note:</b>
 * <tt>intValue()</tt> <i>may</i> be of use to compare against <tt>int</tt> 
 * suberror values; but such usage is so rare we don't mind telling
 * the motivated developer to use <tt>hashCode()</tt> instead of <tt>intValue()</tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */

public final
class Symbol extends javax.telephony.media.symbols.BaseSymbol implements java.io.Serializable { 

    /**
     * Returns a <code>Symbol</code> object 
     * with the given <code>int</code> value. 
     *
     * @param value the int value represented by the <code>Symbol</code>.
     * @return a Symbol object with the given value.
     */
    public static Symbol getSymbol(int value) { return new Symbol(value); }
    
    /** proforma constructor required */
    private Symbol(int value) { super(value); }

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
    public boolean equals(java.lang.Object other) {
	return super.equals(other);
    }

    /**
     * Returns a hashcode for this Symbol.
     * <p>
     * @return a hashcode value for this Symbol.
     */
    public int hashCode() {
	return super.hashCode();
    }

    /** 
     * Return a java.lang.String representation of this Symbol.
     * <p>
     * Attempt to parse the symbol value to identify the 
     * Vendor, java.lang.Object and ItemName.
     * @return a java.lang.String representing this Symbol.
     */
    public java.lang.String toString() {
	return super.toString();
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
     * A JTAPI Media vendor may alter this implementation; 
     * for example, to prepend other package names,
     * or use other techniques to map from name to Symbol.
     * 
     * @param stringName a java.lang.String that names a Symbol
     * @return the named Symbol, or null if stringName is not registered.
     */
    public static Symbol getSymbol(java.lang.String stringName) {
	return BaseSymbol.getSymbol(stringName);
    }
    
    /**
     * Assign a name to a Symbol.
     * A Symbol may be given multiple names.
     * <code>Symbol.toString()</code> uses the last assigned name.
     * <p>
     * The name applies to all Symbol object that are <tt>equal()</tt> to this.
     *
     * @param stringName the name to be given to that Symbol
     */
    public void setName(java.lang.String stringName) {
	super.setName(stringName);
    }

    /**
     * Store Symbol name information to someplace.
     * @param pathName a java.lang.String that indicates where to store the symbol table.
     */
    public static void saveSymbolNames(java.lang.String pathName) {
	BaseSymbol.saveSymbolNames(pathName);
    }

    /**
     * Load Symbol name information from somewhere.
     * 
     * @param pathName a java.lang.String that indicates which symbol table to load.
     *     if <tt>pathName</tt> is null, load standard/default symbol table.
     * <p>
     * <b>Note:</b>
     * The format of the <tt>pathName</tt> java.lang.String is not yet standardized.<br>
     * The implementation supplied in the reference code accepts strings
     * like: <tt>"class:javax.telephony.media.symbols.R2SymbolNames"</tt>.
     * The indicated class file is loaded, which runs its <tt>static</tt>
     * block and invokes <tt>Symbol.setName(java.lang.String)</tt> for each Symbol.
     */ 
    public static void loadSymbolNames(java.lang.String pathName) {
	BaseSymbol.loadSymbolNames(pathName);
    }
}    
