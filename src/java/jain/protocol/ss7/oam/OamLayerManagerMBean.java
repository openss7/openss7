/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package jain.protocol.ss7.oam;

import javax.management.*;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods common to all Protocol Layer Managers. It defines the methods to: <ul>
  *
  * <li>Commit all operations on any OamManagedObjectMBeans in this layer.
  *
  * <li>Get the current configuration of the SS7 Stack being managed.  Invoking this
  * method will create instances of Managed MBeans for each physical element in the system
  * and create the necessary associations between them.
  *
  * <li>Dynamically configure the Protocol Variant and Year supported by this Protocol
  * Layer Manager (if allowed by the implementation). </ul>
  *
  * When creating a MBean an entry must be added to the LookupTable of that MBean's
  * ObjectName and reference, and when a MBean is being deleted the corresponding entry in
  * the LookupTable must be removed.  This is to enable the application to operate
  * successfully without a JMX agent. <p>
  *
  * <center> [Instantiation.jpg] </center><br>
  * <center> This illustrates how JAIN OAM MBeans are instantiated </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface OamLayerManagerMBean {
    /** A Protocol Version constant: ANSI Protocol */
    public static final int PROTOCOL_VARIANT_ANSI_1992 = 1;
    public static final int PROTOCOL_VARIANT_ANSI_1996 = 2;
    /** A Protocol Version constant: ITU Protocol */
    public static final int PROTOCOL_VARIANT_ITU_1993 = 3;
    public static final int PROTOCOL_VARIANT_ITU_1996 = 4;
    public static final int PROTOCOL_VARIANT_ITU_1997 = 5;
    /** A Protocol Version constant: a protocol other then ITU or ANSI */
    public static final int PROTOCOL_VARIANT_OTHER = 6;
    /**
      * Registers the specified NotificationListener with all of the
      * OamManagedObjectMBeans of this Layer Manager. The specified listener will be
      * registered as a NotificationListener with all of the OamManagedObjectMBeans of
      * this Layer Manager using the specified handback and filter for all registrations.
      * Should an MBean be created after this method is called, the listener will not
      * automatically be added to the newly created MBean and this method or
      * addNotificationListener must be called again.
      *
      * @param listener
      * the NotificationListener to be registered with all of the MBeans of this Layer
      * Manager. ie - all OamManagedObjects returned by getAllOamManagedObjects()
      *
      * @param filter
      * The filter object. If not specified, no filtering will be performed before
      * handling notifications.
      *
      * @param handback
      * The context to be sent to the listener when a notification is emitted.
      *
      * @exception NullPointerException
      * if the supplied listener is null
      */
    public void addNotificationListenerToAllMBeans(NotificationListener listener,
            NotificationFilter filter, java.lang.Object handback)
        throws NullPointerException;
    /**
      * Removes the specified NotificationListener from listening to all of the
      * OamManagedObjectMBeans of this Layer Manager. This will have the same effect as
      * calling the removeNotificationListener() method on all OamManagedObjectMBeans
      * returned by getAllOamManagedObjects()
      *
      * @param listener
      * the NotificationListener to be removed
      *
      * @exception NullPointerException
      * if the supplied Listener is null
      */
    public void removeNotificationListenerFromAllMBeans(NotificationListener listener)
        throws NullPointerException;
    /**
      * Returns an array containing the ObjectNames all of the OamManagedObjectMBeans
      * managed by this Layer Manager. E.g. For an Mtp3LayerManagerMbean the returned
      * array will contain the ObjectNames of all of the Mtp3ManagedObjectMBeans that were
      * either: <ul>
      * <li>explicitly created through the create() methods defined on the Mtp3LayerManager, or
      * <li>implicitly created through the readCurrentConfiguration() method </ul>
      */
    public ObjectName[] getAllOamManagedObjects();
    /**
      * Resets the MBeans back to the state they were in after the last call to the
      * commit() or readCurrentConfiguration(). Or in the case of neither of these methods
      * being called then the MBeans are reset to this original state.
      */
    public void rollBack();
    /**
      * Commit all operations on any Managed MBeans in this layer. The implementation of
      * this method shall commit all operations to the underlying proprietary management
      * system. <p>
      *
      * Successfully invoking the 'commit()' command shall have three separate
      * consequences: <ol>
      *
      * <li>When an application requests the creation of a MBean from this Layer Maneger,
      * the Layer Manager should create an instance of the appropriate MBean. Only when
      * this commit() method has been invoked should the corresponding element be
      * committed to the underlying proprietary management system.
      *
      * <li>When an attribute xxxx of an MBean is modified using setXxxx(), the new
      * attribute value will only be returned from getXxxx() and the changes will only be
      * propagated to the underlying proprietary management system when this commit()
      * method has been invoked.
      *
      * <li>When an application requests the deletion of a MBean from a Layer Manager,
      * that MBean should be tagged 'for deletion', but the corresponding element should
      * not be deleted from the underlying proprietary management system until this
      * commit() method has been invoked. </ol>
      *
      * @exception CommitException
      * if any of the operations to be committed were not committed successfully. This
      * CommitException shall list the operations that were to be committed and will
      * indicate whether or not each operation was successfully committed.
      */
    public void commit()
        throws CommitException;
    /**
      * Gets the current configuration of the SS7 Layer being managed.  Invoking this
      * method will create instances of Managed MBeans for each physical element in the
      * system and create the necessary associations between them
      *
      * @exception ReadConfigurationException
      * if an error is encountered while attempting to establish the current system
      * configuration or while creating the Managed MBeans that represent the current
      * system configuration.
      */
    public void readCurrentConfiguration()
        throws ReadConfigurationException;
    /**
      * Returns a java.lang.String that identifies the Vendor that supplied this implementation.
      * @return
      * a string identifying the Vendor implementation
      */
    public java.lang.String getVendorDetails();
    /**
      * Sets the protocol variant and year of the SS7 Protocol Layer represented by this
      * OamLayerManager. If the underlying SS7 Stack has the the capability of being
      * configured dynamically as to what variant and year of the protocol is it
      * supporting then this method will dynamically configure the stack.
      *
      * @param variantAndYear
      * the Protocol Variant and Year that this Protocol Layer shall be configured to.
      * Can be one of the following: <ul>
      * <li>PROTOCOL_VARIANT_ANSI_1993
      * <li>PROTOCOL_VARIANT_ANSI_1997
      * <li>PROTOCOL_VARIANT_ITU_1992
      * <li>PROTOCOL_VARIANT_ITU_1996
      * <li>PROTOCOL_VARIANT_OTHER </ul>
      *
      * @exception UnsupportedOperationException
      * if the implementation does not support the configuration of the protocol variant
      * and year (i.e. Protocol variant and year are read-only values).
      *
      * @exception IllegalArgumentException
      * if the suplied protocol variant and year are in an incorrect format
      */
    public void setProtocolVariantAndYear(Integer variantAndYear)
        throws UnsupportedOperationException, IllegalArgumentException;
    /**
      * Returns the Protocol variant and year.
      *
      * @return
      * one of the following: <ul>
      * <li>PROTOCOL_VARIANT_ANSI_1993
      * <li>PROTOCOL_VARIANT_ANSI_1997
      * <li>PROTOCOL_VARIANT_ITU_1992
      * <li>PROTOCOL_VARIANT_ITU_1996
      * <li>PROTOCOL_VARIANT_OTHER </ul>
      */
    public Integer getProtocolVariantAndYear();
    /**
      * Sets the Name of this Stack. Could be any proprietary format.  Setting the stack
      * name allows the implemention to distinguish between different virtual stacks. The
      * layer managers can be combined into one virtual stack by setting the same stack
      * name for each layer.
      */
    public void setStackName(java.lang.String stackName);
    /**
      * Returns the Stack Name.
      * @return
      * the stack name that was set by the setStackName method.
      */
    public java.lang.String getStackName();
    /** Returns a reference to the OamLookupTable.  */
    public OamLookupTable getLookupTable();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
