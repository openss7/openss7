/*
 @(#) $RCSfile: JainMtpStackImpl.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:36:45 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:36:45 $ by $Author: brian $
 */

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * This class defines the vendor-specific OpenSS7 MTP protocol stack.
  * Each vendor's protocol stack will have an object that implements
  * this interface to control the creation/deletion of proprietary
  * MtpProviders and the implicit attaching and detaching of those
  * MtpProviders to this JainMtpStack implementation. <p>
  *
  * It must be noted that under the <b>JAIN Naming Convention</b>, the
  * <b>lower-level package structure</b> and <b>classname</b> of a
  * proprietary implementation fo the
  * <i>jain.protocol.ss7.mtp.JainMtpStack</i> interface <b>must</b> be
  * <i>jain.protocol.ss7.mtp.JainMtpStackImpl</i>. <p>
  *
  * Under the JAIN naming convention, the <b>Upper-level package
  * structure</b> (pathname) can be used to differentiate between
  * proprietary implementations from different SS7 vendors.  The
  * pathname used by each SS7 Vendor <b>must be</b> the domain name
  * assigned to the vendor in reverse order; e.g. The OpenSS7 Project's
  * would be "org.openss7". <p>
  *
  * It follows that a proprietary implementation of a JainMtpStack will
  * be located at: <br>
  * <i>&lt;pathname&gt;<b>.</b>jain.protocol.ss7.tcap.JainMtpStackImpl</i>
  * <br>
  * <b>Where:</b> <br>
  * <i>pathname</i> is the reverse domain name, e.g. "org.openss7" <br>
  * The resulting Peer JAIN SS7 Object would be located at:
  * <i><b>org.openss7</b>.jain.protocol.ss7.mtp.JainMtpStackImpl</i>
  * <br>
  * An application may create a JainMtpStackImpl by invoking the
  * {@link jain.protocol.ss7.JainSs7Factory#createJainSS7Object} method.
  * The <b>Pathname</b> of the vendor specific implementation of which
  * you want to instantiate of which you want to instantiate can be set
  * before calling this method or the default or current pathname may be
  * used. <p>
  *
  * For applications that require some means to identify multiple stacks
  * (with multiple SPCs) setStackName() can be used.  An application can
  * choose to supply any identifier to this method. <p>
  *
  * <b>Note</b> that a JainMtpStack represents a single SPC.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  * @see JainMtpStack
  * @see JainMtpProviderImpl
  */
public class JainMtpStackImpl implements Runnable, JainMtpStack {
    /** Private network indicator parameter.  */
    private javax.jain.ss7.NetworkIndicator networkIndicator = null;
    /** Private network indicator parameter setting.  */
    private boolean networkIndicatorIsSet = false;
    /** Private signalling point code parameter.  */
    private javax.jain.ss7.SignalingPointCode signalingPointCode = new javax.jain.ss7.SignalingPointCode(0,0,0);
    /** Private signalling point code parameter setting.  */
    private boolean signalingPointCodeIsSet = false;
    /** Private stack specification parameter.  */
    private int stackSpecification = MtpConstants.MTP_PV_ITU_1993;
    /** Private stack specification paramter setting.  */
    private boolean stackSpecificationIsSet = false;
    /** Private stack name parameter.  */
    private java.lang.String stackName = "javax.jain.ss7.mtp.JainMtpProviderImpl";
    /** Private stack name parameter setting.  */
    private boolean stackNameIsSet = false;
    /** Private vendor name parameter.  */
    private java.lang.String vendorName = "org.openss7";
    /** Private vendor name parameter setting.  */
    private boolean vendorNameIsSet = false;
    /** Private provider list.  */
    private java.util.Vector providerList = new java.util.Vector();
    /** Constructs a new JainMtpStackImpl instance.  */
    public JainMtpStackImpl() {
    }
    /** Creates a new Peer (vendor specific) JainMtpProvider whose
      * reference is exposed by this JainMtpStackImpl.
      * @return
      * Peer Jain MTP Provider referenced by this JainMtpStackImpl.
      * @exception javax.jain.PeerUnavailableException
      * Thrown when the class is not found for a specific peer
      * implementation.
      * @exception javax.jain.ParameterNotSetException
      * Thrown wen a mandatory stack parameter is not set.
      */
    public JainMtpProvider createProvider()
        throws javax.jain.PeerUnavailableException,
               javax.jain.ParameterNotSetException {
        if (networkIndicatorIsSet == false)
            throw new javax.jain.ParameterNotSetException("Network Indicator: not set.");
        if (signalingPointCodeIsSet == false)
            throw new javax.jain.ParameterNotSetException("Signaling Point Code: not set.");
        if (stackSpecificationIsSet == false)
            throw new javax.jain.ParameterNotSetException("Stack Specification: not set.");
        if (vendorNameIsSet == false)
            throw new javax.jain.ParameterNotSetException("Vendor Name: not set.");
        JainMtpProvider mtpProvider;
        try {
            Class providerObjectClass = Class.forName(getVendorName() + "." + getStackName());
            java.lang.Object providerObject = providerObjectClass.newInstance();
            mtpProvider = (JainMtpProvider) providerObject;
        } catch (Exception e) {
            throw new javax.jain.PeerUnavailableException("Peer unavailable.");
        }
        providerList.add(mtpProvider);
        return mtpProvider;
    }
    /** Deletes a JainMtpProvider created by and attached to this stack.
      * @param providerToBeDeleted
      * Specifies the provider to be deleted.
      * @exception javax.jain.DeleteProviderException
      * Thrown when the provider to be deleted is not a provider created
      * by this stack, or the provider is busy.  */
    public void deleteProvider(JainMtpProvider providerToBeDeleted)
        throws javax.jain.DeleteProviderException {
        int size = providerList.size();
        providerList.remove(providerToBeDeleted);
        if (size == providerList.size())
            throw new javax.jain.DeleteProviderException();
        mtpProvider.finalize();
    }
    /** Returns an array of JainMtpProviders for this stack.
      * @return
      * An array of JainMtpProvider objects created by and attached to
      * this stack.  */
    public JainMtpProvider[] getProviderList() {
        JainMtpProvider[] providerArray
            = new JainMtpProvider[providerList.size()];
        Iterator i = providerList.iterator();
        for (int n = 0; i.hasNext(); n++)
            providerArray[n] = i.next();
        return providerArray;
    }
    /** Resturns the user's name for this stack.
      * @return
      * A string describing the stack name. */
    public java.lang.String getStackName() {
        return stackName;
    }
    /** Sets the name of the stack as a string.
      * @param stackName
      * The new Stack Name value.  */
    public void setStackName(java.lang.String stackName) {
        this.stackName = stackName;
        this.stackNameIsSet = true;
    }
    /** Returns the Vendor's name for this stack
      * @return
      * A string describing the Vendor's name.  */
    public java.lang.String getVendorName() {
        return vendorName;
    }
    /** Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "org.openss7" similar to the
      * path name of the JAIN Factory.
      * @param vendorName
      * The new Vendor's name.  */
    public void setVendorName(java.lang.String vendorName) {
        this.vendorName = vendorName;
        this.vendorNameIsSet = true;
    }
    /** Gets the stack Specification that this Stack is currently
      * supporting. <p>
      * <b>Note to developers</b>:- This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @return
      * The current Protocol Version of this stack. This may be one of
      * the following values: * <ul>
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_92 STACK_SPECIFICATION_ANSI_92}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_96 STACK_SPECIFICATION_ANSI_96}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_93 STACK_SPECIFICATION_ITU_93}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_97 STACK_SPECIFICATION_ITU_97} </ul>
      * @see MtpConstants */
    public int getStackSpecification() {
        return stackSpecification;
    }
    /** Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible.  <p>
      * <b>Note to developers</b>:- This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @param  stackSpecification
      * One of the following values: * <ul>
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_92 STACK_SPECIFICATION_ANSI_92}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_96 STACK_SPECIFICATION_ANSI_96}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_93 STACK_SPECIFICATION_ITU_93}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_97 STACK_SPECIFICATION_ITU_97} </ul>
      * @exception javax.jain.VersionNotSupportException
      * Thrown when the supplied stack specification cannot be supported
      * by this Stack, or if the Stack cannot change supported stack
      * specifications because it is not in a idle state.
      * @see MtpConstants */
    public void setStackSpecification(int stackSpecification)
        throws javax.jain.VersionNotSupportedException {
        if (providerList.size() == 0) {
            switch (stackSpecification) {
                case MtpConstants.STACK_SPECIFICATION_ANSI_92:
                case MtpConstants.STACK_SPECIFICATION_ANSI_96:
                case MtpConstants.STACK_SPECIFICATION_ITU_93:
                case MtpConstants.STACK_SPECIFICATION_ITU_97:
                    this.stackSpecification = stackSpecification;
                    this.stackSpecificationIsSet = true;
                    return;
            }
            throw new javax.jain.VersionNotSupportedException("Version " + stackSpecification + " out of range.");
        }
        throw new javax.jain.VersionNotSupportedException("Provider already created.");
    }
    /** Sets the signaling point code for the stack.
      * Invoking this method should set the signalling point code
      * associated with the stack.  In the event that the stack is
      * already supporting a signalling point code other than the
      * supplied signalling point code, then the stack will change the
      * supported signalling point code, if possible, or will throw an
      * exception if not possible.
      * @param signalingPointCode
      * The signalling point code to set.
      * @exception javax.jain.protocol.ss7.SS7InvalidParamException
      * Thrown when the supplied signalling point code is invalid, or if
      * the stack cannot change the signalling point code because it is
      * not in the idle state. */
    public void setSignalingPointCode(javax.jain.ss7.SignalingPointCode signalingPointCode)
        throws javax.jain.protocol.ss7.SS7InvalidParamException {
        if (providerList.size() == 0) {
            this.signalingPointCode = signalingPointCode;
            this.signalingPointCodeIsSet = (signalingPointCode != null);
            return;
        }
        throw new javax.jain.protocol.ss7.SS7InvalidParamException("Provider already created.");
    }
    /** Gets the signaling point code associated with the stack.
      * @return
      * The signalling point code associated with the stack.
      * @exception javax.jain.ParameterNotSetException
      * Thrown when the signalling point code has not been set for the
      * stack and there is no default value. */
    public javax.jain.ss7.SignalingPointCode getSignalingPointCode()
        throws javax.jain.ParameterNotSetException {
        if (signalingPointCodeIsSet)
            return signalingPointCode;
        throw new javax.jain.ParameterNotSetException("Signaling Point Code: not set.");
    }
    /** Sets the network indicator for the stack.
      * Invoking this method should set the network indicator associated
      * with the stack.  In the event that the stack is already
      * supporting a network indicator other than the supplied network
      * indicator, then the stack will change the supported network
      * indicator, if possible, or will throw an exception if not
      * possible.
      * @param networkIndicator
      * The network indicator to set.
      * @exception javax.jain.protocol.ss7.SS7InvalidParamException
      * Thrown when the supplied network indicator is invalid, or if the
      * stack cannot change the network indicator because it is not in
      * the idle state.  */
    public void setNetworkIndicator(javax.jain.ss7.NetworkIndicator networkIndicator)
        throws javax.jain.protocol.ss7.SS7InvalidParamException {
        if (provderList.size() == 0) {
            this.networkIndicator = networkIndicator;
            this.networkIndicatorIsSet = (networkIndicator != null);
            return;
        }
        throw new javax.jain.protocol.ss7.SS7InvalidParamException("Provider already created.");
    }
    /** Gets the network indicator associated with the stack.
      * @return
      * The network indicator associated with the stack.
      * @exception javax.jain.ParameterNotSetException
      * Thrown when the network indicator has not been set for the stack
      * and there is no default value. */
    public javax.jain.ss7.NetworkIndicator getNetworkIndicator()
        throws javax.jain.ParameterNotSetException {
        if (networkIndicatorIsSet)
            return networkIndicator;
        throw new javax.jain.ParameterNotSetException("Network Indicator: not set.");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
