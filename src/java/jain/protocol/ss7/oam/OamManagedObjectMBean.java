/*
 @(#) src/java/jain/protocol/ss7/oam/OamManagedObjectMBean.java <p>
 
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

package jain.protocol.ss7.oam;

import javax.management.*;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface is implemented by all JAIN OAM Managed Beans (MBeans).
  * NotificationBroadcaster Interface This interface specifies three methods which MBeans
  * acting as notification sources must implement: <ul>
  *
  * <li> <em>getNotificationInfo</em> gives a potential consumer the description of all
  * notifications this source may emit. This method returns an array of
  * MBeanNotificationInfo objects, each of which describes a notification.
  *
  * <li> <em>addNotificationListener</em> registers a consumers interest in notifications
  * sent by this MBean. This method takes a reference to a NotificationListener object, a
  * reference to a NotificationFilter object, and a hand-back object. The same listener
  * object may be registered more than once, each time with a different hand-back object.
  * This means that the handleNotification method of this listener will be invoked several
  * times, with different hand-back objects. The MBean has to maintain a table of
  * listener, filter and hand-back triplets. When the MBean emits a notification, it
  * invokes the handleNotification method of all the registered NotificationListener
  * objects, with their respective hand-back object.  If the consumer has specified a
  * NotificationFilter when registering as a NotificationListener object, the MBean will
  * invoke the filters isNotificationEnabled method first. Only if the filter returns a
  * positive (true) response will the source then call the notification handler.
  *
  * <li> <em>removeNotificationListener</em> allows a consumer to unregister itself from a
  * notification source. This method takes a reference to a NotificationListener object,
  * as well as a hand-back object. If the hand-back object is provided, only the entry
  * corresponding to this listener and hand-back pair will be removed. The same listener
  * object may still be registered with other hand-back objects. Otherwise, if the
  * hand-back is not provided, all entries corresponding to the listener will be removed.
  * </ul>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface OamManagedObjectMBean extends javax.management.NotificationBroadcaster,javax.management.MBeanRegistration, java.lang.Cloneable, java.io.Serializable {
    /** The Object Type for a Mtp2SapMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_MTP2_SAP".  */
    public static final java.lang.String OBJECT_TYPE_MTP2_SAP = "OBJECT_TYPE_MTP2_SAP";
    /** The Object Type for a Mtp2TimerProfileMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_MTP2_TIMER_PROFILE".  */
    public static final java.lang.String OBJECT_TYPE_MTP2_TIMER_PROFILE = "OBJECT_TYPE_MTP2_TIMER_PROFILE";
    /** The Object Type for a LinkMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_LINK".  */
    public static final java.lang.String OBJECT_TYPE_LINK = "OBJECT_TYPE_LINK";
    /** The Object Type for a LinksetMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_LINKSET".  */
    public static final java.lang.String OBJECT_TYPE_LINKSET = "OBJECT_TYPE_LINKSET";
    /** The Object Type for a RouteMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_ROUTE".  */
    public static final java.lang.String OBJECT_TYPE_ROUTE = "OBJECT_TYPE_ROUTE";
    /** The Object Type for a RoutesetMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_ROUTESET".  */
    public static final java.lang.String OBJECT_TYPE_ROUTESET = "OBJECT_TYPE_ROUTESET";
    /** The Object Type for a Mtp3SapMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_MTP3_SAP".  */
    public static final java.lang.String OBJECT_TYPE_MTP3_SAP = "OBJECT_TYPE_MTP3_SAP";
    /** The Object Type for an OwnSignallingPointMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_OWN_SIGNALLING_POINT".  */
    public static final java.lang.String OBJECT_TYPE_OWN_SIGNALLING_POINT = "OBJECT_TYPE_OWN_SIGNALLING_POINT";
    /** The Object Type for a ScreeningTableMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_SCREENING_TABLE".  */
    public static final java.lang.String OBJECT_TYPE_SCREENING_TABLE = "OBJECT_TYPE_SCREENING_TABLE";
    /** The Object Type for a Mtp3TimerProfileMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_MTP3_TIMER_PROFILE".  */
    public static final java.lang.String OBJECT_TYPE_MTP3_TIMER_PROFILE = "OBJECT_TYPE_MTP3_TIMER_PROFILE";
    /** The Object Type for a ConcernedAreaMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_CONCERNED_AREA".  */
    public static final java.lang.String OBJECT_TYPE_CONCERNED_AREA = "OBJECT_TYPE_CONCERNED_AREA";
    /** The Object Type for a GlobalTitleEntryMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_GLOBAL_TITLE_ENTRY".  */
    public static final java.lang.String OBJECT_TYPE_GLOBAL_TITLE_ENTRY = "OBJECT_TYPE_GLOBAL_TITLE_ENTRY";
    /** The Object Type for a SccpEntitySetMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_SCCP_ENTITY_SET".  */
    public static final java.lang.String OBJECT_TYPE_SCCP_ENTITY_SET = "OBJECT_TYPE_SCCP_ENTITY_SET";
    /** The Object Type for a SccpRoutingControlMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_SCCP_ROUTING_CONTROL".  */
    public static final java.lang.String OBJECT_TYPE_SCCP_ROUTING_CONTROL = "OBJECT_TYPE_SCCP_ROUTING_CONTROL";
    /** The Object Type for a SccpTimerProfileMBean, getObjectType() for this MBean
      * returns "OBJECT_TYPE_SCCP_TIMER_PROFILE".  */
    public static final java.lang.String OBJECT_TYPE_SCCP_TIMER_PROFILE = "OBJECT_TYPE_SCCP_TIMER_PROFILE";
    /** The Object Type for a TcapNodeMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_TCAP_NODE".  */
    public static final java.lang.String OBJECT_TYPE_TCAP_NODE = "OBJECT_TYPE_TCAP_NODE";
    /** The Object Type for a OamTimerValMBean, getObjectType() for this MBean returns
      * "OBJECT_TYPE_OAM_TIMER_VAL".  */
    public static final java.lang.String OBJECT_TYPE_OAM_TIMER_VAL = "OBJECT_TYPE_OAM_TIMER_VAL";
    /** The Object Type for a Mtp2LayerManager, getObjectType() for this MBean returns
      * "OBJECT_TYPE_MTP2_LAYER_MANAGER".  */
    public static final java.lang.String OBJECT_TYPE_MTP2_LAYER_MANAGER = "OBJECT_TYPE_MTP2_LAYER_MANAGER";
    /** The Object Type for a Mtp3LayerManager, getObjectType() for this MBean returns
      * "OBJECT_TYPE_MTP3_LAYER_MANAGER".  */
    public static final java.lang.String OBJECT_TYPE_MTP3_LAYER_MANAGER = "OBJECT_TYPE_MTP3_LAYER_MANAGER";
    /** The Object Type for a SccpLayerManager, getObjectType() for this MBean returns
      * "OBJECT_TYPE_SCCP_LAYER_MANAGER".  */
    public static final java.lang.String OBJECT_TYPE_SCCP_LAYER_MANAGER = "OBJECT_TYPE_SCCP_LAYER_MANAGER";
    /** The Object Type for a TcapLayerManager, getObjectType() for this MBean returns
      * "OBJECT_TYPE_TCAP_LAYER_MANAGER".  */
    public static final java.lang.String OBJECT_TYPE_TCAP_LAYER_MANAGER = "OBJECT_TYPE_TCAP_LAYER_MANAGER";
    /**
      * Returns an OamStatisticNotification of the specified statistic type containing the
      * value of the statistic. For example, if the object implementing this interface is
      * an Mtp3ManagedObject then an Mtp3StatisticNotification will be returned. This
      * method will return the statistic synchronously, whereas the
      * startPollingStatistic() method will return the statistic asynchronously as an
      * event at set intervals.
      *
      * @param statisticType
      * the type of the statistic to be returned [.getStatisticType() == statisticType]
      *
      * @return
      * the OamStatisticNotification containing the requested statistic. Depending on the
      * type of this OamManagedObjectMBean, the type of the returned
      * OamStatisticNotification may be: <ul>
      * <li>Mtp2StatisticNotification
      * <li>Mtp3StatisticNotification
      * <li>SccpStatisticNotification
      * <li>TcapStatisticNotification </ul>
      *
      * @exception StatisticNotSupportedException
      * if the specified statistic is not supported (cannot be collected) by the
      * underlying Stack management system.
      *
      * @exception IllegalOperationException
      * if the specifiecd statistic type cannot be collected for this
      * OamManagedObjectMBean
      */
    public OamStatisticNotification getStatistic(Integer statisticType)
        throws StatisticNotSupportedException, IllegalOperationException;
    /**
      * Initiates the polling of statistic of the specified statistic type on this
      * OamManagedObjectMBean. Each time the specified interval expires, an
      * OamStatisticNotification containing the value of the statistic will be sent
      * asynchronously to the registered NotificationListener that started polling the
      * statistic (by invoking this method), providing that the NotificationListener has
      * registered with this OamManagedObjectMBean using an appropriate Filter. <p>
      *
      * Depending on the type of this OamManagedObjectMBean, the type of the
      * OamStatisticNotification that is fired will be one of: <ul>
      * <li>Mtp2StatisticNotification
      * <li>Mtp3StatisticNotification
      * <li>SccpStatisticNotification
      * <li>TcapStatisticNotification </ul>
      *
      * The specified statistic shall be continually sent after the specified interval to
      * the registered NotificationListener until either: <ul>
      *
      * <li>the polling of this statistic is explicitly stopped by the listener invoking
      * the stopPollingStatistic() method, or
      *
      * <li>the NotificationListener is removed from the list of NotificationListeners by
      * invoking the removeNotificationListener() method inherited from
      * javax.management.NotificationBroadcaster </ul>
      *
      * @param statisticType
      * the type of the statistic to be polled [.getStatisticType() == statisticType]
      *
      * @param interval
      * the polling interval in milliseconds
      *
      * @exception StatisticNotSupportedException
      * if the specified statistic is not supported (cannot be collected) by the
      * underlying Stack management system.
      *
      * @exception IllegalOperationException
      * if the specifiecd statistic type cannot be collected for this
      * OamManagedObjectMBean.
      */
    public void startPollingStatistic(Integer statisticType, Integer interval)
        throws StatisticNotSupportedException, IllegalOperationException;
    /**
      * Stops the polling of statistic of the specified statistic type. <p>
      *
      * <dl>
      * <dt>If
      * <dd>the specified statistic is currently being polled by the NotificationListener
      * that invoked this method 
      * <dt>then
      * <dd>this polling will stop and the specified statistic shall no longer be sent to
      * the registered NotificationListener.  
      * <dt>If 
      * <dd>the specified statistic is no longer being polled by the NotificationListener
      * that invoked this method 
      * <dt>then
      * <dd>then invoking this operation will have no effect.
      * </dl>
      *
      * @param statisticType
      * the type of the statistic for which polling is to stop.
      *
      * @exception IllegalOperationException
      * if the statistic is not being polled on this OamManagedObjectMBean or the
      * specified statistic type cannot be collected for this OamManagedObjectMBean.
      */
    public void stopPollingStatistic(Integer statisticType)
        throws IllegalOperationException;
    /**
      * Sets the name of this OamManagedBean. This can be used to store a proprietary
      * Managed Bean (MBean) identifier.
      *
      * @param name
      * the name of this Managed Bean (MBean)
      *
      * @exception NullPointerException
      * if the supplied name is null
      */
    public void setName(java.lang.String name)
        throws NullPointerException;
    /**
      * Gets the name of this OamManagedBean. This may take the form of a proprietary
      * Managed Bean identifier.
      *
      * @return
      * the name of this Managed Bean (MBean)
      */
    public java.lang.String getName();
    /**
      * Gets the ObjectName of this OamManagedObjectBean. An Object Name uniquely
      * identifies an MBean within an MBean server.  Management applications use this
      * object name to identify the MBean on which to perform management operations. An
      * object name is used in the same way as an object reference. Therefore all JAIN OAM
      * methods accept an Object Name to identify an MBean instance rather than an object
      * reference.  The class ObjectName represents an object name which consists of two
      * parts. <ul>
      * <li>Domain Name
      * <li>Key property list </ul>
      *
      * [domainName]:property=value[,property=value]* <p>
      *
      * For all JAIN OAM MBeans, the domain name should be the package name of the implemenation class from which the object was instantiated. <p>
      *
      * e.g. com.aepona.jain.protocol.ss7.oam.sccp The key property list for all JAIN OAM MBeans should consist of the two properties: <ul>
      * <li>type This represents the object type and is returned by getObjectType().  This
      * property takes the form: type=OBJECT_TYPE_SCCP_SAP
      * <li>id This is a unique object id generated by the implementation layer manager in
      * order to unquely identify multiple instances of the same class. An implementation
      * should implicitly set this id when an object is created. The int equivalent will
      * be returned by getObjectId().  This property takes the form: id=uniqueId where
      * 'uniqueId' is a java.lang.String representation of an integer that is unique to all objects
      * of the same class. </ul>
      *
      * Therefore the complete objectName for an instance of
      * com.aepona.jain.protocol.ss7.oam.sccp.SccpSap implementing the JAIN OAM MBean
      * interface com.aepona.jain.protocol.ss7.oam.sccp.SccpSapMBean will be <p>
      *
      * "com.aepona.jain.protocol.ss7.oam.sccp:type=OBJECT_TYPE_SCCP_SAP,id=1234" <p>
      *
      * where 1234 could be any uniquely generated id by the SccpLayerManagerMBean
      * Implemenation.
      *
      * @return
      * the ObjectName of this Managed Bean (MBean).
      */
    public ObjectName getObjectName();
    /**
      * Returns an object id that is unique within all objects of the same class. This is
      * generated by the implementation layer manager in order to uniquely identify
      * multiple instances of the same class. An implementation should implicitly set this
      * id when an object is created. The java.lang.String equivalent of this method is used as the
      * 'id' property in the objectName.
      */
    public java.lang.String getObjectId();
    /**
      * Returns an Object that may contain proprietary methods for this Managed Bean
      * (MBean).
      *
      * @return
      * a proprietary Object containing additional proprietary information about this
      * Managed Bean (MBean).
      */
    public java.lang.Object getProprietaryInformation();
    /**
      * Returns the Object Type of this Managed Bean (MBean) as a java.lang.String. For example, if
      * an object 'mtpsSap_1' implements Mtp2SapMBean, then mtp2Sap_1.getObjectType()
      * should return "OBJECT_TYPE_MTP2_SAP".
      *
      * @return
      * the object type of this Managed Bean (MBean)
      */
    public java.lang.String getObjectType();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
