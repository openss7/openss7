/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

...
#define	myservice	"servicestore"
#define	mycontext	"local service"
#define	mypci		"local service pci"
...int sd;
struct SSAPref sfs;
register struct SSAPref *sf;
register struct PSAPaddr *pa;
struct AcSAPconnect accs;
register struct AcSAPconnect *acc = &accs;
struct AcSAPindication acis;
register struct AcSAPindication *aci = &acis;
register struct AcSAPabort *aca = &aci->aci_abort;
AEI aei;
OID ctx, pci;
struct PSAPctxlist pcs;
register struct PSAPctxlist *pc = &pcs;
struct RoSAPindication rois;
register struct RoSAPindication *roi = &rois;
register struct RoSAPpreject *rop = &roi->roi_preject;

if ((aei = _str2aei(argv[1], myservice, mycontext, isatty(fileno(stdin)), NULLCP, NULLCP))
    == NULLAEI)
	adios(NULLCP, "unable to resolve service: %s", PY_pepy);
if ((pa = aei2addr(aei)) == NULLPA)
	adios(NULLCP, "address translation failed");

if ((ctx = ode2oid(mycontext)) == NULLOID)
	adios(NULLCP, "%s: unknown object descriptor", mycontext);
if ((ctx = oid_cpy(ctx)) == NULLOID)
	adios(NULLCP, "out of memory");
if ((pci = ode2oid(mypci)) == NULLOID)
	adios(NULLCP, "%s: unknown object descriptor", mypci);
if ((pci = oid_cpy(pci)) == NULLOID)
	adios(NULLCP, "out of memory");
pc->pc_nctx = 1;
pc->pc_ctx[0].pc_id = 1;
pc->pc_ctx[0].pc_asn = pci;
pc->pc_ctx[0].pc_atn = NULLOID;

if ((sf = addr2ref(PLocalHostName())) == NULL) {
	sf = &sfs;
	(void) bzero((char *) sf, sizeof *sf);
}

if (AcAssocRequest(ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
		   0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, NULLQOS, acc, aci)
    == NOTOK)
	acs_adios(aca, "A-ASSOCIATE.REQUEST");

if (acc->acc_result != ACS_ACCEPT)
	adios(NULLCP, "association rejected: [%s]", AcErrString(acc->acc_result));

sd = acc->acc_sd;
ACCFREE(acc);

if (RoSetService(sd, RoPService, roi) == NOTOK)
	ros_adios(rop, "set RO/PS fails");

...
