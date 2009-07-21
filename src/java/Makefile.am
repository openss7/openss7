# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2009-07-21 11:06:15 $
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
# Last Modified $Date: 2009-07-21 11:06:15 $ by $Author: brian $
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
	javax/jain/CreateProviderException.class \
	javax/jain/DeleteProviderException.class \
	javax/jain/IDNotAvailableException.class \
	javax/jain/InvalidAddressException.class \
	javax/jain/InvalidArgumentException.class \
	javax/jain/InvalidPathNameException.class \
	javax/jain/InvalidStateException.class \
	javax/jain/JainEvent.class \
	javax/jain/JainException.class \
	javax/jain/ListenerAlreadyRegisteredException.class \
	javax/jain/ListenerNotRegisteredException.class \
	javax/jain/MandatoryParameterNotSetException.class \
	javax/jain/MethodNotSupportedException.class \
	javax/jain/ParameterNotSetException.class \
	javax/jain/PeerUnavailableException.class \
	javax/jain/PlatformException.class \
	javax/jain/VersionNotSupportedException.class

MY_SECONDARY		+= $(my_javax_jain_CLASSFILES)

javax-jain-$(VERSION).jar: classdist_java.stamp $(my_javax_jain_CLASSFILES)
	$(AM_V_GEN)d='javax/jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_JAVAFILES)

clean-javax_jain:
	@d='javax/jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain.jar
	rm -f -- javax-jain-$(VERSION).jar
	rm -f -- lib-javax-jain.db

CLEAN_LOCAL += clean-javax_jain

nodist_lib_javax_jain_la_SOURCES = javax-jain.jar
lib_javax_jain_la_LIBADD	 =
lib_javax_jain_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jain_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain.jar
ALL_LOCAL		+= javax-jain.jar
jar_DATA		+= javax-jain-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-jain.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain.db
ALL_LOCAL		+= lib-javax-jain.db
endif
endif

## -----------------------------------------------------------------------------

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

javax-jain-ss7-$(VERSION).jar: classdist_java.stamp javax-jain.jar $(my_javax_jain_ss7_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_ss7_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_ss7_JAVAFILES)

clean-javax_jain_ss7:
	@d='javax/jain/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-ss7.jar javax-jain-ss7-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7.db

CLEAN_LOCAL += clean-javax_jain_ss7

nodist_lib_javax_jain_ss7_la_SOURCES = javax-jain-ss7.jar
lib_javax_jain_ss7_la_LIBADD	     =
lib_javax_jain_ss7_la_LDFLAGS	     = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jain_ss7_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain-ss7.jar
ALL_LOCAL		+= javax-jain-ss7.jar
jar_DATA		+= javax-jain-ss7-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-jain-ss7.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain-ss7.db
ALL_LOCAL		+= lib-javax-jain-ss7.db
endif
endif

## -----------------------------------------------------------------------------

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
	javax/jain/protocol/ss7/E164Number.class \
	javax/jain/protocol/ss7/JainSS7Factory.class \
	javax/jain/protocol/ss7/JainSS7Utility.class \
	javax/jain/protocol/ss7/SS7Address.class \
	javax/jain/protocol/ss7/SS7Exception.class \
	javax/jain/protocol/ss7/SS7InvalidParamException.class \
	javax/jain/protocol/ss7/SS7ListenerAlreadyRegisteredException.class \
	javax/jain/protocol/ss7/SS7ListenerNotRegisteredException.class \
	javax/jain/protocol/ss7/SS7MandatoryParameterNotSetException.class \
	javax/jain/protocol/ss7/SS7Number.class \
	javax/jain/protocol/ss7/SS7Parameter.class \
	javax/jain/protocol/ss7/SS7ParameterNotSetException.class \
	javax/jain/protocol/ss7/SS7PeerUnavailableException.class

MY_SECONDARY		+= $(my_javax_jain_protocol_ss7_CLASSFILES)

javax-jain-protocol-ss7-$(VERSION).jar: classdist_java.stamp javax-jain.jar $(my_javax_jain_protocol_ss7_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_protocol_ss7_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_protocol_ss7_JAVAFILES)

clean-javax_jain_protocol_ss7:
	@d='javax/jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-protocol-ss7.jar
	rm -f -- javax-jain-protocol-ss7-$(VERSION).jar
	rm -f -- lib-javax-jain-protocol-ss7.db

CLEAN_LOCAL	+= clean-javax_jain_protocol_ss7

nodist_lib_javax_jain_protocol_ss7_la_SOURCES = javax-jain-protocol-ss7.jar
lib_javax_jain_protocol_ss7_la_LIBADD	      =
lib_javax_jain_protocol_ss7_la_LDFLAGS	      = -export-dynamic -release $(VERSION) -shared
lib_javax_jain_protocol_ss7_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain-protocol-ss7.jar
ALL_LOCAL		+= javax-jain-protocol-ss7.jar
jar_DATA		+= javax-jain-protocol-ss7-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-jain-protocol-ss7.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain-protocol-ss7.db
ALL_LOCAL		+= lib-javax-jain-protocol-ss7.db
endif
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

MY_SECONDARY		+= $(my_jain_CLASSFILES)

jain-$(VERSION).jar: classdist_java.stamp $(my_jain_CLASSFILES)
	$(AM_V_GEN)d='jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_jain_JAVAFILES)
#dist_java_JAVA		+= $(my_jain_JAVAFILES)

clean-jain:
	@d='jain -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain.jar
	rm -f -- jain-$(VERSION).jar
	rm -f -- lib-jain.db

CLEAN_LOCAL	+= clean-jain

nodist_lib_jain_la_SOURCES = jain.jar
lib_jain_la_LIBADD	   =
lib_jain_la_LDFLAGS	   = -module -export-dynamic -release $(VERSION) -shared
lib_jain_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= jain.jar
ALL_LOCAL		+= jain.jar
jar_DATA		+= jain-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-jain.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-jain.db
ALL_LOCAL		+= lib-jain.db
endif
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

MY_SECONDARY		+= $(my_jain_protocol_ss7_CLASSFILES)

jain-protocol-ss7-$(VERSION).jar: classdist_java.stamp jain.jar $(my_jain_protocol_ss7_CLASSFILES)
	$(AM_V_GEN)d='jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_jain_protocol_ss7_JAVAFILES)
#dist_java_JAVA		+= $(my_jain_protocol_ss7_JAVAFILES)

clean-jain_protocol_ss7:
	@d='jain/protocol/ss7 -maxdepth 1' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain-protocol-ss7.jar
	rm -f -- jain-protocol-ss7-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7.db

CLEAN_LOCAL += clean-jain_protocol_ss7

nodist_lib_jain_protocol_ss7_la_SOURCES	= jain-protocol-ss7.jar
lib_jain_protocol_ss7_la_LIBADD		=
lib_jain_protocol_ss7_la_LDFLAGS	= -module -export-dynamic -release $(VERSION) -shared
lib_jain_protocol_ss7_la_GCJFLAGS	= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= jain-protocol-ss7.jar
ALL_LOCAL		+= jain-protocol-ss7.jar
jar_DATA		+= jain-protocol-ss7-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-jain-protocol-ss7.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-jain-protocol-ss7.db
ALL_LOCAL		+= lib-jain-protocol-ss7.db
endif
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
	javax/jain/ss7/isup/UserToUserIndicators.class

MY_SECONDARY		+= $(my_javax_jain_ss7_isup_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_jain_ss7_isup_CLASSFILES)

javax-jain-ss7-isup-$(VERSION).jar: classdist_java.stamp javax-jain-ss7.jar $(my_javax_jain_ss7_isup_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_ss7_isup_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_ss7_isup_JAVAFILES)

clean-javax_jain_ss7_isup:
	@d='javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-ss7-isup.jar javax-jain-ss7-isup-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-isup.db

CLEAN_LOCAL += clean-javax_jain_ss7_isup

nodist_lib_javax_jain_ss7_isup_la_SOURCES = javax-jain-ss7-isup.jar
lib_javax_jain_ss7_isup_la_LIBADD	  =
lib_javax_jain_ss7_isup_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jain_ss7_isup_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain-ss7-isup.jar
ALL_LOCAL		+= javax-jain-ss7-isup.jar
jar_DATA		+= javax-jain-ss7-isup-$(VERSION).jar
gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-isup.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain-ss7-isup.db
ALL_LOCAL		+= lib-javax-jain-ss7-isup.db
endif
endif

## -----------------------------------------------------------------------------

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
	javax/jain/ss7/inap/constants/ActionIndicator.class \
	javax/jain/ss7/inap/constants/ActionPerformed.class \
	javax/jain/ss7/inap/constants/BearerCapabilityChoice.class \
	javax/jain/ss7/inap/constants/CallProcessingOperationCorrelationID.class \
	javax/jain/ss7/inap/constants/CancelChoice.class \
	javax/jain/ss7/inap/constants/CancelID.class \
	javax/jain/ss7/inap/constants/CGEncountered.class \
	javax/jain/ss7/inap/constants/CollectedInfoChoice.class \
	javax/jain/ss7/inap/constants/ComponentChoice.class \
	javax/jain/ss7/inap/constants/ComponentID.class \
	javax/jain/ss7/inap/constants/ComponentIndPrimitiveType.class \
	javax/jain/ss7/inap/constants/ComponentPrimitiveType.class \
	javax/jain/ss7/inap/constants/ComponentReqPrimitiveType.class \
	javax/jain/ss7/inap/constants/ComponentType.class \
	javax/jain/ss7/inap/constants/ConnectedPartyChoice.class \
	javax/jain/ss7/inap/constants/ControlType.class \
	javax/jain/ss7/inap/constants/DialoguePrimitiveType.class \
	javax/jain/ss7/inap/constants/DPAssignment.class \
	javax/jain/ss7/inap/constants/DpSpecificCriteriaChoice.class \
	javax/jain/ss7/inap/constants/ElapsedTimeValueID.class \
	javax/jain/ss7/inap/constants/EntryChoice.class \
	javax/jain/ss7/inap/constants/ErrorCode.class \
	javax/jain/ss7/inap/constants/ErrorTreatment.class \
	javax/jain/ss7/inap/constants/EventSpecificInformationBCSMChoice.class \
	javax/jain/ss7/inap/constants/EventTypeBCSM.class \
	javax/jain/ss7/inap/constants/ExtensionChoice.class \
	javax/jain/ss7/inap/constants/ExtensionType.class \
	javax/jain/ss7/inap/constants/FacilityGroupChoice.class \
	javax/jain/ss7/inap/constants/FacilityGroupID.class \
	javax/jain/ss7/inap/constants/FeatureRequestIndicator.class \
	javax/jain/ss7/inap/constants/FilterCriteriaID.class \
	javax/jain/ss7/inap/constants/FilteringCharacteristicsChoice.class \
	javax/jain/ss7/inap/constants/FilteringCriteriaChoice.class \
	javax/jain/ss7/inap/constants/FilteringTimeOutChoice.class \
	javax/jain/ss7/inap/constants/ForwardingCondition.class \
	javax/jain/ss7/inap/constants/GapCriteriaChoice.class \
	javax/jain/ss7/inap/constants/GapTreatmentChoice.class \
	javax/jain/ss7/inap/constants/GeneralProblem.class \
	javax/jain/ss7/inap/constants/GroupID.class \
	javax/jain/ss7/inap/constants/InformationToSendChoice.class \
	javax/jain/ss7/inap/constants/InfoToSendChoice.class \
	javax/jain/ss7/inap/constants/InvokeProblem.class \
	javax/jain/ss7/inap/constants/LegIDChoice.class \
	javax/jain/ss7/inap/constants/Media.class \
	javax/jain/ss7/inap/constants/MessageIDChoice.class \
	javax/jain/ss7/inap/constants/MessageType.class \
	javax/jain/ss7/inap/constants/MidCallReportType.class \
	javax/jain/ss7/inap/constants/MonitorMode.class \
	javax/jain/ss7/inap/constants/NotificationInformationChoice.class \
	javax/jain/ss7/inap/constants/NumberMatchChoice.class \
	javax/jain/ss7/inap/constants/OperationCodeChoice.class \
	javax/jain/ss7/inap/constants/OperationCode.class \
	javax/jain/ss7/inap/constants/PAbortReason.class \
	javax/jain/ss7/inap/constants/PartyToConnectChoice.class \
	javax/jain/ss7/inap/constants/PartyToDisconnectChoice.class \
	javax/jain/ss7/inap/constants/ProfileIdentifierChoice.class \
	javax/jain/ss7/inap/constants/ReceivedInformationChoice.class \
	javax/jain/ss7/inap/constants/ReceivedStatus.class \
	javax/jain/ss7/inap/constants/RejectType.class \
	javax/jain/ss7/inap/constants/ReportCause.class \
	javax/jain/ss7/inap/constants/ReportCondition.class \
	javax/jain/ss7/inap/constants/RequestedInformationType.class \
	javax/jain/ss7/inap/constants/RequestedInformationValueChoice.class \
	javax/jain/ss7/inap/constants/ResourceAddressChoice.class \
	javax/jain/ss7/inap/constants/ResourceAddressID.class \
	javax/jain/ss7/inap/constants/ResourceIDChoice.class \
	javax/jain/ss7/inap/constants/ResourceStatus.class \
	javax/jain/ss7/inap/constants/ResponseCondition.class \
	javax/jain/ss7/inap/constants/ReturnErrorProblem.class \
	javax/jain/ss7/inap/constants/ReturnResultProblem.class \
	javax/jain/ss7/inap/constants/RoutingAddressChoice.class \
	javax/jain/ss7/inap/constants/SideID.class \
	javax/jain/ss7/inap/constants/TargetLineIdentifierChoice.class \
	javax/jain/ss7/inap/constants/TerminalType.class \
	javax/jain/ss7/inap/constants/TimeOutError.class \
	javax/jain/ss7/inap/constants/TimerID.class \
	javax/jain/ss7/inap/constants/ToneIDChoice.class \
	javax/jain/ss7/inap/constants/TransportAddressChoice.class \
	javax/jain/ss7/inap/constants/TriggerType.class \
	javax/jain/ss7/inap/constants/UAbortReason.class \
	javax/jain/ss7/inap/constants/USIMonitorMode.class \
	javax/jain/ss7/inap/constants/VariablePartChoice.class \
	javax/jain/ss7/inap/constants/VariablePartID.class \
	javax/jain/ss7/inap/datatype/AddressAndService.class \
	javax/jain/ss7/inap/datatype/AllCallSegments.class \
	javax/jain/ss7/inap/datatype/AssociatedCallSegment.class \
	javax/jain/ss7/inap/datatype/BackwardGVNS.class \
	javax/jain/ss7/inap/datatype/BackwardServiceInteractionInd.class \
	javax/jain/ss7/inap/datatype/BCSMEvent.class \
	javax/jain/ss7/inap/datatype/BCSMFailure.class \
	javax/jain/ss7/inap/datatype/BearerCapability.class \
	javax/jain/ss7/inap/datatype/BearerCap.class \
	javax/jain/ss7/inap/datatype/BothGapTreatmentParameters.class \
	javax/jain/ss7/inap/datatype/CalledAddressAndService.class \
	javax/jain/ss7/inap/datatype/CalledPartyNumber.class \
	javax/jain/ss7/inap/datatype/CalledPartySubaddress.class \
	javax/jain/ss7/inap/datatype/CallingAddressAndService.class \
	javax/jain/ss7/inap/datatype/CallingPartyNumber.class \
	javax/jain/ss7/inap/datatype/CallingPartySubaddress.class \
	javax/jain/ss7/inap/datatype/CallSegment.class \
	javax/jain/ss7/inap/datatype/CallSegmentToCancel.class \
	javax/jain/ss7/inap/datatype/CarrierID.class \
	javax/jain/ss7/inap/datatype/Carrier.class \
	javax/jain/ss7/inap/datatype/Cause.class \
	javax/jain/ss7/inap/datatype/ChargingEvent.class \
	javax/jain/ss7/inap/datatype/CollectedDigits.class \
	javax/jain/ss7/inap/datatype/CollectedInfo.class \
	javax/jain/ss7/inap/datatype/Component.class \
	javax/jain/ss7/inap/datatype/ConnectedParty.class \
	javax/jain/ss7/inap/datatype/ControlDigits.class \
	javax/jain/ss7/inap/datatype/CounterAndValue.class \
	javax/jain/ss7/inap/datatype/CSFailure.class \
	javax/jain/ss7/inap/datatype/DestinationRoutingAddress.class \
	javax/jain/ss7/inap/datatype/DigitsGenericDigits.class \
	javax/jain/ss7/inap/datatype/DigitsGenericNumber.class \
	javax/jain/ss7/inap/datatype/DisconnectSpecificInfo.class \
	javax/jain/ss7/inap/datatype/DPSpecificCommonParameters.class \
	javax/jain/ss7/inap/datatype/DPSpecificCriteria.class \
	javax/jain/ss7/inap/datatype/Entry.class \
	javax/jain/ss7/inap/datatype/EventSpecificInformationBCSM.class \
	javax/jain/ss7/inap/datatype/ExtensionField.class \
	javax/jain/ss7/inap/datatype/Extension.class \
	javax/jain/ss7/inap/datatype/FacilityGroup.class \
	javax/jain/ss7/inap/datatype/FilteredCallTreatment.class \
	javax/jain/ss7/inap/datatype/FilteringCharacteristics.class \
	javax/jain/ss7/inap/datatype/FilteringCriteria.class \
	javax/jain/ss7/inap/datatype/FilteringTimeOut.class \
	javax/jain/ss7/inap/datatype/ForwardCallIndicators.class \
	javax/jain/ss7/inap/datatype/ForwardGVNS.class \
	javax/jain/ss7/inap/datatype/ForwardServiceInteractionInd.class \
	javax/jain/ss7/inap/datatype/GapCriteria.class \
	javax/jain/ss7/inap/datatype/GapIndicators.class \
	javax/jain/ss7/inap/datatype/GapOnService.class \
	javax/jain/ss7/inap/datatype/GapTreatment.class \
	javax/jain/ss7/inap/datatype/GenericNumbers.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0001.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0010.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0011.class \
	javax/jain/ss7/inap/datatype/GlobalTitle0100.class \
	javax/jain/ss7/inap/datatype/GlobalTitle.class \
	javax/jain/ss7/inap/datatype/HighLayerCompatibility.class \
	javax/jain/ss7/inap/datatype/InbandInfo.class \
	javax/jain/ss7/inap/datatype/InformationToRecord.class \
	javax/jain/ss7/inap/datatype/InformationToSend.class \
	javax/jain/ss7/inap/datatype/INServiceCompatibilityIndication.class \
	javax/jain/ss7/inap/datatype/IPAddressAndCallSegmentID.class \
	javax/jain/ss7/inap/datatype/IPAddressAndLegID.class \
	javax/jain/ss7/inap/datatype/LegID.class \
	javax/jain/ss7/inap/datatype/Leg.class \
	javax/jain/ss7/inap/datatype/LocationNumber.class \
	javax/jain/ss7/inap/datatype/MessageID.class \
	javax/jain/ss7/inap/datatype/MidCallControlInfo.class \
	javax/jain/ss7/inap/datatype/MidCallControlInformation.class \
	javax/jain/ss7/inap/datatype/MidCallInfoType.class \
	javax/jain/ss7/inap/datatype/MidCallSpecificInfo.class \
	javax/jain/ss7/inap/datatype/MiscCallInfo.class \
	javax/jain/ss7/inap/datatype/OriginalCalledPartyID.class \
	javax/jain/ss7/inap/datatype/PartyToConnect.class \
	javax/jain/ss7/inap/datatype/PartyToDisconnect.class \
	javax/jain/ss7/inap/datatype/ProfileIdentifier.class \
	javax/jain/ss7/inap/datatype/RedirectingPartyID.class \
	javax/jain/ss7/inap/datatype/RedirectionInformation.class \
	javax/jain/ss7/inap/datatype/RequestedInformation.class \
	javax/jain/ss7/inap/datatype/RequestedInformationList.class \
	javax/jain/ss7/inap/datatype/RequestedInformationTypeList.class \
	javax/jain/ss7/inap/datatype/RequestedInformationValue.class \
	javax/jain/ss7/inap/datatype/RequestedUTSI.class \
	javax/jain/ss7/inap/datatype/ResourceAddress.class \
	javax/jain/ss7/inap/datatype/ResourceID.class \
	javax/jain/ss7/inap/datatype/RouteList.class \
	javax/jain/ss7/inap/datatype/ScfID.class \
	javax/jain/ss7/inap/datatype/ServiceAddressInformation.class \
	javax/jain/ss7/inap/datatype/ServiceInteractionIndicatorsTwo.class \
	javax/jain/ss7/inap/datatype/Text.class \
	javax/jain/ss7/inap/datatype/Tone.class \
	javax/jain/ss7/inap/datatype/TransportAddress.class \
	javax/jain/ss7/inap/datatype/TriggerDataIdentifier.class \
	javax/jain/ss7/inap/datatype/VariableMessage.class \
	javax/jain/ss7/inap/datatype/VariablePart.class \
	javax/jain/ss7/inap/event/BeginIndEvent.class \
	javax/jain/ss7/inap/event/BeginReqEvent.class \
	javax/jain/ss7/inap/event/ComponentEvent.class \
	javax/jain/ss7/inap/event/ComponentIndEvent.class \
	javax/jain/ss7/inap/event/ComponentReqEvent.class \
	javax/jain/ss7/inap/event/ContinueIndEvent.class \
	javax/jain/ss7/inap/event/ContinueReqEvent.class \
	javax/jain/ss7/inap/event/DialogueEvent.class \
	javax/jain/ss7/inap/event/DialogueIndEvent.class \
	javax/jain/ss7/inap/event/DialogueReqEvent.class \
	javax/jain/ss7/inap/event/EndIndEvent.class \
	javax/jain/ss7/inap/event/EndReqEvent.class \
	javax/jain/ss7/inap/event/InapEvent.class \
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
	javax/jain/ss7/inap/exception/InvalidApplicationContextException.class \
	javax/jain/ss7/inap/exception/InvalidCallIDException.class \
	javax/jain/ss7/inap/exception/ParameterOutOfRangeException.class \
	javax/jain/ss7/inap/exception/TooManyListenersRegisteredException.class \
	javax/jain/ss7/inap/JainInapListener.class \
	javax/jain/ss7/inap/JainInapProvider.class \
	javax/jain/ss7/inap/JainInapStack.class \
	javax/jain/ss7/inap/operation/ActivateServiceFiltering.class \
	javax/jain/ss7/inap/operation/ActivityTest.class \
	javax/jain/ss7/inap/operation/AnalysedInformation.class \
	javax/jain/ss7/inap/operation/AnalyseInformation.class \
	javax/jain/ss7/inap/operation/ApplyCharging.class \
	javax/jain/ss7/inap/operation/ApplyChargingReport.class \
	javax/jain/ss7/inap/operation/AssistRequestInstructions.class \
	javax/jain/ss7/inap/operation/AuthorizeTermination.class \
	javax/jain/ss7/inap/operation/CallGap.class \
	javax/jain/ss7/inap/operation/CallInformationReport.class \
	javax/jain/ss7/inap/operation/CallInformationRequest.class \
	javax/jain/ss7/inap/operation/Cancel.class \
	javax/jain/ss7/inap/operation/CancelStatusReportRequest.class \
	javax/jain/ss7/inap/operation/CollectedInformation.class \
	javax/jain/ss7/inap/operation/CollectInformation.class \
	javax/jain/ss7/inap/operation/Connect.class \
	javax/jain/ss7/inap/operation/ConnectToResource.class \
	javax/jain/ss7/inap/operation/Continue.class \
	javax/jain/ss7/inap/operation/ContinueWithArgument.class \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociation.class \
	javax/jain/ss7/inap/operation/CreateCallSegmentAssociationResult.class \
	javax/jain/ss7/inap/operation/DFCWithArgument.class \
	javax/jain/ss7/inap/operation/DisconnectForwardConnection.class \
	javax/jain/ss7/inap/operation/DisconnectLeg.class \
	javax/jain/ss7/inap/operation/EntityReleased.class \
	javax/jain/ss7/inap/operation/EstablishTemporaryConnection.class \
	javax/jain/ss7/inap/operation/EventNotificationCharging.class \
	javax/jain/ss7/inap/operation/EventReportBCSM.class \
	javax/jain/ss7/inap/operation/EventReportFacility.class \
	javax/jain/ss7/inap/operation/FacilitySelectedAndAvailable.class \
	javax/jain/ss7/inap/operation/FurnishChargingInformation.class \
	javax/jain/ss7/inap/operation/HoldCallInNetwork.class \
	javax/jain/ss7/inap/operation/InitialDP.class \
	javax/jain/ss7/inap/operation/InitiateCallAttempt.class \
	javax/jain/ss7/inap/operation/ManageTriggerData.class \
	javax/jain/ss7/inap/operation/ManageTriggerDataResult.class \
	javax/jain/ss7/inap/operation/MergeCallSegments.class \
	javax/jain/ss7/inap/operation/MessageReceived.class \
	javax/jain/ss7/inap/operation/MoveCallSegments.class \
	javax/jain/ss7/inap/operation/MoveLeg.class \
	javax/jain/ss7/inap/operation/OAbandon.class \
	javax/jain/ss7/inap/operation/OAnswer.class \
	javax/jain/ss7/inap/operation/OCalledPartyBusy.class \
	javax/jain/ss7/inap/operation/ODisconnect.class \
	javax/jain/ss7/inap/operation/OMidCall.class \
	javax/jain/ss7/inap/operation/ONoAnswer.class \
	javax/jain/ss7/inap/operation/Operation.class \
	javax/jain/ss7/inap/operation/OriginationAttemptAuthorized.class \
	javax/jain/ss7/inap/operation/OriginationAttempt.class \
	javax/jain/ss7/inap/operation/OSuspended.class \
	javax/jain/ss7/inap/operation/PlayAnnouncement.class \
	javax/jain/ss7/inap/operation/PromptAndCollectUserInformation.class \
	javax/jain/ss7/inap/operation/PromptAndReceiveMessage.class \
	javax/jain/ss7/inap/operation/ReceivedInformation.class \
	javax/jain/ss7/inap/operation/Reconnect.class \
	javax/jain/ss7/inap/operation/ReleaseCall.class \
	javax/jain/ss7/inap/operation/ReportUTSI.class \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReport.class \
	javax/jain/ss7/inap/operation/RequestCurrentStatusReportResult.class \
	javax/jain/ss7/inap/operation/RequestEveryStatusChangeReport.class \
	javax/jain/ss7/inap/operation/RequestFirstStatusMatchReport.class \
	javax/jain/ss7/inap/operation/RequestNotificationChargingEvent.class \
	javax/jain/ss7/inap/operation/RequestReportBCSMEvent.class \
	javax/jain/ss7/inap/operation/RequestReportFacilityEvent.class \
	javax/jain/ss7/inap/operation/RequestReportUTSI.class \
	javax/jain/ss7/inap/operation/ResetTimer.class \
	javax/jain/ss7/inap/operation/RouteSelectFailure.class \
	javax/jain/ss7/inap/operation/SelectFacility.class \
	javax/jain/ss7/inap/operation/SelectRoute.class \
	javax/jain/ss7/inap/operation/SendChargingInformation.class \
	javax/jain/ss7/inap/operation/SendFacilityInformation.class \
	javax/jain/ss7/inap/operation/SendSTUI.class \
	javax/jain/ss7/inap/operation/ServiceFilteringResponse.class \
	javax/jain/ss7/inap/operation/SpecializedResourceReport.class \
	javax/jain/ss7/inap/operation/SplitLeg.class \
	javax/jain/ss7/inap/operation/StatusReport.class \
	javax/jain/ss7/inap/operation/TAnswer.class \
	javax/jain/ss7/inap/operation/TBusy.class \
	javax/jain/ss7/inap/operation/TDisconnect.class \
	javax/jain/ss7/inap/operation/TerminationAttemptAuthorized.class \
	javax/jain/ss7/inap/operation/TerminationAttempt.class \
	javax/jain/ss7/inap/operation/TMidCall.class \
	javax/jain/ss7/inap/operation/TNoAnswer.class \
	javax/jain/ss7/inap/operation/TSuspended.class

MY_SECONDARY		+= $(my_javax_jain_ss7_inap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_jain_ss7_inap_CLASSFILES)

javax-jain-ss7-inap-$(VERSION).jar: classdist_java.stamp javax-jain-ss7.jar $(my_javax_jain_ss7_inap_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_ss7_inap_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_ss7_inap_JAVAFILES)

clean-javax_jain_ss7_inap:
	@d='javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_javax_jain_ss7_inap_CLASSFILES)
	rm -f -- javax-jain-ss7-inap.jar javax-jain-ss7-inap-$(VERSION).jar
	rm -f -- lib-javax-jain-ss7-inap.db

CLEAN_LOCAL += clean-javax_jain_ss7_inap

nodist_lib_javax_jain_ss7_inap_la_SOURCES = javax-jain-ss7-inap.jar
lib_javax_jain_ss7_inap_la_LIBADD	  =
lib_javax_jain_ss7_inap_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jain_ss7_inap_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain-ss7-inap.jar
ALL_LOCAL		+= javax-jain-ss7-inap.jar
jar_DATA		+= javax-jain-ss7-inap-$(VERSION).jar
gcjlib_LTLIBRARIES 	+= lib-javax-jain-ss7-inap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain-ss7-inap.db
ALL_LOCAL		+= lib-javax-jain-ss7-inap.db
endif
endif

## -----------------------------------------------------------------------------

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
	javax/jain/protocol/ss7/map/MapErrorEvent.class

MY_SECONDARY		+= $(my_javax_jain_protocol_ss7_map_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_jain_protocol_ss7_map_CLASSFILES)

javax-jain-protocol-ss7-map-$(VERSION).jar: classdist_java.stamp javax-jain-protocol-ss7.jar $(my_javax_jain_protocol_ss7_map_CLASSFILES)
	$(AM_V_GEN)d='javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jain_protocol_ss7_map_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jain_protocol_ss7_map_JAVAFILES)

clean-javax_jain_protocol_ss7_map:
	@d='javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jain-protocol-ss7-map.jar
	rm -f -- javax-jain-protocol-ss7-map-$(VERSION).jar
	rm -f -- lib-javax-jain-protocol-ss7-map.db

CLEAN_LOCAL += clean-javax_jain_protocol_ss7_map

nodist_lib_javax_jain_protocol_ss7_map_la_SOURCES = javax-jain-protocol-ss7-map.jar
lib_javax_jain_protocol_ss7_map_la_LIBADD	  =
lib_javax_jain_protocol_ss7_map_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jain_protocol_ss7_map_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jain-protocol-ss7-map.jar
ALL_LOCAL		+= javax-jain-protocol-ss7-map.jar
jar_DATA		+= javax-jain-protocol-ss7-map-$(VERSION).jar
gcjlib_LTLIBRARIES 	+= lib-javax-jain-protocol-ss7-map.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jain-protocol-ss7-map.db
ALL_LOCAL		+= lib-javax-jain-protocol-ss7-map.db
endif
endif

## -----------------------------------------------------------------------------

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
	javax/sip/address/AddressFactory.class \
	javax/sip/address/Address.class \
	javax/sip/address/Hop.class \
	javax/sip/address/Router.class \
	javax/sip/address/SipURI.class \
	javax/sip/address/TelURL.class \
	javax/sip/address/URI.class \
	javax/sip/ClientTransaction.class \
	javax/sip/DialogDoesNotExistException.class \
	javax/sip/Dialog.class \
	javax/sip/DialogState.class \
	javax/sip/DialogTerminatedEvent.class \
	javax/sip/header/AcceptEncodingHeader.class \
	javax/sip/header/AcceptHeader.class \
	javax/sip/header/AcceptLanguageHeader.class \
	javax/sip/header/AlertInfoHeader.class \
	javax/sip/header/AllowEventsHeader.class \
	javax/sip/header/AllowHeader.class \
	javax/sip/header/AuthenticationInfoHeader.class \
	javax/sip/header/AuthorizationHeader.class \
	javax/sip/header/CallIdHeader.class \
	javax/sip/header/CallInfoHeader.class \
	javax/sip/header/ContactHeader.class \
	javax/sip/header/ContentDispositionHeader.class \
	javax/sip/header/ContentEncodingHeader.class \
	javax/sip/header/ContentLanguageHeader.class \
	javax/sip/header/ContentLengthHeader.class \
	javax/sip/header/ContentTypeHeader.class \
	javax/sip/header/CSeqHeader.class \
	javax/sip/header/DateHeader.class \
	javax/sip/header/Encoding.class \
	javax/sip/header/ErrorInfoHeader.class \
	javax/sip/header/EventHeader.class \
	javax/sip/header/ExpiresHeader.class \
	javax/sip/header/ExtensionHeader.class \
	javax/sip/header/FromHeader.class \
	javax/sip/header/HeaderAddress.class \
	javax/sip/header/HeaderFactory.class \
	javax/sip/header/Header.class \
	javax/sip/header/InReplyToHeader.class \
	javax/sip/header/MaxForwardsHeader.class \
	javax/sip/header/MediaType.class \
	javax/sip/header/MimeVersionHeader.class \
	javax/sip/header/MinExpiresHeader.class \
	javax/sip/header/OptionTag.class \
	javax/sip/header/OrganizationHeader.class \
	javax/sip/header/Parameters.class \
	javax/sip/header/PriorityHeader.class \
	javax/sip/header/ProxyAuthenticateHeader.class \
	javax/sip/header/ProxyAuthorizationHeader.class \
	javax/sip/header/ProxyRequireHeader.class \
	javax/sip/header/RAckHeader.class \
	javax/sip/header/ReasonHeader.class \
	javax/sip/header/RecordRouteHeader.class \
	javax/sip/header/ReferToHeader.class \
	javax/sip/header/ReplyToHeader.class \
	javax/sip/header/RequireHeader.class \
	javax/sip/header/RetryAfterHeader.class \
	javax/sip/header/RouteHeader.class \
	javax/sip/header/RSeqHeader.class \
	javax/sip/header/ServerHeader.class \
	javax/sip/header/SIPETagHeader.class \
	javax/sip/header/SIPIfMatchHeader.class \
	javax/sip/header/SubjectHeader.class \
	javax/sip/header/SubscriptionStateHeader.class \
	javax/sip/header/SupportedHeader.class \
	javax/sip/header/TimeStampHeader.class \
	javax/sip/header/ToHeader.class \
	javax/sip/header/TooManyHopsException.class \
	javax/sip/header/UnsupportedHeader.class \
	javax/sip/header/UserAgentHeader.class \
	javax/sip/header/ViaHeader.class \
	javax/sip/header/WarningHeader.class \
	javax/sip/header/WWWAuthenticateHeader.class \
	javax/sip/InvalidArgumentException.class \
	javax/sip/IOExceptionEvent.class \
	javax/sip/ListeningPoint.class \
	javax/sip/message/MessageFactory.class \
	javax/sip/message/Message.class \
	javax/sip/message/Request.class \
	javax/sip/message/Response.class \
	javax/sip/ObjectInUseException.class \
	javax/sip/PeerUnavailableException.class \
	javax/sip/ProviderDoesNotExistException.class \
	javax/sip/RequestEvent.class \
	javax/sip/ResponseEvent.class \
	javax/sip/ServerTransaction.class \
	javax/sip/SipException.class \
	javax/sip/SipFactory.class \
	javax/sip/SipListener.class \
	javax/sip/SipProvider.class \
	javax/sip/SipStack.class \
	javax/sip/TimeoutEvent.class \
	javax/sip/Timeout.class \
	javax/sip/TransactionAlreadyExistsException.class \
	javax/sip/TransactionDoesNotExistException.class \
	javax/sip/Transaction.class \
	javax/sip/TransactionState.class \
	javax/sip/TransactionTerminatedEvent.class \
	javax/sip/TransactionUnavailableException.class \
	javax/sip/TransportAlreadySupportedException.class \
	javax/sip/TransportNotSupportedException.class

MY_SECONDARY		+= $(my_javax_sip_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_sip_CLASSFILES)

javax-sip-$(VERSION).jar: classdist_java.stamp $(my_javax_sip_CLASSFILES)
	$(AM_V_GEN)d='javax/sip' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_sip_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_sip_JAVAFILES)

clean-javax_sip:
	@d='javax/sip' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-sip.jar
	rm -f -- javax-sip-$(VERSION).jar
	rm -f -- lib-javax-sip.db

CLEAN_LOCAL += clean-javax_sip

nodist_lib_javax_sip_la_SOURCES	= javax-sip.jar
lib_javax_sip_la_LIBADD		=
lib_javax_sip_la_LDFLAGS	= -module -export-dynamic -release $(VERSION) -shared
lib_javax_sip_la_GCJFLAGS	= $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-sip.jar
ALL_LOCAL		+= javax-sip.jar
jar_DATA		+= javax-sip-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-sip.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-sip.db
ALL_LOCAL		+= lib-javax-sip.db
endif
endif

## -----------------------------------------------------------------------------

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

MY_SECONDARY		+= $(my_javax_csapi_cc_jcc_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_csapi_cc_jcc_CLASSFILES)

javax-csapi-cc-jcc-$(VERSION).jar: classdist_java.stamp $(my_javax_csapi_cc_jcc_CLASSFILES)
	$(AM_V_GEN)d='javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_csapi_cc_jcc_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_csapi_cc_jcc_JAVAFILES)

clean-javax_csapi_cc_jcc:
	@d='javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-csapi-cc-jcc.jar
	rm -f -- javax-csapi-cc-jcc-$(VERSION).jar
	rm -f -- lib-javax-csapi-cc-jcc.db

CLEAN_LOCAL += clean-javax_csapi_cc_jcc

nodist_lib_javax_csapi_cc_jcc_la_SOURCES = javax-csapi-cc-jcc.jar
lib_javax_csapi_cc_jcc_la_LIBADD	 =
lib_javax_csapi_cc_jcc_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared
lib_javax_csapi_cc_jcc_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-csapi-cc-jcc.jar
ALL_LOCAL		+= javax-csapi-cc-jcc.jar
jar_DATA		+= javax-csapi-cc-jcc-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-csapi-cc-jcc.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-csapi-cc-jcc.db
ALL_LOCAL		+= lib-javax-csapi-cc-jcc.db
endif
endif

## -----------------------------------------------------------------------------

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

MY_SECONDARY		+= $(my_javax_jcat_CLASSFILES)
my_jar_CLASSFILES	+= $(my_javax_jcat_CLASSFILES)

javax-jcat-$(VERSION).jar: classdist_java.stamp $(my_javax_jcat_CLASSFILES)
	$(AM_V_GEN)d='javax/jcat' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_javax_jcat_JAVAFILES)
#dist_java_JAVA		+= $(my_javax_jcat_JAVAFILES)

clean-javax_jcat:
	@d='javax/jcat' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- javax-jcat.jar
	rm -f -- javax-jcat-$(VERSION).jar
	rm -f -- lib-javax-jcat.db

CLEAN_LOCAL += clean-javax_jcat

nodist_lib_javax_jcat_la_SOURCES = javax-jcat.jar
lib_javax_jcat_la_LIBADD	 =
lib_javax_jcat_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared
lib_javax_jcat_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= javax-jcat.jar
ALL_LOCAL		+= javax-jcat.jar
jar_DATA		+= javax-jcat-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-javax-jcat.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-javax-jcat.db
ALL_LOCAL		+= lib-javax-jcat.db
endif
endif

## -----------------------------------------------------------------------------

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

MY_SECONDARY		+= $(my_jain_protocol_ss7_tcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jain_protocol_ss7_tcap_CLASSFILES)

jain-protocol-ss7-tcap-$(VERSION).jar: classdist_java.stamp jain-protocol-ss7.jar $(my_jain_protocol_ss7_tcap_CLASSFILES)
	$(AM_V_GEN)d='jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

java_JAVAFILES		+= $(my_jain_protocol_ss7_tcap_JAVAFILES)
#dist_java_JAVA		+= $(my_jain_protocol_ss7_tcap_JAVAFILES)

clean-jain_protocol_ss7_tcap:
	@d='jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- jain-protocol-ss7-tcap.jar
	rm -f -- jain-protocol-ss7-tcap-$(VERSION).jar
	rm -f -- lib-jain-protocol-ss7-tcap.db

CLEAN_LOCAL += clean-jain_protocol_ss7_tcap

nodist_lib_jain_protocol_ss7_tcap_la_SOURCES = jain-protocol-ss7-tcap.jar
lib_jain_protocol_ss7_tcap_la_LIBADD	     =
lib_jain_protocol_ss7_tcap_la_LDFLAGS	     = -module -export-dynamic -release $(VERSION) -shared
lib_jain_protocol_ss7_tcap_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= jain-protocol-ss7-tcap.jar
ALL_LOCAL		+= jain-protocol-ss7-tcap.jar
jar_DATA		+= jain-protocol-ss7-tcap-$(VERSION).jar
gcjlib_LTLIBRARIES 	+= lib-jain-protocol-ss7-tcap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-jain-protocol-ss7-tcap.db
ALL_LOCAL		+= lib-jain-protocol-ss7-tcap.db
endif
endif

## -----------------------------------------------------------------------------

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

MY_SECONDARY		+= $(my_jain_protocol_ss7_oam_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jain_protocol_ss7_oam_CLASSFILES)

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
	rm -f -- lib-jain-protocol-ss7-oam.db

CLEAN_LOCAL += clean-jain_protocol_ss7_oam

nodist_lib_jain_protocol_ss7_oam_la_SOURCES = jain-protocol-ss7-oam.jar
lib_jain_protocol_ss7_oam_la_LIBADD	    =
lib_jain_protocol_ss7_oam_la_LDFLAGS	    = -module -export-dynamic -release $(VERSION) -shared
lib_jain_protocol_ss7_oam_la_GCJFLAGS	    = $(AM_GCJFLAGS) $(CNIFLAGS)

if PKG_BUILD_ARCH
my_jar_LINKS		+= jain-protocol-ss7-oam.jar
ALL_LOCAL		+= jain-protocol-ss7-oam.jar
jar_DATA		+= jain-protocol-ss7-oam-$(VERSION).jar
gcjlib_LTLIBRARIES	+= lib-jain-protocol-ss7-oam.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-jain-protocol-ss7-oam.db
ALL_LOCAL		+= lib-jain-protocol-ss7-oam.db
endif
endif

## -----------------------------------------------------------------------------

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

lib_org_openss7_jain_protocol_ss7_oam_la_DEPENDENCIES	= org-openss7-jain-protocol-ss7-oam.jar $(my_oam_INCLUDES)
nodist_lib_org_openss7_jain_protocol_ss7_oam_la_SOURCES	= org-openss7-jain-protocol-ss7-oam.jar
lib_org_openss7_jain_protocol_ss7_oam_la_SOURCES	= $(my_oam_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_oam_la_CPPFLAGS	= $(GCJLIB_INCLUDES)
lib_org_openss7_jain_protocol_ss7_oam_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_jain_protocol_ss7_oam_la_CXXFLAGS	= $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_jain_protocol_ss7_oam_la_GCJFLAGS	= $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_jain_protocol_ss7_oam_la_LIBADD		= 
lib_org_openss7_jain_protocol_ss7_oam_la_LDFLAGS	= -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_jain_protocol_ss7_oam_la_OBJECTS): org-openss7-jain-protocol-ss7-oam.jar $(my_oam_INCLUDES)

org-openss7-jain-protocol-ss7-oam-$(VERSION).jar: classdist_java.stamp $(my_oam_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/jain/protocol/ss7/oam' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_oam_CLASSFILES)
my_jar_CLASSFILES	+= $(my_oam_CLASSFILES)

libopenss7OamCNI_la_DEPENDENCIES = org-openss7-jain-protocol-ss7-oam.jar $(my_oam_INCLUDES)
libopenss7OamCNI_la_SOURCES	 = $(my_oam_SOURCEFILES)
libopenss7OamCNI_la_CPPFLAGS	 = $(GCJLIB_INCLUDES)
libopenss7OamCNI_la_CFLAGS	 = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7OamCNI_la_CXXFLAGS	 = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7OamCNI_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7OamCNI_la_LIBADD	 = $(top_builddir)/libstreams.la
libopenss7OamCNI_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7OamCNI_la_OBJECTS): org-openss7-jain-protocol-ss7-oam.jar $(my_oam_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-jain-protocol-ss7-oam.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-jain-protocol-ss7-oam.db
ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-oam.db
endif
my_jar_LINKS		+= org-openss7-jain-protocol-ss7-oam.jar
ALL_LOCAL		+= org-openss7-jain-protocol-ss7-oam.jar
jar_DATA		+= org-openss7-jain-protocol-ss7-oam-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7OamCNI.la
endif

java_cni_CLASSFILES	+= $(my_oam_CLASSFILES)
java_cni_INCLUDES	+= $(my_oam_INCLUDES)
java_cni_SOURCEFILES	+= $(my_oam_SOURCEFILES)
java_JAVAFILES		+= $(my_oam_JAVAFILES)
#dist_java_JAVA		+= $(my_oam_JAVAFILES)

clean-org_openss7_jain_protocol_ss7_oam:
	@d='org/openss7/jain/protocol/ss7/oam' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_oam_INCLUDES)
	rm -f -- org-openss7-jain-protocol-ss7-oam.jar
	rm -f -- org-openss7-jain-protocol-ss7-oam-$(VERSION).jar
	rm -f -- lib-org-openss7-jain-protocol-ss7-oam.db

CLEAN_LOCAL += clean-org_openss7_jain_protocol_ss7_oam

## -----------------------------------------------------------------------------

my_jtcap_JAVAFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.java \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.java
my_jtcap_CLASSFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.class \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.class
my_jtcap_SOURCEFILES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.cc \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.cc
my_jtcap_INCLUDES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.h \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.h

lib_org_openss7_jain_protocol_ss7_tcap_la_DEPENDENCIES	 = org-openss7-jain-protocol-ss7-tcap.jar $(my_jtcap_INCLUDES)
nodist_lib_org_openss7_jain_protocol_ss7_tcap_la_SOURCES = org-openss7-jain-protocol-ss7-tcap.jar
lib_org_openss7_jain_protocol_ss7_tcap_la_SOURCES	 = $(my_jtcap_SOURCEFILES)
lib_org_openss7_jain_protocol_ss7_tcap_la_CPPFLAGS	 = $(GCJLIB_INCLUDES)
lib_org_openss7_jain_protocol_ss7_tcap_la_CFLAGS	 = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_la_CXXFLAGS	 = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_la_LIBADD	 = $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_tcap_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_jain_protocol_ss7_tcap_la_OBJECTS): org-openss7-jain-protocol-ss7-tcap.jar $(my_jtcap_INCLUDES)

org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar: classdist_java.stamp jain-protocol-ss7.jar $(my_jtcap_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jtcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jtcap_CLASSFILES)

libopenss7JtcapCNI_la_DEPENDENCIES = org-openss7-jain-protocol-ss7-tcap.jar $(my_jtcap_INCLUDES)
libopenss7JtcapCNI_la_SOURCES	   = $(my_jtcap_SOURCEFILES)
libopenss7JtcapCNI_la_CPPFLAGS	   = $(GCJLIB_INCLUDES)
libopenss7JtcapCNI_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JtcapCNI_la_CXXFLAGS	   = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JtcapCNI_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JtcapCNI_la_LIBADD	   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7JtcapCNI_la_LDFLAGS	   = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JtcapCNI_la_OBJECTS): org-openss7-jain-protocol-ss7-tcap.jar $(my_jtcap_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES	+= lib-org-openss7-jain-protocol-ss7-tcap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-jain-protocol-ss7-tcap.db
ALL_LOCAL		+= lib-org-openss7-jain-protocol-ss7-tcap.db
endif
my_jar_LINKS		+= org-openss7-jain-protocol-ss7-tcap.jar
ALL_LOCAL		+= org-openss7-jain-protocol-ss7-tcap.jar
jar_DATA		+= org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JtcapCNI.la
endif

java_cni_CLASSFILES	+= $(my_jtcap_CLASSFILES)
java_cni_INCLUDES	+= $(my_jtcap_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jtcap_SOURCEFILES)
java_JAVAFILES		+= $(my_jtcap_JAVAFILES)
#dist_java_JAVA		+= $(my_jtcap_JAVAFILES)

clean-org_openss7_jain_protocol_ss7_tcap:
	@d='org/openss7/jain/protocol/ss7/tcap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jtcap_INCLUDES)
	rm -f -- org-openss7-jain-protocol-ss7-tcap.jar
	rm -f -- org-openss7-jain-protocol-ss7-tcap-$(VERSION).jar
	rm -f -- lib-org-openss7-jain-protocol-ss7-tcap.db

CLEAN_LOCAL += clean-org_openss7_jain_protocol_ss7_tcap

## -----------------------------------------------------------------------------

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

lib_org_openss7_javax_sip_la_DEPENDENCIES   = org-openss7-javax-sip.jar $(my_jsip_INCLUDES)
nodist_lib_org_openss7_javax_sip_la_SOURCES = org-openss7-javax-sip.jar
lib_org_openss7_javax_sip_la_SOURCES	    = $(my_jsip_SOURCEFILES)
lib_org_openss7_javax_sip_la_CPPFLAGS	    = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_sip_la_CFLAGS	    = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_sip_la_CXXFLAGS	    = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_sip_la_GCJFLAGS	    = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_sip_la_LIBADD	    = $(top_builddir)/libstreams.la
lib_org_openss7_javax_sip_la_LDFLAGS	    = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_sip_la_OBJECTS): org-openss7-javax-sip.jar $(my_jsip_INCLUDES)

org-openss7-javax-sip-$(VERSION).jar: classdist_java.stamp $(my_jsip_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/sip' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jsip_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jsip_CLASSFILES)

libopenss7JsipCNI_la_DEPENDENCIES = org-openss7-javax-sip.jar $(my_jsip_INCLUDES)
libopenss7JsipCNI_la_SOURCES	  = $(my_jsip_SOURCEFILES)
libopenss7JsipCNI_la_CPPFLAGS	  = $(GCJLIB_INCLUDES)
libopenss7JsipCNI_la_CFLAGS	  = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JsipCNI_la_CXXFLAGS	  = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JsipCNI_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JsipCNI_la_LIBADD	  = $(top_builddir)/libstreams.la
libopenss7JsipCNI_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JsipCNI_la_OBJECTS): org-openss7-javax-sip.jar $(my_jsip_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-sip.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-sip.db
ALL_LOCAL		+= lib-org-openss7-javax-sip.db
endif
my_jar_LINKS		+= org-openss7-javax-sip.jar
ALL_LOCAL		+= org-openss7-javax-sip.jar
jar_DATA		+= org-openss7-javax-sip-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JsipCNI.la
endif

java_cni_CLASSFILES	+= $(my_jsip_CLASSFILES)
java_cni_INCLUDES	+= $(my_jsip_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jsip_SOURCEFILES)
java_JAVAFILES		+= $(my_jsip_JAVAFILES)
#dist_java_JAVA		+= $(my_jsip_JAVAFILES)

clean-org_openss7_javax_sip:
	@d='org/openss7/javax/sip' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jsip_INCLUDES)
	rm -f -- org-openss7-javax-sip.jar
	rm -f -- org-openss7-javax-sip-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-sip.db

CLEAN_LOCAL += clean-org_openss7_javax_sip

## -----------------------------------------------------------------------------

my_jcat_JAVAFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.java
my_jcat_CLASSFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.class
my_jcat_SOURCEFILES = \
	org/openss7/javax/jcat/JcatProviderImpl.cc
my_jcat_INCLUDES = \
	org/openss7/javax/jcat/JcatProviderImpl.h
my_jcat_DEPENDS = \
	org/openss7/javax/csapi/cc/jcc/JccPeerImpl.h \
	org/openss7/javax/csapi/cc/jcc/JccProviderImpl.h

lib_org_openss7_javax_jcat_la_DEPENDENCIES   = org-openss7-javax-jcat.jar $(my_jcat_INCLUDES) $(my_jcat_DEPENDS)
nodist_lib_org_openss7_javax_jcat_la_SOURCES = org-openss7-javax-jcat.jar
lib_org_openss7_javax_jcat_la_SOURCES	     = $(my_jcat_SOURCEFILES)
lib_org_openss7_javax_jcat_la_CPPFLAGS	     = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_jcat_la_CFLAGS	     = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jcat_la_CXXFLAGS	     = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_jcat_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_jcat_la_LIBADD	     = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jcat_la_LDFLAGS	     = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_jcat_la_OBJECTS): org-openss7-javax-jcat.jar $(my_jcat_INCLUDES) $(my_jcat_DEPENDS)

org-openss7-javax-jcat-$(VERSION).jar: classdist_java.stamp $(my_jcat_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/jcat' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jcat_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jcat_CLASSFILES)

libopenss7JcatCNI_la_DEPENDENCIES = org-openss7-javax-jcat.jar $(my_jcat_INCLUDES)
libopenss7JcatCNI_la_SOURCES	  = $(my_jcat_SOURCEFILES)
libopenss7JcatCNI_la_CPPFLAGS	  = $(GCJLIB_INCLUDES)
libopenss7JcatCNI_la_CFLAGS	  = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JcatCNI_la_CXXFLAGS	  = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JcatCNI_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JcatCNI_la_LIBADD	  = $(top_builddir)/libstreams.la
libopenss7JcatCNI_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JcatCNI_la_OBJECTS): org-openss7-javax-jcat.jar $(my_jcat_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jcat.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-jcat.db
ALL_LOCAL		+= lib-org-openss7-javax-jcat.db
endif
my_jar_LINKS		+= org-openss7-javax-jcat.jar
ALL_LOCAL		+= org-openss7-javax-jcat.jar
jar_DATA		+= org-openss7-javax-jcat-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JcatCNI.la
endif

java_cni_CLASSFILES	+= $(my_jcat_CLASSFILES)
java_cni_INCLUDES	+= $(my_jcat_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jcat_SOURCEFILES)
java_JAVAFILES		+= $(my_jcat_JAVAFILES)
#dist_java_JAVA		+= $(my_jcat_JAVAFILES)

clean-org_openss7_javax_jcat:
	@d='org/openss7/javax/jcat' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jcat_INCLUDES)
	rm -f -- org-openss7-javax-jcat.jar
	rm -f -- org-openss7-javax-jcat-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jcat.db

CLEAN_LOCAL += clean-org_openss7_javax_jcat

## -----------------------------------------------------------------------------

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

lib_org_openss7_javax_jain_ss7_isup_la_DEPENDENCIES   = org-openss7-javax-jain-ss7-isup.jar $(my_jisup_INCLUDES)
nodist_lib_org_openss7_javax_jain_ss7_isup_la_SOURCES = org-openss7-javax-jain-ss7-isup.jar
lib_org_openss7_javax_jain_ss7_isup_la_SOURCES	      = $(my_jisup_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_isup_la_CPPFLAGS	      = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_jain_ss7_isup_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_ss7_isup_la_CXXFLAGS	      = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_jain_ss7_isup_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_jain_ss7_isup_la_LIBADD	      = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_isup_la_LDFLAGS	      = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_jain_ss7_isup_la_OBJECTS): org-openss7-javax-jain-ss7-isup.jar $(my_jisup_INCLUDES)

org-openss7-javax-jain-ss7-isup-$(VERSION).jar: classdist_java.stamp $(my_jisup_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jisup_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jisup_CLASSFILES)

libopenss7JisupCNI_la_DEPENDENCIES = org-openss7-javax-jain-ss7-isup.jar $(my_jisup_INCLUDES)
libopenss7JisupCNI_la_SOURCES	   = $(my_jisup_SOURCEFILES)
libopenss7JisupCNI_la_CPPFLAGS	   = $(GCJLIB_INCLUDES)
libopenss7JisupCNI_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JisupCNI_la_CXXFLAGS	   = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JisupCNI_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JisupCNI_la_LIBADD	   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7JisupCNI_la_LDFLAGS	   = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JisupCNI_la_OBJECTS): org-openss7-javax-jain-ss7-isup.jar $(my_jisup_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-isup.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-jain-ss7-isup.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-isup.db
endif
my_jar_LINKS		+= org-openss7-javax-jain-ss7-isup.jar
ALL_LOCAL		+= org-openss7-javax-jain-ss7-isup.jar
jar_DATA		+= org-openss7-javax-jain-ss7-isup-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JisupCNI.la
endif

java_cni_CLASSFILES	+= $(my_jisup_CLASSFILES)
java_cni_INCLUDES	+= $(my_jisup_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jisup_SOURCEFILES)
java_JAVAFILES		+= $(my_jisup_JAVAFILES)
#dist_java_JAVA		+= $(my_jisup_JAVAFILES)

clean-org_openss7_javax_jain_ss7_isup:
	@d='org/openss7/javax/jain/ss7/isup' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jisup_INCLUDES)
	rm -f -- org-openss7-javax-jain-ss7-isup.jar
	rm -f -- org-openss7-javax-jain-ss7-isup-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-ss7-isup.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_ss7_isup

## -----------------------------------------------------------------------------

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

lib_org_openss7_javax_jain_ss7_inap_la_DEPENDENCIES   = org-openss7-javax-jain-ss7-inap.jar $(my_jinap_INCLUDES)
nodist_lib_org_openss7_javax_jain_ss7_inap_la_SOURCES = org-openss7-javax-jain-ss7-inap.jar
lib_org_openss7_javax_jain_ss7_inap_la_SOURCES	      = $(my_jinap_SOURCEFILES)
lib_org_openss7_javax_jain_ss7_inap_la_CPPFLAGS	      = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_jain_ss7_inap_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_ss7_inap_la_CXXFLAGS	      = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_jain_ss7_inap_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_jain_ss7_inap_la_LIBADD	      = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_ss7_inap_la_LDFLAGS	      = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_jain_ss7_inap_la_OBJECTS): org-openss7-javax-jain-ss7-inap.jar $(my_jinap_INCLUDES)

org-openss7-javax-jain-ss7-inap-$(VERSION).jar: classdist_java.stamp $(my_jinap_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jinap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jinap_CLASSFILES)

libopenss7JinapCNI_la_DEPENDENCIES = org-openss7-javax-jain-ss7-inap.jar $(my_jinap_INCLUDES)
libopenss7JinapCNI_la_SOURCES	   = $(my_jinap_SOURCEFILES)
libopenss7JinapCNI_la_CPPFLAGS	   = $(GCJLIB_INCLUDES)
libopenss7JinapCNI_la_CFLAGS	   = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JinapCNI_la_CXXFLAGS	   = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JinapCNI_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JinapCNI_la_LIBADD	   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7JinapCNI_la_LDFLAGS	   = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JinapCNI_la_OBJECTS): org-openss7-javax-jain-ss7-inap.jar $(my_jinap_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-ss7-inap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-jain-ss7-inap.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-ss7-inap.db
endif
my_jar_LINKS		+= org-openss7-javax-jain-ss7-inap.jar
ALL_LOCAL		+= org-openss7-javax-jain-ss7-inap.jar
jar_DATA		+= org-openss7-javax-jain-ss7-inap-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JinapCNI.la
endif

java_cni_CLASSFILES	+= $(my_jinap_CLASSFILES)
java_cni_INCLUDES	+= $(my_jinap_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jinap_SOURCEFILES)
java_JAVAFILES		+= $(my_jinap_JAVAFILES)
#dist_java_JAVA		+= $(my_jinap_JAVAFILES)

clean-org_openss7_javax_jain_ss7_inap:
	@d='org/openss7/javax/jain/ss7/inap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jinap_INCLUDES)
	rm -f -- org-openss7-javax-jain-ss7-inap.jar
	rm -f -- org-openss7-javax-jain-ss7-inap-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-ss7-inap.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_ss7_inap

## -----------------------------------------------------------------------------

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

lib_org_openss7_javax_jain_protocol_ss7_map_la_DEPENDENCIES   = org-openss7-javax-jain-protocol-ss7-map.jar $(my_jmap_INCLUDES)
nodist_lib_org_openss7_javax_jain_protocol_ss7_map_la_SOURCES = org-openss7-javax-jain-protocol-ss7-map.jar
lib_org_openss7_javax_jain_protocol_ss7_map_la_SOURCES	      = $(my_jmap_SOURCEFILES)
lib_org_openss7_javax_jain_protocol_ss7_map_la_CPPFLAGS	      = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_jain_protocol_ss7_map_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_la_CXXFLAGS	      = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_jain_protocol_ss7_map_la_LIBADD	      = $(top_builddir)/libstreams.la
lib_org_openss7_javax_jain_protocol_ss7_map_la_LDFLAGS	      = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_jain_protocol_ss7_map_la_OBJECTS): org-openss7-javax-jain-protocol-ss7-map.jar $(my_jmap_INCLUDES)

org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar: classdist_java.stamp $(my_jmap_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jmap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jmap_CLASSFILES)

libopenss7JmapCNI_la_DEPENDENCIES = org-openss7-javax-jain-protocol-ss7-map.jar $(my_jmap_INCLUDES)
libopenss7JmapCNI_la_SOURCES	  = $(my_jmap_SOURCEFILES)
libopenss7JmapCNI_la_CPPFLAGS	  = $(GCJLIB_INCLUDES)
libopenss7JmapCNI_la_CFLAGS	  = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JmapCNI_la_CXXFLAGS	  = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JmapCNI_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JmapCNI_la_LIBADD	  = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7JmapCNI_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JmapCNI_la_OBJECTS): org-openss7-javax-jain-protocol-ss7-map.jar $(my_jmap_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-jain-protocol-ss7-map.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-jain-protocol-ss7-map.db
ALL_LOCAL		+= lib-org-openss7-javax-jain-protocol-ss7-map.db
endif
my_jar_LINKS		+= org-openss7-javax-jain-protocol-ss7-map.jar
ALL_LOCAL		+= org-openss7-javax-jain-protocol-ss7-map.jar
jar_DATA		+= org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JmapCNI.la
endif

java_cni_CLASSFILES	+= $(my_jmap_CLASSFILES)
java_cni_INCLUDES	+= $(my_jmap_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jmap_SOURCEFILES)
java_JAVAFILES		+= $(my_jmap_JAVAFILES)
#dist_java_JAVA		+= $(my_jmap_JAVAFILES)

clean-org_openss7_javax_jain_protocol_ss7_map:
	@d='org/openss7/javax/jain/protocol/ss7/map' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jmap_INCLUDES)
	rm -f -- org-openss7-javax-jain-protocol-ss7-map.jar
	rm -f -- org-openss7-javax-jain-protocol-ss7-map-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-jain-protocol-ss7-map.db

CLEAN_LOCAL += clean-org_openss7_javax_jain_protocol_ss7_map

## -----------------------------------------------------------------------------

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

lib_org_openss7_javax_csapi_cc_jcc_la_DEPENDENCIES   = org-openss7-javax-csapi-cc-jcc.jar $(my_jcc_INCLUDES)
nodist_lib_org_openss7_javax_csapi_cc_jcc_la_SOURCES = org-openss7-javax-csapi-cc-jcc.jar
lib_org_openss7_javax_csapi_cc_jcc_la_SOURCES	     = $(my_jcc_SOURCEFILES)
lib_org_openss7_javax_csapi_cc_jcc_la_CPPFLAGS	     = $(GCJLIB_INCLUDES)
lib_org_openss7_javax_csapi_cc_jcc_la_CFLAGS	     = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_la_CXXFLAGS	     = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_javax_csapi_cc_jcc_la_LIBADD	     = $(top_builddir)/libstreams.la
lib_org_openss7_javax_csapi_cc_jcc_la_LDFLAGS	     = -module -export-dynamic -release $(VERSION) -shared

$(lib_org_openss7_javax_csapi_cc_jcc_la_OBJECTS): org-openss7-javax-csapi-cc-jcc.jar $(my_jcc_INCLUDES)

org-openss7-javax-csapi-cc-jcc-$(VERSION).jar: classdist_java.stamp $(my_jcc_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_jcc_CLASSFILES)
my_jar_CLASSFILES	+= $(my_jcc_CLASSFILES)

libopenss7JccCNI_la_DEPENDENCIES = org-openss7-javax-csapi-cc-jcc.jar $(my_jcc_INCLUDES)
libopenss7JccCNI_la_SOURCES	 = $(my_jcc_SOURCEFILES)
libopenss7JccCNI_la_CPPFLAGS	 = $(GCJLIB_INCLUDES)
libopenss7JccCNI_la_CFLAGS	 = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7JccCNI_la_CXXFLAGS	 = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7JccCNI_la_GCJFLAGS	 = $(AM_GCJFLAGS) $(CNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7JccCNI_la_LIBADD	 = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7JccCNI_la_LDFLAGS	 = -module -export-dynamic -release $(VERSION) -shared

$(libopenss7JccCNI_la_OBJECTS): org-openss7-javax-csapi-cc-jcc.jar $(my_jcc_INCLUDES)

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-javax-csapi-cc-jcc.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-javax-csapi-cc-jcc.db
ALL_LOCAL		+= lib-org-openss7-javax-csapi-cc-jcc.db
endif
my_jar_LINKS		+= org-openss7-javax-csapi-cc-jcc.jar
ALL_LOCAL		+= org-openss7-javax-csapi-cc-jcc.jar
jar_DATA		+= org-openss7-javax-csapi-cc-jcc-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7JccCNI.la
endif

java_cni_CLASSFILES	+= $(my_jcc_CLASSFILES)
java_cni_INCLUDES	+= $(my_jcc_INCLUDES)
java_cni_SOURCEFILES	+= $(my_jcc_SOURCEFILES)
java_JAVAFILES		+= $(my_jcc_JAVAFILES)
#dist_java_JAVA		+= $(my_jcc_JAVAFILES)

clean-org_openss7_javax_csapi_cc_jcc:
	@d='org/openss7/javax/csapi/cc/jcc' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_jcc_INCLUDES)
	rm -f -- org-openss7-javax-csapi-cc-jcc.jar
	rm -f -- org-openss7-javax-csapi-cc-jcc-$(VERSION).jar
	rm -f -- lib-org-openss7-javax-csapi-cc-jcc.db

CLEAN_LOCAL += clean-org_openss7_javax_csapi_cc_jcc

#############################################################################
# This is how we build STREAMS JNI classes for Java.
#############################################################################

#	org/openss7/streams/bandinfo.java
#	org/openss7/streams/strbuf.java
#	org/openss7/streams/StreamsConstants.java
#	org/openss7/streams/Streams.java
#	org/openss7/streams/StreamsJNI.java
#	org/openss7/streams/strfdinsert.java
#	org/openss7/streams/strioctl.java
#	org/openss7/streams/str_list.java
#	org/openss7/streams/str_mlist.java
#	org/openss7/streams/strpeek.java
#	org/openss7/streams/strpmsg.java

my_streams_JAVAFILES = \
	org/openss7/streams/Error.java \
	org/openss7/streams/Strbuf.java \
	org/openss7/streams/Stream.java \
	org/openss7/streams/Streams.java \
	org/openss7/streams/StreamsJNI.java \
	org/openss7/streams/SWIGTYPE_p_p_streams__Stream.java
my_streams_CLASSFILES = \
	org/openss7/streams/Error.class \
	org/openss7/streams/Strbuf.class \
	org/openss7/streams/Stream.class \
	org/openss7/streams/Streams.class \
	org/openss7/streams/StreamsJNI.class \
	org/openss7/streams/SWIGTYPE_p_p_streams__Stream.class
my_streams_SOURCEFILES = \
	org/openss7/streams/streams_wrap.cxx

#nodist_lib_org_openss7_streams_la_SOURCES = org-openss7-streams.jar
lib_org_openss7_streams_la_SOURCES 	  = $(my_streams_JAVAFILES)
lib_org_openss7_streams_la_CPPFLAGS	  = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_streams_la_CFLAGS	  = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_streams_la_CXXFLAGS	  = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_streams_la_GCJFLAGS	  = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_streams_la_LIBADD	  = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_streams_la_LDFLAGS	  = -module -export-dynamic -release $(VERSION) -shared

org-openss7-streams-$(VERSION).jar: classdist_java.stamp $(my_streams_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/streams' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_streams_CLASSFILES)
my_jar_CLASSFILES	+= $(my_streams_CLASSFILES)

nodist_libopenss7StreamsJNI_la_SOURCES = $(my_streams_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7StreamsJNI_la_CPPFLAGS       = -I$(builddir)/org/openss7/streams -I$(srcdir)/org/openss7/streams $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7StreamsJNI_la_CFLAGS	       = $(USER_CFLAGS) -Wno-unused $(USER_DFLAGS)
libopenss7StreamsJNI_la_CXXFLAGS       = $(USER_CXXFLAGS) -Wno-unused $(USER_DXXFLAGS)
libopenss7StreamsJNI_la_GCJFLAGS       = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7StreamsJNI_la_LIBADD	       = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7StreamsJNI_la_LDFLAGS	       = -module -export-dynamic -release $(VERSION) -shared

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-streams.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-streams.db
ALL_LOCAL		+= lib-org-openss7-streams.db
endif
my_jar_LINKS		+= org-openss7-streams.jar
ALL_LOCAL		+= org-openss7-streams.jar
jar_DATA		+= org-openss7-streams-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7StreamsJNI.la
endif

java_cxx_MODULES	+= org/openss7/streams/streams.i
swig_JAVAFILES		+= $(my_streams_JAVAFILES)
MY_SECONDARY		+= $(my_streams_CLASSFILES)

$(my_streams_JAVAFILES): $(my_streams_SOURCEFILES)

java_JAVAFILES		+= $(my_streams_JAVAFILES)
#dist_java_JAVA		+= $(my_streams_JAVAFILES)

clean-org_openss7_streams:
	@d='org/openss7/streams' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_streams_JAVAFILES)
	rm -f -- org/openss7/streams/streams_wrap.c
	rm -f -- org/openss7/streams/streams_wrap.cxx
	rm -f -- org-openss7-streams.jar org-openss7-streams-$(VERSION).jar
	rm -f -- lib-org-openss7-streams.db

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

#nodist_lib_org_openss7_xti_la_SOURCES = org-openss7-xti.jar
lib_org_openss7_xti_la_SOURCES	      = $(my_xti_JAVAFILES)
lib_org_openss7_xti_la_CPPFLAGS	      = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_xti_la_CFLAGS	      = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_xti_la_CXXFLAGS	      = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_xti_la_GCJFLAGS	      = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_xti_la_LIBADD	      = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_xti_la_LDFLAGS	      = -module -export-dynamic -release $(VERSION) -shared

org-openss7-xti-$(VERSION).jar: classdist_java.stamp $(my_xti_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/xti' ; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_xti_CLASSFILES)
my_jar_CLASSFILES	+= $(my_xti_CLASSFILES)

nodist_libopenss7XtiJNI_la_SOURCES = $(my_xti_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtiJNI_la_CPPFLAGS	   = -I$(builddir)/org/openss7/xti -I$(srcdir)/org/openss7/xti $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7XtiJNI_la_CFLAGS	   = $(USER_CFLAGS)
libopenss7XtiJNI_la_CXXFLAGS	   = $(USER_CXXFLAGS)
libopenss7XtiJNI_la_GCJFLAGS	   = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS)
libopenss7XtiJNI_la_LIBADD	   = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtiJNI_la_LDFLAGS	   = -module -export-dynamic -release $(VERSION) -shared

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-xti.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-xti.db
ALL_LOCAL		+= lib-org-openss7-xti.db
endif
my_jar_LINKS		+= org-openss7-xti.jar
ALL_LOCAL		+= org-openss7-xti.jar
jar_DATA		+= org-openss7-xti-$(VERSION).jar
jni_LTLIBRARIES		+= libopenss7XtiJNI.la
endif

java_c_MODULES		+= org/openss7/xti/xti.i
swig_JAVAFILES		+= $(my_xti_JAVAFILES)
MY_SECONDARY		+= $(my_xti_CLASSFILES)

$(my_xti_JAVAFILES): $(my_xti_SOURCEFILES)

java_JAVAFILES		+= $(my_xti_JAVAFILES)
#dist_java_JAVA		+= $(my_xti_JAVAFILES)

clean-org_openss7_xti:
	@d='org/openss7/xti' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_xti_JAVAFILES)
	rm -f -- org/openss7/xti/xti_wrap.c
	rm -f -- org/openss7/xti/xti_wrap.cxx
	rm -f -- org-openss7-xti.jar org-openss7-xti-$(VERSION).jar
	rm -f -- lib-org-openss7-xti.db

CLEAN_LOCAL += clean-org_openss7_xti

#############################################################################
# This is how we build TCAP JNI classes for Java.
#############################################################################

my_tcap_JAVAFILES = \
	org/openss7/tcap/SccpAddress.java \
	org/openss7/tcap/sccp_addr_t.java \
	org/openss7/tcap/SWIGTYPE_p_std__listT_tci__TcapOption_t.java \
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
	org/openss7/tcap/TciError.java \
	org/openss7/tcap/TC_info_ack_t.java \
	org/openss7/tcap/t_opthdr.java

my_tcap_CLASSFILES = \
	org/openss7/tcap/SccpAddress.class \
	org/openss7/tcap/sccp_addr_t.class \
	org/openss7/tcap/SWIGTYPE_p_std__listT_tci__TcapOption_t.class \
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
	org/openss7/tcap/TciError.class \
	org/openss7/tcap/TC_info_ack_t.class \
	org/openss7/tcap/t_opthdr.class
my_tcap_SOURCEFILES = \
	org/openss7/tcap/tcap_wrap.cxx

#nodist_lib_org_openss7_tcap_la_SOURCES = org-openss7-tcap.jar
lib_org_openss7_tcap_la_SOURCES	       = $(my_tcap_JAVAFILES)
lib_org_openss7_tcap_la_CPPFLAGS       = $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_tcap_la_CFLAGS	       = $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_tcap_la_CXXFLAGS       = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_tcap_la_GCJFLAGS       = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_tcap_la_LIBADD	       = $(top_builddir)/libstreams.la
lib_org_openss7_tcap_la_LDFLAGS	       = -module -export-dynamic -release $(VERSION) -shared

org-openss7-tcap-$(VERSION).jar: classdist_java.stamp $(my_xti_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/tcap'; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_xti_CLASSFILES)
my_jar_CLASSFILES	+= $(my_xti_CLASSFILES)

nodist_libopenss7TcapJNI_la_SOURCES = $(my_tcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7TcapJNI_la_CPPFLAGS	    = -I$(builddir)/org/openss7/tcap -I$(srcdir)/org/openss7/tcap $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7TcapJNI_la_CFLAGS	    = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7TcapJNI_la_CXXFLAGS	    = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7TcapJNI_la_GCJFLAGS	    = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7TcapJNI_la_LIBADD	    = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7TcapJNI_la_LDFLAGS	    = -module -export-dynamic -release $(VERSION) -shared

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-tcap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-tcap.db
ALL_LOCAL		+= lib-org-openss7-tcap.db
endif
my_jar_LINKS		+= org-openss7-tcap.jar
ALL_LOCAL		+= org-openss7-tcap.jar
jar_DATA		+= org-openss7-tcap-$(VERSION).jar
#jni_LTLIBRARIES	+= libopenss7TcapJNI.la
endif
EXTRA_LTLIBRARIES	+= libopenss7TcapJNI.la

java_cxx_MODULES	+= org/openss7/tcap/tcap.i
swig_JAVAFILES		+= $(my_tcap_JAVAFILES)
MY_SECONDARY		+= $(my_tcap_CLASSFILES)

$(my_tcap_JAVAFILES): $(my_tcap_SOURCEFILES)

java_JAVAFILES		+= $(my_tcap_JAVAFILES)
#dist_java_JAVA		+= $(my_tcap_JAVAFILES)

clean-org_openss7_tcap:
	@d='org/openss7/tcap'; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_tcap_JAVAFILES)
	rm -f -- org/openss7/tcap/tcap_wrap.c
	rm -f -- org/openss7/tcap/tcap_wrap.cxx
	rm -f -- org-openss7-tcap.jar org-openss7-tcap-$(VERSION).jar
	rm -f -- lib-org-openss7-tcap.db

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

#nodist_lib_org_openss7_xtcap_la_SOURCES = org-openss7-xtcap.jar
lib_org_openss7_xtcap_la_SOURCES	= $(my_xtcap_JAVAFILES)
lib_org_openss7_xtcap_la_CPPFLAGS	= $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
lib_org_openss7_xtcap_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
lib_org_openss7_xtcap_la_CXXFLAGS	= $(USER_CXXFLAGS) $(USER_DXXFLAGS)
lib_org_openss7_xtcap_la_GCJFLAGS	= $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
lib_org_openss7_xtcap_la_LIBADD		= $(top_builddir)/libstreams.la
lib_org_openss7_xtcap_la_LDFLAGS	= -module -export-dynamic -release $(VERSION) -shared

org-openss7-xtcap-$(VERSION).jar: classdist_java.stamp $(my_xtcap_CLASSFILES)
	$(AM_V_GEN)d='org/openss7/xtcap'; \
	find $$d -name '*.class' -type f | sort -u | $(JAR) c@f $@

MY_SECONDARY		+= $(my_xtcap_CLASSFILES)
my_jar_CLASSFILES	+= $(my_xtcap_CLASSFILES)

nodist_libopenss7XtcapJNI_la_SOURCES = $(my_xtcap_SOURCEFILES)
## Note: SWIG is really bad at using aliasing in Java JNI files.
libopenss7XtcapJNI_la_CPPFLAGS	     = -I$(builddir)/org/openss7/xtcap -I$(srcdir)/org/openss7/xtcap $(GCJLIB_INCLUDES) -fno-strict-aliasing -DJNICALL=
libopenss7XtcapJNI_la_CFLAGS	     = $(USER_CFLAGS) $(USER_DFLAGS)
libopenss7XtcapJNI_la_CXXFLAGS	     = $(USER_CXXFLAGS) $(USER_DXXFLAGS)
libopenss7XtcapJNI_la_GCJFLAGS	     = $(AM_GCJFLAGS) $(JNIFLAGS) $(USER_GCJFLAGS) $(USER_GCDFLAGS)
libopenss7XtcapJNI_la_LIBADD	     = $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtcapJNI_la_LDFLAGS	     = -module -export-dynamic -release $(VERSION) -shared

if PKG_BUILD_ARCH
gcjlib_LTLIBRARIES 	+= lib-org-openss7-xtcap.la
if WITH_GCJDBTOOL
gcjdb_DATA		+= lib-org-openss7-xtcap.db
ALL_LOCAL		+= lib-org-openss7-xtcap.db
endif
my_jar_LINKS		+= org-openss7-xtcap.jar
ALL_LOCAL		+= org-openss7-xtcap.jar
jar_DATA		+= org-openss7-xtcap-$(VERSION).jar
#jni_LTLIBRARIES	+= libopenss7XtcapJNI.la
endif
EXTRA_LTLIBRARIES	+= libopenss7XtcapJNI.la

java_c_MODULES		+= org/openss7/xtcap/xtcap.i
swig_JAVAFILES		+= $(my_xtcap_JAVAFILES)
MY_SECONDARY		+= $(my_xtcap_CLASSFILES)

$(my_xtcap_JAVAFILES): $(my_xtcap_SOURCEFILES)

clean-org_openss7_xtcap:
	@d='org/openss7/xtcap' ; \
	find $$d -name '*.class' -type f | $(am__base_remove)
	rm -f -- $(my_xtcap_JAVAFILES)
	rm -f -- org/openss7/xtcap/xtcap_wrap.c
	rm -f -- org/openss7/xtcap/xtcap_wrap.cxx
	rm -f -- org-openss7-xtcap.jar org-openss7-xtcap-$(VERSION).jar
	rm -f -- lib-org-openss7-xtcap.db

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
# Revision 1.1.2.7  2009-07-21 11:06:15  brian
# - changes from release build
#
# Revision 1.1.2.6  2009-07-13 07:13:30  brian
# - changes for multiple distro build
#
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
