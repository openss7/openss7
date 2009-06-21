// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ModemParamValue.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:37 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:37 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class ModemParamValue

java.lang.Object
  |
  +--javax.megaco.message.descriptor.ModemParamValue

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class ModemParamValue
   extends java.lang.Object

   The MEGACO Modem Parameter value class stores the parameter name and
   value for the Modem Descriptor.
     _________________________________________________________________

   Constructor Summary
   ModemParamValue()
              Constructs a Modem Parameter value class that stores the
   parameter name and value for the Modem Descriptor.

   Method Summary
   java.lang.String getParamName()

              The method can be used to get the string identifier of the
   parameter name.
    void setParamName(java.lang.String name)
           throws javax.megaco.InvalidArgumentException

              The method can be used to set the string identifier of the
   parameter name.
   int getParamsValueRelation()

              This method specifies the relation specified in the
   parameter for the parameter value. The relation operator can be one of
   equal, not equal, greater than or less than operator for single value.
   The Megaco parameter is accompanied by a parameter value that can be
   single value or set of values or sublist of values or range of values.
   The relation operator can be equal when the value is set or sublist or
   range. This method specifies both the relation operator and also
   specifies whether the accompaning parameter value is single value or
   set of values or sublist of values or range of value. If the relation
   specifies set or range or sublist, it automatically assumes the
   relation to be "MEGACO_EQUAL". The value types are defined in
   ParamRelation
    java.lang.String[] getParamsValue()

              This method gets the valid parameter which is of string
   type.
    int setParamsValueRelation (ParamRelation paramRelation)
       throws javax.megaco.InvalidArgumentException

              This method specifies the relation specified in the
   parameter for the parameter value. The relation operator can be one of
   equal, not equal, greater than or less than operator for single value.
   The Megaco parameter is accompanied by a parameter value that can be
   single value or set of values or sublist of values or range of values.
   The relation operator can be equal when the value is set or sublist or
   range. This method specifies both the relation operator and also
   specifies whether the accompaning parameter value is single value or
   set of values or sublist of values or range of value. If the relation
   specifies set or range or sublist, it automatically assumes the
   relation to be "MEGACO_EQUAL". The value types are defined in
   ParamRelation. The default value of the relation can be set in
   constructor of each class that derives this class.
    void setParamsValue(java.lang.String[] value)
           throws javax.megaco.InvalidArgumentException

             This method sets the list of parameter values where each
   element is of string type.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  ModemParamValue

public ModemParamValue()

          Constructs a Jain Megaco Modem Parameter Value class that is
          used for specifying the parameters of a modem descriptor.

   Method Detail

  getParamName

public java.lang.String getParamName()

          The method can be used to get the parameter name for the modem
          descriptor parameter.


        Returns:
                paramName - The string value corresponding to the
                parameter name. If the param name is not set then this
                method will return NULL.
     _________________________________________________________________

  setParamName

public void setParamName(java.lang.java.lang.String name)
                     throws javax.megaco.InvalidArgumentException

          The method can be used to set the parameter name for the modem
          descriptor parameter.


        Parameters:
                name - The string value corresponding to the parameter
                name.

        Throws:
                javax.megaco.InvalidArgumentException - Exception shall
                be thrown if the param name is not of proper format.
     _________________________________________________________________

  getParamsValueRelation

public  int getParamsValueRelation()

          The method can be used to get the relation set in the parameter
          for the parameter value as defined in the MEGACO packages. The
          relation operator can be one of equal, not equal, greater than
          or less than operator for single value. The Megaco parameter is
          accompanied by a parameter value that can be single value or
          set of values or sublist of values or range of values. The
          relation operator can be equal when the value is set or sublist
          or range. This method specifies both the relation operator and
          also specifies whether the accompaning parameter value is
          single value or set of values or sublist of values or range of
          value. If the relation specifies set or range or sublist, it
          automatically assumes the relation to be "MEGACO_EQUAL".

        Returns:
                paramRelation - The integer corresponding to the
                parameter relation. The values shall be defined in
                ParamRelation. If the param value is not set then this
                method will return NULL.
     _________________________________________________________________

  getParamsValue

public  java.lang.String[] getParamsValue()

          The method can be used to get the valid parameter which is of
          string type.

        Returns:
                value - The string values that needs to be set for the
                parameter. If the param value is not set then this method
                will return NULL.
     _________________________________________________________________

  setParamsValueRelation

   public void setParamsValueRelation(ParamRelation paramRelation)
           throws javax.megaco.InvalidArgumentException

          The method can be used to set the relation of the value as
          defined in the MEGACO packages. The relation operator can be
          one of equal, not equal, greater than or less than operator for
          single value. The Megaco parameter is accompanied by a
          parameter value that can be single value or set of values or
          sublist of values or range of values. The relation operator can
          be equal when the value is set or sublist or range. This method
          specifies both the relation operator and also specifies whether
          the accompaning parameter value is single value or set of
          values or sublist of values or range of value. If the relation
          specifies set or range or sublist, it automatically assumes the
          relation to be "MEGACO_EQUAL". The default value of the
          relation can be set in constructor of each class that derives
          this class.

        Returns:
                paramRelation - The integer corresponding to the value
                relation. The values shall be defined in ParamRelation.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Param Relation passed to this method is
                NULL.
     _________________________________________________________________

  setParamsValue

public void setParamsValue(java.lang.String[] value)
                     throws javax.megaco.InvalidArgumentException

          This method sets the list of parameter values where each
          element is of string type.

        Parameter:
                value - A vector of string values.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
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
