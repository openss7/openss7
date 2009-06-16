/*
 * Format0.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * A Parser for S100-R2 compatible ECTF defined Symbols with FORMAT==0.
 * <p>
 * The internal numeric value of Format0 is partitioned into fields:<br>
 * Format (2 LSBs), and for format = 00:<br>
 * Reserved (next  2 LSBs), Object (next 12 LSBs), Item (top 16 bits)<br>
 * <p>
 * For ECTF defined Symbols, the Vendor (ECTF) is <i>implicity</i> <b>zero</b>.
 * <p>
 * A static block defines Stringifier print strings for the ECTF object 
 * types.  
 * Done in this file so we can find the private method setObjName().
 * If R2SymbolNames is loaded it provides full print strings
 * for all the Symbols defined in R2Symbol.java (hopefully).
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
// package
public
class Format0 implements Stringifier.Parser {
    /** IIIIIIIIIIIIIIIIOOOOOOOOOOOOXXFF */
    private static final int FORMAT = 00;
    private static final int maskFormat	= 0x0000000f; // format bits [==0]
    private static final int maskVendor	= 0x0000000f; // format & vendor [==0]
    private static final int maskObject	= 0x0000ffff; // object bits 
    private static final int maskItem	= 0xffff0000; // just the Item bits
    
    private static final int venShift 	= 0; 	      // Vendor = Format = 0:
    private static final int objShift 	= 4;
    private static final int itemShift	= 16;
	
    public boolean isFormat(int value) {return (value & maskFormat) == FORMAT;}
    public int getVendor(int value)    {return (value & maskVendor);}
    public int getVendorOnly(int value){return (value & maskVendor)>>venShift;}
    public int getObject(int value)    {return (value & maskObject);}
    public int getObjectOnly(int value){return (value & maskObject)>>objShift;}
    public int getItem(int value)      {return (value & maskItem)>>itemShift;}

    /** Used exclusively for Vendor = ECTF (ven == 0). */
    public static Symbol f0symbol(int obj, int item) {
	return Symbol.getSymbol((item<<itemShift)+(obj<<objShift)+FORMAT);
    }
    /** use Format0 to Stringify Symbols with value:(format==0) */
    static {Stringifier.addParser(new Format0());}


    /** Setup print string translations for "Object" of ECTF 
     * String is assigned to the combination of ObjectVendorFormat
     * as returned by Stringifier.getObject();
     * [value is computed with vendor and format = 0]
     */
    private static void setObjName(int obj, String name) {
	Stringifier.setObjectName(obj<<objShift, name+"_ECTF");
    }

    /** Set object names for the ECTF defined object types. 
     * In this file so it can find the private method setObjName().
     * R2Symbol is an interface, can not have static initializer blocks.
     */
    static {
	setObjName(R2Symbol.Any_		, "Any");
	setObjName(R2Symbol.ASR_		, "ASR");
	setObjName(R2Symbol.CCR_		, "CCR");
	setObjName(R2Symbol.CPR_		, "CPR");
	setObjName(R2Symbol.Container_		, "Container");
	setObjName(R2Symbol.Error_		, "Error");
	setObjName(R2Symbol.FaxReceiver_	, "FaxReceiver");
	setObjName(R2Symbol.FaxSender_		, "FaxSender");
	setObjName(R2Symbol.Faxll_		, "Faxll");
	setObjName(R2Symbol.Fax_		, "Fax");
	setObjName(R2Symbol.Group_		, "Group");
	setObjName(R2Symbol.KVS_		, "KVS");
	setObjName(R2Symbol.Language_		, "Language");
	setObjName(R2Symbol.Macro_		, "Macro");
	setObjName(R2Symbol.Message_		, "Message");
	setObjName(R2Symbol.Player_		, "Player");
	setObjName(R2Symbol.Recorder_		, "Recorder");
	setObjName(R2Symbol.SCR_		, "SCR");
	setObjName(R2Symbol.Session_		, "Session");
	setObjName(R2Symbol.SD_			, "SD");
	setObjName(R2Symbol.SG_			, "SG");
	setObjName(R2Symbol.Admin_		, "Admin");
	setObjName(R2Symbol.SessionManager_	, "SessionManager");
	setObjName(R2Symbol.Symbol_		, "Symbol");
	setObjName(R2Symbol.Application_	, "Application");
	setObjName(R2Symbol.Service_		, "AppService");
    }
}
