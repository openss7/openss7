/*****************************************************************************

 @(#) File: src/util/sdlautomon.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 *****************************************************************************/

static char const ident[] = "src/util/sdlautomon.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * The purpose of this utility is to perform auto-configuration of the
 * signalling links on one or more X400P cards.
 *
 */

/*
 * First order of business is brining up the cards in their default
 * configurations when they have not already been brought up by some other
 * action.  Allow them to settle for at least 10ms so that any blue alarm
 * conditions (receive carrier loss) is detected.
 */


/*
 * Second, determine which links do not have anything attached to them by
 * checking for blue alarms.  Any span with a persistent blue alarm is unusable.
 * Just because there is a blue alarm for default configuration of a span, does
 * not mean that there is nothing connected to the span: it may be attached on a
 * high-impedance monitoring port and the gain is simply insufficient.  So, for
 * all spans receiving blue alarms, attempt to reconfigure the span with
 * additional gain.  The possible settings are:
 *
 * mxSpanLineGain = MXSPANLINUEGAIN_NONE
 * mxSpanLineGain = MXSPANLINUEGAIN_MON0DB
 * mxSpanLineGain = MXSPANLINUEGAIN_MON12DB
 * mxSpanLineGain = MXSPANLINUEGAIN_MON20DB
 * mxSpanLineGain = MXSPANLINUEGAIN_MON26DB
 * mxSpanLineGain = MXSPANLINUEGAIN_MON30DB
 * mxSpanLineGain = MXSPANLINUEGAIN_MON32DB
 *
 * Note that not all gain settings are available for all cards; however, the
 * driver will select the next lowest gain amount and will, in any case, return
 * the setting that is made.  Each span reporting a blue alarm should be walked
 * up through the gain settings until the top is reached.  After each increment,
 * wait at least 10ms for the span to settle and properly report RCL.  Note that
 * it might not be sufficient to stop at a point where the blue alarms clear:
 * the signal might be sufficiently attenuated that the signal cannot get
 * through.
 */

