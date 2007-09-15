# =============================================================================
# 
# @(#) $RCSfile: Makefile,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/09/09 22:58:48 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/09/09 22:58:48 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile,v $
# Revision 0.9.2.2  2007/09/09 22:58:48  brian
# - updates for namespace
#
# Revision 0.9.2.1  2007/09/06 10:58:59  brian
# - added new gdmo files
#
# =============================================================================

GDMOS = \
	EN300292.gdmo \
	ES200653.gdmo \
	ES201097-1.gdmo \
	G.774.03.gdmo \
	G.774.04.gdmo \
	G.774.1.gdmo \
	G.774.2.gdmo \
	G.774.3.gdmo \
	G.774.4.gdmo \
	G.774.5.gdmo \
	G.774.gdmo \
	G.855.1.gdmo \
	ISO10589.1.gdmo \
	ISO10589.2.gdmo \
	ISO10589.gdmo \
	ISO10733.gdmo \
	ISO10733D.gdmo \
	M.3100.2005.gdmo \
	M.3100.gdmo \
	M3100.gdmo \
	M.3641.1994.gdmo \
	M.3641.gdmo \
	MTPacct.gdmo \
	Q.751.1.gdmo \
	Q.751.2.gdmo \
	Q.821.gdmo \
	Q.822.gdmo \
	Q.824.0.gdmo \
	Q.826.gdmo \
	Q.836.1.gdmo \
	Q.2751.1.gdmo \
	T1.116.1D.gdmo \
	T1.116.1.gdmo \
	X.162.gdmo \
	X.281.gdmo \
	X.282.gdmo \
	X.283.gdmo \
	X.284.gdmo \
	X.287.gdmo \
	X.521.gdmo \
	X.721.gdmo \
	X.723.gdmo \
	X.725.gdmo \
	X.737.gdmo \
	X.738.gdmo \
	X.739.gdmo \
	X.740.gdmo \
	X.741.gdmo \
	X.742.gdmo \
	X.743.gdmo \
	X.744.gdmo \
	X.745.gdmo \
	X.746.gdmo \
	X.748.gdmo \
	X.749.gdmo \
	X.750.gdmo \
	X.751.gdmo \
	X.753.gdmo \
	X.754.gdmo \
	X.790.gdmo

EXTRA_GDMOS = \
	X.283.1993.gdmo \
	X.283.1997.gdmo \
	X.737.1992.gdmo \
	X.737.1998.gdmo \
	X.737.2001.gdmo \
	X.738.1993.gdmo \
	X.738.1998.gdmo \
	X.738.2000.gdmo \
	X.739.1993.gdmo \
	X.739.1998.gdmo \
	X.740.1992.gdmo \
	X.740.1995.gdmo \
	X.740.1996.gdmo \
	X.740.1998.gdmo \
	X.741.1995.gdmo \
	X.741.1998.gdmo \
	X.741.2000.gdmo \
	X.742.1995.gdmo \
	X.742.1998.gdmo \
	X.742.2000.gdmo \
	X.742.old.gdmo \
	X.743.1998.gdmo \
	X.743.2001.gdmo \
	X.744.1996.gdmo \
	X.744.1998.gdmo \
	X.745.1993.gdmo \
	X.745.1997.gdmo \
	X.745.1998.gdmo \
	X.745.2000.gdmo \
	X.745.old.gdmo \
	SCCP.gdmo \
	MTPacct.gdmo \
	MTP.gdmo \
	M.3100.old.gdmo \
	M3100.gdmo \
	M.3100.2005.gdmo \
	ISO10733D.gdmo \
	ISO10733.gdmo \
	ISO10589.1.gdmo \
	ISO10589.2.gdmo \
	G.774.03.gdmo \
	G.774.04.gdmo

NEW_GDMOS = $(GDMOS:%.gdmo=%.new.gdmo)

ASNS = \
	EN300292.asn \
	ES200653.asn \
	ES201097-1.asn \
	G.774.03.asn \
	G.774.04.asn \
	G.774.1.asn \
	G.774.2.asn \
	G.774.3.asn \
	G.774.5.asn \
	G.774.asn \
	G.855.1.asn \
	ISO10589.1.asn \
	ISO10589.2.asn \
	ISO10589.asn \
	ISO10733.asn \
	ISO10733D.asn \
	M.3100.2005.asn \
	M.3100.asn \
	M3100.asn \
	M.3641.1994.asn \
	M.3641.asn \
	MTPacct.asn \
	Q.751.1.asn \
	Q.751.2.asn \
	Q.821.asn \
	Q.822.asn \
	Q.824.0.asn \
	Q.826.asn \
	Q.836.1.asn \
	SCCP.asn \
	T1.116.1.asn \
	X.162.asn \
	X.281.asn \
	X.282.asn \
	X.283.asn \
	X.284.asn \
	X.721.asn \
	X.723.asn \
	X.725.asn \
	X.737.asn \
	X.738.asn \
	X.739.asn \
	X.740.asn \
	X.741.asn \
	X.742.asn \
	X.743.asn \
	X.744.asn \
	X.745.asn \
	X.746.asn \
	X.748.asn \
	X.749.asn \
	X.751.asn \
	X.753.asn \
	X.754.asn \
	X.790.asn \
	X.711.asn \
	X.750.asn

EXTRA_ASNS = \
	ASN1.asn \
	Q.824.1.asn \
	MTP.asn \
	X.283.1993.asn \
	X.283.1997.asn \
	X.725.1995.asn \
	X.737.1992.asn \
	X.737.1998.asn \
	X.737.2000.asn \
	X.737.2001.asn \
	X.738.1993.asn \
	X.738.1998.asn \
	X.738.2000.asn \
	X.739.1993.asn \
	X.739.1998.asn \
	X.740.1992.asn \
	X.740.1995.asn \
	X.740.1996.asn \
	X.740.1998.asn \
	X.741.1995.asn \
	X.741.1998.asn \
	X.741.2000.asn \
	X.742.1995.asn \
	X.742.1998.asn \
	X.742.2000.asn \
	X.742.old.asn \
	X.743.1998.asn \
	X.743.2001.asn \
	X.744.1996.asn \
	X.744.1998.asn \
	X.745.1993.asn \
	X.745.1997.asn \
	X.745.1998.asn \
	X.745.2000.asn \
	X.745.old.asn \
	X.746.2000.asn \
	X.746.old.asn \
	X.749.1997.asn \
	X.748.1999.asn \
	X.750.1996.asn \
	X.750.1997.asn \
	X.750.2000.asn \
	X.751.1995.asn \
	X.751.1998.asn \
	X.751.2000.asn \
	X.753.1997.asn \
	X.754.2000.asn

NEW_ASNS = $(ASNS:%.asn=%.new.asn)

HEADERS = $(GDMOS:%.gdmo=%.hh)
STUBFILES = $(GDMOS:%.gdmo=%.stub.cpp)

all: $(NEW_GDMOS)

# $(HEADERS) $(STUBFILES)

asns: $(NEW_ASNS)

gdmos: $(NEW_GDMOS)

headers: $(HEADERS)

stubs: $(STUBFILES)

diffs: $(NEW_GDMOS)
	@list='$(NEW_GDMOS)'; for f in $$list; do \
		g=`echo $$f | sed -e 's,\.new,,'` ; \
		vimdiff $$f $$g ; \
	done

$(NEW_GDMOS) : %.new.gdmo : %.gdmo
	./gdmocheck $< > $@

$(NEW_ASNS) : %.new.asn : %.asn
	./gdmocheck $< > $@

$(HEADERS) : %.hh : %.gdmo
	./gdmohead $< > $@

$(STUBFILES) : %.stub.cpp : %.gdmo
	./gdmostub $< > $@

$(NEW_ASNS) : gdmocheck

$(NEW_GDMOS) : gdmocheck

$(HEADERS) : gdmocheck

$(STUBFILES) : gdmocheck

clean:
	rm -fv $(NEW_ASNS) $(NEW_GDMOS) $(HEADERS) $(STUBFILES)

clean-asns:
	rm -fv $(NEW_ASNS)

clean-gdmos:
	rm -fv $(NEW_GDMOS)

clean-headers:
	rm -fv $(HEADERS)

clean-stubs:
	rm -fv $(STUBFILES)
