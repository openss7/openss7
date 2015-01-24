/*
 @(#) src/java/org/openss7/ss7/sccp/JainSccpStack.java <p>
 
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

package org.openss7.ss7.sccp;

import org.openss7.ss7.*;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required to represent a
  * proprietary JAIN SCCP protocol stack, the implementation of which
  * will be vendor specific.
  * Each vendor's protocol stack will have an object that implements
  * this interface to control the creation/deletion of proprietary
  * <code>JainSccpProviders</code> and the implicit attaching/detaching
  * of those <code>JainSccpProviders</code> to this JainSccpStack
  * implementation. <p>
  *
  * It must be noted that under the <b>JAIN Naming Convention</b> the
  * lower-level package structure and classname of a proprietary
  * implementation of the <i>org.openss7.ss7.sccp.JainSccpStack</i>
  * interface <b>must</b> be
  * <i>org.openss7.ss7.sccp.JainSccpStackImpl</i>. <p>
  *
  * Under the JAIN naming convention, the <b>upper-level package
  * structure</b> (pathname) can be used to differentiate between
  * proprietary implementations from different SS7 Vendors.  The
  * pathname used by each SS7 Vendor <b>must be</b> the domain name
  * assigned to the Vendor in reverse order, e.g. Sun Microsystem's
  * would be 'com.sun' <p>
  *
  * It follows that a proprietary implementation of a JainSccpStack will
  * be located at: <br>
  * <i>&lt;pathname&gt;<b>.</b>org.openss7.ss7.sccp.JainSccpStackImpl</i>
  * <br> <b>Where:</b> <br> <i>pathname</i> = reverse domain name, e.g.
  * 'com.sun' <br> The resulting Peer JAIN SS7 Object would be located
  * at: <i><b>com.sun</b> org.openss7.ss7.sccp.JainSccpStackImpl</i>
  * <br> An application may create a JainSccpStackImpl by invoking the
  * JainSS7Factory.createJainSS7Object() method. The <b>PathName</b> of
  * the vendor specific implementation of which you want to instantiate
  * can be set before calling this method or the default or current
  * pathname may be used. <p>
  *
  * For applications that require some means to identify multiple stacks
  * (with multiple SPCs) setStackName() can be used. An application can
  * choose to supply any identifier to this method. <p>
  *
  * <b>Note</b> that a JainSccpStack represents a single SPC.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  * @see JainSccpProvider
  */
public interface JainSccpStack {

    /**
      *
      * The Signaling Point Code can be implemented in one of the following 4 ways
      *
      * <li>ITU_14bit - Split 3-8-3 across member/cluster/zone.
      * <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone.
      * <li>Chinese_24bit - Split 8-8-8 across member/cluster/zone.
      * <li>Japanese_16bit - Split 5-4-7 across member/cluster/zone.
      *
      * <br>
      * <i>Note to developers:</i> There is no setSignalingPointCode method visible
      * in the JainTcapStack Interface for security reasons. By hiding the
      * setSignalingPointCode method from applications, 3rd party vendors
      * application developers will not be able to configure the signaling Point
      * Code of a specific stack implemetation of the JAIN TCAP API, unless they
      * use the JAIN OAM specification which provides a standard interface into
      * provisioning the TCAP Layer of a SS7 stack. <br>
      * A developer can alternatively implement a setSignalingPointCode method
      * behind the JainTcapStack Interface and allow propreitory JAIN TCAP
      * applications to cast this interface to their implementation in order to
      * provision the Signaling Point Code.
      *
      * @return    the signalingPointCode of this JainTcapStack.
      */
    public SignalingPointCode getSignalingPointCode();

    /**
      * Creates a new Peer (vendor specific)
      * <code>JainSccpProvider</code> whose reference is exposed by this
      * JainSccpStackImpl. This method replaces the
      * createAttachedProvider and createDetachedProvider methods, which
      * have become deprecated in this interface. <p>
      *
      * <i>Note to developers:</i> The implementation of this method
      * should add the newly created <code>JainTcapProvider</code> to
      * the {@link #getProviderList() providerList} once the
      * <code>JainSccpProvider</code> has been successfully created and
      * set the JainSccpStack reference in the newly created
      * <code>JainSccpProvider</code> to <b>this</b> object. <p>
      *
      * <b>Example:-</b> <br>
      * <b>public</b> JainSccpProvider createProvider() <b>throws</b> PeerUnavailableException {
      *      <blockquote><b>try</b> {
      *          <blockquote> JainSccpProviderImpl myProvider = <b>new</b> JainSccpProviderImpl();</blockquote>
      *          } <b>catch</b>(ClassNotFoundException err) {
      *              <blockquote><b>throw new</b> PeerUnavailableException("Couldn't find peer"); </blockquote>
      *          } <br>
      *          myProvider.myStack = <b>this</b>;<br>
      *          <b>return</b> myProvider; </blockquote>
      *  } <p>
      *
      * @return
      * Peer JAIN SCCP Provider referenced by this JainSccpStackImpl.
      *
      * @exception PeerUnavailableException
      * Thrown if the class is not found for a specific peer
      * implementation.
      *
      * @since JAIN TCAP v1.1
      */
    public JainSccpProvider createProvider()
	throws PeerUnavailableException;

    /**
      * Deletes the specified Peer JAIN SCCP Provider attached to this
      * JainSccpStackImpl. <p>
      *
      * <i>Note to developers:</i> The implementation of this method
      * should remove the specified Peer JAIN SCCP Provider from the
      * {@link #getProviderList() getProviderList}.  <p>
      *
      * <b>Example:-</b> <br>
      * <b>public void</b> deleteProvider(JainSccpProvider providerToBeDeleted) <b>throws</b>
      *                   DeleteProviderException { <br>
      *  <blockquote>
      *        if (<b>null</b> == providerToBeDeleted) {
      *
      *        }<br>
      *        JainSccpProvider[] myProviderList = <b>this</b>.getProviderList(); <br>
      *        <b>for</b> (<b>int</b> i=0; i&lt;myProviderList.length; i++) {<br>
      *                <blockquote><b>if</b> (providerToBeDeleted == myProviderList[i]) {
      *                    <blockquote>myProviderList[i] = <b>null</b>;</blockquote>
      *                } <b>else</b> {
      *                      <blockquote><b>throw new</b> DeleteProviderException("Couldn't find provider"); </blockquote>
      *                }</blockquote>
      *        }<br>
      *        <b>return</b>; </blockquote>
      *  }<p>
      *
      * @param providerToBeDeleted
      * The provider to be deleted.
      *
      * @exception DeleteProviderException
      * Thrown if no such provider exists.
      */
    public void deleteProvider(JainSccpProvider providerToBeDeleted)
	throws DeleteProviderException;

    /**
      * Update v1.1: Changed return type of provider List from Vector to
      * Array.
      *
      * Returns the array of Peer JAIN SCCP Providers that have been
      * created by this JainSCCPStackImpl.  All of the Peer JAIN SCCP
      * Providers of this JainSccpStackImpl will be proprietary objects
      * belonging to the same stack vendor.
      *
      * @return
      * A JainSccpProvider Array containing all of the Peer JAIN SCCP
      * Providers created by this JainSccpStackImpl.
      */
    public JainSccpProvider[] getProviderList();

    /**
      * Returns the name of the stack as a string.
      *
      * @return  A string describing the Stack Name.
      */
    public java.lang.String getStackName();

    /**
      * Sets the name of the stack as a string.
      *
      * @param  stackName  The new Stack Name value.
      */
    public void setStackName(java.lang.String stackName);

    /**
      * Returns the Vendor's name for this stack.
      *
      * @return  A string describing the Vendor's name.
      * @since  JAIN TCAP v1.1
      */
    public java.lang.String getVendorName();

    /**
      * Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "com.sun" similar to the path
      * name of the JAIN Factory.
      *
      * @param vendorName The new Vendor's Name.
      * @since JAIN TCAP v1.1
      */
    public void setVendorName(java.lang.String vendorName);

    /**
      * Gets the stack Specification that this Stack is currently
      * supporting. <p>
      *
      * <b>Note to developers</b> :- This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      *
      * @return
      * The current Protocol Version of this stack.  This may be one of
      * the following values: <ul>
      *
      * <li> STACK_SPECIFICATION_ANSI_88
      * <li> STACK_SPECIFICATION_ANSI_92
      * <li> STACK_SPECIFICATION_ANSI_96
      * <li> STACK_SPECIFICATION_ANSI_00
      * <li> STACK_SPECIFICATION_ITU_88
      * <li> STACK_SPECIFICATION_ITU_93
      * <li> STACK_SPECIFICATION_ITU_97
      * <li> STACK_SPECIFICATION_ITU_01 </ul>
      *
      * These values are defined in the SccpConstants class.
      *
      * @since JAIN TCAP v1.1
      *
      * Note: The get/set protocolVersion methods were deprecated to
      * avoid confusion between the stack standards supported and the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      */
    public int getStackSpecification();

    /**
      * Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible. <p>
      *
      * <b>Note to developers</b> :- This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      *
      * @param stackSpecification
      * The stack specification.  This may be one of the following
      * values: <ul>
      *
      * <li> STACK_SPECIFICATION_ANSI_88
      * <li> STACK_SPECIFICATION_ANSI_92
      * <li> STACK_SPECIFICATION_ANSI_96
      * <li> STACK_SPECIFICATION_ANSI_00
      * <li> STACK_SPECIFICATION_ITU_88
      * <li> STACK_SPECIFICATION_ITU_93
      * <li> STACK_SPECIFICATION_ITU_97
      * <li> STACK_SPECIFICATION_ITU_01 </ul>
      *
      * @exception VersionNotSupportedException
      * Thrown if the supplied stack specification cannot be supported
      * by this Stack, or if the Stack cannot change supported stack
      * specifications because it is not in an idle state.
      *
      * @since JAIN TCAP v1.1
      * @see SccpConstants
      */
    public void setStackSpecification(int stackSpecification)
	throws VersionNotSupportedException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
