/*
 @(#) src/java/javax/sip/header/SubscriptionStateHeader.java <p>
 
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

package javax.sip.header;

import javax.sip.*;

/**
    This interface represents the Subscription State header, as defined by RFC3265, this header is
    not part of RFC3261. <p> NOTIFY requests MUST contain SubscriptionState headers which indicate
    the status of the subscription. The subscription states are: <ul> <li> active - If the
    SubscriptionState header value is "active", it means that the subscription has been accepted and
    (in general) has been authorized. If the header also contains an "expires" parameter, the
    subscriber SHOULD take it as the authoritative subscription duration and adjust accordingly. The
    "retry-after" and "reason" parameters have no semantics for "active".  <li> pending - If the
    SubscriptionState value is "pending", the subscription has been received by the notifier, but
    there is insufficient policy information to grant or deny the subscription yet. If the header
    also contains an "expires" parameter, the subscriber SHOULD take it as the authoritative
    subscription duration and adjust accordingly. No further action is necessary on the part of the
    subscriber. The "retry-after" and "reason" parameters have no semantics for "pending".  <li>
    terminated - If the SubscriptionState value is "terminated", the subscriber should consider the
    subscription terminated. The "expires" parameter has no semantics for "terminated". If a reason
    code is present, the client should behave as described in the reason code defined in this
    Header. If no reason code or an unknown reason code is present, the client MAY attempt to
    re-subscribe at any time (unless a "retry-after" parameter is present, in which case the client
    SHOULD NOT attempt re-subscription until after the number of seconds specified by the
    "retry-after" parameter). </ul>
    @version 1.2.2
    @author Monavacon Limited
  */
public interface SubscriptionStateHeader extends Parameters, Header {
    /**
        Name of SubscriptionStateHeader.
      */
    static final java.lang.String NAME = "Subscription-State";
    /**
        Reason Code: The reason why the subscription was terminated is Unknown.
      */
    static final java.lang.String UNKNOWN = "unknown";
    /**
        Reason Code: The subscription has been terminated, but the subscriber SHOULD retry
        immediately with a new subscription.  One primary use of such a status code is to allow
        migration of subscriptions between nodes. The "retry-after" parameter has no semantics for
        "deactivated".
      */
    static final java.lang.String DEACTIVATED = "deactivated";
    /**
        Reason Code: The subscription has been terminated, but the client SHOULD retry at some later
        time. If a "retry-after" parameter is also present, the client SHOULD wait at least the
        number of seconds specified by that parameter before attempting to re-subscribe.
      */
    static final java.lang.String PROBATION = "probation";
    /**
        Reason Code: The subscription has been terminated due to change in authorization policy.
        Clients SHOULD NOT attempt to re-subscribe. The "retry-after" parameter has no semantics for
        "rejected".
      */
    static final java.lang.String REJECTED = "rejected";
    /**
        Reason Code: The subscription has been terminated because it was not refreshed before it
        expired. Clients MAY re-subscribe immediately. The "retry-after" parameter has no semantics
        for "timeout".
      */
    static final java.lang.String TIMEOUT = "timeout";
    /**
        Reason Code: The subscription has been terminated because the notifier could not obtain
        authorization in a timely fashion. If a "retry-after" parameter is also present, the client
        SHOULD wait at least the number of seconds specified by that parameter before attempting to
        re-subscribe; otherwise, the client MAY retry immediately, but will likely get put back into
        pending state.
      */
    static final java.lang.String GIVE_UP = "give-up";
    /**
        Reason Code: The subscription has been terminated because the resource state which was being
        monitored no longer exists.  Clients SHOULD NOT attempt to re-subscribe. The "retry-after"
        parameter has no semantics for "noresource".
      */
    static final java.lang.String NO_RESOURCE = "no-resource";
    /**
        State: The subscription has been accepted and (in general) has been authorized.
      */
    static final java.lang.String ACTIVE = "active";
    /**
        State: The subscription has been terminated, if a reason code is present, the client should
        behave as described in the reason code.
      */
    static final java.lang.String TERMINATED = "terminated";
    /**
        State: The subscription has been received by the notifier, but there is insufficient policy
        information to grant or deny the subscription yet.
      */
    static final java.lang.String PENDING = "pending";
    /**
        Sets the relative expires value of the SubscriptionStateHeader.  The expires value MUST be
        greater than zero and MUST be less than 2**31.
        @param expires The new expires value of this SubscriptionStateHeader.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero.
      */
    void setExpires(int expires)
        throws InvalidArgumentException;
    /**
        Gets the expires value of the SubscriptionStateHeader. This expires value is relative time.
        @return The expires value of the SubscriptionStateHeader.
      */
    int getExpires();
    /**
        Sets the retry after value of the SubscriptionStateHeader. The retry after value MUST be
        greater than zero and MUST be less than 2**31.
        @param retryAfter The new retry after value of this SubscriptionStateHeader.
        @exception InvalidArgumentException Thrown when the supplied value is less than zero.
      */
    void setRetryAfter(int retryAfter)
        throws InvalidArgumentException;
    /**
        Gets the retry after value of the SubscriptionStateHeader. This retry after value is
        relative time.
        @return The retry after value of the SubscriptionStateHeader.
      */
    int getRetryAfter();
    /**
        Gets the reason code of SubscriptionStateHeader.
        @return The comment of this SubscriptionStateHeader, return null if no reason code is
        available.
      */
    java.lang.String getReasonCode();
    /**
        Sets the reason code value of the SubscriptionStateHeader.
        @param reasonCode The new reason code string value of the SubscriptionStateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the reason
        code.
      */
    void setReasonCode(java.lang.String reasonCode)
        throws java.text.ParseException;
    /**
        Gets the state of SubscriptionStateHeader.
        @return The state of this SubscriptionStateHeader.
      */
    java.lang.String getState();
    /**
        Sets the state value of the SubscriptionStateHeader.
        @param state The new state string value of the SubscriptionStateHeader.
        @exception java.text.ParseException Thrown when an error was found while parsing the state.
      */
    void setState(java.lang.String state)
        throws java.text.ParseException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
