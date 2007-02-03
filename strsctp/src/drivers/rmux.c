/*****************************************************************************

 @(#) $RCSfile: rmux.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2007/02/03 03:05:22 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/02/03 03:05:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: rmux.c,v $
 Revision 1.1.2.1  2007/02/03 03:05:22  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: rmux.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2007/02/03 03:05:22 $"

static char const ident[] = "$RCSfile: rmux.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2007/02/03 03:05:22 $";

/*
 *  RMUX is a remote multiplexing driver using SCTP.  Its purpose is to make remote STREAMS devices
 *  appear to be local STREAMS devices, permitting distribution of protocol stacks over multiple
 *  hosts in a fashion transparent to applications programs.  The RMUX driver uses SCTP as its
 *  transport protocol and a slim protocol to achieve transparency to the STREAMS user interface.
 *
 *  RMUX uses a single SCTP association to the remote host to provide access to all of the STREAMS
 *  devices on that host.  RMUX is a multiplexing driver that links SCTP associations beneath it
 *  using a control Stream during the configuration process.  Device numbers on the upper mutiplex
 *  (both major and minor) are configured during the configuration process.
 *
 *  Open and Close
 *
 *  When a user opens one of these defined upper device numbers, RMUX communicates with the remote
 *  host whose daemon process opens the requested device, links it under the RMUX driver at the
 *  remote end and provides a return status to the user.  When the user closes the driver, RMUX
 *  communicates the close to the remote end.
 *
 *  I_LINK and I_PLINK
 *
 *  Local users can also link both remote and local Streams under a remote multiplexing driver.
 *  When linking a Stream, it is communicated to the remote RMUX during the linking process and the
 *  remote RMUX daemon opens a special minor device on its upper multiplex and links it under the
 *  remote driver.  When linking a remote Stream on the same host, the corresponding Stream on the
 *  remote RMUX lower multiplex is unlinked and linked under the appropriate driver.  When linking a
 *  remote Stream on a different host is the same a linking a local Stream.
 *
 *  I_PUSH and I_POP
 *
 *  Local users can push modules onto remote Streams; however, the module pushed is a local module.
 *
 *  STREAMS message handling.
 *
 *  All STREAMS messages are encapsulated and sent to the remote RMUX.  The multiple stream
 *  capability of SCTP is utilized fully.  When a remote Stream is established, an SID is assigned
 *  to each of queue band number for the Stream.  Normal priority messages (e.g, M_PROTO) are sent
 *  using ordered delivery on the SID that corresponds to the queue band.  High priority messages
 *  (e.g, M_PCPROTO) are sent using unordered (???) delivery on the SID that corresponds to high
 *  priority messages (likely SID 0).
 *
 *  Flow Control
 *
 *  Flow control along the Stream is accomplished using specialized RMUX messages that indicate the
 *  band for which flow control is effected.  The receiving RMUX driver places a specalized blocking
 *  message on the appropriate queue pair to establish backpressure.  Additional RMUX message are
 *  used for backenabling and also indicate the band for which the backenable has occurred
 *  (resulting in the removal of the blocking message from the associated queue band).  In this way,
 *  STREAMS flow control operates transparently.
 *
 *  A better approach to flow control would be to leave messages on the local sending queue until
 *  they are acknowledged received by the remote end (using the acknowledged data service of the NPI
 *  interface to SCTP).  This is cleaner in that it does not require blocking messages.  It does
 *  still require a back-enable message, but without band number.
 *
 *  Issues:
 *
 *  1. Module pushing is problematic in that there may be module mismatches between the local and
 *     remote system.  Where closer control is required a workaround is to use the autopush
 *     mechanism on the remote system instead (have modules autopushed on opened drivers instead
 *     of explicitly pushed by the local user).  Another workaround is to use a specialized set of
 *     R_PUSH, R_POP, input-output controls that mirror the I_PUSH, I_POP, input-output controls for
 *     STREAMS.  The problem with this later approach is that the user must be aware that an RMUX
 *     Stream is being used.  (Unless we provide a Stream head option to pass I_PUSH and I_POP
 *     input-output controls down to the driver.  This later approach is probably best as it also
 *     allows remote Streams on to be linked under multiplexing drivers on the same host.)
 *
 *     The remote system will have to unlink the Stream, push or pop the module and then
 *     relink it.
 *
 *  2. Numbering, flags and structure layout.  It is likely that the same STREAMS implementation is
 *     necessary on both the sending and receiving system.  In particular, the remote driver must
 *     understand 32-bit and 64-bit flags for input-output controls.  Where incompatibilities exist,
 *     the module or driver must refrain from using those facilities.
 *
 *  3. All users must properly handle M_ERROR and M_HANGUP messages.  If communication is lost to
 *     the remote RMUX, all corresponding local Streams will be hungup with M_HANGUP.
 *
 *  4. Modules and drivers cannot have knowledge of, nor rely upon, duplicated message blocks,
 *     special alignment or "hidden" data in a message block between modules.  (Not much of an issue
 *     as they shouldn't anyway.)
 *
 *  5. Local Streams representing remote devices will always have service procedures.
 *
 *  6. Modules and drivers cannot have knowledge of, nor rely upon, the context in which messages
 *     are passed to neighbouring modules and drivers.
 *
 *  7. Users cannot rely on the device numbering of opened Streams.
 *
 *  8. Messages passed to the Stream head that are absorbed by the Stream head (e.g., M_ERROR,
 *     M_SIG, M_SETOPTS) on open may be lost for the remote system.  Using knowledge about the Linux
 *     Fast-STREAMS implementation it is possible to determine whether M_HANGUP or M_SETOPTS as
 *     performed while linking.
 *
 *  Advantages:
 *
 *  RMUX is cleaner than SIGTRAN for use within distributed OpenSS7 protocol stack systems.  Instead
 *  of being specific to a protocol layer service primitive interface, RMUX is general purpose and
 *  can be use to export the User-Provider interface for _any_ protocol module.
 *
 *  Non-STREAMS based systems that have SCTP can open and manage STREAMS devices as long as they use
 *  the RMUX protocol directly.
 *
 *  By chaning devices between hosts, it is possible to provide redundant paths to the same device.
 *  This accomplished by opening the device multiple times via multiple systems.  RMUX can be
 *  configured to understand that multiple upper multiplex Streams are related in this fashion and
 *  can provide automatic failover from one path to another.  Because the acknowledged SCTP NPI
 *  service is used, each encapsulated message can be marked with an identifier and the ultimate
 *  receiving device can remove duplications.
 *
 *  RMUX Protocol:
 *
 *  The RMUX protocol communicates by passing protocol messages to peer RMUX drivers on SCTP stream
 *  number 0.
 *
 *  R_OPEN    - open a device
 *  R_CLOSE   - close a device
 *  R_PUSH    - push a module
 *  R_POP     - pop a module
 *  R_LINK    - link a device
 *  R_PLINK   - permanent link a device
 *  R_UNLINK  - unlink a device
 *  R_PUNLINK - permanent unlink a device
 *  R_ENABLE  - enable a queue
 *  R_NORMAL  - pass a normal priority message
 *  R_HIPRI   - pass a high priority message
 *
 *  R_OPEN_REQ/IND: - open a device
 *
 *	32-bit Stream Identifier (local device number)
 *	32-bit open flags
 *	structured credentials
 *	32-bit length of device name
 *	32-bit offset of device name
 *	followed by device name
 *
 *  R_OPEN_RES/CON: - open a device
 *
 *	32-bit Stream Identifier (local device number)
 *	32-bit return value (negative error)
 *
 *  R_CLOSE_REQ/IND: - close a device
 *
 *	32-bit Stream Identifier (local device number)
 *	structured credentials
 *
 *  R_CLOSE_RES/CON: - close a device
 *
 *	32-bit Stream Identifier (local device number)
 *	32-bit return value (negative error)
 *
 *  R_PUSH_REQ/IND: - push a module
 *
 *	32-bit Stream Identifier (local device number)
 *	structured credentials
 *	32-bit length of module name
 *	32-bit offset of module name
 *	followed by module name
 *
 *  R_PUSH_RES/CON: - push a module
 *
 *	32-bit Stream Identifier (local device number)
 *	32-bit return value (negative error)
 *
 */

/*
 * The connection manager responds with an R_OPEN_RES primitive in response to an R_OPEN_IND
 * primitive.  The primitive is passed directly in an R_OPEN_CON message to the remote user.
 */
static int
r_open_res(struct rmux *rm, queue_t *q, mblk_t *mp)
{
}


static int
r_send_open_con(struct rmux *rm, queue_t *q, mblk_t *mp)
{
}

/*
 * When an R_OPEN_REQ message is received for a Stream has not already been linked by the device, it
 * issues an R_OPEN_IND service primitive to the control Stream and awaits a R_OPEN_RES primitive.
 * If no control Stream currently exists, an error code [ENXIO] is returned to the remote end in an
 * R_OPEN_RES message.
 *
 * The connection manager on the control Stream, upon receipt of an R_OPEN_IND primitive should open
 * the appropriate Stream, and when successful, link it under the multiplexing driver, and return an
 * R_OPEN_RES primitive with the multiplex identifier to associate it with the Stream.  When an
 * error occurs during the open of the Stream, an R_OPEN_RES primitive with the error is returned.
 */
static int
r_recv_open_req(struct rmux *rm, queue_t *q, mblk_t *mp)
{
}

/*
 * When an R_REOPEN_REQ message is received for a Stream that has already been linked, the driver
 * simlulates a Stream head module stack open and returns the return value in an R_REOPEN_CON
 * message.
 */
static int
r_recv_reopen_req(struct rmux *rm, queue_t *q, mblk_t *mp)
{
}

/*
 * There are two types of upper Streams: control Streams and user Streams.  The control Stream is a
 * clone minor device that can only be opened once.  User Streams are either clone or non-clone
 * Streams (depending on the response from the remote end).  User Streams must be configured by the
 * control Stream before they can be successfully opened.
 *
 * When configured, the minor device is assigned a lower mux SCTP transport Stream, device
 * identifier that identifies the device to the remote system, and a logical path on the remote
 * system.  The logical path might correspond to the actual filesystem path on the remote system to
 * use to open the device, or might be a logical reference.
 *
 * For a configured minor device, the r_qopen() procedure passes an R_OPEN_REQ message on the SCTP
 * stream to the remote system and blocks awaiting a response, timeout or loss of communications.
 * The R_OPEN_REQ message contains the 32-bit local device instance identifier (derived from the q
 * pointer), the logical path name (representing the device opened), the oflags, and the
 * credientials of the opening user.
 *
 * If STREAMS messages were passed to the Stream head as part of the remote open procedure, a
 * R_NORMAL_IND or R_HIPRI_IND message might arrive before the R_OPEN_CON, in which case the
 * deencapsulation of those messages are passed to the local Stream head (likely M_SETOPTS).
 *
 * If a timeout or loss of communications on the SCTP Stream occurs before the open completes, an
 * appropriate error code is returned and the open call fails.
 *
 * A response R_OPEN_CON message will contain the device instance identifier (as was provided in the
 * R_OPEN_REQ message), applied STREAMS flags (sflag) indicating DRVOPEN or CLONEOPEN, and a return
 * value indicating either success or an error.  When an error occurs, the associated error return
 * code is returned and no other action is taken.  When successful, the returned sflag is examined
 * to determine whether the remote end assigned a unique minor or not.  When sflag is CLONEOPEN, the
 * open will complete as though CLONEOPEN was specified to this call; when DRVOPEN, as though
 * DRVOPEN was specified on the r_qopen() call.
 *
 * When a user Stream is already opened for the first time and a subsequent open call occurs,
 * Another R_OPEN_REQ message is passed to the remote end and the calling process is blocked
 * awaiting the R_OPEN_CON.  The return value from the R_OPEN_CON is again returned.
 */
static streamscall int
r_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
}

/*
 * There are two types of upper streams: control Streams and user Streams.  Control Streams are
 * closed as normal.  User Streams communicate the device arguments (device number and credentials)
 * in a R_CLOSE_REQ message to the remote end and blocks the user awaiting a response (or timeout or
 * communications failure).
 */
static streamscall int
r_qclose(queue_t *q, dev_t *devp, cred_t *crp)
{
}
