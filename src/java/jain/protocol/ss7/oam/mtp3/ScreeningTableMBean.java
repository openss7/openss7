/*
 @(#) $RCSfile: ScreeningTableMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:27 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:27 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods of an STP Screening Table Managed Bean(MBean). This
  * MBean allows the optional SS7 functionality for an STP to identify and to handle
  * unauthorized SS7 messages. Since the prevention of unauthorized use of an STP is an
  * optional function this MBean may be optionally implemented also. <p>
  *
  * It is possible to inhibit/allow messages destined for another signalling point (SP)
  * based on any one or combination of the following options (ITU Q.705, Section 8.2): <dl>
  *
  * <dt>SCR_TYPE_INCOMING_LS_TO_DPC
  * <dd>To inhibit/allow STP access by a combination of designated incoming link sets to
  * designated DPCs.
  *
  * <dt>SCR_TYPE_OUTGOING_LS_TO_DPC
  * <dd>To inhibit/allow STP access by a combination of designated outgoing link sets to
  * designated DPCs.
  *
  * <dt>SCR_TYPE_OPC_TO_DPC
  * <dd>To inhibit/allow STP access by examination of OPC and DPC combination in the
  * incoming STP message. </dl>
  *
  * The option to be used is configurable using the setScreeningType() method. <p>
  *
  * An STP identifying unauthorized SS7 messages should be able, on a per linkset or per
  * signalling point code basis, to <ol>
  *
  * <li> 1. provide all unauthorized SS7 messages with the same handling as authorized
  * traffic; or
  *
  * <li> 2. discard all unauthorized SS7 messages. </ol>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface ScreeningTableMBean extends Mtp3ManagedObjectMBean {
    /**
      * To inhibit/allow STP access by a combination of designated incoming link sets to
      * designated DPCs.
      */
    public static final int SCR_TYPE_INCOMING_LS_TO_DPC = 1;
    /**
      * To inhibit/allow STP access by a combination of designated outgoing link sets to
      * designated DPCs.
      */
    public static final int SCR_TYPE_OUTGOING_LS_TO_DPC = 2;
    /**
      * To inhibit/allow STP access by examination of OPC and DPC combination in the
      * incoming STP message.
      */
    public static final int SCR_TYPE_OPC_TO_DPC = 3;
    /**
      * Sets the screening Type option to be used.
      *
      * @param screeningType
      * either: <ul>
      * <li>SCR_TYPE_INCOMING_LS_TO_DPC<br> To inhibit/allow STP access by a combination
      * of designated incoming link sets to designated DPCs.
      * <li>SCR_TYPE_OUTGOING_LS_TO_DPC<br> To inhibit/allow STP access by a combination
      * of designated outgoing link sets to designated DPCs.
      * <li>SCR_TYPE_OPC_TO_DPC<br> To inhibit/allow STP access by examination of OPC and
      * DPC combination in the incoming STP message. </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied screening type is not one of the defined screening types.
      */
    public void setScreeningType(Integer screeningType)
        throws IllegalArgumentException;
    /**
      * Returns the screening type option that is in use.
      * 
      * @return
      * either: <ul>
      * <li>SCR_TYPE_INCOMING_LS_TO_DPC<br> To inhibit/allow STP access by a combination
      * of designated incoming link sets to designated DPCs.
      * <li>SCR_TYPE_OUTGOING_LS_TO_DPC<br> To inhibit/allow STP access by a combination
      * of designated outgoing link sets to designated DPCs.
      * <li>SCR_TYPE_OPC_TO_DPC<br> To inhibit/allow STP access by examination of OPC and
      * DPC combination in the incoming STP message. </ul>
      */
    public Integer getScreeningType();
    /**
      * Returns an array of ObjectNames representing either all of the allowed Linksets or
      * all of the allowed Originating Point Codes for the specified destination point
      * code.  The type of Objects that is returned in the array of ObjectNames is
      * dependent on the screening type option.
      *
      * @param destSignallingPoint
      * the ObjectName of the destination Signalling Point for which the allowed list is
      * to be returned.
      *
      * @return
      * either: <ul>
      * <li>the allowed Linksets if the screening type is either: <ul>
      *     <li>SCR_TYPE_INCOMING_LS_TO_DPC
      *     <li>SCR_TYPE_OUTGOING_LS_TO_DPC </ul>
      * or
      * <li>the allowed Originating Signalling Points if the screening type is: <ul>
      *     <li>SCR_TYPE_OPC_TO_DPC </ul> </ul>
      */
    public ObjectName[] getValidList(ObjectName destSignallingPoint);
    /**
      * Sets an array of Objects representing either: <ul>
      * <li>all of the allowed Linksets, or
      * <li>all of the allowed Originating Point Codes </ul>
      * for the specified destination point code.
      * The type of Object that is supplied in the Object array (identified from the array
      * of ObjectNames) is dependent on the screening type option. 0
      *
      * @oaram authorisedList
      * an array of ObjectNames identifing the authorised list for the Destination Point
      * Code, either: <ul>
      * <li>any array of the allowed Linksets if the screening type is either: <ul>
      *     <li>SCR_TYPE_INCOMING_LS_TO_DPC
      *     <li>SCR_TYPE_OUTGOING_LS_TO_DPC </ul>
      * or
      * <li>an array of the allowed Originating Signalling Points if the screening type is: <ul>
      *     <li>SCR_TYPE_OPC_TO_DPC </ul> </ul>
      *
      * @oaram destSignallingPoint
      * an ObjectName identifing the destination Signalling Point for which the allowed
      * list is to be returned.
      *
      * @exception TooManyInstancesException
      * if the authorised list is too large for this implementation
      */
    public void setValidList(ObjectName destSignallingPoint, ObjectName[] authorisedList)
        throws TooManyInstancesException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
