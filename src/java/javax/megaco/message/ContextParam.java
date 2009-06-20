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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class ContextParam

java.lang.Object
  |
  +--javax.megaco.message.ContextParam

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ContextParam
   extends java.lang.Object

   The Context Parameter object is a class that shall be used to set the
   context level parameters i.e., topology descriptor, priority, and
   whether emergency is active or not. This is an independent class
   derived from java.util.Object and shall not have any derived classes

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   ContextParam()
              Constructs a class that specifies the context level
   parameters.

   Method Summary
    boolean isEMPresent()
             The method can be used to check whether emergency is present
   as a part of the context level parameters.
    TopologyDescriptor [] getTopologyDescriptor()
             Gets the object refernces of all the topology descriptors
   specified for this context.

                                                                  boolean

   isPriorityPresent()
             The method can be used to check whether priority is present
   as a part of the context level parameters.

                                                                      int

   getPriority()
             The method can be used to get the prority value specified
   for this context.

                                                                     void

   setEM(boolean isEMPresent)
             The method can be used to set if the emegency is present as
   a part of the context level parameters.

                                                                     void

   setPriority(int priority)
        throws javax.megaco.InvalidArgumentException

             The method can be used to set the priority value specified
   for this context.
    public void setTopologyDescriptor(TopologyDescriptor
   topologyDescriptorInfo[])
       throws javax.megaco.InvalidArgumentException

             Sets the vector of Topology Descriptor Information for this
   context.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Constructor Detail

  ContextParam

public ContextParam()

          Constructs a class that shall specify the context level
          parameters.

   Method Detail

  isEMPresent

public boolean isEMPresent()

          The method can be used to check if emergency is present as a
          part of the context level parameters. By default, if the
          setEM() method has not been invoked, then this method should
          return FALSE to indicate that Emeregency is not present.


        Returns:
                Returns true if Emergency is present.
     _________________________________________________________________

  getTopologyDescriptor

   public TopologyDescriptor[] getTopologyDescriptor()

          Gets the object refernces of all the topology descriptors
          specified for this context.


        Returns:
                The vector of the reference to the object identifier of
                type topology descriptor information.
     _________________________________________________________________

  isPriorityPresent

public boolean isPriorityPresent()

          The method can be used to check if the prority value is
          specified for this context.

        Returns:
                Returns true if priority is present. The function is
                called prior to getPriority function to check if priroty
                is present.
   ______________________________________________________________________

  getPriority

   public int getPriority()
        throws javax.megaco.ParameterNotSetException

          The method can be used to get the prority value specified for
          this context. Before invoking this method, isPriorityPresent
          method must be invoked to verify the presence of priority field
          in the class object.

        Returns:
                Returns the priority value specified for this context.

        Throws:
                ParameterNotSetException : This exception is raised if
                the priority is not set and the get method is invoked.
     _________________________________________________________________

  setEM

public void setEM(boolean isEMPresent)

          The method can be used to set if the EM is present as a part of
          the context level parameters.

        Parameters:
                        isEMPresent- The boolean value to indicate if the
                emergency is present as a part of the context level
                parameters.
     _________________________________________________________________

  setPriority

   public void setPriority(int priority)
        throws javax.megaco.InvalidArgumentException

   The method can be used to set the priority value for this context.

   Parameters:
                  priority - Integer value of the priority that shall be
          set. The only valid values specified by the protocol for this
          parameters is between 0 to 15.
          Throws:
                  InvalidArgumentException : The valid values of priority
          ranges between 0 to 15. If any value other than this is set
          then the exception is raised.
     _________________________________________________________________

  setTopologyDescriptor

     public void setTopologyDescriptor(TopologyDescriptor
     topologyDescriptor[])

             throws javax.megaco.InvalidArgumentException

   Sets the vector of Topology Descriptor Information for this command.

          Parameters:
          The vector of reference to the object identifier of type
          topology descriptor information

        
   Throws:
          InvalidArgumentException : This exception is raised if the
          reference of Topology Descriptor passed to this method is NULL.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
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
