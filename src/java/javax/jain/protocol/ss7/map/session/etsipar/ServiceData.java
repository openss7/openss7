/*
 @(#) $RCSfile: ServiceData.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:42 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:42 $ by $Author: brian $
 */

package javax.jain.protocol.ss7.map.session.etsipar;

import javax.jain.protocol.ss7.map.session.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter represents data exchanged between a Mobile Station
  * and a service application in the network (which is a Jain MAP API
  * user).
  * Only characters from the GSM 7 bit Default Alphabet defined in the
  * specified 3G TS 23.038 can be sent. The Jain MAP API implementation
  * is responsible for the encoding. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h4>Parameter components:</h4><ul>
  *
  * <li>data, mandatory component with no default</ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class ServiceData extends MapParameter {
    /**
      * The constructor.
      * @param data
      * Service data; only characters from the GSM 7 bit Default
      * Alphabet defined in the specified 3G TS 23.038.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public ServiceData(java.lang.String data)
        throws SS7InvalidParamException {
        super();
        setData(data);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public ServiceData()
        throws SS7InvalidParamException {
        super();
    }
    /**
      * Change the service data to send/receive to/from the Mobile
      * Station.
      * @param data
      * Service data; only characters from the GSM 7 bit Default
      * Alphabet defined in the specified 3G TS 23.038.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setData(java.lang.String data)
        throws SS7InvalidParamException {
        // FIXME: check data
        m_data = data;
        m_dataIsSet = true;
    }
    /**
      * Get the service data to send/receive to/from the Mobile Station.
      * @return
      * Service data; only characters from the GSM 7 bit Default
      * Alphabet defined in the specified 3G TS 23.038.
      */
    public java.lang.String getData() {
        return m_data;
    }
    protected java.lang.String m_data = "";
    protected boolean m_dataIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
