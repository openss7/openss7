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

package javax.sip.address;

import javax.sip.*;

/**
    This interface represents a user's display name and URI address. The display name of an address
    is optional but if included can be displayed to an end-user. The address URI (most likely a
    SipURI) is the user's address. For example a 'To' address of To: Bob sip:duke@jcp.org would have
    a display name attribute of Bob and an address of sip:duke@jcp.org.
    @version 1.2.2
    @author Monavacon Limited
    @see SipURI
    @see TelURL
  */
public interface Address extends java.lang.Cloneable, java.io.Serializable {
    /**
        Sets the display name of the Address. The display name is an additional user friendly
        personalized text that accompanies the address.
        @param displayName The new string value of the display name.
        @exception java.text.ParseException Thrown to signal that an error has occurred while
        parsing the displayName value.
      */
    public void setDisplayName(java.lang.String displayName) throws java.text.ParseException;
    /**
        Gets the display name of this Address, or null if the attribute is not set.
        @return The display name of this Address.
      */
    public java.lang.String getDisplayName();
    /**
        Sets the URI of this Address. The URI can be either a TelURL or a SipURI.
        @param uri The new URI value of this Address.
      */
    public void setURI(URI uri);
    /**
        Returns the URI of this Address. The type of URI can be determined by the scheme.
        @return URI parmater of the Address object.
      */
    public URI getURI();
    /**
        Returns a string representation of this Address.
        @return The stringified representation of the Address.
      */
    public java.lang.String toString();
    /**
        Indicates whether some other Object is "equal to" this Address.  The actual implementation
        class of a Address object must override the Object.equals method. The new equals method must
        ensure that the implementation of the method is reflexive, symmetric, transitive and for any
        non null value X, X.equals(null) returns false.
        @param obj The Object with which to compare this Address.
        @return True if this Address is "equal to" the object argument and false otherwise.
        @see java.lang.Object
      */
    public boolean equals(java.lang.Object obj);
    /**
        Gets a hash code value for this address. Implementations MUST implement a hashCode method
        that overrides the default hash code method for Objects comparision.
        @return A hash code value.
        @since v1.2
      */
    public int hashCode();
    /**
        This determines if this address is a wildcard address. That is
        ((SipURI)Address.getURI()).getUser() == *;. This method is specific to SIP and SIPS schemes.
        @return True if this address is a wildcard, false otherwise.
      */
    public boolean isWildcard();
    /**
        Clone method. An implementation is expected to override the default Object.clone method and
        return a "deep clone".
        @since v1.2
      */
    public java.lang.Object clone();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
