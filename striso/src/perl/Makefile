# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
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
# Last Modified $Date$ by $Author$
#
# -----------------------------------------------------------------------------
#
# $Log$
# =============================================================================

GDMOS = \
	G.774.1.gdmo \
	G.774.2.gdmo \
	G.774.3.gdmo \
	G.774.4.gdmo \
	G.774.5.gdmo \
	G.774.gdmo \
	ISO10589.gdmo \
	M.3100.gdmo \
	Q.751.1.gdmo \
	Q.751.2.gdmo \
	Q.821.gdmo \
	Q.822.gdmo \
	Q.826.gdmo \
	T1.116.1D.gdmo \
	T1.116.1.gdmo \
	X.283.gdmo \
	X.521.gdmo \
	X.721.gdmo \
	X.723.gdmo \
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
	EN300292.gdmo \
	ES200653.gdmo \
	ES201097-1.gdmo

NEW_GDMOS = $(GDMOS:%.gdmo=%.new.gdmo)
HEADERS = $(GDMOS:%.gdmo=%.hh)
STUBFILES = $(GDMOS:%.gdmo=%.stub.cpp)

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

all: $(NEW_GDMOS)

# $(HEADERS) $(STUBFILES)

$(NEW_GDMOS) : %.new.gdmo : %.gdmo
	./gdmocheck $< > $@

$(HEADERS) : %.hh : %.gdmo
	./gdmohead $< > $@

$(STUBFILES) : %.stub.cpp : %.gdmo
	./gdmostub $< > $@

$(NEW_GDMOS) : Makefile gdmocheck

$(HEADERS) : Makefile gdmocheck

$(STUBFILES) : Makefile gdmocheck

clean:
	rm -fv $(NEW_GDMOS) $(HEADERS) $(STUBFILES)
