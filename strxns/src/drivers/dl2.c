/*****************************************************************************

 @(#) $RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:10:43 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/12/20 23:10:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl2.c,v $
 Revision 0.9.2.1  2006/12/20 23:10:43  brian
 - added new mux drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:10:43 $"

static char const ident[] = "$RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:10:43 $";

/*
 *  This is a multiplexing driver use to support linking DL streams.  When DL streams are linked
 *  under the driver and configured using input-output controls, the DL stream appears as a network
 *  device to Linux.  DL stream can also be opened on the upper multiplex and attached to Linux
 *  network devices.  This permits DL drivers to be exported to Linux and DL users to also use Linux
 *  devices not directly supported with a STREAMS driver.
 *
 *  To manage mapping of upper multiplex DL streams to Linux devices, a specialized intput-output
 *  control and DL primitive are used.  These take as an argument the ascii name of the device and
 *  return a PPA (minor device number) that can be used to open the device directly or can be used
 *  to attach a style 2 DL stream to the device.  To manage mapping of lower multiplex DL streams to
 *  LInux deices, a specialized input-output control is used.  This input-output control takes as an
 *  argument the multiplex id and the ascii device name.  Devices mapped in this way begin life as a
 *  network device of the type revealed by interrogating the DL stream but can be configured using
 *  the normal Linux facilities.
 */
