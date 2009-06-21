// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: PkgItemParam.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:39 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:39 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class PkgItemParam

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItemParam

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
              AnalogLineSParamCad,  AnalogLineSParamFreq,
          AnalogLineSParamInit, AnalogLineSParamMaxdur,
          AnalogLineSParamMindur, AnalogLineSParamStrict, ContParamRes,
          DTMFDetParamDs, DTMFDetParamMeth, GenParamFailureCause,
          GenParamGenCause, GenParamSigID, GenParamTermMeth,
          NetworkParamCs, NetworkParamTh, RTPParamRtppltype,
          ToneParamDur, ToneParamTid, ToneParamTl, ToneGenParamInd,
          ToneGenParamTl
     _________________________________________________________________

   public abstract class PkgItemParam
   extends java.lang.Object

   The MEGACO package item parameter class is an abstract and shall be
   used for setting the parameter name and value attached to an event or
   a signal. The derived class for this would specify the hard coded
   value for their identity, name, type and other parameters, but the
   value for the parameter would be set within this base class.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   PkgItemParam()
              Constructs an abstract MEGACO Package item parameter
   object.

   Method Summary
    abstract int  getParamId()
              The method is used to get the enumerated value of the
   parameter identifier. An hard coded value is returned from the derived
   class.
    abstract int  getParamValueType()
              This method specifies the type of the parameter value, i.e.
   whether string or enumerated value or integer or double value or
   boolean. This method shall be used to get the corresponding value from
   the derived class. The value types are defined in ParamValueType
   final  int  getParamsValueRelation()
           throws javax.megaco.ParameterNotSetException

              This method specifies the relation specified in the
   parameter for the parameter value. The relation operator can be one of
   equal, not equal, greater than or less than operator for single value.
   The MEGACO parameter is accompanied by a parameter value that can be
   single value or set of values or sublist of values or range of values.
   The relation operator can be equal when the value is set or sublist or
   range. This method specifies both the relation operator and also
   specifies whether the accompaning parameter value is single value or
   set of values or sublist of values or range of value. If the relation
   specifies set or range or sublist, it automatically assumes the
   relation to be "MEGACO_EQUAL". The value types are defined in
   ParamRelation
   abstract  int[]  getParamsDescriptorIds()
              This method specifies the valid descriptor ids to which the
   parameter can belong to. This method shall be used to get the
   corresponding values from the derived class. The values of descriptor
   ids are defined in DescriptorType
   abstract  int[]  getParamsItemIds()
              This method specifies the valid item (event/signal) ids to
   which the parameter can belong to. This method shall be used to get
   the corresponding values from the derived class.
   abstract  int  getParamsPkgId()
              This method specifies the package for which the parameter
   is valid. Even though the parameter may be set for an item, but the
   parameter may not be valid for package to which the item belongs, but
   may be valid for a package which has extended this package. This
   method shall be used to get the corresponding values from the derived
   class.
   final  java.lang.String[]  getParamsStringValue()
           throws
   javax.megaco.MethodInvocationExceptionjavax.megaco.ParameterNotSetExce
   ption

              This method gets the valid parameter which is of string
   type. This is to be called only if the getParamValueType returns
   M_ITEM_PARAM_VALUE_STRING. Else shall throw an exception.
   final  int[]  getParamsIntValue()
           throws
   javax.megaco.MethodInvocationExceptionjavax.megaco.ParameterNotSetExce
   ption

              This method gets the valid parameter which is of integer or
   enumerated type. This is to be called only if the getParamValueType
   returns M_ITEM_PARAM_VALUE_INTEGER or M_ITEM_PARAM_VALUE_ENUM. Else
   shall throw an exception.
   final  boolean  getParamsBooleanValue()
           throws
   javax.megaco.MethodInvocationExceptionjavax.megaco.ParameterNotSetExce
   ption

              This method gets the valid parameter which is of type
   boolean. If the parameter value is of type boolean, only one value can
   be returned and shall not be set. This is to be called only if the
   getParamValueType returns M_ITEM_PARAM_VALUE_BOOLEAN. Else shall throw
   an exception.
   final  double[]  getParamsDoubleValue()
           throws
   javax.megaco.MethodInvocationExceptionjavax.megaco.ParameterNotSetExce
   ption

              This method gets the valid parameter which is of double
   type. This is to be called only if the getParamValueType returns
   M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an exception. This shall
   change to INT64 bit value in the next release of RFC3015.
   final int setParamsValueRelation( ParamRelation paramRelation)
         throws javax.megaco.InvalidArgumentException

              This method specifies the relation specified in the
   parameter for the parameter value. The relation operator can be one of
   equal, not equal, greater than or less than operator for single value.
   The MEGACO parameter is accompanied by a parameter value that can be
   single value or set of values or sublist of values or range of values.
   The relation operator can be equal when the value is set or sublist or
   range. This method specifies both the relation operator and also
   specifies whether the accompaning parameter value is single value or
   set of values or sublist of values or range of value. If the relation
   specifies set or range or sublist, it automatically assumes the
   relation to be "MEGACO_EQUAL". The value types are defined in
   ParamRelation. The default value of the relation can be set in
   constructor of each class that derives this class.
   final void setParamsValue(java.lang.String[] value)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.MethodInvocationException

             This method sets the list of parameter values where each
   element is of string type. This is to be called only if the
   getParamValueType returns M_ITEM_PARAM_VALUE_STRING. Else shall throw
   an exception.
   final void setParamsValue(int[] value)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.MethodInvocationException

             This method sets the list of parameter values where each
   element is of integer or enumerated type. This is to be called only if
   the getParamValueType returns M_ITEM_PARAM_VALUE_INTEGER or
   M_ITEM_PARAM_VALUE_ENUM. Else shall throw an exception.
   final  void  setParamsValue(boolean value)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.MethodInvocationException

             This method sets the valid parameter which is of type
   boolean. If the parameter value is of type boolean, only one value can
   be returned and shall not be set. This is to be called only if the
   getParamValueType returns M_ITEM_PARAM_VALUE_BOOLEAN. Else shall throw
   an exception.
   final  void  setParamsValue(double[] value)
           throws javax.megaco.InvalidArgumentException,
   javax.megaco.MethodInvocationException

             This method sets the list of parameter values where each
   element is of double type. This is to be called only if the
   getParamValueType returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw
   an exception. This shall change to INT64 bit value in the next release
   of RFC3015.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  PkgItemParam

public PkgItemParam()

          Constructs a Jain MEGACO package item parameter Object. This is
          an abstract class and can be called only by the derived
          classes.

   Method Detail

  getParamId

public abstract int getParamId()

          The method can be used to get the parameter identifier as
          defined in the MEGACO packages. A hardcoded value is returned
          by the derived class.


        Returns:
                paramId - The integer corresponding to the parameter id.
     _________________________________________________________________

  getParamValueType

public abstract int getParamValueType()

          The method can be used to get the type of the parameter as
          defined in the MEGACO packages. These could be one of string or
          enumerated value or integer or double value or boolean.


        Returns:
                paramType - The integer corresponding to the parameter
                type. The values shall be defined in ParamValueType.
     _________________________________________________________________

  getParamsValueRelation

public final int getParamsValueRelation()
                     throws javax.megaco.ParameterNotSetException

          The method can be used to get the relation set in the parameter
          for the parameter value as defined in the MEGACO packages. The
          relation operator can be one of equal, not equal, greater than
          or less than operator for single value. The MEGACO parameter is
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
                ParamRelation.

        Throws:
                javax.megaco.ParameterNotSetException - Exception shall
                be thrown if the param value is not set.
     _________________________________________________________________

  getParamsDescriptorIds

public abstract int[] getParamsDescriptorIds()

          The method can be used to get the descriptor ids corresponding
          to the parameters to which the parameter can be set. This shall
          be overriden by the derived class and would be returned a
          hardcoded value from the derived class.

        Returns:
                descriptorId - The integer corresponding to the
                descriptor to which the parameter can be used. The values
                shall be defined in DescriptorType.
     _________________________________________________________________

  getParamsItemIds

public abstract int[] getParamsItemIds()

          The method can be used to get the item ids corresponding to the
          parameters to which the parameter can be set. This shall be
          overriden by the derived class and would be returned a
          hardcoded value from the derived class. This method specifies
          the valid item (event/signal) ids to which the parameter can
          belong to.

        Returns:
                itemId - The integer values corresponding to the items to
                which the parameter can be used.
     _________________________________________________________________

  getParamsPkgId

public abstract int getParamsPkgId()

          This method specifies the package for which the parameter is
          valid. Even though the parameter may be set for an item, but
          the parameter may not be valid for package to which the item
          belongs, but may be valid for a package which has extended this
          package. This method shall be used to get the corresponding
          values from the derived class.

        Returns:
                pkgId - The integer values corresponding to the items to
                which the parameter can be used.
     _________________________________________________________________

  getParamsStringValue

public final java.lang.String[] getParamsStringValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid parameter which is of
          string type. This is to be called only if the getParamValueType
          returns M_ITEM_PARAM_VALUE_STRING. Else shall throw an
          exception.

        Returns:
                value - The string values that needs to be set for the
                parameter.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of string type.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getParamsIntValue

public final int[] getParamsIntValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid parameter which is of
          integer type. This is to be called only if the
          getParamValueType returns M_ITEM_PARAM_VALUE_INTEGER or
          M_ITEM_PARAM_VALUE_ENUM. Else shall throw an exception.

        Returns:
                intValues - The integer or the enumerated values
                corresponding to the items to which the parameter can be
                used. In case of enumerated values, the constants would
                be defined in the respective parameter constants class.

        Throws:
                javax.megaco.MethodInvocationException - If the value
                type for the parameter is of enumerated type and the
                value does not match with one of the enumerated members.
                It shall also throw an exception if the value type is
                neither integer nor enumerated.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getParamsBooleanValue

public final boolean getParamsBooleanValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid parameter which is of
          boolean type. If the parameter value is of type boolean, only
          one value can be returned. This is to be called only if the
          getParamValueType returns M_ITEM_PARAM_VALUE_BOOLEAN. Else
          shall throw an exception.

        Returns:
                booValue - The boolean value corresponding to the items
                to which the parameter can be used.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getParamsDoubleValue

public final double[] getParamsDoubleValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid parameter which is of
          double type. This is to be called only if the getParamValueType
          returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an
          exception. This shall change to INT64 bit value in the next
          release of RFC3015.

        Returns:
                doubleValues - The double values corresponding to the
                items to which the parameter can be used.

        Throws:
                javax.megaco.MethodInvocationException - If the value
                type is not double.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  setParamsValueRelation

   public final void setParamsValueRelation(ParamRelation paramRelation)
       throws javax.megaco.InvalidArgumentException

          The method can be used to set the relation of the value as
          defined in the MEGACO packages. The relation operator can be
          one of equal, not equal, greater than or less than operator for
          single value. The MEGACO parameter is accompanied by a
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

public  final void setParamsValue(java.lang.String[] value)
                     throws javax.megaco.MethodInvocationException, javax.megac
o.InvalidArgumentException

          This method sets the list of parameter values where each
          element is of string type. This is to be called only if the
          getParamValueType returns M_ITEM_PARAM_VALUE_STRING. Else shall
          throw an exception.

        Parameter:
                value - A vector of string values.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of string type.
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  setParamsValue

public  final void setParamsValue(int[] value)
                     throws javax.megaco.MethodInvocationException, javax.megac
o.InvalidArgumentException

          This method sets the list of parameter values where each
          element is of integer or enum type. This is to be called only
          if the getParamValueType returns M_ITEM_PARAM_VALUE_INTEGER or
          M_ITEM_PARAM_VALUE_ENUM. If values is of enumerated type, the
          integer value should be one defined in the corresponding
          parameter's constants. Else shall throw an exception.

        Parameter:
                value - A vector of integer values.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of enumerated or integer type.
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  setParamsValue

public  final void setParamsValue(boolean value)
                     throws javax.megaco.MethodInvocationException, javax.megac
o.InvalidArgumentException

          This method sets the parameter value of boolean type. If the
          parameter value is of type boolean, only one value can be set.
          This is to be called only if the getParamValueType returns
          M_ITEM_PARAM_VALUE_BOOLEAN. Else shall throw an exception.

        Parameter:
                value - A boolean value.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of boolean type.
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  setParamsValue

public  final void setParamsValue(double[] value)
                     throws javax.megaco.MethodInvocationException, javax.megac
o.InvalidArgumentException

          This method sets the list of parameter values where each
          element is of double type. This is to be called only if the
          getParamValueType returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall
          throw an exception. This shall change to INT64 bit value in the
          next release of RFC3015.

        Parameter:
                value - A vector of double values.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of double type.
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
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
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
