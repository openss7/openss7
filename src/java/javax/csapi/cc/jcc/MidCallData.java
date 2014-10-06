/*
 @(#) $RCSfile: MidCallData.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:33 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:34:33 $ by $Author: brian $
 */


package javax.csapi.cc.jcc;

/** An instance of this MidCallData interface is returned by
  * JccConnection.getMidCallData() after an event with id
  * JccConnectionEvent.CONNECTION_MID_CALL occured. This interface
  * provides access to the type of service code and (optionally) a
  * string (with e.g. digits) associated with it.
  * @since 1.0b
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface MidCallData {
    /** getServiceCodeType() return constant: the type of service code
      * is unknown, the corresponding value (returned by
      * getServiceCodeValue()) is operator specific.  */
    public static final int SERVICE_CODE_UNDEFINED = 0;
    /** getServiceCodeType() return constant: the user entered a digit
      * sequence during the call, the corresponding value (returned by
      * getServiceCodeValue()) is an ascii representation of the
      * received digits.  */
    public static final int SERVICE_CODE_DIGITS = 1;
    /** getServiceCodeType() return constant: a facility information
      * element is received, the corresponding value (returned by
      * getServiceCodeValue()) contains the facility information element
      * as defined in ITU Q.932.  */
    public static final int SERVICE_CODE_FACILITY = 2;
    /** getServiceCodeType() return constant: a user-to-user message was
      * received. The associated value (returned by
      * getServiceCodeValue()) contains the content of the user-to-user
      * information element. value (returned by getServiceCodeValue()) */
    public static final int SERVICE_CODE_U2U = 3;
    /** getServiceCodeType() return constant: the user performed a
      * hookflash, optionally followed by some digits. The corresponding
      * value (returned by getServiceCodeValue()) is an ascii
      * representation of the entered digits.  */
    public static final int SERVICE_CODE_HOOKFLASH = 4;
    /** getServiceCodeType() return constant: the user pressed the
      * register recall button, optionally followed by some digits. The
      * corresponding value (returned by getServiceCodeValue()) is an
      * ascii representation of the entered digits.  */
    public static final int SERVICE_CODE_RECALL = 5;
    /** Returns the type of the mid call event.
      * @return
      * An int representing the type of the mid call event. Only
      * SERVICE_CODE_UNDEFINED, SERVICE_CODE_DIGITS,
      * SERVICE_CODE_FACILITY, SERVICE_CODE_U2U, SERVICE_CODE_HOOKFLASH
      * or SERVICE_CODE_RECALL are valid return values.  */
    public int getServiceCodeType();
    /** Returns the value of the mid call event.
      * @return
      * A string representing the value of the mid call event. If no
      * value is availabe (e.g. getServiceCodeType() returns
      * SERVICE_CODE_HOOKFLASH) then null is returned.  */
    public java.lang.String getServiceCodeValue();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
