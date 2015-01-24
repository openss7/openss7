/*
 @(#) src/java/org/openss7/jain/protocol/ss7/oam/sccp/SccpRoutingControl.java <p>
 
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

package org.openss7.jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.sccp.*;
import jain.protocol.ss7.oam.mtp3.*;
import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SccpRoutingControl implements SccpRoutingControlMBean {
    public SccpRoutingControl()
        throws MalformedObjectNameException {
        m_objectName = new ObjectName("org.openss7.jain.protocol.ss7.oam.sccp:type=" + getObjectType() + ",id=" + getObjectId());
    }
    /* javax.management.NotificationBroadcaster interface */
    public native void addNotificationListener(NotificationListener listener, NotificationFilter filter, java.lang.Object handback);
    public native MBeanNotificationInfo[] getNotificationInfo();
    public native void removeNotificationListener(NotificationListener listener);
    /* javax.management.MBeanRegistration interface */
    public native void postDeregister();
    public native void postRegister(Boolean registrationDone);
    public native void preDeregister();
    public native ObjectName preRegister(MBeanServer server, ObjectName name);
    /* OamManagedObjectMBean interface */
    public native OamStatisticNotification getStatistic(Integer statisticType)
        throws StatisticNotSupportedException, IllegalOperationException;
    public native void startPollingStatistic(Integer statisticType, Integer interval)
        throws StatisticNotSupportedException, IllegalOperationException;
    public native void stopPollingStatistic(Integer statisticType)
        throws IllegalOperationException;
    public void setName(java.lang.String name)
        throws NullPointerException {
        if (name != null) {
            m_name = name;
            return;
        }
        throw new NullPointerException();
    }
    public java.lang.String getName() {
        return m_name;
    }
    public ObjectName getObjectName() {
        return m_objectName;
    }
    public java.lang.String getObjectId() {
        return new java.lang.String("" + getId() + "");
    }
    public java.lang.Object getProprietaryInformation() {
        return null;
    }
    public java.lang.String getObjectType() {
        return new java.lang.String("OBJECT_TYPE_SCCP_ROUTING_CONTROL");
    }
    /* SccpRoutingControlMBean interface */
    public native void setOwnSignallingPoint(ObjectName ownSignallingPoint);
    public native ObjectName getOwnSignallingPoint();
    public native ObjectName getSccpTimerProfile();
    public native void setSccpTimerProfile(ObjectName timerProfile);
    public native ObjectName[] getGlobalTitleEntries();
    public native ObjectName getGlobalTitleEntry(Integer index);
    public native void setGlobalTitleEntries(ObjectName[] gtEntries)
        throws TooManyInstancesException;
    public native void setGlobalTitleEntry(Integer index, ObjectName gtEntry)
        throws TooManyInstancesException;
    public native Boolean hasGlobalTitle(ObjectName globalTitleEntry);
    public native ObjectName[] getSubsystems();
    public native ObjectName getSubsystem(Integer index);
    public native void setSubsystems(ObjectName[] subsystems)
        throws TooManyInstancesException;
    public native void setSubsystem(Integer index, ObjectName subsystem)
        throws TooManyInstancesException;
    protected native int getId();
    protected java.lang.String m_name = "";
    protected ObjectName m_objectName;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
