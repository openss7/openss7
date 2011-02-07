// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: PkgStatsItem.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:19 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:19 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class PkgStatsItem

java.lang.Object
  |
  +--javax.megaco.pkg.PkgItem
        |
        +--javax.megaco.pkg.PkgStatsItem

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
          NetworkDurStats , NetworkOrStats , NetworkOsStats ,
          RTPDelayStats , RTPJitStats , RTPPlStats , RTPPrStats ,
          RTPPsStats
     _________________________________________________________________

   public abstract class PkgStatsItem
   extends PkgItem

   The MEGACO statistics item class is an abstract class defined as the
   base class for all the statistics items in the MEGACO Package. This
   class shall be used for setting the statistics and their values. This
   extends the PkgItem class. This is an abstract class and hence cannot
   be created as a separate object.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   PkgStatsItem()
              Constructs a MEGACO Statistics item from an abstract class.

   Method Summary
    abstract int  getStatisticsId()
              The method can be used to get the statistics identifier.
    abstract int  getItemValueType()
              This method specifies the type of the parameter value, i.e.
   whether string or enumerated value or integer or boolean or double
   value. The value types are defined in ParamValueType.
   final  int  getItemsValueRelation()
           throws javax.megaco.ParameterNotSetException

              This method specifies the relation specified in the item
   value. The relation operator can be one of equal, not equal, greater
   than or less than operator for single value. The MEGACO parameter is
   accompanied by a parameter value that can be single value or set of
   values or sublist of values or range of values. The relation operator
   can be equal when the value is set or sublist or range. This method
   specifies both the relation operator and also specifies whether the
   accompaning parameter value is single value or set of values or
   sublist of values or range of value. If the relation specifies set or
   range or sublist, it automatically assumes the relation to be
   "MEGACO_EQUAL". The value types are defined in ParamValueType.
   abstract  int[]  getItemsDescriptorIds()
              This method specifies the valid descriptor ids to which the
   item can belong to. This method shall be used to get the corresponding
   values from the derived class. The values of descriptor ids are
   defined in DescriptorType.
   final  java.lang.String[]  getItemStringValue()
           throws javax.megaco.MethodInvocationException,
   javax.megaco.ParameterNotSetException

              This method gets the valid value which is of string type.
   This is to be called only if the getItemValueType returns
   M_ITEM_PARAM_VALUE_STRING. Else shall throw an exception.
   final  int[]  getItemIntValue()
           throws javax.megaco.MethodInvocationException,
   javax.megaco.ParameterNotSetException

              This method gets the valid value which is of integer type.
   This is to be called only if the getItemValueType returns
   M_ITEM_PARAM_VALUE_INTEGER. Else shall throw an exception.
   final  boolean  getItemBooleanValue()
           throws javax.megaco.MethodInvocationException,
   javax.megaco.ParameterNotSetException

              This method gets the valid value which is of type boolean.
   If the item value is of type boolean, only one value can be set. This
   is to be called only if the getItemValueType returns
   M_ITEM_PARAM_VALUE_BOOLEAN. Else shall throw an exception.
   final  double[]  getItemDoubleValue()
           throws javax.megaco.MethodInvocationException,
   javax.megaco.ParameterNotSetException

              This method gets the valid value which is of double type.
   This is to be called only if the getItemValueType returns
   M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an exception. This shall
   change to INT64 bit value in the next release of RFC3015.
   final void  setItemsValueRelation( ParamRelation paramRelation)
              This method specifies the relation specified in the item
   value. The relation operator can be one of equal, not equal, greater
   than or less than operator for single value. The MEGACO parameter is
   accompanied by a parameter value that can be single value or set of
   values or sublist of values or range of values. The relation operator
   can be equal when the value is set or sublist or range. This method
   specifies both the relation operator and also specifies whether the
   accompaning parameter value is single value or set of values or
   sublist of values or range of value. If the relation specifies set or
   range or sublist, it automatically assumes the relation to be
   "MEGACO_EQUAL". The value types are defined in ParamRelationThe
   default value of the relation can be set in constructor of each class
   that derives this class.
   final void setItemValue(java.lang.String[] value)
           throws javax.megaco.InvalidArgumentException

              This method sets the list of values where each element is
   of string type.
   final void setItemValue(int[] value)
           throws javax.megaco.InvalidArgumentException

              This method sets the list of values where each element is
   of integer type.
   final  void  setItemValue(boolean value)

              This method sets the valid value which is of type boolean.
   final  void  setItemValue(double[] value)
           throws javax.megaco.InvalidArgumentException

              This method sets the list of values where each element is
   of double type. This is to be called only if the getItemValueType
   returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an exception. This
   shall change to INT64 bit value in the next release of RFC3015.
    java.lang.String toString()
              Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Methods inherited from class javax.megaco.pkg.PkgItem
   getItemName, getItemId, getItemType, getItemsPkgId

   Constructor Detail

  PkgStatsItem

public PkgStatsItem()

          Constructs a Jain MEGACO Package Statistics Item Object. This
          is an abstract class and can be called only by the derived
          classes.

   Method Detail

  getStatisticsId

public abstract int getStatisticsId()

          The method can be used to get the statistics identifier. This
          method gives the statistics id of the package. This is an
          abstract method and is defined by the methods of the derived
          class which shall return an hard coded value for the statistics
          id.

        Returns:
                statisticsId - The integer corresponding to the
                statistics id.
     _________________________________________________________________

  getItemId

public abstract int getItemId()

          The method can be used to get the statistics identifier. This
          method gives the statistics id of the package. This is an
          abstract method and is defined by the methods of the derived
          class which shall return an hard coded value for the statistics
          id.

        Returns:
                statisticsId - The integer corresponding to the
                statistics id.
     _________________________________________________________________

  getItemType

public final int getItemType()

          This method overrides the corresponding method in PkgItem. This
          shall return an hard coded value to indicate the item type is
          statistics.

        Returns:
                itemType - An integer value for the item type
                corresponding to the statistics. This shall return
                M_ITEM_STATISTICS.
     _________________________________________________________________

  getItemValueType

public abstract int getItemValueType()

          The method can be used to get the type of the value as defined
          in the MEGACO packages. These could be one of string or
          enumerated value or integer or boolean or double value.

        Returns:
                valueType - The integer corresponding to the value type.
                The values shall be defined in ParamValueType.
     _________________________________________________________________

  getItemsValueRelation

public final int getItemsValueRelation()
                     throws javax.megaco.ParameterNotSetException

          The method can be used to get the relation of the value as
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
          relation to be "MEGACO_EQUAL".

        Returns:
                paramRelation - The int corresponding to the value
                relation. The values shall be defined in ParamRelation.

        Throws:
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getItemsDescriptorIds

public abstract int[] getItemsDescriptorIds()

          The method can be used to get the descriptor ids corresponding
          to the item. This shall be overriden by the derived class and
          would be returned a hardcoded value from the derived class.

        Returns:
                descriptorId - The vector of integers corresponding to
                the descriptor to which the item can be used. The values
                shall be defined in DescriptorType.
     _________________________________________________________________

  getItemStringValue

public final java.lang.String[] getItemStringValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to set the valid value which is of
          string type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_STRING. Else shall throw an
          exception.

        Returns:
                value - The string values that needs to be set for the
                item.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of string type.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getItemIntValue

public final int[] getItemIntValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to set the valid item value which is of
          integer type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_INTEGER. Else shall throw an
          exception.

        Returns:
                intValues - The integer values corresponding to the value
                set for the item. The enumerated values shall be defined
                in in the respective package parameter classes.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is of enumerated type and the value does not
                match with one of the enumerated members. It shall also
                throw an exception if the value type is neither integer
                nor enumerated. .
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getItemBooleanValue

public final boolean getItemBooleanValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid value which is of
          boolean type. If the parameter value is of type boolean, only
          one value can be returned. This is to be called only if the
          getItemValueType returns M_ITEM_PARAM_VALUE_BOOLEAN. Else shall
          throw an exception.

        Returns:
                booValue - The boolean value qualifying the item.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of boolean type.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  getItemDoubleValue

public final double[] getItemDoubleValue()
                     throws javax.megaco.MethodInvocationException, javax.megac
o.ParameterNotSetException

          The method can be used to get the valid value which is of
          double type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an
          exception. This shall change to INT64 bit value in the next
          release of RFC3015.

        Returns:
                doubleValues - The double values corresponding to the
                items to which the value is set.

        Throws:
                javax.megaco.MethodInvocationException - Thrown if item
                value type is not of double type.
                javax.megaco.ParameterNotSetException - Thrown if this
                parameter has not been set.
     _________________________________________________________________

  setItemsValueRelation

public final void setItemsValueRelation(ParamRelation paramRelation)

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
          relation to be "MEGACO_EQUAL".

        Returns:
                paramRelation - The integer corresponding to the value
                relation. The values shall be defined in ParamRelation.
     _________________________________________________________________

  setItemValue

public  final void setItemValue(java.lang.String[] value)
                     throws javax.megaco.InvalidArgumentException

          This method sets the list of values where each element is of
          string type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_STRING. Else shall throw an
          exception.

        Parameter:
                value - A vector of string values.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  setItemValue

public  final void setItemValue(int[] value)
                     throws javax.megaco.InvalidArgumentException

          This method sets the list of values where each element is of
          integer type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_INTEGER. If values is of enumerated
          type, the integer value should be one defined in the
          corresponding parameter's constants. Else shall throw an
          exception.

        Parameter:
                value - A vector of integer values.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if invalid
                argument is passed for setting the item value.
     _________________________________________________________________

  setItemValue

public  final void setItemValue(boolean value)

          This method sets the valid value which is of boolean type. If
          the parameter value is of type boolean, only one value can be
          set. This is to be called only if the getItemValueType returns
          M_ITEM_PARAM_VALUE_BOOLEAN. Else shall throw an exception.

        Parameter:
                value - A boolean value.
     _________________________________________________________________

  setItemValue

public  final void setItemValue(double[] value)
                     throws javax.megaco.InvalidArgumentException

          This method sets the list of values where each element is of
          double type. This is to be called only if the getItemValueType
          returns M_ITEM_PARAM_VALUE_DOUBLE. Else shall throw an
          exception. This shall change to INT64 bit value in the next
          release of RFC3015.

        Parameter:
                value - A vector of double values.

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
