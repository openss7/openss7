/*
 * ContainerService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.container;

import javax.telephony.media.*;
import java.util.Dictionary;

/*
 * Copyright 1999, 2000, 2001 by Dialogic, an Intel Company.
 * 1515 Route 10, Parsippany NJ.
 * All rights reserved.
 *
 * This software is the confidential and proprietary information
 * of Dialogic. ("Confidential Information").  
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with Dialogic. 
 */

/**
 * Methods for manipulating Media Data Objects.
 * <h3>Overview</h3>
 * The Container Management API provides an operating system independent
 * mechanism for the storage and interchange of system data.
 * A Container is an object logically residing in the file system of
 * a CT Server and containing zero or more Data Objects. 
 * Data Objects are made up of data and a set
 * of attributes describing the properties of the data.
 *
 * <h3>Why Use Containers?</h3>
 * The API uses containers, rather than simply specifying pathnames in either
 * the client or server operating systems, so that the server can augment
 * the normal features found in OS file systems with the features required by
 * Resources to manipulate media data in a convenient manner. For example, a
 * .WAV file stored in a particular file within the server may be manipulated
 * more conveniently by using a Container API that treats it as a container
 * containing a  Media Data Object (or possibly a collection of such
 * objects) with appropriate coder, data rate, and other parameters defined
 * for it.
 * <p>
 * In many ways Containers and Data Objects are analogous to file systems and
 * their files. The primary difference is that the Container interface is
 * independent of the actual storage strategies employed. That is, it is
 * independent of the storage medium, and of the data type, formatting and
 * organization.
 * 
 * <h3>What is Stored in Containers?</h3>
 * Examples of data types typically stored in Containers include:
 * <ul> <li> Encoded audio
 * </li><li> Text
 * </li><li> FAX
 * </li><li> Speech Recognition grammars
 * </li></ul>
 */
 
/** Methods for manipulating Containers. 
 * Containers correspond to filesystem directories.
 * A Container may contain other Containers, Media Data Objects (MDOs), 
 * file objects, or other data objects.
 * <p>
 * MDO data objects in a Container are used for playing and recording
 * with the Player.play() and Recorder.record() methods.
 * File data objects are used to hold contexts and grammars for the
 * the ASR interface.
 * <P>
 * Containers are manipulated by reference to their full name,
 * which is a String in the usual heirarchical format as 
 * commonly seen in file pathnames or URLs.
 * That is, strings of the form:
 * <br><tt>//servername/container1/.../containerN/dataobject</tt>
 * <p>
 * The separator characters between containers can be either of
 * "/" or "\" (forward slash or back slash), and those characters
 * can only be used as separators.
 * <p>
 * <b>Note:</b> the "<tt>//servername/</tt>" prefix can be omitted;
 * the MediaProvider uses the <tt>servername</tt> of the host
 * to which the MediaProvider is connected.
 * <p>
 * <b>Note:</b> in the future this format may be extended to include
 * additional "protocol" names in the URL format. 
 * Developers should use the ":" character in names with caution.
 * <p>
 * A ContainerService object is obtained using:
 * <br><tt>((ContainerProvider)MediaProvider).getContainerService()</tt>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public 
interface ContainerService extends ContainerConstants {
    /** Store arbitrary user infomation with a Container or DataObject. 
     * @param fullName the full Container or DataObject path String.
     * @param dict a Dictionary of infomation to be stored on the container.
     * @throws ContainerException if anything goes wrong.
     * @see #getInfo
     */
    void setInfo(String fullName, Dictionary dict)
	throws ContainerException;

    /** Return arbitrary user infomation stored with a Container or DataObject. 
     * @param fullName the full Container or DataObject path String.
     * @param keys a Symbol[] selecting the items to retrieve. 
     * <br>If <tt>keys</tt> is <tt>null</tt>, then retrieve all parameters.
     * @param optargs a Dictionary of implementation-specific information.
     * @return a Dictionary of information from the Container or DataObject.
     * @throws ContainerException if anything goes wrong.
     * @see #setInfo
     */
    Dictionary getInfo(String fullName, Symbol[] keys, Dictionary optargs)
	throws ContainerException;

    /** Set system parameters on a Container or DataObject. 
     * @param fullName the full Container or DataObject path String.
     * @param params a Dictionary of parameters to be set.
     * @throws ContainerException if anything goes wrong.
     * @see #getParameters
     */
    void setParameters(String fullName, Dictionary params)
	throws ContainerException;

    /** Get system parameters on a Container or DataObject. 
     * 
     * @param fullName the full Container or DataObject path String.
     * @param keys a Symbol[] selecting the items to retrieve.
     * <br>If <tt>keys</tt> is <tt>null</tt>, then retrieve all parameters.
     * @param optargs a Dictionary of implementation-specific information.
     * @return a Dictionary of information from the Container or DataObject.
     * @throws ContainerException if anything goes wrong.
     * @see #setParameters
     */
    Dictionary getParameters(String fullName, Symbol[] keys, Dictionary optargs)
	throws ContainerException;

    /** Change the <b>static</b> access rights of the named DataObject. 
     * The static mode of the DataObject restricts the mode
     * available when the DataObject is opened for Read or Write.
     * <tt>v_AccessRead</tt> means the object can be opened for read-only.
     * <tt>v_AccessWrite</tt> means the object can be opend 
     * for read, write, or copy.
     * @param fullName the name of the DataObject.
     * @param accessMode one of the Symbols
     * <tt>v_AccessRead</tt>, <tt>v_AccessWrite</tt>
     * @param optargs a Dictionary of implementation-specific information.
     * @throws ContainerException if anything goes wrong.
     */
    void changeMode(String fullName, Symbol accessMode, Dictionary optargs)
	throws ContainerException;

    /** Copy a DataObject to a new DataObject name.
     * The new DataObject may be in the same or a different Container.
     * The new Container must be on the same server.
     * <p>
     * This operation can fail for numerous reasons, including:
     * <ul><li>
     * if the <tt>fullName</tt> or <tt>target</tt> Strings are badly formed, 
     * </li><li>
     * if the indicated Containers do not exist,
     * </li><li>
     * if the <tt>fullName</tt> DataObject is currently open,
     * </li><li>
     * if there are permission problems writing to the target Container,
     * </li><li>
     * or if the Container filesystem is full.
     * </li></ul>
     * @param fullName the current name of the Container
     * @param target the new name for the Container
     * @param optargs a Dictionary of implementation-specific information.
     * @throws ContainerException if anything goes wrong.
     */
    void copy(String fullName, String target, Dictionary optargs)
	throws ContainerException;

    /** Rename a Container or DataObject within its current Container.
     * <p>
     * <b>Note:</b> the <tt>newName</tt> is not a full path; 
     * it is just the last component of the new name,
     * without any path separator characters.
     * @param fullName the current name of Container/DataObject.
     * @param newName the new name for the Container/DataObject.
     * @param optargs a Dictionary of implementation-specific information.
     * @throws ContainerException if <tt>fullName</tt> does not exist, 
     * or if <tt>newName</tt> already exists.
     */
    void rename(String fullName, String newName, Dictionary optargs)
	throws ContainerException;

    /** Remove the named Container or DataObject.
     * If a Container is named, remove the Container and 
     * all DataObjects in that Container.
     * <P>
     * <b>Note:</b>
     * Destroy can succeed even if the Container is in read-only mode.
     * 
     * @param fullName the full Container or DataObject path String.
     * @param optargs a Dictionary of implementation-specific information.
     * @throws ContainerException if container does not exist.
     */
    void destroy(String fullName, Dictionary optargs)
	throws ContainerException;

    /** List Containers and DataObjects contained in the named Container.
     * <p>
     * The order of objects in the list is not specified. 
     * 
     * @param fullName the full Container path String.
     * @param optargs a Dictionary of implementation-specific information.
     * @return a String[] with an element for each object in the container.
     * @throws ContainerException if container does not exist.
     */
    String[] listObjects(String fullName, Dictionary optargs)
	throws ContainerException;

    /** Create a new Container.
     * <p>
     * <b>Note:</b>to create a new Data Object, use openObject().
     *
     * @param fullName the name of the new Container.
     * @param optargs a Dictionary of implementation-specific information.
     * @throws ContainerException container cannot be created.
     */
    void createContainer(String fullName, Dictionary optargs)
	throws ContainerException;

    /** Create a new Media Data Object and open it.
     * Returns a DataObject that can be read or written.
     *
     * @param fullName the name of the DataObject to create.
     * @param accessMode one of the Symbols
     * <tt>v_AccessRead</tt>, <tt>v_AccessWrite</tt>, <tt>v_AccessCopy</tt>
     * @param optargs a Dictionary of implementation-specific information.
     *
     * @return an open DataObject.
     * @throws ContainerException if the Data Object is not opened.
     */

    DataObject createDataObject(String fullName, 
				Symbol accessMode, 
				Dictionary optargs) 
	throws ContainerException;

    /** Open a Media Data Object.
     * Returns a DataObject that can be read or written.
     * <p>
     * <b>Note:</b> 
     * unlike java.io streams, a DataObject is not differentiated
     * between InputStream and OutputStream.
     * <p>
     * Argument <code>accessMode</code> one of the Symbols:
     * <table border="1" cellpadding="3">
     * <tr><td>Symbol:</td><td>Description:</td></tr>
     * <tr><td><code>v_AccessRead</code></td>
     *     <td>Sets non-exclusive READ access</td></tr>
     * <tr><td><code>v_AccessWrite</code></td>
     *     <td>Sets exclusive WRITE access</td></tr>
     * <tr><td><code>v_AccessCopy</code></td>
     *     <td>Sets WRITE access to the object while other
     * READ accesses are being made. 
     * This access mode causes a new copy of the Object
     * payload to be created. When the handle is closed, 
     * the new data will replace the old data such that 
     * any OPENs with READ access that occurred before this close 
     * will continue to see the old data, 
     * and any OPENs with READ access occurring after the
     * close will see the new data.</td></tr>
     * </table>
     * <p>
     * If the named Data Object does not exist, the result is determined
     * by the value of argument <tt>ifNoExist</tt>:
     * <ul>
     * <li> If <tt>ifNoExist</tt> is <tt>v_Fail</tt>, 
     * <br> then throw a ContainerException(e_NoExists).
     * </li>
     * <li> If <tt>ifNoExist</tt> is <tt>v_Create</tt>, 
     * <br> then create an empty MDO.
     * </li>
     * </ul>
     * <p>
     * If argument <tt>accessMode</tt> is 
     * <tt>v_AccessWrite</tt> or <tt>v_AccessCopy</tt>,
     * then argument <tt>truncate</tt> can be used
     * to erase the contents of MDO when it is opened.
     *
     * @param fullName the name of the DataObject to open.
     * @param accessMode one of the Symbols
     * <tt>v_AccessRead</tt>, <tt>v_AccessWrite</tt>, <tt>v_AccessCopy</tt>
     * @param ifNoExist one of <tt>v_Fail</tt> (false) or <tt>v_Create</tt> (true)
     * @param truncate if <tt>true</tt> and 
     * 			<tt>v_AccessWrite</tt> or <tt>v_AccessCopy</tt>
     * 			then immediately erase previous contents.
     * @param optargs a Dictionary of implementation-specific information.
     * 
     * @return an open DataObject.
     * @throws ContainerException if the Data Object is not opened.
     */
    DataObject openDataObject(String fullName, 
			      Symbol accessMode, 
			      boolean ifNoExist,
			      boolean truncate,
			      Dictionary optargs) 
	throws ContainerException;
}
