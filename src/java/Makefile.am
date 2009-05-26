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

javadir = $(datadir)/java

lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES = \
	org/openss7/jain/protocol/ss7/tcap/JainTcapProviderImpl.java \
	org/openss7/jain/protocol/ss7/tcap/JainTcapStackImpl.java

lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CPPFLAGS 	= $(JAVALIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_jain_protocol_ss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared

lib_org_openss7_tcap_jar_la_SOURCES = \
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

lib_org_openss7_tcap_jar_la_CPPFLAGS 	= $(JAVALIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_tcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_tcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_tcap_jar_la_LDFLAGS	= -module -avoid-version -shared

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

lib_org_openss7_xti_jar_la_SOURCES = \
	$(my_xti_JAVAFILES) \
	org/openss7/xti/xti_wrap.c

lib_org_openss7_xti_jar_la_CPPFLAGS 	= $(JAVALIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xti_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xti_jar_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
lib_org_openss7_xti_jar_la_LDFLAGS	= -module -avoid-version -shared

lib_org_openss7_xtcap_jar_la_SOURCES = \
	org/openss7/xtcap/OM_descriptor.java \
	org/openss7/xtcap/OM_padded_object.java \
	org/openss7/xtcap/OMP_return_code.java \
	org/openss7/xtcap/OM_string.java \
	org/openss7/xtcap/OM_value.java \
	org/openss7/xtcap/SWIGTYPE_p_OM_value.java \
	org/openss7/xtcap/SWIGTYPE_p_p_OM_descriptor_struct.java \
	org/openss7/xtcap/SWIGTYPE_p_void.java \
	org/openss7/xtcap/TCAP_feature.java \
	org/openss7/xtcap/TCAP_waiting_sessions.java \
	org/openss7/xtcap/XtcapConstants.java \
	org/openss7/xtcap/Xtcap.java \
	org/openss7/xtcap/XtcapJNI.java

lib_org_openss7_xtcap_jar_la_CPPFLAGS 	= $(JAVALIB_INCLUDES) -fno-strict-aliasing
lib_org_openss7_xtcap_jar_la_CFLAGS	= $(USER_CFLAGS)
lib_org_openss7_xtcap_jar_la_LIBADD	= $(top_builddir)/libstreams.la
lib_org_openss7_xtcap_jar_la_LDFLAGS	= -module -avoid-version -shared

jarlibdir = $(libdir)/gcj/$(PACKAGE_LCNAME)

jarlib_LTLIBRARIES = \
	lib-org-openss7-xtcap.jar.la \
	lib-org-openss7-tcap.jar.la \
	lib-org-openss7-xti.jar.la

EXTRA_LTLIBRARIES = \
	lib-org-openss7-jain-protocol-ss7-tcap.jar.la

dist_java_JAVA =

#dist_java_JAVA = \
#	$(lib_org_openss7_jain_protocol_ss7_tcap_jar_la_SOURCES) \
#	$(my_xti_JAVAFILES) \
#	$(lib_org_openss7_xtcap_jar_la_SOURCES) \
#	$(lib_org_openss7_tcap_jar_la_SOURCES)

JAVAROOT = $(top_builddir)/src/java
#CLASSPATH =
AM_JAVAFLAGS = -target 1.2 -warn:all

JAVALIB_INCLUDES		= -I$(top_builddir)/src/java -I$(top_srcdir)/src/java $(PKG_INCLUDES) $(USER_CPPFLAGS)

javalibdir = $(libdir)/java
javalib_LTLIBRARIES = libopenss7XtiJNI.la

##
## Note: SWIG is really bad at using aliasing in Java JNI files, so we need to suppress this
## optimization in the GNU compiler or things will break.
##
libopenss7XtiJNI_la_SOURCES 	= org/openss7/xti/xti_wrap.c
libopenss7XtiJNI_la_CPPFLAGS 	= $(JAVALIB_INCLUDES) -fno-strict-aliasing
libopenss7XtiJNI_la_CFLAGS	= $(USER_CFLAGS)
libopenss7XtiJNI_la_LIBADD	= $(top_builddir)/libstreams.la $(top_builddir)/libxnet.la
libopenss7XtiJNI_la_LDFLAGS	= -module -avoid-version -shared

SWIG = swig

JAR = fastjar

org/openss7/tcap/tcap_wrap.cxx: org/openss7/tcap/tcap.hh

org/openss7/tcap/tcap_wrap.cxx: org/openss7/tcap/tcap.i
	$(AM_V_at)$(MKDIR_P) `dirname $@`
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	$(SWIG) -java -c++ -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -package `echo $$f | sed -e 's,^//*,,;s,/[^/]*$$,,;s,/,.,g'` -o $@ $<

$(lib_org_openss7_tcap_jar_la_SOURCES:%.java=%.class): classes_tcap_java.stamp

openss7Tcap-$(VERSION).jar: $(lib_org_openss7_tcap_jar_la_SOURCES:%.java=%.class)
	$(AM_V_GEN)$(JAR) cf $@ `find org/openss7/tcap -name '*.class'`

$(lib_org_openss7_tcap_jar_la_SOURCES): org/openss7/tcap/tcap_wrap.cxx

$(my_xti_JAVAFILES): org/openss7/xti/xti_wrap.c

org/openss7/xti/xti_wrap.c: org/openss7/xti/xti.i
	$(AM_V_at)$(MKDIR_P) `dirname $@`
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	$(SWIG) -java -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -package `echo $$f | sed -e 's,^//*,,;s,/[^/]*$$,,;s,/,.,g'` -o $@ $<

$(my_xti_JAVAFILES:%.java=%.class): classes_xti_java.stamp

openss7Xti-$(VERSION).jar: $(my_xti_JAVAFILES:%.java=%.class)
	$(AM_V_GEN)$(JAR) cf $@ `find org/openss7/xti -name '*.class'`

$(lib_org_openss7_xtcap_jar_la_SOURCES): org/openss7/xtcap/xtcap_wrap.c

org/openss7/xtcap/xtcap_wrap.c: org/openss7/xtcap/xtcap.i
	$(AM_V_at)$(MKDIR_P) `dirname $@`
	$(AM_V_GEN)p=$< ; $(am__vpath_adj_setup) $(am__vpath_adj) \
	$(SWIG) -java -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -package `echo $$f | sed -e 's,^//*,,;s,/[^/]*$$,,;s,/,.,g'` -o $@ $<

$(lib_org_openss7_xtcap_jar_la_SOURCES:%.java=%.class): classes_xtcap_java.stamp

openss7Xtcap-$(VERSION).jar: $(lib_org_openss7_xtcap_jar_la_SOURCES:%.java=%.class)
	$(AM_V_GEN)$(JAR) cf $@ `find org/openss7/xtcap -name '*.class'`

ALL_LOCAL += openss7Tcap-$(VERSION).jar  org/openss7/tcap/tcap_wrap.cxx
ALL_LOCAL += openss7Xti-$(VERSION).jar   org/openss7/xti/xti_wrap.c
ALL_LOCAL += openss7Xtcap-$(VERSION).jar org/openss7/xtcap/xtcap_wrap.c

CLEANFILES += *.jar
CLEANFILES += org/openss7/tcap/tcap_wrap.c org/openss7/tcap/tcap_wrap.cxx org/openss7/tcap/*.java org/openss7/tcap/*.class
CLEANFILES += org/openss7/xti/xti_wrap.c org/openss7/xti/xti_wrap.cxx org/openss7/xti/*.java org/openss7/xti/*.class
CLEANFILES += org/openss7/xtcap/xtcap_wrap.c org/openss7/xtcap/xtcap_wrap.cxx org/openss7/xtcap/*.java org/openss7/xtcap/*.class

classes_xti_java.stamp: $(my_xti_JAVAFILES)
	@list1='$?'; list2=; if test -n "$$list1"; then \
	  for p in $$list1; do \
	    if test -f $$p; then d=; else d="$(srcdir)/"; fi; \
	    list2="$$list2 $$d$$p"; \
	    $(ECHO_Q) "  GEN   " `basename $$p .java`.class; \
	  done; \
	  $(ECHO_V) '$(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) '"$$list2"; \
	  $(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) $$list2; \
	else :; fi
	$(AM_V_at)echo timestamp > classes_xti_java.stamp

ALL_LOCAL += classes_xti_java.stamp

install-my_xti_JAVAFILES: classes_xti_java.stamp
	@$(NORMAL_INSTALL)
	test -z "$(javadir)" || $(MKDIR_P) "$(DESTDIR)$(javadir)"
	@test -n "$(my_xti_JAVAFILES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " $(INSTALL_DATA)" "$$@" "'$(DESTDIR)$(javadir)/$$p'"; \
	$(INSTALL_DATA) "$$@" "$(DESTDIR)$(javadir)"

INSTALL_DATA_LOCAL += install-my_xti_JAVAFILES

uninstall-my_xti_JAVAFILES:
	@$(NORMAL_UNINSTALL)
	@test -n "$(my_xti_JAVAFILES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " ( cd '$(DESTDIR)$(javadir)' && rm -f" "$$@" ")"; \
	cd "$(DESTDIR)$(javadir)" && rm -f "$$@"

UNINSTALL_LOCAL += uninstall-my_xti_JAVAFILES

clean-my_xti_JAVAFILES:
	-rm -f *.class classes_xti_java.stamp

CLEAN_LOCAL += clean-my_xti_JAVAFILES


CLEANFILES += classes_xti_java.stamp

classes_tcap_java.stamp: $(lib_org_openss7_tcap_jar_la_SOURCES)
	@list1='$?'; list2=; if test -n "$$list1"; then \
	  for p in $$list1; do \
	    if test -f $$p; then d=; else d="$(srcdir)/"; fi; \
	    list2="$$list2 $$d$$p"; \
	    $(ECHO_Q) "  GEN   " `basename $$p .java`.class; \
	  done; \
	  $(ECHO_V) '$(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) '"$$list2"; \
	  $(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) $$list2; \
	else :; fi
	$(AM_V_at)echo timestamp > classes_tcap_java.stamp

ALL_LOCAL += classes_tcap_java.stamp

install-lib_org_openss7_tcap_jar_la_SOURCES: classes_tcap_java.stamp
	@$(NORMAL_INSTALL)
	test -z "$(javadir)" || $(MKDIR_P) "$(DESTDIR)$(javadir)"
	@test -n "$(lib_org_openss7_tcap_jar_la_SOURCES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " $(INSTALL_DATA)" "$$@" "'$(DESTDIR)$(javadir)/$$p'"; \
	$(INSTALL_DATA) "$$@" "$(DESTDIR)$(javadir)"

INSTALL_DATA_LOCAL += install-lib_org_openss7_tcap_jar_la_SOURCES

uninstall-lib_org_openss7_tcap_jar_la_SOURCES:
	@$(NORMAL_UNINSTALL)
	@test -n "$(lib_org_openss7_tcap_jar_la_SOURCES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " ( cd '$(DESTDIR)$(javadir)' && rm -f" "$$@" ")"; \
	cd "$(DESTDIR)$(javadir)" && rm -f "$$@"

UNINSTALL_LOCAL += uninstall-lib_org_openss7_tcap_jar_la_SOURCES

clean-lib_org_openss7_tcap_jar_la_SOURCES:
	-rm -f *.class classes_tcap_java.stamp

CLEAN_LOCAL += clean-lib_org_openss7_tcap_jar_la_SOURCES

CLEANFILES += classes_tcap_java.stamp

classes_xtcap_java.stamp: $(lib_org_openss7_xtcap_jar_la_SOURCES)
	@list1='$?'; list2=; if test -n "$$list1"; then \
	  for p in $$list1; do \
	    if test -f $$p; then d=; else d="$(srcdir)/"; fi; \
	    list2="$$list2 $$d$$p"; \
	    $(ECHO_Q) "  GEN   " `basename $$p .java`.class; \
	  done; \
	  $(ECHO_V) '$(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) '"$$list2"; \
	  $(CLASSPATH_ENV) $(JAVAC) -d $(JAVAROOT) $(AM_JAVACFLAGS) $(JAVACFLAGS) $$list2; \
	else :; fi
	$(AM_V_at)echo timestamp > classes_xtcap_java.stamp

ALL_LOCAL += classes_xtcap_java.stamp

install-lib_org_openss7_xtcap_jar_la_SOURCES: classes_xtcap_java.stamp
	@$(NORMAL_INSTALL)
	test -z "$(javadir)" || $(MKDIR_P) "$(DESTDIR)$(javadir)"
	@test -n "$(lib_org_openss7_xtcap_jar_la_SOURCES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " $(INSTALL_DATA)" "$$@" "'$(DESTDIR)$(javadir)/$$p'"; \
	$(INSTALL_DATA) "$$@" "$(DESTDIR)$(javadir)"

INSTALL_DATA_LOCAL += install-lib_org_openss7_xtcap_jar_la_SOURCES

uninstall-lib_org_openss7_xtcap_jar_la_SOURCES:
	@$(NORMAL_UNINSTALL)
	@test -n "$(lib_org_openss7_xtcap_jar_la_SOURCES)" && test -n "$(javadir)" || exit 0; \
	set x *.class; shift; test "$$1" != "*.class" || exit 0; \
	echo " ( cd '$(DESTDIR)$(javadir)' && rm -f" "$$@" ")"; \
	cd "$(DESTDIR)$(javadir)" && rm -f "$$@"

UNINSTALL_LOCAL += uninstall-lib_org_openss7_xtcap_jar_la_SOURCES

clean-lib_org_openss7_xtcap_jar_la_SOURCES:
	-rm -f *.class classes_xtcap_java.stamp

CLEAN_LOCAL += clean-lib_org_openss7_xtcap_jar_la_SOURCES

CLEANFILES += classes_xtcap_java.stamp



# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
