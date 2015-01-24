/*
 @(#) src/java/javax/jain/protocol/ss7/map/session/SessionDataReqEvent.java <p>
 
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

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.session.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This primitive (event) is used by the MAP API User to send data to a
  * Mobile Station on a session. <p>
  *
  * The following rules applies for the setting of primitive parameters:
  * <ul>
  *
  * <li>Mandatory parameters with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory parameters with default value need not be set.
  *
  * <li>Optional / conditional parameter is by default not present. Such
  * a parameter becomes present by setting it. </ul>
  *
  * <h4>Primitive parameters:</h4><ul>
  *
  * <li>serviceData, mandatory parameter with no default
  *
  * <li>notify, mandatory parameter with no default
  *
  * <li>alerting, optional parameter </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class SessionDataReqEvent extends SessionEvent {
    /**
      * The constructor.
      * @param source
      * The source of this event. Should be set to the Listner object
      * for the session.
      * @param serviceData
      * The data to be sent.
      * @param notify
      * Flag to indicate if this is a notification to the MS, in which
      * case the MS shall not send data back.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public SessionDataReqEvent(java.lang.Object source, ServiceData serviceData, boolean notify)
        throws SS7InvalidParamException {
        super(source, SESSION_DATA_REQ_ID);
        setServiceData(serviceData);
        setNotify(notify);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public SessionDataReqEvent()
        throws SS7InvalidParamException {
        super(null, SESSION_DATA_REQ_ID);
    }
    /**
      * Change the data to be sent on the session.
      * @param serviceData
      * The data to be sent.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setServiceData(ServiceData serviceData)
        throws SS7InvalidParamException {
        // FIXME: check service data
        m_serviceData = serviceData;
        m_serviceDataIsSet = true;
    }
    /**
      * Get the data to be sent on the session.
      * @return
      * The data to be sent.
      */
    public ServiceData getServiceData() {
        return m_serviceData;
    }
    /**
      * Change the notify flag.
      * @param notify
      * Flag to indicate if this is a notification to the MS, in which
      * case the MS shall not send data back.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setNotify(boolean notify)
        throws SS7InvalidParamException {
        // don't think a boolean can be invalid
        m_notify = notify;
        m_notifyIsSet = true;
    }
    /**
      * Get the notify flag.
      * @return
      * Flag to indicate if this is a notification to the MS, in which
      * case the MS shall not send data back.
      */
    public boolean getNotify() {
        return m_notify;
    }
    /**
      * Set the alerting information to be used to alert the user of the
      * MS.
      * When using this primitive to send data repeatedly on an already
      * established session it might not be necessary to alert the user.
      * @param alerting
      * Parameter indicating level of ugency or type (category) of
      * traffic. Possible values: <ul>
      * <li>ALERTING_LEVEL_0_SILENT
      * <li>ALERTING_LEVEL_1_NORMAL
      * <li>ALERTING_LEVEL_2_URGENT
      * <li>ALERTING_CATEGORY_1
      * <li>ALERTING_CATEGORY_2
      * <li>ALERTING_CATEGORY_3
      * <li>ALERTING_CATEGORY_4
      * <li>ALERTING_CATEGORY_5 </ul>
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setAlerting(int alerting)
        throws SS7InvalidParamException {
        switch (alerting) {
            // XXX: is this alerting level or alerting category???
            case ALERTING_LEVEL_0_SILENT:
            case ALERTING_LEVEL_1_NORMAL:
            case ALERTING_LEVEL_2_URGENT:
                m_alerting = alerting;
                m_alertingIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Alerting Level " + alerting + " out of ragnge.");
        }
    }
    /**
      * Get the alerting information to be used to alert the user of the MS.
      * It shall be checked if this parameter is present before getting it.
      * @return
      * Parameter indicating level of ugency or type (category) of
      * traffic. (See setAlerting for possible values.)
      */
    public int getAlerting() {
        return m_alerting;
    }
    /**
      * Check if the Alerting parameter is present.
      * @return
      * True or false.
      */
    public boolean isAlertingPresent() {
        return m_alertingIsSet;
    }
    protected ServiceData m_serviceData = null;
    protected boolean m_serviceDataIsSet = false;
    protected boolean m_notify = false;
    protected boolean m_notifyIsSet = false;
    protected int m_alerting = 0;
    protected boolean m_alertingIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
