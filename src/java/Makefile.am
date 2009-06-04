# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date$ by $Author$
#
# =============================================================================

##
## General rules
##
include ../../am/openss7.am

#############################################################################
# General Rules
#############################################################################

####javadir = $(datadir)/java
####dist_java_JAVA =
####JAVAROOT = $(builddir)
#####CLASSPATH =
####AM_JAVAFLAGS = -target 1.2 -warn:all

EXTRA_LTLIBRARIES =
EXTRA_DATA =

my_JAVAFILES =

#############################################################################
# This is for generating JAR modules
#############################################################################

# this is where the regular java jar files go
jardir = $(datadir)/java

jar_DATA =

JAR = fastjar

my_jar_LINKS =

my_jar_CLASSFILES =

$(my_jar_LINKS): %.jar : %-$(VERSION).jar
	$(AM_V_GEN)ln -sf $< $@

my_jar_LINKS += openss7.jar
ALL_LOCAL += openss7.jar
jar_DATA += openss7-$(VERSION).jar
CLEANFILES += openss7.jar openss7-$(VERSION).jar

openss7-$(VERSION).jar: $(my_jar_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

#############################################################################
# This is for generating GCJ modules
#############################################################################

# this where the gcj compiled libraries go
gcjlibdir = $(libdir)/gcj/$(PACKAGE_LCNAME)
gcjlib_LTLIBRARIES 	=
gcjlib_DATA		=

GCJDBTOOL		= gcj-dbtool

AM_GCJFLAGS = -I$(builddir) -I$(srcdir) -Wno-deprecated -Wno-unused -Wno-out-of-date
GCJLIB_INCLUDES = -I$(builddir) -I$(srcdir) $(PKG_INCLUDES) $(USER_CPPFLAGS)

.java.class:
@am__fastdepGCJ_TRUE@	$(AM_V_GCJ)$(GCJCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$$(basename $@ .class).Tpo -C -d $(abs_builddir) $<
@am__fastdepGCJ_TRUE@	$(AM_V_at)$(am__mv) $(DEPDIR)/$$(basename $@ .class).Tpo $(DEPDIR)/$$(basename $@ .class).Po
@am__fastdepGCJ_FALSE@	$(AM_V_GCJ) @AM_BACKSLASH@
@AMDEP_TRUE@@am__fastdepGCJ_FALSE@	source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepGCJ_FALSE@	DEPDIR=$(DEPDIR) $(GCJDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepGCJ_FALSE@	$(GCJCOMPILE) -C -d $(abs_buiddir) $<

#############################################################################
# This is how we build JAIN TCAP CNI classes for Java.
#############################################################################

AM_GCJHFLAGS = -I$(builddir) -I$(srcdir)

MY_SUFFIXES += .class

# This is how we build CNI headers and stub files

.class.h: %.class
	$(AM_V_at)$(MKDIR_P) $$(dirname $@)
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	gcjh $(AM_GCJHFLAGS) $(GCJHFLAGS) `echo $$f | sed -e 's,\.class$$,,;s,/,.,g'`

.class.cc:
	$(AM_V_at)$(MKDIR_P) $$(dirname $@)
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	gcjh -stubs $(AM_GCJHFLAGS) $(GCJHFLAGS) `echo $$f | sed -e 's,\.class$$,,;s,/,.,g'`

#############################################################################
# These are simple jars and their compiled equivalents.
#############################################################################

my_javax_jain_ss7_JAVAFILES = \
	javax/jain/ss7/EncodingScheme.java \
	javax/jain/ss7/GlobalTitle.java \
	javax/jain/ss7/GTIndicator0001.java \
	javax/jain/ss7/GTIndicator0010.java \
	javax/jain/ss7/GTIndicator0011.java \
	javax/jain/ss7/GTIndicator0100.java \
	javax/jain/ss7/GTIndicator.java \
	javax/jain/ss7/JainSS7Factory.java \
	javax/jain/ss7/JainSS7Utility.java \
	javax/jain/ss7/NAddressIndicator.java \
	javax/jain/ss7/NetworkIndicator.java \
	javax/jain/ss7/NumberingPlan.java \
	javax/jain/ss7/RoutingIndicator.java \
	javax/jain/ss7/SccpUserAddress.java \
	javax/jain/ss7/SignalingPointCode.java \
	javax/jain/ss7/SPCFormat.java \
	javax/jain/ss7/SS7Address.java \
	javax/jain/ss7/SubSystemAddress.java

my_javax_jain_ss7_CLASSFILES = \
	javax/jain/ss7/EncodingScheme.class \
	javax/jain/ss7/GlobalTitle.class \
	javax/jain/ss7/GTIndicator0001.class \
	javax/jain/ss7/GTIndicator0010.class \
	javax/jain/ss7/GTIndicator0011.class \
	javax/jain/ss7/GTIndicator0100.class \
	javax/jain/ss7/GTIndicator.class \
	javax/jain/ss7/JainSS7Factory.class \
	javax/jain/ss7/JainSS7Utility.class \
	javax/jain/ss7/NAddressIndicator.class \
	javax/jain/ss7/NetworkIndicator.class \
	javax/jain/ss7/NumberingPlan.class \
	javax/jain/ss7/RoutingIndicator.class \
	javax/jain/ss7/SccpUserAddress.class \
	javax/jain/ss7/SignalingPointCode.class \
	javax/jain/ss7/SPCFormat.class \
	javax/jain/ss7/SS7Address.class \
	javax/jain/ss7/SubSystemAddress.class

MY_SECONDARY += $(my_javax_jain_ss7_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_ss7_CLASSFILES)
my_jar_LINKS += javax-jain-ss7.jar
ALL_LOCAL += javax-jain-ss7.jar
jar_DATA += javax-jain-ss7-$(VERSION).jar

javax-jain-ss7-$(VERSION).jar: $(my_javax_jain_ss7_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_javax_jain_ss7_JAVAFILES)

clean-javax_jain_ss7:
	rm -f -- $(my_javax_jain_ss7_CLASSFILES)
	rm -f -- javax-jain-ss7.jar javax-jain-ss7-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7

## Build the whole jar into a library
lib_javax_jain_ss7_jar_la_SOURCES 	= javax-jain-ss7.jar
lib_javax_jain_ss7_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES)
lib_javax_jain_ss7_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_javax_jain_ss7_jar_la_LIBADD	=
lib_javax_jain_ss7_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_javax_jain_ss7_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7.jar.db

lib-javax-jain-ss7.jar.db: javax-jain-ss7.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so


#############################################################################

my_jain_isup_JAVAFILES = \
	javax/jain/CreateProviderException.java \
	javax/jain/DeleteProviderException.java \
	javax/jain/IDNotAvailableException.java \
	javax/jain/InvalidAddressException.java \
	javax/jain/InvalidArgumentException.java \
	javax/jain/InvalidPathNameException.java \
	javax/jain/InvalidStateException.java \
	javax/jain/JainException.java \
	javax/jain/ListenerAlreadyRegisteredException.java \
	javax/jain/ListenerNotRegisteredException.java \
	javax/jain/MandatoryParameterNotSetException.java \
	javax/jain/MethodNotSupportedException.java \
	javax/jain/ParameterNotSetException.java \
	javax/jain/PeerUnavailableException.java \
	javax/jain/PlatformException.java \
	javax/jain/ss7/isup/AlertEvent.java \
	javax/jain/ss7/isup/ansi/AlertAnsiEvent.java \
	javax/jain/ss7/isup/ansi/AnswerAnsiEvent.java \
	javax/jain/ss7/isup/ansi/BusinessGroupAnsi.java \
	javax/jain/ss7/isup/ansi/BwdCallIndAnsi.java \
	javax/jain/ss7/isup/ansi/CallProgressAnsiEvent.java \
	javax/jain/ss7/isup/ansi/CarrierIdAnsi.java \
	javax/jain/ss7/isup/ansi/ChargeNumberAnsi.java \
	javax/jain/ss7/isup/ansi/CircuitAssignMapAnsi.java \
	javax/jain/ss7/isup/ansi/CircuitGrpCharacteristicsIndAnsi.java \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAckAnsiEvent.java \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAnsiEvent.java \
	javax/jain/ss7/isup/ansi/CircuitIdNameAnsi.java \
	javax/jain/ss7/isup/ansi/CircuitReservationAnsiEvent.java \
	javax/jain/ss7/isup/ansi/CircuitValidationRespAnsiEvent.java \
	javax/jain/ss7/isup/ansi/CircuitValidationRespIndAnsi.java \
	javax/jain/ss7/isup/ansi/CommonLangLocationIdAnsi.java \
	javax/jain/ss7/isup/ansi/ExitAnsiEvent.java \
	javax/jain/ss7/isup/ansi/FwdCallIndAnsi.java \
	javax/jain/ss7/isup/ansi/GenericNameAnsi.java \
	javax/jain/ss7/isup/ansi/InfoIndAnsi.java \
	javax/jain/ss7/isup/ansi/InfoReqIndAnsi.java \
	javax/jain/ss7/isup/ansi/InformationAnsiEvent.java \
	javax/jain/ss7/isup/ansi/InformationReqAnsiEvent.java \
	javax/jain/ss7/isup/ansi/OperatorServicesInfoAnsi.java \
	javax/jain/ss7/isup/ansi/OptionalBwdCallIndAnsi.java \
	javax/jain/ss7/isup/ansi/ReleaseAnsiEvent.java \
	javax/jain/ss7/isup/ansi/SetupAnsiEvent.java \
	javax/jain/ss7/isup/ansi/TransactionReqAnsi.java \
	javax/jain/ss7/isup/ansi/TransitNwSelAnsi.java \
	javax/jain/ss7/isup/AnswerEvent.java \
	javax/jain/ss7/isup/BwdCallInd.java \
	javax/jain/ss7/isup/CalledPartyNumber.java \
	javax/jain/ss7/isup/CallFailureIndEvent.java \
	javax/jain/ss7/isup/CallingPartyNumber.java \
	javax/jain/ss7/isup/CallProgressEvent.java \
	javax/jain/ss7/isup/CallReference.java \
	javax/jain/ss7/isup/CauseInd.java \
	javax/jain/ss7/isup/CircuitGrpCommonEvent.java \
	javax/jain/ss7/isup/CircuitGrpMaintenanceEvent.java \
	javax/jain/ss7/isup/CircuitGrpQueryEvent.java \
	javax/jain/ss7/isup/CircuitGrpQueryRespEvent.java \
	javax/jain/ss7/isup/CircuitGrpResetAckEvent.java \
	javax/jain/ss7/isup/CircuitGrpResetEvent.java \
	javax/jain/ss7/isup/CircuitMaintenanceEvent.java \
	javax/jain/ss7/isup/ConfusionEvent.java \
	javax/jain/ss7/isup/ConReq.java \
	javax/jain/ss7/isup/ContinuityEvent.java \
	javax/jain/ss7/isup/EventInformation.java \
	javax/jain/ss7/isup/FacilityEvent.java \
	javax/jain/ss7/isup/ForwardTransferEvent.java \
	javax/jain/ss7/isup/FwdCallInd.java \
	javax/jain/ss7/isup/GenericDigits.java \
	javax/jain/ss7/isup/GenericNumber.java \
	javax/jain/ss7/isup/InfoInd.java \
	javax/jain/ss7/isup/InfoReqInd.java \
	javax/jain/ss7/isup/InformationEvent.java \
	javax/jain/ss7/isup/InformationReqEvent.java \
	javax/jain/ss7/isup/InvalidListenerException.java \
	javax/jain/ss7/isup/IsupConstants.java \
	javax/jain/ss7/isup/IsupErrorIndEvent.java \
	javax/jain/ss7/isup/IsupEvent.java \
	javax/jain/ss7/isup/IsupException.java \
	javax/jain/ss7/isup/IsupParameter.java \
	javax/jain/ss7/isup/IsupUserAddress.java \
	javax/jain/ss7/isup/itu/AlertItuEvent.java \
	javax/jain/ss7/isup/itu/AnswerItuEvent.java \
	javax/jain/ss7/isup/itu/BwdCallIndItu.java \
	javax/jain/ss7/isup/itu/CallDiversionInfoItu.java \
	javax/jain/ss7/isup/itu/CalledPartyNumberItu.java \
	javax/jain/ss7/isup/itu/CallingPartyNumberItu.java \
	javax/jain/ss7/isup/itu/CallProgressItuEvent.java \
	javax/jain/ss7/isup/itu/ConnectedNumberItu.java \
	javax/jain/ss7/isup/itu/ConnectItuEvent.java \
	javax/jain/ss7/isup/itu/CUGInterlockCodeItu.java \
	javax/jain/ss7/isup/itu/EchoControlInfoItu.java \
	javax/jain/ss7/isup/itu/FacilityAcceptItuEvent.java \
	javax/jain/ss7/isup/itu/FacilityItuEvent.java \
	javax/jain/ss7/isup/itu/FacilityRejectItuEvent.java \
	javax/jain/ss7/isup/itu/FacilityRequestItuEvent.java \
	javax/jain/ss7/isup/itu/FwdCallIndItu.java \
	javax/jain/ss7/isup/itu/GenericNumberItu.java \
	javax/jain/ss7/isup/itu/IdentificationReqItuEvent.java \
	javax/jain/ss7/isup/itu/IdentificationRespItuEvent.java \
	javax/jain/ss7/isup/itu/InformationItuEvent.java \
	javax/jain/ss7/isup/itu/InformationReqItuEvent.java \
	javax/jain/ss7/isup/itu/LocationNumberItu.java \
	javax/jain/ss7/isup/itu/MCIDReqIndItu.java \
	javax/jain/ss7/isup/itu/MCIDRespIndItu.java \
	javax/jain/ss7/isup/itu/MessageCompatibilityInfoItu.java \
	javax/jain/ss7/isup/itu/NetworkResourceMgmtItuEvent.java \
	javax/jain/ss7/isup/itu/NwSpecificFacItu.java \
	javax/jain/ss7/isup/itu/OptionalBwdCallIndItu.java \
	javax/jain/ss7/isup/itu/OptionalFwdCallIndItu.java \
	javax/jain/ss7/isup/itu/ParamCompatibilityInfoItu.java \
	javax/jain/ss7/isup/itu/RedirectionNumberItu.java \
	javax/jain/ss7/isup/itu/ReleaseItuEvent.java \
	javax/jain/ss7/isup/itu/ReleaseResponseItuEvent.java \
	javax/jain/ss7/isup/itu/SegmentationItuEvent.java \
	javax/jain/ss7/isup/itu/SetupItuEvent.java \
	javax/jain/ss7/isup/itu/SubsequentAddressItuEvent.java \
	javax/jain/ss7/isup/itu/UserPartTestInfoItuEvent.java \
	javax/jain/ss7/isup/itu/UserTeleserviceInfoItu.java \
	javax/jain/ss7/isup/itu/UserToUserIndicatorsItu.java \
	javax/jain/ss7/isup/itu/UserToUserInfoItuEvent.java \
	javax/jain/ss7/isup/JainIsupListener.java \
	javax/jain/ss7/isup/JainIsupProvider.java \
	javax/jain/ss7/isup/JainIsupStack.java \
	javax/jain/ss7/isup/MaintenanceTimeoutIndEvent.java \
	javax/jain/ss7/isup/NatureConnInd.java \
	javax/jain/ss7/isup/NotificationInd.java \
	javax/jain/ss7/isup/NumberParameter.java \
	javax/jain/ss7/isup/NwStatusIndEvent.java \
	javax/jain/ss7/isup/OptionalBwdCallInd.java \
	javax/jain/ss7/isup/OrigCalledNumber.java \
	javax/jain/ss7/isup/ParameterAlreadySetException.java \
	javax/jain/ss7/isup/ParameterRangeInvalidException.java \
	javax/jain/ss7/isup/PassAlongEvent.java \
	javax/jain/ss7/isup/Precedence.java \
	javax/jain/ss7/isup/PrimitiveInvalidException.java \
	javax/jain/ss7/isup/RangeAndStatus.java \
	javax/jain/ss7/isup/RawMessageEvent.java \
	javax/jain/ss7/isup/ReattemptIndEvent.java \
	javax/jain/ss7/isup/RedirectingNumber.java \
	javax/jain/ss7/isup/RedirectionInfo.java \
	javax/jain/ss7/isup/ReleaseEvent.java \
	javax/jain/ss7/isup/ReleaseResponseEvent.java \
	javax/jain/ss7/isup/RemoteOperations.java \
	javax/jain/ss7/isup/SendStackException.java \
	javax/jain/ss7/isup/SetupEvent.java \
	javax/jain/ss7/isup/StartResetIndEvent.java \
	javax/jain/ss7/isup/StopMaintReqEvent.java \
	javax/jain/ss7/isup/SuspendResumeEvent.java \
	javax/jain/ss7/isup/TransitNwSel.java \
	javax/jain/ss7/isup/UserServiceInfoCommon.java \
	javax/jain/ss7/isup/UserServiceInfo.java \
	javax/jain/ss7/isup/UserServiceInfoPrime.java \
	javax/jain/ss7/isup/UserToUserIndicators.java \
	javax/jain/ss7/JainSS7Factory.java \
	javax/jain/ss7/JainSS7Utility.java \
	javax/jain/ss7/NetworkIndicator.java \
	javax/jain/ss7/SignalingPointCode.java \
	javax/jain/ss7/SPCFormat.java \
	javax/jain/ss7/SS7Address.java \
	javax/jain/VersionNotSupportedException.java

my_javax_jain_ss7_isup_CLASSFILES = \
	javax/jain/CreateProviderException.class \
	javax/jain/DeleteProviderException.class \
	javax/jain/IDNotAvailableException.class \
	javax/jain/InvalidAddressException.class \
	javax/jain/InvalidArgumentException.class \
	javax/jain/InvalidPathNameException.class \
	javax/jain/InvalidStateException.class \
	javax/jain/JainException.class \
	javax/jain/ListenerAlreadyRegisteredException.class \
	javax/jain/ListenerNotRegisteredException.class \
	javax/jain/MandatoryParameterNotSetException.class \
	javax/jain/MethodNotSupportedException.class \
	javax/jain/ParameterNotSetException.class \
	javax/jain/PeerUnavailableException.class \
	javax/jain/PlatformException.class \
	javax/jain/ss7/isup/AlertEvent.class \
	javax/jain/ss7/isup/ansi/AlertAnsiEvent.class \
	javax/jain/ss7/isup/ansi/AnswerAnsiEvent.class \
	javax/jain/ss7/isup/ansi/BusinessGroupAnsi.class \
	javax/jain/ss7/isup/ansi/BwdCallIndAnsi.class \
	javax/jain/ss7/isup/ansi/CallProgressAnsiEvent.class \
	javax/jain/ss7/isup/ansi/CarrierIdAnsi.class \
	javax/jain/ss7/isup/ansi/ChargeNumberAnsi.class \
	javax/jain/ss7/isup/ansi/CircuitAssignMapAnsi.class \
	javax/jain/ss7/isup/ansi/CircuitGrpCharacteristicsIndAnsi.class \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAckAnsiEvent.class \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAnsiEvent.class \
	javax/jain/ss7/isup/ansi/CircuitIdNameAnsi.class \
	javax/jain/ss7/isup/ansi/CircuitReservationAnsiEvent.class \
	javax/jain/ss7/isup/ansi/CircuitValidationRespAnsiEvent.class \
	javax/jain/ss7/isup/ansi/CircuitValidationRespIndAnsi.class \
	javax/jain/ss7/isup/ansi/CommonLangLocationIdAnsi.class \
	javax/jain/ss7/isup/ansi/ExitAnsiEvent.class \
	javax/jain/ss7/isup/ansi/FwdCallIndAnsi.class \
	javax/jain/ss7/isup/ansi/GenericNameAnsi.class \
	javax/jain/ss7/isup/ansi/InfoIndAnsi.class \
	javax/jain/ss7/isup/ansi/InfoReqIndAnsi.class \
	javax/jain/ss7/isup/ansi/InformationAnsiEvent.class \
	javax/jain/ss7/isup/ansi/InformationReqAnsiEvent.class \
	javax/jain/ss7/isup/ansi/OperatorServicesInfoAnsi.class \
	javax/jain/ss7/isup/ansi/OptionalBwdCallIndAnsi.class \
	javax/jain/ss7/isup/ansi/ReleaseAnsiEvent.class \
	javax/jain/ss7/isup/ansi/SetupAnsiEvent.class \
	javax/jain/ss7/isup/ansi/TransactionReqAnsi.class \
	javax/jain/ss7/isup/ansi/TransitNwSelAnsi.class \
	javax/jain/ss7/isup/AnswerEvent.class \
	javax/jain/ss7/isup/BwdCallInd.class \
	javax/jain/ss7/isup/CalledPartyNumber.class \
	javax/jain/ss7/isup/CallFailureIndEvent.class \
	javax/jain/ss7/isup/CallingPartyNumber.class \
	javax/jain/ss7/isup/CallProgressEvent.class \
	javax/jain/ss7/isup/CallReference.class \
	javax/jain/ss7/isup/CauseInd.class \
	javax/jain/ss7/isup/CircuitGrpCommonEvent.class \
	javax/jain/ss7/isup/CircuitGrpMaintenanceEvent.class \
	javax/jain/ss7/isup/CircuitGrpQueryEvent.class \
	javax/jain/ss7/isup/CircuitGrpQueryRespEvent.class \
	javax/jain/ss7/isup/CircuitGrpResetAckEvent.class \
	javax/jain/ss7/isup/CircuitGrpResetEvent.class \
	javax/jain/ss7/isup/CircuitMaintenanceEvent.class \
	javax/jain/ss7/isup/ConfusionEvent.class \
	javax/jain/ss7/isup/ConReq.class \
	javax/jain/ss7/isup/ContinuityEvent.class \
	javax/jain/ss7/isup/EventInformation.class \
	javax/jain/ss7/isup/FacilityEvent.class \
	javax/jain/ss7/isup/ForwardTransferEvent.class \
	javax/jain/ss7/isup/FwdCallInd.class \
	javax/jain/ss7/isup/GenericDigits.class \
	javax/jain/ss7/isup/GenericNumber.class \
	javax/jain/ss7/isup/InfoInd.class \
	javax/jain/ss7/isup/InfoReqInd.class \
	javax/jain/ss7/isup/InformationEvent.class \
	javax/jain/ss7/isup/InformationReqEvent.class \
	javax/jain/ss7/isup/InvalidListenerException.class \
	javax/jain/ss7/isup/IsupConstants.class \
	javax/jain/ss7/isup/IsupErrorIndEvent.class \
	javax/jain/ss7/isup/IsupEvent.class \
	javax/jain/ss7/isup/IsupException.class \
	javax/jain/ss7/isup/IsupParameter.class \
	javax/jain/ss7/isup/IsupUserAddress.class \
	javax/jain/ss7/isup/itu/AlertItuEvent.class \
	javax/jain/ss7/isup/itu/AnswerItuEvent.class \
	javax/jain/ss7/isup/itu/BwdCallIndItu.class \
	javax/jain/ss7/isup/itu/CallDiversionInfoItu.class \
	javax/jain/ss7/isup/itu/CalledPartyNumberItu.class \
	javax/jain/ss7/isup/itu/CallingPartyNumberItu.class \
	javax/jain/ss7/isup/itu/CallProgressItuEvent.class \
	javax/jain/ss7/isup/itu/ConnectedNumberItu.class \
	javax/jain/ss7/isup/itu/ConnectItuEvent.class \
	javax/jain/ss7/isup/itu/CUGInterlockCodeItu.class \
	javax/jain/ss7/isup/itu/EchoControlInfoItu.class \
	javax/jain/ss7/isup/itu/FacilityAcceptItuEvent.class \
	javax/jain/ss7/isup/itu/FacilityItuEvent.class \
	javax/jain/ss7/isup/itu/FacilityRejectItuEvent.class \
	javax/jain/ss7/isup/itu/FacilityRequestItuEvent.class \
	javax/jain/ss7/isup/itu/FwdCallIndItu.class \
	javax/jain/ss7/isup/itu/GenericNumberItu.class \
	javax/jain/ss7/isup/itu/IdentificationReqItuEvent.class \
	javax/jain/ss7/isup/itu/IdentificationRespItuEvent.class \
	javax/jain/ss7/isup/itu/InformationItuEvent.class \
	javax/jain/ss7/isup/itu/InformationReqItuEvent.class \
	javax/jain/ss7/isup/itu/LocationNumberItu.class \
	javax/jain/ss7/isup/itu/MCIDReqIndItu.class \
	javax/jain/ss7/isup/itu/MCIDRespIndItu.class \
	javax/jain/ss7/isup/itu/MessageCompatibilityInfoItu.class \
	javax/jain/ss7/isup/itu/NetworkResourceMgmtItuEvent.class \
	javax/jain/ss7/isup/itu/NwSpecificFacItu.class \
	javax/jain/ss7/isup/itu/OptionalBwdCallIndItu.class \
	javax/jain/ss7/isup/itu/OptionalFwdCallIndItu.class \
	javax/jain/ss7/isup/itu/ParamCompatibilityInfoItu.class \
	javax/jain/ss7/isup/itu/RedirectionNumberItu.class \
	javax/jain/ss7/isup/itu/ReleaseItuEvent.class \
	javax/jain/ss7/isup/itu/ReleaseResponseItuEvent.class \
	javax/jain/ss7/isup/itu/SegmentationItuEvent.class \
	javax/jain/ss7/isup/itu/SetupItuEvent.class \
	javax/jain/ss7/isup/itu/SubsequentAddressItuEvent.class \
	javax/jain/ss7/isup/itu/UserPartTestInfoItuEvent.class \
	javax/jain/ss7/isup/itu/UserTeleserviceInfoItu.class \
	javax/jain/ss7/isup/itu/UserToUserIndicatorsItu.class \
	javax/jain/ss7/isup/itu/UserToUserInfoItuEvent.class \
	javax/jain/ss7/isup/JainIsupListener.class \
	javax/jain/ss7/isup/JainIsupProvider.class \
	javax/jain/ss7/isup/JainIsupStack.class \
	javax/jain/ss7/isup/MaintenanceTimeoutIndEvent.class \
	javax/jain/ss7/isup/NatureConnInd.class \
	javax/jain/ss7/isup/NotificationInd.class \
	javax/jain/ss7/isup/NumberParameter.class \
	javax/jain/ss7/isup/NwStatusIndEvent.class \
	javax/jain/ss7/isup/OptionalBwdCallInd.class \
	javax/jain/ss7/isup/OrigCalledNumber.class \
	javax/jain/ss7/isup/ParameterAlreadySetException.class \
	javax/jain/ss7/isup/ParameterRangeInvalidException.class \
	javax/jain/ss7/isup/PassAlongEvent.class \
	javax/jain/ss7/isup/Precedence.class \
	javax/jain/ss7/isup/PrimitiveInvalidException.class \
	javax/jain/ss7/isup/RangeAndStatus.class \
	javax/jain/ss7/isup/RawMessageEvent.class \
	javax/jain/ss7/isup/ReattemptIndEvent.class \
	javax/jain/ss7/isup/RedirectingNumber.class \
	javax/jain/ss7/isup/RedirectionInfo.class \
	javax/jain/ss7/isup/ReleaseEvent.class \
	javax/jain/ss7/isup/ReleaseResponseEvent.class \
	javax/jain/ss7/isup/RemoteOperations.class \
	javax/jain/ss7/isup/SendStackException.class \
	javax/jain/ss7/isup/SetupEvent.class \
	javax/jain/ss7/isup/StartResetIndEvent.class \
	javax/jain/ss7/isup/StopMaintReqEvent.class \
	javax/jain/ss7/isup/SuspendResumeEvent.class \
	javax/jain/ss7/isup/TransitNwSel.class \
	javax/jain/ss7/isup/UserServiceInfoCommon.class \
	javax/jain/ss7/isup/UserServiceInfo.class \
	javax/jain/ss7/isup/UserServiceInfoPrime.class \
	javax/jain/ss7/isup/UserToUserIndicators.class \
	javax/jain/ss7/JainSS7Factory.class \
	javax/jain/ss7/JainSS7Utility.class \
	javax/jain/ss7/NetworkIndicator.class \
	javax/jain/ss7/SignalingPointCode.class \
	javax/jain/ss7/SPCFormat.class \
	javax/jain/ss7/SS7Address.class \
	javax/jain/VersionNotSupportedException.class

MY_SECONDARY += $(my_javax_jain_ss7_isup_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_ss7_isup_CLASSFILES)
my_jar_LINKS += javax-jain-ss7-isup.jar
ALL_LOCAL += javax-jain-ss7-isup.jar
jar_DATA += javax-jain-ss7-isup-$(VERSION).jar

javax-jain-ss7-isup-$(VERSION).jar: $(my_javax_jain_ss7_isup_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_jain_isup_JAVAFILES)

clean-javax_jain_ss7_isup:
	rm -f -- $(my_javax_jain_ss7_isup_CLASSFILES)
	rm -f -- javax-jain-ss7-isup.jar javax-jain-ss7-isup-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-isup.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7_isup

## Build the whole jar into a library
lib_javax_jain_ss7_isup_jar_la_SOURCES	= javax-jain-ss7-isup.jar
lib_javax_jain_ss7_isup_jar_la_CPPFLAGS = $(GCJLIB_INCLUDES)
lib_javax_jain_ss7_isup_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_javax_jain_ss7_isup_jar_la_LIBADD	=
lib_javax_jain_ss7_isup_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_javax_jain_ss7_isup_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-isup.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7-isup.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7-isup.jar.db

lib-javax-jain-ss7-isup.jar.db: javax-jain-ss7-isup.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_javax_jain_ss7_inap_JAVAFILES = \
	javax/jain/ss7/inap/operation/Operation.java \
	javax/jain/ss7/inap/operation/ActivityTest.java \
	javax/jain/ss7/inap/operation/AnalysedInformation.java \
	javax/jain/ss7/inap/operation/ActivateServiceFiltering.java \
	javax/jain/ss7/inap/operation/CallGap.java \
	javax/jain/ss7/inap/operation/AnalyseInformation.java \
	javax/jain/ss7/inap/operation/ApplyCharging.java \
	javax/jain/ss7/inap/operation/ApplyChargingReport.java \
	javax/jain/ss7/inap/operation/Cancel.java \
	javax/jain/ss7/inap/operation/AssistRequestInstructions.java \
	javax/jain/ss7/inap/operation/AuthorizeTermination.java \
	javax/jain/ss7/inap/operation/Connect.java \
	javax/jain/ss7/inap/operation/CallInformationReport.java \
	javax/jain/ss7/inap/operation/CollectedInformation.java \
	javax/jain/ss7/inap/operation/CollectInformation.java \
	javax/jain/ss7/inap/operation/MoveLeg.java \
	javax/jain/ss7/inap/operation/ConnectToResource.java \
	javax/jain/ss7/inap/operation/Continue.java \
	javax/jain/ss7/inap/operation/ContinueWithArgument.java \
	javax/jain/ss7/inap/operation/CallInformationRequest.java \
	javax/jain/ss7/inap/operation/CancelStatusReportRequest.java \
	javax/jain/ss7/inap/operation/DFCWithArgument.java \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociation.java \
	javax/jain/ss7/inap/operation/DisconnectLeg.java \
	javax/jain/ss7/inap/operation/EntityReleased.java \
	javax/jain/ss7/inap/operation/DisconnectForwardConnection.java \
	javax/jain/ss7/inap/operation/EstablishTemporaryConnection.java \
	javax/jain/ss7/inap/operation/EventReportBCSM.java \
	javax/jain/ss7/inap/operation/EventReportFacility.java \
	javax/jain/ss7/inap/operation/EventNotificationCharging.java \
	javax/jain/ss7/inap/operation/FacilitySelectedAndAvailable.java \
	javax/jain/ss7/inap/operation/HoldCallInNetwork.java \
	javax/jain/ss7/inap/operation/InitialDP.java \
	javax/jain/ss7/inap/operation/InitiateCallAttempt.java \
	javax/jain/ss7/inap/operation/ManageTriggerData.java \
	javax/jain/ss7/inap/operation/FurnishChargingInformation.java \
	javax/jain/ss7/inap/operation/MergeCallSegments.java \
	javax/jain/ss7/inap/operation/MessageReceived.java \
	javax/jain/ss7/inap/operation/MoveCallSegments.java \
	javax/jain/ss7/inap/operation/OAnswer.java \
	javax/jain/ss7/inap/operation/OAbandon.java \
	javax/jain/ss7/inap/operation/TAnswer.java \
	javax/jain/ss7/inap/operation/OCalledPartyBusy.java \
	javax/jain/ss7/inap/operation/ODisconnect.java \
	javax/jain/ss7/inap/operation/OMidCall.java \
	javax/jain/ss7/inap/operation/ONoAnswer.java \
	javax/jain/ss7/inap/operation/OriginationAttempt.java \
	javax/jain/ss7/inap/operation/OSuspended.java \
	javax/jain/ss7/inap/operation/ManageTriggerDataResult.java \
	javax/jain/ss7/inap/operation/PlayAnnouncement.java \
	javax/jain/ss7/inap/operation/ReceivedInformation.java \
	javax/jain/ss7/inap/operation/OriginationAttemptAuthorized.java \
	javax/jain/ss7/inap/operation/PromptAndCollectUserInformation.java \
	javax/jain/ss7/inap/operation/Reconnect.java \
	javax/jain/ss7/inap/operation/ReleaseCall.java \
	javax/jain/ss7/inap/operation/ReportUTSI.java \
	javax/jain/ss7/inap/operation/RequestReportUTSI.java \
	javax/jain/ss7/inap/operation/PromptAndReceiveMessage.java \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReport.java \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReportResult.java \
	javax/jain/ss7/inap/operation/RequestEveryStatusChangeReport.java \
	javax/jain/ss7/inap/operation/RequestFirstStatusMatchReport.java \
	javax/jain/ss7/inap/operation/RequestNotificationChargingEvent.java \
	javax/jain/ss7/inap/operation/RequestReportBCSMEvent.java \
	javax/jain/ss7/inap/operation/ResetTimer.java \
	javax/jain/ss7/inap/operation/RouteSelectFailure.java \
	javax/jain/ss7/inap/operation/SelectFacility.java \
	javax/jain/ss7/inap/operation/SelectRoute.java \
	javax/jain/ss7/inap/operation/SendSTUI.java \
	javax/jain/ss7/inap/operation/RequestReportFacilityEvent.java \
	javax/jain/ss7/inap/operation/SendChargingInformation.java \
	javax/jain/ss7/inap/operation/SplitLeg.java \
	javax/jain/ss7/inap/operation/SendFacilityInformation.java \
	javax/jain/ss7/inap/operation/ServiceFilteringResponse.java \
	javax/jain/ss7/inap/operation/StatusReport.java \
	javax/jain/ss7/inap/operation/TBusy.java \
	javax/jain/ss7/inap/operation/TDisconnect.java \
	javax/jain/ss7/inap/operation/TerminationAttempt.java \
	javax/jain/ss7/inap/operation/SpecializedResourceReport.java \
	javax/jain/ss7/inap/operation/TMidCall.java \
	javax/jain/ss7/inap/operation/TNoAnswer.java \
	javax/jain/ss7/inap/operation/TSuspended.java \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociationResult.java \
	javax/jain/ss7/inap/operation/TerminationAttemptAuthorized.java \
	javax/jain/ss7/inap/constants/ActionIndicator.java \
	javax/jain/ss7/inap/constants/ActionPerformed.java \
	javax/jain/ss7/inap/constants/DialoguePrimitiveType.java \
	javax/jain/ss7/inap/constants/TransportAddressChoice.java \
	javax/jain/ss7/inap/constants/CancelChoice.java \
	javax/jain/ss7/inap/constants/CancelID.java \
	javax/jain/ss7/inap/constants/CGEncountered.java \
	javax/jain/ss7/inap/constants/CollectedInfoChoice.java \
	javax/jain/ss7/inap/constants/ComponentChoice.java \
	javax/jain/ss7/inap/constants/BearerCapabilityChoice.java \
	javax/jain/ss7/inap/constants/ComponentID.java \
	javax/jain/ss7/inap/constants/ComponentType.java \
	javax/jain/ss7/inap/constants/ConnectedPartyChoice.java \
	javax/jain/ss7/inap/constants/ControlType.java \
	javax/jain/ss7/inap/constants/ComponentIndPrimitiveType.java \
	javax/jain/ss7/inap/constants/DPAssignment.java \
	javax/jain/ss7/inap/constants/ComponentReqPrimitiveType.java \
	javax/jain/ss7/inap/constants/ElapsedTimeValueID.java \
	javax/jain/ss7/inap/constants/EntryChoice.java \
	javax/jain/ss7/inap/constants/ErrorCode.java \
	javax/jain/ss7/inap/constants/ErrorTreatment.java \
	javax/jain/ss7/inap/constants/CallProcessingOperationCorrelationID.java \
	javax/jain/ss7/inap/constants/EventTypeBCSM.java \
	javax/jain/ss7/inap/constants/ExtensionChoice.java \
	javax/jain/ss7/inap/constants/ExtensionType.java \
	javax/jain/ss7/inap/constants/FacilityGroupChoice.java \
	javax/jain/ss7/inap/constants/DpSpecificCriteriaChoice.java \
	javax/jain/ss7/inap/constants/FacilityGroupID.java \
	javax/jain/ss7/inap/constants/FeatureRequestIndicator.java \
	javax/jain/ss7/inap/constants/FilterCriteriaID.java \
	javax/jain/ss7/inap/constants/FilteringCharacteristicsChoice.java \
	javax/jain/ss7/inap/constants/ForwardingCondition.java \
	javax/jain/ss7/inap/constants/GapCriteriaChoice.java \
	javax/jain/ss7/inap/constants/GapTreatmentChoice.java \
	javax/jain/ss7/inap/constants/GeneralProblem.java \
	javax/jain/ss7/inap/constants/GroupID.java \
	javax/jain/ss7/inap/constants/FilteringCriteriaChoice.java \
	javax/jain/ss7/inap/constants/InfoToSendChoice.java \
	javax/jain/ss7/inap/constants/InvokeProblem.java \
	javax/jain/ss7/inap/constants/LegIDChoice.java \
	javax/jain/ss7/inap/constants/Media.java \
	javax/jain/ss7/inap/constants/MessageIDChoice.java \
	javax/jain/ss7/inap/constants/MessageType.java \
	javax/jain/ss7/inap/constants/MidCallReportType.java \
	javax/jain/ss7/inap/constants/MonitorMode.java \
	javax/jain/ss7/inap/constants/FilteringTimeOutChoice.java \
	javax/jain/ss7/inap/constants/NumberMatchChoice.java \
	javax/jain/ss7/inap/constants/OperationCode.java \
	javax/jain/ss7/inap/constants/OperationCodeChoice.java \
	javax/jain/ss7/inap/constants/PAbortReason.java \
	javax/jain/ss7/inap/constants/PartyToConnectChoice.java \
	javax/jain/ss7/inap/constants/InformationToSendChoice.java \
	javax/jain/ss7/inap/constants/NotificationInformationChoice.java \
	javax/jain/ss7/inap/constants/PartyToDisconnectChoice.java \
	javax/jain/ss7/inap/constants/ReceivedStatus.java \
	javax/jain/ss7/inap/constants/RejectType.java \
	javax/jain/ss7/inap/constants/ReportCause.java \
	javax/jain/ss7/inap/constants/ReportCondition.java \
	javax/jain/ss7/inap/constants/ProfileIdentifierChoice.java \
	javax/jain/ss7/inap/constants/ReceivedInformationChoice.java \
	javax/jain/ss7/inap/constants/RequestedInformationType.java \
	javax/jain/ss7/inap/constants/ResourceAddressID.java \
	javax/jain/ss7/inap/constants/ResourceIDChoice.java \
	javax/jain/ss7/inap/constants/ResourceStatus.java \
	javax/jain/ss7/inap/constants/ResponseCondition.java \
	javax/jain/ss7/inap/constants/ReturnErrorProblem.java \
	javax/jain/ss7/inap/constants/ReturnResultProblem.java \
	javax/jain/ss7/inap/constants/RoutingAddressChoice.java \
	javax/jain/ss7/inap/constants/SideID.java \
	javax/jain/ss7/inap/constants/RequestedInformationValueChoice.java \
	javax/jain/ss7/inap/constants/TerminalType.java \
	javax/jain/ss7/inap/constants/TimeOutError.java \
	javax/jain/ss7/inap/constants/TimerID.java \
	javax/jain/ss7/inap/constants/ToneIDChoice.java \
	javax/jain/ss7/inap/constants/ResourceAddressChoice.java \
	javax/jain/ss7/inap/constants/TriggerType.java \
	javax/jain/ss7/inap/constants/UAbortReason.java \
	javax/jain/ss7/inap/constants/USIMonitorMode.java \
	javax/jain/ss7/inap/constants/VariablePartChoice.java \
	javax/jain/ss7/inap/constants/VariablePartID.java \
	javax/jain/ss7/inap/constants/EventSpecificInformationBCSMChoice.java \
	javax/jain/ss7/inap/constants/TargetLineIdentifierChoice.java \
	javax/jain/ss7/inap/datatype/TransportAddress.java \
	javax/jain/ss7/inap/datatype/AddressAndService.java \
	javax/jain/ss7/inap/datatype/DigitsGenericNumber.java \
	javax/jain/ss7/inap/datatype/LocationNumber.java \
	javax/jain/ss7/inap/datatype/AllCallSegments.java \
	javax/jain/ss7/inap/datatype/Cause.java \
	javax/jain/ss7/inap/datatype/BackwardGVNS.java \
	javax/jain/ss7/inap/datatype/BCSMEvent.java \
	javax/jain/ss7/inap/datatype/LegID.java \
	javax/jain/ss7/inap/datatype/AssociatedCallSegment.java \
	javax/jain/ss7/inap/datatype/BackwardServiceInteractionInd.java \
	javax/jain/ss7/inap/datatype/DPSpecificCriteria.java \
	javax/jain/ss7/inap/datatype/MidCallControlInfo.java \
	javax/jain/ss7/inap/datatype/MidCallControlInformation.java \
	javax/jain/ss7/inap/datatype/MidCallInfoType.java \
	javax/jain/ss7/inap/datatype/DigitsGenericDigits.java \
	javax/jain/ss7/inap/datatype/BCSMFailure.java \
	javax/jain/ss7/inap/datatype/BearerCap.java \
	javax/jain/ss7/inap/datatype/Tone.java \
	javax/jain/ss7/inap/datatype/BearerCapability.java \
	javax/jain/ss7/inap/datatype/Text.java \
	javax/jain/ss7/inap/datatype/InformationToSend.java \
	javax/jain/ss7/inap/datatype/InbandInfo.java \
	javax/jain/ss7/inap/datatype/MessageID.java \
	javax/jain/ss7/inap/datatype/VariableMessage.java \
	javax/jain/ss7/inap/datatype/VariablePart.java \
	javax/jain/ss7/inap/datatype/CalledPartyNumber.java \
	javax/jain/ss7/inap/datatype/CallingPartyNumber.java \
	javax/jain/ss7/inap/datatype/CallSegment.java \
	javax/jain/ss7/inap/datatype/BothGapTreatmentParameters.java \
	javax/jain/ss7/inap/datatype/CallSegmentToCancel.java \
	javax/jain/ss7/inap/datatype/Carrier.java \
	javax/jain/ss7/inap/datatype/CalledAddressAndService.java \
	javax/jain/ss7/inap/datatype/CalledPartySubaddress.java \
	javax/jain/ss7/inap/datatype/CarrierID.java \
	javax/jain/ss7/inap/datatype/Entry.java \
	javax/jain/ss7/inap/datatype/CallingAddressAndService.java \
	javax/jain/ss7/inap/datatype/ChargingEvent.java \
	javax/jain/ss7/inap/datatype/CollectedDigits.java \
	javax/jain/ss7/inap/datatype/CollectedInfo.java \
	javax/jain/ss7/inap/datatype/CallingPartySubaddress.java \
	javax/jain/ss7/inap/datatype/Component.java \
	javax/jain/ss7/inap/datatype/ConnectedParty.java \
	javax/jain/ss7/inap/datatype/ControlDigits.java \
	javax/jain/ss7/inap/datatype/CounterAndValue.java \
	javax/jain/ss7/inap/datatype/CSFailure.java \
	javax/jain/ss7/inap/datatype/DestinationRoutingAddress.java \
	javax/jain/ss7/inap/datatype/ForwardGVNS.java \
	javax/jain/ss7/inap/datatype/ExtensionField.java \
	javax/jain/ss7/inap/datatype/MiscCallInfo.java \
	javax/jain/ss7/inap/datatype/Extension.java \
	javax/jain/ss7/inap/datatype/MidCallSpecificInfo.java \
	javax/jain/ss7/inap/datatype/FacilityGroup.java \
	javax/jain/ss7/inap/datatype/FilteringCriteria.java \
	javax/jain/ss7/inap/datatype/FilteringTimeOut.java \
	javax/jain/ss7/inap/datatype/GapCriteria.java \
	javax/jain/ss7/inap/datatype/GapOnService.java \
	javax/jain/ss7/inap/datatype/DisconnectSpecificInfo.java \
	javax/jain/ss7/inap/datatype/GapIndicators.java \
	javax/jain/ss7/inap/datatype/GapTreatment.java \
	javax/jain/ss7/inap/datatype/GenericNumbers.java \
	javax/jain/ss7/inap/datatype/DPSpecificCommonParameters.java \
	javax/jain/ss7/inap/datatype/ServiceAddressInformation.java \
	javax/jain/ss7/inap/datatype/GlobalTitle.java \
	javax/jain/ss7/inap/datatype/Leg.java \
	javax/jain/ss7/inap/datatype/ScfID.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0001.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0010.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0011.java \
	javax/jain/ss7/inap/datatype/INServiceCompatibilityIndication.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0100.java \
	javax/jain/ss7/inap/datatype/InformationToRecord.java \
	javax/jain/ss7/inap/datatype/IPAddressAndLegID.java \
	javax/jain/ss7/inap/datatype/ServiceInteractionIndicatorsTwo.java \
	javax/jain/ss7/inap/datatype/PartyToConnect.java \
	javax/jain/ss7/inap/datatype/PartyToDisconnect.java \
	javax/jain/ss7/inap/datatype/ProfileIdentifier.java \
	javax/jain/ss7/inap/datatype/RedirectingPartyID.java \
	javax/jain/ss7/inap/datatype/ForwardServiceInteractionInd.java \
	javax/jain/ss7/inap/datatype/RequestedInformation.java \
	javax/jain/ss7/inap/datatype/EventSpecificInformationBCSM.java \
	javax/jain/ss7/inap/datatype/FilteredCallTreatment.java \
	javax/jain/ss7/inap/datatype/FilteringCharacteristics.java \
	javax/jain/ss7/inap/datatype/RequestedUTSI.java \
	javax/jain/ss7/inap/datatype/ResourceAddress.java \
	javax/jain/ss7/inap/datatype/ResourceID.java \
	javax/jain/ss7/inap/datatype/RouteList.java \
	javax/jain/ss7/inap/datatype/ForwardCallIndicators.java \
	javax/jain/ss7/inap/datatype/HighLayerCompatibility.java \
	javax/jain/ss7/inap/datatype/IPAddressAndCallSegmentID.java \
	javax/jain/ss7/inap/datatype/OriginalCalledPartyID.java \
	javax/jain/ss7/inap/datatype/RedirectionInformation.java \
	javax/jain/ss7/inap/datatype/RequestedInformationValue.java \
	javax/jain/ss7/inap/datatype/RequestedInformationList.java \
	javax/jain/ss7/inap/datatype/RequestedInformationTypeList.java \
	javax/jain/ss7/inap/datatype/TriggerDataIdentifier.java \
	javax/jain/ss7/inap/event/DialogueIndEvent.java \
	javax/jain/ss7/inap/event/DialogueReqEvent.java \
	javax/jain/ss7/inap/event/BeginIndEvent.java \
	javax/jain/ss7/inap/event/BeginReqEvent.java \
	javax/jain/ss7/inap/event/ComponentIndEvent.java \
	javax/jain/ss7/inap/event/ComponentReqEvent.java \
	javax/jain/ss7/inap/event/ContinueIndEvent.java \
	javax/jain/ss7/inap/event/ContinueReqEvent.java \
	javax/jain/ss7/inap/event/EndIndEvent.java \
	javax/jain/ss7/inap/event/EndReqEvent.java \
	javax/jain/ss7/inap/event/InvokeIndEvent.java \
	javax/jain/ss7/inap/event/InvokeReqEvent.java \
	javax/jain/ss7/inap/event/LCancelIndEvent.java \
	javax/jain/ss7/inap/event/LRejectIndEvent.java \
	javax/jain/ss7/inap/event/NoticeIndEvent.java \
	javax/jain/ss7/inap/event/PAbortIndEvent.java \
	javax/jain/ss7/inap/event/ResultIndEvent.java \
	javax/jain/ss7/inap/event/ResultReqEvent.java \
	javax/jain/ss7/inap/event/RRejectIndEvent.java \
	javax/jain/ss7/inap/event/TimerResetReqEvent.java \
	javax/jain/ss7/inap/event/UAbortIndEvent.java \
	javax/jain/ss7/inap/event/UAbortReqEvent.java \
	javax/jain/ss7/inap/event/UCancelReqEvent.java \
	javax/jain/ss7/inap/event/UErrorIndEvent.java \
	javax/jain/ss7/inap/event/UErrorReqEvent.java \
	javax/jain/ss7/inap/event/URejectIndEvent.java \
	javax/jain/ss7/inap/event/URejectReqEvent.java \
	javax/jain/ss7/inap/exception/TooManyListenersRegisteredException.java \
	javax/jain/ss7/inap/exception/InvalidCallIDException.java \
	javax/jain/ss7/inap/exception/ParameterOutOfRangeException.java \
	javax/jain/ss7/inap/exception/InvalidApplicationContextException.java \
	javax/jain/ss7/inap/JainInapListener.java \
	javax/jain/ss7/inap/JainInapProvider.java \
	javax/jain/ss7/inap/JainInapStack.java \
	javax/jain/ss7/EncodingScheme.java \
	javax/jain/ss7/GlobalTitle.java \
	javax/jain/ss7/GTIndicator.java \
	javax/jain/ss7/GTIndicator0001.java \
	javax/jain/ss7/NAddressIndicator.java \
	javax/jain/ss7/NumberingPlan.java \
	javax/jain/ss7/GTIndicator0010.java \
	javax/jain/ss7/GTIndicator0011.java \
	javax/jain/ss7/GTIndicator0100.java \
	javax/jain/ss7/JainSS7Factory.java \
	javax/jain/ss7/JainSS7Utility.java \
	javax/jain/ss7/NetworkIndicator.java \
	javax/jain/ss7/RoutingIndicator.java \
	javax/jain/ss7/SccpUserAddress.java \
	javax/jain/ss7/SS7Address.java \
	javax/jain/ss7/SubSystemAddress.java \
	javax/jain/ss7/SignalingPointCode.java \
	javax/jain/ss7/SPCFormat.java \
	javax/jain/DeleteProviderException.java \
	javax/jain/JainException.java \
	javax/jain/IDNotAvailableException.java \
	javax/jain/InvalidAddressException.java \
	javax/jain/InvalidArgumentException.java \
	javax/jain/InvalidPathNameException.java \
	javax/jain/ListenerNotRegisteredException.java \
	javax/jain/MandatoryParameterNotSetException.java \
	javax/jain/ParameterNotSetException.java \
	javax/jain/PeerUnavailableException.java \
	javax/jain/ListenerAlreadyRegisteredException.java

my_javax_jain_ss7_inap_CLASSFILES = \
	javax/jain/ss7/inap/operation/Operation.class \
	javax/jain/ss7/inap/operation/ActivityTest.class \
	javax/jain/ss7/inap/operation/AnalysedInformation.class \
	javax/jain/ss7/inap/operation/ActivateServiceFiltering.class \
	javax/jain/ss7/inap/operation/CallGap.class \
	javax/jain/ss7/inap/operation/AnalyseInformation.class \
	javax/jain/ss7/inap/operation/ApplyCharging.class \
	javax/jain/ss7/inap/operation/ApplyChargingReport.class \
	javax/jain/ss7/inap/operation/Cancel.class \
	javax/jain/ss7/inap/operation/AssistRequestInstructions.class \
	javax/jain/ss7/inap/operation/AuthorizeTermination.class \
	javax/jain/ss7/inap/operation/Connect.class \
	javax/jain/ss7/inap/operation/CallInformationReport.class \
	javax/jain/ss7/inap/operation/CollectedInformation.class \
	javax/jain/ss7/inap/operation/CollectInformation.class \
	javax/jain/ss7/inap/operation/MoveLeg.class \
	javax/jain/ss7/inap/operation/ConnectToResource.class \
	javax/jain/ss7/inap/operation/Continue.class \
	javax/jain/ss7/inap/operation/ContinueWithArgument.class \
	javax/jain/ss7/inap/operation/CallInformationRequest.class \
	javax/jain/ss7/inap/operation/CancelStatusReportRequest.class \
	javax/jain/ss7/inap/operation/DFCWithArgument.class \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociation.class \
	javax/jain/ss7/inap/operation/DisconnectLeg.class \
	javax/jain/ss7/inap/operation/EntityReleased.class \
	javax/jain/ss7/inap/operation/DisconnectForwardConnection.class \
	javax/jain/ss7/inap/operation/EstablishTemporaryConnection.class \
	javax/jain/ss7/inap/operation/EventReportBCSM.class \
	javax/jain/ss7/inap/operation/EventReportFacility.class \
	javax/jain/ss7/inap/operation/EventNotificationCharging.class \
	javax/jain/ss7/inap/operation/FacilitySelectedAndAvailable.class \
	javax/jain/ss7/inap/operation/HoldCallInNetwork.class \
	javax/jain/ss7/inap/operation/InitialDP.class \
	javax/jain/ss7/inap/operation/InitiateCallAttempt.class \
	javax/jain/ss7/inap/operation/ManageTriggerData.class \
	javax/jain/ss7/inap/operation/FurnishChargingInformation.class \
	javax/jain/ss7/inap/operation/MergeCallSegments.class \
	javax/jain/ss7/inap/operation/MessageReceived.class \
	javax/jain/ss7/inap/operation/MoveCallSegments.class \
	javax/jain/ss7/inap/operation/OAnswer.class \
	javax/jain/ss7/inap/operation/OAbandon.class \
	javax/jain/ss7/inap/operation/TAnswer.class \
	javax/jain/ss7/inap/operation/OCalledPartyBusy.class \
	javax/jain/ss7/inap/operation/ODisconnect.class \
	javax/jain/ss7/inap/operation/OMidCall.class \
	javax/jain/ss7/inap/operation/ONoAnswer.class \
	javax/jain/ss7/inap/operation/OriginationAttempt.class \
	javax/jain/ss7/inap/operation/OSuspended.class \
	javax/jain/ss7/inap/operation/ManageTriggerDataResult.class \
	javax/jain/ss7/inap/operation/PlayAnnouncement.class \
	javax/jain/ss7/inap/operation/ReceivedInformation.class \
	javax/jain/ss7/inap/operation/OriginationAttemptAuthorized.class \
	javax/jain/ss7/inap/operation/PromptAndCollectUserInformation.class \
	javax/jain/ss7/inap/operation/Reconnect.class \
	javax/jain/ss7/inap/operation/ReleaseCall.class \
	javax/jain/ss7/inap/operation/ReportUTSI.class \
	javax/jain/ss7/inap/operation/RequestReportUTSI.class \
	javax/jain/ss7/inap/operation/PromptAndReceiveMessage.class \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReport.class \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReportResult.class \
	javax/jain/ss7/inap/operation/RequestEveryStatusChangeReport.class \
	javax/jain/ss7/inap/operation/RequestFirstStatusMatchReport.class \
	javax/jain/ss7/inap/operation/RequestNotificationChargingEvent.class \
	javax/jain/ss7/inap/operation/RequestReportBCSMEvent.class \
	javax/jain/ss7/inap/operation/ResetTimer.class \
	javax/jain/ss7/inap/operation/RouteSelectFailure.class \
	javax/jain/ss7/inap/operation/SelectFacility.class \
	javax/jain/ss7/inap/operation/SelectRoute.class \
	javax/jain/ss7/inap/operation/SendSTUI.class \
	javax/jain/ss7/inap/operation/RequestReportFacilityEvent.class \
	javax/jain/ss7/inap/operation/SendChargingInformation.class \
	javax/jain/ss7/inap/operation/SplitLeg.class \
	javax/jain/ss7/inap/operation/SendFacilityInformation.class \
	javax/jain/ss7/inap/operation/ServiceFilteringResponse.class \
	javax/jain/ss7/inap/operation/StatusReport.class \
	javax/jain/ss7/inap/operation/TBusy.class \
	javax/jain/ss7/inap/operation/TDisconnect.class \
	javax/jain/ss7/inap/operation/TerminationAttempt.class \
	javax/jain/ss7/inap/operation/SpecializedResourceReport.class \
	javax/jain/ss7/inap/operation/TMidCall.class \
	javax/jain/ss7/inap/operation/TNoAnswer.class \
	javax/jain/ss7/inap/operation/TSuspended.class \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociationResult.class \
	javax/jain/ss7/inap/operation/TerminationAttemptAuthorized.class \
	javax/jain/ss7/inap/constants/ActionIndicator.class \
	javax/jain/ss7/inap/constants/ActionPerformed.class \
	javax/jain/ss7/inap/constants/DialoguePrimitiveType.class \
	javax/jain/ss7/inap/constants/TransportAddressChoice.class \
	javax/jain/ss7/inap/constants/CancelChoice.class \
	javax/jain/ss7/inap/constants/CancelID.class \
	javax/jain/ss7/inap/constants/CGEncountered.class \
	javax/jain/ss7/inap/constants/CollectedInfoChoice.class \
	javax/jain/ss7/inap/constants/ComponentChoice.class \
	javax/jain/ss7/inap/constants/BearerCapabilityChoice.class \
	javax/jain/ss7/inap/constants/ComponentID.class \
	javax/jain/ss7/inap/constants/ComponentType.class \
	javax/jain/ss7/inap/constants/ConnectedPartyChoice.class \
	javax/jain/ss7/inap/constants/ControlType.class \
	javax/jain/ss7/inap/constants/ComponentIndPrimitiveType.class \
	javax/jain/ss7/inap/constants/DPAssignment.class \
	javax/jain/ss7/inap/constants/ComponentReqPrimitiveType.class \
	javax/jain/ss7/inap/constants/ElapsedTimeValueID.class \
	javax/jain/ss7/inap/constants/EntryChoice.class \
	javax/jain/ss7/inap/constants/ErrorCode.class \
	javax/jain/ss7/inap/constants/ErrorTreatment.class \
	javax/jain/ss7/inap/constants/CallProcessingOperationCorrelationID.class \
	javax/jain/ss7/inap/constants/EventTypeBCSM.class \
	javax/jain/ss7/inap/constants/ExtensionChoice.class \
	javax/jain/ss7/inap/constants/ExtensionType.class \
	javax/jain/ss7/inap/constants/FacilityGroupChoice.class \
	javax/jain/ss7/inap/constants/DpSpecificCriteriaChoice.class \
	javax/jain/ss7/inap/constants/FacilityGroupID.class \
	javax/jain/ss7/inap/constants/FeatureRequestIndicator.class \
	javax/jain/ss7/inap/constants/FilterCriteriaID.class \
	javax/jain/ss7/inap/constants/FilteringCharacteristicsChoice.class \
	javax/jain/ss7/inap/constants/ForwardingCondition.class \
	javax/jain/ss7/inap/constants/GapCriteriaChoice.class \
	javax/jain/ss7/inap/constants/GapTreatmentChoice.class \
	javax/jain/ss7/inap/constants/GeneralProblem.class \
	javax/jain/ss7/inap/constants/GroupID.class \
	javax/jain/ss7/inap/constants/FilteringCriteriaChoice.class \
	javax/jain/ss7/inap/constants/InfoToSendChoice.class \
	javax/jain/ss7/inap/constants/InvokeProblem.class \
	javax/jain/ss7/inap/constants/LegIDChoice.class \
	javax/jain/ss7/inap/constants/Media.class \
	javax/jain/ss7/inap/constants/MessageIDChoice.class \
	javax/jain/ss7/inap/constants/MessageType.class \
	javax/jain/ss7/inap/constants/MidCallReportType.class \
	javax/jain/ss7/inap/constants/MonitorMode.class \
	javax/jain/ss7/inap/constants/FilteringTimeOutChoice.class \
	javax/jain/ss7/inap/constants/NumberMatchChoice.class \
	javax/jain/ss7/inap/constants/OperationCode.class \
	javax/jain/ss7/inap/constants/OperationCodeChoice.class \
	javax/jain/ss7/inap/constants/PAbortReason.class \
	javax/jain/ss7/inap/constants/PartyToConnectChoice.class \
	javax/jain/ss7/inap/constants/InformationToSendChoice.class \
	javax/jain/ss7/inap/constants/NotificationInformationChoice.class \
	javax/jain/ss7/inap/constants/PartyToDisconnectChoice.class \
	javax/jain/ss7/inap/constants/ReceivedStatus.class \
	javax/jain/ss7/inap/constants/RejectType.class \
	javax/jain/ss7/inap/constants/ReportCause.class \
	javax/jain/ss7/inap/constants/ReportCondition.class \
	javax/jain/ss7/inap/constants/ProfileIdentifierChoice.class \
	javax/jain/ss7/inap/constants/ReceivedInformationChoice.class \
	javax/jain/ss7/inap/constants/RequestedInformationType.class \
	javax/jain/ss7/inap/constants/ResourceAddressID.class \
	javax/jain/ss7/inap/constants/ResourceIDChoice.class \
	javax/jain/ss7/inap/constants/ResourceStatus.class \
	javax/jain/ss7/inap/constants/ResponseCondition.class \
	javax/jain/ss7/inap/constants/ReturnErrorProblem.class \
	javax/jain/ss7/inap/constants/ReturnResultProblem.class \
	javax/jain/ss7/inap/constants/RoutingAddressChoice.class \
	javax/jain/ss7/inap/constants/SideID.class \
	javax/jain/ss7/inap/constants/RequestedInformationValueChoice.class \
	javax/jain/ss7/inap/constants/TerminalType.class \
	javax/jain/ss7/inap/constants/TimeOutError.class \
	javax/jain/ss7/inap/constants/TimerID.class \
	javax/jain/ss7/inap/constants/ToneIDChoice.class \
	javax/jain/ss7/inap/constants/ResourceAddressChoice.class \
	javax/jain/ss7/inap/constants/TriggerType.class \
	javax/jain/ss7/inap/constants/UAbortReason.class \
	javax/jain/ss7/inap/constants/USIMonitorMode.class \
	javax/jain/ss7/inap/constants/VariablePartChoice.class \
	javax/jain/ss7/inap/constants/VariablePartID.class \
	javax/jain/ss7/inap/constants/EventSpecificInformationBCSMChoice.class \
	javax/jain/ss7/inap/constants/TargetLineIdentifierChoice.class \
	javax/jain/ss7/inap/datatype/TransportAddress.class \
	javax/jain/ss7/inap/datatype/AddressAndService.class \
	javax/jain/ss7/inap/datatype/DigitsGenericNumber.class \
	javax/jain/ss7/inap/datatype/LocationNumber.class \
	javax/jain/ss7/inap/datatype/AllCallSegments.class \
	javax/jain/ss7/inap/datatype/Cause.class \
	javax/jain/ss7/inap/datatype/BackwardGVNS.class \
	javax/jain/ss7/inap/datatype/BCSMEvent.class \
	javax/jain/ss7/inap/datatype/LegID.class \
	javax/jain/ss7/inap/datatype/AssociatedCallSegment.class \
	javax/jain/ss7/inap/datatype/BackwardServiceInteractionInd.class \
	javax/jain/ss7/inap/datatype/DPSpecificCriteria.class \
	javax/jain/ss7/inap/datatype/MidCallControlInfo.class \
	javax/jain/ss7/inap/datatype/MidCallControlInformation.class \
	javax/jain/ss7/inap/datatype/MidCallInfoType.class \
	javax/jain/ss7/inap/datatype/DigitsGenericDigits.class \
	javax/jain/ss7/inap/datatype/BCSMFailure.class \
	javax/jain/ss7/inap/datatype/BearerCap.class \
	javax/jain/ss7/inap/datatype/Tone.class \
	javax/jain/ss7/inap/datatype/BearerCapability.class \
	javax/jain/ss7/inap/datatype/Text.class \
	javax/jain/ss7/inap/datatype/InformationToSend.class \
	javax/jain/ss7/inap/datatype/InbandInfo.class \
	javax/jain/ss7/inap/datatype/MessageID.class \
	javax/jain/ss7/inap/datatype/VariableMessage.class \
	javax/jain/ss7/inap/datatype/VariablePart.class \
	javax/jain/ss7/inap/datatype/CalledPartyNumber.class \
	javax/jain/ss7/inap/datatype/CallingPartyNumber.class \
	javax/jain/ss7/inap/datatype/CallSegment.class \
	javax/jain/ss7/inap/datatype/BothGapTreatmentParameters.class \
	javax/jain/ss7/inap/datatype/CallSegmentToCancel.class \
	javax/jain/ss7/inap/datatype/Carrier.class \
	javax/jain/ss7/inap/datatype/CalledAddressAndService.class \
	javax/jain/ss7/inap/datatype/CalledPartySubaddress.class \
	javax/jain/ss7/inap/datatype/CarrierID.class \
	javax/jain/ss7/inap/datatype/Entry.class \
	javax/jain/ss7/inap/datatype/CallingAddressAndService.class \
	javax/jain/ss7/inap/datatype/ChargingEvent.class \
	javax/jain/ss7/inap/datatype/CollectedDigits.class \
	javax/jain/ss7/inap/datatype/CollectedInfo.class \
	javax/jain/ss7/inap/datatype/CallingPartySubaddress.class \
	javax/jain/ss7/inap/datatype/Component.class \
	javax/jain/ss7/inap/datatype/ConnectedParty.class \
	javax/jain/ss7/inap/datatype/ControlDigits.class \
	javax/jain/ss7/inap/datatype/CounterAndValue.class \
	javax/jain/ss7/inap/datatype/CSFailure.class \
	javax/jain/ss7/inap/datatype/DestinationRoutingAddress.class \
	javax/jain/ss7/inap/datatype/ForwardGVNS.class \
	javax/jain/ss7/inap/datatype/ExtensionField.class \
	javax/jain/ss7/inap/datatype/MiscCallInfo.class \
	javax/jain/ss7/inap/datatype/Extension.class \
	javax/jain/ss7/inap/datatype/MidCallSpecificInfo.class \
	javax/jain/ss7/inap/datatype/FacilityGroup.class \
	javax/jain/ss7/inap/datatype/FilteringCriteria.class \
	javax/jain/ss7/inap/datatype/FilteringTimeOut.class \
	javax/jain/ss7/inap/datatype/GapCriteria.class \
	javax/jain/ss7/inap/datatype/GapOnService.class \
	javax/jain/ss7/inap/datatype/DisconnectSpecificInfo.class \
	javax/jain/ss7/inap/datatype/GapIndicators.class \
	javax/jain/ss7/inap/datatype/GapTreatment.class \
	javax/jain/ss7/inap/datatype/GenericNumbers.class \
	javax/jain/ss7/inap/datatype/DPSpecificCommonParameters.class \
	javax/jain/ss7/inap/datatype/ServiceAddressInformation.class \
	javax/jain/ss7/inap/datatype/GlobalTitle.class \
	javax/jain/ss7/inap/datatype/Leg.class \
	javax/jain/ss7/inap/datatype/ScfID.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0001.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0010.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0011.class \
	javax/jain/ss7/inap/datatype/INServiceCompatibilityIndication.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0100.class \
	javax/jain/ss7/inap/datatype/InformationToRecord.class \
	javax/jain/ss7/inap/datatype/IPAddressAndLegID.class \
	javax/jain/ss7/inap/datatype/ServiceInteractionIndicatorsTwo.class \
	javax/jain/ss7/inap/datatype/PartyToConnect.class \
	javax/jain/ss7/inap/datatype/PartyToDisconnect.class \
	javax/jain/ss7/inap/datatype/ProfileIdentifier.class \
	javax/jain/ss7/inap/datatype/RedirectingPartyID.class \
	javax/jain/ss7/inap/datatype/ForwardServiceInteractionInd.class \
	javax/jain/ss7/inap/datatype/RequestedInformation.class \
	javax/jain/ss7/inap/datatype/EventSpecificInformationBCSM.class \
	javax/jain/ss7/inap/datatype/FilteredCallTreatment.class \
	javax/jain/ss7/inap/datatype/FilteringCharacteristics.class \
	javax/jain/ss7/inap/datatype/RequestedUTSI.class \
	javax/jain/ss7/inap/datatype/ResourceAddress.class \
	javax/jain/ss7/inap/datatype/ResourceID.class \
	javax/jain/ss7/inap/datatype/RouteList.class \
	javax/jain/ss7/inap/datatype/ForwardCallIndicators.class \
	javax/jain/ss7/inap/datatype/HighLayerCompatibility.class \
	javax/jain/ss7/inap/datatype/IPAddressAndCallSegmentID.class \
	javax/jain/ss7/inap/datatype/OriginalCalledPartyID.class \
	javax/jain/ss7/inap/datatype/RedirectionInformation.class \
	javax/jain/ss7/inap/datatype/RequestedInformationValue.class \
	javax/jain/ss7/inap/datatype/RequestedInformationList.class \
	javax/jain/ss7/inap/datatype/RequestedInformationTypeList.class \
	javax/jain/ss7/inap/datatype/TriggerDataIdentifier.class \
	javax/jain/ss7/inap/event/DialogueIndEvent.class \
	javax/jain/ss7/inap/event/DialogueReqEvent.class \
	javax/jain/ss7/inap/event/BeginIndEvent.class \
	javax/jain/ss7/inap/event/BeginReqEvent.class \
	javax/jain/ss7/inap/event/ComponentIndEvent.class \
	javax/jain/ss7/inap/event/ComponentReqEvent.class \
	javax/jain/ss7/inap/event/ContinueIndEvent.class \
	javax/jain/ss7/inap/event/ContinueReqEvent.class \
	javax/jain/ss7/inap/event/EndIndEvent.class \
	javax/jain/ss7/inap/event/EndReqEvent.class \
	javax/jain/ss7/inap/event/InvokeIndEvent.class \
	javax/jain/ss7/inap/event/InvokeReqEvent.class \
	javax/jain/ss7/inap/event/LCancelIndEvent.class \
	javax/jain/ss7/inap/event/LRejectIndEvent.class \
	javax/jain/ss7/inap/event/NoticeIndEvent.class \
	javax/jain/ss7/inap/event/PAbortIndEvent.class \
	javax/jain/ss7/inap/event/ResultIndEvent.class \
	javax/jain/ss7/inap/event/ResultReqEvent.class \
	javax/jain/ss7/inap/event/RRejectIndEvent.class \
	javax/jain/ss7/inap/event/TimerResetReqEvent.class \
	javax/jain/ss7/inap/event/UAbortIndEvent.class \
	javax/jain/ss7/inap/event/UAbortReqEvent.class \
	javax/jain/ss7/inap/event/UCancelReqEvent.class \
	javax/jain/ss7/inap/event/UErrorIndEvent.class \
	javax/jain/ss7/inap/event/UErrorReqEvent.class \
	javax/jain/ss7/inap/event/URejectIndEvent.class \
	javax/jain/ss7/inap/event/URejectReqEvent.class \
	javax/jain/ss7/inap/exception/TooManyListenersRegisteredException.class \
	javax/jain/ss7/inap/exception/InvalidCallIDException.class \
	javax/jain/ss7/inap/exception/ParameterOutOfRangeException.class \
	javax/jain/ss7/inap/exception/InvalidApplicationContextException.class \
	javax/jain/ss7/inap/JainInapListener.class \
	javax/jain/ss7/inap/JainInapProvider.class \
	javax/jain/ss7/inap/JainInapStack.class \
	javax/jain/ss7/EncodingScheme.class \
	javax/jain/ss7/GlobalTitle.class \
	javax/jain/ss7/GTIndicator.class \
	javax/jain/ss7/GTIndicator0001.class \
	javax/jain/ss7/NAddressIndicator.class \
	javax/jain/ss7/NumberingPlan.class \
	javax/jain/ss7/GTIndicator0010.class \
	javax/jain/ss7/GTIndicator0011.class \
	javax/jain/ss7/GTIndicator0100.class \
	javax/jain/ss7/JainSS7Factory.class \
	javax/jain/ss7/JainSS7Utility.class \
	javax/jain/ss7/NetworkIndicator.class \
	javax/jain/ss7/RoutingIndicator.class \
	javax/jain/ss7/SccpUserAddress.class \
	javax/jain/ss7/SS7Address.class \
	javax/jain/ss7/SubSystemAddress.class \
	javax/jain/ss7/SignalingPointCode.class \
	javax/jain/ss7/SPCFormat.class \
	javax/jain/DeleteProviderException.class \
	javax/jain/JainException.class \
	javax/jain/IDNotAvailableException.class \
	javax/jain/InvalidAddressException.class \
	javax/jain/InvalidArgumentException.class \
	javax/jain/InvalidPathNameException.class \
	javax/jain/ListenerNotRegisteredException.class \
	javax/jain/MandatoryParameterNotSetException.class \
	javax/jain/ParameterNotSetException.class \
	javax/jain/PeerUnavailableException.class \
	javax/jain/ListenerAlreadyRegisteredException.class

MY_SECONDARY += $(my_javax_jain_ss7_inap_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_ss7_inap_CLASSFILES)
my_jar_LINKS += javax-jain-ss7-inap.jar
ALL_LOCAL += javax-jain-ss7-inap.jar
jar_DATA += javax-jain-ss7-inap-$(VERSION).jar

javax-jain-ss7-inap-$(VERSION).jar: $(my_javax_jain_ss7_inap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_javax_jain_ss7_inap_JAVAFILES)

clean-javax_jain_ss7_inap:
	rm -f -- $(my_javax_jain_ss7_inap_CLASSFILES)
	rm -f -- javax-jain-ss7-inap.jar javax-jain-ss7-inap-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-inap.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7_inap

## Build the whole jar into a library
lib_javax_jain_ss7_inap_jar_la_SOURCES	= javax-jain-ss7-inap.jar
lib_javax_jain_ss7_inap_jar_la_CPPFLAGS = $(GCJLIB_INCLUDES)
lib_javax_jain_ss7_inap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_javax_jain_ss7_inap_jar_la_LIBADD	=
lib_javax_jain_ss7_inap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_javax_jain_ss7_inap_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-inap.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7-inap.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7-inap.jar.db

lib-javax-jain-ss7-inap.jar.db: javax-jain-ss7-inap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_javax_jain_protocol_ss7_map_JAVAFILES = \
	javax/jain/JainEvent.java \
	javax/jain/JainException.java \
	javax/jain/protocol/ss7/E164Number.java \
	javax/jain/protocol/ss7/SS7ListenerNotRegisteredException.java \
	javax/jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.java \
	javax/jain/protocol/ss7/SS7PeerUnavailableException.java \
	javax/jain/protocol/ss7/SS7MandatoryParameterNotSetException.java \
	javax/jain/protocol/ss7/SS7ParameterNotSetException.java \
	javax/jain/protocol/ss7/JainSS7Factory.java \
	javax/jain/protocol/ss7/SS7Number.java \
	javax/jain/protocol/ss7/SS7Exception.java \
	javax/jain/protocol/ss7/SS7Parameter.java \
	javax/jain/protocol/ss7/map/Imsi.java \
	javax/jain/protocol/ss7/map/Position.java \
	javax/jain/protocol/ss7/map/session/SessionDataIndEvent.java \
	javax/jain/protocol/ss7/map/session/MessageSessionListener.java \
	javax/jain/protocol/ss7/map/session/SessionEvent.java \
	javax/jain/protocol/ss7/map/session/SessionDataReqEvent.java \
	javax/jain/protocol/ss7/map/session/etsipar/ServiceData.java \
	javax/jain/protocol/ss7/map/session/JainMapMessageSessionStack.java \
	javax/jain/protocol/ss7/map/session/SessionOpenReqEvent.java \
	javax/jain/protocol/ss7/map/session/MessageSessionProvider.java \
	javax/jain/protocol/ss7/map/session/SessionCloseReqEvent.java \
	javax/jain/protocol/ss7/map/session/SessionOpenIndEvent.java \
	javax/jain/protocol/ss7/map/MsNumber.java \
	javax/jain/protocol/ss7/map/MsIsdn.java \
	javax/jain/protocol/ss7/map/PositionPointAltitudeUncertEllipsoid.java \
	javax/jain/protocol/ss7/map/transaction/MtMessageConfEvent.java \
	javax/jain/protocol/ss7/map/transaction/MtMessageRecAvailRespEvent.java \
	javax/jain/protocol/ss7/map/transaction/Message.java \
	javax/jain/protocol/ss7/map/transaction/MessageTransactionProvider.java \
	javax/jain/protocol/ss7/map/transaction/MessageTransactionListener.java \
	javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStack.java \
	javax/jain/protocol/ss7/map/transaction/MoMessageIndEvent.java \
	javax/jain/protocol/ss7/map/transaction/MoMessageRespEvent.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/pdu/SmsPdu.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/SubmitReportMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/DeliverMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/DeliverReportMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/StatusReportMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/EtsiMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/SubmitMessage.java \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/CommandMessage.java \
	javax/jain/protocol/ss7/map/transaction/MtMessageRecAvailIndEvent.java \
	javax/jain/protocol/ss7/map/transaction/MtMessageReqEvent.java \
	javax/jain/protocol/ss7/map/transaction/TransactionEvent.java \
	javax/jain/protocol/ss7/map/JainMapStack.java \
	javax/jain/protocol/ss7/map/information/MsInfoReqEvent.java \
	javax/jain/protocol/ss7/map/information/InformationEvent.java \
	javax/jain/protocol/ss7/map/information/MsInfoProvider.java \
	javax/jain/protocol/ss7/map/information/JainMapMsInfoStack.java \
	javax/jain/protocol/ss7/map/information/etsipar/LsaId.java \
	javax/jain/protocol/ss7/map/information/etsipar/InfoRequestSpecifier.java \
	javax/jain/protocol/ss7/map/information/etsipar/LocationAreaId.java \
	javax/jain/protocol/ss7/map/information/etsipar/Location.java \
	javax/jain/protocol/ss7/map/information/etsipar/CellGlobalId.java \
	javax/jain/protocol/ss7/map/information/etsipar/LocationNumber.java \
	javax/jain/protocol/ss7/map/information/etsipar/ServiceAreaId.java \
	javax/jain/protocol/ss7/map/information/etsipar/VlrNumber.java \
	javax/jain/protocol/ss7/map/information/MsInfoConfEvent.java \
	javax/jain/protocol/ss7/map/information/MsInfoListener.java \
	javax/jain/protocol/ss7/map/PositionPoint.java \
	javax/jain/protocol/ss7/map/MapParameter.java \
	javax/jain/protocol/ss7/map/PositionArc.java \
	javax/jain/protocol/ss7/map/position/MsPosReqEvent.java \
	javax/jain/protocol/ss7/map/position/MsPosProvider.java \
	javax/jain/protocol/ss7/map/position/PositionEvent.java \
	javax/jain/protocol/ss7/map/position/MsPosIndEvent.java \
	javax/jain/protocol/ss7/map/position/JainMapMsPosStack.java \
	javax/jain/protocol/ss7/map/position/MsPosRespEvent.java \
	javax/jain/protocol/ss7/map/position/MsPosReportIndEvent.java \
	javax/jain/protocol/ss7/map/position/etsipar/LcsQos.java \
	javax/jain/protocol/ss7/map/position/etsipar/EquipmentId.java \
	javax/jain/protocol/ss7/map/position/etsipar/LcsClientId.java \
	javax/jain/protocol/ss7/map/position/etsipar/Imei.java \
	javax/jain/protocol/ss7/map/position/etsipar/PosRequestSpecifier.java \
	javax/jain/protocol/ss7/map/position/MsPosConfEvent.java \
	javax/jain/protocol/ss7/map/position/MsPosReportRespEvent.java \
	javax/jain/protocol/ss7/map/position/MsPosListener.java \
	javax/jain/protocol/ss7/map/position/PositionTargetId.java \
	javax/jain/protocol/ss7/map/PositionPointAltitude.java \
	javax/jain/protocol/ss7/map/Result.java \
	javax/jain/protocol/ss7/map/PositionPointUncertEllipse.java \
	javax/jain/protocol/ss7/map/PositionPointUncertCircle.java \
	javax/jain/protocol/ss7/map/MapErrorEvent.java \
	javax/jain/protocol/ss7/JainSS7Utility.java \
	javax/jain/protocol/ss7/SS7InvalidParamException.java \
	javax/jain/protocol/ss7/SS7Address.java

my_javax_jain_protocol_ss7_map_CLASSFILES = \
	javax/jain/JainEvent.class \
	javax/jain/JainException.class \
	javax/jain/protocol/ss7/E164Number.class \
	javax/jain/protocol/ss7/SS7ListenerNotRegisteredException.class \
	javax/jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.class \
	javax/jain/protocol/ss7/SS7PeerUnavailableException.class \
	javax/jain/protocol/ss7/SS7MandatoryParameterNotSetException.class \
	javax/jain/protocol/ss7/SS7ParameterNotSetException.class \
	javax/jain/protocol/ss7/JainSS7Factory.class \
	javax/jain/protocol/ss7/SS7Number.class \
	javax/jain/protocol/ss7/SS7Exception.class \
	javax/jain/protocol/ss7/SS7Parameter.class \
	javax/jain/protocol/ss7/map/Imsi.class \
	javax/jain/protocol/ss7/map/Position.class \
	javax/jain/protocol/ss7/map/session/SessionDataIndEvent.class \
	javax/jain/protocol/ss7/map/session/MessageSessionListener.class \
	javax/jain/protocol/ss7/map/session/SessionEvent.class \
	javax/jain/protocol/ss7/map/session/SessionDataReqEvent.class \
	javax/jain/protocol/ss7/map/session/etsipar/ServiceData.class \
	javax/jain/protocol/ss7/map/session/JainMapMessageSessionStack.class \
	javax/jain/protocol/ss7/map/session/SessionOpenReqEvent.class \
	javax/jain/protocol/ss7/map/session/MessageSessionProvider.class \
	javax/jain/protocol/ss7/map/session/SessionCloseReqEvent.class \
	javax/jain/protocol/ss7/map/session/SessionOpenIndEvent.class \
	javax/jain/protocol/ss7/map/MsNumber.class \
	javax/jain/protocol/ss7/map/MsIsdn.class \
	javax/jain/protocol/ss7/map/PositionPointAltitudeUncertEllipsoid.class \
	javax/jain/protocol/ss7/map/transaction/MtMessageConfEvent.class \
	javax/jain/protocol/ss7/map/transaction/MtMessageRecAvailRespEvent.class \
	javax/jain/protocol/ss7/map/transaction/Message.class \
	javax/jain/protocol/ss7/map/transaction/MessageTransactionProvider.class \
	javax/jain/protocol/ss7/map/transaction/MessageTransactionListener.class \
	javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStack.class \
	javax/jain/protocol/ss7/map/transaction/MoMessageIndEvent.class \
	javax/jain/protocol/ss7/map/transaction/MoMessageRespEvent.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/pdu/SmsPdu.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/SubmitReportMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/DeliverMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/DeliverReportMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/StatusReportMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/EtsiMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/SubmitMessage.class \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/CommandMessage.class \
	javax/jain/protocol/ss7/map/transaction/MtMessageRecAvailIndEvent.class \
	javax/jain/protocol/ss7/map/transaction/MtMessageReqEvent.class \
	javax/jain/protocol/ss7/map/transaction/TransactionEvent.class \
	javax/jain/protocol/ss7/map/JainMapStack.class \
	javax/jain/protocol/ss7/map/information/MsInfoReqEvent.class \
	javax/jain/protocol/ss7/map/information/InformationEvent.class \
	javax/jain/protocol/ss7/map/information/MsInfoProvider.class \
	javax/jain/protocol/ss7/map/information/JainMapMsInfoStack.class \
	javax/jain/protocol/ss7/map/information/etsipar/LsaId.class \
	javax/jain/protocol/ss7/map/information/etsipar/InfoRequestSpecifier.class \
	javax/jain/protocol/ss7/map/information/etsipar/LocationAreaId.class \
	javax/jain/protocol/ss7/map/information/etsipar/Location.class \
	javax/jain/protocol/ss7/map/information/etsipar/CellGlobalId.class \
	javax/jain/protocol/ss7/map/information/etsipar/LocationNumber.class \
	javax/jain/protocol/ss7/map/information/etsipar/ServiceAreaId.class \
	javax/jain/protocol/ss7/map/information/etsipar/VlrNumber.class \
	javax/jain/protocol/ss7/map/information/MsInfoConfEvent.class \
	javax/jain/protocol/ss7/map/information/MsInfoListener.class \
	javax/jain/protocol/ss7/map/PositionPoint.class \
	javax/jain/protocol/ss7/map/MapParameter.class \
	javax/jain/protocol/ss7/map/PositionArc.class \
	javax/jain/protocol/ss7/map/position/MsPosReqEvent.class \
	javax/jain/protocol/ss7/map/position/MsPosProvider.class \
	javax/jain/protocol/ss7/map/position/PositionEvent.class \
	javax/jain/protocol/ss7/map/position/MsPosIndEvent.class \
	javax/jain/protocol/ss7/map/position/JainMapMsPosStack.class \
	javax/jain/protocol/ss7/map/position/MsPosRespEvent.class \
	javax/jain/protocol/ss7/map/position/MsPosReportIndEvent.class \
	javax/jain/protocol/ss7/map/position/etsipar/LcsQos.class \
	javax/jain/protocol/ss7/map/position/etsipar/EquipmentId.class \
	javax/jain/protocol/ss7/map/position/etsipar/LcsClientId.class \
	javax/jain/protocol/ss7/map/position/etsipar/Imei.class \
	javax/jain/protocol/ss7/map/position/etsipar/PosRequestSpecifier.class \
	javax/jain/protocol/ss7/map/position/MsPosConfEvent.class \
	javax/jain/protocol/ss7/map/position/MsPosReportRespEvent.class \
	javax/jain/protocol/ss7/map/position/MsPosListener.class \
	javax/jain/protocol/ss7/map/position/PositionTargetId.class \
	javax/jain/protocol/ss7/map/PositionPointAltitude.class \
	javax/jain/protocol/ss7/map/Result.class \
	javax/jain/protocol/ss7/map/PositionPointUncertEllipse.class \
	javax/jain/protocol/ss7/map/PositionPointUncertCircle.class \
	javax/jain/protocol/ss7/map/MapErrorEvent.class \
	javax/jain/protocol/ss7/JainSS7Utility.class \
	javax/jain/protocol/ss7/SS7InvalidParamException.class \
	javax/jain/protocol/ss7/SS7Address.class

MY_SECONDARY += $(my_javax_jain_protocol_ss7_map_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_protocol_ss7_map_CLASSFILES)
my_jar_LINKS += javax-jain-protocol-ss7-map.jar
ALL_LOCAL += javax-jain-protocol-ss7-map.jar
jar_DATA += javax-jain-protocol-ss7-map-$(VERSION).jar

javax-jain-protocol-ss7-map-$(VERSION).jar: $(my_javax_jain_protocol_ss7_map_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_javax_jain_protocol_ss7_map_JAVAFILES)

clean-javax_jain_protocol_ss7_map:
	rm -f -- $(my_javax_jain_protocol_ss7_map_CLASSFILES)
	rm -f -- javax-jain-protocol-ss7-map.jar javax-jain-protocol-ss7-map-$(VERSION).jar
	rm -f -- lib-javax-jain-protocol-ss7-map.jar.db

CLEAN_LOCAL += clean-javax_jain_protocol_ss7_map

## Build the whole jar into a library
lib_javax_jain_protocol_ss7_map_jar_la_SOURCES	= javax-jain-protocol-ss7-map.jar
lib_javax_jain_protocol_ss7_map_jar_la_CPPFLAGS = $(GCJLIB_INCLUDES)
lib_javax_jain_protocol_ss7_map_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_javax_jain_protocol_ss7_map_jar_la_LIBADD	=
lib_javax_jain_protocol_ss7_map_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_javax_jain_protocol_ss7_map_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-javax-jain-protocol-ss7-map.jar.la
gcjlib_DATA		+= lib-javax-jain-protocol-ss7-map.jar.db
ALL_LOCAL		+= lib-javax-jain-protocol-ss7-map.jar.db

lib-javax-jain-protocol-ss7-map.jar.db: javax-jain-protocol-ss7-map.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_javax_csapi_cc_jcc_JAVAFILES = \
	javax/csapi/cc/jcc/CallLoadControlEvent.java \
	javax/csapi/cc/jcc/CallLoadControlListener.java \
	javax/csapi/cc/jcc/EventFilter.java \
	javax/csapi/cc/jcc/InvalidArgumentException.java \
	javax/csapi/cc/jcc/InvalidPartyException.java \
	javax/csapi/cc/jcc/InvalidStateException.java \
	javax/csapi/cc/jcc/JccAddress.java \
	javax/csapi/cc/jcc/JccCallEvent.java \
	javax/csapi/cc/jcc/JccCall.java \
	javax/csapi/cc/jcc/JccCallListener.java \
	javax/csapi/cc/jcc/JccConnectionEvent.java \
	javax/csapi/cc/jcc/JccConnection.java \
	javax/csapi/cc/jcc/JccConnectionListener.java \
	javax/csapi/cc/jcc/JccEvent.java \
	javax/csapi/cc/jcc/JccPeerFactory.java \
	javax/csapi/cc/jcc/JccPeer.java \
	javax/csapi/cc/jcc/JccProviderEvent.java \
	javax/csapi/cc/jcc/JccProvider.java \
	javax/csapi/cc/jcc/JccProviderListener.java \
	javax/csapi/cc/jcc/MethodNotSupportedException.java \
	javax/csapi/cc/jcc/MidCallData.java \
	javax/csapi/cc/jcc/PlatformException.java \
	javax/csapi/cc/jcc/PrivilegeViolationException.java \
	javax/csapi/cc/jcc/ProviderUnavailableException.java \
	javax/csapi/cc/jcc/ResourceUnavailableException.java

my_javax_csapi_cc_jcc_CLASSFILES = \
	javax/csapi/cc/jcc/CallLoadControlEvent.class \
	javax/csapi/cc/jcc/CallLoadControlListener.class \
	javax/csapi/cc/jcc/EventFilter.class \
	javax/csapi/cc/jcc/InvalidArgumentException.class \
	javax/csapi/cc/jcc/InvalidPartyException.class \
	javax/csapi/cc/jcc/InvalidStateException.class \
	javax/csapi/cc/jcc/JccAddress.class \
	javax/csapi/cc/jcc/JccCallEvent.class \
	javax/csapi/cc/jcc/JccCall.class \
	javax/csapi/cc/jcc/JccCallListener.class \
	javax/csapi/cc/jcc/JccConnectionEvent.class \
	javax/csapi/cc/jcc/JccConnection.class \
	javax/csapi/cc/jcc/JccConnectionListener.class \
	javax/csapi/cc/jcc/JccEvent.class \
	javax/csapi/cc/jcc/JccPeerFactory.class \
	javax/csapi/cc/jcc/JccPeer.class \
	javax/csapi/cc/jcc/JccProviderEvent.class \
	javax/csapi/cc/jcc/JccProvider.class \
	javax/csapi/cc/jcc/JccProviderListener.class \
	javax/csapi/cc/jcc/MethodNotSupportedException.class \
	javax/csapi/cc/jcc/MidCallData.class \
	javax/csapi/cc/jcc/PlatformException.class \
	javax/csapi/cc/jcc/PrivilegeViolationException.class \
	javax/csapi/cc/jcc/ProviderUnavailableException.class \
	javax/csapi/cc/jcc/ResourceUnavailableException.class

MY_SECONDARY += $(my_javax_csapi_cc_jcc_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_csapi_cc_jcc_CLASSFILES)
my_jar_LINKS += javax-csapi-cc-jcc.jar
ALL_LOCAL += javax-csapi-cc-jcc.jar
jar_DATA += javax-csapi-cc-jcc-$(VERSION).jar

javax-csapi-cc-jcc-$(VERSION).jar: $(my_javax_csapi_cc_jcc_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_javax_csapi_cc_jcc_JAVAFILES)

clean-javax_csapi_cc_jcc:
	rm -f -- $(my_javax_csapi_cc_jcc_CLASSFILES)
	rm -f -- javax-csapi-cc-jcc.jar javax-csapi-cc-jcc-$(VERSION).jar

CLEAN_LOCAL += clean-javax_csapi_cc_jcc

#############################################################################

my_javax_jcat_JAVAFILES = \
	javax/jcat/JcatAddressEvent.java \
	javax/jcat/JcatAddress.java \
	javax/jcat/JcatAddressListener.java \
	javax/jcat/JcatCallEvent.java \
	javax/jcat/JcatCall.java \
	javax/jcat/JcatConnectionEvent.java \
	javax/jcat/JcatConnection.java \
	javax/jcat/JcatConnectionListener.java \
	javax/jcat/JcatProvider.java \
	javax/jcat/JcatTerminalCapabilities.java \
	javax/jcat/JcatTerminalConnectionEvent.java \
	javax/jcat/JcatTerminalConnection.java \
	javax/jcat/JcatTerminalConnectionListener.java \
	javax/jcat/JcatTerminalEvent.java \
	javax/jcat/JcatTerminal.java \
	javax/jcat/JcatTerminalListener.java

my_javax_jcat_CLASSFILES = \
	javax/jcat/JcatAddressEvent.class \
	javax/jcat/JcatAddress.class \
	javax/jcat/JcatAddressListener.class \
	javax/jcat/JcatCallEvent.class \
	javax/jcat/JcatCall.class \
	javax/jcat/JcatConnectionEvent.class \
	javax/jcat/JcatConnection.class \
	javax/jcat/JcatConnectionListener.class \
	javax/jcat/JcatProvider.class \
	javax/jcat/JcatTerminalCapabilities.class \
	javax/jcat/JcatTerminalConnectionEvent.class \
	javax/jcat/JcatTerminalConnection.class \
	javax/jcat/JcatTerminalConnectionListener.class \
	javax/jcat/JcatTerminalEvent.class \
	javax/jcat/JcatTerminal.class \
	javax/jcat/JcatTerminalListener.class

MY_SECONDARY += $(my_javax_jcat_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jcat_CLASSFILES)
my_jar_LINKS += javax-jcat.jar
ALL_LOCAL += javax-jcat.jar
jar_DATA += javax-jcat-$(VERSION).jar

javax-jcat-$(VERSION).jar: $(my_javax_jcat_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_javax_jcat_JAVAFILES)

clean-javax_jcat:
	rm -f -- $(my_javax_jcat_CLASSFILES)
	rm -f -- javax-jcat.jar javax-jcat-$(VERSION).jar
	rm -f -- lib-javax-jcat.jar.db

CLEAN_LOCAL += clean-javax_jcat

## Build the whole jar into a library
lib_javax_jcat_jar_la_SOURCES	= javax-jcat.jar
lib_javax_jcat_jar_la_CPPFLAGS	= $(GCJLIB_INCLUDES)
lib_javax_jcat_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_javax_jcat_jar_la_LIBADD	=
lib_javax_jcat_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_javax_jcat_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-javax-jcat.jar.la
gcjlib_DATA		+= lib-javax-jcat.jar.db
ALL_LOCAL		+= lib-javax-jcat.jar.db

lib-javax-jcat.jar.db: javax-jcat.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_jain_JAVAFILES = \
	jain/CreateProviderException.java \
	jain/DeleteProviderException.java \
	jain/InvalidAddressException.java \
	jain/InvalidArgumentException.java \
	jain/InvalidPathNameException.java \
	jain/JainException.java \
	jain/ListenerAlreadyRegisteredException.java \
	jain/ListenerNotRegisteredException.java \
	jain/MandatoryParameterNotSetException.java \
	jain/MethodNotSupportedException.java \
	jain/ParameterNotSetException.java \
	jain/PeerUnavailableException.java \
	jain/PlatformException.java

my_jain_CLASSFILES = \
	jain/CreateProviderException.class \
	jain/DeleteProviderException.class \
	jain/InvalidAddressException.class \
	jain/InvalidArgumentException.class \
	jain/InvalidPathNameException.class \
	jain/JainException.class \
	jain/ListenerAlreadyRegisteredException.class \
	jain/ListenerNotRegisteredException.class \
	jain/MandatoryParameterNotSetException.class \
	jain/MethodNotSupportedException.class \
	jain/ParameterNotSetException.class \
	jain/PeerUnavailableException.class \
	jain/PlatformException.class

MY_SECONDARY += $(my_jain_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_CLASSFILES)
my_jar_LINKS += jain.jar
ALL_LOCAL += jain.jar
jar_DATA += jain-$(VERSION).jar

jain-$(VERSION).jar: $(my_jain_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_jain_JAVAFILES)

clean-jain:
	rm -f -- $(my_jain_CLASSFILES)
	rm -f -- jain.jar jain-$(VERSION).jar
	rm -f -- lib-jain.jar.db

CLEAN_LOCAL += clean-jain

## Build the whole jar into a library
lib_jain_jar_la_SOURCES		= jain.jar
lib_jain_jar_la_CPPFLAGS	= $(GCJLIB_INCLUDES)
lib_jain_jar_la_CFLAGS		= $(USER_CFLAGS)
lib_jain_jar_la_LIBADD		=
lib_jain_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_jain_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-jain.jar.la
gcjlib_DATA		+= lib-jain.jar.db
ALL_LOCAL		+= lib-jain.jar.db

lib-jain.jar.db: jain.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_jain_protocol_ss7_JAVAFILES = \
	jain/protocol/ss7/AddressConstants.java \
	jain/protocol/ss7/GlobalTitle.java \
	jain/protocol/ss7/GTIndicator0001.java \
	jain/protocol/ss7/GTIndicator0010.java \
	jain/protocol/ss7/GTIndicator0011.java \
	jain/protocol/ss7/GTIndicator0100.java \
	jain/protocol/ss7/IdNotAvailableException.java \
	jain/protocol/ss7/JainSS7Factory.java \
	jain/protocol/ss7/JainSS7Utility.java \
	jain/protocol/ss7/SccpUserAddress.java \
	jain/protocol/ss7/SignalingPointCode.java \
	jain/protocol/ss7/SS7Address.java \
	jain/protocol/ss7/SS7Exception.java \
	jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.java \
	jain/protocol/ss7/SS7ListenerNotRegisteredException.java \
	jain/protocol/ss7/SS7PeerUnavailableException.java \
	jain/protocol/ss7/SubSystemAddress.java \
	jain/protocol/ss7/UserAddressEmptyException.java \
	jain/protocol/ss7/UserAddressLimitException.java \
	jain/protocol/ss7/VersionNotSupportedException.java

my_jain_protocol_ss7_CLASSFILES = \
	jain/protocol/ss7/AddressConstants.class \
	jain/protocol/ss7/GlobalTitle.class \
	jain/protocol/ss7/GTIndicator0001.class \
	jain/protocol/ss7/GTIndicator0010.class \
	jain/protocol/ss7/GTIndicator0011.class \
	jain/protocol/ss7/GTIndicator0100.class \
	jain/protocol/ss7/IdNotAvailableException.class \
	jain/protocol/ss7/JainSS7Factory.class \
	jain/protocol/ss7/JainSS7Utility.class \
	jain/protocol/ss7/SccpUserAddress.class \
	jain/protocol/ss7/SignalingPointCode.class \
	jain/protocol/ss7/SS7Address.class \
	jain/protocol/ss7/SS7Exception.class \
	jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.class \
	jain/protocol/ss7/SS7ListenerNotRegisteredException.class \
	jain/protocol/ss7/SS7PeerUnavailableException.class \
	jain/protocol/ss7/SubSystemAddress.class \
	jain/protocol/ss7/UserAddressEmptyException.class \
	jain/protocol/ss7/UserAddressLimitException.class \
	jain/protocol/ss7/VersionNotSupportedException.class

MY_SECONDARY += $(my_jain_protocol_ss7_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_protocol_ss7_CLASSFILES)
my_jar_LINKS += jain-protocol-ss7.jar
ALL_LOCAL += jain-protocol-ss7.jar
jar_DATA += jain-protocol-ss7-$(VERSION).jar

jain-protocol-ss7-$(VERSION).jar: $(my_jain_protocol_ss7_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_jain_protocol_ss7_JAVAFILES)

clean-jain_protocol_ss7:
	rm -f -- $(my_jain_protocol_ss7_CLASSFILES)
	rm -f -- jain-protocol-ss7.jar jain-protocol-ss7-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7.jar.db

CLEAN_LOCAL += clean-jain_protocol_ss7

## Build the whole jar into a library
lib_jain_protocol_ss7_jar_la_SOURCES	= jain-protocol-ss7.jar
lib_jain_protocol_ss7_jar_la_CPPFLAGS	= $(GCJLIB_INCLUDES)
lib_jain_protocol_ss7_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_jain_protocol_ss7_jar_la_LIBADD	=
lib_jain_protocol_ss7_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_jain_protocol_ss7_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-jain-protocol-ss7.jar.la
gcjlib_DATA		+= lib-jain-protocol-ss7.jar.db
ALL_LOCAL		+= lib-jain-protocol-ss7.jar.db

lib-jain-protocol-ss7.jar.db: jain-protocol-ss7.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_jain_protocol_ss7_tcap_JAVAFILES = \
	jain/CreateProviderException.java \
	jain/DeleteProviderException.java \
	jain/InvalidAddressException.java \
	jain/InvalidArgumentException.java \
	jain/InvalidPathNameException.java \
	jain/JainException.java \
	jain/ListenerAlreadyRegisteredException.java \
	jain/ListenerNotRegisteredException.java \
	jain/MandatoryParameterNotSetException.java \
	jain/MethodNotSupportedException.java \
	jain/ParameterNotSetException.java \
	jain/PeerUnavailableException.java \
	jain/PlatformException.java \
	jain/protocol/ss7/AddressConstants.java \
	jain/protocol/ss7/GTIndicator0001.java \
	jain/protocol/ss7/GTIndicator0010.java \
	jain/protocol/ss7/GTIndicator0011.java \
	jain/protocol/ss7/GTIndicator0100.java \
	jain/protocol/ss7/GlobalTitle.java \
	jain/protocol/ss7/IdNotAvailableException.java \
	jain/protocol/ss7/JainSS7Factory.java \
	jain/protocol/ss7/JainSS7Utility.java \
	jain/protocol/ss7/SS7Address.java \
	jain/protocol/ss7/SS7Exception.java \
	jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.java \
	jain/protocol/ss7/SS7ListenerNotRegisteredException.java \
	jain/protocol/ss7/SS7PeerUnavailableException.java \
	jain/protocol/ss7/SccpUserAddress.java \
	jain/protocol/ss7/SignalingPointCode.java \
	jain/protocol/ss7/SubSystemAddress.java \
	jain/protocol/ss7/UserAddressEmptyException.java \
	jain/protocol/ss7/UserAddressLimitException.java \
	jain/protocol/ss7/VersionNotSupportedException.java \
	jain/protocol/ss7/tcap/ComponentIndEvent.java \
	jain/protocol/ss7/tcap/ComponentReqEvent.java \
	jain/protocol/ss7/tcap/DialogueIndEvent.java \
	jain/protocol/ss7/tcap/DialogueReqEvent.java \
	jain/protocol/ss7/tcap/GTIndicator0001.java \
	jain/protocol/ss7/tcap/GTIndicator0010.java \
	jain/protocol/ss7/tcap/GTIndicator0011.java \
	jain/protocol/ss7/tcap/GTIndicator0100.java \
	jain/protocol/ss7/tcap/GlobalTitle.java \
	jain/protocol/ss7/tcap/InvalidUserAddressException.java \
	jain/protocol/ss7/tcap/JainTcapListener.java \
	jain/protocol/ss7/tcap/JainTcapProvider.java \
	jain/protocol/ss7/tcap/JainTcapStack.java \
	jain/protocol/ss7/tcap/ProviderNotAttachedException.java \
	jain/protocol/ss7/tcap/TcapConstants.java \
	jain/protocol/ss7/tcap/TcapErrorEvent.java \
	jain/protocol/ss7/tcap/TcapException.java \
	jain/protocol/ss7/tcap/TcapUserAddress.java \
	jain/protocol/ss7/tcap/UnableToDeleteProviderException.java \
	jain/protocol/ss7/tcap/component/ComponentConstants.java \
	jain/protocol/ss7/tcap/component/ErrorIndEvent.java \
	jain/protocol/ss7/tcap/component/ErrorReqEvent.java \
	jain/protocol/ss7/tcap/component/InvokeIndEvent.java \
	jain/protocol/ss7/tcap/component/InvokeReqEvent.java \
	jain/protocol/ss7/tcap/component/LocalCancelIndEvent.java \
	jain/protocol/ss7/tcap/component/Operation.java \
	jain/protocol/ss7/tcap/component/Parameters.java \
	jain/protocol/ss7/tcap/component/RejectIndEvent.java \
	jain/protocol/ss7/tcap/component/RejectReqEvent.java \
	jain/protocol/ss7/tcap/component/ResultIndEvent.java \
	jain/protocol/ss7/tcap/component/ResultReqEvent.java \
	jain/protocol/ss7/tcap/component/TimerResetReqEvent.java \
	jain/protocol/ss7/tcap/component/UserCancelReqEvent.java \
	jain/protocol/ss7/tcap/dialogue/BeginIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/BeginReqEvent.java \
	jain/protocol/ss7/tcap/dialogue/ContinueIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/ContinueReqEvent.java \
	jain/protocol/ss7/tcap/dialogue/DialogueConstants.java \
	jain/protocol/ss7/tcap/dialogue/DialoguePortion.java \
	jain/protocol/ss7/tcap/dialogue/EndIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/EndReqEvent.java \
	jain/protocol/ss7/tcap/dialogue/NoticeIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/ProviderAbortIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/UnidirectionalIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/UnidirectionalReqEvent.java \
	jain/protocol/ss7/tcap/dialogue/UserAbortIndEvent.java \
	jain/protocol/ss7/tcap/dialogue/UserAbortReqEvent.java

my_jain_protocol_ss7_tcap_CLASSFILES = \
	jain/CreateProviderException.class \
	jain/DeleteProviderException.class \
	jain/InvalidAddressException.class \
	jain/InvalidArgumentException.class \
	jain/InvalidPathNameException.class \
	jain/JainException.class \
	jain/ListenerAlreadyRegisteredException.class \
	jain/ListenerNotRegisteredException.class \
	jain/MandatoryParameterNotSetException.class \
	jain/MethodNotSupportedException.class \
	jain/ParameterNotSetException.class \
	jain/PeerUnavailableException.class \
	jain/PlatformException.class \
	jain/protocol/ss7/AddressConstants.class \
	jain/protocol/ss7/GTIndicator0001.class \
	jain/protocol/ss7/GTIndicator0010.class \
	jain/protocol/ss7/GTIndicator0011.class \
	jain/protocol/ss7/GTIndicator0100.class \
	jain/protocol/ss7/GlobalTitle.class \
	jain/protocol/ss7/IdNotAvailableException.class \
	jain/protocol/ss7/JainSS7Factory.class \
	jain/protocol/ss7/JainSS7Utility.class \
	jain/protocol/ss7/SS7Address.class \
	jain/protocol/ss7/SS7Exception.class \
	jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.class \
	jain/protocol/ss7/SS7ListenerNotRegisteredException.class \
	jain/protocol/ss7/SS7PeerUnavailableException.class \
	jain/protocol/ss7/SccpUserAddress.class \
	jain/protocol/ss7/SignalingPointCode.class \
	jain/protocol/ss7/SubSystemAddress.class \
	jain/protocol/ss7/UserAddressEmptyException.class \
	jain/protocol/ss7/UserAddressLimitException.class \
	jain/protocol/ss7/VersionNotSupportedException.class \
	jain/protocol/ss7/tcap/ComponentIndEvent.class \
	jain/protocol/ss7/tcap/ComponentReqEvent.class \
	jain/protocol/ss7/tcap/DialogueIndEvent.class \
	jain/protocol/ss7/tcap/DialogueReqEvent.class \
	jain/protocol/ss7/tcap/GTIndicator0001.class \
	jain/protocol/ss7/tcap/GTIndicator0010.class \
	jain/protocol/ss7/tcap/GTIndicator0011.class \
	jain/protocol/ss7/tcap/GTIndicator0100.class \
	jain/protocol/ss7/tcap/GlobalTitle.class \
	jain/protocol/ss7/tcap/InvalidUserAddressException.class \
	jain/protocol/ss7/tcap/JainTcapListener.class \
	jain/protocol/ss7/tcap/JainTcapProvider.class \
	jain/protocol/ss7/tcap/JainTcapStack.class \
	jain/protocol/ss7/tcap/ProviderNotAttachedException.class \
	jain/protocol/ss7/tcap/TcapConstants.class \
	jain/protocol/ss7/tcap/TcapErrorEvent.class \
	jain/protocol/ss7/tcap/TcapException.class \
	jain/protocol/ss7/tcap/TcapUserAddress.class \
	jain/protocol/ss7/tcap/UnableToDeleteProviderException.class \
	jain/protocol/ss7/tcap/component/ComponentConstants.class \
	jain/protocol/ss7/tcap/component/ErrorIndEvent.class \
	jain/protocol/ss7/tcap/component/ErrorReqEvent.class \
	jain/protocol/ss7/tcap/component/InvokeIndEvent.class \
	jain/protocol/ss7/tcap/component/InvokeReqEvent.class \
	jain/protocol/ss7/tcap/component/LocalCancelIndEvent.class \
	jain/protocol/ss7/tcap/component/Operation.class \
	jain/protocol/ss7/tcap/component/Parameters.class \
	jain/protocol/ss7/tcap/component/RejectIndEvent.class \
	jain/protocol/ss7/tcap/component/RejectReqEvent.class \
	jain/protocol/ss7/tcap/component/ResultIndEvent.class \
	jain/protocol/ss7/tcap/component/ResultReqEvent.class \
	jain/protocol/ss7/tcap/component/TimerResetReqEvent.class \
	jain/protocol/ss7/tcap/component/UserCancelReqEvent.class \
	jain/protocol/ss7/tcap/dialogue/BeginIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/BeginReqEvent.class \
	jain/protocol/ss7/tcap/dialogue/ContinueIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/ContinueReqEvent.class \
	jain/protocol/ss7/tcap/dialogue/DialogueConstants.class \
	jain/protocol/ss7/tcap/dialogue/DialoguePortion.class \
	jain/protocol/ss7/tcap/dialogue/EndIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/EndReqEvent.class \
	jain/protocol/ss7/tcap/dialogue/NoticeIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/ProviderAbortIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/UnidirectionalIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/UnidirectionalReqEvent.class \
	jain/protocol/ss7/tcap/dialogue/UserAbortIndEvent.class \
	jain/protocol/ss7/tcap/dialogue/UserAbortReqEvent.class


MY_SECONDARY += $(my_jain_protocol_ss7_tcap_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_protocol_ss7_tcap_CLASSFILES)
my_jar_LINKS += jain-protocol-ss7-tcap.jar
ALL_LOCAL += jain-protocol-ss7-tcap.jar
jar_DATA += jain-protocol-ss7-tcap-$(VERSION).jar

jain-protocol-ss7-tcap-$(VERSION).jar: $(my_jain_protocol_ss7_tcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_jain_protocol_ss7_tcap_JAVAFILES)

clean-jain_protocol_ss7_tcap:
	rm -f -- $(my_jain_protocol_ss7_tcap_CLASSFILES)
	rm -f -- jain-protocol-ss7-tcap.jar jain-protocol-ss7-tcap-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7-tcap.jar.db

CLEAN_LOCAL += clean-jain_protocol_ss7_tcap

## Build the whole jar into a library
lib_jain_protocol_ss7_tcap_jar_la_SOURCES	= jain-protocol-ss7-tcap.jar
lib_jain_protocol_ss7_tcap_jar_la_CPPFLAGS	= $(GCJLIB_INCLUDES)
lib_jain_protocol_ss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_jain_protocol_ss7_tcap_jar_la_LIBADD	=
lib_jain_protocol_ss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_jain_protocol_ss7_tcap_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-jain-protocol-ss7-tcap.jar.la
gcjlib_DATA		+= lib-jain-protocol-ss7-tcap.jar.db
ALL_LOCAL		+= lib-jain-protocol-ss7-tcap.jar.db

lib-jain-protocol-ss7-tcap.jar.db: jain-protocol-ss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

#############################################################################

my_jain_protocol_ss7_oam_JAVAFILES = \
	jain/protocol/ss7/oam/CommitException.java \
	jain/protocol/ss7/oam/IllegalOperationException.java \
	jain/protocol/ss7/oam/mtp2/Mtp2AlarmNotification.java \
	jain/protocol/ss7/oam/mtp2/Mtp2ErrorNotification.java \
	jain/protocol/ss7/oam/mtp2/Mtp2LayerManagerMBean.java \
	jain/protocol/ss7/oam/mtp2/Mtp2ManagedObjectMBean.java \
	jain/protocol/ss7/oam/mtp2/Mtp2SapMBean.java \
	jain/protocol/ss7/oam/mtp2/Mtp2StatisticNotification.java \
	jain/protocol/ss7/oam/mtp2/Mtp2TimerProfileMBean.java \
	jain/protocol/ss7/oam/mtp3/LinkMBean.java \
	jain/protocol/ss7/oam/mtp3/LinkSetMBean.java \
	jain/protocol/ss7/oam/mtp3/Mtp3AlarmNotification.java \
	jain/protocol/ss7/oam/mtp3/Mtp3ErrorNotification.java \
	jain/protocol/ss7/oam/mtp3/Mtp3LayerManagerMBean.java \
	jain/protocol/ss7/oam/mtp3/Mtp3ManagedObjectMBean.java \
	jain/protocol/ss7/oam/mtp3/Mtp3SapMBean.java \
	jain/protocol/ss7/oam/mtp3/Mtp3StatisticNotification.java \
	jain/protocol/ss7/oam/mtp3/Mtp3TimerProfileMBean.java \
	jain/protocol/ss7/oam/mtp3/OwnSignallingPointMBean.java \
	jain/protocol/ss7/oam/mtp3/RouteMBean.java \
	jain/protocol/ss7/oam/mtp3/RouteSetMBean.java \
	jain/protocol/ss7/oam/mtp3/ScreeningTableMBean.java \
	jain/protocol/ss7/oam/mtp3/SignallingPointMBean.java \
	jain/protocol/ss7/oam/OamAlarmNotification.java \
	jain/protocol/ss7/oam/OamErrorNotification.java \
	jain/protocol/ss7/oam/OamLayerManagerMBean.java \
	jain/protocol/ss7/oam/OamLookupTable.java \
	jain/protocol/ss7/oam/OamManagedObjectMBean.java \
	jain/protocol/ss7/oam/OamStatisticNotification.java \
	jain/protocol/ss7/oam/OamTimerProfileMBean.java \
	jain/protocol/ss7/oam/OamTimerValMBean.java \
	jain/protocol/ss7/oam/ReadConfigurationException.java \
	jain/protocol/ss7/oam/sccp/ConcernedAreaMBean.java \
	jain/protocol/ss7/oam/sccp/GlobalTitleEntryMBean.java \
	jain/protocol/ss7/oam/sccp/SccpAlarmNotification.java \
	jain/protocol/ss7/oam/sccp/SccpEntitySetMBean.java \
	jain/protocol/ss7/oam/sccp/SccpErrorNotification.java \
	jain/protocol/ss7/oam/sccp/SccpLayerManagerMBean.java \
	jain/protocol/ss7/oam/sccp/SccpManagedObjectMBean.java \
	jain/protocol/ss7/oam/sccp/SccpRoutingControlMBean.java \
	jain/protocol/ss7/oam/sccp/SccpSapMBean.java \
	jain/protocol/ss7/oam/sccp/SccpStatisticNotification.java \
	jain/protocol/ss7/oam/sccp/SccpTimerProfileMBean.java \
	jain/protocol/ss7/oam/StatisticNotSupportedException.java \
	jain/protocol/ss7/oam/tcap/TcapAlarmNotification.java \
	jain/protocol/ss7/oam/tcap/TcapErrorNotification.java \
	jain/protocol/ss7/oam/tcap/TcapLayerManagerMBean.java \
	jain/protocol/ss7/oam/tcap/TcapManagedObjectMBean.java \
	jain/protocol/ss7/oam/tcap/TcapNodeMBean.java \
	jain/protocol/ss7/oam/tcap/TcapStatisticNotification.java \
	jain/protocol/ss7/oam/TooManyInstancesException.java

my_jain_protocol_ss7_oam_CLASSFILES = \
	jain/protocol/ss7/oam/CommitException.class \
	jain/protocol/ss7/oam/IllegalOperationException.class \
	jain/protocol/ss7/oam/mtp2/Mtp2AlarmNotification.class \
	jain/protocol/ss7/oam/mtp2/Mtp2ErrorNotification.class \
	jain/protocol/ss7/oam/mtp2/Mtp2LayerManagerMBean.class \
	jain/protocol/ss7/oam/mtp2/Mtp2ManagedObjectMBean.class \
	jain/protocol/ss7/oam/mtp2/Mtp2SapMBean.class \
	jain/protocol/ss7/oam/mtp2/Mtp2StatisticNotification.class \
	jain/protocol/ss7/oam/mtp2/Mtp2TimerProfileMBean.class \
	jain/protocol/ss7/oam/mtp3/LinkMBean.class \
	jain/protocol/ss7/oam/mtp3/LinkSetMBean.class \
	jain/protocol/ss7/oam/mtp3/Mtp3AlarmNotification.class \
	jain/protocol/ss7/oam/mtp3/Mtp3ErrorNotification.class \
	jain/protocol/ss7/oam/mtp3/Mtp3LayerManagerMBean.class \
	jain/protocol/ss7/oam/mtp3/Mtp3ManagedObjectMBean.class \
	jain/protocol/ss7/oam/mtp3/Mtp3SapMBean.class \
	jain/protocol/ss7/oam/mtp3/Mtp3StatisticNotification.class \
	jain/protocol/ss7/oam/mtp3/Mtp3TimerProfileMBean.class \
	jain/protocol/ss7/oam/mtp3/OwnSignallingPointMBean.class \
	jain/protocol/ss7/oam/mtp3/RouteMBean.class \
	jain/protocol/ss7/oam/mtp3/RouteSetMBean.class \
	jain/protocol/ss7/oam/mtp3/ScreeningTableMBean.class \
	jain/protocol/ss7/oam/mtp3/SignallingPointMBean.class \
	jain/protocol/ss7/oam/OamAlarmNotification.class \
	jain/protocol/ss7/oam/OamErrorNotification.class \
	jain/protocol/ss7/oam/OamLayerManagerMBean.class \
	jain/protocol/ss7/oam/OamLookupTable.class \
	jain/protocol/ss7/oam/OamManagedObjectMBean.class \
	jain/protocol/ss7/oam/OamStatisticNotification.class \
	jain/protocol/ss7/oam/OamTimerProfileMBean.class \
	jain/protocol/ss7/oam/OamTimerValMBean.class \
	jain/protocol/ss7/oam/ReadConfigurationException.class \
	jain/protocol/ss7/oam/sccp/ConcernedAreaMBean.class \
	jain/protocol/ss7/oam/sccp/GlobalTitleEntryMBean.class \
	jain/protocol/ss7/oam/sccp/SccpAlarmNotification.class \
	jain/protocol/ss7/oam/sccp/SccpEntitySetMBean.class \
	jain/protocol/ss7/oam/sccp/SccpErrorNotification.class \
	jain/protocol/ss7/oam/sccp/SccpLayerManagerMBean.class \
	jain/protocol/ss7/oam/sccp/SccpManagedObjectMBean.class \
	jain/protocol/ss7/oam/sccp/SccpRoutingControlMBean.class \
	jain/protocol/ss7/oam/sccp/SccpSapMBean.class \
	jain/protocol/ss7/oam/sccp/SccpStatisticNotification.class \
	jain/protocol/ss7/oam/sccp/SccpTimerProfileMBean.class \
	jain/protocol/ss7/oam/StatisticNotSupportedException.class \
	jain/protocol/ss7/oam/tcap/TcapAlarmNotification.class \
	jain/protocol/ss7/oam/tcap/TcapErrorNotification.class \
	jain/protocol/ss7/oam/tcap/TcapLayerManagerMBean.class \
	jain/protocol/ss7/oam/tcap/TcapManagedObjectMBean.class \
	jain/protocol/ss7/oam/tcap/TcapNodeMBean.class \
	jain/protocol/ss7/oam/tcap/TcapStatisticNotification.class \
	jain/protocol/ss7/oam/TooManyInstancesException.class

MY_SECONDARY += $(my_jain_protocol_ss7_oam_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_protocol_ss7_oam_CLASSFILES)
my_jar_LINKS += jain-protocol-ss7-oam.jar
ALL_LOCAL += jain-protocol-ss7-oam.jar
jar_DATA += jain-protocol-ss7-oam-$(VERSION).jar

jain-protocol-ss7-oam-$(VERSION).jar: $(my_jain_protocol_ss7_oam_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_JAVAFILES += $(my_jain_protocol_ss7_oam_JAVAFILES)

clean-jain_protocol_ss7_oam:
	rm -f -- $(my_jain_protocol_ss7_oam_CLASSFILES)
	rm -f -- jain-protocol-ss7-oam.jar jain-protocol-ss7-oam-$(VERSION).jar

CLEAN_LOCAL += clean-jain_protocol_ss7_oam

#############################################################################


#############################################################################
# These are the CNI modules
#############################################################################

my_oam_JAVAFILES = \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2LayerManager.java \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2Sap.java \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2TimerProfile.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/Link.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/LinkSet.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3LayerManager.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3Sap.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3TimerProfile.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/OwnSignallingPoint.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/Route.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/RouteSet.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/ScreeningTable.java \
	org/openss7/jain/protocol/ss7/oam/mtp3/SignallingPoint.java \
	org/openss7/jain/protocol/ss7/oam/sccp/ConcernedArea.java \
	org/openss7/jain/protocol/ss7/oam/sccp/GlobalTitleEntry.java \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpEntitySet.java \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpLayerManager.java \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpRoutingControl.java \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpSap.java \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpTimerProfile.java \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapLayerManager.java \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapNode.java
my_oam_CLASSFILES = \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2LayerManager.class \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2Sap.class \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2TimerProfile.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/Link.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/LinkSet.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3LayerManager.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3Sap.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3TimerProfile.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/OwnSignallingPoint.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/Route.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/RouteSet.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/ScreeningTable.class \
	org/openss7/jain/protocol/ss7/oam/mtp3/SignallingPoint.class \
	org/openss7/jain/protocol/ss7/oam/sccp/ConcernedArea.class \
	org/openss7/jain/protocol/ss7/oam/sccp/GlobalTitleEntry.class \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpEntitySet.class \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpLayerManager.class \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpRoutingControl.class \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpSap.class \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpTimerProfile.class \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapLayerManager.class \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapNode.class
my_oam_SOURCEFILES = \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2LayerManager.cc \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2Sap.cc \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2TimerProfile.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/Link.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/LinkSet.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3LayerManager.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3Sap.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3TimerProfile.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/OwnSignallingPoint.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/Route.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/RouteSet.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/ScreeningTable.cc \
	org/openss7/jain/protocol/ss7/oam/mtp3/SignallingPoint.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/ConcernedArea.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/GlobalTitleEntry.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpEntitySet.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpLayerManager.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpRoutingControl.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpSap.cc \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpTimerProfile.cc \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapLayerManager.cc \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapNode.cc
my_oam_INCLUDES = \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2LayerManager.h \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2Sap.h \
	org/openss7/jain/protocol/ss7/oam/mtp2/Mtp2TimerProfile.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/Link.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/LinkSet.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3LayerManager.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3Sap.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/Mtp3TimerProfile.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/OwnSignallingPoint.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/Route.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/RouteSet.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/ScreeningTable.h \
	org/openss7/jain/protocol/ss7/oam/mtp3/SignallingPoint.h \
	org/openss7/jain/protocol/ss7/oam/sccp/ConcernedArea.h \
	org/openss7/jain/protocol/ss7/oam/sccp/GlobalTitleEntry.h \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpEntitySet.h \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpLayerManager.h \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpRoutingControl.h \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpSap.h \
	org/openss7/jain/protocol/ss7/oam/sccp/SccpTimerProfile.h \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapLayerManager.h \
	org/openss7/jain/protocol/ss7/oam/tcap/TcapNode.h
my_oam_HEADERCLASSES =

lib_org_openss7_jain_protocol_ss7_oam_jar_la_SOURCES 	= org-openss7-jain-protocol-ss7-oam.jar $(my_oam_INCLUDES) $(my_oam_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_oam_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_jain_protocol_ss7_oam_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_jain_protocol_ss7_oam_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_oam_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_jain_protocol_ss7_oam_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-oam.jar.la
gcjlib_DATA		+= lib-org-openss7-jain-protocol-ss7-oam.jar.db
ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-oam.jar.db

lib-org-openss7-jain-protocol-ss7-oam.jar.db: org-openss7-jain-protocol-ss7-oam.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_oam_INCLUDES): %.h : %.class
$(my_oam_SOURCEFILES): $(my_oam_INCLUDES)

#MY_PRECIOUS += $(my_oam_SOURCEFILES)

MY_INTERMEDIATE += $(my_oam_INCLUDES) $(my_oam_HEADERCLASSES)

my_jar_LINKS += org-openss7-jain-protocol-ss7-oam.jar
ALL_LOCAL += org-openss7-jain-protocol-ss7-oam.jar
jar_DATA += org-openss7-jain-protocol-ss7-oam-$(VERSION).jar

org-openss7-jain-protocol-ss7-oam-$(VERSION).jar: $(my_oam_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_oam_CLASSFILES)
my_jar_CLASSFILES += $(my_oam_CLASSFILES)

my_JAVAFILES += $(my_oam_JAVAFILES)

clean-org_openss7_jain_protocol_ss7_oam:
	rm -f -- $(my_oam_CLASSFILES)
	rm -f -- $(my_oam_INCLUDES)
	rm -f -- $(my_oam_HEADERCLASSES)
	rm -f -- org-openss7-jain-protocol-ss7-oam.jar org-openss7-jain-protocol-ss7-oam-$(VERSION).jar
	rm -f -- lib-org-openss7-jain-protocol-ss7-oam.jar.db

CLEAN_LOCAL += clean-org_openss7_jain_protocol_ss7_oam

#############################################################################

my_jtcap_JAVAFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.java \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.java
my_jtcap_CLASSFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.class \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.class
my_jtcap_SOURCEFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.cc \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.cc
my_jtcap_INCLUDES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.h \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.h
my_jtcap_HEADERCLASSES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.class \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.class

lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES 	= org-openss7-jain-protocol-ss7-tcap.jar $(my_jtcap_INCLUDES) $(my_jtcap_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-tcap.jar.la
gcjlib_DATA		+= lib-org-openss7-jain-protocol-ss7-tcap.jar.db
ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-tcap.jar.db

lib-org-openss7-jain-protocol-ss7-tcap.jar.db: org-openss7-jain-protocol-ss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jtcap_INCLUDES): %.h : %.class
$(my_jtcap_SOURCEFILES): $(my_jtcap_INCLUDES)

#MY_PRECIOUS += $(my_jtcap_SOURCEFILES)

MY_INTERMEDIATE += $(my_jtcap_INCLUDES) $(my_jtcap_HEADERCLASSES)

my_jar_LINKS += org-openss7-jain-protocol-ss7-tcap.jar
ALL_LOCAL += org-openss7-jain-protocol-ss7-tcap.jar
jar_DATA += org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar

org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar: $(my_jtcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jtcap_CLASSFILES)
my_jar_CLASSFILES += $(my_jtcap_CLASSFILES)

my_JAVAFILES += $(my_jtcap_JAVAFILES)

clean-org_openss7_jain_protocol_ss7_tcap:
	rm -f -- $(my_jtcap_CLASSFILES)
	rm -f -- $(my_jtcap_INCLUDES)
	rm -f -- $(my_jtcap_HEADERCLASSES)
	rm -f -- org-openss7-jain-protocol-ss7-tcap.jar org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar
	rm -f -- lib-org-openss7-jain-protocol-ss7-tcap.jar.db

CLEAN_LOCAL += clean-org_openss7_jain_protocol_ss7_tcap

#############################################################################

my_jsip_JAVAFILES = \
	org/openss7/javax/sip/SipProviderImpl.java \
	org/openss7/javax/sip/SipStackImpl.java
my_jsip_CLASSFILES = \
	org/openss7/javax/sip/SipProviderImpl.class \
	org/openss7/javax/sip/SipStackImpl.class
my_jsip_SOURCEFILES = \
	org/openss7/javax/sip/SipProviderImpl.cc \
	org/openss7/javax/sip/SipStackImpl.cc
my_jsip_INCLUDES = \
	org/openss7/javax/sip/SipProviderImpl.h \
	org/openss7/javax/sip/SipStackImpl.h
my_jsip_HEADERCLASSES = \
	org/openss7/javax/sip/SipProviderImpl.class \
	org/openss7/javax/sip/SipStackImpl.class

lib_org_openss7_javax_sip_jar_la_SOURCES 	= org-openss7-javax-sip.jar $(my_jsip_INCLUDES) $(my_jsip_SOURCEFILES)
lib_org_openss7_javax_sip_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_sip_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_sip_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_sip_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_sip_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-sip.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-sip.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-sip.jar.db

lib-org-openss7-javax-sip.jar.db: org-openss7-javax-sip.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jsip_INCLUDES): %.h : %.class
$(my_jsip_SOURCEFILES): $(my_jsip_INCLUDES)

#MY_PRECIOUS += $(my_jsip_SOURCEFILES)

MY_INTERMEDIATE += $(my_jsip_INCLUDES) $(my_jsip_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-sip.jar
ALL_LOCAL += org-openss7-javax-sip.jar
jar_DATA += org-openss7-javax-sip-$(VERSION).jar

org-openss7-javax-sip-$(VERSION).jar: $(my_jsip_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jsip_CLASSFILES)
my_jar_CLASSFILES += $(my_jsip_CLASSFILES)

my_JAVAFILES += $(my_jsip_JAVAFILES)

clean-org_openss7_javax_sip:
	rm -f -- $(my_jsip_CLASSFILES)
	rm -f -- $(my_jsip_INCLUDES)
	rm -f -- $(my_jsip_HEADERCLASSES)
	rm -f -- org-openss7-javax-sip.jar org-openss7-javax-sip-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-sip.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_sip

#############################################################################

my_jcat_JAVAFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.java
my_jcat_CLASSFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.class
my_jcat_SOURCEFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.cc
my_jcat_INCLUDES = \
	org/openss7/javax/jcat/JcatProviderImpl.h \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.h \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.h
my_jcat_HEADERCLASSES = \
	org/openss7/javax/jcat/JcatProviderImpl.class \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.class \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.class

lib_org_openss7_javax_jcat_jar_la_SOURCES 	= org-openss7-javax-jcat.jar $(my_jcat_INCLUDES) $(my_jcat_SOURCEFILES)
lib_org_openss7_javax_jcat_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jcat_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jcat_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jcat_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jcat_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jcat.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jcat.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jcat.jar.db

lib-org-openss7-javax-jcat.jar.db: org-openss7-javax-jcat.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jcat_INCLUDES): %.h : %.class
$(my_jcat_SOURCEFILES): $(my_jcat_INCLUDES)

#MY_PRECIOUS += $(my_jcat_SOURCEFILES)

MY_INTERMEDIATE += $(my_jcat_INCLUDES) $(my_jcat_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-jcat.jar
ALL_LOCAL += org-openss7-javax-jcat.jar
jar_DATA += org-openss7-javax-jcat-$(VERSION).jar

org-openss7-javax-jcat-$(VERSION).jar: $(my_jcat_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jcat_CLASSFILES)
my_jar_CLASSFILES += $(my_jcat_CLASSFILES)

my_JAVAFILES += $(my_jcat_JAVAFILES)

clean-org_openss7_javax_jcat:
	rm -f -- $(my_jcat_CLASSFILES)
	rm -f -- $(my_jcat_INCLUDES)
	rm -f -- $(my_jcat_HEADERCLASSES)
	rm -f -- org-openss7-javax-jcat.jar org-openss7-javax-jcat-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jcat.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_jcat

#############################################################################

my_jisup_JAVAFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.java \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.java
my_jisup_CLASSFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.class \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.class
my_jisup_SOURCEFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.cc \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.cc
my_jisup_INCLUDES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.h \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.h

lib_org_openss7_javax_jain_ss7_isup_jar_la_SOURCES 	= org-openss7-javax-jain-ss7-isup.jar $(my_jisup_INCLUDES) $(my_jisup_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_isup_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_isup_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_ss7_isup_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_isup_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_isup_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-isup.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-ss7-isup.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-isup.jar.db

lib-org-openss7-javax-jain-ss7-isup.jar.db: org-openss7-javax-jain-ss7-isup.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jisup_INCLUDES): %.h : %.class
$(my_jisup_SOURCEFILES): $(my_jisup_INCLUDES)

#MY_PRECIOUS += $(my_jisup_SOURCEFILES)

MY_INTERMEDIATE += $(my_jisup_INCLUDES) $(my_jisup_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-jain-ss7-isup.jar
ALL_LOCAL += org-openss7-javax-jain-ss7-isup.jar
jar_DATA += org-openss7-javax-jain-ss7-isup-$(VERSION).jar

org-openss7-javax-jain-ss7-isup-$(VERSION).jar: $(my_jisup_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jisup_CLASSFILES)
my_jar_CLASSFILES += $(my_jisup_CLASSFILES)

my_JAVAFILES += $(my_jisup_JAVAFILES)

clean-org_openss7_javax_jain_ss7_isup:
	rm -f -- $(my_jisup_CLASSFILES)
	rm -f -- $(my_jisup_INCLUDES)
	rm -f -- $(my_jisup_HEADERCLASSES)
	rm -f -- org-openss7-javax-jain-ss7-isup.jar org-openss7-javax-jain-ss7-isup-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-ss7-isup.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_ss7_isup

#############################################################################

my_jinap_JAVAFILES = \
	org/openss7/javax/jain/ss7/inap/JainInapProviderImpl.java \
	org/openss7/javax/jain/ss7/inap/JainInapStackImpl.java
my_jinap_CLASSFILES = \
	org/openss7/javax/jain/ss7/inap/JainInapProviderImpl.class \
	org/openss7/javax/jain/ss7/inap/JainInapStackImpl.class
my_jinap_SOURCEFILES = \
	org/openss7/javax/jain/ss7/inap/JainInapProviderImpl.cc \
	org/openss7/javax/jain/ss7/inap/JainInapStackImpl.cc
my_jinap_INCLUDES = \
	org/openss7/javax/jain/ss7/inap/JainInapProviderImpl.h \
	org/openss7/javax/jain/ss7/inap/JainInapStackImpl.h
my_jinap_HEADERCLASSES = \
	org/openss7/javax/jain/ss7/inap/JainInapProviderImpl.class \
	org/openss7/javax/jain/ss7/inap/JainInapStackImpl.class

lib_org_openss7_javax_jain_ss7_inap_jar_la_SOURCES 	= org-openss7-javax-jain-ss7-inap.jar $(my_jinap_INCLUDES) $(my_jinap_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_inap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_inap_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_ss7_inap_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_inap_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_inap_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-inap.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-ss7-inap.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-inap.jar.db

lib-org-openss7-javax-jain-ss7-inap.jar.db: org-openss7-javax-jain-ss7-inap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jinap_INCLUDES): %.h : %.class
$(my_jinap_SOURCEFILES): $(my_jinap_INCLUDES)

#MY_PRECIOUS += $(my_jinap_SOURCEFILES)

MY_INTERMEDIATE += $(my_jinap_INCLUDES) $(my_jinap_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-jain-ss7-inap.jar
ALL_LOCAL += org-openss7-javax-jain-ss7-inap.jar
jar_DATA += org-openss7-javax-jain-ss7-inap-$(VERSION).jar

org-openss7-javax-jain-ss7-inap-$(VERSION).jar: $(my_jinap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jinap_CLASSFILES)
my_jar_CLASSFILES += $(my_jinap_CLASSFILES)

my_JAVAFILES += $(my_jinap_JAVAFILES)

clean-org_openss7_javax_jain_ss7_inap:
	rm -f -- $(my_jinap_CLASSFILES)
	rm -f -- $(my_jinap_INCLUDES)
	rm -f -- $(my_jinap_HEADERCLASSES)
	rm -f -- org-openss7-javax-jain-ss7-inap.jar org-openss7-javax-jain-ss7-inap-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-ss7-inap.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_ss7_inap

#############################################################################

my_jmap_JAVAFILES = \
	org/openss7/javax/jain/protocol/ss7/map/JainMapStackImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/information/JainMapMsInfoStackImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/information/MsInfoProviderImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/position/JainMapMsPosStackImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/position/MsPosProviderImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/session/JainMapMessageSessionStackImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/session/MessageSessionProviderImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStackImpl.java \
	org/openss7/javax/jain/protocol/ss7/map/transaction/MessageTransactionProviderImpl.java
my_jmap_CLASSFILES = \
	org/openss7/javax/jain/protocol/ss7/map/JainMapStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/information/JainMapMsInfoStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/information/MsInfoProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/position/JainMapMsPosStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/position/MsPosProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/session/JainMapMessageSessionStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/session/MessageSessionProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/transaction/MessageTransactionProviderImpl.class
my_jmap_SOURCEFILES = \
	org/openss7/javax/jain/protocol/ss7/map/JainMapStackImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/information/JainMapMsInfoStackImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/information/MsInfoProviderImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/position/JainMapMsPosStackImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/position/MsPosProviderImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/session/JainMapMessageSessionStackImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/session/MessageSessionProviderImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStackImpl.cc \
	org/openss7/javax/jain/protocol/ss7/map/transaction/MessageTransactionProviderImpl.cc
my_jmap_INCLUDES = \
	org/openss7/javax/jain/protocol/ss7/map/JainMapStackImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/information/JainMapMsInfoStackImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/information/MsInfoProviderImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/position/JainMapMsPosStackImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/position/MsPosProviderImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/session/JainMapMessageSessionStackImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/session/MessageSessionProviderImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStackImpl.h \
	org/openss7/javax/jain/protocol/ss7/map/transaction/MessageTransactionProviderImpl.h
my_jmap_HEADERCLASSES = \
	org/openss7/javax/jain/protocol/ss7/map/JainMapStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/information/JainMapMsInfoStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/information/MsInfoProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/position/JainMapMsPosStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/position/MsPosProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/session/JainMapMessageSessionStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/session/MessageSessionProviderImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/transaction/JainMapMessageTransactionStackImpl.class \
	org/openss7/javax/jain/protocol/ss7/map/transaction/MessageTransactionProviderImpl.class

lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_SOURCES 	= org-openss7-javax-jain-protocol-ss7-map.jar $(my_jmap_INCLUDES) $(my_jmap_SOURCEFILES)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.db

lib-org-openss7-javax-jain-protocol-ss7-map.jar.db: org-openss7-javax-jain-protocol-ss7-map.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jmap_INCLUDES): %.h : %.class
$(my_jmap_SOURCEFILES): $(my_jmap_INCLUDES)

#MY_PRECIOUS += $(my_jmap_SOURCEFILES)

MY_INTERMEDIATE += $(my_jmap_INCLUDES) $(my_jmap_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-jain-protocol-ss7-map.jar
ALL_LOCAL += org-openss7-javax-jain-protocol-ss7-map.jar
jar_DATA += org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar

org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar: $(my_jmap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jmap_CLASSFILES)
my_jar_CLASSFILES += $(my_jmap_CLASSFILES)

my_JAVAFILES += $(my_jmap_JAVAFILES)

clean-org_openss7_javax_jain_protocol_ss7_map:
	rm -f -- $(my_jmap_CLASSFILES)
	rm -f -- $(my_jmap_INCLUDES)
	rm -f -- $(my_jmap_HEADERCLASSES)
	rm -f -- org-openss7-javax-jain-protocol-ss7-map.jar org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-protocol-ss7-map.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_protocol_ss7_map

#############################################################################

my_jcc_JAVAFILES = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.java \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.java
my_jcc_CLASSFILES = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.class \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.class
my_jcc_SOURCEFILES = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.cc \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.cc
my_jcc_INCLUDES = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.h \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.h
my_jcc_HEADERCLASSES = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.class \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.class

lib_org_openss7_javax_csapi_cc_jcc_jar_la_SOURCES 	= org-openss7-javax-csapi-cc-jcc.jar $(my_jcc_INCLUDES) $(my_jcc_SOURCEFILES)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_csapi_cc_jcc_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-csapi-cc-jcc.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-csapi-cc-jcc.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-csapi-cc-jcc.jar.db

lib-org-openss7-javax-csapi-cc-jcc.jar.db: org-openss7-javax-csapi-cc-jcc.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

$(my_jcc_INCLUDES): %.h : %.class
$(my_jcc_SOURCEFILES): $(my_jcc_INCLUDES)

#MY_PRECIOUS += $(my_jcc_SOURCEFILES)

MY_INTERMEDIATE += $(my_jcc_INCLUDES) $(my_jcc_HEADERCLASSES)

my_jar_LINKS += org-openss7-javax-csapi-cc-jcc.jar
ALL_LOCAL += org-openss7-javax-csapi-cc-jcc.jar
jar_DATA += org-openss7-javax-csapi-cc-jcc-$(VERSION).jar

org-openss7-javax-csapi-cc-jcc-$(VERSION).jar: $(my_jcc_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_jcc_CLASSFILES)
my_jar_CLASSFILES += $(my_jcc_CLASSFILES)

my_JAVAFILES += $(my_jcc_JAVAFILES)

clean-org_openss7_javax_csapi_cc_jcc:
	rm -f -- $(my_jcc_CLASSFILES)
	rm -f -- $(my_jcc_INCLUDES)
	rm -f -- $(my_jcc_HEADERCLASSES)
	rm -f -- org-openss7-javax-csapi-cc-jcc.jar org-openss7-javax-csapi-cc-jcc-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-csapi-cc-jcc.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_csapi_cc_jcc

#############################################################################
# These are the SWIG JNI modules
#############################################################################

# this is where the JNI libraries go
jnilibdir = $(libdir)/java
jnilib_LTLIBRARIES =

SWIG = swig

my_c_WRAPPERS =

# this is how we use SWIG to generate headers and wrapper files for C

$(my_c_WRAPPERS): %_wrap.c: %.i
	$(AM_V_at)$(MKDIR_P) $$(dirname $@)
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	$(SWIG) -java -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -package `echo $$f | sed -e 's,^//*,,;s,/[^/]*$$,,;s,/,.,g'` -o $@ $<

# this is how we use SWIG to generate headers and wrapper files for C++

my_cxx_WRAPPERS =

$(my_cxx_WRAPPERS): %_wrap.cxx : %.i %.hh
	$(AM_V_at)$(MKDIR_P) $$(dirname $@)
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	$(SWIG) -java -c++ -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -package `echo $$f | sed -e 's,^//*,,;s,/[^/]*$$,,;s,/,.,g'` -o $@ $<

#############################################################################
# This is how we build STREAMS JNI classes for Java.
#############################################################################

my_streams_JAVAFILES = \
	org/openss7/streams/bandinfo.java \
	org/openss7/streams/strbuf.java \
	org/openss7/streams/StreamsConstants.java \
	org/openss7/streams/Streams.java \
	org/openss7/streams/StreamsJNI.java \
	org/openss7/streams/strfdinsert.java \
	org/openss7/streams/strioctl.java \
	org/openss7/streams/str_list.java \
	org/openss7/streams/str_mlist.java \
	org/openss7/streams/strpeek.java \
	org/openss7/streams/strpmsg.java

my_streams_CLASSFILES = \
	org/openss7/streams/bandinfo.class \
	org/openss7/streams/strbuf.class \
	org/openss7/streams/StreamsConstants.class \
	org/openss7/streams/Streams.class \
	org/openss7/streams/StreamsJNI.class \
	org/openss7/streams/strfdinsert.class \
	org/openss7/streams/strioctl.class \
	org/openss7/streams/str_list.class \
	org/openss7/streams/str_mlist.class \
	org/openss7/streams/strpeek.class \
	org/openss7/streams/strpmsg.class
my_streams_SOURCEFILES = \
	org/openss7/streams/streams_wrap.c

lib_org_openss7_streams_jar_la_SOURCES	= org-openss7-streams.jar $(my_streams_SOURCEFILES)
lib_org_openss7_streams_jar_la_CPPFLAGS = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_streams_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_streams_jar_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_streams_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_streams_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

EXTRA_LTLIBRARIES 	+= lib-org-openss7-streams.jar.la
EXTRA_DATA		+= lib-org-openss7-streams.jar.db
#gcjlib_LTLIBRARIES 	+= lib-org-openss7-streams.jar.la
#gcjlib_DATA		+= lib-org-openss7-streams.jar.db
#ALL_LOCAL		+= lib-org-openss7-streams.jar.db

lib-org-openss7-streams.jar.db: org-openss7-streams.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

my_jar_LINKS += org-openss7-streams.jar
ALL_LOCAL += org-openss7-streams.jar
jar_DATA += org-openss7-streams-$(VERSION).jar

org-openss7-streams-$(VERSION).jar: $(my_streams_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_streams_CLASSFILES)
my_jar_CLASSFILES += $(my_streams_CLASSFILES)

libopenss7StreamsJNI_la_SOURCES 	= $(my_streams_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7StreamsJNI_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
libopenss7StreamsJNI_la_CFLAGS		= $(USER_CFLAGS)
libopenss7StreamsJNI_la_LIBADD		= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7StreamsJNI_la_LDFLAGS		= -module -avoid-version -shared

#jnilib_LTLIBRARIES += libopenss7StreamsJNI.la
EXTRA_LTLIBRARIES += libopenss7StreamsJNI.la

my_c_WRAPPERS += org/openss7/streams/streams_wrap.c
my_cxx_WRAPPERS += org/openss7/streams/streams_wrap.cxx
MY_INTERMEDIATE += org/openss7/streams/streams_wrap.c
MY_INTERMEDIATE += org/openss7/streams/streams_wrap.cxx
MY_INTERMEDIATE += $(my_streams_JAVAFILES)
MY_SECONDARY += $(my_streams_CLASSFILES)
$(my_streams_JAVAFILES): $(my_streams_SOURCEFILES)

my_JAVAFILES += $(my_streams_JAVAFILES)

clean-org_openss7_streams:
	rm -f -- $(my_streams_JAVAFILES)
	rm -f -- $(my_streams_CLASSFILES)
	rm -f -- org/openss7/streams/streams_wrap.c
	rm -f -- org/openss7/streams/streams_wrap.cxx
	rm -f -- org-openss7-streams.jar org-openss7-streams-$(VERSION).jar
	rm -f -- lib-org-openss7-streams.jar.db

CLEAN_LOCAL += clean-org_openss7_streams

#############################################################################
# This is how we build XTI JNI classes for Java.
#############################################################################

my_xti_JAVAFILES = \
	org/openss7/xti/netbuf.java \
	org/openss7/xti/rate.java \
	org/openss7/xti/reqvalue.java \
	org/openss7/xti/t_ap_pc_item.java \
	org/openss7/xti/t_ap_pco_el.java \
	org/openss7/xti/t_ap_syn_off.java \
	org/openss7/xti/t_bind.java \
	org/openss7/xti/t_call.java \
	org/openss7/xti/t_discon.java \
	org/openss7/xti/thrpt.java \
	org/openss7/xti/t_info.java \
	org/openss7/xti/t_iovec.java \
	org/openss7/xti/t_leaf_status.java \
	org/openss7/xti/t_mosiaddr.java \
	org/openss7/xti/t_optmgmt.java \
	org/openss7/xti/transdel.java \
	org/openss7/xti/t_uderr.java \
	org/openss7/xti/t_unitdata.java \
	org/openss7/xti/XtiConstants.java \
	org/openss7/xti/Xti.java \
	org/openss7/xti/XtiJNI.java

my_xti_CLASSFILES = \
	org/openss7/xti/netbuf.class \
	org/openss7/xti/rate.class \
	org/openss7/xti/reqvalue.class \
	org/openss7/xti/t_ap_pc_item.class \
	org/openss7/xti/t_ap_pco_el.class \
	org/openss7/xti/t_ap_syn_off.class \
	org/openss7/xti/t_bind.class \
	org/openss7/xti/t_call.class \
	org/openss7/xti/t_discon.class \
	org/openss7/xti/thrpt.class \
	org/openss7/xti/t_info.class \
	org/openss7/xti/t_iovec.class \
	org/openss7/xti/t_leaf_status.class \
	org/openss7/xti/t_mosiaddr.class \
	org/openss7/xti/t_optmgmt.class \
	org/openss7/xti/transdel.class \
	org/openss7/xti/t_uderr.class \
	org/openss7/xti/t_unitdata.class \
	org/openss7/xti/XtiConstants.class \
	org/openss7/xti/Xti.class \
	org/openss7/xti/XtiJNI.class

my_xti_SOURCEFILES = \
	org/openss7/xti/xti_wrap.c

lib_org_openss7_xti_jar_la_SOURCES 	= org-openss7-xti.jar $(my_xti_SOURCEFILES)
lib_org_openss7_xti_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xti_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xti_jar_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_xti_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_xti_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

gcjlib_LTLIBRARIES 	+= lib-org-openss7-xti.jar.la
gcjlib_DATA		+= lib-org-openss7-xti.jar.db
ALL_LOCAL		+= lib-org-openss7-xti.jar.db

lib-org-openss7-xti.jar.db: org-openss7-xti.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

my_jar_LINKS += org-openss7-xti.jar
ALL_LOCAL += org-openss7-xti.jar
jar_DATA += org-openss7-xti-$(VERSION).jar

org-openss7-xti-$(VERSION).jar: $(my_tcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_tcap_CLASSFILES)
my_jar_CLASSFILES += $(my_tcap_CLASSFILES)

libopenss7XtiJNI_la_SOURCES 		= $(my_xti_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtiJNI_la_CPPFLAGS 		= $(GCJLIB_INCLUDES) -fno-strict-aliasing
libopenss7XtiJNI_la_CFLAGS		= $(USER_CFLAGS)
libopenss7XtiJNI_la_LIBADD		= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtiJNI_la_LDFLAGS		= -module -avoid-version -shared

jnilib_LTLIBRARIES += libopenss7XtiJNI.la

my_c_WRAPPERS += org/openss7/xti/xti_wrap.c
my_cxx_WRAPPERS += org/openss7/xti/xti_wrap.cxx
MY_INTERMEDIATE += org/openss7/xti/xti_wrap.c
MY_INTERMEDIATE += org/openss7/xti/xti_wrap.cxx
MY_INTERMEDIATE += $(my_xti_JAVAFILES)
MY_SECONDARY += $(my_xti_CLASSFILES)
$(my_xti_JAVAFILES): $(my_xti_SOURCEFILES)

my_JAVAFILES += $(my_xti_JAVAFILES)

clean-org_openss7_xti:
	rm -f -- $(my_xti_JAVAFILES)
	rm -f -- $(my_xti_CLASSFILES)
	rm -f -- org/openss7/xti/xti_wrap.c
	rm -f -- org/openss7/xti/xti_wrap.cxx
	rm -f -- org-openss7-xti.jar org-openss7-xti-$(VERSION).jar
	rm -f -- lib-org-openss7-xti.jar.db

CLEAN_LOCAL += clean-org_openss7_xti

#############################################################################
# This is how we build TCAP JNI classes for Java.
#############################################################################

my_tcap_JAVAFILES = \
	org/openss7/tcap/SccpAddress.java \
	org/openss7/tcap/sccp_addr_t.java \
	org/openss7/tcap/SWIGTYPE_p_listTTcapOptions_R_t.java \
	org/openss7/tcap/SWIGTYPE_p_listTtci__TcapOption_R_t.java \
	org/openss7/tcap/TcapAbort.java \
	org/openss7/tcap/TcapAccept.java \
	org/openss7/tcap/TcapAllow.java \
	org/openss7/tcap/TcapBegin.java \
	org/openss7/tcap/TcapCancel.java \
	org/openss7/tcap/TcapComponentEvent.java \
	org/openss7/tcap/TcapCont.java \
	org/openss7/tcap/TcapCoord.java \
	org/openss7/tcap/TcapData.java \
	org/openss7/tcap/TcapDialogueEvent.java \
	org/openss7/tcap/TcapEnd.java \
	org/openss7/tcap/TcapEndPoint.java \
	org/openss7/tcap/TcapError.java \
	org/openss7/tcap/TcapEvent.java \
	org/openss7/tcap/TcapInvoke.java \
	org/openss7/tcap/Tcap.java \
	org/openss7/tcap/TcapJNI.java \
	org/openss7/tcap/TcapManagement.java \
	org/openss7/tcap/TcapOptionBuffer.java \
	org/openss7/tcap/TcapOption.java \
	org/openss7/tcap/TcapReject.java \
	org/openss7/tcap/TcapResult.java \
	org/openss7/tcap/TcapState.java \
	org/openss7/tcap/TcapStream.java \
	org/openss7/tcap/TcapUni.java \
	org/openss7/tcap/TciError.java

my_tcap_CLASSFILES = \
	org/openss7/tcap/SccpAddress.class \
	org/openss7/tcap/sccp_addr_t.class \
	org/openss7/tcap/SWIGTYPE_p_listTTcapOptions_R_t.class \
	org/openss7/tcap/SWIGTYPE_p_listTtci__TcapOption_R_t.class \
	org/openss7/tcap/TcapAbort.class \
	org/openss7/tcap/TcapAccept.class \
	org/openss7/tcap/TcapAllow.class \
	org/openss7/tcap/TcapBegin.class \
	org/openss7/tcap/TcapCancel.class \
	org/openss7/tcap/TcapComponentEvent.class \
	org/openss7/tcap/TcapCont.class \
	org/openss7/tcap/TcapCoord.class \
	org/openss7/tcap/TcapData.class \
	org/openss7/tcap/TcapDialogueEvent.class \
	org/openss7/tcap/TcapEnd.class \
	org/openss7/tcap/TcapEndPoint.class \
	org/openss7/tcap/TcapError.class \
	org/openss7/tcap/TcapEvent.class \
	org/openss7/tcap/TcapInvoke.class \
	org/openss7/tcap/Tcap.class \
	org/openss7/tcap/TcapJNI.class \
	org/openss7/tcap/TcapManagement.class \
	org/openss7/tcap/TcapOptionBuffer.class \
	org/openss7/tcap/TcapOption.class \
	org/openss7/tcap/TcapReject.class \
	org/openss7/tcap/TcapResult.class \
	org/openss7/tcap/TcapState.class \
	org/openss7/tcap/TcapStream.class \
	org/openss7/tcap/TcapUni.class \
	org/openss7/tcap/TciError.class

my_tcap_SOURCEFILES = \
	org/openss7/tcap/tcap_wrap.cxx

lib_org_openss7_tcap_jar_la_SOURCES 	= org-openss7-tcap.jar $(my_tcap_SOURCEFILES)
lib_org_openss7_tcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_tcap_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

EXTRA_LTLIBRARIES 	+= lib-org-openss7-tcap.jar.la
EXTRA_DATA		+= lib-org-openss7-tcap.jar.db
#gcjlib_LTLIBRARIES 	+= lib-org-openss7-tcap.jar.la
#gcjlib_DATA		+= lib-org-openss7-tcap.jar.db
#ALL_LOCAL		+= lib-org-openss7-tcap.jar.db

lib-org-openss7-tcap.jar.db: org-openss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

my_jar_LINKS += org-openss7-tcap.jar
ALL_LOCAL += org-openss7-tcap.jar
jar_DATA += org-openss7-tcap-$(VERSION).jar

org-openss7-tcap-$(VERSION).jar: $(my_xti_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_xti_CLASSFILES)
my_jar_CLASSFILES += $(my_xti_CLASSFILES)

libopenss7TcapJNI_la_SOURCES 		= $(my_tcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7TcapJNI_la_CPPFLAGS 		= $(GCJLIB_INCLUDES) -fno-strict-aliasing
libopenss7TcapJNI_la_CFLAGS		= $(USER_CFLAGS)
libopenss7TcapJNI_la_LIBADD		= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7TcapJNI_la_LDFLAGS		= -module -avoid-version -shared

#jnilib_LTLIBRARIES += libopenss7TcapJNI.la
EXTRA_LTLIBRARIES += libopenss7TcapJNI.la

my_c_WRAPPERS += org/openss7/tcap/tcap_wrap.c
my_cxx_WRAPPERS += org/openss7/tcap/tcap_wrap.cxx
MY_INTERMEDIATE += org/openss7/tcap/tcap_wrap.c
MY_INTERMEDIATE += org/openss7/tcap/tcap_wrap.cxx
MY_INTERMEDIATE += $(my_tcap_JAVAFILES)
MY_SECONDARY += $(my_tcap_CLASSFILES)
$(my_tcap_JAVAFILES): $(my_tcap_SOURCEFILES)

my_JAVAFILES += $(my_tcap_JAVAFILES)

clean-org_openss7_tcap:
	rm -f -- $(my_tcap_JAVAFILES)
	rm -f -- $(my_tcap_CLASSFILES)
	rm -f -- org/openss7/tcap/tcap_wrap.c
	rm -f -- org/openss7/tcap/tcap_wrap.cxx
	rm -f -- org-openss7-tcap.jar org-openss7-tcap-$(VERSION).jar
	rm -f -- lib-org-openss7-tcap.jar.db

CLEAN_LOCAL += clean-org_openss7_tcap

#############################################################################
# This is how we build XTCAP JNI classes for Java.
#############################################################################

my_xtcap_JAVAFILES = \
	org/openss7/xtcap/OM_descriptor.java \
	org/openss7/xtcap/OM_padded_object.java \
	org/openss7/xtcap/OMP_return_code.java \
	org/openss7/xtcap/OM_string.java \
	org/openss7/xtcap/OM_value.java \
	org/openss7/xtcap/SWIGTYPE_p_OM_value.java \
	org/openss7/xtcap/SWIGTYPE_p_p_char.java \
	org/openss7/xtcap/SWIGTYPE_p_p_OM_descriptor_struct.java \
	org/openss7/xtcap/SWIGTYPE_p_void.java \
	org/openss7/xtcap/TCAP_feature.java \
	org/openss7/xtcap/TCAP_waiting_sessions.java \
	org/openss7/xtcap/XtcapConstants.java \
	org/openss7/xtcap/Xtcap.java \
	org/openss7/xtcap/XtcapJNI.java
my_xtcap_CLASSFILES = \
	org/openss7/xtcap/OM_descriptor.class \
	org/openss7/xtcap/OM_padded_object.class \
	org/openss7/xtcap/OMP_return_code.class \
	org/openss7/xtcap/OM_string.class \
	org/openss7/xtcap/OM_value.class \
	org/openss7/xtcap/SWIGTYPE_p_OM_value.class \
	org/openss7/xtcap/SWIGTYPE_p_p_char.class \
	org/openss7/xtcap/SWIGTYPE_p_p_OM_descriptor_struct.class \
	org/openss7/xtcap/SWIGTYPE_p_void.class \
	org/openss7/xtcap/TCAP_feature.class \
	org/openss7/xtcap/TCAP_waiting_sessions.class \
	org/openss7/xtcap/XtcapConstants.class \
	org/openss7/xtcap/Xtcap.class \
	org/openss7/xtcap/XtcapJNI.class
my_xtcap_SOURCEFILES = \
	org/openss7/xtcap/xtcap_wrap.c

lib_org_openss7_xtcap_jar_la_SOURCES 	= org-openss7-xtcap.jar $(my_xtcap_SOURCEFILES)
lib_org_openss7_xtcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xtcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xtcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_xtcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_xtcap_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

EXTRA_LTLIBRARIES 	+= lib-org-openss7-xtcap.jar.la
EXTRA_DATA		+= lib-org-openss7-xtcap.jar.db
#gcjlib_LTLIBRARIES 	+= lib-org-openss7-xtcap.jar.la
#gcjlib_DATA		+= lib-org-openss7-xtcap.jar.db
#ALL_LOCAL		+= lib-org-openss7-xtcap.jar.db

lib-org-openss7-xtcap.jar.db: org-openss7-xtcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/$$(basename $@ .db).so

my_jar_LINKS += org-openss7-xtcap.jar
ALL_LOCAL += org-openss7-xtcap.jar
jar_DATA += org-openss7-xtcap-$(VERSION).jar

org-openss7-xtcap-$(VERSION).jar: $(my_xtcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

MY_SECONDARY += $(my_xtcap_CLASSFILES)
my_jar_CLASSFILES += $(my_xtcap_CLASSFILES)

libopenss7XtcapJNI_la_SOURCES 		= $(my_xtcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtcapJNI_la_CPPFLAGS 		= $(GCJLIB_INCLUDES) -fno-strict-aliasing
libopenss7XtcapJNI_la_CFLAGS		= $(USER_CFLAGS)
libopenss7XtcapJNI_la_LIBADD		= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtcapJNI_la_LDFLAGS		= -module -avoid-version -shared

#jnilib_LTLIBRARIES += libopenss7XtcapJNI.la
EXTRA_LTLIBRARIES += libopenss7XtcapJNI.la

my_c_WRAPPERS += org/openss7/xtcap/xtcap_wrap.c
my_cxx_WRAPPERS += org/openss7/xtcap/xtcap_wrap.cxx
MY_INTERMEDIATE += org/openss7/xtcap/xtcap_wrap.c
MY_INTERMEDIATE += org/openss7/xtcap/xtcap_wrap.cxx
MY_INTERMEDIATE += $(my_xtcap_JAVAFILES)
MY_SECONDARY += $(my_xtcap_CLASSFILES)
$(my_xtcap_JAVAFILES): $(my_xtcap_SOURCEFILES)

clean-org_openss7_xtcap:
	rm -f -- $(my_xtcap_JAVAFILES)
	rm -f -- $(my_xtcap_CLASSFILES)
	rm -f -- org/openss7/xtcap/xtcap_wrap.c
	rm -f -- org/openss7/xtcap/xtcap_wrap.cxx
	rm -f -- org-openss7-xtcap.jar org-openss7-xtcap-$(VERSION).jar
	rm -f -- lib-org-openss7-xtcap.jar.db

CLEAN_LOCAL += clean-org_openss7_xtcap

my_JAVAFILES += $(my_xtcap_JAVAFILES)

.INTERMEDIATE: $(MY_INTERMEDIATE)
.SECONDARY: $(MY_SECONDARY)

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
