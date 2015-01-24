/*
 @(#) src/java/javax/sip/address/TelURL.java <p>
 
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


package javax.sip.address;

import javax.sip.header.*;
import javax.sip.*;

/**
    This class represents Tel URLs, which are used for addressing. The Tel URL starts with the
    scheme tel:. This tells the local entity that what follows is a URL that should be parsed as
    described in RFC2806. After that, the URL contains the phone number of the remote entity. <p>
    Within a SIP Message, TelURLs can be used to indicate the source and intended destination of a
    Request, redirection addresses and the current destination of a Request. All these Headers may
    contain TelURLs. <p> The TelURL interface extends the generic URI interface and provides
    additional convenience methods for the following components of a TelURL address, above the
    generic URI class: <ul> <li>ISDN Subaddress - Phone numbers can also contain subaddresses, which
    are used to identify different remote entities under the same phone number.  <li>Post Dial -
    Phone numbers can also contain a post-dial sequence.  This is what is often used with voice
    mailboxes and other services that are controlled by dialing numbers from your phone keypad while
    the call is in progress.  <li>Global - Phone numbers can be either "global" or "local". Global
    numbers are unambiguous everywhere. Local numbers are usable only within a certain area.
    <li>URL parameters - Parameters affecting a request constructed from this URL. URL parameters
    are added to the end of the URL component and are separated by semi-colons. URL parameters take
    the form: parameter-name "=" parameter-value </ul> See RFC2806 for more information on the use
    of TelURL's.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface TelURL extends URI, Parameters {
    /**
        Returns true if this TelURL is global i.e. if the TelURI has a global phone user.
        @return True if this TelURL represents a global phone user, and false otherwise.
      */
    public boolean isGlobal();
    /**
        Sets phone user of this TelURL to be either global or local.  The default value is false,
        hence the TelURL is defaulted to local.
        @param global The boolean value indicating if the TelURL has a global phone user.
      */
    public void setGlobal(boolean global);
    /**
        Sets post dial of this TelURL. The post-dial sequence describes what and when the local
        entity should send to the phone line.
        @param postDial New value of the postDial parameter.
        @exception java.text.ParseException Thrown when an error is encountered while parsing the
        value.
      */
    public void setPostDial(java.lang.String postDial) throws java.text.ParseException;
    /**
        Returns the value of the postDial parameter, or null if it is not set.
        @return The value of the postDial parameter.
      */
    public java.lang.String getPostDial();
    /**
        Sets phone number of this TelURL. The phone number may either be local or global determined
        by the isGlobal method in this interface. The phoneNumber argument should not contain the
        "+" associated with telephone numbers.
        @param phoneNumber New value of the phoneNumber parameter.
        @exception java.text.ParseException Thrown when an error is encountered while parsing the
        value.
      */
    public void setPhoneNumber(java.lang.String phoneNumber) throws java.text.ParseException;
    /**
        Returns the value of the phoneNumber parameter. This method will not return the "+"
        associated with telephone numbers.
        @return The value of the phoneNumber parameter.
      */
    public java.lang.String getPhoneNumber();
    /**
        Sets ISDN subaddress of this TelURL. If a subaddress is present, it is appended to the phone
        number after ";isub=".
        @param isdnSubAddress New value of the isdnSubAddress parameter.
        @exception java.text.ParseException Thrown when an error is encountered while parsing the
        value.
      */
    public void setIsdnSubAddress(java.lang.String isdnSubAddress) throws java.text.ParseException;
    /**
        Returns the value of the isdnSubAddress parameter, or null if it is not set.
        @return The value of the isdnSubAddress parameter.
      */
    public java.lang.String getIsdnSubAddress();
    /**
        Sets the phone context of this TelURL.
        @param phoneContext New value of the phoneContext parameter.
        @exception java.text.ParseException Thrown when an error is encountered while parsing the
        value.
        @since v1.2
      */
    public void setPhoneContext(java.lang.String phoneContext) throws java.text.ParseException;
    /**
        Returns the value of the phoneContext parameter, or null if it is not set.
        @return The value of the phoneContext parameter.
        @since v1.2
      */
    public java.lang.String getPhoneContext();
    /**
        This method returns the URI as a string.
        @return java.lang.String The stringified version of the URI.
      */
    public java.lang.String toString();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
