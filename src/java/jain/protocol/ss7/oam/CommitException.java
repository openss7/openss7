/*
 @(#) $RCSfile: CommitException.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:24 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:24 $ by $Author: brian $
 */

package jain.protocol.ss7.oam;

import jain.*;
import jain.protocol.ss7.*;

import java.lang.Object;
import java.lang.Throwable;
import java.lang.Exception;

/**
  * This Exception is thrown by a JAIN OAM Managed MBean if the commit() command
  * has been invoked and not all of the operations were successfully comitted.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CommitException extends SS7Exception {
    /**
      * Constructs a new CommitException.
      */
    public CommitException() {
        super();
    }
    /**
      * Adds an entry to the log file of this CommitException.
      * @param success
      * Either: <ul>
      * <li>SUCCESS - the operation successfully completed.
      * <li>FAILED - the operation failed
      * <li>UNKNOWN - could not determine whether the operation was successful or not. </ul>
      * @param mBeanObjectName
      * The ObjectName of the MBean.
      * @param operation
      * The opertion that was to be committed.
      * @exception NullPointerException
      * If any of the Objects supplied as parameters are null.
      */
    public void addEntry(int success, java.lang.String mBeanObjectName, java.lang.String operation)
        throws NullPointerException {
        /*
        if (mBeanObjectName == null || operation == null)
            throw new NullPointerException();
        m_success = success;
        m_mBeanObjectName = mBeanObjectName;
        m_operation = operation;
        */
    }
    /**
      * Returns a log of the operations.
      * @return a log in the format:
      * 
      * <code>
      * 1 : [MBeanObjectName] : [operationRequested] : [SUCCESS | FAILED | UNKNOWN] ; <br>
      * 2 : [MBeanObjectName] : [operationRequested] : [SUCCESS | FAILED | UNKNOWN] ; <br>
      * ..... .... .... .... .... <br>
      * N : [MBeanObjectName] : [operationRequested] : [SUCCESS | FAILED | UNKNOWN] ; <br> <br>
      * Eg. <br>
      * 1 : com.aepona.jain.protocol.ss7.oam.mtp3:type=OBJECT_TYPE_RO UTE,id=23 : setDestinationPointCode : SUCCESS ; <br>
      * 2 : com.aepona.jain.protocol.ss7.oam.mtp3:type=OBJECT_TYPE_RO UTE,id=24 : setPriority : FAILED ; <br>
      * 3 : com.aepona.jain.protocol.ss7.oam.mtp3:type=OBJECT_TYPE_LI NK_SET,id=4 : setDestinationPointCode : UNKNOWN ; <br>
      * 4 : com.aepona.jain.protocol.ss7.oam.mtp3:type=OBJECT_TYPE_LI NK_SET,id=6 : addLink : SUCCESS ; <br>
      * </code>
      */
    public java.lang.String getLog() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
