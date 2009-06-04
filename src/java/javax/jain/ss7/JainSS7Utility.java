/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Module Name   : JAIN Common API
 *  File Name     : JainSS7Utility.java
 *  Originator    : Phelim O'Doherty
 *  Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

/**
 * Utility class for JAIN SS7.
 * All methods are static and shall only be used within the JAIN SS7 packages
 *
 * @version 1.1
 * @author Sun Microsystems Inc.
 */

public class JainSS7Utility {

    private JainSS7Utility(){
    }

    /**
    * Convert an array of bytes to String with hex values.
    * @param data A byte array.
    * @param start The start in data from where to convert.
    * @param end The end in data.
    * @return A byte sequence ready to convert to a String, i.e: <BR>
    * <pre>new String(bytesToHex(data, 0, len)) </pre>
    */
    public static byte[] bytesToHex(byte[] data, int start, int end) {
        byte[] hexNums = new byte[(end-start)*3];
        int hexPos = 0;
        byte loByte = 0;
        byte hiByte = 0;

        for(int i=start; i<data.length && i<end; i++) {
            hiByte = (byte) ((data[i] & 0xF0) >> 4);
            loByte = (byte) (data[i] & 0x0F);
            //Insert ascii 0..9 or A..F:
            hexNums[hexPos++] = (byte) (hiByte > 9 ? hiByte+55 : hiByte+48);
            hexNums[hexPos++] = (byte) (loByte > 9 ? loByte+55 : loByte+48);
            hexNums[hexPos++] = 32; //ascii space
        }
        return hexNums;
    }
}
