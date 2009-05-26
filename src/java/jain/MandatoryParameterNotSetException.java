/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  AePONA Limited, Interpoint Building
 *  20-24 York Street, Belfast BT15 1AQ
 *  N. Ireland.
 *
 *
 *  Module Name   : JAIN TCAP API
 *  File Name     : MandatoryParameterNotSetException.java
 *  Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     16/10/2000  Phelim O'Doherty    Updated after public release and
 *                                          certification process comments.
 *  1.1     11/07/01    Eugene Bell         Updated for JAIN Exception strategy
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain;

/**
* This Exception is thrown as a result of the following two situations:
* <OL>
*   <LI>
*   if all of the mandatory parameters required by an
*   implementation of a {@Link jain.protocol.ss7.tcap.JainTcapProvider} to send either a
*   {@Link jain.protocol.ss7.tcap.ComponentReqEvent} or a
*   {@Link jain.protocol.ss7.tcap.DialogueReqEvent} have not been set <B>or</B>
*   <LI>if an attempt is made to access a
*   <a href="protocol/ss7/tcap/package-summary.html">JAIN TCAP Mandatory</a>
*   parameter that has not been set within either a
*   {@Link jain.protocol.ss7.tcap.ComponentReqEvent}
*   or a {@Link jain.protocol.ss7.tcap.DialogueReqEvent}.
* </OL>
* The distinction between the two situations in which the exception is thrown is that
* the JainTcapProvider will throw the exception if all of the mandatory parameters for
* the underlying variant are not set (the JAIN TCAP Mandatory parameters <b>plus</b>
* additional parameters that  are mandatory for the underlying variant only), whereas the
* accessor methods on the Events will throw the exception for JAIN TCAP Mandatory parameters
* <B>only</B>.
* <BR>
*
* @version 1.1
* @author Sun Microsystems Inc.
*
*/
public class MandatoryParameterNotSetException extends ParameterNotSetException implements java.io.Serializable {

    /**
    * Constructs a new <code>MandatoryParameterNotSetException</code> with
    * the specified detail message.
    * <P>
    * @param <var>msg</var> the detail message. <B>NB:</B>
    * It is recommended that the detail message returned in the
    * <CODE>MandatoryParameterNotSetException</CODE> should be a <CODE>String</CODE> of the form:
    * <P>
    * <CENTER><B>"Parameter:<parameterName> not set"</B></CENTER>
    */
    public MandatoryParameterNotSetException(String msg) {
      super(msg);
    }

    /**
    * Constructs a new <code>MandatoryParameterNotSetException</code>
    */
    public MandatoryParameterNotSetException() {
      super();
    }
}

