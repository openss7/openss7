/*
 @(#) src/java/javax/jain/ss7/inap/datatype/ServiceAddressInformation.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class defines the Service Address Information DataType.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ServiceAddressInformation implements java.io.Serializable {
    /** Constructor For ServiceAddressInformation.  */
    public ServiceAddressInformation(MiscCallInfo miscCallInfo) {
        setMiscCallInfo(miscCallInfo);
    }
    /** Gets Service Key.  */
    public int getServiceKey()
        throws ParameterNotSetException {
        if (isServiceKey)
            return serviceKey;
        throw new ParameterNotSetException("Service Key: not set.");
    }
    /** Sets Service Key.  */
    public void setServiceKey(int serviceKey)
        throws IllegalArgumentException {
        if (0 <= serviceKey && serviceKey <= 2147483647) {
            this.serviceKey = serviceKey;
            isServiceKey = true;
            return;
        }
        throw new IllegalArgumentException("Service Key: " + serviceKey + ", out of range.");
    }
    /** Indicates if the Service Key optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceKeyPresent() {
        return isServiceKey;
    }
    /** Gets Misc Call Info.  */
    public MiscCallInfo getMiscCallInfo() {
        return miscCallInfo;
    }
    /** Sets Misc Call Info.  */
    public void setMiscCallInfo(MiscCallInfo miscCallInfo) {
        this.miscCallInfo = miscCallInfo;
    }
    /** Gets Trigger Type.  */
    public TriggerType getTriggerType()
        throws ParameterNotSetException {
        if (isTriggerType)
            return triggerType;
        throw new ParameterNotSetException("Trigger Type: not set.");
    }
    /** Sets Trigger Type.  */
    public void setTriggerType(TriggerType triggerType) {
        this.triggerType = triggerType;
        isTriggerType = true;
    }
    /** Indicates if the Trigger Type optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isTriggerTypePresent() {
        return isTriggerType;
    }
    private int serviceKey;
    private boolean isServiceKey = false;
    private MiscCallInfo miscCallInfo;
    private TriggerType triggerType;
    private boolean isTriggerType = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
