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

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource
Interface Error
     _________________________________________________________________

public interface Error

   Symbols defining errors.
     _________________________________________________________________

   Field Summary
   static Symbol e_BadArg

   static Symbol e_BadASI

   static Symbol e_BadContainer

   static Symbol e_BadContainerName

   static Symbol e_BadContext

   static Symbol e_BadDataFlow

   static Symbol e_BadDataID

   static Symbol e_BadDestAddress

   static Symbol e_BadEvent

   static Symbol e_BadFileFormat

   static Symbol e_BadFunction

   static Symbol e_BadGrammarContext

   static Symbol e_BadGroup

   static Symbol e_BadGroupConfig

   static Symbol e_BadKey

   static Symbol e_BadKVSet

   static Symbol e_BadList

   static Symbol e_BadMode

   static Symbol e_BadMSC

   static Symbol e_BadObject

   static Symbol e_BadObjectName

   static Symbol e_BadParm

   static Symbol e_BadParmName

   static Symbol e_BadParmType

   static Symbol e_BadParmValue

   static Symbol e_BadPattern

   static Symbol e_BadResource

   static Symbol e_BadRTC

   static Symbol e_BadRTCAction

   static Symbol e_BadRTCCondition

   static Symbol e_BadRule

   static Symbol e_BadRuleExpansion

   static Symbol e_BadServer

   static Symbol e_BadService

   static Symbol e_BadSession

   static Symbol e_BadSignalID

   static Symbol e_BadSM

   static Symbol e_BadTargetSession

   static Symbol e_BadTDD

   static Symbol e_BadTVM

   static Symbol e_BadValueType

   static Symbol e_BadWord

   static Symbol e_BandwidthMismatch

   static Symbol e_Busy

   static Symbol e_CallDropped

   static Symbol e_Coder

   static Symbol e_Comms

   static Symbol e_ConnExists

   static Symbol e_ContainerExists

   static Symbol e_ContainerFull

   static Symbol e_ContainerNotExist

   static Symbol e_CouldNotDrop

   static Symbol e_Destroyed

   static Symbol e_Directionality

   static Symbol e_Disconnected

   static Symbol e_DupHandler

   static Symbol e_EndOfKeys

   static Symbol e_Exists

   static Symbol e_HandlerNotFound

   static Symbol e_IncorrectContext

   static Symbol e_InfiniteLoop

   static Symbol e_InUse

   static Symbol e_InvalidAccessMode

   static Symbol e_InvalidCCMessage

   static Symbol e_InvalidFile

   static Symbol e_InvalidHandle

   static Symbol e_InvalidParameter

   static Symbol e_InvalidPosition

   static Symbol e_InvalidTDD

   static Symbol e_InvPosition

   static Symbol e_KeyNotFound

   static Symbol e_Locked

   static Symbol e_MSCFail

   static Symbol e_NoBandwidth

   static Symbol e_NoBuffering

   static Symbol e_NoConnection

   static Symbol e_NoConnections

   static Symbol e_NoConsultationCall

   static Symbol e_NoContainer

   static Symbol e_NoDeviceAvailable

   static Symbol e_NoDonor

   static Symbol e_NoExists

   static Symbol e_NoProfile

   static Symbol e_NotAnswered

   static Symbol e_NotConfigured

   static Symbol e_NoTermination

   static Symbol e_NotFound

   static Symbol e_NotOnStack

   static Symbol e_NotPaused

   static Symbol e_NoTranStopped

   static Symbol e_NotSupported

   static Symbol e_NullArray

   static Symbol e_ObjectExists

   static Symbol e_ObjectFull

   static Symbol e_ObjectNotExist

   static Symbol e_OK
             indicates there was no error.
   static Symbol e_OutOfData

   static Symbol e_OutOfService

   static Symbol e_Paused

   static Symbol e_Permission

   static Symbol e_Quota

   static Symbol e_Refused

   static Symbol e_ResourceUnavailable

   static Symbol e_ServiceDefined

   static Symbol e_ServiceNotDefined

   static Symbol e_ServiceNotRegistered

   static Symbol e_ShuttingDown

   static Symbol e_Stopped

   static Symbol e_System

   static Symbol e_Timeout

   static Symbol e_Unconnectable

   static Symbol e_UnstoppableTran

   static Symbol e_ValueIsArray

   static Symbol e_ValueNotArray



   Field Detail

  e_OK

static final Symbol e_OK

          indicates there was no error.
     _________________________________________________________________

  e_BadArg

static final Symbol e_BadArg
     _________________________________________________________________

  e_BadASI

static final Symbol e_BadASI
     _________________________________________________________________

  e_BadContainer

static final Symbol e_BadContainer
     _________________________________________________________________

  e_BadContainerName

static final Symbol e_BadContainerName
     _________________________________________________________________

  e_BadContext

static final Symbol e_BadContext
     _________________________________________________________________

  e_BadDataFlow

static final Symbol e_BadDataFlow
     _________________________________________________________________

  e_BadDataID

static final Symbol e_BadDataID
     _________________________________________________________________

  e_BadDestAddress

static final Symbol e_BadDestAddress
     _________________________________________________________________

  e_BadEvent

static final Symbol e_BadEvent
     _________________________________________________________________

  e_BadFileFormat

static final Symbol e_BadFileFormat
     _________________________________________________________________

  e_BadFunction

static final Symbol e_BadFunction
     _________________________________________________________________

  e_BadGrammarContext

static final Symbol e_BadGrammarContext
     _________________________________________________________________

  e_BadGroup

static final Symbol e_BadGroup
     _________________________________________________________________

  e_BadGroupConfig

static final Symbol e_BadGroupConfig
     _________________________________________________________________

  e_BadKey

static final Symbol e_BadKey
     _________________________________________________________________

  e_BadKVSet

static final Symbol e_BadKVSet
     _________________________________________________________________

  e_BadList

static final Symbol e_BadList
     _________________________________________________________________

  e_BadMode

static final Symbol e_BadMode
     _________________________________________________________________

  e_BadObject

static final Symbol e_BadObject
     _________________________________________________________________

  e_BadObjectName

static final Symbol e_BadObjectName
     _________________________________________________________________

  e_BadParm

static final Symbol e_BadParm
     _________________________________________________________________

  e_BadParmName

static final Symbol e_BadParmName
     _________________________________________________________________

  e_BadParmType

static final Symbol e_BadParmType
     _________________________________________________________________

  e_BadParmValue

static final Symbol e_BadParmValue
     _________________________________________________________________

  e_BadPattern

static final Symbol e_BadPattern
     _________________________________________________________________

  e_BadResource

static final Symbol e_BadResource
     _________________________________________________________________

  e_BadRTC

static final Symbol e_BadRTC
     _________________________________________________________________

  e_BadRTCAction

static final Symbol e_BadRTCAction
     _________________________________________________________________

  e_BadRTCCondition

static final Symbol e_BadRTCCondition
     _________________________________________________________________

  e_BadRule

static final Symbol e_BadRule
     _________________________________________________________________

  e_BadRuleExpansion

static final Symbol e_BadRuleExpansion
     _________________________________________________________________

  e_BadServer

static final Symbol e_BadServer
     _________________________________________________________________

  e_BadService

static final Symbol e_BadService
     _________________________________________________________________

  e_BadSession

static final Symbol e_BadSession
     _________________________________________________________________

  e_BadSignalID

static final Symbol e_BadSignalID
     _________________________________________________________________

  e_BadSM

static final Symbol e_BadSM
     _________________________________________________________________

  e_BadTargetSession

static final Symbol e_BadTargetSession
     _________________________________________________________________

  e_BadTDD

static final Symbol e_BadTDD
     _________________________________________________________________

  e_BadMSC

static final Symbol e_BadMSC
     _________________________________________________________________

  e_BadTVM

static final Symbol e_BadTVM
     _________________________________________________________________

  e_BadValueType

static final Symbol e_BadValueType
     _________________________________________________________________

  e_BadWord

static final Symbol e_BadWord
     _________________________________________________________________

  e_BandwidthMismatch

static final Symbol e_BandwidthMismatch
     _________________________________________________________________

  e_Busy

static final Symbol e_Busy
     _________________________________________________________________

  e_CallDropped

static final Symbol e_CallDropped
     _________________________________________________________________

  e_Coder

static final Symbol e_Coder
     _________________________________________________________________

  e_Comms

static final Symbol e_Comms
     _________________________________________________________________

  e_ConnExists

static final Symbol e_ConnExists
     _________________________________________________________________

  e_ContainerExists

static final Symbol e_ContainerExists
     _________________________________________________________________

  e_ContainerFull

static final Symbol e_ContainerFull
     _________________________________________________________________

  e_ContainerNotExist

static final Symbol e_ContainerNotExist
     _________________________________________________________________

  e_CouldNotDrop

static final Symbol e_CouldNotDrop
     _________________________________________________________________

  e_Destroyed

static final Symbol e_Destroyed
     _________________________________________________________________

  e_Directionality

static final Symbol e_Directionality
     _________________________________________________________________

  e_Disconnected

static final Symbol e_Disconnected
     _________________________________________________________________

  e_DupHandler

static final Symbol e_DupHandler
     _________________________________________________________________

  e_EndOfKeys

static final Symbol e_EndOfKeys
     _________________________________________________________________

  e_Exists

static final Symbol e_Exists
     _________________________________________________________________

  e_HandlerNotFound

static final Symbol e_HandlerNotFound
     _________________________________________________________________

  e_IncorrectContext

static final Symbol e_IncorrectContext
     _________________________________________________________________

  e_InfiniteLoop

static final Symbol e_InfiniteLoop
     _________________________________________________________________

  e_InUse

static final Symbol e_InUse
     _________________________________________________________________

  e_InvalidAccessMode

static final Symbol e_InvalidAccessMode
     _________________________________________________________________

  e_InvalidCCMessage

static final Symbol e_InvalidCCMessage
     _________________________________________________________________

  e_InvalidFile

static final Symbol e_InvalidFile
     _________________________________________________________________

  e_InvalidHandle

static final Symbol e_InvalidHandle
     _________________________________________________________________

  e_InvalidParameter

static final Symbol e_InvalidParameter
     _________________________________________________________________

  e_InvPosition

static final Symbol e_InvPosition
     _________________________________________________________________

  e_InvalidPosition

static final Symbol e_InvalidPosition
     _________________________________________________________________

  e_InvalidTDD

static final Symbol e_InvalidTDD
     _________________________________________________________________

  e_KeyNotFound

static final Symbol e_KeyNotFound
     _________________________________________________________________

  e_Locked

static final Symbol e_Locked
     _________________________________________________________________

  e_NoBandwidth

static final Symbol e_NoBandwidth
     _________________________________________________________________

  e_NoBuffering

static final Symbol e_NoBuffering
     _________________________________________________________________

  e_NoConnection

static final Symbol e_NoConnection
     _________________________________________________________________

  e_NoConnections

static final Symbol e_NoConnections
     _________________________________________________________________

  e_NoConsultationCall

static final Symbol e_NoConsultationCall
     _________________________________________________________________

  e_NoContainer

static final Symbol e_NoContainer
     _________________________________________________________________

  e_NoDeviceAvailable

static final Symbol e_NoDeviceAvailable
     _________________________________________________________________

  e_NoDonor

static final Symbol e_NoDonor
     _________________________________________________________________

  e_NoExists

static final Symbol e_NoExists
     _________________________________________________________________

  e_NoProfile

static final Symbol e_NoProfile
     _________________________________________________________________

  e_NotAnswered

static final Symbol e_NotAnswered
     _________________________________________________________________

  e_NotConfigured

static final Symbol e_NotConfigured
     _________________________________________________________________

  e_NoTermination

static final Symbol e_NoTermination
     _________________________________________________________________

  e_NotFound

static final Symbol e_NotFound
     _________________________________________________________________

  e_NotOnStack

static final Symbol e_NotOnStack
     _________________________________________________________________

  e_NotPaused

static final Symbol e_NotPaused
     _________________________________________________________________

  e_NoTranStopped

static final Symbol e_NoTranStopped
     _________________________________________________________________

  e_NotSupported

static final Symbol e_NotSupported
     _________________________________________________________________

  e_NullArray

static final Symbol e_NullArray
     _________________________________________________________________

  e_ObjectExists

static final Symbol e_ObjectExists
     _________________________________________________________________

  e_ObjectFull

static final Symbol e_ObjectFull
     _________________________________________________________________

  e_ObjectNotExist

static final Symbol e_ObjectNotExist
     _________________________________________________________________

  e_OutOfData

static final Symbol e_OutOfData
     _________________________________________________________________

  e_OutOfService

static final Symbol e_OutOfService
     _________________________________________________________________

  e_Paused

static final Symbol e_Paused
     _________________________________________________________________

  e_Permission

static final Symbol e_Permission
     _________________________________________________________________

  e_Quota

static final Symbol e_Quota
     _________________________________________________________________

  e_Refused

static final Symbol e_Refused
     _________________________________________________________________

  e_ResourceUnavailable

static final Symbol e_ResourceUnavailable
     _________________________________________________________________

  e_ServiceDefined

static final Symbol e_ServiceDefined
     _________________________________________________________________

  e_ServiceNotDefined

static final Symbol e_ServiceNotDefined
     _________________________________________________________________

  e_ServiceNotRegistered

static final Symbol e_ServiceNotRegistered
     _________________________________________________________________

  e_ShuttingDown

static final Symbol e_ShuttingDown
     _________________________________________________________________

  e_Stopped

static final Symbol e_Stopped
     _________________________________________________________________

  e_System

static final Symbol e_System
     _________________________________________________________________

  e_Timeout

static final Symbol e_Timeout
     _________________________________________________________________

  e_MSCFail

static final Symbol e_MSCFail
     _________________________________________________________________

  e_Unconnectable

static final Symbol e_Unconnectable
     _________________________________________________________________

  e_UnstoppableTran

static final Symbol e_UnstoppableTran
     _________________________________________________________________

  e_ValueIsArray

static final Symbol e_ValueIsArray
     _________________________________________________________________

  e_ValueNotArray

static final Symbol e_ValueNotArray
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright © 2007-2008 Oracle and/or its affiliates. © Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.resource;
