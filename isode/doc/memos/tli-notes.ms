.TL
Notes on TLI integration into ISODE
.AU
Julian Onions
.AI
X-Tel Services Ltd
Nottingham University
Nottingham.
.AU
Alan Young
.AI
Concurrent Computer Corp
227 Bath Road
SLOUGH  SL1 4AX
.NH
Introduction
.LP
The following notes document the changes made to ISODE to add the TLI
interface. This interface was done in two stages.
.IP 1.
The first stage was to add a TP0 layer running over RFC-1006 over
TLI/TCP interface. This interface was a 'proof of concept' exercise to
prove that ISODE really will run over TLI and that the Sun TLI
interface worked.
.IP 2.
The second stage was to add a TLI interface at the transport level.
This has been tested on a Beta release of Concurrent's RTnet-OSI R02
running on RTU 5.0 and an early release (Alpha 3) of RTU 6.0.
.NH
Implementation of TP0/RFC-1006/TLI/TCP stack.
.LP
This stack is enabled by having the following C preprocessor macros
defined in config.h
.DS
#define TCP /* we have TCP */
#define TLI_TCP /* provided by TLI interface */
#define TLI_POLL /* if you want poll(2) instead of select(2) */
.DE
The following files were changed and modified for the
TP0/RFC-1006/TLI/TCP stack.
.IP h/internet.h
Some minimal changes were made to include another TCP interface.
Nothing particularly significant was added.
.IP compat/internet_tli.c
This file is new for this interface. It contains all the interface
code between TP0 and the TLI interface assuming RFC-1006 layering.
.IP compat/select.c
A new xselect interface was written that uses poll(2) rather than
select. This assumes that poll(2) will work on all configured
interfaces as well as any other streams that may need to be selected
on (such as standard input/output in some applications).
Use of this interface instead of select(2) is chosen by the TLI_POLL
#define.
.IP tsap/tp0ts.c
One minor change here was necessary, to disable the use of the
writev(2) in this interface. ISODE makes use of writev(2) where
possible for greater efficiency, but the TLI interface does not permit
this style of data transfer.
.LP
This completed the implementation. ISODE  compiles with this interface
and works satisfactorily with such applications as imisc and ftam.
It has been tested on SunOS 4.1 and an early release (Alpha 3) of
Concurrent's RTU 6.0.
.NH
Implementation of the direct TLI interface.
.LP
More substantial changes were required for this interface. This stack
is enabled by the following C preprocessor macros.
.DS
#define TP4
#define TLI_TP
#define TLI_POLL /* if you want poll(2) instead of select(2) */
.DE
The TP4 macro seems to indicate that this is a TP4 stack, but in
actual fact it is a generic TP stack. ISODE distinguishes between TP0
for which it expects a network interface and TP4 for which it assumes
a transport interface. For these reasons it is easier to make it
believe there is a TP4 interface, allowing direct access to the
transport.
.LP
The following files were modified for this interface.
.IP h/tp4.h
Additions were made in this file to add the new stack in. The changes
are fairly minimal, and not very significant.
.IP h/tpkt.h
An additional key was added here to indicate a saved TLI structure.
.IP compat/select.c
The same code as in the previous example was used here. This uses
poll(2) instead of select(2) and the same constraints apply.
Again, which interface to use is chose by the TLI_POLL #define.
.IP tsap/ts2tli.c
This is where the major work of the code is. It contains the primary
interface routines.
.IP tsap/tsaplisten.c
Code was added here to allow TLI listeners to be started. This should
support both static and dynamic listeners, though the exact TLI
infra-structure has an impact.
.IP tsap/tsaprespond.c
Code was added here to allow recovery of state from a TLI interface.
The changes are minimal and essentially trivial.
.LP
This concludes the summary of changes. The missing parts of the
implementation and the more important
design decisions are now discussed.
.NH
Unfinished code and possible problems.
.LP
Firstly, the address translation routines are specific to the provider.
The current versions work for Concurrent's RTnet-OSI R02 and it is known
that the same format is used by some other providers.  It is also known that
there are providers that use althernate formats.
The two functions that need to be customised are
.DS
tp42gen()
gen2tp4()
.DE
The first of these takes a generic ISODE transport address structure
and converts it to a specific interface format. The
routine is passed an ISODE transport address pointer and a TLI netbuf
structure to be filled in.
.LP
The second routine does the inverse function and takes the same
arguments.
.LP
Options negotiation, such as use of expedited data, is also provider
specific.
.LP
The particular TLI device is defined as a C preprocessor macro. It is
currently defined as
.DS
#define TPTLI_DEV "/dev/ositpi"
.DE
which may be wrong. It can be overridden in the config.h file.
As devices are unlikely to vary for an ISODE stack built on a
particular platform, this should be sufficient.
.LP
The most difficult part of the port (and the part which other people
have found hard) is the listener. The TLI interface and ISODE match
reasonably well here, but there is a mismatch in descriptors.
ISODE expects the BSD sockets model. Here when a new connection indication
arrives you get a new file descriptor and all subsequent activity is
done using this.
.LP
In TLI you get the option of a new file descriptor, but only when you
accept the call. If user-data is to be sent out then the accept
must be delayed. As ISODE will get confused if there is one file
descriptor being used for two purposes (listening for new connections on
and partially accepting new connections) it seems safest to dup(2) the
file descriptor at this point and use it for the new descriptor.
According to the TLI and TPI specs this should be safe.
Later on when the connection is fully
accepted, the old file descriptor is closed and the new one
obtained from t_accept is dup()ed into its place. This keeps ISODE
happy.
.LP
The bigest problem arises when a fork()ed (and possibly exec()ed)
process exits without either accepting or rejecting the connection.
Eventually the provider will time out (T1 timer) the outstanding
T-CONNECT.INDICATION and the listener will get a T-DISCONNECT.INDICATION
on the original descriptor.
.LP
The alternative is to accept fully the connection when it arrives and
ignore the possibility of sending back user data on the connect-confirm.
or that the application might want to reject the connection.
This is probably not that fatal as transport user-data is not used
very much if at all, but it seems safest to keep the full ISODE
semantics if possible. Anyway, since it is up to the server process to
decide if it wants to accept the connection, it is still better to
wait.
.LP
A further complication to this debate is the issue of the T1 timer if
the provider really is TP4/CLNS.  The problem is that the same timer, which
in a minimal implementation (which covers most implementations) is a
configurable constant, is used both to time out a connect and to
generate data retries.  If the delayed accept approach is taken then the
timer must be long enough to swap in the tsapd and fork() and exec() the
server and for it to decide if it wishes to accept or reject the connection.
Since all this may happen on a loaded machine, with the executables
accessed via NFS and with access to the Directory Service,
one can imagine that this may be several tens of seconds.  But for
efficient recovery from dropped packets T1 wants to be very small, as
little as tens of milliseconds on an ethernet LAN.
The solution is for the provider to implement an adaptive T1 timer (that
works of the measured round-trip delay) once
in data-transfer state, but unfortunately this is rare.
.LP
One other area of interest is the connect phase. This could be split into
two parts, by forcing the t_connect to behave asynchronously even if the
endpoint was not opened as such, but this is not necessary as any received
user data and negotiated options can be communicated to TRetry() on the
tsapblk structure. So TRetry() may or may not have to do a t_rcvconnect().
.LP
Other problems are bound to occur as this sort of interface can only
really be tested with several working interfaces.
