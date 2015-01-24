/*
 @(#) src/java/jain/protocol/ss7/oam/TooManyInstancesException.java <p>
 
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

package jain.protocol.ss7.oam;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This Exception is thrown whenever an attempt is made to set or create more elements
  * than a particular implementation permits. <p>
  *
  * For example, invoking someMBean.setElement(int index, ElementType element); with an
  * index that is greater than the permitted number of elements, or
  * someMBean.setElement(ElementType[] elements); where elements is an array with a size
  * greater than the allowed size. <p>
  *
  * Similarly, all create() methods will throw this Exception if an attempt is made to
  * create more instances of a particular Managed MBean than the implementation allows.
  * <p>
  *
  * The Object Type will identify the Managed MBean that has caused this Exception to be
  * thrown.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class TooManyInstancesException extends SS7Exception {
    /**
      * Constructs a new TooManyInstancesException identifying the OAM Managed MBean type
      * that caused the Exception to be thrown
      *
      * @param objectType
      * the Object Type of the managed object
      */
    public TooManyInstancesException(java.lang.String objectType) {
        super();
        m_objectType = objectType;
    }
    /**
      * Constructs a new TooManyInstancesException with the specified detail message and
      * the object Type of the managed object that caused the exception
      *
      * @param msg
      * the message detail of this Exception.
      *
      * @param objectType
      * the Object Type of the managed object
      */
    public TooManyInstancesException(java.lang.String msg, java.lang.String objectType) {
        super(msg);
        m_objectType = objectType;
    }
    /**
      * Returns an Type that identifies the Managed MBean type of which there are too many
      * instances
      *
      * @return
      * the Object Type of the managed object MBean
      */
    public java.lang.String getObjectType() {
        return m_objectType;
    }
    protected java.lang.String m_objectType;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
