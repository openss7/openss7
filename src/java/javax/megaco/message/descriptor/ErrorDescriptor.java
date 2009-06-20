// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class ErrorDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.ErrorDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ErrorDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   error descriptor.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   ErrorDescriptor(int errorCode)
              throws javax.megaco.InvalidArgumentException

              Constructs an Error Descriptor object. This extends the
   Descriptor class. It defines the Error descriptor of the Megaco. This
   contains the error code and optionally contains the error string
   qualifying the error code.

   Method Summary
    int  getProtErrorCode()
              The method is used to get Megaco protocol specified error
   code in error desciptor.
    void  setErrorString(java.lang.String errorStr)
           throws javax.megaco.InvalidArgumentException

              The method is used to set Megaco protocol specified error
   string corresponding to the error code in error desciptor. If error
   string is not set then shall return the null value.
    java.lang.String  getErrorString()
              The method is used to get Megaco protocol specified error
   string corresponding to the error code in error desciptor. This is
   optionally set.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ErrorDescriptor

   public ErrorDescriptor(int errorCode)
        throws javax.megaco.InvalidArgumentException

          Constructs a Error Descriptor. This contains the protocol
          specified error code and optionally the error string qualifying
          the error code.

        Throws:
                InvalidArgumentException - Thrown if an invalid error
                code is set.
     _________________________________________________________________

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type Error
          descriptor. This method overrides the corresponding method of
          the base class Descriptor.


        Returns:
                Returns an integer value that identifies this object as
                the type of Error descriptor. It returns that it is Error
                Descriptor i.e., M_ERROR_DESC.
     _________________________________________________________________

  getProtErrorCode

public final int getProtErrorCode()

          This method cannot be overridden by the derived class. This
          method returns the error code spcified by the Megaco protocol.


        Returns:
                Returns an integer value that identifies the error code.
     _________________________________________________________________

  setErrorString

public final void setErrorString(java.lang.String errorStr)
                     throws javax.megaco.InvalidArgumentException

          This method cannot be overridden by the derived class. This
          method sets the error string corresponding to the error code as
          specified by the Megaco protocol.


        Parameters:
                Returns the string corresponding to the error code.

        Throws:
                InvalidArgumentException - Thrown if an invalid string is
                set.
     _________________________________________________________________

  getErrorString

public final java.lang.String getErrorString()

          This method cannot be overridden by the derived class. This
          method returns the error string corresponding to the error code
          as specified by the Megaco protocol.


        Returns:
                Returns the string corresponding to the error code. If
                the error string is not set then this method would return
                NULL.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright © 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
