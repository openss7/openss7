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

package javax.sip;

import javax.sip.address.*;
import javax.sip.header.*;
import javax.sip.message.*;

/**
    This class contains the enumerations that define the underlying state of an existing dialog.
    There are three explicit states for a dialog, namely: <ul> <li>Early - A dialog is in the
    "early" state, which occurs when it is created when a provisional response is recieved to the
    INVITE Request.  <li>Confirmed - A dialog transitions to the "confirmed" state when a 2xx final
    response is received to the INVITE Request.  <li>Terminated - A dialog transitions to the
    "terminated" state for all other reasons or if no response arrives at all on the dialog.  </ul>
    @version 1.2.2
    @author Monavacon Limited
  */
public final class DialogState implements java.io.Serializable {
    /**
        This constant value indicates the internal value of the "Early" constant.  This constant has
        an integer value of 0.
      */
    public static final int _EARLY = 0;
    /**
        This constant value indicates the internal value of the "Confirmed" constant.  This constant
        has an integer value of 1.
      */
    public static final int _CONFIRMED = 1;
    /**
        @deprecated Since v1.2. This state does not exist in a dialog.  This constant value
        indicates the internal value of the "Completed" constant.  This constant has an integer
        value of 2.
      */
    public static final int _COMPLETED = 2;
    /**
        This constant value indicates the internal value of the "Terminated" constant.  This
        constant has an integer value of 3.
      */
    public static final int _TERMINATED = 3;
    /**
        Private internal integer value of the constant class.
      */
    private int m_value;
    /**
        Private constructor for the constant class.
      */
    private DialogState(int value) {
        m_value = value;
    }
    /**
        This method returns the object value of the DialogState
        @param dialogState The integer value of the DialogState.
        @return The DialogState Object.
      */
    public static DialogState getObject(int dialogState) {
        return new DialogState(dialogState);
    }
    /**
        This method returns the integer value of the DialogState
        @return The integer value of the DialogState.
      */
    public int getValue() {
        return m_value;
    }
    /**
        Compare this dialog state for equality with another.
        @param obj The object to compare this with.
        @return True if obj is an instance of this class representing the same dialog state as this,
        false otherwise.
        @since 1.2
      */
    public boolean equals(java.lang.Object obj) {
        if (obj == null)
            return false;
        if (obj instanceof DialogState) {
            DialogState other = (DialogState) obj;
            if (other.m_value == m_value)
                return true;
        }
        return false;
    }
    /**
        Get a hash code value for this dialog state.
        @return A hash code value.
        @since 1.2
     */
    public int hashCode() {
        return m_value;
    }
    /**
        This method returns a string version of this class.
        @return The string version of the DialogState
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jsip.DialogState");
        b.append("\n\tm_value = " + m_value);
        return b.toString();
    }
    /**
        This constant value indicates that the dialog state is "Early".
      */
    public static final DialogState EARLY
        = new DialogState(_EARLY);
    /**
        This constant value indicates that the dialog state is "Confirmed".
      */
    public static final DialogState CONFIRMED
        = new DialogState(_CONFIRMED);
    /**
        @deprecated Since v1.2. This state does not exist in a dialog.  This constant value
        indicates that the dialog state is "Completed".
      */
    public static final DialogState COMPLETED
        = new DialogState(_COMPLETED);
    /**
        This constant value indicates that the dialog state is "Terminated".
      */
    public static final DialogState TERMINATED
        = new DialogState(_TERMINATED);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
