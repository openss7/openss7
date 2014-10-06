// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ContactHeader.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:53 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:53 $ by $Author: brian $
 */

package javax.sip.header;

import javax.sip.*;

/**
    A Contact header field value provides a URI whose meaning depends on the type of request or
    response it is in. A Contact header field value can contain a display name, a URI with URI
    parameters, and header parameters. <p> The Contact header field provides a SIP or SIPS URI that
    can be used to contact that specific instance of the User Agent for subsequent requests. The
    Contact header field MUST be present and contain exactly one SIP or SIPS URI in any request that
    can result in the establishment of a dialog. For the methods defined in this specification, that
    includes only the INVITE request. For these requests, the scope of the Contact is global. That
    is, the Contact header field value contains the URI at which the User Agent would like to
    receive requests, and this URI MUST be valid even if used in subsequent requests outside of any
    dialogs. <p> If the Request-URI or top Route header field value contains a SIPS URI, the Contact
    header field MUST contain a SIPS URI as well. <p> Messages and Contact Headers <ul>
    <li>Requests: A contact header is mandatory for INVITE's and optional for ACK, OPTIONS and
    REGISTER requests. This allows the callee to send future Requests, such as BYE Requests,
    directly to the caller instead of through a series of proxies.  <li>Informational Responses - A
    contact header is optional in a Informational Response to an INVITE request. It has the same
    semantics in an Informational Response as a Success Response.  <li>Success Responses - A contact
    header is mandatory in response to INVITE's and optional in response to OPTIONS and REGISTER
    requests. An user agent server sending a Success Response to an INIVTE must insert a
    ContactHeader in the Response indicating the SIP address under which it is reachable most
    directly for future SIP Requests.  <li>Redirection Responses - A contact header is optional in
    response to INVITE's, OPTIONS, REGISTER and BYE requests. A proxy may also delete the contact
    header.  <li>Ambiguous Header: - A contact header is optional in response to INVITE, OPTIONS,
    REGISTER and BYE requests. </ul> The ContactHeader defines the Contact parameters "q" and
    "expires".  The q-value value is used to prioritize addresses in a list of contact addresses.
    The expires value suggests an expiration interval that indicates how long the client would like
    a registration to be valid for a specific address. These parameters are only used when the
    Contact is present in a: <ul> <li>REGISTER request <li>REGISTER response <li>3xx response</ul>
    <dt>For Example:<br><code> Contact: "Mr. Watson" sip:watson@worcester.jcp.org; q=0.7;<br>
    expires=3600, "Mr. Watson" mailto:watson@jcp.org.com; q=0.1</code>
    @see HeaderAddress
    @see Parameters
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ContactHeader extends HeaderAddress, Parameters, Header {
    /**
        Name of ContactHeader.
      */
    static final java.lang.String NAME  = "Contact";
    /**
        Sets the value of the expires parameter as delta-seconds. When a client sends a REGISTER
        request, it MAY suggest an expiration interval that indicates how long the client would like
        the registration to be valid for a specific address. There are two ways in which a client
        can suggest an expiration interval for a binding:<ul> <li>through an Expires header field
        <li>an "expires" Contact header parameter.</ul> The latter allows expiration intervals to be
        suggested on a per-binding basis when more than one binding is given in a single REGISTER
        request, whereas the former suggests an expiration interval for all Contact header field
        values that do not contain the "expires" parameter. If neither mechanism for expressing a
        suggested expiration time is present in a REGISTER, the client is indicating its desire for
        the server to choose. <p> A User Agent requests the immediate removal of a binding by
        specifying an expiration interval of "0" for that contact address in a REGISTER request.
        User Agents SHOULD support this mechanism so that bindings can be removed before their
        expiration interval has passed. The REGISTER-specific Contact header field value of "*"
        applies to all registrations, but it MUST NOT be used unless the Expires header field is
        present with a value of "0". The "*" value can be determined if
        "this.getNameAddress().isWildcard() = = true".
        @param expires New relative value of the expires parameter. 0 implies removal of
        Registration specified in Contact Header.
        @exception InvalidArgumentException Thrown when supplied value is less than zero.
      */
    void setExpires(int expires)
        throws InvalidArgumentException;
    /**
        Returns the value of the expires parameter or -1 if no expires parameter was specified or if
        the parameter value cannot be parsed as an int.
        @return Value of the expires parameter measured in delta-seconds, O implies removal of
        Registration specified in Contact Header.
      */
    int getExpires();
    /**
        Sets the qValue value of the Name Address. If more than one Contact is sent in a REGISTER
        request, the registering UA intends to associate all of the URIs in these Contact header
        field values with the address-of-record present in the To field. This list can be
        prioritized with the "q" parameter in the Contact header field. The "q" parameter indicates
        a relative preference for the particular Contact header field value compared to other
        bindings for this address-of-record. A value of -1 indicates the qValue paramater is not
        set.
        @param qValue The new float value of the q-value parameter.
        @exception InvalidArgumentException Thrown when the q-value parameter value is not -1 or
        between 0 and 1.
      */
    void setQValue(float qValue)
        throws InvalidArgumentException;
    /**
        Returns the value of the q-value parameter of this ContactHeader. The q-value parameter
        indicates the relative preference amongst a set of locations. q-values are decimal numbers
        from 0 to 1, with higher values indicating higher preference.
        @return The q-value parameter of this ContactHeader, -1 if the q-value is not set.
      */
    float getQValue();
    /**
        Sets a wildcard on this contact address that is "*" is assigned to the contact header so
        that the header will have the format of Contact: *.
        @since v1.2
      */
    void setWildCard();
    /**
        Returns a boolean value that indicates if the contact header has the format of Contact: *.
        @return True if this is a wildcard address, false otherwise.
        @since v1.2
      */
    boolean isWildCard();
}
