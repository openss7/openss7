.\" pic |troff -ms
.nr PS 12
.nr VS 14
.ND
.sp 2
.TL
Directory navigation in the Quipu X.500 system
.sp 4
.AU
Paul Barker
Colin J. Robbins
.sp 4
.AI
12th October 1989
.AB
.nh
OSI Directory Services have recently been standardised according to the
X.500 / IS 9594 standard.  This first part of this paper gives a 
brief overview of the Directory Services model.
.PP
Quipu [1][2] is one of the first implementations of the X.500 standard and
has been developed at UCL. \(dg
.FS
\(dg The work was originally funded by under the European Strategic 
Program for Research
into Information Technology (ESPRIT). Quipu was developed as a deliverable
for INCA, project 395.  Quipu is currently funded by the U.K. Joint Network
Team.
.FE
Quipu is
publicly available as part of the ISODE [3] package.
.PP
One of the key aspects of Directory Services not fully specified
in the standard is that of managing the distribution of the Directory. The
approach taken by the Quipu system in representing Directory knowledge and
handling Directory navigation across heterogeneous networks is described
here.  The issues raised by this are discussed.
.PP
KEYWORDS: OSI, Directory, Quipu, Distributed System, Navigation.
.AE
.sp 5
.NH
Introduction
.LP
The first part of this paper introduces 
OSI Directory Services. These have recently been standardised according to 
the CCITT
X.500 recommendations / ISO 9594 International Standards [4].  This paper
gives a very brief overview of the standards framework.
.LP
The remainder of the paper discusses the approach taken by Quipu with regard
to directory navigation.  The discussion focusses on how Quipu attempts to
provide a robust and efficient service, given less than fully reliable,
heterogeneous networks.
.NH
Overview of Directory Services model
.LP
The OSI Directory is intended to support human user querying, allowing
users to find, inter alia, telephone and address information of
organisations and other users.  
.LP
It is also intended to support electronic
communication such as message handling systems and file transfer.
The Directory provides name to address mapping to support, for example, OSI
presentation address look-ups. Message handling systems will be provided with 
support for user-friendly naming, security and
distribution lists.
.NH 2
Directory characteristics
.LP
In essence the Directory is a database with certain key characteristics.
.IP 1.
The Directory is intended to be very large and highly distributed. It is
anticipated that the Directory will be distributed largely on an
organisational basis.
.IP 2.
The Directory is hierarchically structured, the entries being arranged in
the form of a tree. Entries near the root of the tree will usually represent
objects such as countries and organisations, entries at or near the leaves
of the tree will represent people, equipment or application processes.
.IP 3.
Read and search operations will dominate over modification operations.
.IP 4.
Temporary inconsistencies in the data are acceptable.  This greatly
facilitates the replication of data in the Directory by obviating concerns
about record locking and atomic operations.
.NH 2
Directory Object Model
.LP
The Directory can be decomposed into objects as in figure 1.
.KF
.PS
arc at 1.307,9.134 from 1.800,9.637 to 1.863,8.700
ellipse at 2.200,9.200 wid 1.125 ht 1.125
ellipse at 4.638,8.363 wid 1.075 ht 1.075
ellipse at 5.888,9.262 wid 1.225 ht 1.225
ellipse at 3.925,9.488 wid 1.025 ht 1.025
line from 2.840,9.002 to 2.737,9.012 to 2.823,8.955
line from 2.737,9.012 to 4.112,8.512
line from 4.010,8.523 to 4.112,8.512 to 4.027,8.570
line from 5.166,8.788 to 5.112,8.700 to 5.201,8.753
line from 5.112,8.700 to 5.362,8.950
line from 5.309,8.862 to 5.362,8.950 to 5.274,8.897
line from 4.590,9.458 to 4.487,9.450 to 4.582,9.409
line from 4.487,9.450 to 5.237,9.325
line from 5.135,9.317 to 5.237,9.325 to 5.143,9.366
line from 2.831,9.367 to 2.737,9.325 to 2.840,9.318
line from 2.737,9.325 to 3.425,9.450
line from 3.331,9.408 to 3.425,9.450 to 3.322,9.457
line from 3.300,10.137 to 7.237,10.137 to 7.237,7.513 to 3.300,7.513 to 3.300,10.137
.ps 11
"The Directory" at 5.425,7.606 ljust
.ps 11
"Figure 1:  Directory Object Model" at 2.300,6.918 ljust
.ps 11
"DSA 3" at 4.362,8.293 ljust
.ps 11
"DSA 2" at 5.612,9.231 ljust
.ps 11
"DSA 1" at 3.675,9.481 ljust
.ps 11
"DUA" at 1.988,9.168 ljust
.ps 11
"USER" at 0.863,9.106 ljust
.PE
.KE
.sp 2
.LP
A user accesses the Directory by means of a 
.I
Directory User Agent
.R
(DUA).
The DUA communicates with the Directory by using 
.I
Directory Access Protocol
.R
(DAP).
.PP
The Directory comprises a collection of 
.I
Directory System Agents
.R
(DSAs). Each
DSA has an associated database which holds some portion of the global
database.  The
DSAs cooperate to provide the Directory Service.  The DSAs communicate with
each other by using
.I
Directory System Protocol
.R
(DSP).
.PP
The distributed operation of the Directory is implemented by using one or
more of the following modes of operation:
.IP
Chaining is where a DSA passes an operation onto a further DSA, awaits the
response, and passes it back to the initiating DUA.
.IP
Referral is where a DSA returns a reference to another DSA back to the
initiating DUA or DSA.  This reference consists of the name of another DSA 
which the operation might be passed to.
.IP
Multicasting is where a request is broadcast to several DSAs, which may
then collectively resolve the request.
.LP
The combination of these modes of operation used by the Quipu implementation
are discussed later.
.NH 2
Structure of the Directory
.LP
It was noted earlier that the Directory is organised 
hierarchically in the form of
a tree.  The Directory database is usually referred to as the
.I
Directory Information Tree
.R
(DIT).
.PP
The overall structure of the DIT is shown in figure 2.
.sp
.KF
.PS
line from 4.050,6.950 to 4.862,6.638
line from 4.760,6.650 to 4.862,6.638 to 4.778,6.697
line from 3.237,6.950 to 2.862,6.638
line from 2.923,6.721 to 2.862,6.638 to 2.955,6.682
line from 4.362,6.638 to 6.300,6.638 to 6.300,6.138 to 4.362,6.138 to 4.362,6.638
line from 1.613,6.638 to 3.612,6.638 to 3.612,6.138 to 1.613,6.138 to 1.613,6.638
line from 5.362,8.075 to 5.737,7.450
line from 5.665,7.523 to 5.737,7.450 to 5.707,7.549
line from 4.800,8.075 to 3.862,7.450
line from 3.932,7.526 to 3.862,7.450 to 3.960,7.485
line from 4.862,9.075 to 5.300,8.575
line from 5.215,8.634 to 5.300,8.575 to 5.253,8.667
line from 4.300,9.075 to 3.237,8.512
line from 3.314,8.581 to 3.237,8.512 to 3.338,8.537
line from 3.550,9.887 to 4.425,9.512
line from 4.323,9.529 to 4.425,9.512 to 4.343,9.575
line from 2.862,9.887 to 2.050,9.512
line from 2.130,9.577 to 2.050,9.512 to 2.151,9.532
line from 4.925,7.450 to 6.612,7.450 to 6.612,7.013 to 4.925,7.013 to 4.925,7.450
line from 2.737,7.450 to 4.300,7.450 to 4.300,6.950 to 2.737,6.950 to 2.737,7.450
line from 4.675,8.575 to 6.237,8.575 to 6.237,8.075 to 4.675,8.075 to 4.675,8.575
line from 2.175,8.512 to 3.675,8.512 to 3.675,8.075 to 2.175,8.075 to 2.175,8.512
line from 3.987,9.512 to 5.300,9.512 to 5.300,9.075 to 3.987,9.075 to 3.987,9.512
line from 1.300,9.512 to 2.550,9.512 to 2.550,9.075 to 1.300,9.075 to 1.300,9.512
line from 2.675,10.262 to 3.800,10.262 to 3.800,9.887 to 2.675,9.887 to 2.675,10.262
.ps 11
"OU=Computer Science" at 2.800,7.168 ljust
.ps 11
"University" at 2.487,8.168 ljust
.ps 11
"O=Nottingham" at 2.425,8.356 ljust
.ps 11
"OU=Physics" at 5.175,7.168 ljust
.ps 11
"College London" at 4.925,8.231 ljust
.ps 11
"O=University" at 4.987,8.418 ljust
.ps 11
"Figure 2:  Hypothetical DIT" at 1.988,5.606 ljust
.ps 11
"CN=Colin Robbins" at 4.675,6.356 ljust
.ps 11
"CN=Paul Barker" at 1.925,6.356 ljust
.ps 11
"C=GB" at 4.300,9.293 ljust
.ps 11
"C=NL" at 1.488,9.293 ljust
.ps 11
"ROOT" at 2.925,10.043 ljust
.PE
.KE
.sp 2
The hypothetical
entries illustrate the hierarchy of the Directory and how entries are named
within the Directory.  At each point in the Directory, entries are
differentiated by unambiguous
.I
Relative Distinguished Names
.R
(RDNs). Thus, in figure 2, "C=GB" and "C=NL" are RDNs under the root of the
DIT.
An entry's
.I
Distinguished Name
.R
is derived by concatenating all the RDNs of the entries from the root of the
tree to the entry itself. 
.LP
Each entry may be further decomposed as shown in figure 3.
.KS
.PS
line from 5.237,7.700 to 6.112,7.200
line from 3.675,7.700 to 1.300,7.200
line from 3.800,9.262 to 5.487,8.575
line from 2.612,9.262 to 1.863,8.575
line from 5.050,6.950 to 5.987,6.950 to 5.987,6.325 to 5.050,6.325 to 5.050,6.950
line from 3.112,6.950 to 4.237,6.950 to 4.237,6.325 to 3.112,6.325 to 3.112,6.950
line from 1.488,6.950 to 2.800,6.950 to 2.800,6.325 to 1.488,6.325 to 1.488,6.950
line from 1.300,7.200 to 6.112,7.200 to 6.112,6.013 to 1.300,6.013 to 1.300,7.200
line from 3.675,8.262 to 5.237,8.262 to 5.237,7.700 to 3.675,7.700 to 3.675,8.262
line from 2.112,8.262 to 3.362,8.262 to 3.362,7.700 to 2.112,7.700 to 2.112,8.262
line from 1.863,8.575 to 5.487,8.575 to 5.487,7.450 to 1.863,7.450 to 1.863,8.575
line from 4.925,9.700 to 6.112,9.700 to 6.112,9.262 to 4.925,9.262 to 4.925,9.700
line from 2.612,9.700 to 3.800,9.700 to 3.800,9.262 to 2.612,9.262 to 2.612,9.700
line from 1.238,9.700 to 2.300,9.700 to 2.300,9.262 to 1.238,9.262 to 1.238,9.700
line from 0.988,10.012 to 6.237,10.012 to 6.237,9.012 to 0.988,9.012 to 0.988,10.012
.ps 11
"Figure 3: Structure of an entry" at 2.300,5.481 ljust
.ps 11
"Value" at 5.175,6.606 ljust
.ps 11
"Value" at 3.362,6.606 ljust
.ps 11
"Value" at 1.675,6.606 ljust
.ps 11
"Value(s)" at 3.925,7.918 ljust
.ps 11
"Type" at 2.300,7.918 ljust
.ps 11
"..." at 3.987,9.356 ljust
.ps 11
"Attribute" at 5.050,9.418 ljust
.ps 11
"Attribute" at 2.737,9.418 ljust
.ps 11
"Attribute" at 1.300,9.418 ljust
.PE
.KE
.sp 2
.PP
An entry comprises a set of attributes, which in turn consist of a type and
a value or set of values.  It should be noted that an entry's distinguished
name is merely a special attribute type-value pair.  For example, an entry
for a human being will have, inter alia, an attribute type
.I
Common Name.
.R
This attribute will often be multi-valued.  The Common Name attribute for
Steve Kille's entry might take the values "Steve Kille", "Stephen E. Kille"
and "S. Kille" with "Steve Kille" being the distinguished value.
.NH 2
Accessing the Directory
.LP
A user makes use of the Directory by means of the
.I
Directory Abstract Service.
.R
The services provided are grouped into three
.I
ports,
.R
the read port, the search port and the modify port. 
.PP
The read and search
ports provide querying facilities onto the Directory.  It is possible to
read or compare an entry identified by its distinguished name. The powerful
search operation allows querying of entire sub-trees, returning specified
attributes for all entries which satisfy the criteria specified in the search
arguments.  This allows entries to be identified by attributes other than
just the distinguished name and thus provides users with a highly flexible
mechanism for identifying entries and retrieving information 
from the Directory.
.PP
Modification operations allow the addition and removal of
entries from the DIT, the amendment of entries and the renaming of entries.
.NH 2
Other aspects Of Directory Services
.LP
There are many aspects of the Directory Services standard which cannot be
described in detail. Such aspects include:
.IP \(bu
Access control
.IP \(bu
Authentication
.IP \(bu
Service controls
.IP \(bu
Schemas
.IP \(bu
Use of OSI
.sp 3
.NH 
Distributed Operations in Quipu
.PP
The remainder of the paper focusses on the issue of distributed operations.
As the Directory is widely distributed, 
.I
knowledge
.R
must be maintained of how the DIT is distributed amongst the collection of
DSAs which comprise the Directory.  The standard does not specify how this
knowledge should be represented in the Directory.
The approach followed by Quipu is discussed.
.LP
It was noted earlier that the model allows for several modes of interaction
between DSAs as they cooperate to service requests made by DUAs; namely
chaining, referral and multicasting.  The approach used by Quipu is discussed,
with particular reference to the problem of coping with the situation where
the DIT is fragmented into DSAs on disjoint networks.
.NH 2
Directory Service requests requiring distributed operation
.LP
When considering the effects of directory distribution, there are four
possible results which can result from a request being presented by a DUA to
the DSA at the directory access point.
.IP i)
The request may be satisfied locally.
.IP ii)
The "local" DSA may be able to determine that the request cannot be serviced
by any DSA. The directory knowledge indicates that the entry required would
be held in that DSA if such an entry existed.  In this case the DSA would
return a
.I
name error
.R
to the DUA.
.IP iii)
A request is made to the local DSA which requires navigating down to 
a sub-tree not
held locally.  A set of references is acquired
indicating other DSAs which might be able to
satisfy the request.
.IP iv)
A request is made which requires navigating to a higher point in the tree 
than that held locally.  The addresses of DSAs nearer the root must be found
from local tables.
.LP
The rest of the paper discusses how Quipu proceeds in cases iii and iv above.
.NH 2
Representing directory knowledge
.LP
Case iii above requires the existence of knowledge information.  This is
information which a DSA has about which entries it holds and how to locate
other entries in the Directory.
The standard does not specify how or where this knowledge is stored.
Quipu takes the approach that the OSI Directory itself should be used, and
stores the knowledge in the DIT.
.PP
The first step in storing the knowledge is to give every DSA in the
directory an entry in the DIT which contains information about the DSA. 
For example
the DSA holding the data for University College London has the
distinguished name "(Country=GB, CommonName=Vicuna)", and has the following attributes:-
.DS
presentationAddress= Internet=128.16.8.50+50987 | X121=23421920030045
description= A wild animal of the Alpacca family, 
description= DSA running on vs1 holding full UCL bit of tree.
supportedApplicationContext= x500DAP & x500DSP & QuipuDSP
CommonName= Vicuna
objectClass= quipuDSA & dSA & applicationEntity & top
.DE
The first thing to notice is the name. It is a Quipu convention that all
Quipu DSAs are named after endangered South American wildlife. Quipu was
originally developed under the aegis of the ESPRIT project, INCA.
.LP
The above entry enables a DSA to determine the address or addresses of other 
DSAs.
However, a DSA still needs to determine which DSA to
contact to answer a particular request.  Quipu DSAs achieves this by requiring
that every non-leaf object 
has a "masterDSA" attribute, the value of which is the DN of the DSA to
contact.  
It is important to note that Quipu makes an important simplification of the
model in this respect.  It is assumed that if an entry is held in a DSA,
then all sibling entries are held in that DSA.  This assumption allows for a
relatively straightforward replication mechanism based on Quipu's getedb
mechanism. This is discussed in [1].
.PP
In addition, Quipu has added the concept of 
.I
slave
.R
DSAs to the model.
These are DSAs which hold a shadow copy of some data, and are prepared
to answer requests regarding that data.  
Thus a non-leaf entry may have "slaveDSA" attributes which give the DNs of
DSAs that hold such data.
.RS
.B
.sp
A Caveat on naming DSAs
.R
.LP
Using this approach, care must be taken to name the DSAs high enough in the
DIT to prevent looping.
For example consider a DSA holding the subtree for "(Country=GB, Organisation=University
College London)" which is named
"(Country=GB, Organisation=University College London, CommonName=Vicuna)".
If an operation attempted to list the subordinates of "(Country=GB, Organisation=University
College London)", a referral would have to be made to the DSA 
"(Country=GB, Organisation=University College London, CommonName=Vicuna)". This would require the 
entry "(Country=GB, Organisation=University College London, CommonName=Vicuna)" to be read by the DSA.
To read this entry, the DSA would have to know how to navigate to 
"(Country=GB, Organisation=University College London)" -- but does not know how to do that,
without seeing the "(Country=GB, Organisation=University College London, CommonName=Vicuna)"
entry!
Thus a (detectable) loop has been created.
To avoid this, DSAs should be named at the same level, or higher, in the DIT as
the entries they are holding.
This has the effect that there are lots of DSAs named at the higher
levels of the DIT.
.RE
.LP
When an operation cannot be satisfied locally,
a list of DSAs which either master or shadow the information will be 
generated by looking at these attributes.
We will now consider how Quipu chooses DSAs from this list to resolve
the request.
.NH 2
DSA selection criteria
.LP
It will be seen that randomly selecting a DSA from a list of possible DSAs
is not an optimal strategy.  The reasons for this are discussed below.
Quipu uses a number of criteria when establishing which DSA it will forward
a request to.  Rather than picking a single "best" DSA, Quipu sorts the list
of DSAs into an order of preference.  
A simple insert sort algorithm is used which successively compares pairs
of DSAs to see which is the "best".
.PP
It is worth noting here the reason why Quipu sorts the list of DSAs rather
than merely selecting the best DSA.  As will be explained in some detail
shortly, a Quipu DSA is able to make some assumptions about another 
DSA's behaviour if it knows that it is a Quipu DSA.  The semantics of X.500
dictate that a subordinate reference contains a single DSA 
if a request cannot be
satisfied at a given DSA.  However, the syntax of X.500 allows more than one 
DSA
to be named in a continuation reference.  Quipu sometimes takes advantage 
of this when communicating with other Quipu DSAs, by passing a
.I
Quipu-Specific Subordinate Reference
.R
(QSSR) which references multiple DSAs. QSSRs cannot always be used 
as some requests, for example
modification operations, and operations which specify the "don't use copy"
service control, must be directed at the sole master DSA.  In these cases a
standard subordinate reference is used.
.PP
This section discusses the criteria
which are used. The order of discussion indicates the weight given 
to the criteria.  The less important criteria are only used if no preference
can be deduced from the more important.
.NH 3
DAP only DSAs
.LP
DSAs which do not support DSP impose referral mode when other 
considerations might tend to favour chaining.  This restriveness means that
such DSAs are not favoured and any such DSAs
are placed at the bottom of the sorted DSAs list.
.NH 3
Prefer a Quipu DSA
.LP
The first choice it to select a Quipu DSA.
The main reason for this is that the DSAs can then talk over their own
application context (rather than the standard X500 DSP context), which
allows them to make a few simplifying assumptions, e.g. QSSRs (although 
the protocol used is the same).
.LP
This is represented in the Directory by a DSA having the attribute type
.I
Supported Application Context
.R
with a value "quipuApplicationContext".
.NH 3
Prefer a reliable DSA
.LP
Experience with Quipu-5.0 in which a DSA was chosen effectively at random
(but for the same query the same "random" DSA would be picked!)
showed that the network connections to some DSAs were much more unreliable
than others.
As a result, a lot of time was spent attempting associations that were almost
certain to fail.
Thus a mechanism has been introduced which attempts to identify reliable
DSAs.
.LP
To make this choice
every DSA holds the following information on each other DSA it tries to
contact:
.IP \(bu
Distinguished name of DSA
.IP \(bu
Time of last attempt
.IP \(bu
Time of last success
.IP \(bu
No. of failures since last success
.LP
Every time an association is attempted to a DSA, its DSAInfo is found, and
the 
.I
lastAttempt
.R
field is set to the current time.
If the association succeeds
.I
lastSuccess
.R
is set to the current time, and 
.I
failures-since-last-success
.R
is set to zero.
If the association fails
.I
failures-since-last-success
.R
is incremented.
.LP
The notion of
.I
recent
.R
success or failure is used to decide which DSA to prefer.  "Recent" is in
practice the value of the tailorable variable selected to age the cache of
connectivity information.  It is not at present clear what the optimum
timeout period is for aging this information.  This area requires further
experimentation.
.LP
The following algorithm is then used to select the more reliable DSA.
.LP
If both DSAs have been accessed successfully recently, prefer a DSA which
has suffered no recent communication failures.
If either communication with both DSAs has failed recently, or neither DSA
has a record of failure, then some other DSA selection criterion must be
used.  No attempt is made to discriminate between DSAs on the basis of how
recently the successes or failures occurred.
.LP
If only one of the DSAs has been successfully contacted recently, prefer
that DSA unless it also has a record of recent failure. In the case of a
recent failure, prefer the other DSA, unless it also failed recently in
which case no discrimination can be made.
.LP
If neither DSA has been contacted successfully recently, some other
criterion must be used to choose between the DSAs.
.NH 3
Prefer a close DSA
.LP
A close DSA may be preferable for a number of reasons.
Network charges may be lower, or non-existent, for proximate DSAs.
Physically close DSAs may often be connected by networks offering greater
bandwidth.  Physically close DSAs may be separated by fewer gateways than
DSAs separated by great distances.
.LP
The following sections suggest 3 ways a 
.I
close
.R
DSA may be selected.
.sp
.LP
Clearly it is preferable to choose a DSA on the same local area network, 
or using
the preferred network type if possible.
To make this decision, we need a method of addressing DSAs on different
networks, that is:
.IP i)
compatible with the standard, that is it can be stored in the "presentation
address" attribute of a DSA;
.IP ii)
can supply sub network details.
.LP
OSI purists may well be alarmed at this point.  Network layer details should
be hidden from applications.  NSAPs should not contain routing information.
.LP
However, at present, real users do not use OSI network services. Network
services are currently provided largely by TCP/IP and X.25 (1980) networks.
These network domains are themselves not fully connected. TCP/IP is often
used on LANs which are not connected to the Internet. X.25 domains exist,
such as the U.K.'s JANET, which are not fully connected to the international
X.25 networks.
.PP
Until OSI network services are available to and used by almost all users, a
work-around solution is required.  Kille [5] has defined a mapping between
the various network address spaces and OSI presentation addresses.  This
uses a part of the Telex address space to hold the encoded addresses.
.sp
.LP
Every DSA has a distinguished name and this can be used to select a potentially
close DSA.
For example, if our DSA is called "Country=GB, Organisation=University
College London, CommonName=Tamarin", and 
we have a references to DSAs "Country=US, CommonName=Fruit Bat", and "Country=GB, CommonName=Vicuna",
then on the basis of distinguished names, "Country=GB, CommonName=Vicuna" is 
.I
likely
.R
to be closer.
.sp
.LP
DSAs may be managed in Directory Management Domains (DMD) for accounting
purposes.  If a DSA is in the same DMD as the requestor, then if may be best
to use this DSA in preference to a DSA in a different domain.
.LP
Quipu does not currently use this as a selector, as the concept of DMD has
not been utilised fully in current pilot exercises, thus the selector would
always return "no difference" when comparing two DSAs.
.NH 3
Need for experimentation
.LP
How successful this algorithm is in practice remains to be seen.
Quipu-6.0, which attempts to make the above decisions, is about to 
be released.
However successful the algorithm proves to be, one may be fairly confident
that the method is better that a random selection.
.NH 2
Chaining, referrals, multicasting
.LP
Having decided which DSA or DSAs to contact to follow references, the
decision of which intercation model to use still has to be made.  This is
now considered.
.PP
Quipu has a basic framework for interaction between DUA and DSA, and between
two DSAs.  We will see later that are several situations which force
departure from this model.  
.LP
The basic model is as follows:
.IP
If the first DSA contacted cannot satisfy a request, it chains that request
on to a second DSA.  If the second DSA cannot satisfy the request it sends a
referral back to the initial DSA which then chains the request to the
referenced
DSA.  From the viewpoint of the DUA, the model is one of chaining.  From the
viewpoint of the first DSA, the model is one of referral.
.sp
.LP
The advantages of this model are as follows:
.IP i)
The work of the DUA is simplified by placing a heavy onus on the DSA at 
the DUA's access point.  All references are
followed by the DSA.  The DUA only needs a single access point onto the
Directory.
.IP ii)
A corollary of the access point DSA shouldering the burden of chasing
referrals is that the DSA is able to cache all the information that it
acquires from other DSAs.  Caching can dramatically improve performance for
all the DUAs and DSAs which communicate with that DSA.  Obviously care needs
to be exercised as the cache ages and caches have to be purged periodically.
Great care also needs to be taken that access control mechanisms are not
circumvented by the use of caching.
.IP iii)
The DUA only needs to be on the same network as its access point DSA. Full
connectivity with the Directory can be achieved so long as that DSA can
contact other DSAs by chaining or referral.  It should be noted that this
problem can be circumvented by the use of transport service bridges.
.IP iv)
The model is a good basis for a charging policy.  
The best model for charging would be one of DUA referral where all charges
could be assigned to the originating DUAs.  For reasons already discussed
this is not the best model for a variety of other reasons.  The DSA referral
model provides a reasonable, second best approach.
All DUA requests which
generate requests across wide area, chargeable networks, are initiated by a
single DSA which represents the DUA.  It is clearly very difficult to
administer a charging policy for any model which allows for a
substantial amount of chaining.
To cope with this problem, Quipu in fact allows a DSA to "defend" itself
against chaining requests by setting a "dsp_chaining" variable to "off".
.IP v)
The DSA referral model allows more control over an operation and may be
beneficial if some DSAs are not highly reliable.  Under the chaining model,
if knowledge is fairly minimal, unreliable DSAs may cause part of the DIT to
become detached and unreachable.  Under the DSA referral model, a local
Directory administrator can try and guard against this by ensuring that
considerable knowledge is held locally.
.LP
It should be noted that the above model cannot always be adhered to.  The
following reasons all require a different approach.
.IP 
Service controls might, for example, be set such that chaining is prohibited
whereas the model indicates that a DSA should chain.
.IP
Some DSA implementations only support DAP although supporting DSP is a
requirement of the standard. If such DSAs are referenced when a request
cannot be satisfied locally, the request can only be pursued further
by DUA referral.
.IP
It is a fact of life, as noted earlier, that DSAs will
be run on disjoint networks.  Ensuring that the Directory does not itself
become disjointed requires cognisance of the underlying networks when
assessing whether to chain or refer a directory request.
.IP
A basic assumption of the model is that DSAs should trust each other.
However, such trust can in practice only be based on DSAs being able to
authenticate each other.  Quipu does not currently use authentication
between DSAs for the following reasons: simple authentication is regarded as
being too simple to forge to be worthwhile; strong authentication is
time-consuming and requires a framework to manage the requisite certificates.
However, the performance of the encryption algorithms has been considerably
improved of late and strong authentication is being actively considered for
the next release of Quipu.
For this reason, Quipu will not perform modification
operations over DSP.  Thus DSAs must send referrals back to a DUA, whatever
the model suggests is the preferred mode of interaction.
.NH 2
Chaining preferred
.LP
If the above model indicates that chaining is preferred, the following
algorithm is then used to finally select a DSA to contact:
.IP
The ordered list of DSAs is searched to see if there is a connection already
open to any of the DSAs.  If there is, the request is forwarded to the first
such DSA on the list.
.IP
If the DSA does not have a connection open to any of the DSAs in the list,
the DSA tries to open a connection to each DSA in turn until a connection
attempt is successful.
.IP
If all connection attempts fail, the DSA then tries to send a referral. The
DSA attempts to select the first DSA in the list which appears to be on a
compatible network.
.IP
If none of the DSAs in the list appear to be on a compatible network, a
referral is sent back which names the first DSA in the list.
.NH 2
Referral preferred
.LP
If the model indicates that referral is preferred, the following procedure
is used.  It should be noted that the network compatibility testing is
crucial in creating a widely distributed Directory spread over
heterogeneous networks.
.IP
The DSA searched the list for any DSA with apparent network compatibility
with the calling DUA or DSA.
.IP
If any DSA is found which appears to be on compatible network, then if 
the initiating DSA is a Quipu DSA, the list of references are returned
to that DSA. If the initiator is not a Quipu DSA, then only the single,
"network compatible" reference is returned.  If the initiating DSA is a Quipu
DSA and receives a list of DSAs, the procedure described earlier to sort the
DSAs into an order of preference, is followed.  The initiating DSA must
discard any earlier lists of DSAs it had compiled or received earlier while
trying to
complete the operation, on receipt of a further list of references.
.IP
If no DSA in the list appears to on a network compatible with the
originator, then an attempt is made to chain the operation, service controls
permitting.  If chaining fails, a referral is sent to the originating DSA.
.NH 2
Multicasting
.LP
In general, Quipu does not need to multicast because of the assumption that all
sibling entries are held within a single DSA.
However there are two occasions when Quipu makes use of multicasting.
.IP i)
Subtree searches, where the subtree is held in multiple DSAs.
.IP ii)
When following a non-specific subordinate references, generated by non-Quipu
DSAs.
.NH
Conclusions
.LP
The approach used by Quipu to store OSI Directory knowledge has been described.
It seems prudent that this information should be stored in the Directory
itself.
.PP
The Directory is distributed across a large number of DSAs.  These DSAs may
reside on disjoint networks.  The approach taken by Quipu to solve this
problem has been discussed.
.PP
Some replication of the Directory will tend to improve the Directory's
resilience to DSA failure and may also improve performance generally.  The 
mechanisms used by Quipu to determine which
DSA to forward requests to has been described.
.PP
Some differences between the standard X.500 model and the Quipu
implementation have been noted.  Quipu takes account of DSAs being
situated on disjoint networks.  Furthermore, Quipu tries to provide a robust
and efficient service by noting DSA reliability, connectivity and proximity.
.sp 3
.br
.B
References
.R
.IP [1]
"The design of Quipu", Stephen E. Kille, Research Note RN/89/19, Department
of Computer Science, University College London, March 1989.
.IP [2]
"The QUIPU Directory Service", Stephen E. Kille, IFIP WG 6.5 Conference on
Message Handling Systems and Distributed Applications, pp173-186. North
Holland Publishing, October 1988.
.IP [3]
"The ISO Development Environment Users's Manual (Version 5.0)", 
Marshall T. Rose, The Wollongong Group, Palo Alto, March 1989.
.IP [4]
"The Directory - Overview of Concepts, Models and Services", X.500 and ISO
9594, 1988.
.IP [5]
"An interim approach to the use of network addresses", Stephen E. Kille, 
Research Note RN/89/19, Department of Computer Science, 
University College London, March 1989.
