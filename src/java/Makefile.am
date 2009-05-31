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
gcjlib_LTLIBRARIES =

AM_GCJFLAGS = -I$(builddir) -I$(srcdir) -Wno-deprecated -Wno-unused -Wno-out-of-date
GCJLIB_INCLUDES = -I$(builddir) -I$(srcdir) $(PKG_INCLUDES) $(USER_CPPFLAGS)

.java.class:
@am__fastdepGCJ_TRUE@	$(AM_V_GCJ)p=$@; $(am__vpath_adj_setup) $(am__vpath_adj) $(GCJCOMPILE) -MT $@ -MD -MP -MF $$(dirname $$f)/$(DEPDIR)/$$(basename $$f .class).Tpo -C -d $(abs_builddir) $<
@am__fastdepGCJ_TRUE@	$(AM_V_at)p=$@; $(am__vpath_adj_setup) $(am__vpath_adj) $(am__mv) $$(dirname $$f)/$(DEPDIR)/$$(basename $$f .class).Tpo $$(dirname $$f)/$(DEPDIR)/$$(basename $$f .class).Po
@am__fastdepGCJ_FALSE@	$(AM_V_GCJ) @AM_BACKSLASH@
@AMDEP_TRUE@@am__fastdepGCJ_FALSE@	source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepGCJ_FALSE@	DEPDIR=$(DEPDIR) $(GCJDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepGCJ_FALSE@	$(GCJCOMPILE) -C -d $(abs_buiddir) $<

#############################################################################
# These are the CNI modules
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
# This is how we build JAIN TCAP CNI classes for Java.
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

lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES 	= $(my_jtcap_CLASSFILES) $(my_jtcap_INCLUDES) $(my_jtcap_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_GCJFLAGS	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-jain-protocol-ss7-tcap.jar.la

$(my_jtcap_INCLUDES): %.h : %.class
$(my_jtcap_SOURCEFILES): $(my_jtcap_INCLUDES)

MY_PRECIOUS += $(my_jtcap_SOURCEFILES)

CLEANFILES += $(my_jtcap_INCLUDES) $(my_jtcap_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jtcap_INCLUDES) $(my_jtcap_HEADERCLASSES)

my_jar_LINKS += openss7Jtcap.jar
ALL_LOCAL += openss7Jtcap.jar
jar_DATA += openss7Jtcap-$(VERSION).jar
CLEANFILES += openss7Jtcap.jar openss7Jtcap-$(VERSION).jar

openss7Jtcap-$(VERSION).jar: $(my_jtcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jtcap_CLASSFILES)

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

lib_org_openss7_javax_sip_jar_la_SOURCES 	= $(my_jsip_CLASSFILES) $(my_jsip_INCLUDES) $(my_jsip_SOURCEFILES)
lib_org_openss7_javax_sip_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_sip_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_sip_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_sip_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_sip_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-sip.jar.la

$(my_jsip_INCLUDES): %.h : %.class
$(my_jsip_SOURCEFILES): $(my_jsip_INCLUDES)

MY_PRECIOUS += $(my_jsip_SOURCEFILES)

CLEANFILES += $(my_jsip_INCLUDES) $(my_jsip_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jsip_INCLUDES) $(my_jsip_HEADERCLASSES)

my_jar_LINKS += openss7Jsip.jar
ALL_LOCAL += openss7Jsip.jar
jar_DATA += openss7Jsip-$(VERSION).jar
CLEANFILES += openss7Jsip.jar openss7Jsip-$(VERSION).jar

openss7Jsip-$(VERSION).jar: $(my_jsip_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jsip_CLASSFILES)

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

lib_org_openss7_javax_jcat_jar_la_SOURCES 	= $(my_jcat_CLASSFILES) $(my_jcat_INCLUDES) $(my_jcat_SOURCEFILES)
lib_org_openss7_javax_jcat_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jcat_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jcat_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jcat_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jcat_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-jcat.jar.la

$(my_jcat_INCLUDES): %.h : %.class
$(my_jcat_SOURCEFILES): $(my_jcat_INCLUDES)

MY_PRECIOUS += $(my_jcat_SOURCEFILES)

CLEANFILES += $(my_jcat_INCLUDES) $(my_jcat_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jcat_INCLUDES) $(my_jcat_HEADERCLASSES)

my_jar_LINKS += openss7Jcat.jar
ALL_LOCAL += openss7Jcat.jar
jar_DATA += openss7Jcat-$(VERSION).jar
CLEANFILES += openss7Jcat.jar openss7Jcat-$(VERSION).jar

openss7Jcat-$(VERSION).jar: $(my_jcat_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jcat_CLASSFILES)

#############################################################################

my_jisup_JAVAFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.java \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.java \
	javax/jain/ss7/EncodingScheme.java \
	javax/jain/ss7/GlobalTitle.java \
	javax/jain/ss7/GTIndicator0001.java \
	javax/jain/ss7/GTIndicator0010.java \
	javax/jain/ss7/GTIndicator0011.java \
	javax/jain/ss7/GTIndicator0100.java \
	javax/jain/ss7/GTIndicator.java \
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
	javax/jain/ss7/NAddressIndicator.java \
	javax/jain/ss7/NetworkIndicator.java \
	javax/jain/ss7/NumberingPlan.java \
	javax/jain/ss7/RoutingIndicator.java \
	javax/jain/ss7/SccpUserAddress.java \
	javax/jain/ss7/SignalingPointCode.java \
	javax/jain/ss7/SPCFormat.java \
	javax/jain/ss7/SS7Address.java \
	javax/jain/ss7/SubSystemAddress.java
my_jisup_CLASSFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.class \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.class \
	javax/jain/ss7/EncodingScheme.class \
	javax/jain/ss7/GlobalTitle.class \
	javax/jain/ss7/GTIndicator0001.class \
	javax/jain/ss7/GTIndicator0010.class \
	javax/jain/ss7/GTIndicator0011.class \
	javax/jain/ss7/GTIndicator0100.class \
	javax/jain/ss7/GTIndicator.class \
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
	javax/jain/ss7/NAddressIndicator.class \
	javax/jain/ss7/NetworkIndicator.class \
	javax/jain/ss7/NumberingPlan.class \
	javax/jain/ss7/RoutingIndicator.class \
	javax/jain/ss7/SccpUserAddress.class \
	javax/jain/ss7/SignalingPointCode.class \
	javax/jain/ss7/SPCFormat.class \
	javax/jain/ss7/SS7Address.class \
	javax/jain/ss7/SubSystemAddress.class
my_jisup_SOURCEFILES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.cc \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.cc
my_jisup_INCLUDES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.h \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.h \
	javax/jain/ss7/EncodingScheme.h \
	javax/jain/ss7/GlobalTitle.h \
	javax/jain/ss7/GTIndicator0001.h \
	javax/jain/ss7/GTIndicator0010.h \
	javax/jain/ss7/GTIndicator0011.h \
	javax/jain/ss7/GTIndicator0100.h \
	javax/jain/ss7/GTIndicator.h \
	javax/jain/ss7/isup/AlertEvent.h \
	javax/jain/ss7/isup/ansi/AlertAnsiEvent.h \
	javax/jain/ss7/isup/ansi/AnswerAnsiEvent.h \
	javax/jain/ss7/isup/ansi/BusinessGroupAnsi.h \
	javax/jain/ss7/isup/ansi/BwdCallIndAnsi.h \
	javax/jain/ss7/isup/ansi/CallProgressAnsiEvent.h \
	javax/jain/ss7/isup/ansi/CarrierIdAnsi.h \
	javax/jain/ss7/isup/ansi/ChargeNumberAnsi.h \
	javax/jain/ss7/isup/ansi/CircuitAssignMapAnsi.h \
	javax/jain/ss7/isup/ansi/CircuitGrpCharacteristicsIndAnsi.h \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAckAnsiEvent.h \
	javax/jain/ss7/isup/ansi/CircuitGrpResetAnsiEvent.h \
	javax/jain/ss7/isup/ansi/CircuitIdNameAnsi.h \
	javax/jain/ss7/isup/ansi/CircuitReservationAnsiEvent.h \
	javax/jain/ss7/isup/ansi/CircuitValidationRespAnsiEvent.h \
	javax/jain/ss7/isup/ansi/CircuitValidationRespIndAnsi.h \
	javax/jain/ss7/isup/ansi/CommonLangLocationIdAnsi.h \
	javax/jain/ss7/isup/ansi/ExitAnsiEvent.h \
	javax/jain/ss7/isup/ansi/FwdCallIndAnsi.h \
	javax/jain/ss7/isup/ansi/GenericNameAnsi.h \
	javax/jain/ss7/isup/ansi/InfoIndAnsi.h \
	javax/jain/ss7/isup/ansi/InfoReqIndAnsi.h \
	javax/jain/ss7/isup/ansi/InformationAnsiEvent.h \
	javax/jain/ss7/isup/ansi/InformationReqAnsiEvent.h \
	javax/jain/ss7/isup/ansi/OperatorServicesInfoAnsi.h \
	javax/jain/ss7/isup/ansi/OptionalBwdCallIndAnsi.h \
	javax/jain/ss7/isup/ansi/ReleaseAnsiEvent.h \
	javax/jain/ss7/isup/ansi/SetupAnsiEvent.h \
	javax/jain/ss7/isup/ansi/TransactionReqAnsi.h \
	javax/jain/ss7/isup/ansi/TransitNwSelAnsi.h \
	javax/jain/ss7/isup/AnswerEvent.h \
	javax/jain/ss7/isup/BwdCallInd.h \
	javax/jain/ss7/isup/CalledPartyNumber.h \
	javax/jain/ss7/isup/CallFailureIndEvent.h \
	javax/jain/ss7/isup/CallingPartyNumber.h \
	javax/jain/ss7/isup/CallProgressEvent.h \
	javax/jain/ss7/isup/CallReference.h \
	javax/jain/ss7/isup/CauseInd.h \
	javax/jain/ss7/isup/CircuitGrpCommonEvent.h \
	javax/jain/ss7/isup/CircuitGrpMaintenanceEvent.h \
	javax/jain/ss7/isup/CircuitGrpQueryEvent.h \
	javax/jain/ss7/isup/CircuitGrpQueryRespEvent.h \
	javax/jain/ss7/isup/CircuitGrpResetAckEvent.h \
	javax/jain/ss7/isup/CircuitGrpResetEvent.h \
	javax/jain/ss7/isup/CircuitMaintenanceEvent.h \
	javax/jain/ss7/isup/ConfusionEvent.h \
	javax/jain/ss7/isup/ConReq.h \
	javax/jain/ss7/isup/ContinuityEvent.h \
	javax/jain/ss7/isup/EventInformation.h \
	javax/jain/ss7/isup/FacilityEvent.h \
	javax/jain/ss7/isup/ForwardTransferEvent.h \
	javax/jain/ss7/isup/FwdCallInd.h \
	javax/jain/ss7/isup/GenericDigits.h \
	javax/jain/ss7/isup/GenericNumber.h \
	javax/jain/ss7/isup/InfoInd.h \
	javax/jain/ss7/isup/InfoReqInd.h \
	javax/jain/ss7/isup/InformationEvent.h \
	javax/jain/ss7/isup/InformationReqEvent.h \
	javax/jain/ss7/isup/InvalidListenerException.h \
	javax/jain/ss7/isup/IsupConstants.h \
	javax/jain/ss7/isup/IsupErrorIndEvent.h \
	javax/jain/ss7/isup/IsupEvent.h \
	javax/jain/ss7/isup/IsupException.h \
	javax/jain/ss7/isup/IsupParameter.h \
	javax/jain/ss7/isup/IsupUserAddress.h \
	javax/jain/ss7/isup/itu/AlertItuEvent.h \
	javax/jain/ss7/isup/itu/AnswerItuEvent.h \
	javax/jain/ss7/isup/itu/BwdCallIndItu.h \
	javax/jain/ss7/isup/itu/CallDiversionInfoItu.h \
	javax/jain/ss7/isup/itu/CalledPartyNumberItu.h \
	javax/jain/ss7/isup/itu/CallingPartyNumberItu.h \
	javax/jain/ss7/isup/itu/CallProgressItuEvent.h \
	javax/jain/ss7/isup/itu/ConnectedNumberItu.h \
	javax/jain/ss7/isup/itu/ConnectItuEvent.h \
	javax/jain/ss7/isup/itu/CUGInterlockCodeItu.h \
	javax/jain/ss7/isup/itu/EchoControlInfoItu.h \
	javax/jain/ss7/isup/itu/FacilityAcceptItuEvent.h \
	javax/jain/ss7/isup/itu/FacilityItuEvent.h \
	javax/jain/ss7/isup/itu/FacilityRejectItuEvent.h \
	javax/jain/ss7/isup/itu/FacilityRequestItuEvent.h \
	javax/jain/ss7/isup/itu/FwdCallIndItu.h \
	javax/jain/ss7/isup/itu/GenericNumberItu.h \
	javax/jain/ss7/isup/itu/IdentificationReqItuEvent.h \
	javax/jain/ss7/isup/itu/IdentificationRespItuEvent.h \
	javax/jain/ss7/isup/itu/InformationItuEvent.h \
	javax/jain/ss7/isup/itu/InformationReqItuEvent.h \
	javax/jain/ss7/isup/itu/LocationNumberItu.h \
	javax/jain/ss7/isup/itu/MCIDReqIndItu.h \
	javax/jain/ss7/isup/itu/MCIDRespIndItu.h \
	javax/jain/ss7/isup/itu/MessageCompatibilityInfoItu.h \
	javax/jain/ss7/isup/itu/NetworkResourceMgmtItuEvent.h \
	javax/jain/ss7/isup/itu/NwSpecificFacItu.h \
	javax/jain/ss7/isup/itu/OptionalBwdCallIndItu.h \
	javax/jain/ss7/isup/itu/OptionalFwdCallIndItu.h \
	javax/jain/ss7/isup/itu/ParamCompatibilityInfoItu.h \
	javax/jain/ss7/isup/itu/RedirectionNumberItu.h \
	javax/jain/ss7/isup/itu/ReleaseItuEvent.h \
	javax/jain/ss7/isup/itu/ReleaseResponseItuEvent.h \
	javax/jain/ss7/isup/itu/SegmentationItuEvent.h \
	javax/jain/ss7/isup/itu/SetupItuEvent.h \
	javax/jain/ss7/isup/itu/SubsequentAddressItuEvent.h \
	javax/jain/ss7/isup/itu/UserPartTestInfoItuEvent.h \
	javax/jain/ss7/isup/itu/UserTeleserviceInfoItu.h \
	javax/jain/ss7/isup/itu/UserToUserIndicatorsItu.h \
	javax/jain/ss7/isup/itu/UserToUserInfoItuEvent.h \
	javax/jain/ss7/isup/JainIsupListener.h \
	javax/jain/ss7/isup/JainIsupProvider.h \
	javax/jain/ss7/isup/JainIsupStack.h \
	javax/jain/ss7/isup/MaintenanceTimeoutIndEvent.h \
	javax/jain/ss7/isup/NatureConnInd.h \
	javax/jain/ss7/isup/NotificationInd.h \
	javax/jain/ss7/isup/NumberParameter.h \
	javax/jain/ss7/isup/NwStatusIndEvent.h \
	javax/jain/ss7/isup/OptionalBwdCallInd.h \
	javax/jain/ss7/isup/OrigCalledNumber.h \
	javax/jain/ss7/isup/ParameterAlreadySetException.h \
	javax/jain/ss7/isup/ParameterRangeInvalidException.h \
	javax/jain/ss7/isup/PassAlongEvent.h \
	javax/jain/ss7/isup/Precedence.h \
	javax/jain/ss7/isup/PrimitiveInvalidException.h \
	javax/jain/ss7/isup/RangeAndStatus.h \
	javax/jain/ss7/isup/RawMessageEvent.h \
	javax/jain/ss7/isup/ReattemptIndEvent.h \
	javax/jain/ss7/isup/RedirectingNumber.h \
	javax/jain/ss7/isup/RedirectionInfo.h \
	javax/jain/ss7/isup/ReleaseEvent.h \
	javax/jain/ss7/isup/ReleaseResponseEvent.h \
	javax/jain/ss7/isup/RemoteOperations.h \
	javax/jain/ss7/isup/SendStackException.h \
	javax/jain/ss7/isup/SetupEvent.h \
	javax/jain/ss7/isup/StartResetIndEvent.h \
	javax/jain/ss7/isup/StopMaintReqEvent.h \
	javax/jain/ss7/isup/SuspendResumeEvent.h \
	javax/jain/ss7/isup/TransitNwSel.h \
	javax/jain/ss7/isup/UserServiceInfoCommon.h \
	javax/jain/ss7/isup/UserServiceInfo.h \
	javax/jain/ss7/isup/UserServiceInfoPrime.h \
	javax/jain/ss7/isup/UserToUserIndicators.h \
	javax/jain/ss7/JainSS7Factory.h \
	javax/jain/ss7/JainSS7Utility.h \
	javax/jain/ss7/NAddressIndicator.h \
	javax/jain/ss7/NetworkIndicator.h \
	javax/jain/ss7/NumberingPlan.h \
	javax/jain/ss7/RoutingIndicator.h \
	javax/jain/ss7/SccpUserAddress.h \
	javax/jain/ss7/SignalingPointCode.h \
	javax/jain/ss7/SPCFormat.h \
	javax/jain/ss7/SS7Address.h \
	javax/jain/ss7/SubSystemAddress.h
my_jisup_HEADERCLASSES = \
	org/openss7/javax/jain/ss7/isup/JainIsupProviderImpl.class \
	org/openss7/javax/jain/ss7/isup/JainIsupStackImpl.class \
	javax/jain/ss7/EncodingScheme.class \
	javax/jain/ss7/GlobalTitle.class \
	javax/jain/ss7/GTIndicator0001.class \
	javax/jain/ss7/GTIndicator0010.class \
	javax/jain/ss7/GTIndicator0011.class \
	javax/jain/ss7/GTIndicator0100.class \
	javax/jain/ss7/GTIndicator.class \
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
	javax/jain/ss7/NAddressIndicator.class \
	javax/jain/ss7/NetworkIndicator.class \
	javax/jain/ss7/NumberingPlan.class \
	javax/jain/ss7/RoutingIndicator.class \
	javax/jain/ss7/SccpUserAddress.class \
	javax/jain/ss7/SignalingPointCode.class \
	javax/jain/ss7/SPCFormat.class \
	javax/jain/ss7/SS7Address.class \
	javax/jain/ss7/SubSystemAddress.class

lib_org_openss7_javax_jain_ss7_isup_jar_la_SOURCES 	= $(my_jisup_CLASSFILES) $(my_jisup_INCLUDES) $(my_jisup_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_isup_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_isup_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_ss7_isup_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_isup_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_isup_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-jain-ss7-isup.jar.la

$(my_jisup_INCLUDES): %.h : %.class
$(my_jisup_SOURCEFILES): $(my_jisup_INCLUDES)

MY_PRECIOUS += $(my_jisup_SOURCEFILES)

CLEANFILES += $(my_jisup_INCLUDES) $(my_jisup_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jisup_INCLUDES) $(my_jisup_HEADERCLASSES)

my_jar_LINKS += openss7Jisup.jar
ALL_LOCAL += openss7Jisup.jar
jar_DATA += openss7Jisup-$(VERSION).jar
CLEANFILES += openss7Jisup.jar openss7Jisup-$(VERSION).jar

openss7Jisup-$(VERSION).jar: $(my_jisup_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jisup_CLASSFILES)

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

lib_org_openss7_javax_jain_ss7_inap_jar_la_SOURCES 	= $(my_jinap_CLASSFILES) $(my_jinap_INCLUDES) $(my_jinap_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_inap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_inap_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_ss7_inap_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_inap_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_inap_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-jain-ss7-inap.jar.la

$(my_jinap_INCLUDES): %.h : %.class
$(my_jinap_SOURCEFILES): $(my_jinap_INCLUDES)

MY_PRECIOUS += $(my_jinap_SOURCEFILES)

CLEANFILES += $(my_jinap_INCLUDES) $(my_jinap_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jinap_INCLUDES) $(my_jinap_HEADERCLASSES)

my_jar_LINKS += openss7Jinap.jar
ALL_LOCAL += openss7Jinap.jar
jar_DATA += openss7Jinap-$(VERSION).jar
CLEANFILES += openss7Jinap.jar openss7Jinap-$(VERSION).jar

openss7Jinap-$(VERSION).jar: $(my_jinap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jinap_CLASSFILES)

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

lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_SOURCES 	= $(my_jmap_CLASSFILES) $(my_jmap_INCLUDES) $(my_jmap_SOURCEFILES)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-jain-protocol-ss7-map.jar.la

$(my_jmap_INCLUDES): %.h : %.class
$(my_jmap_SOURCEFILES): $(my_jmap_INCLUDES)

MY_PRECIOUS += $(my_jmap_SOURCEFILES)

CLEANFILES += $(my_jmap_INCLUDES) $(my_jmap_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jmap_INCLUDES) $(my_jmap_HEADERCLASSES)

my_jar_LINKS += openss7Jmap.jar
ALL_LOCAL += openss7Jmap.jar
jar_DATA += openss7Jmap-$(VERSION).jar
CLEANFILES += openss7Jmap.jar openss7Jmap-$(VERSION).jar

openss7Jmap-$(VERSION).jar: $(my_jmap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jmap_CLASSFILES)

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

lib_org_openss7_javax_csapi_cc_jcc_jar_la_SOURCES 	= $(my_jcc_CLASSFILES) $(my_jcc_INCLUDES) $(my_jcc_SOURCEFILES)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CFLAGS 	= $(USER_CFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LIBADD 	= $(top_builddir)/libstreams.la
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LDFLAGS 	= -module -avoid-version -shared
lib_org_openss7_javax_csapi_cc_jcc_jar_la_GCJFLAGS 	= -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-javax-csapi-cc-jcc.jar.la

$(my_jcc_INCLUDES): %.h : %.class
$(my_jcc_SOURCEFILES): $(my_jcc_INCLUDES)

MY_PRECIOUS += $(my_jcc_SOURCEFILES)

CLEANFILES += $(my_jcc_INCLUDES) $(my_jcc_HEADERCLASSES)
MY_INTERMEDIATE += $(my_jcc_INCLUDES) $(my_jcc_HEADERCLASSES)

my_jar_LINKS += openss7Jcc.jar
ALL_LOCAL += openss7Jcc.jar
jar_DATA += openss7Jcc-$(VERSION).jar
CLEANFILES += openss7Jcc.jar openss7Jcc-$(VERSION).jar

openss7Jcc-$(VERSION).jar: $(my_jcc_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

my_jar_CLASSFILES += $(my_jcc_CLASSFILES)

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

lib_org_openss7_streams_jar_la_SOURCES 	= $(my_streams_CLASSFILES) $(my_streams_SOURCEFILES)
lib_org_openss7_streams_jar_la_CPPFLAGS = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_streams_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_streams_jar_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_streams_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_streams_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

#gcjlib_LTLIBRARIES += lib-org-openss7-streams.jar.la
EXTRA_LTLIBRARIES += lib-org-openss7-streams.jar.la

my_jar_LINKS += openss7Streams.jar
ALL_LOCAL += openss7Streams.jar
jar_DATA += openss7Streams-$(VERSION).jar
CLEANFILES += openss7Streams.jar openss7Streams-$(VERSION).jar

openss7Streams-$(VERSION).jar: $(my_streams_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

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
CLEANFILES += org/openss7/streams/streams_wrap.c
CLEANFILES += org/openss7/streams/streams_wrap.cxx
CLEANFILES += $(my_streams_JAVAFILES) $(my_streams_CLASSFILES)
MY_INTERMEDIATE += org/openss7/streams/streams_wrap.c
MY_INTERMEDIATE += org/openss7/streams/streams_wrap.cxx
MY_INTERMEDIATE += $(my_streams_JAVAFILES) $(my_streams_CLASSFILES)
$(my_streams_JAVAFILES): $(my_streams_SOURCEFILES)

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

lib_org_openss7_xti_jar_la_SOURCES 	= $(my_xti_CLASSFILES) $(my_xti_SOURCEFILES)
lib_org_openss7_xti_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xti_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xti_jar_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_xti_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_xti_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

gcjlib_LTLIBRARIES += lib-org-openss7-xti.jar.la

my_jar_LINKS += openss7Tcap.jar
ALL_LOCAL += openss7Tcap.jar
jar_DATA += openss7Tcap-$(VERSION).jar
CLEANFILES += openss7Tcap.jar openss7Tcap-$(VERSION).jar

openss7Tcap-$(VERSION).jar: $(my_tcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

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
CLEANFILES += org/openss7/xti/xti_wrap.c
CLEANFILES += org/openss7/xti/xti_wrap.cxx
CLEANFILES += $(my_xti_JAVAFILES) $(my_xti_CLASSFILES)
MY_INTERMEDIATE += org/openss7/xti/xti_wrap.c
MY_INTERMEDIATE += org/openss7/xti/xti_wrap.cxx
MY_INTERMEDIATE += $(my_xti_JAVAFILES) $(my_xti_CLASSFILES)
$(my_xti_JAVAFILES): $(my_xti_SOURCEFILES)

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

lib_org_openss7_tcap_jar_la_SOURCES 	= $(my_tcap_CLASSFILES) $(my_tcap_SOURCEFILES)
lib_org_openss7_tcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_tcap_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

#gcjlib_LTLIBRARIES += lib-org-openss7-tcap.jar.la
EXTRA_LTLIBRARIES += lib-org-openss7-tcap.jar.la

my_jar_LINKS += openss7Xti.jar
ALL_LOCAL += openss7Xti.jar
jar_DATA += openss7Xti-$(VERSION).jar
CLEANFILES += openss7Xti.jar openss7Xti-$(VERSION).jar

openss7Xti-$(VERSION).jar: $(my_xti_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

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
CLEANFILES += org/openss7/tcap/tcap_wrap.c
CLEANFILES += org/openss7/tcap/tcap_wrap.cxx
CLEANFILES += $(my_tcap_JAVAFILES) $(my_tcap_CLASSFILES)
MY_INTERMEDIATE += org/openss7/tcap/tcap_wrap.c
MY_INTERMEDIATE += org/openss7/tcap/tcap_wrap.cxx
MY_INTERMEDIATE += $(my_tcap_JAVAFILES) $(my_tcap_CLASSFILES)
$(my_tcap_JAVAFILES): $(my_tcap_SOURCEFILES)

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

lib_org_openss7_xtcap_jar_la_SOURCES 	= $(my_xtcap_CLASSFILES) $(my_xtcap_SOURCEFILES)
lib_org_openss7_xtcap_jar_la_CPPFLAGS 	= $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xtcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xtcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_xtcap_jar_la_LDFLAGS	= -module -avoid-version -shared
lib_org_openss7_xtcap_jar_la_GCJFLAGS	= -fjni -findirect-dispatch

#gcjlib_LTLIBRARIES += lib-org-openss7-xtcap.jar.la
EXTRA_LTLIBRARIES += lib-org-openss7-xtcap.jar.la

my_jar_LINKS += openss7Xtcap.jar
ALL_LOCAL += openss7Xtcap.jar
jar_DATA += openss7Xtcap-$(VERSION).jar
CLEANFILES += openss7Xtcap.jar openss7Xtcap-$(VERSION).jar

openss7Xtcap-$(VERSION).jar: $(my_xtcap_CLASSFILES)
	$(AM_V_GEN)$(JAR) cf $@ $^

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
CLEANFILES += org/openss7/xtcap/xtcap_wrap.c
CLEANFILES += org/openss7/xtcap/xtcap_wrap.cxx
CLEANFILES += $(my_xtcap_JAVAFILES) $(my_xtcap_CLASSFILES)
MY_INTERMEDIATE += org/openss7/xtcap/xtcap_wrap.c
MY_INTERMEDIATE += org/openss7/xtcap/xtcap_wrap.cxx
MY_INTERMEDIATE += $(my_xtcap_JAVAFILES) $(my_xtcap_CLASSFILES)
$(my_xtcap_JAVAFILES): $(my_xtcap_SOURCEFILES)

.INTERMEDIATE: $(MY_INTERMEDIATE)

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
