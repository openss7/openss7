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

...if ((sf = addr2ref(PLocalHostName())) == NULL) {
	sf = &sfs;
	(void) bzero((char *) sf, sizeof *sf);
}

/* read command line arguments here... */

if (AcAssocRequest(ctx, NULLAEI, NULLAEI, NULLPA, pa, pc, NULLOID,
		   0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, NULLQOS, acc, aci)
    == NOTOK)
	error("A-ASSOCIATE.REQUEST: %s", AcErrString(aca->aca_reason));

if (acc->acc_result != ACS_ACCEPT)
	error("association rejected: %s", AcErrString(aca->aca_reason));

sd = acc->acc_sd;
ACCFREE(acc);

if (RoSetService(sd, RoPService, &rois) == NOTOK)
	error("RoSetService: %s", RoErrString(rop->rop_reason));

invoke(sd);			/* invoke the operations, etc. */

if (AcRelRequest(sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
	error("A-RELEASE.REQUEST: %s", AcErrString(aca->aca_reason));

if (!acr->acr_affirmative) {
	(void) AcUAbortRequest(sd, NULLPEP, 0, aci);
	error("release rejected by peer: %d", acr->acr_reason);
}

ACRFREE(acr);

exit(0);
}
