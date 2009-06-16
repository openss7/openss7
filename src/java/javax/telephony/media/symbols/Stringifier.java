/*
 * Stringifier.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** Methods to compose print names for Symbols.
 * Includes an interface to methods that parse the
 * various Symbol formats.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class Stringifier {
    /** object with this interface helps us parse the
     * various symbol formats.*/
    public static
    interface Parser {
	/** return true if this Parser can parse this symbol value. */
	boolean isFormat(int value);
	
	/**
	 * Isolate the bit field containing Vendor & Format.
	 * @return the int value of the Vendor field.
	 */
	int getVendor(int value);

	/**
	 * Return the official ECTF vendor value.
	 */
	int getVendorOnly(int value);
	
	/**
	 * Isolate the bit field containing Object, Vendor, Format bits.
	 * @return the int value of the Object, Vendor, and Format fields
	 */
	int getObject(int value);
	
	/**
	 * @return the int field containing only the Object bits.
	 */
	int getObjectOnly(int value);
	
	/**
	 * the bit field containing the Item, right-shifted as necessary.
	 * @return the int value of the Item segment of this Symbol.
	 */
	int getItem(int value);
    }

    public
    static Parser[] parsers = new Parser[0];

    public static void addParser(Parser parser) {
	synchronized (parsers) {
	    int len = parsers.length;
	    Parser[] parsers1 = new Parser[len+1];
	    System.arraycopy(parsers, 0, parsers1, 0, len);
	    parsers1[len] = parser;
	    parsers = parsers1;
	}
    }
    public static Parser getParser(int value) {
	for(int i = parsers.length-1; i>=0; i--) 
	    if (parsers[i].isFormat(value)) return parsers[i];
	return null;
    }
    
    /**
     * Decode and print as many of the Symbol components as
     * are registered.
     * Unregistered segments are printed in hex.
     */
    public static String toString(BaseSymbol symbol) {
	// see if whole name is registered for this value
	String symstr = (String)symbolName.get(symbol);
	if (symstr != null) return symstr;
	// whole value is not registered, try parsing it:
	
	int value = symbol.hashCode();
	// look for a Symbol parser for this format:
	Parser parser = getParser(value);
	if (parser == null)	// if no Parser, then give up.
	    // everything in hex, don't bother with the underbars:
	    return "Symbol(0x" + Integer.toString(value, 16)+")";
	

	// This Symbol not registered, item will have to be in hex.
 	String itemstr = "0x" + Integer.toString(parser.getItem(value), 16);

	// if Obj_VENDOR is registered use that:
	int objectValue = parser.getObject(value);
	symstr = getObjectName(objectValue);
	if (symstr != null) return symstr + "_" + itemstr;

	// Object name not registered, use hex for Object segment.
	String objstr = "0x"+Integer.toString(parser.getObjectOnly(value),16);

	// Check for known vendor:
	int vendorValue = parser.getVendorOnly(value);
	String vendor = getVendorName(vendorValue);
	if (vendor != null) return objstr+"_"+vendor+"_"+itemstr;
	
	// everything in hex, but parsed into fields:
	vendor = "0x"+Integer.toString(parser.getVendorOnly(value),16);
	return objstr+"_"+vendor+"_"+itemstr;

    }
    /** Hashtable maps Symbol to its Name */
    static Hashtable symbolName = new Hashtable();
    public static void setSymbolName(BaseSymbol symbol, String name) {
	if (name != null)
	    symbolName.put(symbol, name);
	else
	    symbolName.remove(symbol);
    }

    /** map from ObjectVendor to name */
    protected static Hashtable venobjName = new Hashtable();

    /** allow vendors to label their ObjectVendorFormat combination */
    public static void setObjectName(int obj, String name) {
	if (name != null)
	    venobjName.put(new Integer(obj), name);
	else 
	    venobjName.remove(new Integer(obj));
    }
    /** @return the String associated with a given ObjectVendorFormat */
    protected static String getObjectName(int objectValue) {
	return (String)venobjName.get(new Integer(objectValue));
    }

    /* There should also be a "Symbol.parseName(String)"
     * which tries to reconstitute a Symbol based on a toString() result.
     * That is, recode from hex if necessary.
     */

    /** Register a print String associated with the given vendor ID. 
     * Vendor IDs and the associated names are assigned by ECTF.
     * @see "http://www.ectf.org/ectf/tech/vendint.htm"
     */
    public static void registerVendor(int vendor, String name) {
	if (name != null)
	    vendorString.put(new Integer(vendor), name);
	else
	    vendorString.remove(new Integer(vendor));
    }
    /** Return Vendor's name String, if it is registered. 
     * @see registerVendor
     */
    protected static String getVendorName(int vendorValue) {
	return (String)vendorString.get(new Integer(vendorValue));
    }
    /** Contains String names for each vendorID. 
     * Could be an Array, since the VendorID is compact small int
     */
    private static Hashtable vendorString = new Hashtable();

    /**
     * We put these into a table with the String versions, 
     * so one can translate Symbol values back to reasonable names.
     * <p>
     * This table downloaded 7/10/2000 from
     * <a href="http://www.ectf.org/ectf/tech/vendrgty.htm">
     * http://www.ectf.org/specs/vendrgty.htm</a>
     */
    static {
	registerVendor(0, "ECTF");	// ECTF 
	registerVendor(1, "DLGC");	// Dialogic 
	registerVendor(2, "AMTEV");	// Amteva 
	registerVendor(3, "NMS");	// Natural Microsystems 
	registerVendor(4, "BRT");	// Brooktrout 
	registerVendor(5, "AMTEL");	// Amtelco 
	registerVendor(6, "NORTL");	// Nortel Networks 
	registerVendor(7, "PERI");	// Periphonics 
	registerVendor(8, "DTAG");	// Deutsche Telekom 
	registerVendor(9, "RKWL");	// Rockwell Electronic Commerce 
	registerVendor(10, "ASPT");	// Aspect Telecommunications 
	registerVendor(11, "PIKA");	// Pika Tech. 
	registerVendor(12, "CSELT");	// CSELT 
	registerVendor(13, "ACU");	// Aculab Plc 
	registerVendor(14, "DCSR");	// TEMIC 
	registerVendor(15, "MCI");	// MCI 
	registerVendor(16, "PTI");	// Performance Technologies, Inc. 
	registerVendor(17, "SPCO");	// Space Connection NV 
	registerVendor(18, "ATT");	// AT & T 
	registerVendor(19, "WM");	// Wilhelm Mikroelektronik Gmbh 
	registerVendor(20, "ETTS");	// Elan Informatique 
	registerVendor(21, "LHSP");	// Lernout & Hauspie Speech Products 
	registerVendor(22, "VICORP");	// Vicorp Interactive Systems, Inc. 
	registerVendor(23, "DTB");	// Deutsche Telecom Berkom GmbH 
	registerVendor(24, "PSP");	// Philips Speech Processing 
	registerVendor(25, "TSI");	// TimeShift, Inc. 
	registerVendor(26, "PHYLO");	// Phylon Communications, Inc. 
	registerVendor(27, "OTF");	// Commetrex Corporation 
	registerVendor(28, "ACB");	// AcoustiCode Bar GmbH 
	registerVendor(29, "VIPR");	// Viper Communications 
	registerVendor(30, "ESP");	// ESP, LLC 
	registerVendor(31, "ATS");	// ATS Advanced Technology Systems srl 
	registerVendor(32, "ND");	// NTT DATA Corporation 
	registerVendor(33, "NP1");	// N + 1, Inc. 
	registerVendor(34, "AVTEC");	// Avtec, Inc. 
	registerVendor(35, "LU");	// Lucent Technologies 
	registerVendor(36, "PAS");	// PAS GmbH 
	registerVendor(37, "VCEBD");	// Voiceboard Corporation 
	registerVendor(38, "IDX");	// InfoDynamix 
	registerVendor(39, "NUANC");	// Nuance Communications 
	registerVendor(40, "ASND");	// Ascend Communications, Inc. 
	registerVendor(41, "OC");	// OpenComm do Brasil Ltda. 
	registerVendor(42, "SWI");	// SpeechWorks, International 
	registerVendor(43, "MSFT");	// Microsoft Corporation 
	registerVendor(44, "RCON");	// RealConnect (Pty) Ltd 
	registerVendor(45, "WIT");	// Witness Systems, Inc. 
	registerVendor(46, "DRI");	// DATA RACE, Inc. 
	registerVendor(47, "TSD");	// Telesoft Design Ltd. 
	registerVendor(48, "EIC");	// Eicon Technology 
	registerVendor(49, "PCZO");	// Picazo Communications 
	registerVendor(50, "XIOX");	// Xiox Corporation 
	registerVendor(51, "TOPC");	// TOPCALL International AG 
	registerVendor(52, "BABU");	// Object Web 
	registerVendor(53, "WEB");	// Webversant 
	registerVendor(54, "GROUP");	// Group 2000 Nederland B.V. 
	registerVendor(55, "CELIT");	// Cellit, Inc. 
	registerVendor(56, "NECSY");	// Necsy S.p.A 
	registerVendor(57, "TSYS");	// Thinking Systems 
	registerVendor(58, "IGEN");	// Intergen Group, Ltd. 
	registerVendor(59, "ASFT");	// Artisoft, Inc. 
	registerVendor(60, "TPSW");	// Plant Technology Solutions 
	registerVendor(61, "EXLNT");	// Excelerant Software Services 
	registerVendor(62, "ALNAD");	// Alcatel 
	registerVendor(63, "IT");	// InfoTalk Technology 
	registerVendor(64, "CARES");	// Centurion Cares, Inc. 
	registerVendor(65, "SPRO");	// SoftPro, LLC 
	registerVendor(66, "AS");	// Altitude Software 
	registerVendor(67, "PRTY");	// Parity Software 
	// added for JTAPI-1.4
	registerVendor(68, "ELIX");	// Elix
	registerVendor(69, "REAG");	// Reagency Systems Corporation 
	registerVendor(70, "SYS");	// SYS Telekom AS 
	registerVendor(71, "ATSN");	// Artesyn Technologies 
	registerVendor(72, "FJ");	// Fujitsu Limited 
	registerVendor(73, "ORCL");	// Oracle 
	registerVendor(74, "RANE");	// Rane Technologies 
	registerVendor(75, "AMCAT");	// American Computer & Telephone 
	// registerVendor(76, "IMEDIA");	// Imedia Corporation 
	registerVendor(77, "IMAN");	// Infoman AG 
	registerVendor(78, "ENVOX");	// Envox 
	registerVendor(79, "PIVO");	// Pivotech Systems 
	registerVendor(80, "SSOL");	// System Solutions 
	registerVendor(81, "PHO");	// Phoneware 
	registerVendor(82, "OKE");	// Connect-IT Communication 
	registerVendor(83, "3COM");	// 3Com Voice Solutions 
	// registerVendor(84, "G3NOVA");	// G3 NOVA Technology 
	registerVendor(85, "ASG");	// Atlantic Systems Group 
	registerVendor(86, "NDAG");	// Notes Development AG 
	registerVendor(87, "XNDIA");	// Excendia 
    }
}
