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

package javax.sip.header;

import javax.sip.*;

/**
    The To header field first and foremost specifies the desired "logical" recipient of the request,
    or the address-of-record of the user or resource that is the target of this request. This may or
    may not be the ultimate recipient of the request. Requests and Responses must contain a
    ToHeader, indicating the desired recipient of the Request.  The UAS or redirect server copies
    the ToHeader into its Response. <p> The To header field MAY contain a SIP or SIPS URI, but it
    may also make use of other URI schemes i.e the telURL, when appropriate. All SIP implementations
    MUST support the SIP URI scheme. Any implementation that supports TLS MUST support the SIPS URI
    scheme.  Like the From header field, it contains a URI and optionally a display name,
    encapsulated in a Address. <p> A UAC may learn how to populate the To header field for a
    particular request in a number of ways. Usually the user will suggest the To header field
    through a human interface, perhaps inputting the URI manually or selecting it from some sort of
    address book. Using the string to form the user part of a SIP URI implies that the User Agent
    wishes the name to be resolved in the domain to the right-hand side (RHS) of the at-sign in the
    SIP URI. Using the string to form the user part of a SIPS URI implies that the User Agent wishes
    to communicate securely, and that the name is to be resolved in the domain to the RHS of the
    at-sign. The RHS will frequently be the home domain of the requestor, which allows for the home
    domain to process the outgoing request. This is useful for features like "speed dial" that
    require interpretation of the user part in the home domain. <p> The telURL may be used when the
    User Agent does not wish to specify the domain that should interpret a telephone number that has
    been input by the user. Rather, each domain through which the request passes would be given that
    opportunity. As an example, a user in an airport might log in and send requests through an
    outbound proxy in the airport. If they enter "411" (this is the phone number for local directory
    assistance in the United States), that needs to be interpreted and processed by the outbound
    proxy in the airport, not the user's home domain. In this case, tel:411 would be the right
    choice. <p> Two To header fields are equivalent if their URIs match, and their parameters match.
    Extension parameters in one header field, not present in the other are ignored for the purposes
    of comparison. This means that the display name and presence or absence of angle brackets do not
    affect matching. <ul> <li> The "Tag" parameter - is used in the To and From header fields of SIP
    messages. It serves as a general mechanism to identify a dialog, which is the combination of the
    Call-ID along with two tags, one from each participant in the dialog. When a UA sends a request
    outside of a dialog, it contains a From tag only, providing "half" of the dialog ID. The dialog
    is completed from the response(s), each of which contributes the second half in the To header
    field. When a tag is generated by a UA for insertion into a request or response, it MUST be
    globally unique and cryptographically random with at least 32 bits of randomness.  Besides the
    requirement for global uniqueness, the algorithm for generating a tag is implementation
    specific. Tags are helpful in fault tolerant systems, where a dialog is to be recovered on an
    alternate server after a failure. A UAS can select the tag in such a way that a backup can
    recognize a request as part of a dialog on the failed server, and therefore determine that it
    should attempt to recover the dialog and any other state associated with it. </ul> A request
    outside of a dialog MUST NOT contain a To tag; the tag in the To field of a request identifies
    the peer of the dialog. Since no dialog is established, no tag is present. <p> For Example: <br>
    <code> To: Carol sip:carol@jcp.org <br> To: Duke sip:duke@jcp.org;tag=287447 </code>
    @see HeaderAddress
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ToHeader extends HeaderAddress, Parameters, Header {
    /**
        Name of the ToHeader.
      */
    static final java.lang.String NAME = "To";
    /**
          Sets the tag parameter of the ToHeader. The tag in the To field
          of a request identifies the peer of the dialog. If no dialog is
          established, no tag is present.

          The To Header MUST contain a new "tag" parameter. When acting
          as a UAC the To "tag" is maintained by the SipProvider from the
          dialog layer, however whan acting as a UAS the To "tag" is
          assigned by the application. That is the tag assignment for
          outbound responses for messages in a dialog is only the
          responsibility of the application for the first outbound
          response. If AUTOMATIC_DIALOG_SUPPORT is set to on (default
          behavior) then, after dialog establishment, the stack will take
          care of the tag assignment. Null is acceptable as a tag value.
          Supplying null for the tag results in a header without a tag.

        Parameters:
                tag - - the new tag of the To Header

        Throws:
                java.text.ParseException - which signals that an error
                has been reached unexpectedly while parsing the Tag
                value.
      */
    void setTag(java.lang.String tag)
        throws java.text.ParseException;
    /**
        Gets tag of ToHeader. The Tag parameter identified the Peer of the dialogue.
        @return The tag parameter of the ToHeader. Returns null if no Tag is present, i.e no
        dialogue is established.
      */
    java.lang.String getTag();
    /**
        Compare this ToHeader for equality with another. This method overrides the equals method in
        javax.sip.Header. This method specifies object equality as outlined by RFC3261. Two To
        header fields are equivalent if their URIs match, and their parameters match. Extension
        parameters in one header field, not present in the other are ignored for the purposes of
        comparison. This means that the display name and presence or absence of angle brackets do
        not affect matching. When comparing header fields, field names are always case-insensitive.
        Unless otherwise stated in the definition of a particular header field, field values,
        parameter names, and parameter values are case-insensitive. Tokens are always
        case-insensitive. Unless specified otherwise, values expressed as quoted strings are
        case-sensitive.
        @param obj The object to compare this ToHeader with.
        @return True if obj is an instance of this class representing the same ToHeader as this,
        false otherwise.
        @since v1.2
      */
    boolean equals(java.lang.Object obj);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
