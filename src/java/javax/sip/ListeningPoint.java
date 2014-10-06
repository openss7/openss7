/*
 @(#) $RCSfile: ListeningPoint.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:51 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:51 $ by $Author: brian $
 */

package javax.sip;

/**
    This interface represents a unique IP network listening point, which consists of port transport
    and IP. A ListeningPoint is a Java representation of the socket that a SipProvider messaging
    entity uses to send and receive messages. <p> The ListeningPoint also includes an optional
    sent-by string parameter.  If set, this string will be placed in the sent-by parameter of the
    top most Via header of outgoing requests.
    @version 1.2.2
    @author Monavacon Limited
  */
public interface ListeningPoint extends java.lang.Cloneable, java.io.Serializable {
    /**
        Transport constant: TCP.
      */
    static final java.lang.String TCP = "tcp";
    /**
        Transport constant: UDP.
      */
    static final java.lang.String UDP = "udp";
    /**
        Transport constant: SCTP.
      */
    static final java.lang.String SCTP = "sctp";
    /**
        Transport constant: TLS over TCP.
      */
    static final java.lang.String TLS = "tls";
    /**
        Port Constant: Default port 5060. This constant should only be used when the transport of
        the ListeningPoint is set to UDP, TCP or SCTP.
      */
    static final int PORT_5060 = 5060;
    /**
        Port Constant: Default port 5061. This constant should only be used when the transport of
        the Listening Point is set to TLS over TCP or TCP assuming the scheme is "sips".
      */
    static final int PORT_5061 = 5061;
    /**
        Gets the port of the ListeningPoint. The default port of a ListeningPoint is dependent on
        the scheme and transport. For example: <ul> <li> The default port is 5060, if the transport
        UDP and the scheme is sip:.  <li> The default port is 5060, if the transport is TCP and the
        scheme is sip:.  <li> The default port is 5060, if the transport is SCTP and the scheme is
        sip:.  <li> The default port is 5061, if the transport is TLS over TCP and the scheme is
        sip:.  <li> The default port is 5061, if the transport is TCP and the scheme is sips:. </ul>
        @return The integer value of the port.
      */
    int getPort();
    /**
        Gets the transport of the ListeningPoint.
        @return
                the string value of the transport.
      */
    java.lang.String getTransport();
    /**
        Gets the IP of the ListeningPoint.
        @return The string value of the IP address.
        @since v1.2
      */
    java.lang.String getIPAddress();
    /**
        Sets the sentBy string for this ListeningPoint. The sentBy java.lang.String is placed in the top most
        Via header of outgoing requests. This parameter is optional and if it is not set, the top
        most Via header will use the IP address and port assigned to the listening point for the
        sentBy field.
        @param sentBy  The sentBy string to be set in requests top most Via headers.
        @exception java.text.ParseException Thrown when an error was found while parsing the sentBy
        value.
        @since v1.2
      */
    void setSentBy(java.lang.String sentBy) throws java.text.ParseException;
    /**
        Gets the sentBy attribute of the ListeningPoint.
        @return the string value of the sentBy attribute.
        @since v1.2
      */
    java.lang.String getSentBy();
    /**
        This method indicates whether the specified object is equal to this Listening Point. The
        specified object is equal to this ListeningPoint if the specified object is a ListeningPoint
        and the transport and port in the specified Listening Point is the same as this Listening
        Point.
        @param obj  The object with which to compare this ListeningPoint.
        @return True if this ListeningPoint is "equal to" the obj argument; false otherwise.
      */
    boolean equals(java.lang.Object obj);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
