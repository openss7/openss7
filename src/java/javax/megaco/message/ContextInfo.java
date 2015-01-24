// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/ContextInfo.java <p>
 
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class ContextInfo

java.lang.Object
  |
  +--javax.megaco.message.ContextInfo

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ContextInfo
   extends java.lang.Object

   This class defines the context information block that contains the
   context id sent in the protocol and the context level parameters.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_CTX_NULL
              Identifies the null context. It shall be defined as 0.
   static int M_CTX_CHOOSE
              Identifies the context id to be choose. It shall be defined
   as 0xFFFFFFFE.
   static int M_CTX_ALL
              Identifies the context id to be ALL. It shall be defined as
   0xFFFFFFFF.

   Constructor Summary
   ContextInfo(int contextId)
       throws javax.megaco.InvalidArgumentException

              Constructs a class that specifies the context id and the
   context level parameters.

   Method Summary
    int getContextId()
              Gets the context id and returns an unsigned 32bit value.
    public ContextParam getContextParam()
             Returns the context request when the action handle occurs in
   the command request and returns context properties when the action
   handle occurs in the command response.
    public void setContextParam (ContextParam contextParam)
       throws javax.megaco.InvalidArgumentException

             Sets a context request when the context info occurs in the
   command request and sets context properties when the context info
   occurs in the command response.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_CTX_NULL

public static final int M_CTX_NULL

          Identifies context to be null.
          Its value shall be set to 0.
     _________________________________________________________________

  M_CTX_CHOOSE

public static final int M_CTX_CHOOSE

          Identifies context to be choose.
          Its value shall be set to 0xFFFFFFFE.
     _________________________________________________________________

  M_CTX_ALL

public static final int M_CTX_ALL

          Identifies context to be ALL.
          Its value shall be set to 0xFFFFFFFF.
     _________________________________________________________________

   Constructor Detail

  ContextInfo

public ContextInfo(int contextId)
        throws javax.megaco.InvalidArgumentException

          Constructs a class that shall specify the context id and the
          related prameters.



         1. If the context id is to be given as null, then context id is
            given as M_CTX_NULL.
         2. If the context id is to be given as CHOOSE, then context id
            is given as M_CTX_CHOOSE.
         3. If the context id is to be given as ALL, then context id is
            given as M_CTX_ALL.


   Parameters:
          contextId - An unsigned integer value of 32 bit containing the
          context identifier for the action in which the command is to be
          sent.

   Throws:
          InvalidArgumentException : This exception is raised if the
          value of context Id passed to this method is less than 0.



   Method Detail

  getContextId

public int getContextId()

          This method returns the context id for this context
          information.


        Returns:
                Returns an integer value that identifies the context.
     _________________________________________________________________

  getContextParam

public ContextParam getContextParam()

          This method returns an object reference of context parameter
          that shall specify the context request for a action request and
          context properties for action response.


        Returns:
                Returns an object reference of the context parameter when
                set else shall return a NULL value.
     _________________________________________________________________

  setContextParam

   public void setContextParam(ContextParam contextParam)
       throws javax.megaco.InvalidArgumentException

          This method sets an object reference of context parameter that
          shall specify the context request for a action request and
          context properties for action response.


        Parameters:
                contextParam - An object reference for the context
                parameter.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Context Param passed to this method is
                NULL.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
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
