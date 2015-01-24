// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/descriptor/ModemDescriptor.java <p>
 
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
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class ModemDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.ModemDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class ModemDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   modem descriptor.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   ModemDescriptor (ModemParam[] modemParam)
       throwsjavax.megaco.InvalidArgumentException

              Constructs an Modem Descriptor object with the vector of
   modem param. This extends the Descriptor class. It defines the Modem
   descriptor of the Megaco.

   Method Summary
    final ModemParam[] getModemParam()
              Returns a vector of object of class modem param.
    ModemParamValue[] getModemParamValue()
              Gets the vector of name and parameter values for specifying
   the property of the modem. This would return a NULL value when the
   param value has not been set.
    void  setModemParamValue (ModemParamValue[] param)
           throws javax.megaco.InvalidArgumentException

              Sets the vector of name and parameter values for specifying
   the property of the modem. If the parameter is not be set then this
   method need not be called.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ModemDescriptor

   public ModemDescriptor(ModemParam[] modemParam)
       throws javax.megaco.InvalidArgumentException

          Constructs a Modem Descriptor with a vector of object
          references to ModemParam.

    
   Throws:
          InvalidArgumentException : This exception is raised if the
          reference of vector of Modem Param passed to this method is
          NULL.
     _________________________________________________________________

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type Modem
          descriptor. This method overrides the corresponding method of
          the base class Descriptor.


        Returns:
                Returns an integer value that identifies this object as
                the type of modem descriptor. It returns that it is Modem
                Descriptor i.e., M_MODEM_DESC.
     _________________________________________________________________

  getModemParam

public ModemParam[] getModemParam()

          This method returns the vector of the object of class
          ModemParam. The class ModemParam contains the modem type and
          extension string if the modem type is extension parameter.


        Returns:
                Returns a vector of objects of class ModemParam.
     _________________________________________________________________

  getModemParamValue

public ModemParamValue[] getModemParamValue()
            throws javax.megaco.ParameterNotSetException, javax.megaco.MethodIn
vocationException

          Gets the vector of name and parameter values for specifying the
          property of the modem. This would return a NULL value when the
          param value has not been set.


        Returns:
                Returns the vector of name and parameter values for
                specifying the property of the modem. This would return a
                NULL value when the param value has not been set.
     _________________________________________________________________

  setModemParamValue

public void setModemParamValue(ModemParamValue[] param)
            throws javax.megaco.InvalidArgumentException

          Sets the vector of name and parameter values for specifying the
          property of the modem. If the parameter is not be set then this
          method need not be called.


        Parameters:
                param - Sets the vector of the object refernces to
                name-param value pair for modem properties.

        Throws:
                javax.megaco.InvalidArgumentException if the param name
                or param value or the relation used is not one of the
                allowed values or the vector of ModemParamValue is set to
                NULL.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
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
