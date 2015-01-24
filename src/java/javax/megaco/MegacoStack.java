// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/MegacoStack.java <p>
 
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

javax.megaco
Interface MegacoStack
     _________________________________________________________________

   public interface MegacoStack

   This interface defines the methods that are required to represent a
   proprietary JAIN MEGACO protocol stack, the implementation of which
   will be vendor-specific. Methods are defined for creating and deleting
   instances of a MegacoProvider.
     _________________________________________________________________

   Method Summary
    MegacoProvider createProvider()
             throwsjavax.megaco.CreateProviderException

             Creates a new Peer (vendor-specific) MegacoProvider and
   returns a reference to it.

    void deleteProvider(MegacoProvider  MegacoProvider)
             throwsjavax.megaco.DeleteProviderException

             Deletes the specified MegacoProvider.

    MegacoProvider getProvider(int  assocHandle)
             throwsjavax.megaco.NonExistentAssocException

             Gets the MegacoProvider reference for the specified
   assocHandle.

    java.lang.String getProtocolVersion()
             Gets the currently-set version of the MEGACO protocol in use
   by an object that implements the MegacoStack interface.
    void setProtocolVersion(java.lang.String protocolVersion)
             throwsjavax.megaco.VersionNotSupportedException

             Sets the version of the MEGACO protocol to be used by an
   object that implements the MegacoStack interface.

   Method Detail

  createProvider

public MegacoProvider createProvider()
                                throws javax.megaco.CreateProviderException

          Creates a new Peer (vendor-specific) MegacoProvider and returns
          a reference to it.


        Returns:
                A reference to the MegacoProviderImpl Object that exposes
                the MegacoProvider interface.

        Throws:
                 CreateProviderException - Indicates that a problem
                occurred during the instantiation of the
                MegacoProviderImpl class.
     _________________________________________________________________

  deleteProvider

public void deleteProvider(MegacoProvider MegacoProvider)
                    throws javax.megaco.DeleteProviderException

          Deletes the specified MegacoProvider.


        Throws:
                DeleteProviderException - Indicates that a problem
                occurred during the deletion of the Provider Object.
     _________________________________________________________________

  getProvider

public MegacoProvider getProvider(int assocHandle)
                    throws javax.megaco.NonExistentAssocException

          Gets the reference of the specified MegacoProvider.


        Throws:
                NonExistentAssocException - Indicates that no Provider
                Object exists for the specified AssocHandle.
     _________________________________________________________________

  getProtocolVersion

public java.lang.String getProtocolVersion()

          Gets the currently-set version of the MEGACO protocol in use by
          an object that i mplements the MegacoStack interface.


        Returns:
                The currently-set version of the MEGACO protocol in use
                by an object that implements the MegacoStack interface.
     _________________________________________________________________

  setProtocolVersion

public void setProtocolVersion(java.lang.String protocolVersion)
                    throws javax.megaco.VersionNotSupportedException

          Sets the highest version of the MEGACO protocol to be used by
          an object that implements the MegacoStack interface. For each
          association the version used may be less than or greater than
          this.


        Parameters:
                protocolVersion - The version of the MEGACO protocol to
                be used by an object that implements the MegacoStack
                interface.

        Throws:
                VersionNotSupportedException - Indicates that the version
                is not supported by the stack.
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
