// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/pkg/PackageName.java <p>
 
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

package jain.protocol.ip.mgcp.pkg;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.pkg
Class PackageName

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.pkg.PackageName

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class PackageName
   extends java.lang.Object
   implements java.io.Serializable

   The PackageName class is used to define the names of the MGCP event
   packages. Two static factory methods are provided: one which accepts
   the "name" of a package (a string) and an integer value to be
   associated uniquely with the named package; the other which accepts
   only the "name" of the package, leaving it to the factory method to
   assign a unique integer value. Both factory methods return a reference
   to a PackageName object whose attributes are the name of the package
   and the associated integer value. If the first factory method is
   invoked with a name that is already associated with another integer
   value, or with an integer value that is already associated with
   another name, the method throws an exception.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int ALL_PACKAGES
             Signifies the "All Packages" wildcard.
   static PackageName AllPackages
             Encapsulates the "All Packages" wildcard constant.
   static PackageName Announcement
             Encapsulates the Announcement Server package constant.
   static int ANNOUNCEMENT
             Signifies the Announcement Server package.
   static PackageName Dtmf
             Encapsulates the DTMF package constant.
   static int DTMF
             Signifies the DTMF package.
   static int GENERIC_MEDIA
             Signifies the Generic Media package.
   static PackageName GenericMedia
             Encapsulates the Generic Media package constant.
   static PackageName Handset
             Encapsulates the Handset package constant.
   static int HANDSET
             Signifies the Handset package.
   static PackageName Line
             Encapsulates the Line package constant.
   static int LINE
             Signifies the Line package.
   static PackageName Mf
             Encapsulates the MF package constant.
   static int MF
             Signifies the MF package.
   static PackageName Nas
             Encapsulates the Network Access Server package constant.
   static int NAS
             Signifies the Network Access Server package.
   static PackageName Rtp
             Encapsulates the RTP package constant.
   static int RTP
             Signifies the RTP package.
   static PackageName Script
             Encapsulates the Script package constant.
   static int SCRIPT
             Signifies the Script package.
   static PackageName Trunk
             Encapsulates the Trunk package constant.
   static int TRUNK
             Signifies the Trunk package.
   static int USER_DEFINED_PACKAGE_START_VALUE
             Represents the initial integer value that can be used for
   user-defined package names.



   Method Summary
   static PackageName factory(java.lang.String packageName)
             The factory method for generating a new MGCP package where
   the integer value to be associated with the package is left to the
   method to supply.
   static PackageName factory(java.lang.String packageName,
   int packageValue)
             The factory method for generating a new MGCP package name
   where an integer value to be associated with the package name is
   supplied.
   static int getCurrentLargestPackageValue()
             Gets the largest package value that has been used thus far.
    int intValue()
             Gets the integer value that identifies the package uniquely.
    java.lang.String toString()
             Overrides java.lang.Object.toString().



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  ALL_PACKAGES

public static final int ALL_PACKAGES

          Signifies the "All Packages" wildcard.
     _________________________________________________________________

  AllPackages

public static final PackageName AllPackages

          Encapsulates the "All Packages" wildcard constant.
     _________________________________________________________________

  Announcement

public static final PackageName Announcement

          Encapsulates the Announcement Server package constant.
     _________________________________________________________________

  ANNOUNCEMENT

public static final int ANNOUNCEMENT

          Signifies the Announcement Server package.
     _________________________________________________________________

  Dtmf

public static final PackageName Dtmf

          Encapsulates the DTMF package constant.
     _________________________________________________________________

  DTMF

public static final int DTMF

          Signifies the DTMF package.
     _________________________________________________________________

  GENERIC_MEDIA

public static final int GENERIC_MEDIA

          Signifies the Generic Media package.
     _________________________________________________________________

  GenericMedia

public static final PackageName GenericMedia

          Encapsulates the Generic Media package constant.
     _________________________________________________________________

  Handset

public static final PackageName Handset

          Encapsulates the Handset package constant.
     _________________________________________________________________

  HANDSET

public static final int HANDSET

          Signifies the Handset package.
     _________________________________________________________________

  Line

public static final PackageName Line

          Encapsulates the Line package constant.
     _________________________________________________________________

  LINE

public static final int LINE

          Signifies the Line package.
     _________________________________________________________________

  Mf

public static final PackageName Mf

          Encapsulates the MF package constant.
     _________________________________________________________________

  MF

public static final int MF

          Signifies the MF package.
     _________________________________________________________________

  Nas

public static final PackageName Nas

          Encapsulates the Network Access Server package constant.
     _________________________________________________________________

  NAS

public static final int NAS

          Signifies the Network Access Server package.
     _________________________________________________________________

  Rtp

public static final PackageName Rtp

          Encapsulates the RTP package constant.
     _________________________________________________________________

  RTP

public static final int RTP

          Signifies the RTP package.
     _________________________________________________________________

  Script

public static final PackageName Script

          Encapsulates the Script package constant.
     _________________________________________________________________

  SCRIPT

public static final int SCRIPT

          Signifies the Script package.
     _________________________________________________________________

  Trunk

public static final PackageName Trunk

          Encapsulates the Trunk package constant.
     _________________________________________________________________

  TRUNK

public static final int TRUNK

          Signifies the Trunk package.
     _________________________________________________________________

  USER_DEFINED_PACKAGE_START_VALUE

public static final int USER_DEFINED_PACKAGE_START_VALUE

          Represents the initial integer value that can be used for
          user-defined package names.

   Method Detail

  factory

public static PackageName factory(java.lang.String packageName)

          The factory method for generating a new MGCP package where the
          integer value to be associated with the package is left to the
          method to supply.

        Parameters:
                packageName - The string that represents the name of the
                package.
     _________________________________________________________________

  factory

public static PackageName factory(java.lang.String packageName,
                                  int packageValue)
                           throws java.lang.IllegalArgumentException

          The factory method for generating a new MGCP package name where
          an integer value to be associated with the package name is
          supplied.

        Parameters:
                packageName - The string that represents the name of the
                package.
                packageValue - The integer value to be uniquely
                associated with the name of the package.

        Throws:
                java.lang.IllegalArgumentException - Thrown if another
                package name is already associated with the specified
                package value, or if the specified package name is
                already associated with another package value.
     _________________________________________________________________

  getCurrentLargestPackageValue

public static int getCurrentLargestPackageValue()

          Gets the largest package value that has been used thus far.
          When calling PackageName.factory(packageName, packageValue),
          can use PackageName.getCurrentLargestPackageValue() + 1 for
          packageValue to ensure that a non-conflicting package value
          will be used.
     _________________________________________________________________

  intValue

public int intValue()

          Gets the integer value that identifies the package uniquely.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().

        Overrides:
                toString in class java.lang.Object

        Returns:
                The name of the package.
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
