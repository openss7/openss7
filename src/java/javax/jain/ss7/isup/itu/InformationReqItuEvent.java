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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The InformationReqItuEvent class is a sub class of the JAIN ISUP Core
  * InformationReqEvent class and is used for handling of parameters for the ITU variant
  * of Information Request Message.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class InformationReqItuEvent extends InformationReqEvent {
    /** Constructs a new InformationReqItuEvent, with only the JAIN ISUP Mandatory
      * parameters by calling the super class constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param in_infoInd  Information indicators.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public InformationReqItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            InfoReqInd in_infoReqInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, in_infoReqInd);
    }
    /** Gets the NwSpecificFacItu parameter of the message.
      * @return The NwSpecificFacItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public NwSpecificFacItu getNwSpecificFac()
        throws ParameterNotSetException {
        if (m_nsf != null)
            return m_nsf;
        throw new ParameterNotSetException("NwSpecificFacItu not set.");
    }
    /** Sets the NwSpecificFacItu parameter of the message.
      * @param nsf  The NwSpecificFacItu parameter of the event.
      */
    public void setNwSpecificFac(NwSpecificFacItu nsf) {
        m_nsf = nsf;
    }
    /** Indicates if the NwSpecificFacItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isNwSpecificFacPresent() {
        return (m_nsf != null);
    }
    /** Gets the ParamCompatibilityInfoItu parameter of the message.
      * @return The ParamCompatibilityInfoItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_pci != null)
            return m_pci;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the ParamCompatibilityInfoItu parameter of the message.
      * @param pci  The ParamCompatibilityInfoItu parameter of the event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci) {
        m_pci = pci;
    }
    /** Indicates if the ParamCompatibilityInfoItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_pci != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * InformationReqItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.InformationReqItuEvent");
        b.append("\n\tm_nsf: " + m_nsf);
        b.append("\n\tm_pci: " + m_pci);
        return b.toString();
    }
    protected NwSpecificFacItu m_nsf = null;
    protected ParamCompatibilityInfoItu m_pci = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
