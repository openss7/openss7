/*
 * Async_ContainerService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media.container.async;

import javax.telephony.media.*;
import javax.telephony.media.container.*;
import java.util.EventListener;
import java.util.Dictionary;

/** 
 * Non-blocking methods for manipulating Containers. 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_ContainerService extends ContainerService {

    /** Store arbitrary user infomation with a Container or DataObject.
     * @see ContainerService#setInfo setInfo
     */
    Async_ContainerEvent async_setInfo(String fullName, Dictionary dict);

    /** Return arbitrary user infomation stored with a Container or DataObject. 
     * @see ContainerService#getInfo getInfo
     */
    Async_ContainerEvent async_getInfo(String fullName, Symbol[] keys, Dictionary optargs);

    /** set system parameters on a Container or DataObject.
     * @see ContainerService#setParameters setParameters
     */
    Async_ContainerEvent async_setParameters(String fullName, Dictionary params) ;

    /** get system parameters on a Container or DataObject.
     * @see ContainerService#getParameters getParameters
     */
    Async_ContainerEvent async_getParameters(String fullName, Symbol[] keys, Dictionary optargs) ;

    /** change the access rights of a container. 
     * @param the name of the DataObject to open.
     * @param accessMode one of the Symbols
     * <tt>v_AccessRead</tt>, <tt>v_AccessWrite</tt>
    
     * @see ContainerService#changeMode changeMode
     */
    Async_ContainerEvent async_changeMode(String fullName, Symbol accessMode, Dictionary optargs);

    /** Copy one DataObject to new DataObject name.
     * @see ContainerService#copy copy
     */
    Async_ContainerEvent async_copy(String fullName, 
				    String target, 
				    Dictionary optargs);

    /** Rename a Container or DataObject within its current Container.
     * <p>
     * <b>Note:</b> the <tt>newName</tt> is not a full path; 
     * it is just the last component of the new name, without any path separator characters.
     * @param fullName the current name of Container/DataObject.
     * @param newName the new name for the Container/DataObject.
     * @see ContainerService#rename rename
     */
    Async_ContainerEvent async_rename(String fullName, String newName, Dictionary optargs);

    /** remove this container and all data objects.
     * <P>
     * <b>Note:</b>
     * Destroy can succeed even if this container is open in read-only mode.
     * @see ContainerService#destroy destroy
     */
    Async_ContainerEvent async_destroy(String fullName, Dictionary optargs);

    /** list inner objects and containers. 
     * @see ContainerService#listObjects listObjects
     */
    Async_ContainerEvent async_listObjects(String fullName, Dictionary optargs);

    /** Create a new Container.
     * <p>
     * <b>Note:</b>to create a new Data Object, use async_openObject().
     *
     * @param the name of the new Container.
     * @see ContainerService#createContainer createContainer
     */
    Async_ContainerEvent async_createContainer(String fullName, Dictionary optargs);

    /**
     * Create a new Data Object and open it.
     * @see ContainerService#createDataObject createDataObject
     */
    Async_ContainerEvent async_createDataObject(String fullName, 
						Symbol accessMode, 
						Dictionary optargs);

    /**
     * Open a Data Object.
     * @see ContainerService#openDataObject openDataObject
     */
    Async_ContainerEvent async_openDataObject(String fullName, 
					      Symbol accessMode, 
					      boolean ifNoExist,
					      boolean truncate,
					      Dictionary optargs);

    /** Add listener for Container event completions. 
     * @param listener an Async_ContainerListener
     */
    void addListener(EventListener listener);

    /** Remove listener for Container event completions.
     * @param listener an Async_ContainerListener
     */
    void removeListener(EventListener listener);
}
