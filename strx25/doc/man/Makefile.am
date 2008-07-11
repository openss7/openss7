# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2008-07-11 09:46:27 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
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
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2008-07-11 09:46:27 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = 

man_MANS = \
	man3/libsx25.3 \
	man3/SX25.3 \
	man3/sx25.3 \
	man3/endpadent.3 \
	man3/endxhostent.3 \
	man3/equalx25.3 \
	man3/getnettype.3 \
	man3/getpadbyaddr.3 \
	man3/getpadent.3 \
	man3/getxhostbyaddr.3 \
	man3/getxhostbyname.3 \
	man3/getxhostent.3 \
	man3/libx25.3 \
	man3/linkidtox25.3 \
	man3/padtos.3 \
	man3/setpadent.3 \
	man3/setxhostent.3 \
	man3/snidtox25.3 \
	man3/stox25.3 \
	man3/x25diags.3 \
	man3/x25_find_link_parameters.3 \
	man3/x25_read_config_parameters.3 \
	man3/x25_read_config_parameters_file.3 \
	man3/x25_save_link_parameters.3 \
	man3/x25_set_parse_error_function.3 \
	man3/x25tolinkid.3 \
	man3/x25tos.3 \
	man3/x25tosnid.3 \
	man3/x25_write_config_parameters.3 \
	man3/x25_write_config_parameters_file.3 \
	man3/xti_x25.3 \
	man3/XX25.3 \
	man4/cd-llc.4 \
	man4/cdpmod.4 \
	man4/cons.4 \
	man4/dlaix.4 \
	man4/dl-llc.4 \
	man4/hdlc.4 \
	man4/ifr.4 \
	man4/ipx25.4 \
	man4/ixe.4 \
	man4/lapb.4 \
	man4/llc2.4 \
	man4/lli.4 \
	man4/mfe.4 \
	man4/mlp.4 \
	man4/nli3.4 \
	man4/npi.4 \
	man4/plp.4 \
	man4/s_cons.4 \
	man4/s_lli.4 \
	man4/s_nli3.4 \
	man4/s_npi.4 \
	man4/s_wan.4 \
	man4/s_cdi.4 \
	man4/s_dlpi.4 \
	man4/s_xx25.4 \
	man4/wan.4 \
	man4/X25.4 \
	man4/x25-lapb.4 \
	man4/x25_lapb.4 \
	man4/x25-llc2.4 \
	man4/x25_llc2.4 \
	man4/x25-llc.4 \
	man4/x25-plp.4 \
	man4/x25_plp.4 \
	man4/xos.4 \
	man4/xot.4 \
	man4/xx25.4 \
	man5/extraformat.5 \
	man5/ifrdlciconf.5 \
	man5/ifrtemplate.5 \
	man5/lapbtemplate.5 \
	man5/llc2template.5 \
	man5/lsapformat.5 \
	man5/mfeprotoconf.5 \
	man5/mfetemplate.5 \
	man5/padent.5 \
	man5/qosformat.5 \
	man5/strx25.5 \
	man5/wanmapconf.5 \
	man5/wantemplate.5 \
	man5/x25_primitives.5 \
	man5/x25conf.5 \
	man5/x25template.5 \
	man5/xaddrf.5 \
	man5/xhostent.5 \
	man7/N_Abort.7 \
	man7/N_CC.7 \
	man7/N_CI.7 \
	man7/N_DAck.7 \
	man7/N_Data.7 \
	man7/N_DC.7 \
	man7/N_DI.7 \
	man7/N_EAck.7 \
	man7/N_EData.7 \
	man7/N_getlinkstats.7 \
	man7/N_getoneVCstats.7 \
	man7/N_getpvcmap.7 \
	man7/N_getstats.7 \
	man7/N_getVCstats.7 \
	man7/N_getVCstatus.7 \
	man7/NLI.7 \
	man7/nli.7 \
	man7/WAN.7 \
	man7/wan.7 \
	man7/N_linkconfig.7 \
	man7/N_linkent.7 \
	man7/N_linkmode.7 \
	man7/N_linkread.7 \
	man7/N_nuidel.7 \
	man7/N_nuiget.7 \
	man7/N_nuimget.7 \
	man7/N_nuiput.7 \
	man7/N_nuireset.7 \
	man7/N_putpvcmap.7 \
	man7/N_PVC_ATTACH.7 \
	man7/N_PVC_DETACH.7 \
	man7/N_RC.7 \
	man7/N_RI.7 \
	man7/N_traceoff.7 \
	man7/N_traceon.7 \
	man7/N_X25_ADD_ROUTE.7 \
	man7/N_X25_FLUSH_ROUTE.7 \
	man7/N_X25_GET_NEXT_ROUTE.7 \
	man7/N_X25_GET_ROUTE.7 \
	man7/N_X25_RM_ROUTE.7 \
	man7/N_Xcanlis.7 \
	man7/N_Xelisten.7 \
	man7/N_Xlisten.7 \
	man7/N_zerostats.7 \
	man7/npi_x25.7 \
	man8/ifrdlci.8 \
	man8/ifrstat.8 \
	man8/ifrtune.8 \
	man8/ixemap.8 \
	man8/ixetune.8 \
	man8/linkadd.8 \
	man8/linkdel.8 \
	man8/linklist.8 \
	man8/linkreset.8 \
	man8/linkstart.8 \
	man8/linkstate.8 \
	man8/linkstop.8 \
	man8/lltune.8 \
	man8/mfeproto.8 \
	man8/mfevc.8 \
	man8/mlptune.8 \
	man8/nuimap.8 \
	man8/pvcmap.8 \
	man8/strx25_mknod.8 \
	man8/vcstat.8 \
	man8/wanmap.8 \
	man8/wantune.8 \
	man8/x25diags.8 \
	man8/x25file.8 \
	man8/x25info.8 \
	man8/x25netd.8 \
	man8/x25route.8 \
	man8/x25stat.8 \
	man8/x25trace.8 \
	man8/x25tune.8 \
	man8/x32map.8

dist_noinst_MANS = \
	man3/libsx25.3.man \
	man3/SX25.3.man \
	man3/sx25.3.man \
	man3/endpadent.3.man \
	man3/endxhostent.3.man \
	man3/equalx25.3.man \
	man3/getnettype.3.man \
	man3/getpadbyaddr.3.man \
	man3/getpadent.3.man \
	man3/getxhostbyaddr.3.man \
	man3/getxhostbyname.3.man \
	man3/getxhostent.3.man \
	man3/libx25.3.man \
	man3/linkidtox25.3.man \
	man3/padtos.3.man \
	man3/setpadent.3.man \
	man3/setxhostent.3.man \
	man3/snidtox25.3.man \
	man3/stox25.3.man \
	man3/x25diags.3.man \
	man3/x25_find_link_parameters.3.man \
	man3/x25_read_config_parameters.3.man \
	man3/x25_read_config_parameters_file.3.man \
	man3/x25_save_link_parameters.3.man \
	man3/x25_set_parse_error_function.3.man \
	man3/x25tolinkid.3.man \
	man3/x25tos.3.man \
	man3/x25tosnid.3.man \
	man3/x25_write_config_parameters.3.man \
	man3/x25_write_config_parameters_file.3.man \
	man3/xti_x25.3.man \
	man3/XX25.3.man \
	man4/cd-llc.4.man \
	man4/cdpmod.4.man \
	man4/cons.4.man \
	man4/dlaix.4.man \
	man4/dl-llc.4.man \
	man4/hdlc.4.man \
	man4/ifr.4.man \
	man4/ipx25.4.man \
	man4/ixe.4.man \
	man4/lapb.4.man \
	man4/llc2.4.man \
	man4/lli.4.man \
	man4/mfe.4.man \
	man4/mlp.4.man \
	man4/nli3.4.man \
	man4/npi.4.man \
	man4/plp.4.man \
	man4/s_cons.4.man \
	man4/s_lli.4.man \
	man4/s_nli3.4.man \
	man4/s_npi.4.man \
	man4/s_wan.4.man \
	man4/s_cdi.4.man \
	man4/s_dlpi.4.man \
	man4/s_xx25.4.man \
	man4/wan.4.man \
	man4/X25.4.man \
	man4/x25-lapb.4.man \
	man4/x25_lapb.4.man \
	man4/x25-llc2.4.man \
	man4/x25_llc2.4.man \
	man4/x25-llc.4.man \
	man4/x25-plp.4.man \
	man4/x25_plp.4.man \
	man4/xos.4.man \
	man4/xot.4.man \
	man4/xx25.4.man \
	man5/extraformat.5.man \
	man5/ifrdlciconf.5.man \
	man5/ifrtemplate.5.man \
	man5/lapbtemplate.5.man \
	man5/llc2template.5.man \
	man5/lsapformat.5.man \
	man5/mfeprotoconf.5.man \
	man5/mfetemplate.5.man \
	man5/padent.5.man \
	man5/qosformat.5.man \
	man5/strx25.5.man \
	man5/wanmapconf.5.man \
	man5/wantemplate.5.man \
	man5/x25_primitives.5.man \
	man5/x25conf.5.man \
	man5/x25template.5.man \
	man5/xaddrf.5.man \
	man5/xhostent.5.man \
	man7/N_Abort.7.man \
	man7/N_CC.7.man \
	man7/N_CI.7.man \
	man7/N_DAck.7.man \
	man7/N_Data.7.man \
	man7/N_DC.7.man \
	man7/N_DI.7.man \
	man7/N_EAck.7.man \
	man7/N_EData.7.man \
	man7/N_getlinkstats.7.man \
	man7/N_getoneVCstats.7.man \
	man7/N_getpvcmap.7.man \
	man7/N_getstats.7.man \
	man7/N_getVCstats.7.man \
	man7/N_getVCstatus.7.man \
	man7/NLI.7.man \
	man7/nli.7.man \
	man7/WAN.7.man \
	man7/wan.7.man \
	man7/N_linkconfig.7.man \
	man7/N_linkent.7.man \
	man7/N_linkmode.7.man \
	man7/N_linkread.7.man \
	man7/N_nuidel.7.man \
	man7/N_nuiget.7.man \
	man7/N_nuimget.7.man \
	man7/N_nuiput.7.man \
	man7/N_nuireset.7.man \
	man7/N_putpvcmap.7.man \
	man7/N_PVC_ATTACH.7.man \
	man7/N_PVC_DETACH.7.man \
	man7/N_RC.7.man \
	man7/N_RI.7.man \
	man7/N_traceoff.7.man \
	man7/N_traceon.7.man \
	man7/N_X25_ADD_ROUTE.7.man \
	man7/N_X25_FLUSH_ROUTE.7.man \
	man7/N_X25_GET_NEXT_ROUTE.7.man \
	man7/N_X25_GET_ROUTE.7.man \
	man7/N_X25_RM_ROUTE.7.man \
	man7/N_Xcanlis.7.man \
	man7/N_Xelisten.7.man \
	man7/N_Xlisten.7.man \
	man7/N_zerostats.7.man \
	man7/npi_x25.7.man \
	man8/ifrdlci.8.man \
	man8/ifrstat.8.man \
	man8/ifrtune.8.man \
	man8/ixemap.8.man \
	man8/ixetune.8.man \
	man8/linkadd.8.man \
	man8/linkdel.8.man \
	man8/linklist.8.man \
	man8/linkreset.8.man \
	man8/linkstart.8.man \
	man8/linkstate.8.man \
	man8/linkstop.8.man \
	man8/lltune.8.man \
	man8/mfeproto.8.man \
	man8/mfevc.8.man \
	man8/mlptune.8.man \
	man8/nuimap.8.man \
	man8/pvcmap.8.man \
	man8/strx25_mknod.8.man \
	man8/vcstat.8.man \
	man8/wanmap.8.man \
	man8/wantune.8.man \
	man8/x25diags.8.man \
	man8/x25file.8.man \
	man8/x25info.8.man \
	man8/x25netd.8.man \
	man8/x25route.8.man \
	man8/x25stat.8.man \
	man8/x25trace.8.man \
	man8/x25tune.8.man \
	man8/x32map.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.7  2008-07-11 09:46:27  brian
# - roughed in implementation, updated documentation
#
# Revision 0.9.2.6  2008-07-01 12:31:04  brian
# - updated man pages, drafts, specs, header files
#
# Revision 0.9.2.5  2008-06-18 16:45:23  brian
# - widespread updates
#
# Revision 0.9.2.4  2008-05-07 16:01:37  brian
# - added NLI X.25-PLP CONS and XX25 implemetnation'
# doc/man/man3/XX25.3.man
# doc/man/man3/xti_x25.3.man
# doc/man/man4/X25.4.man
# doc/man/man5/strx25.5.man
# doc/man/man7/dlpi_lapb.7.man
# doc/man/man7/dlpi_llc2.7.man
# doc/man/man7/dlpi_x25.7.man
# src/drivers/npi.c
# src/include/npi_x25.h
# src/include/netx25/nli.h
# src/include/sys/npi_x25.h
# src/modules/dcc.h
# src/modules/npi.c
# src/modules/xx25.c
#
#
# cvsfiles=
#
# Revision 0.9.2.3  2008-05-05 15:35:02  brian
# - be strict with MORE_data and DATA_flag
#
# Revision 0.9.2.2  2008-05-03 11:46:47  brian
# - added manual pages
#
# Revision 0.9.2.1  2008-05-03 10:46:36  brian
# - added package files
#
# =============================================================================
