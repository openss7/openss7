// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: RestartInProgress.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:20 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:20 $ by $Author: brian $
 */

package jain.protocol.ip.mgcp.message;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.message
Class RestartInProgress

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--jain.protocol.ip.mgcp.JainMgcpEvent
              |
              +--jain.protocol.ip.mgcp.JainMgcpCommandEvent
                    |
                    +--jain.protocol.ip.mgcp.message.RestartInProgress

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class RestartInProgress
   extends JainMgcpCommandEvent

   An MGCP RestartInProgress command. Sent by a Gateway to signal that an
   endpoint, or a group of endpoints, are being taken through a restart.
   This command elicits a RestartInProgressResponse event.

   See Also: 
          RestartInProgressResponse, Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source



   Constructor Summary
   RestartInProgress(java.lang.Object source,
   EndpointIdentifier endpointIdentifier, RestartMethod restartMethod)
             Constructs a new RestartInProgress (Command) Event object.



   Method Summary
    ReasonCode getReasonCode()
             Gets the Reason Code.
    int getRestartDelay()
             Gets the Restart Delay.
    RestartMethod getRestartMethod()
             Gets the Restart Method.
    void setReasonCode(ReasonCode reasonCode)
             Sets the Reason Code.
    void setRestartDelay(int restartDelay)
             Sets the Restart Delay.
    void setRestartMethod(RestartMethod restartMethod)
             Sets the Restart Method parameter.
    java.lang.String toString()
             Overrides java.lang.Object.toString().



   Methods inherited from class
   jain.protocol.ip.mgcp.JainMgcpCommandEvent
   BuildCommandHeader, getEndpointIdentifier, setEndpointIdentifier



   Methods inherited from class jain.protocol.ip.mgcp.JainMgcpEvent
   BuildListParmLine, getObjectIdentifier, getTransactionHandle,
   setTransactionHandle



   Methods inherited from class java.util.EventObject
   getSource



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Constructor Detail

  RestartInProgress

public RestartInProgress(java.lang.Object source,
                         EndpointIdentifier endpointIdentifier,
                         RestartMethod restartMethod)
                  throws java.lang.IllegalArgumentException

          Constructs a new RestartInProgress (Command) Event object.

        Parameters:
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.
                restartMethod - Identifies the type of restart being
                done.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                endpoint identifier or the restart method parameter is
                null-valued.

   Method Detail

  getReasonCode

public ReasonCode getReasonCode()

          Gets the Reason Code.

        Returns:
                The Reason Code.
     _________________________________________________________________

  getRestartDelay

public int getRestartDelay()

          Gets the Restart Delay.

        Returns:
                The Restart Delay.
     _________________________________________________________________

  getRestartMethod

public RestartMethod getRestartMethod()

          Gets the Restart Method.

        Returns:
                The Restart Method.
     _________________________________________________________________

  setReasonCode

public void setReasonCode(ReasonCode reasonCode)

          Sets the Reason Code.

        Parameters:
                reasonCode - Optional parameter. The Reason Code.
     _________________________________________________________________

  setRestartDelay

public void setRestartDelay(int restartDelay)
                     throws java.lang.IllegalArgumentException

          Sets the Restart Delay.

        Parameters:
                restartDelay - Optional parameter. The Restart Delay, in
                seconds.

        Throws:
                java.lang.IllegalArgumentException - Thrown if restart
                delay is not in the range 0-999999.
     _________________________________________________________________

  setRestartMethod

public void setRestartMethod(RestartMethod restartMethod)
                      throws java.lang.IllegalArgumentException

          Sets the Restart Method parameter.

        Parameters:
                restartMethod - Identifies the type of restart being
                done.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                restart method parameter is null-valued.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().

        Overrides:
                toString in class java.util.EventObject
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright © 2000 Sun Microsystems, Inc.
*/
