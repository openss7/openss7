/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing an ISUP Connection Request parameter which is
  * common to both the ITU and ANSI variants.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class ConReq implements java.io.Serializable {
    /** Constructs a new ConReq class, parameters with default values.  */
    public ConReq() {
        super();
    }
    /** Constructs a ConReq class from the specified input parameters.
      * @param in_localRef  The local reference, range 0 to 16,777,215.
      * @param in_spc  The point code in the JAIN ISUP Signaling Point
      * Code format.
      * @param in_protoClass  The protocol class, range 0 to 255.
      * @param in_credit  The credit, range 0 to 255.
      * @exception ParameterRangeInvalidException  Thrown when value is
      * out of range.  */
    public ConReq(int in_localRef, SignalingPointCode in_spc, short in_protoClass, short in_credit)
        throws ParameterRangeInvalidException {
        this();
        this.setLocalReference(in_localRef);
        this.setProtocolClass(in_protoClass);
        this.setSignalingPointCode(in_spc);
        this.setCredit(in_credit);
    }
    /** Gets the Local Reference of the parameter.
      * @return int the LocalReference value, range 0 to 16,777,215.  */
    public int getLocalReference() {
        return m_localRef;
    }
    /** Sets the Local Reference of the parameter.  
      * @param aLocalReference  Local reference value, range 0 to
      * 0xffffff.
      * @exception ParameterRangeInvalidException  If the sub-field is
      * out of range.  */
    public void setLocalReference(int aLocalReference)
        throws ParameterRangeInvalidException {
        if (0 > aLocalReference || aLocalReference > 16777215)
            throw new ParameterRangeInvalidException("javax.jain.ss7.siup.ConReq#m_localRef value " + aLocalReference + " is out of range.");
        m_localRef= aLocalReference;
    }
    /** Gets the Point Code field of the parameter.
      * @return  The PointCode value which is in the JAIN ISUP Signaling
      * Point Code format.  */
    public SignalingPointCode getSignalingPointCode() {
        return m_spc;
    }
    /** Sets the Point Code field of the parameter.  
      * @param aPointCode  The point code value which is in the JAIN
      * ISUP Signaling Point Code format.
      * @exception ParameterRangeInvalidException  Thrown if the value
      * is out of range.  */
    public void setSignalingPointCode(SignalingPointCode aPointCode)
        throws ParameterRangeInvalidException {
        m_spc = aPointCode;
    }
    /** Gets the Protocol Class field of the parameter.
      * @return The ProtocolClass value in the range 0 to 255.  */
    public short getProtocolClass() {
        return m_protoClass;
    }
    /** Sets the Protocol Class field of the parameter.
      * @param aProtocolClass  The ProtocolClass value, range 0 to 255.
      * @exception ParameterRangeInvalidException  Thrown if the
      * sub-field is out of the specified range.  */
    public void setProtocolClass(short aProtocolClass)
        throws ParameterRangeInvalidException {
        if (0 > aProtocolClass || aProtocolClass > 255)
            throw new ParameterRangeInvalidException("javax.jain.ss7.isup.ConReq#m_credit " + aProtocolClass + " is out of range");
        m_protoClass = aProtocolClass;
    }
    /** Gets the Credit field of the parameter.
      * @return The Credit value, range 0 to 255.  */
    public short getCredit() {
        return m_credit;
    }
    /** Sets the Credit field of the parameter.
      * @param aCredit  The credit value, range 0 to 255.
      * @throws ParameterRangeInvalidException  If the sub-field is out
      * of range.  */
    public void setCredit(short aCredit)
        throws ParameterRangeInvalidException {
        if (0 > aCredit || aCredit > 255)
            throw new ParameterRangeInvalidException("javax.jain.ss7.isup.ConReq#m_credit " + aCredit + " is out of range");
        m_credit = aCredit;
    }
    /** The toString method retrieves a string containing the values of
      * the members of the ConReq class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ConReq:");
        b.append("\n\tm_localRef: " + m_localRef);
        b.append("\n\tm_spc: " + m_spc);
        b.append("\n\tm_protoClass: " + m_protoClass);
        b.append("\n\tm_credit: " + m_credit);
        return b.toString();
    }

    protected int m_localRef;
    protected SignalingPointCode m_spc;
    protected short m_protoClass;
    protected short m_credit;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
