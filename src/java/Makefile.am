# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2009-07-09 12:05:15 $
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
# Last Modified $Date: 2009-07-09 12:05:15 $ by $Author: brian $
#
# =============================================================================

##
## General rules
##
include ../../am/openss7.am

##
## Java rules
##
include ../../am/java.am

## accumulate for swig.am
java_c_MODULES		=
java_cxx_MODULES	=

javadoc_GROUPS += \
	-group "Base Jain API Packages" jain:javax.jain \
	-group "Base Jain SS7 API Packages" jain.protocol.ss7:javax.jain.ss7:javax.jain.protocol.ss7:org.openss7.ss7 \
	-group "Jain OAM API (JSR 18) Packages" jain.protocol.ss7.oam*:org.openss7.jain.protocol.ss7.oam* \
	-group "Jain TCAP API (JSR 11) Packages" jain.protocol.ss7.tcap*:org.openss7.jain.protocol.ss7.tcap* \
	-group "Jain MAP API (JSR 29) Packages" javax.jain.protocol.ss7.map*:org.openss7.javax.jain.protocol.ss7.map* \
	-group "Jain INAP API (JSR 35) Packages" javax.jain.ss7.inap*:org.openss7.javax.jain.ss7.inap* \
	-group "Jain ISUP API (JSR 17) Packages" javax.jain.ss7.isup*:org.openss7.javax.jain.ss7.isup* \
	-group "Jain SCCP API Packages" org.openss7.ss7.sccp* \
	-group "Jain MTP API Packages" org.openss7.ss7.mtp* \
	-group "Jain JCC API (JSR 21) Packages" javax.csapi.cc.jcc*:org.openss7.javax.csapi.cc.jcc* \
	-group "Jain JCAT API (JSR 122) Packages" javax.jcat*:org.openss7.javax.jcat* \
	-group "Base Jain IP API Packages" jain.protocol.ip \
	-group "Jain MGCP API (JSR 23) Packages" jain.protocol.ip.mgcp* \
	-group "Jain MEGACO API (JSR 79) Packages" javax.megaco* \
	-group "Jain H323 API (JSR 81) Packages" jain.protocol.h323* \
	-group "Jain SIP API (JSR 32) Packages" javax.sip*:org.openss7.javax.sip* \
	-group "Jain ENUM API (JSR 161) Packages" javax.enum* \
	-group "Jain TAPI API (JSR 43) Packages" javax.telephony*

javadoc_PACKAGES += \
	jain \
	jain.protocol.ip \
	javax.megaco \
	javax.megaco.association \
	javax.megaco.pkg \
	javax.megaco.pkg.ToneGenPkg \
	javax.megaco.pkg.AnalogLineSPkg \
	javax.megaco.pkg.RTPPkg \
	javax.megaco.pkg.ContinuityPkg \
	javax.megaco.pkg.BaseRootPkg \
	javax.megaco.pkg.DTMFGenPkg \
	javax.megaco.pkg.NetworkPkg \
	javax.megaco.pkg.DTMFDetPkg \
	javax.megaco.pkg.TDMCktPkg \
	javax.megaco.pkg.CPToneDetPkg \
	javax.megaco.pkg.ToneDetPkg \
	javax.megaco.pkg.CPToneGenPkg \
	javax.megaco.pkg.GenericPkg \
	javax.megaco.message \
	javax.megaco.message.descriptor \
	jain.protocol.ss7 \
	jain.protocol.ss7.oam \
	jain.protocol.ss7.oam.mtp2 \
	jain.protocol.ss7.oam.mtp3 \
	jain.protocol.ss7.oam.sccp \
	jain.protocol.ss7.oam.tcap \
	jain.protocol.ss7.tcap \
	jain.protocol.ss7.tcap.component \
	jain.protocol.ss7.tcap.dialogue \
	javax \
	javax.csapi \
	javax.csapi.cc \
	javax.csapi.cc.jcc \
	javax.jain \
	javax.jain.protocol \
	javax.jain.protocol.ss7 \
	javax.jain.protocol.ss7.map \
	javax.jain.protocol.ss7.map.information \
	javax.jain.protocol.ss7.map.information.etsipar \
	javax.jain.protocol.ss7.map.position \
	javax.jain.protocol.ss7.map.position.etsipar \
	javax.jain.protocol.ss7.map.session \
	javax.jain.protocol.ss7.map.session.etsipar \
	javax.jain.protocol.ss7.map.transaction \
	javax.jain.protocol.ss7.map.transaction.etsipar \
	javax.jain.protocol.ss7.map.transaction.etsipar.message \
	javax.jain.protocol.ss7.map.transaction.etsipar.pdu \
	javax.jain.ss7 \
	javax.jain.ss7.inap \
	javax.jain.ss7.inap.constants \
	javax.jain.ss7.inap.datatype \
	javax.jain.ss7.inap.event \
	javax.jain.ss7.inap.exception \
	javax.jain.ss7.inap.operation \
	javax.jain.ss7.isup \
	javax.jain.ss7.isup.ansi \
	javax.jain.ss7.isup.itu \
	javax.jcat \
	javax.management \
	javax.management.event \
	javax.management.loading \
	javax.management.modelmbean \
	javax.management.monitor \
	javax.management.namespaces \
	javax.management.openmbean \
	javax.management.relation \
	javax.management.remote \
	javax.management.remote.generic \
	javax.management.remote.jmxmp \
	javax.management.remote.message \
	javax.management.remote.rmi \
	javax.management.timer \
	javax.oss \
	javax.oss.fault \
	javax.sip \
	javax.sip.address \
	javax.sip.header \
	javax.sip.message \
	javax.slee \
	javax.slee.connection \
	javax.slee.facilities \
	javax.slee.management \
	javax.slee.nullactivity \
	javax.slee.profile \
	javax.slee.profile.query \
	javax.slee.resource \
	javax.slee.serviceactivity \
	javax.slee.transaction \
	javax.slee.usage \
	javax.telephony \
	javax.telephony.callcenter \
	javax.telephony.callcenter.capabilities \
	javax.telephony.callcenter.events \
	javax.telephony.callcontrol \
	javax.telephony.callcontrol.capabilities \
	javax.telephony.callcontrol.events \
	javax.telephony.capabilities \
	javax.telephony.events \
	javax.telephony.media \
	javax.telephony.media.async \
	javax.telephony.media.capabilities \
	javax.telephony.media.connection \
	javax.telephony.media.connection.async \
	javax.telephony.media.container \
	javax.telephony.media.container.async \
	javax.telephony.media.events \
	javax.telephony.media.messaging \
	javax.telephony.media.provider \
	javax.telephony.media.symbols \
	javax.telephony.mobile \
	javax.telephony.phone \
	javax.telephony.phone.capabilities \
	javax.telephony.phone.events \
	javax.telephony.privatedata \
	javax.telephony.privatedata.capabilities \
	javax.telephony.privatedata.events \
	org \
	org.omg \
	org.omg.stub \
	org.omg.stub.javax \
	org.omg.stub.javax.management \
	org.omg.stub.javax.management.remote \
	org.omg.stub.javax.management.remote.rmi \
	org.openss7 \
	org.openss7.jain \
	org.openss7.jain.protocol \
	org.openss7.jain.protocol.ss7 \
	org.openss7.jain.protocol.ss7.oam \
	org.openss7.jain.protocol.ss7.oam.mtp2 \
	org.openss7.jain.protocol.ss7.oam.mtp3 \
	org.openss7.jain.protocol.ss7.oam.sccp \
	org.openss7.jain.protocol.ss7.oam.tcap \
	org.openss7.jain.protocol.ss7.tcap \
	org.openss7.javax \
	org.openss7.javax.csapi \
	org.openss7.javax.csapi.cc \
	org.openss7.javax.csapi.cc.jcc \
	org.openss7.javax.jain \
	org.openss7.javax.jain.protocol \
	org.openss7.javax.jain.protocol.ss7 \
	org.openss7.javax.jain.protocol.ss7.map \
	org.openss7.javax.jain.protocol.ss7.map.information \
	org.openss7.javax.jain.protocol.ss7.map.position \
	org.openss7.javax.jain.protocol.ss7.map.session \
	org.openss7.javax.jain.protocol.ss7.map.transaction \
	org.openss7.javax.jain.ss7 \
	org.openss7.javax.jain.ss7.inap \
	org.openss7.javax.jain.ss7.isup \
	org.openss7.javax.jcat \
	org.openss7.javax.sip \
	org.openss7.jcat \
	org.openss7.ss7 \
	org.openss7.ss7.mtp \
	org.openss7.ss7.sccp \
	org.openss7.streams \
	org.openss7.tcap \
	org.openss7.tcapi \
	org.openss7.xtcap \
	org.openss7.xti

EXTRA_javadoc_PACKAGES = \
	javax.media \
	javax.media.mscontrol \
	javax.media.mscontrol.mediagroup \
	javax.media.mscontrol.mediagroup.asrtts \
	javax.media.mscontrol.mediagroup.fax \
	javax.media.mscontrol.mediagroup.http \
	javax.media.mscontrol.mediagroup.rtsp \
	javax.media.mscontrol.mediagroup.signals \
	javax.media.mscontrol.mediagroup.sms \
	javax.media.mscontrol.mixer \
	javax.media.mscontrol.networkconnection \
	javax.media.mscontrol.resource \
	javax.media.mscontrol.resource.common \
	javax.media.mscontrol.resource.symbols \
	javax.media.mscontrol.resource.video \
	javax.media.mscontrol.vxml \
	jain.protocol.ip.mgcp \
	jain.protocol.ip.mgcp.pkg \
	jain.protocol.ip.mgcp.message \
	jain.protocol.ip.mgcp.message.parms

#############################################################################
# These are simple jars and their compiled equivalents.
#############################################################################

my_javax_jain_JAVAFILES = \
	javax/jain/CreateProviderException.java \
	javax/jain/DeleteProviderException.java \
	javax/jain/IDNotAvailableException.java \
	javax/jain/InvalidAddressException.java \
	javax/jain/InvalidArgumentException.java \
	javax/jain/InvalidPathNameException.java \
	javax/jain/InvalidStateException.java \
	javax/jain/JainEvent.java \
	javax/jain/JainException.java \
	javax/jain/ListenerAlreadyRegisteredException.java \
	javax/jain/ListenerNotRegisteredException.java \
	javax/jain/MandatoryParameterNotSetException.java \
	javax/jain/MethodNotSupportedException.java \
	javax/jain/ParameterNotSetException.java \
	javax/jain/PeerUnavailableException.java \
	javax/jain/PlatformException.java \
	javax/jain/VersionNotSupportedException.java
my_javax_jain_CLASSFILES = \
	$(my_javax_jain_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_jain_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-jain.jar
ALL_LOCAL	+= javax-jain.jar
jar_DATA	+= javax-jain-$(VERSION).jar
endif

javax-jain-$(VERSION).jar: classdist_java.stamp $(my_javax_jain_CLASSFILES)
	$(AM_V_GEN)d='javax/jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

clean-javax_jain:
	@d='javax/jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain.jar
	rm -f -- javax-jain-$(VERSION).jar
	rm -f -- lib-javax-jain.jar.db

CLEAN_LOCAL += clean-javax_jain

nodist_lib_javax_jain_jar_la_SOURCES	= javax-jain.jar
lib_javax_jain_jar_la_LIBADD		=
lib_javax_jain_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-javax-jain.jar.la
gcjlib_DATA		+= lib-javax-jain.jar.db
ALL_LOCAL		+= lib-javax-jain.jar.db
endif

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
	$(my_javax_jain_ss7_JAVAFILES:%.java=%.class)

MY_SECONDARY += $(my_javax_jain_ss7_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS += javax-jain-ss7.jar
ALL_LOCAL += javax-jain-ss7.jar
jar_DATA += javax-jain-ss7-$(VERSION).jar
endif

javax-jain-ss7-$(VERSION).jar: classdist_java.stamp javax-jain.jar $(my_javax_jain_ss7_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

clean-javax_jain_ss7:
	@d='javax/jain/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-ss7.jar javax-jain-ss7-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7

nodist_lib_javax_jain_ss7_jar_la_SOURCES	= javax-jain-ss7.jar
lib_javax_jain_ss7_jar_la_LIBADD		=
lib_javax_jain_ss7_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_ss7_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-javax-jain-ss7.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7.jar.db
endif

my_javax_jain_protocol_ss7_JAVAFILES = \
	javax/jain/protocol/ss7/E164Number.java \
	javax/jain/protocol/ss7/JainSS7Factory.java \
	javax/jain/protocol/ss7/JainSS7Utility.java \
	javax/jain/protocol/ss7/SS7Address.java \
	javax/jain/protocol/ss7/SS7Exception.java \
	javax/jain/protocol/ss7/SS7InvalidParamException.java \
	javax/jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.java \
	javax/jain/protocol/ss7/SS7ListenerNotRegisteredException.java \
	javax/jain/protocol/ss7/SS7MandatoryParameterNotSetException.java \
	javax/jain/protocol/ss7/SS7Number.java \
	javax/jain/protocol/ss7/SS7Parameter.java \
	javax/jain/protocol/ss7/SS7ParameterNotSetException.java \
	javax/jain/protocol/ss7/SS7PeerUnavailableException.java
my_javax_jain_protocol_ss7_CLASSFILES = \
	$(my_javax_jain_protocol_ss7_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_jain_protocol_ss7_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-jain-protocol-ss7.jar
ALL_LOCAL	+= javax-jain-protocol-ss7.jar
jar_DATA	+= javax-jain-protocol-ss7-$(VERSION).jar
endif

javax-jain-protocol-ss7-$(VERSION).jar: classdist_java.stamp javax-jain.jar $(my_javax_jain_protocol_ss7_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

clean-javax_jain_protocol_ss7:
	@d='javax/jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-protocol-ss7.jar
	rm -f -- javax-jain-protocol-ss7-$(VERSION).jar
	rm -f -- lib-javax-jain-protocol-ss7.jar.db

CLEAN_LOCAL	+= clean-javax_jain_protocol_ss7

nodist_lib_javax_jain_protocol_ss7_jar_la_SOURCES	= javax-jain-protocol-ss7.jar
lib_javax_jain_protocol_ss7_jar_la_LIBADD		=
lib_javax_jain_protocol_ss7_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_protocol_ss7_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-javax-jain-protocol-ss7.jar.la
gcjlib_DATA		+= lib-javax-jain-protocol-ss7.jar.db
ALL_LOCAL		+= lib-javax-jain-protocol-ss7.jar.db
endif

## -----------------------------------------------------------------------------

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
	$(my_jain_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_jain_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= jain.jar
ALL_LOCAL	+= jain.jar
jar_DATA	+= jain-$(VERSION).jar
endif

jain-$(VERSION).jar: classdist_java.stamp $(my_jain_CLASSFILES)
	$(AM_V_GEN)d='jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

clean-jain:
	@d='jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain.jar
	rm -f -- jain-$(VERSION).jar
	rm -f -- lib-jain.jar.db

CLEAN_LOCAL	+= clean-jain

nodist_lib_jain_jar_la_SOURCES	= jain.jar
lib_jain_jar_la_LIBADD		=
lib_jain_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_jain_jar_la_GCJFLAGS	= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-jain.jar.la
gcjlib_DATA		+= lib-jain.jar.db
ALL_LOCAL		+= lib-jain.jar.db
endif

## -----------------------------------------------------------------------------

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
	$(my_jain_protocol_ss7_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_jain_protocol_ss7_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= jain-protocol-ss7.jar
ALL_LOCAL	+= jain-protocol-ss7.jar
jar_DATA	+= jain-protocol-ss7-$(VERSION).jar
endif

jain-protocol-ss7-$(VERSION).jar: classdist_java.stamp jain.jar $(my_jain_protocol_ss7_CLASSFILES)
	$(AM_V_GEN)d='jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

clean-jain_protocol_ss7:
	@d='jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain-protocol-ss7.jar
	rm -f -- jain-protocol-ss7-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7.jar.db

CLEAN_LOCAL += clean-jain_protocol_ss7

nodist_lib_jain_protocol_ss7_jar_la_SOURCES	= jain-protocol-ss7.jar
lib_jain_protocol_ss7_jar_la_LIBADD		=
lib_jain_protocol_ss7_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_jain_protocol_ss7_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-jain-protocol-ss7.jar.la
gcjlib_DATA		+= lib-jain-protocol-ss7.jar.db
ALL_LOCAL		+= lib-jain-protocol-ss7.jar.db
endif

## -----------------------------------------------------------------------------

my_javax_jain_ss7_isup_JAVAFILES = \
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
	javax/jain/ss7/isup/UserToUserIndicators.java
my_javax_jain_ss7_isup_CLASSFILES = \
	$(my_javax_jain_ss7_isup_JAVAFILES:%.java=%.class)

MY_SECONDARY += $(my_javax_jain_ss7_isup_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_ss7_isup_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS += javax-jain-ss7-isup.jar
ALL_LOCAL += javax-jain-ss7-isup.jar
jar_DATA += javax-jain-ss7-isup-$(VERSION).jar
endif

javax-jain-ss7-isup-$(VERSION).jar: classdist_java.stamp javax-jain-ss7.jar $(my_javax_jain_ss7_isup_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_jain_ss7_isup_JAVAFILES)
#dist_java_JAVA += $(my_javax_jain_ss7_isup_JAVAFILES)

clean-javax_jain_ss7_isup:
	@d='javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-ss7-isup.jar javax-jain-ss7-isup-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-isup.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7_isup

nodist_lib_javax_jain_ss7_isup_jar_la_SOURCES	= javax-jain-ss7-isup.jar
lib_javax_jain_ss7_isup_jar_la_LIBADD		=
lib_javax_jain_ss7_isup_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_ss7_isup_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-javax-jain-ss7-isup.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-isup.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7-isup.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7-isup.jar.db
endif

lib-javax-jain-ss7-isup.jar.db: javax-jain-ss7-isup.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

#############################################################################

my_javax_jain_ss7_inap_JAVAFILES = \
	javax/jain/ss7/inap/constants/ActionIndicator.java \
	javax/jain/ss7/inap/constants/ActionPerformed.java \
	javax/jain/ss7/inap/constants/BearerCapabilityChoice.java \
	javax/jain/ss7/inap/constants/CallProcessingOperationCorrelationID.java \
	javax/jain/ss7/inap/constants/CancelChoice.java \
	javax/jain/ss7/inap/constants/CancelID.java \
	javax/jain/ss7/inap/constants/CGEncountered.java \
	javax/jain/ss7/inap/constants/CollectedInfoChoice.java \
	javax/jain/ss7/inap/constants/ComponentChoice.java \
	javax/jain/ss7/inap/constants/ComponentID.java \
	javax/jain/ss7/inap/constants/ComponentIndPrimitiveType.java \
	javax/jain/ss7/inap/constants/ComponentPrimitiveType.java \
	javax/jain/ss7/inap/constants/ComponentReqPrimitiveType.java \
	javax/jain/ss7/inap/constants/ComponentType.java \
	javax/jain/ss7/inap/constants/ConnectedPartyChoice.java \
	javax/jain/ss7/inap/constants/ControlType.java \
	javax/jain/ss7/inap/constants/DialoguePrimitiveType.java \
	javax/jain/ss7/inap/constants/DPAssignment.java \
	javax/jain/ss7/inap/constants/DpSpecificCriteriaChoice.java \
	javax/jain/ss7/inap/constants/ElapsedTimeValueID.java \
	javax/jain/ss7/inap/constants/EntryChoice.java \
	javax/jain/ss7/inap/constants/ErrorCode.java \
	javax/jain/ss7/inap/constants/ErrorTreatment.java \
	javax/jain/ss7/inap/constants/EventSpecificInformationBCSMChoice.java \
	javax/jain/ss7/inap/constants/EventTypeBCSM.java \
	javax/jain/ss7/inap/constants/ExtensionChoice.java \
	javax/jain/ss7/inap/constants/ExtensionType.java \
	javax/jain/ss7/inap/constants/FacilityGroupChoice.java \
	javax/jain/ss7/inap/constants/FacilityGroupID.java \
	javax/jain/ss7/inap/constants/FeatureRequestIndicator.java \
	javax/jain/ss7/inap/constants/FilterCriteriaID.java \
	javax/jain/ss7/inap/constants/FilteringCharacteristicsChoice.java \
	javax/jain/ss7/inap/constants/FilteringCriteriaChoice.java \
	javax/jain/ss7/inap/constants/FilteringTimeOutChoice.java \
	javax/jain/ss7/inap/constants/ForwardingCondition.java \
	javax/jain/ss7/inap/constants/GapCriteriaChoice.java \
	javax/jain/ss7/inap/constants/GapTreatmentChoice.java \
	javax/jain/ss7/inap/constants/GeneralProblem.java \
	javax/jain/ss7/inap/constants/GroupID.java \
	javax/jain/ss7/inap/constants/InformationToSendChoice.java \
	javax/jain/ss7/inap/constants/InfoToSendChoice.java \
	javax/jain/ss7/inap/constants/InvokeProblem.java \
	javax/jain/ss7/inap/constants/LegIDChoice.java \
	javax/jain/ss7/inap/constants/Media.java \
	javax/jain/ss7/inap/constants/MessageIDChoice.java \
	javax/jain/ss7/inap/constants/MessageType.java \
	javax/jain/ss7/inap/constants/MidCallReportType.java \
	javax/jain/ss7/inap/constants/MonitorMode.java \
	javax/jain/ss7/inap/constants/NotificationInformationChoice.java \
	javax/jain/ss7/inap/constants/NumberMatchChoice.java \
	javax/jain/ss7/inap/constants/OperationCodeChoice.java \
	javax/jain/ss7/inap/constants/OperationCode.java \
	javax/jain/ss7/inap/constants/PAbortReason.java \
	javax/jain/ss7/inap/constants/PartyToConnectChoice.java \
	javax/jain/ss7/inap/constants/PartyToDisconnectChoice.java \
	javax/jain/ss7/inap/constants/ProfileIdentifierChoice.java \
	javax/jain/ss7/inap/constants/ReceivedInformationChoice.java \
	javax/jain/ss7/inap/constants/ReceivedStatus.java \
	javax/jain/ss7/inap/constants/RejectType.java \
	javax/jain/ss7/inap/constants/ReportCause.java \
	javax/jain/ss7/inap/constants/ReportCondition.java \
	javax/jain/ss7/inap/constants/RequestedInformationType.java \
	javax/jain/ss7/inap/constants/RequestedInformationValueChoice.java \
	javax/jain/ss7/inap/constants/ResourceAddressChoice.java \
	javax/jain/ss7/inap/constants/ResourceAddressID.java \
	javax/jain/ss7/inap/constants/ResourceIDChoice.java \
	javax/jain/ss7/inap/constants/ResourceStatus.java \
	javax/jain/ss7/inap/constants/ResponseCondition.java \
	javax/jain/ss7/inap/constants/ReturnErrorProblem.java \
	javax/jain/ss7/inap/constants/ReturnResultProblem.java \
	javax/jain/ss7/inap/constants/RoutingAddressChoice.java \
	javax/jain/ss7/inap/constants/SideID.java \
	javax/jain/ss7/inap/constants/TargetLineIdentifierChoice.java \
	javax/jain/ss7/inap/constants/TerminalType.java \
	javax/jain/ss7/inap/constants/TimeOutError.java \
	javax/jain/ss7/inap/constants/TimerID.java \
	javax/jain/ss7/inap/constants/ToneIDChoice.java \
	javax/jain/ss7/inap/constants/TransportAddressChoice.java \
	javax/jain/ss7/inap/constants/TriggerType.java \
	javax/jain/ss7/inap/constants/UAbortReason.java \
	javax/jain/ss7/inap/constants/USIMonitorMode.java \
	javax/jain/ss7/inap/constants/VariablePartChoice.java \
	javax/jain/ss7/inap/constants/VariablePartID.java \
	javax/jain/ss7/inap/datatype/AddressAndService.java \
	javax/jain/ss7/inap/datatype/AllCallSegments.java \
	javax/jain/ss7/inap/datatype/AssociatedCallSegment.java \
	javax/jain/ss7/inap/datatype/BackwardGVNS.java \
	javax/jain/ss7/inap/datatype/BackwardServiceInteractionInd.java \
	javax/jain/ss7/inap/datatype/BCSMEvent.java \
	javax/jain/ss7/inap/datatype/BCSMFailure.java \
	javax/jain/ss7/inap/datatype/BearerCapability.java \
	javax/jain/ss7/inap/datatype/BearerCap.java \
	javax/jain/ss7/inap/datatype/BothGapTreatmentParameters.java \
	javax/jain/ss7/inap/datatype/CalledAddressAndService.java \
	javax/jain/ss7/inap/datatype/CalledPartyNumber.java \
	javax/jain/ss7/inap/datatype/CalledPartySubaddress.java \
	javax/jain/ss7/inap/datatype/CallingAddressAndService.java \
	javax/jain/ss7/inap/datatype/CallingPartyNumber.java \
	javax/jain/ss7/inap/datatype/CallingPartySubaddress.java \
	javax/jain/ss7/inap/datatype/CallSegment.java \
	javax/jain/ss7/inap/datatype/CallSegmentToCancel.java \
	javax/jain/ss7/inap/datatype/CarrierID.java \
	javax/jain/ss7/inap/datatype/Carrier.java \
	javax/jain/ss7/inap/datatype/Cause.java \
	javax/jain/ss7/inap/datatype/ChargingEvent.java \
	javax/jain/ss7/inap/datatype/CollectedDigits.java \
	javax/jain/ss7/inap/datatype/CollectedInfo.java \
	javax/jain/ss7/inap/datatype/Component.java \
	javax/jain/ss7/inap/datatype/ConnectedParty.java \
	javax/jain/ss7/inap/datatype/ControlDigits.java \
	javax/jain/ss7/inap/datatype/CounterAndValue.java \
	javax/jain/ss7/inap/datatype/CSFailure.java \
	javax/jain/ss7/inap/datatype/DestinationRoutingAddress.java \
	javax/jain/ss7/inap/datatype/DigitsGenericDigits.java \
	javax/jain/ss7/inap/datatype/DigitsGenericNumber.java \
	javax/jain/ss7/inap/datatype/DisconnectSpecificInfo.java \
	javax/jain/ss7/inap/datatype/DPSpecificCommonParameters.java \
	javax/jain/ss7/inap/datatype/DPSpecificCriteria.java \
	javax/jain/ss7/inap/datatype/Entry.java \
	javax/jain/ss7/inap/datatype/EventSpecificInformationBCSM.java \
	javax/jain/ss7/inap/datatype/ExtensionField.java \
	javax/jain/ss7/inap/datatype/Extension.java \
	javax/jain/ss7/inap/datatype/FacilityGroup.java \
	javax/jain/ss7/inap/datatype/FilteredCallTreatment.java \
	javax/jain/ss7/inap/datatype/FilteringCharacteristics.java \
	javax/jain/ss7/inap/datatype/FilteringCriteria.java \
	javax/jain/ss7/inap/datatype/FilteringTimeOut.java \
	javax/jain/ss7/inap/datatype/ForwardCallIndicators.java \
	javax/jain/ss7/inap/datatype/ForwardGVNS.java \
	javax/jain/ss7/inap/datatype/ForwardServiceInteractionInd.java \
	javax/jain/ss7/inap/datatype/GapCriteria.java \
	javax/jain/ss7/inap/datatype/GapIndicators.java \
	javax/jain/ss7/inap/datatype/GapOnService.java \
	javax/jain/ss7/inap/datatype/GapTreatment.java \
	javax/jain/ss7/inap/datatype/GenericNumbers.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0001.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0010.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0011.java \
	javax/jain/ss7/inap/datatype/GlobalTitle0100.java \
	javax/jain/ss7/inap/datatype/GlobalTitle.java \
	javax/jain/ss7/inap/datatype/HighLayerCompatibility.java \
	javax/jain/ss7/inap/datatype/InbandInfo.java \
	javax/jain/ss7/inap/datatype/InformationToRecord.java \
	javax/jain/ss7/inap/datatype/InformationToSend.java \
	javax/jain/ss7/inap/datatype/INServiceCompatibilityIndication.java \
	javax/jain/ss7/inap/datatype/IPAddressAndCallSegmentID.java \
	javax/jain/ss7/inap/datatype/IPAddressAndLegID.java \
	javax/jain/ss7/inap/datatype/LegID.java \
	javax/jain/ss7/inap/datatype/Leg.java \
	javax/jain/ss7/inap/datatype/LocationNumber.java \
	javax/jain/ss7/inap/datatype/MessageID.java \
	javax/jain/ss7/inap/datatype/MidCallControlInfo.java \
	javax/jain/ss7/inap/datatype/MidCallControlInformation.java \
	javax/jain/ss7/inap/datatype/MidCallInfoType.java \
	javax/jain/ss7/inap/datatype/MidCallSpecificInfo.java \
	javax/jain/ss7/inap/datatype/MiscCallInfo.java \
	javax/jain/ss7/inap/datatype/OriginalCalledPartyID.java \
	javax/jain/ss7/inap/datatype/PartyToConnect.java \
	javax/jain/ss7/inap/datatype/PartyToDisconnect.java \
	javax/jain/ss7/inap/datatype/ProfileIdentifier.java \
	javax/jain/ss7/inap/datatype/RedirectingPartyID.java \
	javax/jain/ss7/inap/datatype/RedirectionInformation.java \
	javax/jain/ss7/inap/datatype/RequestedInformation.java \
	javax/jain/ss7/inap/datatype/RequestedInformationList.java \
	javax/jain/ss7/inap/datatype/RequestedInformationTypeList.java \
	javax/jain/ss7/inap/datatype/RequestedInformationValue.java \
	javax/jain/ss7/inap/datatype/RequestedUTSI.java \
	javax/jain/ss7/inap/datatype/ResourceAddress.java \
	javax/jain/ss7/inap/datatype/ResourceID.java \
	javax/jain/ss7/inap/datatype/RouteList.java \
	javax/jain/ss7/inap/datatype/ScfID.java \
	javax/jain/ss7/inap/datatype/ServiceAddressInformation.java \
	javax/jain/ss7/inap/datatype/ServiceInteractionIndicatorsTwo.java \
	javax/jain/ss7/inap/datatype/Text.java \
	javax/jain/ss7/inap/datatype/Tone.java \
	javax/jain/ss7/inap/datatype/TransportAddress.java \
	javax/jain/ss7/inap/datatype/TriggerDataIdentifier.java \
	javax/jain/ss7/inap/datatype/VariableMessage.java \
	javax/jain/ss7/inap/datatype/VariablePart.java \
	javax/jain/ss7/inap/event/BeginIndEvent.java \
	javax/jain/ss7/inap/event/BeginReqEvent.java \
	javax/jain/ss7/inap/event/ComponentEvent.java \
	javax/jain/ss7/inap/event/ComponentIndEvent.java \
	javax/jain/ss7/inap/event/ComponentReqEvent.java \
	javax/jain/ss7/inap/event/ContinueIndEvent.java \
	javax/jain/ss7/inap/event/ContinueReqEvent.java \
	javax/jain/ss7/inap/event/DialogueEvent.java \
	javax/jain/ss7/inap/event/DialogueIndEvent.java \
	javax/jain/ss7/inap/event/DialogueReqEvent.java \
	javax/jain/ss7/inap/event/EndIndEvent.java \
	javax/jain/ss7/inap/event/EndReqEvent.java \
	javax/jain/ss7/inap/event/InapEvent.java \
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
	javax/jain/ss7/inap/exception/InvalidApplicationContextException.java \
	javax/jain/ss7/inap/exception/InvalidCallIDException.java \
	javax/jain/ss7/inap/exception/ParameterOutOfRangeException.java \
	javax/jain/ss7/inap/exception/TooManyListenersRegisteredException.java \
	javax/jain/ss7/inap/JainInapListener.java \
	javax/jain/ss7/inap/JainInapProvider.java \
	javax/jain/ss7/inap/JainInapStack.java \
	javax/jain/ss7/inap/operation/ActivateServiceFiltering.java \
	javax/jain/ss7/inap/operation/ActivityTest.java \
	javax/jain/ss7/inap/operation/AnalysedInformation.java \
	javax/jain/ss7/inap/operation/AnalyseInformation.java \
	javax/jain/ss7/inap/operation/ApplyCharging.java \
	javax/jain/ss7/inap/operation/ApplyChargingReport.java \
	javax/jain/ss7/inap/operation/AssistRequestInstructions.java \
	javax/jain/ss7/inap/operation/AuthorizeTermination.java \
	javax/jain/ss7/inap/operation/CallGap.java \
	javax/jain/ss7/inap/operation/CallInformationReport.java \
	javax/jain/ss7/inap/operation/CallInformationRequest.java \
	javax/jain/ss7/inap/operation/Cancel.java \
	javax/jain/ss7/inap/operation/CancelStatusReportRequest.java \
	javax/jain/ss7/inap/operation/CollectedInformation.java \
	javax/jain/ss7/inap/operation/CollectInformation.java \
	javax/jain/ss7/inap/operation/Connect.java \
	javax/jain/ss7/inap/operation/ConnectToResource.java \
	javax/jain/ss7/inap/operation/Continue.java \
	javax/jain/ss7/inap/operation/ContinueWithArgument.java \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociation.java \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociationResult.java \
	javax/jain/ss7/inap/operation/DFCWithArgument.java \
	javax/jain/ss7/inap/operation/DisconnectForwardConnection.java \
	javax/jain/ss7/inap/operation/DisconnectLeg.java \
	javax/jain/ss7/inap/operation/EntityReleased.java \
	javax/jain/ss7/inap/operation/EstablishTemporaryConnection.java \
	javax/jain/ss7/inap/operation/EventNotificationCharging.java \
	javax/jain/ss7/inap/operation/EventReportBCSM.java \
	javax/jain/ss7/inap/operation/EventReportFacility.java \
	javax/jain/ss7/inap/operation/FacilitySelectedAndAvailable.java \
	javax/jain/ss7/inap/operation/FurnishChargingInformation.java \
	javax/jain/ss7/inap/operation/HoldCallInNetwork.java \
	javax/jain/ss7/inap/operation/InitialDP.java \
	javax/jain/ss7/inap/operation/InitiateCallAttempt.java \
	javax/jain/ss7/inap/operation/ManageTriggerData.java \
	javax/jain/ss7/inap/operation/ManageTriggerDataResult.java \
	javax/jain/ss7/inap/operation/MergeCallSegments.java \
	javax/jain/ss7/inap/operation/MessageReceived.java \
	javax/jain/ss7/inap/operation/MoveCallSegments.java \
	javax/jain/ss7/inap/operation/MoveLeg.java \
	javax/jain/ss7/inap/operation/OAbandon.java \
	javax/jain/ss7/inap/operation/OAnswer.java \
	javax/jain/ss7/inap/operation/OCalledPartyBusy.java \
	javax/jain/ss7/inap/operation/ODisconnect.java \
	javax/jain/ss7/inap/operation/OMidCall.java \
	javax/jain/ss7/inap/operation/ONoAnswer.java \
	javax/jain/ss7/inap/operation/Operation.java \
	javax/jain/ss7/inap/operation/OriginationAttemptAuthorized.java \
	javax/jain/ss7/inap/operation/OriginationAttempt.java \
	javax/jain/ss7/inap/operation/OSuspended.java \
	javax/jain/ss7/inap/operation/PlayAnnouncement.java \
	javax/jain/ss7/inap/operation/PromptAndCollectUserInformation.java \
	javax/jain/ss7/inap/operation/PromptAndReceiveMessage.java \
	javax/jain/ss7/inap/operation/ReceivedInformation.java \
	javax/jain/ss7/inap/operation/Reconnect.java \
	javax/jain/ss7/inap/operation/ReleaseCall.java \
	javax/jain/ss7/inap/operation/ReportUTSI.java \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReport.java \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReportResult.java \
	javax/jain/ss7/inap/operation/RequestEveryStatusChangeReport.java \
	javax/jain/ss7/inap/operation/RequestFirstStatusMatchReport.java \
	javax/jain/ss7/inap/operation/RequestNotificationChargingEvent.java \
	javax/jain/ss7/inap/operation/RequestReportBCSMEvent.java \
	javax/jain/ss7/inap/operation/RequestReportFacilityEvent.java \
	javax/jain/ss7/inap/operation/RequestReportUTSI.java \
	javax/jain/ss7/inap/operation/ResetTimer.java \
	javax/jain/ss7/inap/operation/RouteSelectFailure.java \
	javax/jain/ss7/inap/operation/SelectFacility.java \
	javax/jain/ss7/inap/operation/SelectRoute.java \
	javax/jain/ss7/inap/operation/SendChargingInformation.java \
	javax/jain/ss7/inap/operation/SendFacilityInformation.java \
	javax/jain/ss7/inap/operation/SendSTUI.java \
	javax/jain/ss7/inap/operation/ServiceFilteringResponse.java \
	javax/jain/ss7/inap/operation/SpecializedResourceReport.java \
	javax/jain/ss7/inap/operation/SplitLeg.java \
	javax/jain/ss7/inap/operation/StatusReport.java \
	javax/jain/ss7/inap/operation/TAnswer.java \
	javax/jain/ss7/inap/operation/TBusy.java \
	javax/jain/ss7/inap/operation/TDisconnect.java \
	javax/jain/ss7/inap/operation/TerminationAttemptAuthorized.java \
	javax/jain/ss7/inap/operation/TerminationAttempt.java \
	javax/jain/ss7/inap/operation/TMidCall.java \
	javax/jain/ss7/inap/operation/TNoAnswer.java \
	javax/jain/ss7/inap/operation/TSuspended.java

my_javax_jain_ss7_inap_CLASSFILES = \
	$(my_javax_jain_ss7_inap_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_jain_ss7_inap_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_ss7_inap_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-jain-ss7-inap.jar
ALL_LOCAL	+= javax-jain-ss7-inap.jar
jar_DATA	+= javax-jain-ss7-inap-$(VERSION).jar
endif

javax-jain-ss7-inap-$(VERSION).jar: classdist_java.stamp javax-jain-ss7.jar $(my_javax_jain_ss7_inap_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_jain_ss7_inap_JAVAFILES)
#dist_java_JAVA += $(my_javax_jain_ss7_inap_JAVAFILES)

clean-javax_jain_ss7_inap:
	@d='javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_javax_jain_ss7_inap_CLASSFILES)
	rm -f -- javax-jain-ss7-inap.jar javax-jain-ss7-inap-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-inap.jar.db

CLEAN_LOCAL += clean-javax_jain_ss7_inap

nodist_lib_javax_jain_ss7_inap_jar_la_SOURCES	= javax-jain-ss7-inap.jar
lib_javax_jain_ss7_inap_jar_la_LIBADD		=
lib_javax_jain_ss7_inap_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_ss7_inap_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-javax-jain-ss7-inap.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-inap.jar.la
gcjlib_DATA		+= lib-javax-jain-ss7-inap.jar.db
ALL_LOCAL		+= lib-javax-jain-ss7-inap.jar.db
endif

lib-javax-jain-ss7-inap.jar.db: javax-jain-ss7-inap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

#############################################################################

my_javax_jain_protocol_ss7_map_JAVAFILES = \
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
	javax/jain/protocol/ss7/map/MapErrorEvent.java

my_javax_jain_protocol_ss7_map_CLASSFILES = \
	$(my_javax_jain_protocol_ss7_map_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_jain_protocol_ss7_map_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jain_protocol_ss7_map_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-jain-protocol-ss7-map.jar
ALL_LOCAL	+= javax-jain-protocol-ss7-map.jar
jar_DATA	+= javax-jain-protocol-ss7-map-$(VERSION).jar
endif

javax-jain-protocol-ss7-map-$(VERSION).jar: classdist_java.stamp javax-jain-protocol-ss7.jar $(my_javax_jain_protocol_ss7_map_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_jain_protocol_ss7_map_JAVAFILES)
#dist_java_JAVA += $(my_javax_jain_protocol_ss7_map_JAVAFILES)

clean-javax_jain_protocol_ss7_map:
	@d='javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-protocol-ss7-map.jar
	rm -f -- javax-jain-protocol-ss7-map-$(VERSION).jar
	rm -f -- lib-javax-jain-protocol-ss7-map.jar.db

CLEAN_LOCAL += clean-javax_jain_protocol_ss7_map

nodist_lib_javax_jain_protocol_ss7_map_jar_la_SOURCES	= javax-jain-protocol-ss7-map.jar
lib_javax_jain_protocol_ss7_map_jar_la_LIBADD		=
lib_javax_jain_protocol_ss7_map_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jain_protocol_ss7_map_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-javax-jain-protocol-ss7-map.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-javax-jain-protocol-ss7-map.jar.la
gcjlib_DATA		+= lib-javax-jain-protocol-ss7-map.jar.db
ALL_LOCAL		+= lib-javax-jain-protocol-ss7-map.jar.db
endif

lib-javax-jain-protocol-ss7-map.jar.db: javax-jain-protocol-ss7-map.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

#############################################################################

my_javax_sip_JAVAFILES = \
	javax/sip/address/AddressFactory.java \
	javax/sip/address/Address.java \
	javax/sip/address/Hop.java \
	javax/sip/address/Router.java \
	javax/sip/address/SipURI.java \
	javax/sip/address/TelURL.java \
	javax/sip/address/URI.java \
	javax/sip/ClientTransaction.java \
	javax/sip/DialogDoesNotExistException.java \
	javax/sip/Dialog.java \
	javax/sip/DialogState.java \
	javax/sip/DialogTerminatedEvent.java \
	javax/sip/header/AcceptEncodingHeader.java \
	javax/sip/header/AcceptHeader.java \
	javax/sip/header/AcceptLanguageHeader.java \
	javax/sip/header/AlertInfoHeader.java \
	javax/sip/header/AllowEventsHeader.java \
	javax/sip/header/AllowHeader.java \
	javax/sip/header/AuthenticationInfoHeader.java \
	javax/sip/header/AuthorizationHeader.java \
	javax/sip/header/CallIdHeader.java \
	javax/sip/header/CallInfoHeader.java \
	javax/sip/header/ContactHeader.java \
	javax/sip/header/ContentDispositionHeader.java \
	javax/sip/header/ContentEncodingHeader.java \
	javax/sip/header/ContentLanguageHeader.java \
	javax/sip/header/ContentLengthHeader.java \
	javax/sip/header/ContentTypeHeader.java \
	javax/sip/header/CSeqHeader.java \
	javax/sip/header/DateHeader.java \
	javax/sip/header/Encoding.java \
	javax/sip/header/ErrorInfoHeader.java \
	javax/sip/header/EventHeader.java \
	javax/sip/header/ExpiresHeader.java \
	javax/sip/header/ExtensionHeader.java \
	javax/sip/header/FromHeader.java \
	javax/sip/header/HeaderAddress.java \
	javax/sip/header/HeaderFactory.java \
	javax/sip/header/Header.java \
	javax/sip/header/InReplyToHeader.java \
	javax/sip/header/MaxForwardsHeader.java \
	javax/sip/header/MediaType.java \
	javax/sip/header/MimeVersionHeader.java \
	javax/sip/header/MinExpiresHeader.java \
	javax/sip/header/OptionTag.java \
	javax/sip/header/OrganizationHeader.java \
	javax/sip/header/Parameters.java \
	javax/sip/header/PriorityHeader.java \
	javax/sip/header/ProxyAuthenticateHeader.java \
	javax/sip/header/ProxyAuthorizationHeader.java \
	javax/sip/header/ProxyRequireHeader.java \
	javax/sip/header/RAckHeader.java \
	javax/sip/header/ReasonHeader.java \
	javax/sip/header/RecordRouteHeader.java \
	javax/sip/header/ReferToHeader.java \
	javax/sip/header/ReplyToHeader.java \
	javax/sip/header/RequireHeader.java \
	javax/sip/header/RetryAfterHeader.java \
	javax/sip/header/RouteHeader.java \
	javax/sip/header/RSeqHeader.java \
	javax/sip/header/ServerHeader.java \
	javax/sip/header/SIPETagHeader.java \
	javax/sip/header/SIPIfMatchHeader.java \
	javax/sip/header/SubjectHeader.java \
	javax/sip/header/SubscriptionStateHeader.java \
	javax/sip/header/SupportedHeader.java \
	javax/sip/header/TimeStampHeader.java \
	javax/sip/header/ToHeader.java \
	javax/sip/header/TooManyHopsException.java \
	javax/sip/header/UnsupportedHeader.java \
	javax/sip/header/UserAgentHeader.java \
	javax/sip/header/ViaHeader.java \
	javax/sip/header/WarningHeader.java \
	javax/sip/header/WWWAuthenticateHeader.java \
	javax/sip/InvalidArgumentException.java \
	javax/sip/IOExceptionEvent.java \
	javax/sip/ListeningPoint.java \
	javax/sip/message/MessageFactory.java \
	javax/sip/message/Message.java \
	javax/sip/message/Request.java \
	javax/sip/message/Response.java \
	javax/sip/ObjectInUseException.java \
	javax/sip/PeerUnavailableException.java \
	javax/sip/ProviderDoesNotExistException.java \
	javax/sip/RequestEvent.java \
	javax/sip/ResponseEvent.java \
	javax/sip/ServerTransaction.java \
	javax/sip/SipException.java \
	javax/sip/SipFactory.java \
	javax/sip/SipListener.java \
	javax/sip/SipProvider.java \
	javax/sip/SipStack.java \
	javax/sip/TimeoutEvent.java \
	javax/sip/Timeout.java \
	javax/sip/TransactionAlreadyExistsException.java \
	javax/sip/TransactionDoesNotExistException.java \
	javax/sip/Transaction.java \
	javax/sip/TransactionState.java \
	javax/sip/TransactionTerminatedEvent.java \
	javax/sip/TransactionUnavailableException.java \
	javax/sip/TransportAlreadySupportedException.java \
	javax/sip/TransportNotSupportedException.java

my_javax_sip_CLASSFILES = \
	$(my_javax_sip_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_sip_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_sip_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-sip.jar
ALL_LOCAL	+= javax-sip.jar
jar_DATA	+= javax-sip-$(VERSION).jar
endif

javax-sip-$(VERSION).jar: classdist_java.stamp $(my_javax_sip_CLASSFILES)
	$(AM_V_GEN)d='javax/sip' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_sip_JAVAFILES)
#dist_java_JAVA += $(my_javax_sip_JAVAFILES)

clean-javax_sip:
	@d='javax/sip' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-sip.jar
	rm -f -- javax-sip-$(VERSION).jar
	rm -f -- lib-javax-sip.jar.db

CLEAN_LOCAL += clean-javax_sip

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
	$(my_javax_csapi_cc_jcc_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_csapi_cc_jcc_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_csapi_cc_jcc_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-csapi-cc-jcc.jar
ALL_LOCAL	+= javax-csapi-cc-jcc.jar
jar_DATA	+= javax-csapi-cc-jcc-$(VERSION).jar
endif

javax-csapi-cc-jcc-$(VERSION).jar: classdist_java.stamp $(my_javax_csapi_cc_jcc_CLASSFILES)
	$(AM_V_GEN)d='javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_csapi_cc_jcc_JAVAFILES)
#dist_java_JAVA += $(my_javax_csapi_cc_jcc_JAVAFILES)

clean-javax_csapi_cc_jcc:
	@d='javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-csapi-cc-jcc.jar
	rm -f -- javax-csapi-cc-jcc-$(VERSION).jar
	rm -f -- lib-javax-csapi-cc-jcc.jar.db

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
	$(my_javax_jcat_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_javax_jcat_CLASSFILES)
my_jar_CLASSFILES += $(my_javax_jcat_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= javax-jcat.jar
ALL_LOCAL	+= javax-jcat.jar
jar_DATA	+= javax-jcat-$(VERSION).jar
endif

javax-jcat-$(VERSION).jar: classdist_java.stamp $(my_javax_jcat_CLASSFILES)
	$(AM_V_GEN)d='javax/jcat' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_javax_jcat_JAVAFILES)
#dist_java_JAVA += $(my_javax_jcat_JAVAFILES)

clean-javax_jcat:
	@d='javax/jcat' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jcat.jar
	rm -f -- javax-jcat-$(VERSION).jar
	rm -f -- lib-javax-jcat.jar.db

CLEAN_LOCAL += clean-javax_jcat

nodist_lib_javax_jcat_jar_la_SOURCES	= javax-jcat.jar
lib_javax_jcat_jar_la_LIBADD		=
lib_javax_jcat_jar_la_LDFLAGS		= -module -avoid-version -shared
lib_javax_jcat_jar_la_GCJFLAGS		= $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-javax-jcat.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-javax-jcat.jar.la
gcjlib_DATA		+= lib-javax-jcat.jar.db
ALL_LOCAL		+= lib-javax-jcat.jar.db
endif

lib-javax-jcat.jar.db: javax-jcat.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

#############################################################################

my_jain_protocol_ss7_tcap_JAVAFILES = \
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
	$(my_jain_protocol_ss7_tcap_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_jain_protocol_ss7_tcap_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_protocol_ss7_tcap_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= jain-protocol-ss7-tcap.jar
ALL_LOCAL	+= jain-protocol-ss7-tcap.jar
jar_DATA	+= jain-protocol-ss7-tcap-$(VERSION).jar
endif

jain-protocol-ss7-tcap-$(VERSION).jar: classdist_java.stamp jain-protocol-ss7.jar $(my_jain_protocol_ss7_tcap_CLASSFILES)
	$(AM_V_GEN)d='jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_jain_protocol_ss7_tcap_JAVAFILES)
#dist_java_JAVA += $(my_jain_protocol_ss7_tcap_JAVAFILES)

clean-jain_protocol_ss7_tcap:
	@d='jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain-protocol-ss7-tcap.jar
	rm -f -- jain-protocol-ss7-tcap-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7-tcap.jar.db

CLEAN_LOCAL += clean-jain_protocol_ss7_tcap

nodist_lib_jain_protocol_ss7_tcap_jar_la_SOURCES = jain-protocol-ss7-tcap.jar
lib_jain_protocol_ss7_tcap_jar_la_LIBADD	  =
lib_jain_protocol_ss7_tcap_jar_la_LDFLAGS	  = -module -avoid-version -shared
lib_jain_protocol_ss7_tcap_jar_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-jain-protocol-ss7-tcap.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-jain-protocol-ss7-tcap.jar.la
gcjlib_DATA		+= lib-jain-protocol-ss7-tcap.jar.db
ALL_LOCAL		+= lib-jain-protocol-ss7-tcap.jar.db
endif

lib-jain-protocol-ss7-tcap.jar.db: jain-protocol-ss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

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
	$(my_jain_protocol_ss7_oam_JAVAFILES:%.java=%.class)

MY_SECONDARY	+= $(my_jain_protocol_ss7_oam_CLASSFILES)
my_jar_CLASSFILES += $(my_jain_protocol_ss7_oam_CLASSFILES)
if PKG_BUILD_ARCH
my_jar_LINKS	+= jain-protocol-ss7-oam.jar
ALL_LOCAL	+= jain-protocol-ss7-oam.jar
jar_DATA	+= jain-protocol-ss7-oam-$(VERSION).jar
endif

jain-protocol-ss7-oam-$(VERSION).jar: classdist_java.stamp $(my_jain_protocol_ss7_oam_CLASSFILES)
	$(AM_V_GEN)d='jain/protocol/ss7/oam' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES += $(my_jain_protocol_ss7_oam_JAVAFILES)
#dist_java_JAVA += $(my_jain_protocol_ss7_oam_JAVAFILES)

clean-jain_protocol_ss7_oam:
	@d='jain/protocol/ss7/oam' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain-protocol-ss7-oam.jar
	rm -f -- jain-protocol-ss7-oam-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7-oam.jar.db

CLEAN_LOCAL += clean-jain_protocol_ss7_oam

EXTRA_DIST += \
	javax/management/NotificationBroadcasterSupport.class \
	javax/management/MBeanServerDelegateMBean.class \
	javax/management/QualifiedAttributeValueExp.class \
	javax/management/Descriptor.class \
	javax/management/BooleanValueExp.class \
	javax/management/NotificationListener.class \
	javax/management/StringValueExp.class \
	javax/management/MBeanServerFactory$$1.class \
	javax/management/RuntimeMBeanException.class \
	javax/management/MBeanServerFactory.class \
	javax/management/ServiceNotFoundException.class \
	javax/management/remote/JMXProviderException.class \
	javax/management/remote/JMXServiceURL.class \
	javax/management/remote/JMXConnectorServerMBean.class \
	javax/management/remote/JMXAuthenticator.class \
	javax/management/remote/JMXConnectorServerFactory.class \
	javax/management/remote/message/HandshakeEndMessage.class \
	javax/management/remote/message/TLSMessage.class \
	javax/management/remote/message/NotificationRequestMessage.class \
	javax/management/remote/message/Message.class \
	javax/management/remote/message/MBeanServerResponseMessage.class \
	javax/management/remote/message/HandshakeErrorMessage.class \
	javax/management/remote/message/SASLMessage.class \
	javax/management/remote/message/MBeanServerRequestMessage.class \
	javax/management/remote/message/CloseMessage.class \
	javax/management/remote/message/HandshakeBeginMessage.class \
	javax/management/remote/message/VersionMessage.class \
	javax/management/remote/message/NotificationResponseMessage.class \
	javax/management/remote/message/ProfileMessage.class \
	javax/management/remote/JMXConnectorServer.class \
	javax/management/remote/JMXConnectionNotification.class \
	javax/management/remote/rmi/RMIConnector$$2.class \
	javax/management/remote/rmi/RMIConnectionImpl$$RMIServerCommunicatorAdmin.class \
	javax/management/remote/rmi/RMIConnector$$RemoteMBeanServerConnection.class \
	javax/management/remote/rmi/RMIConnector$$ObjectInputStreamWithLoader.class \
	javax/management/remote/rmi/RMIConnectionImpl$$PrivilegedOperation.class \
	javax/management/remote/rmi/RMIConnector$$RMINotifClient.class \
	javax/management/remote/rmi/RMIConnectionImpl$$6.class \
	javax/management/remote/rmi/RMIConnectionImpl$$5.class \
	javax/management/remote/rmi/RMIConnectionImpl$$1.class \
	javax/management/remote/rmi/RMIConnection.class \
	javax/management/remote/rmi/RMIConnectionImpl$$4.class \
	javax/management/remote/rmi/RMIConnector$$4.class \
	javax/management/remote/rmi/RMIConnectionImpl$$7.class \
	javax/management/remote/rmi/RMIIIOPServerImpl.class \
	javax/management/remote/rmi/RMIJRMPServerImpl.class \
	javax/management/remote/rmi/RMIServerImpl_Stub.class \
	javax/management/remote/rmi/RMIConnectionImpl$$2.class \
	javax/management/remote/rmi/RMIConnectionImpl_Stub.class \
	javax/management/remote/rmi/RMIServerImpl.class \
	javax/management/remote/rmi/RMIServer.class \
	javax/management/remote/rmi/RMIConnector$$3.class \
	javax/management/remote/rmi/RMIConnector$$RMIClientCommunicatorAdmin.class \
	javax/management/remote/rmi/RMIConnectorServer.class \
	javax/management/remote/rmi/RMIConnectionImpl.class \
	javax/management/remote/rmi/RMIConnector.class \
	javax/management/remote/rmi/RMIConnectionImpl$$3.class \
	javax/management/remote/rmi/RMIConnector$$1.class \
	javax/management/remote/rmi/RMIConnectionImpl$$8.class \
	javax/management/remote/TargetedNotification.class \
	javax/management/remote/jmxmp/JMXMPConnectorServer.class \
	javax/management/remote/jmxmp/JMXMPConnector.class \
	javax/management/remote/JMXConnectorFactory.class \
	javax/management/remote/JMXConnectorFactory$$1.class \
	javax/management/remote/JMXConnectorServerProvider.class \
	javax/management/remote/SubjectDelegationPermission.class \
	javax/management/remote/JMXServerErrorException.class \
	javax/management/remote/MBeanServerForwarder.class \
	javax/management/remote/JMXConnectorProvider.class \
	javax/management/remote/JMXConnectorFactory$$3.class \
	javax/management/remote/NotificationResult.class \
	javax/management/remote/JMXPrincipal.class \
	javax/management/remote/JMXConnector.class \
	javax/management/remote/JMXConnectorFactory$$2.class \
	javax/management/remote/generic/ServerIntermediary$$PrivilegedRequestJob.class \
	javax/management/remote/generic/GenericConnector$$RequestHandler.class \
	javax/management/remote/generic/ClientIntermediary$$GenericClientCommunicatorAdmin.class \
	javax/management/remote/generic/ObjectWrapping.class \
	javax/management/remote/generic/GenericConnector$$1.class \
	javax/management/remote/generic/GenericConnectorServer.class \
	javax/management/remote/generic/GenericConnectorServer$$Receiver.class \
	javax/management/remote/generic/ServerIntermediary$$GenericServerCommunicatorAdmin.class \
	javax/management/remote/generic/ClientIntermediary.class \
	javax/management/remote/generic/ServerIntermediary$$RequestHandler.class \
	javax/management/remote/generic/GenericConnectorServer$$1.class \
	javax/management/remote/generic/ServerIntermediary$$5.class \
	javax/management/remote/generic/ServerIntermediary$$4.class \
	javax/management/remote/generic/MessageConnection.class \
	javax/management/remote/generic/ClientIntermediary$$GenericClientNotifForwarder.class \
	javax/management/remote/generic/ConnectionClosedException.class \
	javax/management/remote/generic/GenericConnector$$RemoteMBeanServerConnection.class \
	javax/management/remote/generic/MessageConnectionServer.class \
	javax/management/remote/generic/GenericConnector.class \
	javax/management/remote/generic/GenericConnector$$ResponseMsgWrapper.class \
	javax/management/remote/generic/GenericConnectorServer$$ClientCreation.class \
	javax/management/remote/generic/ServerIntermediary$$1.class \
	javax/management/remote/generic/ServerIntermediary$$3.class \
	javax/management/remote/generic/ServerIntermediary.class \
	javax/management/remote/generic/ServerIntermediary$$2.class \
	javax/management/remote/generic/ServerIntermediary$$6.class \
	javax/management/OrQueryExp.class \
	javax/management/NumericValueExp.class \
	javax/management/InstanceNotFoundException.class \
	javax/management/monitor/GaugeMonitorMBean.class \
	javax/management/monitor/StringMonitor.class \
	javax/management/monitor/CounterMonitor.class \
	javax/management/monitor/GaugeMonitor.class \
	javax/management/monitor/GaugeAlarmClock.class \
	javax/management/monitor/MonitorNotification.class \
	javax/management/monitor/MonitorMBean.class \
	javax/management/monitor/StringAlarmClock.class \
	javax/management/monitor/Monitor.class \
	javax/management/monitor/CounterAlarmClock.class \
	javax/management/monitor/StringMonitorMBean.class \
	javax/management/monitor/MonitorSettingException.class \
	javax/management/monitor/CounterMonitorMBean.class \
	javax/management/AndQueryExp.class \
	javax/management/ObjectName.class \
	javax/management/MBeanNotificationInfo.class \
	javax/management/InstanceAlreadyExistsException.class \
	javax/management/MBeanAttributeInfo.class \
	javax/management/MBeanFeatureInfo.class \
	javax/management/ValueExp.class \
	javax/management/MBeanParameterInfo.class \
	javax/management/MBeanServerPermission.class \
	javax/management/BadAttributeValueExpException.class \
	javax/management/BetweenQueryExp.class \
	javax/management/MBeanServer.class \
	javax/management/JMException.class \
	javax/management/InQueryExp.class \
	javax/management/AttributeValueExp.class \
	javax/management/BadBinaryOpValueExpException.class \
	javax/management/NotificationBroadcaster.class \
	javax/management/AttributeNotFoundException.class \
	javax/management/MBeanServerDelegate.class \
	javax/management/Query.class \
	javax/management/loading/MLetObjectInputStream.class \
	javax/management/loading/MLetParser.class \
	javax/management/loading/MLetMBean.class \
	javax/management/loading/MLetContent.class \
	javax/management/loading/ClassLoaderRepository.class \
	javax/management/loading/MLet.class \
	javax/management/loading/DefaultLoaderRepository.class \
	javax/management/BinaryOpValueExp.class \
	javax/management/relation/RelationTypeNotFoundException.class \
	javax/management/relation/InvalidRoleInfoException.class \
	javax/management/relation/RoleStatus.class \
	javax/management/relation/RelationSupport.class \
	javax/management/relation/RoleNotFoundException.class \
	javax/management/relation/RoleInfo.class \
	javax/management/relation/RoleResult.class \
	javax/management/relation/InvalidRelationServiceException.class \
	javax/management/relation/RelationServiceNotRegisteredException.class \
	javax/management/relation/MBeanServerNotificationFilter.class \
	javax/management/relation/RelationNotification.class \
	javax/management/relation/RoleUnresolvedList.class \
	javax/management/relation/RelationSupportMBean.class \
	javax/management/relation/RelationNotFoundException.class \
	javax/management/relation/Relation.class \
	javax/management/relation/InvalidRelationTypeException.class \
	javax/management/relation/RelationException.class \
	javax/management/relation/RoleInfoNotFoundException.class \
	javax/management/relation/InvalidRelationIdException.class \
	javax/management/relation/RoleUnresolved.class \
	javax/management/relation/RoleList.class \
	javax/management/relation/InvalidRoleValueException.class \
	javax/management/relation/RelationTypeSupport.class \
	javax/management/relation/RelationService.class \
	javax/management/relation/Role.class \
	javax/management/relation/RelationType.class \
	javax/management/relation/RelationServiceMBean.class \
	javax/management/MBeanRegistrationException.class \
	javax/management/Notification.class \
	javax/management/ReflectionException.class \
	javax/management/MBeanServerNotification.class \
	javax/management/QueryExp.class \
	javax/management/DescriptorAccess.class \
	javax/management/MBeanOperationInfo.class \
	javax/management/MBeanException.class \
	javax/management/InvalidAttributeValueException.class \
	javax/management/AttributeList.class \
	javax/management/DynamicMBean.class \
	javax/management/InvalidApplicationException.class \
	javax/management/IntrospectionException.class \
	javax/management/MalformedObjectNameException.class \
	javax/management/RuntimeOperationsException.class \
	javax/management/ObjectInstance.class \
	javax/management/ClassAttributeValueExp.class \
	javax/management/NotificationFilter.class \
	javax/management/NotificationFilterSupport.class \
	javax/management/BinaryRelQueryExp.class \
	javax/management/RuntimeErrorException.class \
	javax/management/MBeanServerFactory$$PublicClassLoaderRepository.class \
	javax/management/MatchQueryExp.class \
	javax/management/MBeanInfo.class \
	javax/management/OperationsException.class \
	javax/management/NotCompliantMBeanException.class \
	javax/management/timer/TimerAlarmClockNotification.class \
	javax/management/timer/TimerNotification.class \
	javax/management/timer/Timer.class \
	javax/management/timer/TimerMBean.class \
	javax/management/timer/TimerAlarmClock.class \
	javax/management/DefaultLoaderRepository.class \
	javax/management/AttributeChangeNotification.class \
	javax/management/QueryEval.class \
	javax/management/ListenerNotFoundException.class \
	javax/management/openmbean/OpenMBeanAttributeInfoSupport.class \
	javax/management/openmbean/OpenMBeanParameterInfo.class \
	javax/management/openmbean/InvalidOpenTypeException.class \
	javax/management/openmbean/TabularType.class \
	javax/management/openmbean/CompositeData.class \
	javax/management/openmbean/OpenMBeanConstructorInfoSupport.class \
	javax/management/openmbean/OpenDataException.class \
	javax/management/openmbean/OpenMBeanOperationInfo.class \
	javax/management/openmbean/KeyAlreadyExistsException.class \
	javax/management/openmbean/OpenType.class \
	javax/management/openmbean/TabularData.class \
	javax/management/openmbean/OpenMBeanInfoSupport.class \
	javax/management/openmbean/OpenMBeanInfo.class \
	javax/management/openmbean/InvalidKeyException.class \
	javax/management/openmbean/ArrayType.class \
	javax/management/openmbean/TabularDataSupport.class \
	javax/management/openmbean/OpenMBeanAttributeInfo.class \
	javax/management/openmbean/OpenMBeanParameterInfoSupport.class \
	javax/management/openmbean/CompositeType.class \
	javax/management/openmbean/SimpleType.class \
	javax/management/openmbean/CompositeDataSupport.class \
	javax/management/openmbean/OpenMBeanOperationInfoSupport.class \
	javax/management/openmbean/OpenMBeanConstructorInfo.class \
	javax/management/modelmbean/InvalidTargetObjectTypeException.class \
	javax/management/modelmbean/ModelMBeanConstructorInfo.class \
	javax/management/modelmbean/ModelMBeanInfoSupport.class \
	javax/management/modelmbean/ModelMBeanInfo.class \
	javax/management/modelmbean/XMLParseException.class \
	javax/management/modelmbean/DescriptorSupport.class \
	javax/management/modelmbean/RequiredModelMBean.class \
	javax/management/modelmbean/ModelMBeanNotificationInfo.class \
	javax/management/modelmbean/ModelMBean.class \
	javax/management/modelmbean/ModelMBeanOperationInfo.class \
	javax/management/modelmbean/ModelMBeanAttributeInfo.class \
	javax/management/modelmbean/ModelMBeanNotificationBroadcaster.class \
	javax/management/AttributeChangeNotificationFilter.class \
	javax/management/MBeanConstructorInfo.class \
	javax/management/PersistentMBean.class \
	javax/management/MBeanRegistration.class \
	javax/management/NotificationBroadcasterSupport$$ListenerInfo.class \
	javax/management/NotQueryExp.class \
	javax/management/Attribute.class \
	javax/management/JMRuntimeException.class \
	javax/management/BadStringOperationException.class

############################################################################


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
	$(my_oam_JAVAFILES:%.java=%.class)
my_oam_SOURCEFILES = \
	$(my_oam_JAVAFILES:%.java=%.cc)
my_oam_INCLUDES = \
	$(my_oam_JAVAFILES:%.java=%.h)
my_oam_HEADERCLASSES = \
	$(my_oam_JAVAFILES:%.java=%.class)

## nodist_lib_org_openss7_jain_protocol_ss7_oam_jar_la_SOURCES = org-openss7-jain-protocol-ss7-oam.jar
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_SOURCES	     = $(my_oam_INCLUDES) $(my_oam_SOURCEFILES)
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_CPPFLAGS	     = $(GCJLIB_INCLUDES) -fno-strict-aliasing
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_CFLAGS	     = $(USER_CFLAGS) $(USER_DFLAGS)
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_LIBADD	     = $(top_builddir)/libstreams.la
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_LDFLAGS	     = -module -avoid-version -shared
## lib_org_openss7_jain_protocol_ss7_oam_jar_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(CNIFLAGS)
## 
## ##EXTRA_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-oam.jar.la
## if PKG_BUILD_ARCH
## gcjlib_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-oam.jar.la
## gcjlib_DATA		+= lib-org-openss7-jain-protocol-ss7-oam.jar.db
## ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-oam.jar.db
## endif
## 
## lib-org-openss7-jain-protocol-ss7-oam.jar.db: org-openss7-jain-protocol-ss7-oam.jar
## 	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so
## 
## java_cni_CLASSES	+= $(my_oam_CLASSFILES)
## 
## MY_INTERMEDIATE += $(my_oam_HEADERCLASSES)
## 
## if PKG_BUILD_ARCH
## my_jar_LINKS += org-openss7-jain-protocol-ss7-oam.jar
## ALL_LOCAL += org-openss7-jain-protocol-ss7-oam.jar
## jar_DATA += org-openss7-jain-protocol-ss7-oam-$(VERSION).jar
## endif
## 
## org-openss7-jain-protocol-ss7-oam-$(VERSION).jar: classdist_java.stamp $(my_oam_CLASSFILES)
##	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
##	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
##	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@
## 
## MY_SECONDARY += $(my_oam_CLASSFILES)
## my_jar_CLASSFILES += $(my_oam_CLASSFILES)
## 
## java_JAVAFILES += $(my_oam_JAVAFILES)
## #dist_java_JAVA += $(my_oam_JAVAFILES)
## 
## clean-org_openss7_jain_protocol_ss7_oam:
## 	rm -f -- $(my_oam_CLASSFILES)
## 	rm -f -- $(my_oam_INCLUDES)
## 	rm -f -- $(my_oam_HEADERCLASSES)
## 	rm -f -- org-openss7-jain-protocol-ss7-oam.jar org-openss7-jain-protocol-ss7-oam-$(VERSION).jar
## 	rm -f -- lib-org-openss7-jain-protocol-ss7-oam.jar.db
## 
## CLEAN_LOCAL += clean-org_openss7_jain_protocol_ss7_oam

#############################################################################

my_jtcap_JAVAFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.java \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.java
my_jtcap_CLASSFILES = \
	$(my_jtcap_JAVAFILES:%.java=%.class)
my_jtcap_SOURCEFILES = \
	$(my_jtcap_JAVAFILES:%.java=%.cc)
my_jtcap_INCLUDES = \
	$(my_jtcap_JAVAFILES:%.java=%.h)
my_jtcap_HEADERCLASSES = \
	$(my_jtcap_JAVAFILES:%.java=%.class)

nodist_lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES = org-openss7-jain-protocol-ss7-tcap.jar
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES	      = $(my_jtcap_INCLUDES) $(my_jtcap_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CPPFLAGS	      = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LIBADD	      = $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LDFLAGS	      = -module -avoid-version -shared
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-tcap.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-tcap.jar.la
gcjlib_DATA		+= lib-org-openss7-jain-protocol-ss7-tcap.jar.db
ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-tcap.jar.db
endif

lib-org-openss7-jain-protocol-ss7-tcap.jar.db: org-openss7-jain-protocol-ss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jtcap_CLASSFILES)

MY_INTERMEDIATE += $(my_jtcap_HEADERCLASSES)


if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-jain-protocol-ss7-tcap.jar
ALL_LOCAL += org-openss7-jain-protocol-ss7-tcap.jar
jar_DATA += org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar
endif

org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar: classdist_java.stamp $(my_jtcap_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jtcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jtcap_CLASSFILES)
java_JAVAFILES		+= $(my_jtcap_JAVAFILES)
#dist_java_JAVA		+= $(my_jtcap_JAVAFILES)

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
	$(my_jsip_JAVAFILES:%.java=%.class)
my_jsip_SOURCEFILES = \
	$(my_jsip_JAVAFILES:%.java=%.cc)
my_jsip_INCLUDES = \
	$(my_jsip_JAVAFILES:%.java=%.h)
my_jsip_HEADERCLASSES = \
	$(my_jsip_JAVAFILES:%.java=%.class)

nodist_lib_org_openss7_javax_sip_jar_la_SOURCES = org-openss7-javax-sip.jar
lib_org_openss7_javax_sip_jar_la_SOURCES	 = $(my_jsip_INCLUDES) $(my_jsip_SOURCEFILES)
lib_org_openss7_javax_sip_jar_la_CPPFLAGS	 = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_sip_jar_la_CFLAGS		 = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_sip_jar_la_LIBADD		 = $(top_builddir)/libstreams.la
lib_org_openss7_javax_sip_jar_la_LDFLAGS	 = -module -avoid-version -shared
lib_org_openss7_javax_sip_jar_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-sip.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-sip.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-sip.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-sip.jar.db
endif

lib-org-openss7-javax-sip.jar.db: org-openss7-javax-sip.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jsip_CLASSFILES)

MY_INTERMEDIATE += $(my_jsip_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-sip.jar
ALL_LOCAL += org-openss7-javax-sip.jar
jar_DATA += org-openss7-javax-sip-$(VERSION).jar
endif

org-openss7-javax-sip-$(VERSION).jar: classdist_java.stamp $(my_jsip_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jsip_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jsip_CLASSFILES)

java_JAVAFILES		+= $(my_jsip_JAVAFILES)
#dist_java_JAVA		+= $(my_jsip_JAVAFILES)

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
	$(my_jcat_JAVAFILES:%.java=%.class)
my_jcat_SOURCEFILES = \
	$(my_jcat_JAVAFILES:%.java=%.cc)
my_jcat_INCLUDES = \
	$(my_jcat_JAVAFILES:%.java=%.h) \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.h \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.h
my_jcat_HEADERCLASSES = \
	$(my_jcat_JAVAFILES:%.java=%.class) \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.class \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.class

nodist_lib_org_openss7_javax_jcat_jar_la_SOURCES	= org-openss7-javax-jcat.jar
lib_org_openss7_javax_jcat_jar_la_SOURCES	 = $(my_jcat_INCLUDES) $(my_jcat_SOURCEFILES)
lib_org_openss7_javax_jcat_jar_la_CPPFLAGS	 = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jcat_jar_la_CFLAGS	 = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jcat_jar_la_LIBADD	 = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jcat_jar_la_LDFLAGS	 = -module -avoid-version -shared
lib_org_openss7_javax_jcat_jar_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-jcat.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jcat.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jcat.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jcat.jar.db
endif

lib-org-openss7-javax-jcat.jar.db: org-openss7-javax-jcat.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jcat_CLASSFILES)

MY_INTERMEDIATE += $(my_jcat_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-jcat.jar
ALL_LOCAL += org-openss7-javax-jcat.jar
jar_DATA += org-openss7-javax-jcat-$(VERSION).jar
endif

org-openss7-javax-jcat-$(VERSION).jar: classdist_java.stamp $(my_jcat_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jcat_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jcat_CLASSFILES)

java_JAVAFILES		+= $(my_jcat_JAVAFILES)
#dist_java_JAVA		+= $(my_jcat_JAVAFILES)

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
	$(my_jisup_JAVAFILES:%.java=%.class)
my_jisup_SOURCEFILES = \
	$(my_jisup_JAVAFILES:%.java=%.cc)
my_jisup_INCLUDES = \
	$(my_jisup_JAVAFILES:%.java=%.h)

nodist_lib_org_openss7_javax_jain_ss7_isup_jar_la_SOURCES = org-openss7-javax-jain-ss7-isup.jar
lib_org_openss7_javax_jain_ss7_isup_jar_la_SOURCES	   = $(my_jisup_INCLUDES) $(my_jisup_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_isup_jar_la_CPPFLAGS	   = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_isup_jar_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_ss7_isup_jar_la_LIBADD	   = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_isup_jar_la_LDFLAGS	   = -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_isup_jar_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-isup.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-isup.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-ss7-isup.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-isup.jar.db
endif

lib-org-openss7-javax-jain-ss7-isup.jar.db: org-openss7-javax-jain-ss7-isup.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jisup_CLASSFILES)

MY_INTERMEDIATE += $(my_jisup_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-jain-ss7-isup.jar
ALL_LOCAL += org-openss7-javax-jain-ss7-isup.jar
jar_DATA += org-openss7-javax-jain-ss7-isup-$(VERSION).jar
endif

org-openss7-javax-jain-ss7-isup-$(VERSION).jar: classdist_java.stamp $(my_jisup_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jisup_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jisup_CLASSFILES)

java_JAVAFILES		+= $(my_jisup_JAVAFILES)
#dist_java_JAVA		+= $(my_jisup_JAVAFILES)

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
	$(my_jinap_JAVAFILES:%.java=%.class)
my_jinap_SOURCEFILES = \
	$(my_jinap_JAVAFILES:%.java=%.cc)
my_jinap_INCLUDES = \
	$(my_jinap_JAVAFILES:%.java=%.h)
my_jinap_HEADERCLASSES = \
	$(my_jinap_JAVAFILES:%.java=%.class)

nodist_lib_org_openss7_javax_jain_ss7_inap_jar_la_SOURCES = org-openss7-javax-jain-ss7-inap.jar
lib_org_openss7_javax_jain_ss7_inap_jar_la_SOURCES	   = $(my_jinap_INCLUDES) $(my_jinap_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_inap_jar_la_CPPFLAGS	   = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_ss7_inap_jar_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_ss7_inap_jar_la_LIBADD	   = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_inap_jar_la_LDFLAGS	   = -module -avoid-version -shared
lib_org_openss7_javax_jain_ss7_inap_jar_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-inap.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-inap.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-ss7-inap.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-inap.jar.db
endif

lib-org-openss7-javax-jain-ss7-inap.jar.db: org-openss7-javax-jain-ss7-inap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jinap_CLASSFILES)

MY_INTERMEDIATE += $(my_jinap_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-jain-ss7-inap.jar
ALL_LOCAL += org-openss7-javax-jain-ss7-inap.jar
jar_DATA += org-openss7-javax-jain-ss7-inap-$(VERSION).jar
endif

org-openss7-javax-jain-ss7-inap-$(VERSION).jar: classdist_java.stamp $(my_jinap_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jinap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jinap_CLASSFILES)

java_JAVAFILES		+= $(my_jinap_JAVAFILES)
#dist_java_JAVA		+= $(my_jinap_JAVAFILES)

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
	$(my_jmap_JAVAFILES:%.java=%.class)
my_jmap_SOURCEFILES = \
	$(my_jmap_JAVAFILES:%.java=%.cc)
my_jmap_INCLUDES = \
	$(my_jmap_JAVAFILES:%.java=%.h)
my_jmap_HEADERCLASSES = \
	$(my_jmap_JAVAFILES:%.java=%.class)

nodist_lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_SOURCES = org-openss7-javax-jain-protocol-ss7-map.jar
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_SOURCES 	   = $(my_jmap_INCLUDES) $(my_jmap_SOURCEFILES)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CPPFLAGS	   = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LIBADD	   = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_LDFLAGS	   = -module -avoid-version -shared
lib_org_openss7_javax_jain_protocol_ss7_map_jar_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-protocol-ss7-map.jar.db
endif

lib-org-openss7-javax-jain-protocol-ss7-map.jar.db: org-openss7-javax-jain-protocol-ss7-map.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jmap_CLASSFILES)

MY_INTERMEDIATE += $(my_jmap_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-jain-protocol-ss7-map.jar
ALL_LOCAL += org-openss7-javax-jain-protocol-ss7-map.jar
jar_DATA += org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar
endif

org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar: classdist_java.stamp $(my_jmap_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jmap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jmap_CLASSFILES)

java_JAVAFILES		+= $(my_jmap_JAVAFILES)
#dist_java_JAVA		+= $(my_jmap_JAVAFILES)

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
	$(my_jcc_JAVAFILES:%.java=%.class)
my_jcc_SOURCEFILES = \
	$(my_jcc_JAVAFILES:%.java=%.cc)
my_jcc_INCLUDES = \
	$(my_jcc_JAVAFILES:%.java=%.h)
my_jcc_HEADERCLASSES = \
	$(my_jcc_JAVAFILES:%.java=%.class)

nodist_lib_org_openss7_javax_csapi_cc_jcc_jar_la_SOURCES = org-openss7-javax-csapi-cc-jcc.jar
lib_org_openss7_javax_csapi_cc_jcc_jar_la_SOURCES	  = $(my_jcc_INCLUDES) $(my_jcc_SOURCEFILES)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CPPFLAGS	  = $(GCJLIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_javax_csapi_cc_jcc_jar_la_CFLAGS	  = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LIBADD	  = $(top_builddir)/libstreams.la
lib_org_openss7_javax_csapi_cc_jcc_jar_la_LDFLAGS	  = -module -avoid-version -shared
lib_org_openss7_javax_csapi_cc_jcc_jar_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-javax-csapi-cc-jcc.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-csapi-cc-jcc.jar.la
gcjlib_DATA		+= lib-org-openss7-javax-csapi-cc-jcc.jar.db
ALL_LOCAL		+= lib-org-openss7-javax-csapi-cc-jcc.jar.db
endif

lib-org-openss7-javax-csapi-cc-jcc.jar.db: org-openss7-javax-csapi-cc-jcc.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

java_cni_CLASSES	+= $(my_jcc_CLASSFILES)

MY_INTERMEDIATE += $(my_jcc_HEADERCLASSES)

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-javax-csapi-cc-jcc.jar
ALL_LOCAL += org-openss7-javax-csapi-cc-jcc.jar
jar_DATA += org-openss7-javax-csapi-cc-jcc-$(VERSION).jar
endif

org-openss7-javax-csapi-cc-jcc-$(VERSION).jar: classdist_java.stamp $(my_jcc_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jcc_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jcc_CLASSFILES)

java_JAVAFILES		+= $(my_jcc_JAVAFILES)
#dist_java_JAVA		+= $(my_jcc_JAVAFILES)

clean-org_openss7_javax_csapi_cc_jcc:
	rm -f -- $(my_jcc_CLASSFILES)
	rm -f -- $(my_jcc_INCLUDES)
	rm -f -- $(my_jcc_HEADERCLASSES)
	rm -f -- org-openss7-javax-csapi-cc-jcc.jar org-openss7-javax-csapi-cc-jcc-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-csapi-cc-jcc.jar.db

CLEAN_LOCAL += clean-org_openss7_javax_csapi_cc_jcc

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
	$(my_streams_JAVAFILES:%.java=%.class)
my_streams_SOURCEFILES = \
	org/openss7/streams/streams_wrap.c

nodist_lib_org_openss7_streams_jar_la_SOURCES	= org-openss7-streams.jar
lib_org_openss7_streams_jar_la_SOURCES	= $(my_streams_SOURCEFILES)
lib_org_openss7_streams_jar_la_CPPFLAGS       = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_streams_jar_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_streams_jar_la_LIBADD	      = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_streams_jar_la_LDFLAGS	      = -module -avoid-version -shared
lib_org_openss7_streams_jar_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(JNIFLAGS)

#EXTRA_LTLIBRARIES 	+= lib-org-openss7-streams.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-streams.jar.la
gcjlib_DATA		+= lib-org-openss7-streams.jar.db
ALL_LOCAL		+= lib-org-openss7-streams.jar.db
endif

lib-org-openss7-streams.jar.db: org-openss7-streams.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-streams.jar
ALL_LOCAL += org-openss7-streams.jar
jar_DATA += org-openss7-streams-$(VERSION).jar
endif

org-openss7-streams-$(VERSION).jar: classdist_java.stamp $(my_streams_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_streams_CLASSFILES)
my_jar_CLASSFILES	+= $(my_streams_CLASSFILES)

libopenss7StreamsJNI_la_SOURCES	 = $(my_streams_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7StreamsJNI_la_CPPFLAGS = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7StreamsJNI_la_CFLAGS	 = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7StreamsJNI_la_LIBADD	 = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7StreamsJNI_la_LDFLAGS	 = -module -avoid-version -shared

EXTRA_LTLIBRARIES += libopenss7StreamsJNI.la
#if PKG_BUILD_ARCH
#jni_LTLIBRARIES += libopenss7StreamsJNI.la
#endif

java_c_MODULES		+= org/openss7/streams/streams.i
swig_JAVAFILES		+= $(my_streams_JAVAFILES)
MY_SECONDARY		+= $(my_streams_CLASSFILES)

$(my_streams_JAVAFILES): $(my_streams_SOURCEFILES)

java_JAVAFILES		+= $(my_streams_JAVAFILES)
#dist_java_JAVA		+= $(my_streams_JAVAFILES)

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
	$(my_xti_JAVAFILES:%.java=%.class)
my_xti_SOURCEFILES = \
	org/openss7/xti/xti_wrap.c

nodist_lib_org_openss7_xti_jar_la_SOURCES = org-openss7-xti.jar
lib_org_openss7_xti_jar_la_SOURCES	   = $(my_xti_SOURCEFILES)
lib_org_openss7_xti_jar_la_CPPFLAGS	   = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_xti_jar_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_xti_jar_la_LIBADD	   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_xti_jar_la_LDFLAGS	   = -module -avoid-version -shared
lib_org_openss7_xti_jar_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(JNIFLAGS)

##EXTRA_LTLIBRARIES 	+= lib-org-openss7-xti.jar.la
if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-xti.jar.la
gcjlib_DATA		+= lib-org-openss7-xti.jar.db
ALL_LOCAL		+= lib-org-openss7-xti.jar.db
endif

lib-org-openss7-xti.jar.db: org-openss7-xti.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-xti.jar
ALL_LOCAL += org-openss7-xti.jar
jar_DATA += org-openss7-xti-$(VERSION).jar
endif

org-openss7-xti-$(VERSION).jar: classdist_java.stamp $(my_tcap_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_tcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_tcap_CLASSFILES)

libopenss7XtiJNI_la_SOURCES  = $(my_xti_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtiJNI_la_CPPFLAGS = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7XtiJNI_la_CFLAGS   = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7XtiJNI_la_LIBADD   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtiJNI_la_LDFLAGS  = -module -avoid-version -shared

##EXTRA_LTLIBRARIES += libopenss7XtiJNI.la
if PKG_BUILD_ARCH
jni_LTLIBRARIES += libopenss7XtiJNI.la
endif

java_c_MODULES		+= org/openss7/xti/xti.i
swig_JAVAFILES		+= $(my_xti_JAVAFILES)
MY_SECONDARY		+= $(my_xti_CLASSFILES)

$(my_xti_JAVAFILES): $(my_xti_SOURCEFILES)

java_JAVAFILES		+= $(my_xti_JAVAFILES)
#dist_java_JAVA		+= $(my_xti_JAVAFILES)

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
	org/openss7/tcap/SWIGTYPE_p_listT_TcapOptions_R_t.java \
	org/openss7/tcap/SWIGTYPE_p_listT_tci__TcapOption_R_t.java \
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
	$(my_tcap_JAVAFILES:%.java=%.class)
my_tcap_SOURCEFILES = \
	org/openss7/tcap/tcap_wrap.cxx

nodist_lib_org_openss7_tcap_jar_la_SOURCES = org-openss7-tcap.jar
lib_org_openss7_tcap_jar_la_SOURCES	    = $(my_tcap_SOURCEFILES)
lib_org_openss7_tcap_jar_la_CPPFLAGS	    = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_tcap_jar_la_CFLAGS	    = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_tcap_jar_la_LIBADD	    = $(top_builddir)/libstreams.la
lib_org_openss7_tcap_jar_la_LDFLAGS	    = -module -avoid-version -shared
lib_org_openss7_tcap_jar_la_GCJFLAGS	    = $(AM_GCJFLAGS) $(JNIFLAGS)

EXTRA_LTLIBRARIES 	+= lib-org-openss7-tcap.jar.la
#if PKG_BUILD_ARCH
#gcjlib_LTLIBRARIES 	+= lib-org-openss7-tcap.jar.la
#gcjlib_DATA		+= lib-org-openss7-tcap.jar.db
#ALL_LOCAL		+= lib-org-openss7-tcap.jar.db
#endif

lib-org-openss7-tcap.jar.db: org-openss7-tcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-tcap.jar
ALL_LOCAL += org-openss7-tcap.jar
jar_DATA += org-openss7-tcap-$(VERSION).jar
endif

org-openss7-tcap-$(VERSION).jar: classdist_java.stamp $(my_xti_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_xti_CLASSFILES)
my_jar_CLASSFILES	+= $(my_xti_CLASSFILES)

libopenss7TcapJNI_la_SOURCES	= $(my_tcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7TcapJNI_la_CPPFLAGS	= $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7TcapJNI_la_CFLAGS	= $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7TcapJNI_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7TcapJNI_la_LDFLAGS	= -module -avoid-version -shared

EXTRA_LTLIBRARIES += libopenss7TcapJNI.la
#if PKG_BUILD_ARCH
#jni_LTLIBRARIES += libopenss7TcapJNI.la
#endif

java_cxx_MODULES	+= org/openss7/tcap/tcap.i
swig_JAVAFILES		+= $(my_tcap_JAVAFILES)
MY_SECONDARY		+= $(my_tcap_CLASSFILES)

$(my_tcap_JAVAFILES): $(my_tcap_SOURCEFILES)

java_JAVAFILES		+= $(my_tcap_JAVAFILES)
#dist_java_JAVA		+= $(my_tcap_JAVAFILES)

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
	$(my_xtcap_JAVAFILES:%.java=%.class)
my_xtcap_SOURCEFILES = \
	org/openss7/xtcap/xtcap_wrap.c

nodist_lib_org_openss7_xtcap_jar_la_SOURCES = org-openss7-xtcap.jar
lib_org_openss7_xtcap_jar_la_SOURCES	     = $(my_xtcap_SOURCEFILES)
lib_org_openss7_xtcap_jar_la_CPPFLAGS	     = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_xtcap_jar_la_CFLAGS	     = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_xtcap_jar_la_LIBADD	     = $(top_builddir)/libstreams.la
lib_org_openss7_xtcap_jar_la_LDFLAGS	     = -module -avoid-version -shared
lib_org_openss7_xtcap_jar_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(JNIFLAGS)

EXTRA_LTLIBRARIES 	+= lib-org-openss7-xtcap.jar.la
#if PKG_BUILD_ARCH
#gcjlib_LTLIBRARIES 	+= lib-org-openss7-xtcap.jar.la
#gcjlib_DATA		+= lib-org-openss7-xtcap.jar.db
#ALL_LOCAL		+= lib-org-openss7-xtcap.jar.db
#endif

lib-org-openss7-xtcap.jar.db: org-openss7-xtcap.jar
	$(AM_V_GEN)$(GCJDBTOOL) -f $@ $< $(gcjlibdir)/`basename $@ .db`.so

if PKG_BUILD_ARCH
my_jar_LINKS += org-openss7-xtcap.jar
ALL_LOCAL += org-openss7-xtcap.jar
jar_DATA += org-openss7-xtcap-$(VERSION).jar
endif

org-openss7-xtcap-$(VERSION).jar: classdist_java.stamp $(my_xtcap_CLASSFILES)
	$(AM_V_GEN)strip=`echo '-$(VERSION).jar' | sed 's,.,.,g'` ; \
	d=`echo '$@' | sed "s,$$strip$$,,;s,-,/,g"` ; \
	find "$$d" -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_xtcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_xtcap_CLASSFILES)

libopenss7XtcapJNI_la_SOURCES	= $(my_xtcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtcapJNI_la_CPPFLAGS	= $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7XtcapJNI_la_CFLAGS	= $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7XtcapJNI_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtcapJNI_la_LDFLAGS	= -module -avoid-version -shared

EXTRA_LTLIBRARIES += libopenss7XtcapJNI.la
#if PKG_BUILD_ARCH
#jni_LTLIBRARIES += libopenss7XtcapJNI.la
#endif

java_c_MODULES		+= org/openss7/xtcap/xtcap.i
swig_JAVAFILES		+= $(my_xtcap_JAVAFILES)
MY_SECONDARY		+= $(my_xtcap_CLASSFILES)

$(my_xtcap_JAVAFILES): $(my_xtcap_SOURCEFILES)

clean-org_openss7_xtcap:
	rm -f -- $(my_xtcap_JAVAFILES)
	rm -f -- $(my_xtcap_CLASSFILES)
	rm -f -- org/openss7/xtcap/xtcap_wrap.c
	rm -f -- org/openss7/xtcap/xtcap_wrap.cxx
	rm -f -- org-openss7-xtcap.jar org-openss7-xtcap-$(VERSION).jar
	rm -f -- lib-org-openss7-xtcap.jar.db

CLEAN_LOCAL += clean-org_openss7_xtcap

java_JAVAFILES		+= $(my_xtcap_JAVAFILES)
#dist_java_JAVA		+= $(my_xtcap_JAVAFILES)

# =============================================================================

#MY_INTERMEDIATE	+= $(swig_JAVAFILES)

## Distribute everything generated by SwiG so that the casual builder does
## not need to have swig installed.

EXTRA_DIST += $(swig_JAVAFILES)

EXTRA_DIST += $(java_JAVAFILES)

EXTRA_DIST += \
	jain/package.html \
	jain/protocol/ip/mgcp/message/package.html \
	jain/protocol/ip/mgcp/message/parms/package.html \
	jain/protocol/ip/mgcp/package.html \
	jain/protocol/ip/mgcp/pkg/package.html \
	jain/protocol/ip/package.html \
	jain/protocol/package.html \
	jain/protocol/ss7/oam/mtp2/package.html \
	jain/protocol/ss7/oam/mtp3/package.html \
	jain/protocol/ss7/oam/package.html \
	jain/protocol/ss7/oam/sccp/package.html \
	jain/protocol/ss7/oam/tcap/package.html \
	jain/protocol/ss7/package.html \
	jain/protocol/ss7/tcap/component/package.html \
	jain/protocol/ss7/tcap/dialogue/package.html \
	jain/protocol/ss7/tcap/package.html \
	javax/csapi/cc/jcc/package.html \
	javax/csapi/cc/package.html \
	javax/csapi/package.html \
	javax/jain/package.html \
	javax/jain/protocol/package.html \
	javax/jain/protocol/ss7/map/information/etsipar/package.html \
	javax/jain/protocol/ss7/map/information/package.html \
	javax/jain/protocol/ss7/map/package.html \
	javax/jain/protocol/ss7/map/position/etsipar/package.html \
	javax/jain/protocol/ss7/map/position/package.html \
	javax/jain/protocol/ss7/map/session/etsipar/package.html \
	javax/jain/protocol/ss7/map/session/package.html \
	javax/jain/protocol/ss7/map/transaction/etsipar/message/package.html \
	javax/jain/protocol/ss7/map/transaction/etsipar/package.html \
	javax/jain/protocol/ss7/map/transaction/etsipar/pdu/package.html \
	javax/jain/protocol/ss7/map/transaction/package.html \
	javax/jain/protocol/ss7/package.html \
	javax/jain/ss7/inap/constants/package.html \
	javax/jain/ss7/inap/datatype/package.html \
	javax/jain/ss7/inap/event/package.html \
	javax/jain/ss7/inap/exception/package.html \
	javax/jain/ss7/inap/operation/package.html \
	javax/jain/ss7/inap/package.html \
	javax/jain/ss7/isup/ansi/package.html \
	javax/jain/ss7/isup/itu/package.html \
	javax/jain/ss7/isup/package.html \
	javax/jain/ss7/package.html \
	javax/jcat/package.html \
	javax/management/event/package.html \
	javax/management/loading/package.html \
	javax/management/modelmbean/package.html \
	javax/management/monitor/package.html \
	javax/management/namespaces/package.html \
	javax/management/openmbean/package.html \
	javax/management/package.html \
	javax/management/relation/package.html \
	javax/management/remote/generic/package.html \
	javax/management/remote/jmxmp/package.html \
	javax/management/remote/message/package.html \
	javax/management/remote/package.html \
	javax/management/remote/rmi/package.html \
	javax/management/timer/package.html \
	javax/media/mscontrol/mediagroup/asrtts/package.html \
	javax/media/mscontrol/mediagroup/fax/package.html \
	javax/media/mscontrol/mediagroup/http/package.html \
	javax/media/mscontrol/mediagroup/package.html \
	javax/media/mscontrol/mediagroup/rtsp/package.html \
	javax/media/mscontrol/mediagroup/signals/package.html \
	javax/media/mscontrol/mediagroup/sms/package.html \
	javax/media/mscontrol/mixer/package.html \
	javax/media/mscontrol/networkconnection/package.html \
	javax/media/mscontrol/package.html \
	javax/media/mscontrol/resource/common/package.html \
	javax/media/mscontrol/resource/package.html \
	javax/media/mscontrol/resource/symbols/package.html \
	javax/media/mscontrol/resource/video/package.html \
	javax/media/mscontrol/vxml/package.html \
	javax/media/package.html \
	javax/megaco/association/package.html \
	javax/megaco/message/descriptor/package.html \
	javax/megaco/message/package.html \
	javax/megaco/package.html \
	javax/megaco/pkg/AnalogLineSPkg/package.html \
	javax/megaco/pkg/BaseRootPkg/package.html \
	javax/megaco/pkg/ContinuityPkg/package.html \
	javax/megaco/pkg/CPToneDetPkg/package.html \
	javax/megaco/pkg/CPToneGenPkg/package.html \
	javax/megaco/pkg/DTMFDetPkg/package.html \
	javax/megaco/pkg/DTMFGenPkg/package.html \
	javax/megaco/pkg/GenericPkg/package.html \
	javax/megaco/pkg/NetworkPkg/package.html \
	javax/megaco/pkg/package.html \
	javax/megaco/pkg/RTPPkg/package.html \
	javax/megaco/pkg/TDMCktPkg/package.html \
	javax/megaco/pkg/ToneDetPkg/package.html \
	javax/megaco/pkg/ToneGenPkg/package.html \
	javax/oss/fault/package.html \
	javax/oss/package.html \
	javax/sip/address/package.html \
	javax/sip/header/package.html \
	javax/sip/message/package.html \
	javax/sip/package.html \
	javax/slee/connection/package.html \
	javax/slee/facilities/package.html \
	javax/slee/management/package.html \
	javax/slee/nullactivity/package.html \
	javax/slee/package.html \
	javax/slee/profile/package.html \
	javax/slee/profile/query/package.html \
	javax/slee/resource/package.html \
	javax/slee/serviceactivity/package.html \
	javax/slee/transaction/package.html \
	javax/slee/usage/package.html \
	javax/telephony/callcenter/capabilities/package.html \
	javax/telephony/callcenter/events/package.html \
	javax/telephony/callcenter/package.html \
	javax/telephony/callcontrol/capabilities/package.html \
	javax/telephony/callcontrol/events/package.html \
	javax/telephony/callcontrol/package.html \
	javax/telephony/capabilities/package.html \
	javax/telephony/events/package.html \
	javax/telephony/media/async/package.html \
	javax/telephony/media/capabilities/package.html \
	javax/telephony/media/connection/async/package.html \
	javax/telephony/media/connection/package.html \
	javax/telephony/media/container/async/package.html \
	javax/telephony/media/container/package.html \
	javax/telephony/media/events/package.html \
	javax/telephony/media/messaging/package.html \
	javax/telephony/media/package.html \
	javax/telephony/media/provider/package.html \
	javax/telephony/media/symbols/package.html \
	javax/telephony/mobile/package.html \
	javax/telephony/package.html \
	javax/telephony/phone/capabilities/package.html \
	javax/telephony/phone/events/package.html \
	javax/telephony/phone/package.html \
	javax/telephony/privatedata/capabilities/package.html \
	javax/telephony/privatedata/events/package.html \
	javax/telephony/privatedata/package.html \
	org/omg/package.html \
	org/omg/stub/javax/management/package.html \
	org/omg/stub/javax/management/remote/package.html \
	org/omg/stub/javax/management/remote/rmi/package.html \
	org/omg/stub/javax/package.html \
	org/omg/stub/package.html \
	org/openss7/jain/package.html \
	org/openss7/jain/protocol/package.html \
	org/openss7/jain/protocol/ss7/oam/mtp2/package.html \
	org/openss7/jain/protocol/ss7/oam/mtp3/package.html \
	org/openss7/jain/protocol/ss7/oam/package.html \
	org/openss7/jain/protocol/ss7/oam/sccp/package.html \
	org/openss7/jain/protocol/ss7/oam/tcap/package.html \
	org/openss7/jain/protocol/ss7/package.html \
	org/openss7/jain/protocol/ss7/tcap/package.html \
	org/openss7/javax/csapi/cc/jcc/package.html \
	org/openss7/javax/csapi/cc/package.html \
	org/openss7/javax/csapi/package.html \
	org/openss7/javax/jain/package.html \
	org/openss7/javax/jain/protocol/package.html \
	org/openss7/javax/jain/protocol/ss7/map/information/package.html \
	org/openss7/javax/jain/protocol/ss7/map/package.html \
	org/openss7/javax/jain/protocol/ss7/map/position/package.html \
	org/openss7/javax/jain/protocol/ss7/map/session/package.html \
	org/openss7/javax/jain/protocol/ss7/map/transaction/package.html \
	org/openss7/javax/jain/protocol/ss7/package.html \
	org/openss7/javax/jain/ss7/inap/package.html \
	org/openss7/javax/jain/ss7/isup/package.html \
	org/openss7/javax/jain/ss7/package.html \
	org/openss7/javax/jcat/package.html \
	org/openss7/javax/package.html \
	org/openss7/javax/sip/package.html \
	org/openss7/jcat/package.html \
	org/openss7/package.html \
	org/openss7/ss7/mtp/package.html \
	org/openss7/ss7/package.html \
	org/openss7/ss7/sccp/package.html \
	org/openss7/streams/package.html \
	org/openss7/tcapi/package.html \
	org/openss7/tcap/package.html \
	org/openss7/xtcap/package.html \
	org/openss7/xti/package.html

EXTRA_DIST += \
	jain/doc-files/figure1.gif \
	jain/doc-files/figure2.gif \
	jain/doc-files/figure3.gif \
	jain/doc-files/figure4.gif \
	jain/doc-files/gt0001.jpg \
	jain/doc-files/javalogo.gif \
	javax/csapi/cc/jcc/doc-files/callstates.gif \
	javax/csapi/cc/jcc/doc-files/connection.gif \
	javax/csapi/cc/jcc/doc-files/providerstates.gif \
	javax/jain/doc-files/javalogo.gif \
	javax/jain/doc-files/rs \
	javax/jain/doc-files/rs/images \
	javax/jain/doc-files/rs/images/aepona.gif \
	javax/jain/doc-files/rs/images/arrowleft.gif \
	javax/jain/doc-files/rs/images/arrowright.gif \
	javax/jain/doc-files/rs/images/datakinetics.gif \
	javax/jain/doc-files/rs/images/ericsson.gif \
	javax/jain/doc-files/rs/images/figure1_1.gif \
	javax/jain/doc-files/rs/images/figure1_2.gif \
	javax/jain/doc-files/rs/images/figure1_3.gif \
	javax/jain/doc-files/rs/images/figure1_4.gif \
	javax/jain/doc-files/rs/images/figure2_1.gif \
	javax/jain/doc-files/rs/images/figure2_2.gif \
	javax/jain/doc-files/rs/images/figure2_3.gif \
	javax/jain/doc-files/rs/images/figure2_4.gif \
	javax/jain/doc-files/rs/images/figure2_5.gif \
	javax/jain/doc-files/rs/images/figure2_6.gif \
	javax/jain/doc-files/rs/images/figure2_7.gif \
	javax/jain/doc-files/rs/images/figure2_8.gif \
	javax/jain/doc-files/rs/images/figure2_9.gif \
	javax/jain/doc-files/rs/images/figure3_1.gif \
	javax/jain/doc-files/rs/images/figure3_2.gif \
	javax/jain/doc-files/rs/images/figure3_3.gif \
	javax/jain/doc-files/rs/images/figure3_4.gif \
	javax/jain/doc-files/rs/images/figure3_5.gif \
	javax/jain/doc-files/rs/images/figure3_6.gif \
	javax/jain/doc-files/rs/images/figure3_7.gif \
	javax/jain/doc-files/rs/images/figure3_8.gif \
	javax/jain/doc-files/rs/images/figure3_9.gif \
	javax/jain/doc-files/rs/images/figure4_1.gif \
	javax/jain/doc-files/rs/images/figure5_1.gif \
	javax/jain/doc-files/rs/images/figure5_2.gif \
	javax/jain/doc-files/rs/images/figure6_1.gif \
	javax/jain/doc-files/rs/images/figure6_2.gif \
	javax/jain/doc-files/rs/images/figure6_3.gif \
	javax/jain/doc-files/rs/images/figure6_4.gif \
	javax/jain/doc-files/rs/images/figure6_5.gif \
	javax/jain/doc-files/rs/images/figure6_6.gif \
	javax/jain/doc-files/rs/images/figure7_1.gif \
	javax/jain/doc-files/rs/images/figure7_2.gif \
	javax/jain/doc-files/rs/images/figure7_3.gif \
	javax/jain/doc-files/rs/images/figure7_4.gif \
	javax/jain/doc-files/rs/images/figure7_5.gif \
	javax/jain/doc-files/rs/images/javalogo.gif \
	javax/jain/doc-files/rs/images/mahindra.jpg \
	javax/jain/doc-files/rs/images/motorola.gif \
	javax/jain/doc-files/rs/images/sun.gif \
	javax/jain/doc-files/rs/images/telcordia.gif \
	javax/jain/doc-files/rs/images/ulticom.jpg \
	javax/jcat/doc-files/jcat_connection.gif \
	javax/jcat/doc-files/jcat_termconnection.gif \
	javax/telephony/callcenter/doc-files/ACDAddr.gif \
	javax/telephony/callcenter/doc-files/acdcon-direct.gif \
	javax/telephony/callcenter/doc-files/acdcon-indirect.gif \
	javax/telephony/callcenter/doc-files/acdconnection-states.gif \
	javax/telephony/callcenter/doc-files/ACDMgrAd.gif \
	javax/telephony/callcenter/doc-files/Acdtest.gif \
	javax/telephony/callcenter/doc-files/Agnstat.gif \
	javax/telephony/callcenter/doc-files/Agntest.gif \
	javax/telephony/callcenter/doc-files/callcent-agentstate.gif \
	javax/telephony/callcenter/doc-files/callcent-routestate.gif \
	javax/telephony/callcenter/doc-files/CallCtlConnect.gif \
	javax/telephony/callcenter/doc-files/CallCtlConnRelation.gif \
	javax/telephony/callcenter/doc-files/CallCtlConnStates.gif \
	javax/telephony/callcenter/doc-files/CallCtlTermConnStates.gif \
	javax/telephony/callcenter/doc-files/Rtestat.gif \
	javax/telephony/callcenter/doc-files/Rtetest.gif \
	javax/telephony/callcontrol/doc-files/ACDAddr.gif \
	javax/telephony/callcontrol/doc-files/ACDMgrAd.gif \
	javax/telephony/callcontrol/doc-files/Acdtest.gif \
	javax/telephony/callcontrol/doc-files/Agnstat.gif \
	javax/telephony/callcontrol/doc-files/Agntest.gif \
	javax/telephony/callcontrol/doc-files/CallCtlConnect.gif \
	javax/telephony/callcontrol/doc-files/callctl-connectionstates.gif \
	javax/telephony/callcontrol/doc-files/CallCtlConnRelation.gif \
	javax/telephony/callcontrol/doc-files/CallCtlConnStates.gif \
	javax/telephony/callcontrol/doc-files/CallCtlTermConnRelation.gif \
	javax/telephony/callcontrol/doc-files/CallCtlTermConnStates.gif \
	javax/telephony/callcontrol/doc-files/callctl-terminalconnectionstates.gif \
	javax/telephony/callcontrol/doc-files/cyan-ball.gif \
	javax/telephony/callcontrol/doc-files/Rtestat.gif \
	javax/telephony/callcontrol/doc-files/Rtetest.gif \
	javax/telephony/doc-files/cmodel.gif \
	javax/telephony/doc-files/core-callstates.gif \
	javax/telephony/doc-files/core-connectionstates.gif \
	javax/telephony/doc-files/core-events.gif \
	javax/telephony/doc-files/core-terminalconnectionstates.gif \
	javax/telephony/doc-files/desktop.gif \
	javax/telephony/doc-files/network.gif \
	javax/telephony/doc-files/packages.gif \
	javax/telephony/doc-files/providerstates.gif \
	javax/telephony/doc-files/timeline.gif \
	javax/telephony/events/doc-files/core-events.gif \
	javax/telephony/media/doc-files/AnswerPhone.java \
	javax/telephony/media/doc-files/AsrStates.gif \
	javax/telephony/media/doc-files/AsrTrainStates.gif \
	javax/telephony/media/doc-files/Figure1.gif \
	javax/telephony/media/doc-files/FollowMe.java \
	javax/telephony/media/doc-files/GroupAnatomy.gif \
	javax/telephony/media/doc-files/HelloWorld.java \
	javax/telephony/media/doc-files/InCall.java \
	javax/telephony/media/doc-files/PlayerStates.gif

include ../../am/swig.am

#SECONDARY: $(MY_SECONDARY)
#INTERMEDIATE: $(MY_INTERMEDIATE)

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 1.1.2.5  2009-07-09 12:05:15  brian
# - updated implementation
#
# Revision 1.1.2.4  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.3  2009-06-29 07:35:37  brian
# - improvements to build process
#
# Revision 1.1.2.2  2009-06-22 03:34:25  brian
# - updates for release
#
# Revision 1.1.2.1  2009-06-21 11:27:54  brian
# - added files to new distro
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
