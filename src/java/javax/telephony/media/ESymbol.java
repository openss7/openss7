/*
 * ESymbol.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * Presents a Java object for each Symbol.
 * <p>
 * The fields of this interace contain Symbol objects
 * that can be assigned or used as target object for <tt>.equals()</tt>
 * However, standard/portable S.410 applications should find all
 * the necessary API Symbols in the fields of the interfaces that
 * actually use those Symbols;
 * and those fields will have names that conform to the 
 * <a href=Symbol#prefixconvention>Symbol prefix convention</a>:
 * <tt>(a_, ev_, p_, etc.)</tt>
 * <p>
 * Names and values in ESymbol are compatible with the ECTF Symbol
 * definitions (from S.100-R2) using the <i>canonical</i> names.
 * Each ESymbol field name <tt>ESymbol.Obj_ItemName</tt>
 * is equivalent to the S.100 <tt>Obj_ECTF_ItemName</tt>. 
 * <p>
 * <b>Note:</b> ESymbol field names of the form "<tt>Obj_</tt>", with a trailing
 * underscore are <tt>int</tt> fields holding an ECTF Object value.
 * <br>For example: <tt>int Player_ = 0x0F</tt>
 * <p>
 * <b>Note:</b>Symbols are assigned to public fields in other interfaces,
 * and that is the preferred way to access Symbols.
 * This interface is included to allow consistant access
 * to additional Symbols that may be useful for a particular implementation.
 * <p>
 * <b>Note:</b> The actual Symbol definitions are inherited, 
 * so they generate a smaller javadoc page.
 * Also because the values are not generally relevant to API programmers.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
interface ESymbol extends javax.telephony.media.symbols.R2Symbol {}

