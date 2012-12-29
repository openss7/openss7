
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4,	# also RL length in octets
	RT_24BIT_PC => 7,	# also RL length in octets

	DX_UNKNOWN => 0,
	DX_HALF_DUPLEX => 1,
	DX_FULL_DUPLEX => 2,

	HT_UNKNOWN => 0,
	HT_BASIC => 3,		# also link level header length
	HT_EXTENDED => 6,	# also link level header length

	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3,

	SLL_DIR_RECEIVED    => 0,   # sent to us by somebody else
	SLL_DIR_BROADCAST   => 1,   # broadcast by somebody else
	SLL_DIR_MULTICAST   => 2,   # multicast by somebody else
	SLL_DIR_MONITOR	    => 3,   # sent to somebody else by somebody else
	SLL_DIR_SENT	    => 4,   # sent by us

	DLT_ETHERNET => 1,
	DLT_LINUX_SLL => 113,
	DLT_MTP2_WITH_PHDR  => 139,

	PPI_M2PA => 5,
	PPI_M2UA => 2,
	PPI_M3UA => 3,
	PPI_SUA => 4,

	PORT_M2PA => 3565,
	PORT_M2UA => 2904,
	PORT_M3UA => 2905,
	PORT_SUA => 14001,

	CH_DATA => 0,
	CH_INIT => 1,
	CH_INIT_ACK => 2,
	CH_SACK => 3,
	CH_BEAT => 4,
	CH_BEAT_ACK => 5,
	CH_ABORT => 6,
	CH_SHUTDOWN => 7,
	CH_SHUTDOWN_ACK => 8,
	CH_ERROR => 9,
	CH_COOKIE_ECHO => 10,
	CH_COOKIE_ACK => 11,
	CH_SHUTDOWN_COMPLETE => 14,
};

# -------------------------------------
package Decoder;
# -------------------------------------
# A Decoder is a context for decoding messages.  It contains information that is
# not present in each message but is necessary for the decoding of all messages.
# This can be used as a base class for other objects.
# -------------------------------------

sub add_pdu {
	my ($self,$pdu,@args) = @_;
	my $meth = 'add_pdu_'.(split(/::/,ref $pdu))[-1];
	return $sub->(@_) if my $sub = $self->can($meth);
	return unless my $sdu = $pdu->decode($self,@args);
	return $self->add_pdu($sdu,@args);
}

# -------------------------------------
package Decoder::Stream; our @ISA = qw(Decoder);
# -------------------------------------
# An SCTP Stream decoder must maintain state information to reassemble SCTP data
# chunks into messages.
# -------------------------------------
#package Decoder::Stream;
sub init {
	my $self = shift;
	my ($decoder,$chk) = @_;
	$self->{decoder} = $decoder; # parent decoder
	$self->{dir}     = $chk->{dir};
	$self->{stream}  = $chk->{stream};
	$self->{chunks}  = [];
	$decoder->{streams}{$self->{dir}}{$self->{stream}} = $self;
}
#package Decoder::Stream;
sub fini {
	my $self = shift;
	delete $decoder->{streams}{$self->{dir}}{$self->{stream}}
		if my $decoder = $self->{decoder};
	foreach (qw(decoder stream chunks)) { delete $self->{$_} }
}
#package Decoder::Stream;
sub process_msg {
	my ($self,$chunks) = @_;
	my $decoder = $self->{decoder};
	if (my $msg = Message::ULP->new($decoder,$chunks)) {
		$decoder->add_pdu($msg);
	}
}
#package Decoder::Stream;
sub add_chk {
	my $self = shift;
	my ($chk,$oos) = @_;
	return if $chk->{flags} & 0x4;
	my $beg = (($chunk->{flags} & 0x2) != 0);
	my $end = (($chunk->{flags} & 0x1) != 0);
	if ($beg) {
		$self->{chunks} = [];
		$self->{ssn} = $chunk->{ssn};
		if ($end) {
			$self->process_msg([$chunk]);
		} else {
			push @{$self->{chunks}},$chunk;
		}
		($self->{beg},$self->{end}) = ($beg,$end);
		return;
	}
	if ($oos or $self->{ssn} != $chunk->{ssn}) {
		$self->{chunks} = [];
		return;
	}
	if ($end) {
		push @{$self->{chunks}}, $chunk;
		$self->process_msg($self->{chunks});
		$self->{chunks} = [];
	}
}

# -------------------------------------
package Decoder::SCTP; our @ISA = qw(Decoder);
# -------------------------------------
# An SCTP decoder must maintain state information to reassembled SCTP data
# chunks into messages
# -------------------------------------
#package Decoder::SCTP;
sub init {
	my $self = shift;
	my $type = ref $self;
	my $pkt  = pop;
	
	$self->{ppi}     = $pkt->{ppi};
	$self->{sport}   = $pkt->{sport};
	$self->{dport}   = $pkt->{dport};
	$self->{vtag}    = $pkt->{vtag};
	$self->{acks}    = {0=>undef,1=>undef}; # cumm. acks
	$self->{tsns}    = {0=>{},   1=>{},  }; # data chunks
	$self->{streams} = {0=>{},   1=>{},  }; # streams
	$self->{chunks}  = {0=>[],   1=>[],  }; # reassembled messages
}
#package Decoder::SCTP;
sub fini {
	my $self = shift;
	foreach (qw(acks tsns streams chunks ulps)) { delete $self->{$_} }
	foreach (qw(ppi sport dport vtag)) { delete $self->{$_} }
}
#package Decoder::SCTP;
sub add_chk {
	my ($self,$pdu,@args) = @_;
	my $meth = 'add_chk_'.(split(/::/,ref $pdu))[-1];
	$sub->(@_) if my $sub = $self->can($meth);
}
#package Decoder::SCTP;
sub tst_chk {
	my ($self,$pdu,@args) = @_;
	my $meth = 'tst_chk_'.(split(/::/,ref $pdu))[-1];
	$sub->(@_) if my $sub = $self->can($meth);
}
#package Decoder::SCTP;
sub add_pdu_SCTP {
	my ($self,$pdu) = (shift,shift);
	foreach (@{$pdu->{chunks}}) {
		$self->add_chk($_,@_);
	}
}
#package Decoder::SCTP;
sub tst_pdu_SCTP {
	my ($self,$pdu) = (shift,shift);
	foreach (@{$pdu->{chunks}}) {
		return 1 if $self->tst_chk($pdu,$_,@_);
	}
}
#package Decoder::SCTP;
sub add_chk_DATA {
	my ($self,$chk) = (shift,shift);
	my $dir = $chk->{dir};
	$self->{acks}{$dir} = $chk->{tsn} unless exists $self->{acks}{$dir};
	my $cumm = $self->{acks}{$dir};
	my $num = $chk->{tsn} - $cumm;
	return if $num <= 0; # duplicated or cleared TSN
	return if exists $self->{tsns}{$dir}{$num}; # duplicate not cleared TSN
	$self->{tsns}{$dir}{$num} = $chk;
}
#package Decoder::SCTP;
sub add_chk_ack {
	my ($self,$chk) = @_;
	my $dir = $chk->{dir}^0x1;
	$self->{acks}{$dir} = $chk->{cumm} unless exists $self->{acks}{$dir};
	my $cumm = $self->{acks}{$dir};
	my $ack = $chk->{cumm} - $cumm;
	return if $ack <= 0; # doesn't ack anything
	my $seq = undef;
	foreach my $num (sort {$a<=>$b} keys %{$self->{tsns}{$dir}}) {
		$seq = $num unless defined $seq;
		my $c = delete $self->{tsns}{$dir}{$num};
		if ($num <= $ack) {
			my $oos = ($seq != $num);
			# clear chunks numbered less than or equal to $ack
			my $stream = Decoder::Stream->new($self,$c,$dir)
				unless $stream = $self->{streams}{$dir}{$c->{stream}};
			$stream->add_chk($c,$dir,$oos);
		} else {
			# renumber chunks in tsn queue to $num - $ack
			$self->{tsns}{$dir}{$num-$ack} = $c;
		}
	}
}
#package Decoder::SCTP;
sub add_chk_SACK {
	return shift->add_chk_ack(@_);
}
#package Decoder::SCTP;
sub add_chk_SHUTDOWN {
	return shift->add_chk_ack(@_);
}
#package Decoder::SCTP;
sub tst_chk_DATA {
	my ($self,$pdu,$chk) = @_;
	unless (exists $self->{acks}{0} or exists $self->{acks}{1}) {
		# no data or ack chunk received yet, assume forward direction
		$pdu->{dir} = 0;
		return 1;
	}
	my $found = undef;
	for (my $dir = 0;$dir<2;$dir++) {
		next unless exists $self->{acks}{$dir};
		my $cumm = $self->{acks}{$dir};
		my $num = $chk->{tsn} - $cumm;
		# TSN would certainly have been filled in by fast-retransmit
		next if -30 > $num or $num > 30;
		# both match, cannot tell
		return if defined $found;
		$found = $dir;
	}
	if (defined $found) {
		$pdu->{dir} = $found;
		return 1;
	}
}
#package Decoder::SCTP;
sub tst_chk_ack {
	my ($self,$pdu,$chk) = @_;
	unless (exists $self->{acks}{0} or exists $self->{acks}{1}) {
		# no data or ack chunk received yet, assume reverse direction
		$pdu->{dir} = 1;
		return 1;
	}
	my $found = undef;
	for (my $dir = 1;$dir>= 0;$dir--) {
		next unless exists $self->{acks}{$dir};
		my $cumm = $self->{acks}{$dir};
		my $ack = $chk->{cumm} - $cumm;
		# Cumm acks must be fairly close together
		next if -30 > $num or $num > 30;
		# both match, cannot tell
		return if defined $found;
		$found = $dir;
	}
	if (defined $found) {
		$pdu->{dir} = $found ^ 0x1;
		return 1;
	}
}
#package Decoder::SCTP;
sub tst_chk_SACK {
	return shift->tst_chk_ack(@_);
}
#package Decoder::SCTP;
sub tst_chk_SHUTDOWN {
	return shift->tst_chk_ack(@_);
}


# -------------------------------------
package Decoder::SCTP::M2PA; our @ISA = qw(Decoder::SCTP);
package Decoder::SCTP::M2UA; our @ISA = qw(Decoder::SCTP);
package Decoder::SCTP::M3UA; our @ISA = qw(Decoder::SCTP);
package Decoder::SCTP::SUA;  our @ISA = qw(Decoder::SCTP);
# -------------------------------------

# -------------------------------------
package Frame;
# -------------------------------------
#package Frame;
sub testit {
	return 1;
}
#package Frame;
sub try {
	my $self = {};
	my $type = shift;
	bless $self,$type;
	my $pdu = shift;
	$self->{pdu} = $pdu;
	$self->{hdr} = $pdu->{hdr};
	$self->{buf} = $pdu->{buf};
	$self->{beg} = $pdu->{doff};
	$self->{off} = $pdu->{doff};
	$self->{end} = $pdu->{dend};
	return $self->testit(@_);
}
#package Frame;
sub detect {
	return;
}
#package Frame;
sub unpack {
	my $self = shift;
	my ($decoder,$stop) = @_;
	my $type = 'Frame::Pcap';
	bless $self,$type;
	return $self->unpack(@_) unless $stop and $self->isa($stop);
	return 0;
}
#package Frame;
sub new {
	my $self = {};
	my $type = shift;
	bless $self,$type;
	my $pdu = shift;
	$self->{pdu} = $pdu;
	$self->{hdr} = $pdu->{hdr};
	$self->{buf} = $pdu->{buf};
	$self->{beg} = $pdu->{doff};
	$self->{off} = $pdu->{doff};
	$self->{end} = $pdu->{dend};
	return $self->unpack(@_);
}
#package Frame;
sub decode {
	return undef;
}
#package Frame;
sub dissect {
	my $self = shift;
	my ($decoder,$stop) = @_;
	my ($type,$ret) = (ref $self,undef);
	my @layers = ();
	bless $self,'Frame::Pcap';
	while ($ret = $self->dissect(@_) and ref $self ne $type) {
		push @layers, $ret;
		last if $stop and $self->isa($stop);
		$type = ref $self;
	}
	return \@layers;
}
#package Frame;
sub pbits {
	my ($val,$msk) = @_;
	my $s=' ';
	my $len = length($msk);
	my @v = unpack('C*',$val);
	my @m = unpack('C*',$msk);
	for (my $l=0;$l<$len;$l++,$s=' '.$s) {
		my ($bv,$bm) = (pop @v,pop @m);
		for (my $b=0;$b<8;$b++,$bm>>=1,$bv>>=1) {
			if ($bm&0x1) {
				if ($bv&0x1) {
					$s='1'.$s;
				} else {
					$s='0'.$s;
				}
			} else {
				$s='-'.$s;
			}
		}
	}
	return $s;
}
#package Frame;
sub bytes {
	my ($val,$msk,$len) = @_;
	my $s=' ';
	for (my $l=0;$l<$len;$l++,$s=' '.$s) {
		for (my $b=0;$b<8;$b++,$msk>>=1) {
			if ($msk&0x1) {
				if ($val&0x1) {
					$s='1'.$s;
				} else {
					$s='0'.$s;
				}
				$val>>=1;
			} else {
				$s='-'.$s;
			}
		}
	}
	return $s;
}
#package Frame;
sub spare {
	my ($val,$msk,$len) = @_;
	my $s = ' ';
	for (my $l=0;$l<$len;$l++,$s=' '.$s) {
		for (my $b=0;$b<8;$b++,$msk>>=1) {
			if ($msk&0x1) {
				if ($val&0x1) {
					$s='1'.$s;
				} else {
					$s='X'.$s;
				}
				$val>>=1;
			} else {
				$s='-'.$s;
			}
		}
	}
	return $s;
}

# -------------------------------------
package Frame::Pcap; our @ISA = qw(Frame);
# -------------------------------------
sub new {
	my ($type,$src,$dlt,$buf,$hdr) = @_;
	my $self = {
		src=>$src,
		dlt=>$dlt,
		buf=>$buf,
		hdr=>$hdr,
		beg=>0,
		off=>0,
		end=>$hdr->{caplen},
	};
	bless $self,$type;
	return $self;
}
use constant {
	DLT_TYPES=>{
		&::DLT_ETHERNET=>'Ethernet',
		&::DLT_LINUX_SLL=>'LinuxSLL',
		&::DLT_MTP2_WITH_PHDR=>'Mtp2WithPhdr',
	},
};
sub unpack {
	my $self = shift;
	my $type = DLT_TYPES->{$self->{dlt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self->upack(@_);
}
use constant {
	DLT_NAMES=>{
		&::DLT_ETHERNET=>'DLT_ETHERNET',
		&::DLT_LINUX_SLL=>'DLT_LINUX_SLL',
		&::DLT_MTP2_WITH_PHDR=>'DLT_MTP2_WITH_PHDR',
	},
};
sub dissect {
	my $self = shift;
	my $name = DLT_NAMES->{$self->{dlt}};
	return 'PCAP Header', [
		[ undef, undef, 'Data Link Type', $name ],
		[ undef, undef, 'Package Length', $self->{hdr}{len}],
		[ undef, undef, 'Capture Length', $self->{hdr}{caplen}],
		[ undef, undef, 'Seconds', $self->{hdr}{tv_sec} ],
		[ undef, undef, 'Microseconds', $self->{hdr}{tv_usec} ],
	], @_;
}
# -------------------------------------
package Frame::Pcap::Ethernet; our @ISA = qw(Frame::Pcap);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->{off} + 14 <= $self->{end};
	$self->{dhwa} = substr(${$self->{buf}},$self->{off},6);
	$self->{off} += 6;
	$self->{shwa} = substr(${$self->{buf}},$self->{off},6);
	$self->{off} += 6;
	$self->{etyp} = unpack('n',substr(${$self->{buf}},$self->{off},2));
	$self->{off} += 2;
	$self->{elen} = $self->{etyp};
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	return $self;
}
sub decode {
	return new Frame::IEEE802(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	return $self->SUPER::dissect('Ethernet header', [
		[$off+ 0,Frame::pbits($self->{dhwa},$mask),'Dest Eth Addr',undef ],
		[$off+ 6,Frame::pbits($self->{shwa},$mask),'Srce Eth Addr',undef ],
		[$off+12,Frame::bytes($self->{etyp},0xffff,2),'Etype/Len',$self->{etyp}],
	], @_,);
}
# -------------------------------------
package Frame::Pcap::LinuxSLL; our @ISA = qw(Frame::Pcap);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->{off} + 16 <= $self->{end};
	(
		$self->{from},
		$self->{arph},
		$self->{llen},
	) =
	unpack('nnn',substr(${$self->{buf}},$self->{off},6));
	$self->{off} += 6;
	my $len = $self->{llen}; $len = 8 unless $self->{llen} <= 8;
	$self->{shwa} = substr(${$self->{buf}},$self->{off},$len);
	$self->{off} += 8;
	$self->{etyp} = unpack('n',substr(${$self->{buf}},$self->{off},2));
	$self->{off} += 2;
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	return $self;
}
sub decode {
	return new Frame::IEEE802(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	return unless $self->unpack(@_);
	my $len = $self->{llen}; $len = 8 unless $self->{llen} <= 8;
	my $mask = substr(pack('NN',0xffffffff,0xffffffff),0,$len);
	my $from = {
		::SLL_DIR_RECEIVED  => 'SLL_DIR_RECEIVED',
		::SLL_DIR_BROADCAST => 'SLL_DIR_BROADCAST',
		::SLL_DIR_MULTICAST => 'SLL_DIR_MULTICAST',
		::SLL_DIR_MONITOR   => 'SLL_DIR_MONITOR',
		::SLL_DIR_SENT	    => 'SLL_DIR_SENT',
	}{$self->{from}};
	return $self->SUPER::dissect('Linux Cooked Header', [
		[$off +  0, Frame::bytes($self->{from},0xffff,2),'From',$from],
		[$off +  2, Frame::bytes($self->{arph},0xffff,2),'Arp Hardware', sprintf('0x%04X',$self->{arph})],
		[$off +  4, Frame::bytes($self->{llen},0xffff,2),'Addr Length',$self->{llen}],
		[$off +  6, Frame::pbits($self->{shwa},$mask), 'Srce Addr',undef],
		[$off + 14, Frame::bytes($self->{etyp},0xffff,2),'Ethertype',sprintf('0x%04X',$self->{etype})],
	],@_);
}
# -------------------------------------
package Frame::Pcap::Mtp2WithPhdr; our @ISA = qw(Frame::Pcap);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->{off} + 4 <= $self->{end};
	(
		$self->{way},
		$self->{xsn},
		$self->{ppa},
	)
	= unpack('CCn',substr(${$self->{buf}},$self->{off},4));
	$self->{off} += 4;
	$self->{dir} = (($self->{way} ^ 0x1) ^ 0x1);
	$self->{card} = ($self->{ppa} >> 7) & 0x3;
	$self->{span} = ($self->{ppa} >> 5) & 0x3;
	$self->{slot} = ($self->{ppa} >> 0) & 0x1f;
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	return $self;
}
sub decode {
	return new Message::MTP2(@_);
}
sub dissect {
	my $self = shift;
	my $off = 0;
	return $self->SUPER::dissect('MTP2 Pseudo-header', [
		[$off+ 0,Frame::bytes($self->{way},0xff000000,4),'Way',$way],
		[$off+ 0,Frame::bytes($self->{xsn},0x00ff0000,4),'Annex A',$xsn],
		[$off+ 0,Frame::bytes($self->{ppa},0x0000ffff,4),'PPA', "$self->{card}:$self->{span}:$self->{slot}"],
	],@_);
}
# -------------------------------------
package Frame::IEEE802; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	ETHER_TYPES=>{
		0x0800=>'IP',
		0x0806=>'ARP',
		0x8100=>'TPID',
		0x88cc=>'LLDP',
	},
};
sub unpack {
	my $self = shift;
	my $pdu = $self->{pdu};
	$self->{etyp} = $pdu->{etyp};
	$self->{shwa} = $pdu->{shwa};
	$self->{dhwa} = $pdu->{dhwa} if $pdu->{dhwa};
	my $type = ETHER_TYPES->{$self->{etyp}};
	if ($type eq 'TPID') { # priority/VLAN tagged frame
		return unless $self->{off} + 4 <= $self->{end};
		(
			$self->{tci},
			$self->{etyp},
		) =
		unpack('nn',substr(${$self->{buf}},$self->{off},4));
		$self->{off}+=4;
		$self->{upri} = ($self->{tci} >> 13) & 0x7;
		$self->{cfi}  = ($self->{tci} >> 12) & 0x1;
		$self->{vlan} = ($self->{tci} >>  0) & 0x0fff;
		$type = ETHER_TYPES->{$self->{etyp}};
		$type = undef if $type eq 'TPID';
	}
	unless ($type) {
		bless $self,ref($self).'::LLC';
		return $self->unpack(@_);
	}
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	if (exists $self->{vlan}) {
		return 'IEEE 802.1P Tag', [
			[$off+0,Frame::bytes($self->{upri},0xe000,2),'User Priority',$self->{upri}],
			[$off+0,Frame::bytes($self->{cfi}, 0x1000,2),'CFI',$self->{cfi}],
			[$off+0,Frame::bytes($self->{vlan},0x0fff,2),'VLAN',$self->{vlan}],
		],@_;
	}
}
# -------------------------------------
package Frame::IEEE802::IP; our @ISA = qw(Frame::IEEE802);
# -------------------------------------
sub unpack { return $self }
sub decode {
	return new Frame::IP(@_);
}
# -------------------------------------
package Frame::IEEE802::ARP; our @ISA = qw(Frame::IEEE802);
# -------------------------------------
sub unpack { return $self }
sub decode {
	return new Frame::ARP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLDP; our @ISA = qw(Frame::IEEE802);
# -------------------------------------
sub unpack { return $self }
sub decode {
	return new Frame::LLDP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC; our @ISA = qw(Frame::IEEE802);
# -------------------------------------
use constant {
	SNAP_HDR    =>pack('CCC',0xaa,0xaa,0x03),
	NLPID_HDR   =>pack('CCC',0xfe,0xfe,0x03),
}
sub unpack {
	my $self = shift;
	$self->{llc} = substr(${$self->{buf}},$self->{off},3);
	$self->{off} += 3;
	(
		$self->{dsap},
		$self->{ssap},
		$self->{ctrl},
	) =
	unpack('CCC',$self->{llc});
	if ($self->{llc} eq SNAP_HDR) {
		bless $self,ref($self).'::SNAP';
		return $self->unpack(@_);
	}
	if ($self->{llc} eq NLPID_HDR) {
		bless $self,ref($self).'::NLPID';
		return $self->unpack(@_);
	}
	return;
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	my $dsap = sprintf('0x%02X',$self->{dsap});
	my $ssap = sprintf('0x%02X',$self->{ssap});
	return $self->SUPER::dissect('LLC Header', [
		[$off+0,Frame::bytes($self->{dsap},0xff,1),'DSAP',$dsap],
		[$off+1,Frame::bytes($self->{ssap},0xff,1),'SSAP',$ssap],
		[$off+2,Frame::bytes($self->{ctrl},0xff,1),'Control','UI'],
		@_,
	]);
}
# -------------------------------------
package Frame::IEEE802::LLC::SNAP; our @ISA = qw(Frame::IEEE802::LLC);
# -------------------------------------
use constant {
	ETHER_TYPES=>{
		0x0800=>'IP',
		0x0806=>'ARP',
		0x88cc=>'LLDP',
	},
	SNAP_OUI    =>pack('CCC',0x00,0x80,0xc2),
	RFC1402_OUI =>pack('CCC',0x00,0x00,0x00),
};
sub unpack {
	my $self = shift;
	$self->{snap} = $self->{llc};
	$self->{oui}  = substr(${$self->{buf}},$self->{off},3);
	$self->{off} += 3;
	return unless $self->{oui} eq SNAP_OUI or $self->{oui} eq RFC1402_OUI;
	$self->{etyp} = unpack('n',substr(${$self->{buf}},$self->{off},2));
	$self->{off} += 2;
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	my $type = ETHER_TYPES->{$self->{etyp}};
	return unless $type;
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg} + 3;
	my $mask = pack('C*',0xff,0xff,0xff);
	my $oui = sprintf('%02X-%02X-%02X',unpack('C*',$self->{oui}));
	my $etyp = sprintf('%s(%04X)',ETHER_TYPES->{$self->{etyp}},$self->{etyp});
	return $self->SUPER::dissect(
		[$off+0,Frame::pbits($self->{oui},$mask),'OUI',$oui],
		[$off+3,Frame::bytes($self->{etype},0xffff,2),'Ethertype',$etyp],
	@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::SNAP::IP;  our @ISA = qw(Frame::IEEE802::LLC::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::IP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::SNAP::ARP; our @ISA = qw(Frame::IEEE802::LLC::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::ARP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::SNAP::LLDP; our @ISA = qw(Frame::IEEE802::LLC::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::LLDP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID; our @ISA = qw(Frame::IEEE802::LLC);
# -------------------------------------
use constant {
	NLPID_TYPES=>{
		0x80=>'SNAP',
		0xCC=>'IP',
	},
};
sub unpack {
	my $self = shift;
	$self->{i802} = $self->{llc};
	$self->{nlpid} = unpack('C',substr(${$self->{buf}},$self->{off},1));
	$self->{off} += 1;
	my $type = NLPID_TYPES->{$self->{nlpid}};
	return unless $type;
	if ($type eq 'SNAP') {
		bless $self,ref($self)."::$type";
		return $self->unpack(@_);
	} elsif ($type eq 'IP') {
		$self->{doff} = $self->{off};
		$self->{dend} = $self->{end};
		bless $self,ref($self)."::$type";
		return $self;
	}
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg} + 3;
	my $nlpid = sprintf('%s(%02X)',NLPID_TYPES->{$self->{nlpid}}, $self->{nlpid});
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{nlpid},0xff,1),'NLPID',$nlpid],
	@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID::IP; our @ISA = qw(Frame::IEEE802::LLC::NLPID);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::IP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID::SNAP; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	ETHER_TYPES=>{
		0x0800=>'IP',
		0x0806=>'ARP',
		0x88cc=>'LLDP',
	},
	SNAP_OUI    =>pack('CCC',0x00,0x80,0xc2),
	RFC1402_OUI =>pack('CCC',0x00,0x00,0x00),
};
sub unpack {
	my $self = shift;
	$self->{oui} = substr(${$self->{buf}},$self->{off},3);
	$self->{off} += 3;
	return unless $self->{oui} eq SNAP_OUI or $self->{oui} eq RFC1402_OUI;
	$self->{etyp} = unpack('n',substr(${$self->{buf}},$self->{off},2));
	$self->{off} += 2;
	my $type = ETHER_TYPES->{$etyp};
	return unless $type;
	$self->{doff} = $self->{off};
	$self->{dend} = $self->{end};
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg} + 4;
	my $oui = sprintf('%02X-%02X-%02X',unpack('C*',$self->{oui}));
	my $etyp = sprintf('%s(%04X)',ETHER_TYPES->{$self->{etyp}},$self->{etyp});
	return $self->SUPER::dissect(
		[$off+0,Frame::pbits($self->{oui},$mask),'OUI',$oui],
		[$off+3,Frame::bytes($self->{etype},0xffff,2),'Ethertype',$etyp],
	@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID::SNAP::IP;  our @ISA = qw(Frame::IEEE802::LLC::NLPID::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::IP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID::SNAP::LLDP; our @ISA = qw(Frame::IEEE802::LLC::NLPID::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::LLDP(@_);
}
# -------------------------------------
package Frame::IEEE802::LLC::NLPID::SNAP::ARP; our @ISA = qw(Frame::IEEE802::LLC::NLPID::SNAP);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Frame::ARP(@_);
}
# -------------------------------------
package Frame::ARP; our @ISA = qw(Frame);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $pdu = $self->{pdu};
	$self->{shwa} = $pdu->{shwa};
	$self->{dhwa} = $pdu->{dhwa} if $pdu->{dhwa};
	return unless $self->{off} + 28 <= $self->{end};
	(
		$self->{arphrd},
		$self->{arppro},
		$self->{arphln},
		$self->{arppln},
		$self->{arpop},
	) =
	unpack('nnccn',substr($$self->{buf},$self->{off},8));
	$self->{off} += 8;
	return unless $self->{arpop} == 1 or $self->{arpop} == 2;
	return unless $self->{arphdr} == 1;
	return unless $self->{arphln} == 6;
	return unless $self->{arppln} == 4;
	return unless $self->{arppro} == 0x0800;
	$self->{arpsha} = substr($$self->{buf},$self->{off},6);
	$self->{off} += 6;
	$self->{arpspa} = unpack('N',substr($$self->{buf},$self->{off},4));
	$self->{off} += 4;
	$self->{arptha} = substr($$self->{buf},$self->{off},6);
	$self->{off} += 6;
	$self->{arptpa} = unpack('N',substr($$self->{buf},$self->{off},4));
	$self->{off} += 4;
	return $self;
}
use constant {
	ARPOP_TYPE=>{
		0x1=>'Request',
		0x2=>'Reply',
	},
};
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	my $arpspa = sprintf('%d.%d.%d.%d',
		($self->{arpspa}>>24)&0xff, ($self->{arpspa}>>16)&0xff,
		($self->{arpspa}>> 8)&0xff, ($self->{arpspa}>> 0)&0xff);
	my $arptpa = sprintf('%d.%d.%d.%d',
		($self->{arptpa}>>24)&0xff, ($self->{arptpa}>>16)&0xff,
		($self->{arptpa}>> 8)&0xff, ($self->{arptpa}>> 0)&0xff);
	my $arpsha = sprintf('%02x:%02x:%02x:%02x:%02x:%02x',
		unpack('C*',$self->{arpsha}));
	my $arptha = sprintf('%02x:%02x:%02x:%02x:%02x:%02x',
		unpack('C*',$self->{arptha}));
	my $arpop = ARPOP_TYPE->{$self->{arpop}}."($self->{arpop})";
	my $mask = pack('C*',0xff,0xff,0xff,0xff,0xff,0xff);
	return 'ARP', [
		[$off+ 0,Frame::bytes($self->{arphrd},0xffff,2),'ar$hrd','Ethernet(1)',],
		[$off+ 2,Frame::bytes($self->{arppro},0xffff,2),'ar$pro','IP(0x0800)',],
		[$off+ 4,Frame::bytes($self->{arphln},0xff,1),'ar$hln',$self->{arphln},],
		[$off+ 5,Frame::bytes($self->{arppln},0xff,1),'ar$pln',$self->{arppln},],
		[$off+ 6,Frame::bytes($self->{arpop},0xffff,2),'ar$op',$arpop],
		[$off+ 8,Frame::pbits($self->{arpsha},$mask),'ar$sha',$arpsha],
		[$off+14,Frame::bytes($self->{arpspa},0xffffffff,4),$arpspa],
		[$off+18,Frame::pbits($self->{arptha},$mask),'ar$tha',$arptha],
		[$off+24,Frame::bytes($self->{arptpa},0xffffffff,4),$arptpa],
	];
}
# -------------------------------------
package Frame::LLDP; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	TAG_TYPE=>{
		0=>'End of LLDPDU',
		1=>'Chassis ID',
		2=>'Port ID',
		3=>'Time To Live',
		4=>'Port Description',
		5=>'System Name',
		6=>'System Description',
		8=>'System Capabilities',
		9=>'Management Address',
		127=>'Organizationally Specific TLV',
	},
	CHASSIS_ID_TYPE=>{
		1=>'Chassis component',
		2=>'Interface alias',
		3=>'Port component',
		4=>'MAC address',
		5=>'Network address',
		6=>'Interface name',
		7=>'Locally assigned',
	},
	PORT_ID_TYPE=>{
		1=>'Interface alias',
		2=>'Port component',
		3=>'MAC address',
		4=>'Network address',
		5=>'Interface name',
		6=>'Agent circuit ID',
		7=>'Locally assigned',
	},
	TAG_OUI=>{
		0x0080c2=>'IEEE 802.1',
		0x00120f=>'IEEE 802.3',
		0x0012bb=>'TIA',
	},
	TAG_SUBTYPE=>{
		0x0080c201=>'Port VLAN ID',
		0x0080c202=>'Port and Protocol VLAN ID',
		0x0080c203=>'VLAN Name',
		0x0080c204=>'Protocol Identity',
		0x00120f01=>'MAC/PHY Configuration/Status',
		0x00120f02=>'Power Via MDI',
		0x00120f03=>'Link Aggregation',
		0x00120f04=>'Maximum Frame Size',
		0x0012bb01=>'Media Capabilities',
		0x0012bb02=>'Network Policy',
		0x0012bb03=>'Location Identification',
		0x0012bb04=>'Extended Power-via-MDI',
		0x0012bb05=>'Inventory - Hardware Revision',
		0x0012bb06=>'Inventory - Firmware Revision',
		0x0012bb07=>'Inventory - Software Revision',
		0x0012bb08=>'Inventory - Serial Number',
		0x0012bb09=>'Inventory - Manufacturer Name',
		0x0012bb0a=>'Inventory - Model Name',
		0x0012bb0b=>'Inventory - Asset ID',
	},
	TIA_APP_TYPE=>{
		1=>'Voice',
		2=>'Voice Signalling',
		3=>'Guest Voice',
		4=>'Guest Voice Signalling',
		5=>'Softphone Voice',
		6=>'Video Conferencing',
		7=>'Streaming Video',
		8=>'Video Signalling',
	},
	TIA_LOC_TYPE=>{
		0=>'Invalid',
		1=>'Coordiante-based LCI', # RFC 3825 lat,long,alt
		2=>'Civic Address LCI', # draft-ietf-geopriv-dhcp-civil-05
		3=>'ECS ELIN', # 10-digits in NA
	},
};
sub unpack {
	my $self = shift;
	my $pdu = $self->{pdu};
	$self->{shwa} = $pdu->{shwa};
	$self->{dhwa} = $pdu->{dhwa} if $pdu->{dhwa};
	return unless $self->{off} + 2 <= $self->{end};
	my ($tl,$tag,$len);
	$tl = unpack('n',substr($$self->{buf},$self->{off},2)); $self->{off} += 2;
	($tag,$len) = (($tl>>9)&0x7f,($tl>>0)&0x1ff);
	return unless $tag == 1; # Chassis ID
	return unless $len >= 1;
	return unless $self->{off} + $len <= $self->{end};
	$self->{chidtype} = unpack('C',substr($$self->{buf},$self->{off},1)); $self->{off} += 1;
	return unless exists CHASSIS_ID_TYPE->{$self->{chidtype}};
	$self->{chid} = substr($$self->{buf},$self->{off},$len-1); $self->{off} += $len - 1;
	$tl = unpack('n',substr($$self->{buf},$self->{off},2)); $self->{off} += 2;
	($tag,$len) = (($tl>>9)&0x7f,($tl>>0)&0x1ff);
	return unless $tag == 2; # Port ID
	return unless $len >= 1;
	return unless $self->{off} + $len <= $self->{end};
	$self->{ptidtype} = unpack('C',substr($$self->{buf},$self->{off},1)); $self->{off} += 1;
	return unless exists PORT_ID_TYPE->{$self->{ptidtype}};
	$self->{ptid} = substr($$self->{buf},$self->{off},$len-1); $self->{off} += $len - 1;
	$tl = unpack('n',substr($$self->{buf},$self->{off},2)); $self->{off} += 2;
	($tag,$len) = (($tl>>9)&0x7f,($tl>>0)&0x1ff);
	return unless $tag == 3; # Time to live
	return unless $len == 2;
	return unless $self->{off} + $len <= $self->{end};
	$self->{ttl} = unpack('n',substr($$self->{buf},$self->{off},2)); $self->{off} += 2;
	for (;;) {
		$tl = unpack('n',substr($$self->{buf},$self->{off},2)); $self->{off} += 2;
		($tag,$len) = (($tl>>9)&0x7f,($tl>>0)&0x1ff);
		last if $tag == 0;
		return unless $self->{off} + $len <= $self->{end};
	}
}
# -------------------------------------
package Frame::IP; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	IP_PROTOS=>{
		132=>'SCTP',
	},
};
sub unpack {
	my $self = shift;
	my $byte;
	(
		$byte,
		$self->{dsn},
		$self->{dlen},
		$self->{ipid},
		$self->{frag},
		$self->{frago},
		$self->{ttl},
		$self->{ipproto},
		$self->{ipcsum},
		$self->{saddr},
		$self->{daddr},
	)
	= unpack('CCnnCCCCnNN',substr(${$self->{buf}},$self->{off},20));
	$self->{ipvers} = ($byte & 0xf0) >> 4;
	return if $self->{ipvers} != 4;
	$self->{iplen} = ($byte & 0x0f) << 2;
	return if $self->{iplen} < 20;
	$self->{doff} = $self->{off} + $self->{iplen};
	$self->{dend} = $self->{doff} + $self->{dlen};
	my $type = IP_PROTOS->{$self->{ipproto}};
	return unless $type;
	return $self;
}
sub decode {
	return new Frame::SCTP(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	my $saddr = sprintf('%d.%d.%d.%d',
		($self->{saddr}>>24)&0xff, ($self->{saddr}>>16)&0xff,
		($self->{saddr}>> 8)&0xff, ($self->{saddr}>> 0)&0xff);
	my $daddr = sprintf('%d.%d.%d.%d',
		($self->{daddr}>>24)&0xff, ($self->{daddr}>>16)&0xff,
		($self->{daddr}>> 8)&0xff, ($self->{daddr}>> 0)&0xff);
	my $ipproto = sprintf('%s(%d)',IP_PROTOS->{$self->{ipproto},$self->{ipproto});
	return 'IP Header', [
		[$off +  0, Frame::bytes($self->{ipvers},0xf0,1),'IP Version',$self->{ipvers}],
		[$off +  0, Frame::bytes($self->{iplen},0x0f,1),'Header Length',$self->{iplen}<<2],
		[$off +  1, Frame::bytes($self->{dsn},0xff,1),'DSN',undef],
		[$off +  2, Frame::bytes($self->{plen},0xffff,2),'Length',$self->{plen}],
		[$off +  4, Frame::bytes($self->{ipid},0xffff,2),'Id',sprintf('0x%04X',$self->{ipid})],
		[$off +  6, Frame::bytes($self->{frag},0xff,1),'Frag',undef],
		[$off +  7, Frame::bytes($self->{frago},0xff,1),'Frag Offset',$self->{frago}],
		[$off +  8, Frame::bytes($self->{ttl},0xff,1),'Time to live',$self->{ttl}],
		[$off +  9, Frame::bytes($self->{ipproto},0xff,1),'Protocol',$ipproto],
		[$off + 10, Frame::bytes($self->{ipcsum},0xffff,2),'Checksum',sprintf('0x%04X',$self->{ipcsum})],
		[$off + 12, Frame::bytes($self->{saddr},0xffffffff,4),'Srce Addr',$saddr],
		[$off + 16, Frame::bytes($self->{daddr},0xffffffff,4),'Dest Addr',$daddr],
		@_,
	];
}
# -------------------------------------
package Chunk; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	CHUNK_TYPES=>{
		::CH_DATA		=> 'DATA',
		::CH_INIT		=> 'INIT',
		::CH_INIT_ACK		=> 'INIT_ACK',
		::CH_SACK		=> 'SACK',
		::CH_BEAT		=> 'BEAT',
		::CH_BEAT_ACK		=> 'BEAT_ACK',
		::CH_ABORT		=> 'ABORT',
		::CH_SHUTDOWN		=> 'SHUTDOWN',
		::CH_SHUTDOWN_ACK	=> 'SHUTDOWN_ACK',
		::CH_ERROR		=> 'ERROR',
		::CH_COOKIE_ECHO	=> 'COOKIE_ECHO',
		::CH_COOKIE_ACK		=> 'COOKIE_ACK',
		::CH_SHUTDOWN_COMPLETE	=> 'SHUTDOWN_COMPLETE',
	}
};
sub new {
	my $type = shift;
	my $self = {};
	bless $self,$type;
	my $pdu = shift;
	(
		$self->{type},
		$self->{flags},
		$self->{clen},
	)
	= unpack('CCn',substr($$pdu->{buf},$pdu->{off},4));
	$self->{pdu} = $pdu;
	$self->{dir} = $pdu->{dir};
	$self->{hdr} = $pdu->{hdr};
	$self->{buf} = $pdu->{buf};
	my $doff = $pdu->{off} + 4;
	my $dend = $pdu->{off} + $self->{clen};
	$self->{off} = $doff;
	$self->{beg} = $doff;
	$self->{end} = $dend;
	$pdu->{off} += ($self->{clen}+3)&(~0x3);
	return unless exists CHUNK_TYPES->{$self->{type}};
	my $type = CHUNK_TYPES->{$self->{type}};
	bless $self,ref($self)."::$type";
	return $self->unpack(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $type = CHUNK_TYPES->{$self->{type}};
	return "$type Chunk", [
		[$off+ 0, Frame::bytes($self->{type}, 0xff000000,1),'Type',   $type ],
		[$off+ 0, Frame::bytes($self->{flags},0x00ff0000,1),'Flags',  sprintf('0x%02X',$self->{flags}) ],
		[$off+ 0, Frame::bytes($self->{clen}, 0x0000ffff,2),'Length', $self->{clen} ],
		@_,
	];
}
# -------------------------------------
package Chunk::DATA;		  our @ISA = qw(Chunk);
# -------------------------------------
use constant {
	PPI_TYPES=>{ 0=>'Unspecified', 2=>'M2UA', 3=>'M3UA', 4=>'SUA', 5=>'M2PA' }
};

sub unpack {
	my $self = shift;
	(
		$self->{tsn},
		$self->{stream},
		$self->{ssn},
		$self->{ppi},
	)
	= unpack('NnnN',substr(${$self->{buf}},$self->{off},12)); $self->{off} += 12;
	$self->{pdu}{ppi} = $self->{ppi};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $ppi = $self->{ppi}; $ppi = PPI_TYPES->{$ppi}."($ppi)" if exists PPI_TYPES->{$ppi};
	return $self->SUPER::dissect(
		[$off+ 4,Frame::bytes($self->{tsn},   0xffffffff,4),'TSN',   sprintf('0x%08X',$self->{tsn})],
		[$off+ 8,Frame::bytes($self->{stream},0xffff0000,4),'Stream',$self->{stream}],
		[$off+ 8,Frame::bytes($self->{ssn},   0x0000ffff,4),'SSN',   $self->{ssn}],
		[$off+12,Frame::bytes($self->{ppi},   0xffffffff,4),'PPI',   $ppi],
	@_);
}
# -------------------------------------
package Chunk::SACK;		  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	(
		$self->{cumm},
		$self->{arwnd},
		$self->{nacks},
		$self->{ndups},
	)
	= unpack('NNnn',substr(${$self->{buf}},$self->{off},12)); $self->{off} += 12;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		[$off+ 4,Frame::bytes($self->{cumm}, 0xffffffff,4),'Cumm Ack',sprintf('0x%08X',$self->{cumm})],
		[$off+ 8,Frame::bytes($self->{arwnd},0xffffffff,4),'Ad Recv Win',$self->{arwnd}],
		[$off+12,Frame::bytes($self->{nacks},0xffff0000,4),'Gap Acks',$self->{nacks}],
		[$off+12,Frame::bytes($self->{ndups},0x0000ffff,4),'Duplicates',$self->{dups}],
		#TODO: dissect the gap acks
	@_);
}
# -------------------------------------
package Chunk::SHUTDOWN;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($network,$pkt) = @_;
	$self->{cumm} = unpack('N',substr(${$self->{buf}},$self->{off},4));
	$self->{off} += 4;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		[$off+ 4,Frame::bytes($self->{cumm}, 0xffffffff,4),'Cumm Ack',sprintf('0x%08X',$self->{cumm})],
	@_);
}
# -------------------------------------
package Chunk::INIT;		  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::INIT_ACK;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::BEAT;		  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::BEAT_ACK;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::ABORT;		  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::SHUTDOWN_ACK;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::ERROR;		  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::COOKIE_ECHO;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::COOKIE_ACK;	  our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------
package Chunk::SHUTDOWN_COMPLETE; our @ISA = qw(Chunk);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{data} = substr(${$self->{buf}},$self->{off},$self->{end}-$self->{off});
	return $self;
}
# -------------------------------------

# -------------------------------------
package Frame::SCTP; our @ISA = qw(Frame);
# -------------------------------------
use constant {
	SIGTRAN_PORTS=>{
		&::PORT_M2PA => [ 'M2PA', ::PPI_M2PA ],
		&::PORT_M2UA => [ 'M2UA', ::PPI_M2UA ],
		&::PORT_M3UA => [ 'M3UA', ::PPI_M3UA ],
		&::PORT_SUA  => [ 'SUA',  ::PPI_SUA  ],
	},
	PPIS=>{
		&::PPI_M2UA => 'M2UA',
		&::PPI_M3UA => 'M3UA',
		&::PPI_SUA  => 'SUA',
		&::PPI_M2PA => 'M2PA',
	},
};
sub unpack {
	my $self = shift;
	(
		$self->{sport},
		$self->{dport},
		$self->{vtag},
		$self->{sctpcsum},
	)
	= unpack('nnNN',substr(${$self->{buf}},$self->{off},12));  $self->{off} += 12;
	$self->{chunks} = [];
	while ($self->{end} >= $self->{off} + 4) {
		my $chunk = new Chunk($self,@_);
		push @{$self->{chunks}}, $chunk if $chunk;
	}
	return $self;
}
sub testit {
	my $self = shift;
	return -1 unless $self->unpack(@_);
	my ($decoder) = @_;
	my $type = PPIS->{$self->{ppi}} if exists $self->{ppi} and exists PPIS->{$self->{ppi}};
	if (!defined $type) {
		return -1 if exists $self->{ppi} and $self->{ppi} != 0;
		if (exists SIGTRAN_PORTS->{$self->{sport}}) {
			$type = SIGTRAN_PORTS->{$self->{sport}}[0];
			$self->{ppi} = SIGTRAN_PORTS->{$self->{sport}}{1];
		}
		if (exists SIGTRAN_PORTS->{$self->{dport}}) {
			$type = SIGTRAN_PORTS->{$self->{dport}}[0];
			$self->{ppi} = SIGTRAN_PORTS->{$self->{dport}}{1];
		}
		return unless defined $type;
	}
	$decoder->setppi($self->{ppi});
	return 1;
}
sub hmmm {
	my $sub = ref($self); $sub =~ s/::/_/g;
	my $decoder = shift; $sub = $decoder->can($sub);
	return unless $sub;
	return $sub->($decoder,$self);

	my $assoc;
	if (defined $type) {
		$type = 'Association::'.$type;
		$assoc = $type->get(@_,$self);
	} else {
		$type = 'Association';
		$assoc = $type->find(@_,$self);
		unless ($assoc) {
			warn "$self: cannot determine payload type";
			return undef;
		}
		$self->{ppi} = $assoc->{ppi};
		$type = ref $assoc;
	}
	my ($network) = @_;

	my $dir = exists $assoc->objb->{ips}{$self->{saddr}} ? 1 : 0;
	$assoc->add_pkt($network,$self,$dir);

	if ($self->{shwa}) {
		Etherlink->get($network,$self->{shwa},$self->{dhwa},$self)->add_pkt($network,$self,0);
		Flow->get($network,$self->{shwa},$self->{saddr},$self)->add_pkt($network,$self,0);
	}

	if ($self->{dhwa}) {
		Etherlink->get($network,$self->{dhwa},$self->{shwa},$self)->add_pkt($network,$self,1);
		Flow->get($network,$self->{dhwa},$self->{daddr},$self)->add_pkt($network,$self,1);
	}
	return 1;
}
sub dissect {
	my $self = shift;
	my $off = $self->{sctp}{off};
	my $len = $self->{sctp}{end} - $off;
	my $sport = $self->{sport}; $sport = SIGTRAN_PORTS->{$self->{sport}}[0]."($sport)" if exists SIGTRAN_PORTS->{$self->{sport}};
	my $dport = $self->{dport}; $dport = SIGTRAN_PORTS->{$self->{dport}}[0]."($dport)" if exists SIGTRAN_PORTS->{$self->{dport}};
	my @fields = ('SCTP Packet Header', [
		[ $off + 0, Frame::bytes($self->{sport},0xffff0000,4), 'Srce Port', $sport ],
		[ $off + 0, Frame::bytes($self->{dport},0x0000ffff,4), 'Dest Port', $dport ],
		[ $off + 4, Frame::bytes($self->{vtag},0xffffffff,4), 'Verification Tag', sprintf('0x%08X',$self->{vtag})],
		[ $off + 8, Frame::bytes($self->{sctpcsum},0xffffffff,4), 'Checksum', sprintf('0x%08X',$self->{sctpcsum})],
	]);
	foreach (@{$self->{chunks}}) {
		push @fields,$_->dissect();
	}
	return @fields;
}

# -------------------------------------
package Message; our @ISA = qw(Frame);
# -------------------------------------
sub mt {
	my $self = shift;
	my $type = ref $self;
	my @fields = split(/::/,$type);
	return $fields[2];
}
sub unpack_MF {
	my ($self,$parm,$off,$len,$sub) = @_;
	return unless $off + $len <= $self->{end};
	return unless $sub = $self->can("unpack_PT_$parm");
	return $sub->($self,$off,$len);
}
sub unpack_MV {
	my ($self,$parm,$off,$ptr,$len,$sub) = @_;
	return unless $off < $self->{end};
	$ptr = unpack('C',substr($$self->{buf},$off,1)) + $off;
	return unless $ptr < $self->{end};
	$len = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
	return unless $ptr + $len <= $self->{end};
	return unless $sub = $self->can("unpack_PT_$parm");
	return $sub->($self,$ptr,$len);
}
sub unpack_OP {
	my ($self,$off,$ptr,$sub) = @_;
	return unless $off < $self->{end};
	$ptr = unpack('C',substr($$self->{buf},$off,1));
	return 1 unless $ptr; # empty list
	$ptr += $off;
	return unless $ptr < $self->{end};
	return unless $sub = $self->can('unpack_oparms');
	return $sub->($self,$ptr);
}

# -------------------------------------
package Message::ULP;  our @ISA = qw(Message);
# -------------------------------------
#package Message::ULP;
sub new {
	my $type = shift;
	my $self = {};
	bless $self,$type;
	my ($decoder,$chks) = @_;
	my $pdu = $chks->[0];
	$self->{pdus}   = $chks;
	$self->{dat}    = '';
	$self->{pdu}    = $pdu;
	$self->{hdr}    = $pdu->{hdr};
	$self->{buf}    = \$self->{dat};
        $self->{beg}    = 0;
	$self->{off}    = 0;
	$self->{end}    = 0;
	$self->{dir}    = $pdu->{dir};
	$self->{stream} = $pdu->{stream};
	$self->{ppi}    = $decoder->{ppi};
	foreach (@$chks) {
		$self->{dat} .= substr($$chk->{buf},$chk->{doff},$chk->{dlen});
		$self->{end} += $chk->{dlen};
	}
	$self->{li}     = $self->{end};
	return $self->unpack(@_);
}
use constant {
	SIGTRAN_TYPES=>{
		0=>['MGMT',{
				 1=>'ERR',
				 2=>'NTFY',
			}],
		1=>['XFER',{
				 1=>'DATA',
			}],
		2=>['SSNM',{
				 1=>'DUNA',
				 2=>'DAVA',
				 3=>'DAUD',
				 4=>'SCON',
				 5=>'DUPU',
				 6=>'DRST',
			}],
		3=>['ASPSM',{
				 1=>'ASPUP_REQ',
				 2=>'ASPDN_REQ',
				 3=>'HBEAT_REQ',
				 4=>'ASPUP_ACK',
				 5=>'ASPDN_ACK',
				 6=>'HBEAT_ACK',
			}],
		4=>['ASPTM',{
				 1=>'ASPAC_REQ',
				 2=>'ASPIA_REQ',
				 3=>'ASPAC_ACK',
				 4=>'ASPIA_ACK',
			}],
		5=>['QPTM',{
				 1=>'Data_Request',
				 2=>'Data_Indication',
				 3=>'Unit_Data_Request',
				 4=>'Unit_Data_Indication',
				 5=>'Establish_Request',
				 6=>'Establish_Confirm',
				 7=>'Establish_Indication',
				 8=>'Release_Request',
				 9=>'Release_Confirm',
				10=>'Release_Indication',
			}],
		6=>['MAUP',{
				 1=>'Data',
				 2=>'Establish_Request',
				 3=>'Establish_Confirm',
				 4=>'Release_Request',
				 5=>'Release_Confirm',
				 6=>'Release_Indication',
				 7=>'State_Request',
				 8=>'State_Confirm',
				 9=>'State_Indication',
				10=>'Data_Retrieval_Request',
				11=>'Data_Retrieval_Confirm',
				12=>'Data_Retrieval_Indication',
				13=>'Data_Retrieval_Complete_Indication',
				14=>'Congestion_Indication',
				15=>'Data_Acknowledge',
			}],
		7=>['CL',{
				 1=>'CLDT',
				 2=>'CLDR',
			}],
		8=>['CO',{
				 1=>'CORE',
				 2=>'COAK',
				 3=>'COREF',
				 4=>'RELRE',
				 5=>'RELCO',
				 6=>'RESCO',
				 7=>'RESRE',
				 8=>'CODT',
				 9=>'CODA',
				10=>'COERR',
				11=>'COIT',
			}],
		9=>['RKM',{
				 1=>'REG_REQ',
				 2=>'REG_RSP',
				 3=>'DEREG_REQ',
				 4=>'DEREG_RSP',
			}],
		10=>['IIM',{
				 1=>'REG_REQ',
				 2=>'REG_RSP',
				 3=>'DEREG_REQ',
				 4=>'DEREG_RSP',
			}],
		11=>['M2PA',{
				 1=>'DATA',
				 2=>'STATUS',
			}],
	},
};
sub unpack {
	my $self = shift;
	return if $self->{end} < $self->{off} + 8;
	(
		$self->{vers},
		$self->{resv},
		$self->{clas},
		$self->{type},
		$self->{mlen},
	) =
	unpack('CCCCN',substr(${$self->{buf}},$self->{off},8));
	return if $self->{end} < $self->{off} + $self->{mlen};
	$self->{end} = $self->{off} + (($self->{mlen}+3)&(~0x3));
	$self->{off} += 8;
	return if $self->{vers} != 1;
	return unless exists SIGTRAN_TYPES->{$self->{clas}};
	my $clas = SIGTRAN_TYPES->{$self->{clas}}[0];
	return unless exists SIGTRAN_TYPES->{$self->{clas}}[1]{$self->{type}};
	my $type = SIGTRAN_TYPES->{$self->{clas}}[1]{$self->{type}};
	bless $self,ref($self)."::$clas\::$type";
	return $self->unpack(@_);
}
use constant {
	PPI_TYPES=>{ 0=>'Unspecified', 2=>'M2UA', 3=>'M3UA', 4=>'SUA', 5=>'M2PA' }
};
sub dissect {
	my $self = shift;
	my $off = 0;
	my $prot = PPI_TYPES->{$self->{ppi}};
	my $clas = SIGTRAN_TYPES->{$self->{clas}}[0];
	my $type = SIGTRAN_TYPES->{$self->{clas}}[1]{$self->{type}};
	return [
		"$prot Header $clas\:$type",
		[$off +  0,Frame::bytes($self->{vers},0xff000000,1),'Version',"$prot($self->{vers})"],
		[$off +  0,Frame::bytes($self->{resv},0x00ff0000,1),'Reserved',$self->{resv}],
		[$off +  0,Frame::bytes($self->{clas},0x0000ff00,1),'Class',"$clas($self->{clas})"],
		[$off +  0,Frame::bytes($self->{type},0x000000ff,1),'Type',"$type($self->{type})"],
		[$off +  4,Frame::bytes($self->{mlen},0xffffffff,4),'Length',$self->{mlen}],
	];
}
use constant {
	ULP_PT_IIID	=>0x0001,
	ULP_PT_TIID	=>0x0003,
	ULP_PT_INFO	=>0x0004,
	ULP_PT_RC	=>0x0006,
	ULP_PT_DIAG	=>0x0007,
	ULP_PT_RIID	=>0x0008,
	ULP_PT_HBDATA	=>0x0009,
	ULP_PT_TMODE	=>0x000b,
	ULP_PT_ECODE	=>0x000c,
	ULP_PT_STATUS	=>0x000d,
	ULP_PT_ASPID	=>0x0011,
	ULP_PT_APC	=>0x0012,
	ULP_PT_CID	=>0x0013,
	ULP_PT_RRESULT	=>0x0014,
	ULP_PT_DRESULT	=>0x0015,
	ULP_PT_RSTATUS	=>0x0016,
	ULP_PT_DSTATUS	=>0x0017,
	ULP_PT_KEYID	=>0x0018,

	SUA_PT_HOPC	=>0x0101,
	SUA_PT_SA	=>0x0102,
	SUA_PT_DA	=>0x0103,
	SUA_PT_SRN	=>0x0104,
	SUA_PT_DRN	=>0x0105,
	SUA_PT_CAUSE	=>0x0106,
	SUA_PT_SEQNO	=>0x0107,
	SUA_PT_RSEQNO	=>0x0108,
	SUA_PT_ASPCAP	=>0x0109,
	SUA_PT_CREDIT	=>0x010a,
	SUA_PT_DATA	=>0x010b,
	SUA_PT_UCAUSE	=>0x010c,
	SUA_PT_NA	=>0x010d,
	SUA_PT_RKEY	=>0x010e,
	SUA_PT_DRNLAB	=>0x010f,
	SUA_PT_TIDLAB	=>0x0110,
	SUA_PT_RANGE	=>0x0111,
	SUA_PT_SMI	=>0x0112,
	SUA_PT_IMP	=>0x0113,
	SUA_PT_PRIO	=>0x0114,
	SUA_PT_PCLASS	=>0x0115,
	SUA_PT_SEQCTL	=>0x0116,
	SUA_PT_SEGM	=>0x0117,
	SUA_PT_CONG	=>0x0118,

	SUA_PT_GTI	=>0x8001,
	SUA_PT_PC	=>0x8002,
	SUA_PT_SSN	=>0x8003,
	SUA_PT_IPV4	=>0x8004,
	SUA_PT_HOST	=>0x8005,
	SUA_PT_IPV6	=>0x8006,

	M3UA_PT_NA	=>0x0200,
	M3UA_PT_UCAUSE	=>0x0204,
	M3UA_PT_CONG	=>0x0205,
	M3UA_PT_CDEST	=>0x0206,
	M3UA_PT_RKEY	=>0x0207,
	M3UA_PT_RRESULT	=>0x0208,
	M3UA_PT_DRESULT	=>0x0209,
	M3UA_PT_KEYID	=>0x020a,
	M3UA_PT_DPC	=>0x020b,
	M3UA_PT_SI	=>0x020c,
	M3UA_PT_OPCLIST	=>0x020e,
	M3UA_PT_DATA	=>0x0210,
	M3UA_PT_RSTATUS	=>0x0212,
	M3UA_PT_DSTATUS	=>0x0213,

	M2UA_PT_PDATA1	=>0x0300,
	M2UA_PT_PDATA2	=>0x0301,
	M2UA_PT_STATE	=>0x0302,
	M2UA_PT_EVENT	=>0x0303,
	M2UA_PT_CONGEST	=>0x0304,
	M2UA_PT_DISCARD	=>0x0305,
	M2UA_PT_ACTION	=>0x0306,
	M2UA_PT_SEQNUM	=>0x0307,
	M2UA_PT_RTVRES	=>0x0308,
	M2UA_PT_LKEY	=>0x0309,
	M2UA_PT_KEYID	=>0x030a,
	M2UA_PT_SDTI	=>0x030b,
	M2UA_PT_SDLI	=>0x030c,
	M2UA_PT_RRESULT	=>0x030d,
	M2UA_PT_RSTATUS	=>0x030e,
	M2UA_PT_DRESULT	=>0x030f,
	M2UA_PT_DSTATUS	=>0x0310,
};

sub unpack_ULP_UCAUS {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	($self->{$lab}{caus},$self->{$lab}{user}) = unpack('nn',${$self->{buf}},$off,4);
}
sub unpack_ULP_LABEL {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	($self->{$lab}{beg},$self->{$lab}{end},$self->{$lab}{val}) = unpack('CCn',${$self->{buf}},$off,4);
}
sub unpack_ULP_PDATA {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{$lab} = substr(${$self->{buf}},$off,$len);
}
sub unpack_ULP_U8BIT {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	$self->{$lab} = unpack('N',${$self->{buf}},$off,4) & 0xff;
}
sub unpack_ULP_32BIT {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	$self->{$lab} = unpack('N',${$self->{buf}},$off,4);
}
sub unpack_ULP_32LST {
	my ($self,$off,$end,$len,$lab) = @_;
	while ($end >= $off + 4) {
		my $val = unpack('N',substr(${$self->{buf}},$off,4));
		push @{$self->{$lab.'s'}},$val if $val;
		$off += 4;
	}
}
sub unpack_ULP_PCODE {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	$self->{$lab} = unpack('N',substr($$self->{buf},$off,4)) &  0xffffff;
}
sub unpack_ULP_PCLST {
	my ($self,$off,$end,$len,$lab) = @_;
	while ($end >= $off + 4) {
		my $val = unpack('N',substr($$self->{buf},$off,4));
		my $spc = ($val >>  0) & 0xffffff;
		my $msk = ($val >> 24) & 0xff;
		push @{$self->{$lab.'s'}},[$msk,$val];
		$off += 4;
	}
}
sub unpack_ULP_STRNG {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	$self->{$lab} = substr(${$self->{buf}},$off,$len);
}
sub unpack_ULP_CMPND {
	my ($self,$off,$end,$len,$lab) = @_;
	my $cmpnd = { off=>0, end=>$len, dat=>substr(${$self->{buf}},$off,$len) };
	return unless Message::ULP::unpack_parms($cmpnd,@_);
	push @{$self->{$lab.'s'}}, $cmpnd;
}

sub unpack_ULP_ADDR {
	my ($self,$off,$end,$len,$lab) = @_;
	my $addr = { off=>4, end=>$len, dat=>substr(${$self->{buf}},$off,$len) };
	(
		$addr->{ri},
		$addr->{ai},
	) =
	unpack('nn',substr(${$self->{buf}},$off,4));
	return unless Message::ULP::unpack_parms($addr,@_);
	$self->{$lab} = $addr;
}
sub unpack_ULP_PT_IIID {
	my ($self,$off,$end,$len,$lab) = @_;
	while ($end >= $off + 4) {
		$self->{ctx} = substr(${$self->{buf}},$off,4);
		$self->{$lab} = unpack('N',$self->{ctx});
		push @{$self->{ctxs}}, $self->{ctx};
		push @{$self->{$lab.'s'}}, $self->{$lab};
		$off += 4;
	}
}
sub unpack_ULP_PT_TIID {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{ctx} = substr(${$self->{buf}},$off,$len);
	$self->{$lab} = $self->{ctx};
	push @{$self->{ctxs}}, $self->{ctx};
	push @{$self->{$lab.'s'}}, $self->{$lab};
}
sub unpack_ULP_PT_INFO		{ return shift->Message::ULP::unpack_ULP_PDATA(@_) }
sub unpack_ULP_PT_RC {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	while ($end >= $off + 4) {
		$self->{ctx} = substr(${$self->{buf}},$off,4);
		$self->{$lab} = unpack('N',$self->{ctx});
		push @{$self->{ctxs}}, $self->{ctx};
		push @{$self->{$lab.'s'}}, $self->{$lab};
		$off += 4;
	}
}
sub unpack_ULP_PT_DIAG		{ return shift->Message::ULP::unpack_ULP_PDATA(@_) }
sub unpack_ULP_PT_RIID {
	my ($self,$off,$end,$len,$lab) = @_;
	while ($end >= $off + 8) {
		my ($beg,$fin) = unpack('NN',substr(${$self->{buf}},$off,8));
		for (my $i = $beg; $i <= $fin; $i++) {
			$self->{ctx} = pack('N',$i);
			$self->{$lab} = $i;
			push @{$self->{ctxs}}, $self->{ctx};
			push @{$self->{$lab.'s'}}, $self->{$lab};
		}
		$off += 8;
	}
}
sub unpack_ULP_PT_HBDATA	{ return shift->Message::ULP::unpack_ULP_PDATA(@_) }
sub unpack_ULP_PT_TMODE		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_ECODE		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_STATUS {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	(
		$self->{$lab}{type},
		$self->{$lab}{info},
	) =
	unpack('nn',${$self->{buf}},$off,4);
}
sub unpack_ULP_PT_ASPID		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_APC		{ return shift->Message::ULP::unpack_ULP_PCLST(@_) }
sub unpack_ULP_PT_CID		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_RRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_ULP_PT_DRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_ULP_PT_RSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_DSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_ULP_PT_KEYID		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_HOPC		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_SA		{ return shift->Message::ULP::unpack_ULP_ADDR(@_) }
sub unpack_SUA_PT_DA		{ return shift->Message::ULP::unpack_ULP_ADDR(@_) }
sub unpack_SUA_PT_SRN		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_DRN		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_CAUSE {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	(
		$self->{$lab}{type},
		$self->{$lab}{valu},
	) =
	unpack('nn',${$self->{buf}},$off,4);
}
sub unpack_SUA_PT_SEQNO {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	$self->{$lab}{rseqno} = ($val >> 9)&0x7f;
	$self->{$lab}{sseqno} = ($val >> 0)&0xff;
	$self->{$lab}{m}      = ($val >> 8)&0x01;
}
sub unpack_SUA_PT_RSEQNO	{ return shift->Message::ULP::unpack_ULP_U8BIT(@_) }
sub unpack_SUA_PT_ASPCAP {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	($self->{$lab}{spare},$self->{$lab}{iwf},$self->{$lab}{iw}) = unpack('nCC',substr(${$self->{buf}},$off,4));
}
sub unpack_SUA_PT_CREDIT	{ return shift->Message::ULP::unpack_ULP_U8BIT(@_) }
sub unpack_SUA_PT_DATA {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{doff} = $off;
	$self->{dlen} = $len;
	$self->{dend} = $off + $len;
}
sub unpack_SUA_PT_UCAUSE	{ return shift->Message::ULP::unpack_ULP_UCAUS(@_) }
sub unpack_SUA_PT_NA		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_RKEY		{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_SUA_PT_DRNLAB	{ return shift->Message::ULP::unpack_ULP_LABEL(@_) }
sub unpack_SUA_PT_TIDLAB	{ return shift->Message::ULP::unpack_ULP_LABEL(@_) }
sub unpack_SUA_PT_RANGE		{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_SUA_PT_SMI		{ return shift->Message::ULP::unpack_ULP_U8BIT(@_) }
sub unpack_SUA_PT_IMP		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_PRIO		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_PCLASS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_SEQCTL	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_SEGM {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 4;
	$self->{$lab}{fr}     = unpack('C',substr(${$self->{buf}},$off,1));
	$self->{$lab}{segref} = unpack('N',substr(${$self->{buf}},$off,4)) & 0xffffff;
}
sub unpack_SUA_PT_CONG		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_GTI {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 8;
	(
		$self->{gti},
		$self->{num},
		$self->{tt},
		$self->{np},
		$self->{nai},
	) =
	unpack('NCCCC',substr(${$self->{buf}},$off,8));
	$self->{digs} = substr(${$self->{buf}},$off+8,$len-8);
}
sub unpack_SUA_PT_PC		{ return shift->Message::ULP::unpack_ULP_PCODE(@_) }
sub unpack_SUA_PT_SSN		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_IPV4		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_SUA_PT_HOST		{ return shift->Message::ULP::unpack_ULP_STRNG(@_) }
sub unpack_SUA_PT_IPV6		{ return shift->Message::ULP::unpack_ULP_STRNG(@_) }
sub unpack_M3UA_PT_NA		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M3UA_PT_UCAUSE	{ return shift->Message::ULP::unpack_ULP_UCAUS(@_) }
sub unpack_M3UA_PT_CONG		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M3UA_PT_CDEST	{ return shift->Message::ULP::unpack_ULP_PCODE(@_) }
sub unpack_M3UA_PT_RKEY		{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M3UA_PT_RRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M3UA_PT_DRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M3UA_PT_KEYID	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M3UA_PT_DPC		{ return shift->Message::ULP::unpack_ULP_PCODE(@_) }
sub unpack_M3UA_PT_SI {
	my ($self,$off,$end,$len,$lab) = @_;
	while ($end > $off) {
		my $val = unpack('C',substr(${$self->{buf}},$off,1));
		push @{$self->{$lab.'s'}},$val if $val;
		$off++;
	}
}
sub unpack_M3UA_PT_OPCLIST	{ return shift->Message::ULP::unpack_ULP_32LST(@_) }
sub unpack_M3UA_PT_DATA {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len >= 12;
	(
		$self->{opc},
		$self->{dpc},
		$self->{si},
		$self->{ni},
		$self->{mp},
		$self->{sls},
	) =
	unpack('NNCCCC',substr(${$self->{buf}},$off,12));
	$self->{doff} = $off + 12;
	$self->{dlen} = $len - 12;
	$self->{dend} = $off + $len;
	$self->{li} = $self->{dlen};
}
sub unpack_M3UA_PT_RSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M3UA_PT_DSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_PDATA1 {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{doff} = $off;
	$self->{dlen} = $len;
	$self->{dend} = $off + $len;
	$self->{li} = $self->{dlen};
}
sub unpack_M2UA_PT_PDATA2 {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{doff} = $off + 1;
	$self->{dlen} = $len - 1;
	$self->{dend} = $off + $len;
	$self->{li} = $self->{dlen};
	$self->{li0} = unpack('C',substr(${$self->{buf}},$off,1)) >> 6;
	$self->{mp} = $self->{li0};
	$self->{rt} = ::RT_24BIT_PC;
	$self->{pr} = ::MP_JAPAN;
}
sub unpack_M2UA_PT_STATE	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_EVENT	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_CONGEST	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_DISCARD	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_ACTION	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_SEQNUM	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_RTVRES	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_LKEY		{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M2UA_PT_KEYID	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_SDTI		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_SDLI		{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_RRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M2UA_PT_RSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }
sub unpack_M2UA_PT_DRESULT	{ return shift->Message::ULP::unpack_ULP_CMPND(@_) }
sub unpack_M2UA_PT_DSTATUS	{ return shift->Message::ULP::unpack_ULP_32BIT(@_) }

use constant {
	ULP_PARMS_UNPACK=>{
		&ULP_PT_IIID	=> [\&Message::ULP::unpack_ULP_PT_IIID,	   'iiid'   ],
		&ULP_PT_TIID	=> [\&Message::ULP::unpack_ULP_PT_TIID,	   'tiid'   ],
		&ULP_PT_INFO	=> [\&Message::ULP::unpack_ULP_PT_INFO,	   'info'   ],
		&ULP_PT_RC	=> [\&Message::ULP::unpack_ULP_PT_RC,	   'rc'	    ],
		&ULP_PT_DIAG	=> [\&Message::ULP::unpack_ULP_PT_DIAG,	   'diag'   ],
		&ULP_PT_RIID	=> [\&Message::ULP::unpack_ULP_PT_RIID,	   'riid'   ],
		&ULP_PT_HBDATA	=> [\&Message::ULP::unpack_ULP_PT_HBDATA,  'hbdata' ],
		&ULP_PT_TMODE	=> [\&Message::ULP::unpack_ULP_PT_TMODE,   'tmode'  ],
		&ULP_PT_ECODE	=> [\&Message::ULP::unpack_ULP_PT_ECODE,   'ecode'  ],
		&ULP_PT_STATUS	=> [\&Message::ULP::unpack_ULP_PT_STATUS,  'status' ],
		&ULP_PT_ASPID	=> [\&Message::ULP::unpack_ULP_PT_ASPID,   'aspid'  ],
		&ULP_PT_APC	=> [\&Message::ULP::unpack_ULP_PT_APC,	   'apc'    ],
		&ULP_PT_CID	=> [\&Message::ULP::unpack_ULP_PT_CID,	   'cid'    ],
		&ULP_PT_RRESULT	=> [\&Message::ULP::unpack_ULP_PT_RRESULT, 'rresult'],
		&ULP_PT_DRESULT	=> [\&Message::ULP::unpack_ULP_PT_DRESULT, 'dresult'],
		&ULP_PT_RSTATUS	=> [\&Message::ULP::unpack_ULP_PT_RSTATUS, 'rstatus'],
		&ULP_PT_DSTATUS	=> [\&Message::ULP::unpack_ULP_PT_DSTATUS, 'dstatus'],
		&ULP_PT_KEYID	=> [\&Message::ULP::unpack_ULP_PT_KEYID,   'keyid'  ],
		&SUA_PT_HOPC	=> [\&Message::ULP::unpack_SUA_PT_HOPC,	   'hopc'   ],
		&SUA_PT_SA	=> [\&Message::ULP::unpack_SUA_PT_SA,	   'sa'	    ],
		&SUA_PT_DA	=> [\&Message::ULP::unpack_SUA_PT_DA,	   'da'	    ],
		&SUA_PT_SRN	=> [\&Message::ULP::unpack_SUA_PT_SRN,	   'srn'    ],
		&SUA_PT_DRN	=> [\&Message::ULP::unpack_SUA_PT_DRN,	   'drn'    ],
		&SUA_PT_CAUSE	=> [\&Message::ULP::unpack_SUA_PT_CAUSE,   'cause'  ],
		&SUA_PT_SEQNO	=> [\&Message::ULP::unpack_SUA_PT_SEQNO,   'seqno'  ],
		&SUA_PT_RSEQNO	=> [\&Message::ULP::unpack_SUA_PT_RSEQNO,  'rseqno' ],
		&SUA_PT_ASPCAP	=> [\&Message::ULP::unpack_SUA_PT_ASPCAP,  'aspcap' ],
		&SUA_PT_CREDIT	=> [\&Message::ULP::unpack_SUA_PT_CREDIT,  'credit' ],
		&SUA_PT_DATA	=> [\&Message::ULP::unpack_SUA_PT_DATA,	   'data'   ],
		&SUA_PT_UCAUSE	=> [\&Message::ULP::unpack_SUA_PT_UCAUSE,  'ucause' ],
		&SUA_PT_NA	=> [\&Message::ULP::unpack_SUA_PT_NA,	   'na'	    ],
		&SUA_PT_RKEY	=> [\&Message::ULP::unpack_SUA_PT_RKEY,	   'rkey'   ],
		&SUA_PT_DRNLAB	=> [\&Message::ULP::unpack_SUA_PT_DRNLAB,  'drnlab' ],
		&SUA_PT_TIDLAB	=> [\&Message::ULP::unpack_SUA_PT_TIDLAB,  'tidlab' ],
		&SUA_PT_RANGE	=> [\&Message::ULP::unpack_SUA_PT_RANGE,   'range'  ],
		&SUA_PT_SMI	=> [\&Message::ULP::unpack_SUA_PT_SMI,	   'smi'    ],
		&SUA_PT_IMP	=> [\&Message::ULP::unpack_SUA_PT_IMP,	   'imp'    ],
		&SUA_PT_PRIO	=> [\&Message::ULP::unpack_SUA_PT_PRIO,	   'prio'   ],
		&SUA_PT_PCLASS	=> [\&Message::ULP::unpack_SUA_PT_PCLASS,  'pclass' ],
		&SUA_PT_SEQCTL	=> [\&Message::ULP::unpack_SUA_PT_SEQCTL,  'seqctl' ],
		&SUA_PT_SEGM	=> [\&Message::ULP::unpack_SUA_PT_SEGM,	   'segm'   ],
		&SUA_PT_CONG	=> [\&Message::ULP::unpack_SUA_PT_CONG,	   'cong'   ],
		&SUA_PT_GTI	=> [\&Message::ULP::unpack_SUA_PT_GTI,	   'gti'    ],
		&SUA_PT_PC	=> [\&Message::ULP::unpack_SUA_PT_PC,	   'pc'	    ],
		&SUA_PT_SSN	=> [\&Message::ULP::unpack_SUA_PT_SSN,	   'ssn'    ],
		&SUA_PT_IPV4	=> [\&Message::ULP::unpack_SUA_PT_IPV4,	   'ipv4'   ],
		&SUA_PT_HOST	=> [\&Message::ULP::unpack_SUA_PT_HOST,	   'host'   ],
		&SUA_PT_IPV6	=> [\&Message::ULP::unpack_SUA_PT_IPV6,	   'ipv6'   ],
		&M3UA_PT_NA	=> [\&Message::ULP::unpack_M3UA_PT_NA,	   'na'	    ],
		&M3UA_PT_UCAUSE	=> [\&Message::ULP::unpack_M3UA_PT_UCAUSE, 'ucause' ],
		&M3UA_PT_CONG	=> [\&Message::ULP::unpack_M3UA_PT_CONG,   'cong'   ],
		&M3UA_PT_CDEST	=> [\&Message::ULP::unpack_M3UA_PT_CDEST,  'cdest'  ],
		&M3UA_PT_RKEY	=> [\&Message::ULP::unpack_M3UA_PT_RKEY,   'rkey'   ],
		&M3UA_PT_RRESULT=> [\&Message::ULP::unpack_M3UA_PT_RRESULT,'rresult'],
		&M3UA_PT_DRESULT=> [\&Message::ULP::unpack_M3UA_PT_DRESULT,'dresult'],
		&M3UA_PT_KEYID	=> [\&Message::ULP::unpack_M3UA_PT_KEYID,  'keyid'  ],
		&M3UA_PT_DPC	=> [\&Message::ULP::unpack_M3UA_PT_DPC,	   'dpc'    ],
		&M3UA_PT_SI	=> [\&Message::ULP::unpack_M3UA_PT_SI,	   'si'	    ],
		&M3UA_PT_OPCLIST=> [\&Message::ULP::unpack_M3UA_PT_OPCLIST,'opclist'],
		&M3UA_PT_DATA	=> [\&Message::ULP::unpack_M3UA_PT_DATA,   'data'   ],
		&M3UA_PT_RSTATUS=> [\&Message::ULP::unpack_M3UA_PT_RSTATUS,'rstatus'],
		&M3UA_PT_DSTATUS=> [\&Message::ULP::unpack_M3UA_PT_DSTATUS,'dstatus'],
		&M2UA_PT_PDATA1	=> [\&Message::ULP::unpack_M2UA_PT_PDATA1, 'pdata1' ],
		&M2UA_PT_PDATA2	=> [\&Message::ULP::unpack_M2UA_PT_PDATA2, 'pdata2' ],
		&M2UA_PT_STATE	=> [\&Message::ULP::unpack_M2UA_PT_STATE,  'state'  ],
		&M2UA_PT_EVENT	=> [\&Message::ULP::unpack_M2UA_PT_EVENT,  'event'  ],
		&M2UA_PT_CONGEST=> [\&Message::ULP::unpack_M2UA_PT_CONGEST,'congest'],
		&M2UA_PT_DISCARD=> [\&Message::ULP::unpack_M2UA_PT_DISCARD,'discard'],
		&M2UA_PT_ACTION	=> [\&Message::ULP::unpack_M2UA_PT_ACTION, 'action' ],
		&M2UA_PT_SEQNUM	=> [\&Message::ULP::unpack_M2UA_PT_SEQNUM, 'seqnum' ],
		&M2UA_PT_RTVRES	=> [\&Message::ULP::unpack_M2UA_PT_RTVRES, 'rtvres' ],
		&M2UA_PT_LKEY	=> [\&Message::ULP::unpack_M2UA_PT_LKEY,   'lkey'   ],
		&M2UA_PT_KEYID	=> [\&Message::ULP::unpack_M2UA_PT_KEYID,  'keyid'  ],
		&M2UA_PT_SDTI	=> [\&Message::ULP::unpack_M2UA_PT_SDTI,   'sdti'   ],
		&M2UA_PT_SDLI	=> [\&Message::ULP::unpack_M2UA_PT_SDLI,   'sdli'   ],
		&M2UA_PT_RRESULT=> [\&Message::ULP::unpack_M2UA_PT_RRESULT,'rresult'],
		&M2UA_PT_RSTATUS=> [\&Message::ULP::unpack_M2UA_PT_RSTATUS,'rstatus'],
		&M2UA_PT_DRESULT=> [\&Message::ULP::unpack_M2UA_PT_DRESULT,'dresult'],
		&M2UA_PT_DSTATUS=> [\&Message::ULP::unpack_M2UA_PT_DSTATUS,'dstatus'],
	},
};

sub unpack_parms {
	my $self = shift;
	while ($self->{off} + 4 <= $self->{end}) {
		my ($tag,$len) = unpack('nn',substr(${$self->{buf}},$self->{off},4));
		my $off = $self->{off} + 4;
		my $end = $self->{off} + (($len+3)&(~0x3));
		return if $end > $self->{end};
		return unless $len >= 4;
		if (exists ULP_PARMS_UNPACK->{$tag}) {
			my @args = @{ULP_PARMS_UNPACK->{$tag}};
			my $sub = shift @args; $sub->($self,$off,$end,$len-4,@args,@_);
		}
		$self->{off} = $end;
	}
	return if $self->{len} < $self->{off};
	return 1;
}

sub dissect_ULP_PT_IIID {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end >= $off + 4) {
		my $val = unpack('N',${$self->{buf}},$off,4);
		push @fields, [$off,Frame::bytes($val,0xffffffff,4),'Int IID',$val];
	}
	return \@fields;
}
sub dissect_ULP_PT_TIID {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','Text IID',Frame::ptext(substr(${$self->{buf}},$off,$len))], ];
}
sub dissect_ULP_PT_INFO {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','INFO',Frame::ptext(substr(${$self->{buf}},$off,$len))], ];
}
sub dissect_ULP_PT_RC {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end >= $off + 4) {
		my $val = unpack('N',${$self->{buf}},$off,4);
		push @fields, [$off,Frame::bytes($val,0xffffffff,4),'RC',$val];
	}
	return \@fields;
}
sub dissect_ULP_PT_DIAG {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','DIAG',Frame::ptext(substr(${$self->{buf}},$off,$len))], ];
}
sub dissect_ULP_PT_RIID {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end >= $off + 8) {
		my ($beg,$fin) = unpack('NN',substr(${$self->{buf}},$off,8));
		push @fields,
			[$off+0,Frame::bytes($beg,0xffffffff,4),'IID Start',$beg],
			[$off+4,Frame::bytes($fin,0xffffffff,4),'IID Stop',$fin];
	}
	return \@fields;
}
sub dissect_ULP_PT_HBDATA {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','HBDATA',Frame::pdata(substr(${$self->{buf}},$off,$len))], ];
}
use constant { TMODE_TYPE=>{ 1=>'Override', 2=>'Loadshare', 3=>'Broadcast' } };
sub dissect_ULP_PT_TMODE {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $tmode = $val; $tmode = TMODE_TYPE->{$val}."($val)" if exists TMODE_TYPE->{$val};
	return [ [$off,Frame::bytes($val,0xffffffff,4),'TMODE',$tmode] ];
}
use constant {
	ECODE_TYPE=>{
		0x01=>'Invalid Version',
		0x02=>'Invalid Interface Identifier',
		0x03=>'Unsupported Message Class',
		0x04=>'Unsupported Message Type',
		0x05=>'Unsupported Traffic Mode Type',
		0x06=>'Unexpected Message',
		0x07=>'Protocol Error',
		0x08=>'Unsupported Interface Identifier Type',
		0x09=>'Invalid Stream Identifier',
#		0x0a=>'Unassigned TEI',			# IUA
#		0x0b=>'Unrecognized SAPI',		# IUA
#		0x0c=>'Invalid TEI, SAPI Combination',	# IUA
		0x0d=>'Refused - Management Blocking',
		0x0e=>'ASP Identifier Required',
		0x0f=>'Invalid ASP Identifier',
		0x10=>'ASP Active for Interface Identifiers',
		0x11=>'Invalid Parameter Value',
		0x12=>'Parameter Field Error',
		0x13=>'Unexpected Parameter',
		0x14=>'Destination Status Unknown',
		0x15=>'Invalid Network Appearance',
		0x16=>'Missing Parameter',
#		0x17=>'',
#		0x18=>'',
		0x19=>'Invalid Routing Context',
		0x1a=>'No Configured AS for ASP',
		0x1b=>'Subsystem Status Unknown',
		0x1c=>'Invalid Loadsharing Label',
#		0x1c=>'Channel Number out of range',	# DUA
#		0x1d=>'Channel Number not configured',	# DUA
	}
};
sub dissect_ULP_PT_ECODE {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $ecode = $val; $ecode = "$val ('".ECODE_TYPE->{$val}."')" if exists ECODE_TYPE->{$val};
	return [ [$off,Frame::bytes($val,0xffffffff,4),'ECODE',$ecode] ];
}
use constant {
	STATUS_TYPES=>{
		1=>'AS-State_Change',
		2=>'Other',
	},
	STATUS_INFOS=>{
		0x00010001=>'Reserved',
		0x00010002=>'AS-INACTIVE',
		0x00010003=>'AS-ACTIVE',
		0x00010004=>'AS-PENDING',
		0x00020001=>'Insufficient ASPs Active in AS',
		0x00020002=>'Alternate ASP Active',
		0x00020003=>'ASP Failure',
	},
};
sub dissect_ULP_PT_STATUS {
	my ($self,$off,$end,$len) = @_;
	my ($val1,$val2) = unpack('nn',${$self->{buf}},$off,4);
	my $val0 = unpack('N',${$self->{buf}},$off,4);
	my $stype = $val1; $stype = $style." ('".STATUS_TYPES->{$val1}."')" if exists STATUS_TYPES->{$val1};
	my $sinfo = $val2; $sinfo = $sinfo." ('".STATUS_INFOS->{$val0}."')" if exists STATUS_INFOS->{$val0};
	return [
		[$off,Frame::bytes($val1,0xffff0000,4),'Status Type',$stype],
		[$off,Frame::bytes($val2,0x0000ffff,4),'Status Info',$sinfo],
	];
}
sub dissect_ULP_PT_ASPID {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'ASPID',$val] ];
}
sub dissect_ULP_PT_APC {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end >= $off + 4) {
		my $spa = unpack('C',substr(${$self->{buf}},$off,1));
		my $val = unpack('N',substr(${$self->{buf}},$off,4)) & 0xffffff;
		my $apc = $val;
		if ($val&(~0x3fff)) {
			$apc = (($apc>>16)&0xff).'-'.(($apc>>8)&0xff).'-'.(($apc>>0)&0xff);
			push @fields,
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::bytes($val,0x00ffffff,4),'APC',  $apc];
		} else {
			$apc = (($apc>>11)&0x07).'-'.(($apc>>3)&0xff).'-'.(($apc>>0)&0x07);
			my $apc0 = ($apc >> 14) & 0x3ff;
			push @fields,
			[$off,Frame::spare($spa, 0xff000000,4),'Spare', $spa],
			[$off,Frame::spare($apc0,0x00ffc000,4),'Unused',$apc0],
			[$off,Frame::bytes($val, 0x00003fff,4),'APC',   $apc];
		}
	}
	return \@fields;
}
sub dissect_ULP_PT_CID {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'CORID',$val] ];
}
sub dissect_ULP_PT_RRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_ULP_PT_DRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_ULP_PT_RSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Reg Status',$val] ];
}
sub dissect_ULP_PT_DSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Dereg Status',$val] ];
}
sub dissect_ULP_PT_KEYID {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Key ID',$val] ];
}
sub dissect_SUA_PT_HOPC {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Hop Counter',$val] ];
}
sub dissect_SUA_PT_SA {
	my ($self,$off,$end,$len) = @_;
	my ($val1,$val2) = unpack('nn',${$self->{buf}},$off,4);
	return [
		[$off,Frame::bytes($val1,0xffff0000,4),'RI',$val1],
		[$off,Frame::bytes($val2,0x0000ffff,4),'AI',$val2],
		@{Message::ULP::dissect_parms($self,$off+4,$end,$len-4)},
	];
}
sub dissect_SUA_PT_DA {
	my ($self,$off,$end,$len) = @_;
	my ($val1,$val2) = unpack('nn',${$self->{buf}},$off,4);
	return [
		[$off,Frame::bytes($val1,0xffff0000,4),'RI',$val1],
		[$off,Frame::bytes($val2,0x0000ffff,4),'AI',$val2],
		@{Message::ULP::dissect_parms($self,$off+4,$end,$len-4)},
	];
}
sub dissect_SUA_PT_SRN {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'SRN',$val] ];
}
sub dissect_SUA_PT_DRN {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'DRN',$val] ];
}
sub dissect_SUA_PT_CAUSE {
	my ($self,$off,$end,$len) = @_;
	my ($val1,$val2) = unpack('nn',substr(${$self->{buf}},$off,4));
	return [
		[$off,Frame::bytes($val1,0xffff0000,4),'Cause Type',$val1],
		[$off,Frame::bytes($val2,0x0000ffff,4),'Cause Value',$val2],
	];
}
sub dissect_SUA_PT_SEQNO {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	my $spare  = ($val >> 16);
	my $rseqno = ($val >>  9)&0x7f;
	my $sseqno = ($val >>  0)&0xff;
	my $m      = ($val >>  8)&0x01;
	return [
		[$off,Frame::spare($spare, 0xffff0000,4),'Spare',$spare],
		[$off,Frame::bytes($rseqno,0x0000fe00,4),'Recv Seq No',$rseqno],
		[$off,Frame::bytes($m,     0x00000100,4),'M',$m],
		[$off,Frame::bytes($sseqno,0x000000ff,4),'Send Seq No',$sseqno],
	];
}
sub dissect_SUA_PT_RSEQNO {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	my $spare = $val >> 8;
	my $rseqno = $val & 0xff;
	return [
		[$off,Frame::spare($spare, 0xffffff00,4),'Spare',$spare],
		[$off,Frame::bytes($rseqno,0x000000ff,4),'Recv Seq No',$rseqno],
	];
}
sub dissect_SUA_PT_ASPCAP {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	$iw  = ($val >> 0) & 0xff;
	$iwf = ($val >> 8) & 0xff;
	$spa = ($val >>16);
	return [
		[$off,Frame::spare($spa,0xffff0000,4),'Spare',$spa],
		[$off,Frame::bytes($iwf,0x0000ff00,4),'I/W Flags',$iwf],
		[$off,Frame::bytes($iw, 0x000000ff,4),'I/W',$iw],
	];
}
sub dissect_SUA_PT_CREDIT {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spare = $val >> 8;
	my $credit = $val & 0xff;
	return [
		[$off,Frame::spare($spare, 0xffffff00,4),'Spare',$spare],
		[$off,Frame::bytes($credit,0x000000ff,4),'Credit',$credit],
	];
}
sub dissect_SUA_PT_DATA {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','Data',Frame::pdata(substr(${$self->{buf}},$off,$len))] ];
}
sub dissect_SUA_PT_UCAUSE {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $caus = ($val >> 16) & 0xffff;
	my $user = ($val >>  0) & 0xffff;
	return [
		[$off,Frame::bytes($caus,0xffff0000,4),'Cause',$caus],
		[$off,Frame::bytes($user,0x0000ffff,4),'User',$user],
	];
}
sub dissect_SUA_PT_NA {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'NA',$val] ];
}
sub dissect_SUA_PT_RKEY {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_SUA_PT_DRNLAB {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $b = ($val >> 24) & 0xff;
	my $e = ($val >> 16) & 0xff;
	my $v = ($val >>  0) & 0xffff;
	return [
		[$off,Frame::bytes($b,0xff000000,4),'DRN Begin',$b],
		[$off,Frame::bytes($e,0x00ff0000,4),'DRN End',$e],
		[$off,Frame::bytes($v,0x0000ffff,4),'DRN Value',$v],
	];
}
sub dissect_SUA_PT_TIDLAB {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $b = ($val >> 24) & 0xff;
	my $e = ($val >> 16) & 0xff;
	my $v = ($val >>  0) & 0xffff;
	return [
		[$off,Frame::bytes($b,0xff000000,4),'TID Begin',$b],
		[$off,Frame::bytes($e,0x00ff0000,4),'TID End',$e],
		[$off,Frame::bytes($v,0x0000ffff,4),'TID Value',$v],
	];
}
sub dissect_SUA_PT_RANGE {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_SUA_PT_SMI {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = ($val >> 8);
	my $smi = $val & 0xff;
	return [
		[$off,Frame::spare($spa,0xffffff00,4),'Spare',$spa],
		[$off,Frame::bytes($smi,0x000000ff,4),'SMI',$smi],
	];
}
sub dissect_SUA_PT_IMP {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = ($val >> 8);
	my $imp = $val & 0xff;
	return [
		[$off,Frame::spare($spa,0xffffff00,4),'Spare',$spa],
		[$off,Frame::bytes($imp,0x000000ff,4),'Importance',$imp],
	];
}
sub dissect_SUA_PT_PRIO {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Priority',$val], ];
}
sub dissect_SUA_PT_PCLASS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = ($val >> 8);
	my $pcl = $val & 0xff;
	return [
		[$off,Frame::spare($spa,0xffffff00,4),'Spare',$spa],
		[$off,Frame::bytes($pcl,0x000000ff,4),'Protocol Class',$pcl],
	];
}
sub dissect_SUA_PT_SEQCTL {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Seq Control',$val], ];
}
sub dissect_SUA_PT_SEGM {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $fr = ($val >> 24) & 0xff;
	my $sr = $val & 0xffffff;
	return [
		[$off,Frame::bytes($fr,0xff000000,4),'Fragment',$fr],
		[$off,Frame::bytes($sr,0x00ffffff,4),'Segment Reference',$sr],
	];
}
sub dissect_SUA_PT_CONG {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Cong Level',$val], ];
}
sub dissect_SUA_PT_GTI {
	my ($self,$off,$end,$len) = @_;
	my ($gti, $num, $tt, $np, $nai) = unpack('NCCCC',substr(${$self->{buf}},$off,8));
	my $digs = substr(${$self->{buf}},$off+8,$len-8);
	return [
		[$off+0,Frame::bytes($gti,0xffffffff,4),'GTI',$gti],
		[$off+4,Frame::bytes($num,0xff000000,4),'Num Digits',$num],
		[$off+4,Frame::bytes($tt, 0x00ff0000,4),'Transl Type',$tt],
		[$off+4,Frame::bytes($np, 0x0000ff00,4),'Numb Plan',$np],
		[$off+4,Frame::bytes($nai,0x000000ff,4),'Nat Addr Ind',$nai],
		[$off+8,' ******** ','Digits',Frame::pdigs(substr(${$self->{buf}},$off+8,$len-8),$num)],
	];
}
sub dissect_SUA_PT_PC {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = $val >> 24;
	my $pc  = $val & 0xffffff;
	if ($pc&(~0x3fff)) {
		$pc = (($pc>>16)&0xff).'-'.(($pc>>8)&0xff).'-'.(($pc>>0)&0xff);
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::bytes($val,0x00ffffff,4),'Point Code',$pc],
		];
	} else {
		$pc = (($pc>>11)&0x07).'-'.(($pc>>3)&0xff).'-'.(($pc>>0)&0x07);
		my $pc0 = ($val >> 14) & 0x3ff;
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::spare($pc0,0x00ffc000,4),'Unused',$pc0],
			[$off,Frame::bytes($val,0x00003fff,4),'Point Code',$pc],
		];
	}
}
sub dissect_SUA_PT_SSN {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = $val >> 8;
	my $ssn = $val & 0xff;
	return [
		[$off,Frame::spare($spa,0xffffff00,4),'Spare',$spa],
		[$off,Frame::bytes($ssn,0x000000ff,4),'SSN',$ssn],
	];
}
sub dissect_SUA_PT_IPV4 {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $ipa = (($val>>24)&0xff).'.'.(($val>>16)&0xff).'.'.(($val>>8)&0xff).'.'.(($val>>0)&0xff);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'IPv4 Addr',$ipa] ];
}
sub dissect_SUA_PT_HOST {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','Hostname',Frame::ptext(substr(${$self->{buf}},$off,$len))] ];
}
sub dissect_SUA_PT_IPV6 {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','IPv6 Addr',Frame::pdata(substr(${$self->{buf}},$off,$len))] ];
}
sub dissect_M3UA_PT_NA {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'NA',$val] ];
}
sub dissect_M3UA_PT_UCAUSE {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $caus = ($val >> 16) & 0xffff;
	my $user = ($val >>  0) & 0xffff;
	return [
		[$off,Frame::bytes($caus,0xffff0000,4),'Cause',$caus],
		[$off,Frame::bytes($user,0x0000ffff,4),'User',$user],
	];
}
sub dissect_M3UA_PT_CONG {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Cong Level',$val] ];
}
sub dissect_M3UA_PT_CDEST {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = $val >> 24;
	my $dpc = $val & 0xffffff;
	if ($dpc&(~0x3fff)) {
		$dpc = (($dpc>>16)&0xff).'-'.(($dpc>>8)&0xff).'-'.(($dpc>>0)&0xff);
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::bytes($val,0x00ffffff,4),'Dest PC',$dpc],
		];
	} else {
		$dpc = (($dpc>>11)&0x07).'-'.(($dpc>>3)&0xff).'-'.(($dpc>>0)&0x07);
		my $pc0 = ($val >> 14) & 0x3ff;
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::spare($pc0,0x00ffc000,4),'Unused',$pc0],
			[$off,Frame::bytes($val,0x00003fff,4),'Dest PC',$dpc],
		];
	}
}
sub dissect_M3UA_PT_RKEY {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_M3UA_PT_RRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_M3UA_PT_DRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms($self,$off,$end,$len);
}
sub dissect_M3UA_PT_KEYID {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Key ID',$val] ];
}
sub dissect_M3UA_PT_DPC {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',${$self->{buf}},$off,4);
	my $spa = $val >> 24;
	my $dpc = $val & 0xffffff;
	if ($dpc&(~0x3fff)) {
		$dpc = (($dpc>>16)&0xff).'-'.(($dpc>>8)&0xff).'-'.(($dpc>>0)&0xff);
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::bytes($val,0x00ffffff,4),'DPC',$dpc],
		];
	} else {
		$dpc = (($dpc>>11)&0x07).'-'.(($dpc>>3)&0xff).'-'.(($dpc>>0)&0x07);
		my $pc0 = ($val >> 14) & 0x3ff;
		return [
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::spare($pc0,0x00ffc000,4),'Unused',$pc0],
			[$off,Frame::bytes($val,0x00003fff,4),'DPC',$dpc],
		];
	}
}
sub dissect_M3UA_PT_SI {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end > $off) {
		my $val = unpack('C',substr(${$self->{buf}},$off,1));
		if ($val) {
			push @fields,
			[$off,Field::bytes($val,0xff,1),'SI',$val];
		} else {
			push @fields,
			[$off,Field::spare($val,0xff,1),'Spare',$val];
		}
	}
	return \@fields;
}
sub dissect_M3UA_PT_OPCLIST {
	my ($self,$off,$end,$len) = @_;
	my @fields = ();
	while ($end >= $off + 4) {
		my $spa = unpack('C',substr(${$self->{buf}},$off,1));
		my $val = unpack('N',substr(${$self->{buf}},$off,4)) & 0xffffff;
		my $opc = $val;
		if ($val&(~0x3fff)) {
			$opc = (($opc>>16)&0xff).'-'.(($opc>>8)&0xff).'-'.(($opc>>0)&0xff);
			push @fields,
			[$off,Frame::spare($spa,0xff000000,4),'Spare',$spa],
			[$off,Frame::bytes($val,0x00ffffff,4),'OPC',  $opc];
		} else {
			$opc = (($opc>>11)&0x07).'-'.(($opc>>3)&0xff).'-'.(($opc>>0)&0x07);
			my $opc0 = ($opc >> 14) & 0x3ff;
			push @fields,
			[$off,Frame::spare($spa, 0xff000000,4),'Spare', $spa ],
			[$off,Frame::spare($opc0,0x00ffc000,4),'Unused',$opc0],
			[$off,Frame::bytes($val, 0x00003fff,4),'OPC',   $opc ];
		}
	}
	return \@fields;
}
sub dissect_M3UA_PT_DATA {
	my ($self,$off,$end,$len) = @_;
	my ($opc, $dpc, $si, $ni, $mp, $sls) = unpack('NNCCCC',substr(${$self->{buf}},$off,12));
	my ($osp, $dsp) = (($opc>>24)&0xff,(($dpc>>24)&0xff));
	$opc &= 0xffffff; $dpc &= 0xffffff;
	my @fields = ();
	if (($opc|$dpc)&(~0x3fff)) {
		my $OPC = (($opc>>16)&0xff).'-'.(($opc>>8)&0xff).'-'.(($opc>>0)&0xff);
		my $DPC = (($dpc>>16)&0xff).'-'.(($dpc>>8)&0xff).'-'.(($dpc>>0)&0xff);
		push @fields,
			[$off+ 0,Frame::spare($osp,0xff000000,4),'Spare',$osp],
			[$off+ 0,Frame::bytes($opc,0x00ffffff,4),'OPC',$OPC],
			[$off+ 4,Frame::spare($dsp,0xff000000,4),'Spare',$dsp],
			[$off+ 4,Frame::bytes($dpc,0x00ffffff,4),'DPC',$DPC];
	} else {
		my ($op0,$dp0) = (($opc>>14)&0x3ff,($dpc>>24)&0x3ff);
		$opc &= 0x3fff; $dpc &= 0x3fff;
		my $OPC = (($opc>>11)&0x07).'-'.(($opc>>3)&0xff).'-'.(($opc>>0)&0x07);
		my $DPC = (($dpc>>11)&0x07).'-'.(($dpc>>3)&0xff).'-'.(($dpc>>0)&0x07);
		push @fields,
			[$off+ 0,Frame::spare($osp,0xff000000,4),'Spare',$osp],
			[$off+ 0,Frame::spare($os0,0x00ffc000,4),'Unused',$os0],
			[$off+ 0,Frame::bytes($opc,0x00003fff,4),'OPC',$OPC],
			[$off+ 4,Frame::spare($dsp,0xff000000,4),'Spare',$dsp],
			[$off+ 4,Frame::spare($ds0,0x00ffc000,4),'Unused',$ds0],
			[$off+ 4,Frame::bytes($dpc,0x00003fff,4),'DPC',$DPC];
	}
	push @fields,
		[$off+ 8,Frame::bytes($si, 0xff000000,4),'SI',$si],
		[$off+ 8,Frame::bytes($ni, 0x00ff0000,4),'NI',$ni],
		[$off+ 8,Frame::bytes($mp, 0x0000ff00,4),'MP',$mp],
		[$off+ 8,Frame::bytes($sls,0x000000ff,4),'SLS',$sls],
		[$off+12,' ******** ','Data',Frame::pdata(substr(${$self->{buf}},$off+12,$len-12))];
	return \@fields;
}
sub dissect_M3UA_PT_RSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Reg Status',$val] ];
}
sub dissect_M3UA_PT_DSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Dereg Status',$val] ];
}
sub dissect_M2UA_PT_PDATA1 {
	my ($self,$off,$end,$len) = @_;
	return [ [$off,' ******** ','Data',Frame::pdata(substr(${$self->{buf}},$off,$len))] ];
}
sub dissect_M2UA_PT_PDATA2 {
	my ($self,$off,$end,$len) = @_;
	my $li0 = unpack('C',substr(${$self->{buf}},$off,1));
	my $spa = $li0 & 0x3f; $li0 >>= 6;
	return [
		[$off,Frame::bytes($li0,0xc0,1),'MP',$li0],
		[$off,Frame::spare($spa,0x3f,1),'Spare',$spa],
		[$off,' ******** ','Data',Frame::pdata(substr(${$self->{buf}},$off+1,$len-1))],
	];
}
sub dissect_M2UA_PT_STATE {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'State',$val] ];
}
sub dissect_M2UA_PT_EVENT {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Event',$val] ];
}
sub dissect_M2UA_PT_CONGEST {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Congest level',$val] ];
}
sub dissect_M2UA_PT_DISCARD {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Discard level',$val] ];
}
sub dissect_M2UA_PT_ACTION {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Action',$val] ];
}
sub dissect_M2UA_PT_SEQNUM {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Seq Numb',$val] ];
}
sub dissect_M2UA_PT_RTVRES {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Retrieve Result',$val] ];
}
sub dissect_M2UA_PT_LKEY {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms(@_);
}
sub dissect_M2UA_PT_KEYID {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Key ID',$val] ];
}
sub dissect_M2UA_PT_SDTI {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	my $spar = $val >> 16;
	my $sdti = $val & 0xffff;
	return [
		[$off,Frame::spare($spar,0xffff0000,4),'Spare',$spar],
		[$off,Frame::bytes($sdti,0x0000ffff,4),'SDTI', $sdti],
	];
}
sub dissect_M2UA_PT_SDLI {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	my $spar = $val >> 16;
	my $sdli = $val & 0xffff;
	return [
		[$off,Frame::spare($spar,0xffff0000,4),'Spare',$spar],
		[$off,Frame::bytes($sdli,0x0000ffff,4),'SDLI', $sdli],
	];
}
sub dissect_M2UA_PT_RRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms(@_);
}
sub dissect_M2UA_PT_RSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Reg Status',$val] ];
}
sub dissect_M2UA_PT_DRESULT {
	my ($self,$off,$end,$len) = @_;
	return Message::ULP::dissect_parms(@_);
}
sub dissect_M2UA_PT_DSTATUS {
	my ($self,$off,$end,$len) = @_;
	my $val = unpack('N',substr(${$self->{buf}},$off,4));
	return [ [$off,Frame::bytes($val,0xffffffff,4),'Dereg Status',$val] ];
}

use constant {
	ULP_PARMS_DISSECT=>{
		&ULP_PT_IIID	=> [\&Message::ULP::dissect_ULP_PT_IIID,	'IIID',		'Integer interface identifier'],
		&ULP_PT_TIID	=> [\&Message::ULP::dissect_ULP_PT_TIID,	'TIID',		'Text interface identifier'],
		&ULP_PT_INFO	=> [\&Message::ULP::dissect_ULP_PT_INFO,	'INFO',		'Information'],
		&ULP_PT_RC	=> [\&Message::ULP::dissect_ULP_PT_RC,		'RC',		'Routing context'],
		&ULP_PT_DIAG	=> [\&Message::ULP::dissect_ULP_PT_DIAG,	'DIAG',		'Diagnostic'],
		&ULP_PT_RIID	=> [\&Message::ULP::dissect_ULP_PT_RIID,	'RIID',		'Integer interface identifier range'],
		&ULP_PT_HBDATA	=> [\&Message::ULP::dissect_ULP_PT_HBDATA,	'HB Data',	'Heartbeat data'],
		&ULP_PT_TMODE	=> [\&Message::ULP::dissect_ULP_PT_TMODE,	'TMODE',	'Traffic mode type'],
		&ULP_PT_ECODE	=> [\&Message::ULP::dissect_ULP_PT_ECODE,	'ECODE',	'Error code'],
		&ULP_PT_STATUS	=> [\&Message::ULP::dissect_ULP_PT_STATUS,	'Status',	'Notification Status'],
		&ULP_PT_ASPID	=> [\&Message::ULP::dissect_ULP_PT_ASPID,	'ASP Id',	'ASP identifier'],
		&ULP_PT_APC	=> [\&Message::ULP::dissect_ULP_PT_APC,		'APC',		'Affected point code'],
		&ULP_PT_CID	=> [\&Message::ULP::dissect_ULP_PT_CID,		'Corr Id',	'Correlation identifier'],
		&ULP_PT_RRESULT	=> [\&Message::ULP::dissect_ULP_PT_RRESULT,	'Result',	'Registration result'],
		&ULP_PT_DRESULT	=> [\&Message::ULP::dissect_ULP_PT_DRESULT,	'Result',	'Deregistration result'],
		&ULP_PT_RSTATUS	=> [\&Message::ULP::dissect_ULP_PT_RSTATUS,	'Status',	'Registration status'],
		&ULP_PT_DSTATUS	=> [\&Message::ULP::dissect_ULP_PT_DSTATUS,	'Status',	'Deregistration status'],
		&ULP_PT_KEYID	=> [\&Message::ULP::dissect_ULP_PT_KEYID,	'Key Id',	'Local key identifier'],
		&SUA_PT_HOPC	=> [\&Message::ULP::dissect_SUA_PT_HOPC,	'HOPC',		'Hop counter'],
		&SUA_PT_SA	=> [\&Message::ULP::dissect_SUA_PT_SA,		'SA',		'Source address'],
		&SUA_PT_DA	=> [\&Message::ULP::dissect_SUA_PT_DA,		'DA',		'Destination address'],
		&SUA_PT_SRN	=> [\&Message::ULP::dissect_SUA_PT_SRN,		'SRN',		'Source reference number'],
		&SUA_PT_DRN	=> [\&Message::ULP::dissect_SUA_PT_DRN,		'DRN',		'Destinration reference number'],
		&SUA_PT_CAUSE	=> [\&Message::ULP::dissect_SUA_PT_CAUSE,	'Cause',	'SCCP cause'],
		&SUA_PT_SEQNO	=> [\&Message::ULP::dissect_SUA_PT_SEQNO,	'SEQNO',	'Sequence number'],
		&SUA_PT_RSEQNO	=> [\&Message::ULP::dissect_SUA_PT_RSEQNO,	'RSEQNO',	'Receive sequence number'],
		&SUA_PT_ASPCAP	=> [\&Message::ULP::dissect_SUA_PT_ASPCAP,	'ASPCAP',	'ASP capabilities'],
		&SUA_PT_CREDIT	=> [\&Message::ULP::dissect_SUA_PT_CREDIT,	'Credit',	'Credit'],
		&SUA_PT_DATA	=> [\&Message::ULP::dissect_SUA_PT_DATA,	'Data',		'SCCP-user data'],
		&SUA_PT_UCAUSE	=> [\&Message::ULP::dissect_SUA_PT_UCAUSE,	'UCAUSE',	'User/cause'],
		&SUA_PT_NA	=> [\&Message::ULP::dissect_SUA_PT_NA,		'NA',		'Network appearance'],
		&SUA_PT_RKEY	=> [\&Message::ULP::dissect_SUA_PT_RKEY,	'RK',		'Routing key'],
		&SUA_PT_DRNLAB	=> [\&Message::ULP::dissect_SUA_PT_DRNLAB,	'DRN Label',	'Destination reference number label'],
		&SUA_PT_TIDLAB	=> [\&Message::ULP::dissect_SUA_PT_TIDLAB,	'TID Label',	'Transaction identifier label'],
		&SUA_PT_RANGE	=> [\&Message::ULP::dissect_SUA_PT_RANGE,	'Range',	'Address range'],
		&SUA_PT_SMI	=> [\&Message::ULP::dissect_SUA_PT_SMI,		'SMI',		'System multiplicity indicator'],
		&SUA_PT_IMP	=> [\&Message::ULP::dissect_SUA_PT_IMP,		'IMP',		'Importance'],
		&SUA_PT_PRIO	=> [\&Message::ULP::dissect_SUA_PT_PRIO,	'PRIO',		'Priority'],
		&SUA_PT_PCLASS	=> [\&Message::ULP::dissect_SUA_PT_PCLASS,	'PCLASS',	'Protocol class'],
		&SUA_PT_SEQCTL	=> [\&Message::ULP::dissect_SUA_PT_SEQCTL,	'SEQCTL',	'Sequence control'],
		&SUA_PT_SEGM	=> [\&Message::ULP::dissect_SUA_PT_SEGM,	'SEGM',		'Segmentation'],
		&SUA_PT_CONG	=> [\&Message::ULP::dissect_SUA_PT_CONG,	'CONG',		'Congestion level'],
		&SUA_PT_GTI	=> [\&Message::ULP::dissect_SUA_PT_GTI,		'GTI',		'Global title indicator'],
		&SUA_PT_PC	=> [\&Message::ULP::dissect_SUA_PT_PC,		'PC',		'Point code'],
		&SUA_PT_SSN	=> [\&Message::ULP::dissect_SUA_PT_SSN,		'SSN',		'Subsystem number'],
		&SUA_PT_IPV4	=> [\&Message::ULP::dissect_SUA_PT_IPV4,	'IPv4',		'Internet Protocol version 4 address'],
		&SUA_PT_HOST	=> [\&Message::ULP::dissect_SUA_PT_HOST,	'Host',		'Host name'],
		&SUA_PT_IPV6	=> [\&Message::ULP::dissect_SUA_PT_IPV6,	'IPV6',		'Internet Protocol version 6 address'],
		&M3UA_PT_NA	=> [\&Message::ULP::dissect_M3UA_PT_NA,		'NA',		'Network appearance'],
		&M3UA_PT_UCAUSE	=> [\&Message::ULP::dissect_M3UA_PT_UCAUSE,	'UCAUSE',	'User/cause'],
		&M3UA_PT_CONG	=> [\&Message::ULP::dissect_M3UA_PT_CONG,	'CONG',		'Congestion level'],
		&M3UA_PT_CDEST	=> [\&Message::ULP::dissect_M3UA_PT_CDEST,	'CDEST',	'Concerned destination'],
		&M3UA_PT_RKEY	=> [\&Message::ULP::dissect_M3UA_PT_RKEY,	'RK',		'Routing key'],
		&M3UA_PT_RRESULT=> [\&Message::ULP::dissect_M3UA_PT_RRESULT,	'Result',	'Registration result'],
		&M3UA_PT_DRESULT=> [\&Message::ULP::dissect_M3UA_PT_DRESULT,	'Result',	'Deregistration result'],
		&M3UA_PT_KEYID	=> [\&Message::ULP::dissect_M3UA_PT_KEYID,	'Key Id',	'Local key identifier'],
		&M3UA_PT_DPC	=> [\&Message::ULP::dissect_M3UA_PT_DPC,	'DPC',		'Destination point code'],
		&M3UA_PT_SI	=> [\&Message::ULP::dissect_M3UA_PT_SI,		'SI',		'Service indicators'],
		&M3UA_PT_OPCLIST=> [\&Message::ULP::dissect_M3UA_PT_OPCLIST,	'OPC List',	'Originating point code list'],
		&M3UA_PT_DATA	=> [\&Message::ULP::dissect_M3UA_PT_DATA,	'Data',		'MTP-user Data'],
		&M3UA_PT_RSTATUS=> [\&Message::ULP::dissect_M3UA_PT_RSTATUS,	'Status',	'Registration status'],
		&M3UA_PT_DSTATUS=> [\&Message::ULP::dissect_M3UA_PT_DSTATUS,	'Status',	'Deregistration status'],
		&M2UA_PT_PDATA1	=> [\&Message::ULP::dissect_M2UA_PT_PDATA1,	'Prot Data 1',	'Protocol data 1'],
		&M2UA_PT_PDATA2	=> [\&Message::ULP::dissect_M2UA_PT_PDATA2,	'Prot Data 2',	'Protocol data 2 (TTC)'],
		&M2UA_PT_STATE	=> [\&Message::ULP::dissect_M2UA_PT_STATE,	'State',	'State'],
		&M2UA_PT_EVENT	=> [\&Message::ULP::dissect_M2UA_PT_EVENT,	'Event',	'Event'],
		&M2UA_PT_CONGEST=> [\&Message::ULP::dissect_M2UA_PT_CONGEST,	'Congest',	'Congest level'],
		&M2UA_PT_DISCARD=> [\&Message::ULP::dissect_M2UA_PT_DISCARD,	'Discard',	'Discard level'],
		&M2UA_PT_ACTION	=> [\&Message::ULP::dissect_M2UA_PT_ACTION,	'Action',	'Action'],
		&M2UA_PT_SEQNUM	=> [\&Message::ULP::dissect_M2UA_PT_SEQNUM,	'Seqnum',	'Sequence number'],
		&M2UA_PT_RTVRES	=> [\&Message::ULP::dissect_M2UA_PT_RTVRES,	'Result',	'Retrieval result'],
		&M2UA_PT_LKEY	=> [\&Message::ULP::dissect_M2UA_PT_LKEY,	'LK',		'Link key'],
		&M2UA_PT_KEYID	=> [\&Message::ULP::dissect_M2UA_PT_KEYID,	'Key Id',	'Local key identifier'],
		&M2UA_PT_SDTI	=> [\&Message::ULP::dissect_M2UA_PT_SDTI,	'SDTI',		'Signalling terminal identifier'],
		&M2UA_PT_SDLI	=> [\&Message::ULP::dissect_M2UA_PT_SDLI,	'SDLI',		'Signalling data link identifier'],
		&M2UA_PT_RRESULT=> [\&Message::ULP::dissect_M2UA_PT_RRESULT,	'Result',	'Registration result'],
		&M2UA_PT_RSTATUS=> [\&Message::ULP::dissect_M2UA_PT_RSTATUS,	'Status',	'Resgistration status'],
		&M2UA_PT_DRESULT=> [\&Message::ULP::dissect_M2UA_PT_DRESULT,	'Result',	'Deregistration result'],
		&M2UA_PT_DSTATUS=> [\&Message::ULP::dissect_M2UA_PT_DSTATUS,	'Status',	'Deregistration status'],
	},
};

sub dissect_parms {
	my ($self,$off,$end,$mlen) = @_;
	my @parms = ();
	while ($off + 4 <= $end) {
		my ($tag,$len) = unpack('nn',substr(${$self->{buf}},$off,4));
		my $pt = ULP_PARM_TYPES->{$tag}.'('.sprintf('0x%04X',$tag).')';
		push @parms, "Parameter $pt",
			[$off+0,Frame::bytes($tag,0xffff0000,4),'Tag',$pt],
			[$off+0,Frame::bytes($len,0x0000ffff,4),'Length',$len];
		my $sub = ULP_PARMS_DISSECT->{$tag} if exists ULP_PARMS_DISSECT->{$tag};
		if (defined $sub) {
			push @parms, @{$sub->($self,$off+4,$off+(($len+3)&(~0x3)),$len-4)};
		} else {
			push @parms, [$off+4,' ******** ','Data',Frame::pdata(substr(${$self->{buf}},$off+4,$len-4))];
		}
	}
	return \@parms;
}

# -------------------------------------
package Message::M2PA; our @ISA = qw(Message::ULP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return if $self->{mlen} < 16;
	return if $self->{end} < $self->{off} + 8;
	$self->{m2pa}{off} = $self->{off};
	$self->{m2pa}{end} = $self->{end};
	(
		$self->{bsn},
		$self->{fsn},
	) =
	unpack('NN',substr(${$self->{buf}},$self->{off},8));
	$self->{off} += 8;
	$self->{bsn} &= 0x00ffffff;
	$self->{fsn} &= 0x00ffffff;
	$self->{li} = $self->{mlen} - 16;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{m2pa}{off};
	return [
		@{$self->SUPER::dissect(@_)},
		[$off+0,Frame::bytes($self->{bsn}, 0x00ffffff,4),'BSN',$self->{bsn}],
		[$off+4,Frame::bytes($self->{fsn}, 0x00ffffff,4),'FSN',$self->{fsn}],
	];
}

# -------------------------------------
package Message::M2PA::ACK; our @ISA = qw(Message::M2PA);
# -------------------------------------

# -------------------------------------
package Message::M2PA::DATA; our @ISA = qw(Message::M2PA);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_header(@_);
	$self->{li0} = unpack('C',substr(${$self->{buf}},$self->{off},1)) >> 6;
	if ($self->{li0} == 0) {
		bless $self,'Message::M2PA::ACK';
		return $self;
	}
	$self->{off}++; $self->{li}--;
	$self->{doff} = $self->{off};
	$self->{dlen} = $self->{li};
	$self->{dend} = $self->{off} + $self->{li};
	return $self;
}
sub testit {
	my $self = shift;
	return -1 unless $self->unpack(@_);
	my ($decoder) = @_;
	$decoder->{ht} = ::HT_EXTENDED;
	if ($decoder->{pr} == ::MP_UNKNOWN) {
		if ($self->{li0} != 0) {
			$decoder->setpr(::MP_JAPAN);
			$decoder->setrt(::RT_24BIT_PC);
		}
	}
	return 1;
}
sub decode {
	return new Message::MTP3(@_);
}
sub detect {
	return try Message::MTP3(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{m2pa}{off} + 8;
	my $li00 = unpack('C',substr(${$self->{buf}},$off,1)) & 0x3f;
	return [
		@{$self->SUPER::dissect(@_)},
		[$off+0,Frame::spare($li00,0x3f,1),'Spare',$li00],
		[$off+0,Frame::bytes($self->{li0},0xc0,1),'LI Spare Bits',$self->{li0}],
	];
}
# -------------------------------------
package Message::M2PA::STATUS; our @ISA = qw(Message::M2PA);
# -------------------------------------
use constant {
	M2PA_STATUS_TYPES=>{
		1=>'ALIGNMENT',
		2=>'PROVING_NORMAL',
		3=>'PROVING_EMERGENCY',
		4=>'IN_SERVICE',
		5=>'PROCESSOR_OUTAGE',
		6=>'PROCESSOR_OUTAGE_ENDED',
		7=>'BUSY',
		8=>'BUSY_ENDED',
		9=>'OUT_OF_SERVICE',
	},
};
sub unpack {
	my $self = shift;
	return unless $self->SUPER::unpack(@_);
	return unless $self->{end} >= $self->{off} + 4;
	$self->{stat} = unpack('N',substr(${$self->{buf}},$self->{off},4));
	$self->{off} += 4;  $self->{li} -= 4;
	return unless exists M2PA_STATUS_TYPES->{$self->{stat}};
	$self->{filler} = substr(${$self->{buf}},$self->{off},$self->{li});
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{m2pa}{off} + 8;
	my $stat = $self->{stat}; $stat = M2PA_STATUS_TYPES->{$stat}."($stat)";
	return [
		@{$self->SUPER::dissect(@_)},
		[$off+0,Frame::bytes($self->{stat},0xffffffff),'Status',$stat],
	];
}
# -------------------------------------
package Message::M2PA::ALIGNMENT;              our @ISA = qw(Message::M2PA);
package Message::M2PA::PROVING_NORMAL;         our @ISA = qw(Message::M2PA);
package Message::M2PA::PROVING_EMERGENCY;      our @ISA = qw(Message::M2PA);
package Message::M2PA::IN_SERVICE;             our @ISA = qw(Message::M2PA);
package Message::M2PA::PROCESSOR_OUTAGE;       our @ISA = qw(Message::M2PA);
package Message::M2PA::PROCESSOR_OUTAGE_ENDED; our @ISA = qw(Message::M2PA);
package Message::M2PA::BUSY;                   our @ISA = qw(Message::M2PA);
package Message::M2PA::BUSY_ENDED;             our @ISA = qw(Message::M2PA);
package Message::M2PA::OUT_OF_SERVICE;         our @ISA = qw(Message::M2PA);
# -------------------------------------

# -------------------------------------
package Message::M2UA; our @ISA = qw(Message::ULP);
package Message::M2UA::MAUP; our @ISA = qw(Message::M2UA);
# -------------------------------------
package Message::M2UA::MAUP::Data;                               our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{doff};
	return $self;
}
sub decode {
	return new Message::MTP3(@_);
}
sub detect {
	return try Message::MTP3(@_);
}
# -------------------------------------
package Message::M2UA::MAUP::Data_Acknowledge;                   our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{cid};
	return $self;
}

# -------------------------------------
package Message::M2UA::MAUP::State_Request;                      our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{state};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::State_Confirm;                      our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{state};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::State_Indication;                   our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{event};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::Congestion_Indication;              our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{congest};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::Data_Retrieval_Request;             our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{action};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::Data_Retrieval_Confirm;             our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{action} and exists $self->{result};
	return $self;
}
# -------------------------------------
package Message::M2UA::MAUP::Data_Retrieval_Indication;          our @ISA = qw(Message::M2UA::MAUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_parms(@_);
	return unless exists $self->{doff};
	return $self;
}
sub decode {
	return new Message::MTP3(@_);
}
sub detect {
	return try Message::MTP3(@_);
}

# -------------------------------------
package Message::M2UA::MGMT; our @ISA = qw(Message::M2UA);
# -------------------------------------
package Message::M2UA::MGMT::ERR; our @ISA = qw(Message::M2UA::MGMT);
package Message::M2UA::MGMT::NTFY; our @ISA = qw(Message::M2UA::MGMT);
# -------------------------------------

# -------------------------------------
package Message::M2UA::ASPSM; our @ISA = qw(Message::M2UA);
# -------------------------------------
package Message::M2UA::ASPSM::ASPUP_REQ; our @ISA = qw(Message::M2UA::ASPSM);
package Message::M2UA::ASPSM::ASPUP_ACK; our @ISA = qw(Message::M2UA::ASPSM);
package Message::M2UA::ASPSM::ASPDN_REQ; our @ISA = qw(Message::M2UA::ASPSM);
package Message::M2UA::ASPSM::ASPDN_ACK; our @ISA = qw(Message::M2UA::ASPSM);
package Message::M2UA::ASPSM::HBEAT_REQ; our @ISA = qw(Message::M2UA::ASPSM);
package Message::M2UA::ASPSM::HBEAT_ACK; our @ISA = qw(Message::M2UA::ASPSM);
# -------------------------------------

# -------------------------------------
package Message::M2UA::ASPTM; our @ISA = qw(Message::M2UA);
# -------------------------------------
package Message::M2UA::ASPTM::ASPAC_REQ; our @ISA = qw(Message::M2UA::ASPTM);
package Message::M2UA::ASPTM::ASPAC_ACK; our @ISA = qw(Message::M2UA::ASPTM);
package Message::M2UA::ASPTM::ASPIA_REQ; our @ISA = qw(Message::M2UA::ASPTM);
package Message::M2UA::ASPTM::ASPIA_ACK; our @ISA = qw(Message::M2UA::ASPTM);
# -------------------------------------

# -------------------------------------
package Message::M2UA::IIM; our @ISA = qw(Message::M2UA);
# -------------------------------------
package Message::M2UA::IIM::REG_REQ;	our @ISA = qw(Message::M2UA::IIM);
package Message::M2UA::IIM::REG_RSP;	our @ISA = qw(Message::M2UA::IIM);
package Message::M2UA::IIM::DEREG_REQ;	our @ISA = qw(Message::M2UA::IIM);
package Message::M2UA::IIM::DEREG_RSP;	our @ISA = qw(Message::M2UA::IIM);
# -------------------------------------

# -------------------------------------
package Message::M3UA; our @ISA = qw(Message::ULP);
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->SUPER::unpack_parms(@_);
	return $self;
}

# -------------------------------------
package Message::M3UA::XFER; our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::XFER::DATA; our @ISA = qw(Message::M3UA::XFER);
# -------------------------------------
use constant { SI_VALUES=>{ 0=>'SNMM', 1=>'SNTM', 2=>'SNSM', 3=>'SCCP', 4=>'TUP', 5=>'ISUP' } };
sub decode {
	my $self = shift;
	my $type = SI_VALUES->{$self->{si}} if exists SI_VALUES->{$self->{si}}; $type = 'USER' unless defined $type;
	my $prot = (($self->{dpc}|$self->{opc})&(~0x3fff)) ? 'ANSI' : 'ITUT';
	return new Message::$prot::$type($self,@_);
}
# -------------------------------------
package Message::M3UA::SSNM;  our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::SSNM::DUNA; our @ISA = qw(Message::M3UA::SSNM);
package Message::M3UA::SSNM::DAVA; our @ISA = qw(Message::M3UA::SSNM);
package Message::M3UA::SSNM::DAUD; our @ISA = qw(Message::M3UA::SSNM);
package Message::M3UA::SSNM::SCON; our @ISA = qw(Message::M3UA::SSNM);
package Message::M3UA::SSNM::DUPU; our @ISA = qw(Message::M3UA::SSNM);
package Message::M3UA::SSNM::DRST; our @ISA = qw(Message::M3UA::SSNM);
# -------------------------------------

# -------------------------------------
package Message::M3UA::MGMT;  our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::MGMT::ERR; our @ISA = qw(Message::M3UA::MGMT);
package Message::M3UA::MGMT::NTFY; our @ISA = qw(Message::M3UA::MGMT);
# -------------------------------------

# -------------------------------------
package Message::M3UA::ASPSM; our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::ASPSM::ASPUP; our @ISA = qw(Message::M3UA::ASPSM);
package Message::M3UA::ASPSM::ASPDN; our @ISA = qw(Message::M3UA::ASPSM);
package Message::M3UA::ASPSM::HBEAT; our @ISA = qw(Message::M3UA::ASPSM);
# -------------------------------------
package Message::M3UA::ASPSM::ASPUP_REQ; our @ISA = qw(Message::M3UA::ASPSM::ASPUP);
package Message::M3UA::ASPSM::ASPDN_REQ; our @ISA = qw(Message::M3UA::ASPSM::ASPDN);
package Message::M3UA::ASPSM::HBEAT_REQ; our @ISA = qw(Message::M3UA::ASPSM::HBEAT);
package Message::M3UA::ASPSM::ASPUP_ACK; our @ISA = qw(Message::M3UA::ASPSM::ASPUP);
package Message::M3UA::ASPSM::ASPDN_ACK; our @ISA = qw(Message::M3UA::ASPSM::ASPDN);
package Message::M3UA::ASPSM::HBEAT_ACK; our @ISA = qw(Message::M3UA::ASPSM::HBEAT);
# -------------------------------------

# -------------------------------------
package Message::M3UA::ASPTM; our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::ASPTM::ASPAC_REQ; our @ISA = qw(Message::M3UA::ASPTM);
package Message::M3UA::ASPTM::ASPIA_REQ; our @ISA = qw(Message::M3UA::ASPTM);
package Message::M3UA::ASPTM::ASPAC_ACK; our @ISA = qw(Message::M3UA::ASPTM);
package Message::M3UA::ASPTM::ASPIA_ACK; our @ISA = qw(Message::M3UA::ASPTM);
# -------------------------------------

# -------------------------------------
package Message::M3UA::RKM;   our @ISA = qw(Message::M3UA);
# -------------------------------------
package Message::M3UA::RKM::REG_REQ;   our @ISA = qw(Message::M3UA::RKM);
package Message::M3UA::RKM::REG_RSP;   our @ISA = qw(Message::M3UA::RKM);
package Message::M3UA::RKM::DEREG_REQ; our @ISA = qw(Message::M3UA::RKM);
package Message::M3UA::RKM::DEREG_RSP; our @ISA = qw(Message::M3UA::RKM);
# -------------------------------------

# -------------------------------------
package Message::SUA; our @ISA = qw(Message::ULP);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{sua}{off} = $self->{off};
	$self->{sua}{end} = $self->{end};
	return unless $self->SUPER::unpack_parms(@_);
	return $self;
}
sub decode {
	return undef unless exists $self->{doff};
	return new Message::SCCP::USER(shift,@_);
}

# -------------------------------------
package Message::SUA::CLDT; our @ISA = qw(Message::SUA);
package Message::SUA::CLDR; our @ISA = qw(Message::SUA);
# -------------------------------------
package Message::SUA::CORE; our @ISA = qw(Message::SUA);
package Message::SUA::COAK; our @ISA = qw(Message::SUA);
package Message::SUA::COREF; our @ISA = qw(Message::SUA);
package Message::SUA::RELRE; our @ISA = qw(Message::SUA);
package Message::SUA::RELCO; our @ISA = qw(Message::SUA);
package Message::SUA::RESCO; our @ISA = qw(Message::SUA);
package Message::SUA::RESRE; our @ISA = qw(Message::SUA);
package Message::SUA::CODT; our @ISA = qw(Message::SUA);
package Message::SUA::CODA; our @ISA = qw(Message::SUA);
package Message::SUA::COERR; our @ISA = qw(Message::SUA);
package Message::SUA::COIT; our @ISA = qw(Message::SUA);
# -------------------------------------

# -------------------------------------
package Message::MTP2; our @ISA = qw(Message);
# -------------------------------------
use constant {
	LI_TYPES=>{
		0=>'FISU',
		1=>'LSSU',
		2=>'LSSU',
		3=>'MSU',
	},
};
sub unpack {
	my $self = shift;
	my ($decoder) = @_;
	if (($self->{ht} = $decoder->{ht}) == ::HT_BASIC) {
		(
			$self->{bsn},
			$self->{fsn},
			$self->{li},
		)
		= unpack('CCC',substr(${$self->{buf}},$self->{off},3));

		$self->{bib} = ($self->{bsn} >> 7) & 0x01; $self->{bsn} &= 0x7f;
		$self->{fib} = ($self->{fsn} >> 7) & 0x01; $self->{fsn} &= 0x7f;
		$self->{li0} = ($self->{li}  >> 6} & 0x03; $self->{li}  &= 0x3f;
		$self->{off} += 3;
	} elsif ($self->{ht} == ::HT_EXTENDED) {
		(
			$self->{bsn},
			$self->{fsn},
			$self->{li},
		)
		= unpack('vvv',substr(${$self->{buf}},$self->{off},6));
		$self->{bib} = ($self->{bsn} >> 15) & 0x01; $self->{bsn} &= 0x0fff;
		$self->{fib} = ($self->{fsn} >> 15) & 0x01; $self->{fsn} &= 0x0fff;
		$self->{li0} = ($self->{li}  >> 14) & 0x03; $self->{li}  &= 0x01ff;
		$self->{off} += 6;
	} else {
		return;
	}
	my $len = $self->{end} - $self->{off};
	if (($self->{li} != $len) && ($len <= 63 || $self->{li} != 63)) {
		print STDERR "W: $self: bad length indicator $self->{li} != $$len\n";
	}
	if ($self->{li0} != 0) {
		$decoder->setrt(::RT_24BIT_PC) if $decoder->{rt} == ::RT_UNKNOWN;
		$decoder->setpr(::MP_JAPAN)    if $decoder->{pr} == ::MP_UNKNOWN;
	}
	my $li = $self->{li}; $li = 3 if $li > 3;
	my $type = LI_TYPES->[$self->{li}];
	bless $self,ref($self)."::$type";
	return $self->unpack(@_);
}
sub testit {
	my $self = shift;
	my ($decoder) = @_;
	my $len = $self->{end} - $self->{off};
	if (3 <= $len && $len <= 5) {
		$decoder->setht(::HT_BASIC);
		return 1;
	} elsif (6 <= $len && $len <= 8) {
		$decoder->setht(::HT_EXTENDED);
		return 1;
	}
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	if ($self->{ht} == ::HT_EXTENDED) {
		my $bsn0 = (unpack('v',substr(${$self->{buf}},$off+0,2)) >> 12) & 0x7;
		my $fsn0 = (unpack('v',substr(${$self->{buf}},$off+2,2)) >> 12) & 0x7;
		my $li0  = (unpack('v',substr(${$self->{buf}},$off+4,2)) >>  9) & 0x1f;
		my $li = $self->{li}; $li = 3 if $li > 3; $li = LI_TYPES->{$li}."($self->{li})";
		return 'MTP Level 2 Extended Header', [
			[$off+ 0,Frame::bytes($self->{bsn},0x0fff,2),'BSN',$self->{bsn}],
			[$off+ 0,Frame::spare($bsn0,       0x7000,2),'Spare',$bsn0],
			[$off+ 0,Frame::bytes($self->{bib},0x8000,2),'BIB',$self->{bib}],
			[$off+ 2,Frame::bytes($self->{fsn},0x0fff,2),'FSN',$self->{fsn}],
			[$off+ 2,Frame::spare($fsn0,       0x7000,2),'Spare',$fsn0],
			[$off+ 2,Frame::bytes($self->{fib},0x8000,2),'FIB',$self->{fib}],
			[$off+ 4,Frame::bytes($self->{li}, 0x01ff,2),'LI',$li],
			[$off+ 4,Frame::spare($li0,        0x3e00,2),'Spare',$li0],
			[$off+ 4,Frame::bytes($self->{li0},0xc000,2),'LI0',$self->{li0}],
		],@_;
	} elsif ($self->{ht} == ::HT_BASIC) {
		my $li = $self->{li}; $li = 3 if $li > 3; $li = LI_TYPES->{$li}."($self->{li})";
		return 'MTP Level 2 Basic Header', [
			[$off+ 0,Frame::bytes($self->{bsn},0x7f,1),'BSN',$self->{bsn}],
			[$off+ 0,Frame::bytes($self->{bib},0x80,1),'BIB',$self->{bib}],
			[$off+ 1,Frame::bytes($self->{fsn},0x7f,1),'FSN',$self->{fsn}],
			[$off+ 1,Frame::bytes($self->{fib},0x80,1),'FIB',$self->{fib}],
			[$off+ 2,Frame::bytes($self->{li}, 0x3f,1),'LI', $li} ],
			[$off+ 2,Frame::bytes($self->{li0},0xc0,1),'LI0',$self->{li0}],
		],@_;
	} else {
		return 'Unknown MTP Level 2 Header Type', [
			[$off,' ******** ','DATA',Frame::pdata(substr($$self->{buf},$self->{off},$self->{end}-$self->{off})],
		],@_;
	}
}

# -------------------------------------
package Message::MTP2::FISU; our @ISA = qw(Message::MTP2);
# -------------------------------------
package Message::MTP2::LSSU; our @ISA = qw(Message::MTP2);
# -------------------------------------
use constant {
	LSSU_TYPES=>{ 'SIO', 'SIN', 'SIE', 'SIOS', 'SIPO', 'SIB' }
};
sub unpack {
	my $self = shift;
	$self->{stat} = unpack('C',substr(${$self->{buf}},$self->{off},1)) & 0x7;
	$self->{off}++;
	return unless exists LSSU_TYPES->[$self->{stat}];
	my $type = LSSU_TYPES->[$self->{stat}];
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg} + $self->{ht};
	my $stat = $self->{stat}; $stat = LSSU_TYPES->[$stat]."($stat)";
	my ($mask,$stat0,$mask0);
	my $li = $self->{li};
	if ($li == 1) {
		$mask  = 0x07;
		$stat0 = unpack('C',substr(${$self->{buf}},$off,1)) >> 3;
		$mask0 = 0xf8;
	} else {
		$mask  = 0x0007;
		$stat0 = unpack('v',substr(${$self->{buf}},$off,2)) >> 3;
		$mask0 = 0xfff8;
	}
	return $self->SUPER::dissect(@_), "Link Status $stat", [
		[$off+0,Frame::bytes($self->{stat},$mask, $li),'Status',$stat],
		[$off+0,Frame::spare($stat0,       $mask0,$li),'Spare',$stat0],
	];
};

package Message::MTP2::LSSU::SIO;  our @ISA = qw(Message::MTP2::LSSU);
package Message::MTP2::LSSU::SIN;  our @ISA = qw(Message::MTP2::LSSU);
package Message::MTP2::LSSU::SIE;  our @ISA = qw(Message::MTP2::LSSU);
package Message::MTP2::LSSU::SIOS; our @ISA = qw(Message::MTP2::LSSU);
package Message::MTP2::LSSU::SIPO; our @ISA = qw(Message::MTP2::LSSU);
package Message::MTP2::LSSU::SIB;  our @ISA = qw(Message::MTP2::LSSU);
# -------------------------------------
package Message::MTP2::MSU;        our @ISA = qw(Message::MTP2);
# -------------------------------------
sub unpack { return shift }
sub decode {
	return new Message::MTP3(@_);
}
sub detect {
	return try Message::MTP3(@_);
}

# -------------------------------------
package Message::MTP3; our @ISA = qw(Message);
# -------------------------------------
use constant {
	SI_VALUES=>{
		0=>'SNMM',
		1=>'SNTM',
		2=>'SNSM',
		3=>'SCCP',
		4=>'TUP',
		5=>'ISUP',
	},
};
#package Message::MTP3;
sub unpack {
	my $self = shift;
	{
		my ($decoder) = @_;
		$self->{ht} = $decoder->{ht};
		$self->{rt} = $decoder->{rt};
		$self->{pr} = $decoder->{pr};
	}
	$self->{si} = unpack('C',substr(${$self->{buf}},$self->{off},1));
	$self->{ni} = ($self->{si} >> 6) & 0x03;
	$self->{mp} = ($self->{si} >> 4) & 0x03;
	$self->{si} &= 0x0f;
	my $type = (exists SI_VALUES->{$self->{si}}) ? SI_VALUES->{$self->{si}} : 'USER';
	$self->{mp} = $self->{li0} if $self->{pr} == ::MP_JAPAN;
	$self->{mp} = 0 if $self->{pr} == ::MP_INTERNATIONAL;
	$self->{off}++;
	my $flav;
	if ($self->{ni} < 2 or $self->{rt} == ::RT_14BIT_PC) {
		return unless $self->{li} >= 6;
		$self->{dpc} = (unpack('v',substr(${$self->{buf}},$self->{off}+0,2)) >> 0) & 0x3fff;
		$self->{opc} = (unpack('V',substr(${$self->{buf}},$self->{off}+1,4)) >> 2) & 0x3fff;
		$self->{sls} = (unpack('C',substr(${$self->{buf}},$self->{off}+3,1)) >> 4) & 0x0f;
		$self->{off} += 4;
		$flav = 'ITUT';
	} elsif ($self->{rt} == ::RT_24BIT_PC) {
		return unless $self->{li} >= 9;
		$self->{dpc} = unpack('V',substr(${$self->{buf}},$self->{off}+0,4)) & 0x00ffffff;
		$self->{opc} = unpack('V',substr(${$self->{buf}},$self->{off}+3,4)) & 0x00ffffff;
		$self->{sls} = unpack('C',substr(${$self->{buf}},$self->{off}+6,1));
		$self->{off} += 7;
		$flav = 'ANSI';
	} else {
		return -1;
	}
	bless $self,"Message::$flav::$type";
	return $self->unpack(@_);
}
#package Message::MTP3;
use constant {
	NI_VALUES=>{
		0=>'International',
		1=>'International reserved',
		2=>'National',
		3=>'National reserved',
	},
};
#package Message::MTP3;
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	my $si = $self->{si}; $si = SI_VALUES->{$si}."($si)" if exists SI_VALUES->{$si};
	my $ni = $self->{ni}; $ni = NI_VALUES->{$ni}."($ni)";
	if ($self->{ni} < 2 or $self->{rt} == ::RT_14BIT_PC) {
		my $dpc = $self->{dpc}; $dpc = (($dpc>>11)&0x7).'-'.(($dpc>>3)&0xff).'-'.(($dpc>>0)&0x7);
		my $opc = $self->{opc}; $opc = (($opc>>11)&0x7).'-'.(($opc>>3)&0xff).'-'.(($opc>>0)&0x7);
		my $sls = $self->{sls};
		return "ITU-T $si Message", [
			[$off+0,Frame::bytes($self->{si},0x0f,1),'SI',$si],
			'ITU-T Routing Label', [
				[$off+0,Frame::bytes($self->{mp},0x30,1),'MP',$self->{mp}],
				[$off+0,Frame::bytes($self->{ni},0xc0,1),'NI',$ni],
				[$off+1,Frame::bytes($self->{dpc},0x00003fff,4),'DPC',$dpc],
				[$off+1,Frame::bytes($self->{opc},0x0fffc000,4),'OPC',$opc],
				[$off+1,Frame::bytes($self->{sls},0xf0000000,4),'SLS',$sls],
			],
		],@_;
	} elsif ($self->{rt} == ::RT_24BIT_PC) {
		my $dpc = $self->{dpc}; $dpc = (($dpc>>16)&0xff).'-'.(($dpc>>8)&0xff).'-'.(($dpc>>0)&0xff);
		my $opc = $self->{opc}; $opc = (($opc>>16)&0xff).'-'.(($opc>>8)&0xff).'-'.(($opc>>0)&0xff);
		my $sls = $self->{sls};
		return "ANSI $si Message", [
			[$off+0,Frame::bytes($self->{si},0x0f,1),'SI',$si],
			'ANSI Routing Label', [
				[$off+0,Frame::bytes($self->{mp},0x30,1),'MP',$self->{mp}],
				[$off+0,Frame::bytes($self->{ni},0xc0,1),'NI',$ni],
				[$off+1,Frame::bytes($self->{dpc},0xffffff,3),'DPC',$dpc],
				[$off+4,Frame::bytes($self->{opc},0xffffff,3),'OPC',$opc],
				[$off+7,Frame::bytes($self->{sls},0xff,1),'SLS',$sls],
			],
		],@_;
	} else {
		return "Unknown RL for $si Message", [
			[$off,' ******** ','DATA',Frame::pdata(substr($$self->{buf},$self->{beg},$self->{end}-$self->{beg}],
		],@_;
	}
}
#package Message::MTP3;
sub testit {
	my $self = shift;
	my ($decoder) = @_;

	$self->{si} = unpack('C',substr(${$self->{buf}},$self->{off},1));
	$self->{ni} = ($self->{si} >> 6) & 0x03;
	$self->{mp} = ($self->{si} >> 4) & 0x03;
	$self->{si} &= 0x0f;
	unless (exists SI_VALUES->{$self->{si}}) {
		warn "$self: no message type for si=$self->{si}";
	}
	if ($self->{ni} == 0) {
		$decoder->setrt(::RT_14BIT_PC);
		$decoder->setpr(::MP_INTERNATIONAL);
	}
	if ($decoder->{pr} == ::MP_UNKNOWN) {
		$decoder->setpr(::MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($decoder->{pr} == ::MP_JAPAN) {
		$self->{mp} = $self->{li0};
	} elsif ($decoder->{pr} == ::MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	if ($self->{li} < ::HT_EXTENDED) {
		warn "$self: too short for RL, li = $self->{li}";
		return -1;
	}
	if ($self->{li} < 9 || ($self->{si} == 5 && $self->{li} < 11)) {
		$decoder->setrt(::RT_14BIT_PC);
	}
	if ($decoder->{rt} == ::RT_UNKNOWN) {
		my @b = (unpack('C*', substr(${$self->{buf}}, $self->{off}, 11)));
		my $ret = $self->_checkRoutingLabelType(\@b);
		return if $ret <= 0;
	}
	return 1;
}
#package Message::MTP3;
use constant {
	PT_YES => 1,
	PT_MAYBE => 0,
	PT_NO => -1
};
#package Message::MTP3;
sub _checkRoutingLabelType {
	my $self = shift;
	if ($self->{si} == 0) {
		return $self->_checkSnmm(@_);
	} elsif ($self->{si} == 1) {
		return $self->_checkSntm(@_);
	} elsif ($self->{si} == 2) {
		return $self->_checkSnsm(@_);
	} elsif ($self->{si} == 3) {
		return $self->_checkSccp(@_);
	} elsif ($self->{si} == 5) {
		return $self->_checkIsup(@_);
	}
	return 0;
}
#package Message::MTP3;
sub _checkSnmm {
	my $self = shift;
	my $ansi = $self->_checkAnsiSnmm(@_);
	my $itut = $self->_checkItutSnmm(@_);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$self->{rt} = ::RT_24BIT_PC;
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$self->{rt} = ::RT_14BIT_PC;
		} else {
			$self->{rt} = ::RT_24BIT_PC;
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$self->{rt} = ::RT_14BIT_PC;
		return 1;
	}
}
#package Message::MTP3;
sub _checkItutSnmm {
	my ($self,$b) = @_;
	my $mt = (($b->[5] & 0x0f) << 4) | ($b->[5] >> 4);
	if ($self->{li} == 6) {
		return PT_YES if $mt == 0x17 || $mt == 0x27 || $mt == 0x13 || $mt == 0x28
			|| $mt == 0x38 || $mt == 0x48 || $mt == 0x12 || $mt == 0x22 || $mt == 0x16
			|| $mt == 0x26 || $mt == 0x36 || $mt == 0x46 || $mt == 0x56 || $mt == 0x66
			|| $mt == 0x76 || $mt == 0x86;
		return PT_NO;
	}
	if ($self->{li} == 7) {
		return PT_YES if $mt == 0x11 || $mt == 0x21 || $mt == 0x51 || $mt == 0x61;
		return PT_NO;
	}
	if ($self->{li} == 8) {
		return PT_YES if $mt == 0x23 || $mt == 0x14 || $mt == 0x24 || $mt == 0x34
			|| $mt == 0x44 || $mt == 0x54 || $mt == 0x64 || $mt == 0x15 || $mt == 0x25
			|| $mt == 0x35 || $mt == 0x45;
		return PT_NO;
	}
	if ($self->{li} == 9) {
		return PT_YES if $mt == 0x1a || $mt == 0x2a || $mt == 0x3a;
		return PT_NO;
	}
	return PT_NO;
}
#package Message::MTP3;
sub _checkAnsiSnmm {
	my ($self,$b) = @_;
	my $mt = (($b->[8] & 0x0f) << 4) | ($b->[8] >> 4);
	return PT_NO if $self->{li} < 9;
	if ($self->{li} == 9) {
		return PT_YES if $mt == 0x17 || $mt == 0x27 || $mt == 0x13;
		return PT_NO;
	}
	if ($self->{li} == 10) {
		return PT_YES if $mt == 0x28 || $mt == 0x38 || $mt == 0x38 || $mt == 0x48
			|| $mt == 0x12 || $mt == 0x22 || $mt == 0x16 || $mt == 0x26 || $mt == 0x36
			|| $mt == 0x46 || $mt == 0x56 || $mt == 0x66 || $mt == 0x76 || $mt == 0x86;
		return PT_NO;
	}
	if ($self->{li} == 11) {
		return PT_YES if $mt == 0x11 || $mt == 0x21 || $mt == 0x51 || $mt == 0x61;
		return PT_NO;
	}
	if ($self->{li} == 12) {
		return PT_YES if $mt == 0x14 || $mt == 0x24 || $mt == 0x34 || $mt == 0x44
			|| $mt == 0x54 || $mt == 0x64 || $mt == 0x15 || $mt == 0x25 || $mt == 0x35
			|| $mt == 0x45 || $mt == 0x18;
		return PT_NO;
	}
	if ($self->{li} == 13) {
		return PT_YES if $mt == 0x23 || $mt == 0x1a || $mt == 0x2a || $mt == 0x3a;
		return PT_NO;
	}
	return PT_NO;
}
#package Message::MTP3;
sub _checkSntm {
	my $self = shift;
	my $ansi = $self->_checkAnsiSntm(@_);
	my $itut = $self->checkItutSntm(@_);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$self->{rt} = ::RT_24BIT_PC;
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$self->{rt} = ::RT_14BIT_PC;
		} else {
			$self->{rt} = ::RT_24BIT_PC;
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$self->{rt} = ::RT_14BIT_PC;
		return 1;
	}
}
#package Message::MTP3;
sub _checkItutSntm {
	my ($self,$b) = @_;
	my $mt = (($b->[5] & 0x0f) << 4) | ($b->[5] >> 4);
	return PT_NO if 7 > $self->{li} || $self->{li} > 22;
	return PT_NO if $mt != 0x11 && $mt != 0x12;
	if (($b->[6] >> 4) == $self->{li} - 7) {
		return PT_YES if 7 <= $self->{li} && $self->{li} <= 9;
		return PT_MAYBE;
	}
	return PT_NO;
}
#package Message::MTP3;
sub _checkAnsiSntm {
	my ($self,$b) = @_;
	my $mt = (($b->[8] & 0x0f) << 4) | ($b->[8] >> 4);
	return PT_NO if 10 > $self->{li} || $self->{li} > 25;
	return PT_NO if $mt != 0x11 && $mt != 0x12;
	if (($b->[9] >> 4) == $self->{li} - 10) {
		return PT_YES if 23 <= $self->{li} && $self->{li} <= 25;
		return PT_MAYBE;
	}
	return PT_NO;
}
#package Message::MTP3;
sub _checkSnsm {
	my $self = shift;
	my $ansi = $self->_checkAnsiSnsm(@_);
	my $itut = $self->_checkItutSntm(@_);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$self->{rt} = ::RT_24BIT_PC;
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$self->{rt} = ::RT_14BIT_PC;
		} else {
			$self->{rt} = ::RT_24BIT_PC;
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$self->{rt} = ::RT_14BIT_PC;
		return 1;
	}
}
#package Message::MTP3;
sub _checkItutSnsm {
	return PT_NO;
}
#package Message::MTP3;
sub _checkAnsiSnsm {
	my $self = shift;
	return $self->_checkAnsiSntm(@_);
}
#package Message::MTP3;
sub _checkSccp {
	my $self = shift;
	my $ansi = $self->_checkAnsiSccp(@_);
	my $itut = $self->_checkItutSccp(@_);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$self->{rt} = ::RT_24BIT_PC;
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$self->{rt} = ::RT_14BIT_PC;
		} else {
			$self->{rt} = ::RT_24BIT_PC;
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$self->{rt} = ::RT_14BIT_PC;
		return 1;
	}
}
#package Message::MTP3;
sub _checkItutSccp {
	my ($self,$b) = @_;
	my $mt = $b->[5];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message::MTP3;
sub _checkAnsiSccp {
	my ($self,$b) = @_;
	my $mt = $b->[8];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message::MTP3;
sub _checkIsup {
	my $self = shift;
	my $ansi = $self->_checkAnsiIsup(@_);
	my $itut = $self->_checkItutIsup(@_);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$self->{rt} = ::RT_24BIT_PC;
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$self->{rt} = ::RT_14BIT_PC;
		} else {
			$self->{rt} = ::RT_24BIT_PC;
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$self->{rt} = ::RT_14BIT_PC;
		return 1;
	}
}
#package Message::MTP3;
sub _checkItutIsup {
	my ($self,$b) = @_;
	my $mt = $b->[7];
	return PT_NO if $self->{li} < 8;
	if ($mt == 0x11 || $mt == 0x12 || $mt == 0x13 || $mt == 0x14 || $mt == 0x15 || $mt == 0x16 || $mt == 0x24 || $mt == 0x2e || $mt == 0x30 || $mt == 0x31) {
		return PT_NO if $self->{li} != 8;
		return PT_YES;
	}
	if ($mt == 0x05) {
		return PT_NO if $self->{li} != 9;
		return PT_YES;
	}
	if ($mt == 0x10 || $mt == 0x27 || $mt == 0x08 || $mt == 0x09 || $mt == 0x34 || $mt == 0x33 || $mt == 0x35 || $mt == 0x36 || $mt == 0x37 || $mt == 0x38 || $mt == 0x32) {
		return PT_NO if $self->{li} < 9;
		if ($self->{li} > 9) {
			return PT_NO if $b->[8] == 0;
			return PT_YES if $self->{li} < 11;
			return PT_MAYBE;
		}
		return PT_NO if $b->[8] != 0;
		return PT_YES;
	}
	if ($mt == 0x03 || $mt == 0x04 || $mt == 0x06 || $mt == 0x0d || $mt == 0x0e || $mt == 0x1c || $mt == 0x1d || $mt == 0x1e || $mt == 0x1f || $mt == 0x20 || $mt == 0x2c) {
		return PT_NO if $self->{li} < 10;
		if ($self->{li} > 10) {
			return PT_NO if $b->[8] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[8] != 0;
		return PT_YES;
	}
	if ($mt == 0x01) {
		return PT_NO if $self->{li} < 18;
		return PT_MAYBE;
	}
	if ($mt == 0x21 || $mt == 0x2b) {
		return PT_NO if $self->{li} < 14;
		return PT_MAYBE;
	}
	if ($mt == 0x02 || $mt == 0x0c || $mt == 0x2f) {
		return PT_NO if $self->{li} < 13;
		return PT_MAYBE;
	}
	if ($mt == 0x17 || $mt == 0x2d || $mt == 0x18 || $mt == 0x1a || $mt == 0x19 || $mt == 0x1b) {
		return PT_NO if $self->{li} < 12;
		return PT_MAYBE;
	}
	if ($mt == 0x07) {
		return PT_NO if $self->{li} < 11;
		if ($self->{li} > 11) {
			return PT_NO if $b->[10] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[10] != 0;
		return PT_YES;
	}
	if ($mt == 0x29 || $mt == 0x2a) {
		return PT_NO if $self->{li} < 11;
		return PT_MAYBE;
	}
	if ($mt == 0x28) {
		return PT_MAYBE;
	}
	if ($mt == 0x22 || $mt == 0x23 || $mt == 0xe9 || $mt == 0xea || $mt == 0xeb || $mt == 0xec || $mt == 0xed || $mt == 0xf8 || $mt == 0xfc || $mt == 0xfd || $mt == 0xfe || $mt == 0xff || $mt == 0x25 || $mt == 0x26) {
		return PT_NO;
	}
	return PT_NO;
}
#package Message::MTP3;
sub _checkAnsiIsup {
	my ($self,$b) = @_;
	my $mt = $b->[10];
	return PT_NO if $self->{li} < 11;
	if ($mt == 0x10 || $mt == 0x11 || $mt == 0x12 || $mt == 0x13 || $mt == 0x14 || $mt == 0x15 || $mt == 0x16 || $mt == 0x24 || $mt == 0x2e || $mt == 0xe9 || $mt == 0xec) {
		return PT_NO if $self->{li} != 11;
		return PT_MAYBE;
	}
	if ($mt == 0xed) {
		return PT_NO if $self->{li} < 12;
		if ($self->{li} > 12) {
			return PT_NO if $b->[11] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[11] != 0;
		return PT_MAYBE;
	}
	if ($mt == 0x05 || $mt == 0xea) {
		return PT_NO if $self->{li} != 12;
		return PT_MAYBE;
	}
	if (exists ISUP_TYPES->{$mt}) {
		return PT_MAYBE;
	}
	return PT_MAYBE;
}


# -------------------------------------
package Message::ITUT::MTP3; our @ISA = qw(Message::MTP3);
# -------------------------------------
use constant {
	SI_VALUES=>{
		0=>'SNMM',
		1=>'SNTM',
		2=>'SNSM',
		3=>'SCCP',
		4=>'TUP',
		5=>'ISUP',
	},
	NI_VALUES=>{
		0=>'International',
		1=>'International reserved',
		2=>'National',
		3=>'National reserved',
	},
};
sub dissect {
	my $off = $self->{beg};
	my $si = $self->{si}; $si = SI_VALUES->{$si}."($si)" if exists SI_VALUES->{$si};
	my $ni = $self->{ni}; $ni = NI_VALUES->{$ni}."($ni)";
	my $dpc = $self->{dpc}; $dpc = (($dpc>>11)&0x7).'-'.(($dpc>>3)&0xff).'-'.(($dpc>>0)&0x7);
	my $opc = $self->{opc}; $opc = (($opc>>11)&0x7).'-'.(($opc>>3)&0xff).'-'.(($opc>>0)&0x7);
	my $sls = $self->{sls};
	return "ITU-T $si Message", [
		[$off+0,Frame::bytes($self->{si},0x0f,1),'SI',$si],
		'ITU-T Routing Label', [
			[$off+0,Frame::bytes($self->{mp},0x30,1),'MP',$self->{mp}],
			[$off+0,Frame::bytes($self->{ni},0xc0,1),'NI',$ni],
			[$off+1,Frame::bytes($self->{dpc},0x00003fff,4),'DPC',$dpc],
			[$off+1,Frame::bytes($self->{opc},0x0fffc000,4),'OPC',$opc],
			[$off+1,Frame::bytes($self->{sls},0xf0000000,4),'SLS',$sls],
		],
		@_,
	];
}
# -------------------------------------
package Message::ANSI::MTP3; our @ISA = qw(Message::MTP3);
# -------------------------------------
use constant {
	SI_VALUES=>{
		0=>'SNMM',
		1=>'SNTM',
		2=>'SNSM',
		3=>'SCCP',
		4=>'TUP',
		5=>'ISUP',
	},
	NI_VALUES=>{
		0=>'International',
		1=>'International reserved',
		2=>'National',
		3=>'National reserved',
	},
};
sub dissect {
	my $off = $self->{beg};
	my $si = $self->{si}; $si = SI_VALUES->{$si}."($si)" if exists SI_VALUES->{$si};
	my $ni = $self->{ni}; $ni = NI_VALUES->{$ni}."($ni)";
	my $dpc = $self->{dpc}; $dpc = (($dpc>>16)&0xff).'-'.(($dpc>>8)&0xff).'-'.(($dpc>>0)&0xff);
	my $opc = $self->{opc}; $opc = (($opc>>16)&0xff).'-'.(($opc>>8)&0xff).'-'.(($opc>>0)&0xff);
	my $sls = $self->{sls};
	return "ANSI $si Message", [
		[$off+0,Frame::bytes($self->{si},0x0f,1),'SI',$si],
		'ANSI Routing Label', [
			[$off+0,Frame::bytes($self->{mp},0x30,1),'MP',$self->{mp}],
			[$off+0,Frame::bytes($self->{ni},0xc0,1),'NI',$ni],
			[$off+1,Frame::bytes($self->{dpc},0xffffff,3),'DPC',$dpc],
			[$off+4,Frame::bytes($self->{opc},0xffffff,3),'OPC',$opc],
			[$off+7,Frame::bytes($self->{sls},0xff,1),'SLS',$sls],
		];
		@_,
	];
}
# -------------------------------------
# -------------------------------------
package Message::MTP3::MGMT; our @ISA = qw(Message::MTP3);
# -------------------------------------
sub dissect {
	my $self = shift;
	return $self->Message::MTP3::dissect(@_) if exists $self->{mtp3};
	return $self->Message::M3UA::XFER::DATA::dissect(@_) if exists $self->{m3ua};
}

# -------------------------------------
package Message::SNMM; our @ISA = qw(Message::MTP3::MGMT);
# -------------------------------------
use constant {
	SNMM_TYPES=>{
		0x11 => 'COO',
		0x12 => 'COA',
		0x15 => 'CBD',
		0x16 => 'CBA',
		0x21 => 'ECO',
		0x22 => 'ECA',
		0x31 => 'RCT',
		0x32 => 'TFC',
		0x41 => 'TFP',
		0x42 => 'TCP',
		0x43 => 'TFR',
		0x44 => 'TCR',
		0x45 => 'TFA',
		0x46 => 'TCA',
		0x51 => 'RST',
		0x52 => 'RSR',
		0x53 => 'RCP',
		0x54 => 'RCR',
		0x61 => 'LIN',
		0x62 => 'LUN',
		0x63 => 'LIA',
		0x64 => 'LUA',
		0x65 => 'LID',
		0x66 => 'LFU',
		0x67 => 'LLT',
		0x68 => 'LRT',
		0x71 => 'TRA',
		0x72 => 'TRW',
		0x81 => 'DLC',
		0x82 => 'CSS',
		0x83 => 'CNS',
		0x84 => 'CNP',
		0xa1 => 'UPU',
		0xa2 => 'UPA',
		0xa3 => 'UPT',
	},
};
sub unpack {
	my $self = shift;
	$self->{snmm}{off} = $self->{off};
	$self->{snmm}{end} = $self->{end};
	$self->{mt} = unpack('C',substr(${$self->{buf}},$self->{off},1)); $self->{off} += 1;
	$self->{h0} = ($self->{mt}>>0)&0xf;
	$self->{h1} = ($self->{mt}>>4)&0xf;
	$self->{mt} = ($self->{h0}<<4)|$self->{h1};
	my $type = SNMM_TYPES->{$self->{mt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self->unpack(@_);
}
sub dissect {
	my $self = shift;
	my $off = $self->{beg};
	my $mt = $self->{mt}; $mt = SNMM_TYPES->{$mt}.sprintf('(%02x)',$mt);
	return "$mt Message", [
		[$off+0,Frame::bytes($self->{h0},0x0f,1),'H0',sprintf('0x%1X',$self->{h0})],
		[$off+0,Frame::bytes($self->{h1},0xf0,1),'H1',sprintf('0x%1X',$self->{h1})],
		@_,
	];
}
# -------------------------------------
package Message::ITUT::SNMM; our @ISA = qw(Message::SNMM);
package Message::ANSI::SNMM; our @ISA = qw(Message::SNMM);
# -------------------------------------

# -------------------------------------
package Message::ITUT::SNMM::COM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{fsnl} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{fsnl} &= 0x7f;
	$self->{slc} = $self->{sls};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $fsnl0 = unpack('C',substr(${$self->{buf}},$off,1)) >> 7
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{fsnl},0x7f,1),'FSNL',$self->{fsnl}],
		[$off+0,Frame::spare($fsnl0,0x80,1),'Spare',$fsnl0],
	);
}
package Message::ITUT::SNMM::COO; our @ISA = qw(Message::ITUT::SNMM::COM);
package Message::ITUT::SNMM::COA; our @ISA = qw(Message::ITUT::SNMM::COM);
# -------------------------------------
package Message::ANSI::SNMM::COM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{fsnl} = unpack('v',substr(${$self->{buf}},$off,2)); $off+=2;
	$self->{slc} = $self->{fsnl} & 0x0f;
	$self->{fsnl} >>= 4;
	$self->{fsnl} &= 0x7f;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $fsnl0 = unpack('C',substr(${$self->{buf}},$off+1))>>3;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{slc}, 0x000f,2),'SLC', $self->{slc}],
		[$off+0,Frame::bytes($self->{fsnl},0x07f0,2),'FSNL',$self->{fsnl}],
		[$off+0,Frame::spare($fsnl0,       0xf800,2),'Spare',$fsnl0],
	);
}
package Message::ANSI::SNMM::COO; our @ISA = qw(Message::ANSI::SNMM::COM);
package Message::ANSI::SNMM::COA; our @ISA = qw(Message::ANSI::SNMM::COM);
# -------------------------------------
package Message::ITUT::SNMM::CBM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{cbc} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{slc} = $self->{sls};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{cbc},0xff,1),'CBC',$self->{cbc}],
	);
}
package Message::ITUT::SNMM::CBD; our @ISA = qw(Message::ITUT::SNMM::CBM);
package Message::ITUT::SNMM::CBA; our @ISA = qw(Message::ITUT::SNMM::CBM);
# -------------------------------------
package Message::ANSI::SNMM::CBM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{cbc} = unpack('v',substr(${$self->{buf}},$off,2)); $off+=2;
	$self->{slc} = $self->{cbc} & 0x0f;
	$self->{cbc} >>= 4;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{slc},0x000f,2),'SLC',$self->{slc}],
		[$off+0,Frame::bytes($self->{cbc},0xfff0,2),'CBC',$self->{cbc}],
	);
}
package Message::ANSI::SNMM::CBD; our @ISA = qw(Message::ANSI::SNMM::CBM);
package Message::ANSI::SNMM::CBA; our @ISA = qw(Message::ANSI::SNMM::CBM);
# -------------------------------------
package Message::ITUT::SNMM::SLM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{slc} = $self->{sls};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect();
}
package Message::ITUT::SNMM::ECO; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::ECA; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LIN; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LUN; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LIA; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LUA; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LID; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LFU; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LLT; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::LRT; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::CSS; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::CNS; our @ISA = qw(Message::ITUT::SNMM::SLM);
package Message::ITUT::SNMM::CNP; our @ISA = qw(Message::ITUT::SNMM::SLM);
# -------------------------------------
package Message::ANSI::SNMM::SLM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{slc} = unpack('C',substr(${$self->{buf}},$off,1)) & 0x0f; $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $slc0 = unpack('C',substr(${$self->{buf}},$off,1))>>4;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{slc},0x0f,1),'SLC',$self->{slc}],
		[$off+0,Frame::spare($slc0,       0xf0,1),'Spare',$slc0],
	);
}
package Message::ANSI::SNMM::ECO; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::ECA; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LIN; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LUN; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LIA; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LUA; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LID; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LFU; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LLT; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::LRT; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::CSS; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::CNS; our @ISA = qw(Message::ANSI::SNMM::SLM);
package Message::ANSI::SNMM::CNP; our @ISA = qw(Message::ANSI::SNMM::SLM);
# -------------------------------------
package Message::SNMM::SIF; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect();
}
# -------------------------------------
package Message::ITUT::SNMM::SIF; our @ISA = qw(Message::SNMM::SIF);
package Message::ITUT::SNMM::RCT; our @ISA = qw(Message::ITUT::SNMM::SIF);
package Message::ITUT::SNMM::TRA; our @ISA = qw(Message::ITUT::SNMM::SIF);
package Message::ITUT::SNMM::TRW; our @ISA = qw(Message::ITUT::SNMM::SIF);
package Message::ANSI::SNMM::SIF; our @ISA = qw(Message::SNMM::SIF);
package Message::ANSI::SNMM::RCT; our @ISA = qw(Message::ANSI::SNMM::SIF);
package Message::ANSI::SNMM::TRA; our @ISA = qw(Message::ANSI::SNMM::SIF);
package Message::ANSI::SNMM::TRW; our @ISA = qw(Message::ANSI::SNMM::SIF);
# -------------------------------------
package Message::ITUT::SNMM::TFC; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('v',substr(${$self->{buf}},$off,2)); $off+=2;
	$self->{stat} = ($self->{dest} >> 14) & 0x3;
	$self->{dest} &= 0x3fff;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>11)&0x7).'-'.(($dest>>3)&0xff).'-'.(($dest>>0)&0x7);
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x3fff,2),'DEST',$dest],
		[$off+0,Frame::bytes($self->{stat},0xc000,2),'STAT',$self->{stat}],
	);
}
#package Message::ITUT::SNMM::TFC; our @ISA = qw(Message::ITUT::SNMM);
# -------------------------------------
package Message::ANSI::SNMM::TFC; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('V',substr(${$self->{buf}},$off,4)); $off+=4;
	$self->{stat} = ($self->{dest} >> 24) & 0x3;
	$self->{dest} &= 0x00ffffff;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>16)&0xff).'-'.(($dest>>8)&0xff).'-'.(($dest>>0)&0xff);
	my $stat0 = unpack('C',substr(${$self->{buf}},$off+3,1))>>2;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x00ffffff,4),'DEST',$dest],
		[$off+0,Frame::bytes($self->{stat},0x03000000,4),'STAT',$self->{stat}],
		[$off+0,Frame::spare($stat0,       0xfc000000,4),'Spare',$stat0],
	);
}
#package Message::ANSI::SNMM::TFC; our @ISA = qw(Message::ANSI::SNMM);
# -------------------------------------
package Message::ITUT::SNMM::TFM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('v',substr(${$self->{buf}},$off,2)) & 0x3fff; $off+=2;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>11)&0x7).'-'.(($dest>>3)&0xff).'-'.(($dest>>0)&0x7);
	my $dest0 = unpack('C',substr(${$self->{buf}},$off+1,1))>>6;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x3fff,2),'DEST',$dest],
		[$off+0,Frame::bytes($dest0,       0xc000,2),'Spare',$dest0],
	);
}
package Message::ITUT::SNMM::TFP; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::TCP; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::TFR; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::TCR; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::TFA; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::TCA; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::RST; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::RSR; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::RCP; our @ISA = qw(Message::ITUT::SNMM::TFM);
package Message::ITUT::SNMM::RCR; our @ISA = qw(Message::ITUT::SNMM::TFM);
# -------------------------------------
package Message::ANSI::SNMM::TFM; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('V',substr(${$self->{buf}}.pack('C',0),$off,4)) & 0x00ffffff; $off+=4;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>16)&0xff).'-'.(($dest>>8)&0xff).'-'.(($dest>>0)&0xff);
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x00ffffff,4),'DEST',$dest],
	);
}
package Message::ANSI::SNMM::TFP; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::TCP; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::TFR; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::TCR; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::TFA; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::TCA; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::RST; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::RSR; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::RCP; our @ISA = qw(Message::ANSI::SNMM::TFM);
package Message::ANSI::SNMM::RCR; our @ISA = qw(Message::ANSI::SNMM::TFM);
# -------------------------------------
package Message::ITUT::SNMM::DLC; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{sdli} = unpack('v',substr(${$self->{buf}},$off,2)) & 0x0fff; $off+=2;
	$self->{slc} = $self->{sls};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $sdli0 = unpack('C',substr(${$self->{buf}},$off+1,1)) >> 4;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{sdli},0x0fff,2),'SDLI',$self->{sdli}],
		[$off+0,Frame::spare($sdli0,       0xf000,2),'Spare',$sdli0],
	);
}
#package Message::ITUT::SNMM::DLC; our @ISA = qw(Message::ITUT::SNMM);
# -------------------------------------
package Message::ANSI::SNMM::DLC; our @ISA = qw(Message::SNMM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{sdli} = unpack('V',substr(${$self->{buf}}.pack('C',0),$off,4)); $off+=4;
	$self->{slc} = $self->{sdli} & 0x0f;
	$self->{sdli} >>= 4;
	$self->{sdli} &= 0x3fff;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $sdli0 = unpack('C',substr(${$self->{buf}},$off+2,1)) >> 2;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{slc}, 0x00000f,3),'SLC', $self->{slc}],
		[$off+0,Frame::bytes($self->{sdli},0x03fff0,3),'SDLI',$self->{sdli}],
		[$off+0,Frame::spare($sdli0,       0xfc0000,3),'Spare',$sdli0],
	);
}
#package Message::ANSI::SNMM::DLC; our @ISA = qw(Message::ANSI::SNMM);
# -------------------------------------
package Message::ITUT::SNMM::UPM; our @ISA = qw(Message::SNMM);
# -------------------------------------
use constant {
	UPI_VALUES=>[
		'Spare', 'Spare',  'Spare',  'SCCP',
		'TUP',   'ISUP',   'DUP',    'Spare',
		'MTUP',  'B-ISUP', 'S-ISUP', 'Spare',
		'Spare', 'Spare',  'Spare',  'Spare',
	],
};
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('v',substr(${$self->{buf}},$off,2)) & 0x3fff; $off+=2;
	$self->{upi}  = unpack('C',substr(${$self->{buf}},$off,1)) & 0x0f;   $off+=1;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>11)&0x7).'-'.(($dest>>3)&0xff).'-'.(($dest>>0)&0x7);
	my $dest0 = unpack('C',substr(${$self->{buf}},$off+1,1))>>6;
	my $upi = $self->{upi}; $upi = UPI_VALUES->[$upi]."($upi)";
	my $upi0 = unpack('C',substr(${$self->{buf}},$off+2,1))>>4;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x003fff,3),'DEST',$dest],
		[$off+0,Frame::spare($dest0,       0x00c000,3),'Spare',$dest0],
		[$off+0,Frame::bytes($self->{upi}, 0x0f0000,3),'UPI',$upi],
		[$off+0,Frame::spare($upi0,        0xf00000,3),'Spare',$upi0],
	);
}
package Message::ITUT::SNMM::UPU; our @ISA = qw(Message::ITUT::SNMM::UPM);
package Message::ITUT::SNMM::UPA; our @ISA = qw(Message::ITUT::SNMM::UPM);
package Message::ITUT::SNMM::UPT; our @ISA = qw(Message::ITUT::SNMM::UPM);
# -------------------------------------
package Message::ANSI::SNMM::UPM; our @ISA = qw(Message::SNMM);
# -------------------------------------
use constant {
	UPI_VALUES=>[
		'Spare', 'Spare',  'Spare',  'SCCP',
		'TUP',   'ISUP',   'DUP',    'Spare',
		'MTUP',  'B-ISUP', 'S-ISUP', 'Spare',
		'Spare', 'Spare',  'Spare',  'Spare',
	],
};
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{dest} = unpack('V',substr(${$self->{buf}},$off,4)); $off+=4;
	$self->{upi} = ($self->{dest} >> 24) & 0x0f;
	$self->{dest} &= 0x00ffffff;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $dest = $self->{dest}; $dest = (($dest>>16)&0xff).'-'.(($dest>>8)&0xff).'-'.(($dest>>0)&0xff);
	my $upi = $self->{upi}; $upi = UPI_VALUES->[$upi]."($upi)";
	my $upi0 = unpack('C',substr(${$self->{buf}},$off+3,1))>>4;
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{dest},0x00ffffff,4),'DEST',$dest],
		[$off+0,Frame::bytes($self->{upi}, 0x0f000000,4),'UPI',$upi],
		[$off+0,Frame::spare($upi0,        0xf0000000,4),'Spare',$upi0],
	);
}
package Message::ANSI::SNMM::UPU; our @ISA = qw(Message::ANSI::SNMM::UPM);
package Message::ANSI::SNMM::UPA; our @ISA = qw(Message::ANSI::SNMM::UPM);
package Message::ANSI::SNMM::UPT; our @ISA = qw(Message::ANSI::SNMM::UPM);
# -------------------------------------

# -------------------------------------
package Message::SNTM; our @ISA = qw(Message::MTP3::MGMT);
# -------------------------------------
use constant {
	SNTM_TYPES=>{
		0x11 => 'SLTM',
		0x12 => 'SLTA',
	},
};
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{mt} = unpack('C',substr(${$self->{buf}},$off,1)); $off+=1;
	$self->{h0} = ($self->{mt}>>0)&0xf;
	$self->{h1} = ($self->{mt}>>4)&0xf;
	$self->{mt} = ($self->{h0}<<4)|$self->{h1};
	my $type = SNTM_TYPES->{$self->{mt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $mt = $self->{mt}; $mt = SNTM_TYPES->{$mt}.sprintf('(%02x)',$mt);
	return [
		@{$self->SUPER::dissect(@_)},
		"$mt Message",
		[$off+0,Frame::bytes($self->{h0},0x0f,1),'H0',sprintf('0x%1X',$self->{h0})],
		[$off+0,Frame::bytes($self->{h1},0xf0,1),'H1',sprintf('0x%1X',$self->{h1})],
	];
}
# -------------------------------------
package Message::ITUT::SNTM; our @ISA = qw(Message::SNTM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{tli} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{slc} = $self->{sls};
	$self->{tli} >>= 4;
	$self->{tli} &= 0x0f;
	$self->{tdata} = substr(${$self->{buf}},$self->{off},$self->{tli});
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return [
		@{$self->SUPER::dissect(@_)},
		[$off+1,Frame::bytes($self->{tli},0xf0,1),'TLI',$self->{tli}],
		#TODO: print data
	];
}
# -------------------------------------
package Message::ITUT::SNTM::SLTM; our @ISA = qw(Message::ITUT::SNTM);
package Message::ITUT::SNTM::SLTA; our @ISA = qw(Message::ITUT::SNTM);
# -------------------------------------
# -------------------------------------
package Message::ANSI::SNTM; our @ISA = qw(Message::SNTM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{tli} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{slc} = $self->{tli} & 0x0f;
	$self->{tli} >>= 4;
	$self->{tli} &= 0x0f;
	$self->{tdata} = substr(${$self->{buf}},$self->{off},$self->{tli});
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return [
		@{$self->SUPER::dissect(@_)},
		[$off+1,Frame::bytes($self->{slc},0x0f,1),'SLC',$self->{slc}],
		[$off+1,Frame::bytes($self->{tli},0xf0,1),'TLI',$self->{tli}],
		#TODO: print data
	];
}
# -------------------------------------
package Message::ANSI::SNTM::SLTM; our @ISA = qw(Message::ANSI::SNTM);
package Message::ANSI::SNTM::SLTA; our @ISA = qw(Message::ANSI::SNTM);
# -------------------------------------
# -------------------------------------
package Message::SNSM; our @ISA = qw(Message::MTP3::MGMT);
# -------------------------------------
use constant {
	SNSM_TYPES=>{
		0x11 => 'SLTM',
		0x12 => 'SLTA',
	},
};
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{mt} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{h0} = ($self->{mt}>>0)&0xf;
	$self->{h1} = ($self->{mt}>>4)&0xf;
	$self->{mt} = ($self->{h0}<<4)|$self->{h1};
	my $type = SNSM_TYPES->{$self->{mt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $mt = $self->{mt}; $mt = SNSM_TYPES->{$mt}.sprintf('(%02x)',$mt);
	return [
		@{$self->SUPER::dissect(@_)},
		"$mt Message",
		[$off+0,Frame::bytes($self->{h0},0x0f,1),'H0',sprintf('0x%1X',$self->{h0})],
		[$off+0,Frame::bytes($self->{h1},0xf0,1),'H1',sprintf('0x%1X',$self->{h1})],
	];
}
# -------------------------------------
package Message::ITUT::SNSM; our @ISA = qw(Message::SNSM);
# -------------------------------------
sub unpack {
	my $self = shift;
	# doesn't exists for ITU
	return $self;
}
# -------------------------------------
package Message::ITUT::SNSM::SLTM; our @ISA = qw(Message::ITUT::SNSM);
package Message::ITUT::SNSM::SLTA; our @ISA = qw(Message::ITUT::SNSM);
# -------------------------------------
# -------------------------------------
package Message::ANSI::SNSM; our @ISA = qw(Message::SNSM);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{tli} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$self->{slc} = $self->{tli} & 0x0f;
	$self->{tli} >>= 4;
	$self->{tli} &= 0x0f;
	$self->{tdata} = substr(${$self->{buf}},$off,$self->{tli});
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		[$off+0,Frame::bytes($self->{slc},0x0f,1),'SLC',$self->{slc}],
		[$off+0,Frame::bytes($self->{tli},0xf0,1),'TLI',$self->{tli}],
		[$off+1,' ******** ','TDATA',Frame::pdata($self->{tdata})],
	);
}
# -------------------------------------
package Message::ANSI::SNSM::SLTM; our @ISA = qw(Message::ANSI::SNSM);
package Message::ANSI::SNSM::SLTA; our @ISA = qw(Message::ANSI::SNSM);
# -------------------------------------

# -------------------------------------
package Message::SCCP; our @ISA = qw(Message);
# -------------------------------------
use constant {
	SCCP_MSG_TYPE=>{
		0x01 => 'CR',
		0x02 => 'CC',
		0x03 => 'CREF',
		0x04 => 'RLSD',
		0x05 => 'RLC',
		0x06 => 'DT1',
		0x07 => 'DT2',
		0x08 => 'AK',
		0x09 => 'UDT',
		0x0a => 'UDTS',
		0x0b => 'ED',
		0x0c => 'EA',
		0x0d => 'RSR',
		0x0e => 'RSC',
		0x0f => 'ERR',
		0x10 => 'IT',
		0x11 => 'XUDT',
		0x12 => 'XUDTS',
		0x13 => 'LUDT',
		0x14 => 'LUDTS',
	},
};
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	$self->{mt} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	my $type = SCCP_MSG_TYPE->{$self->{mt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	my $layers;
	if (exists $self->{mtp3}) {
		$layers = $self->SUPER::dissect(@_);
	} else { # if (exists $self->{m3ua}) {
		$layers = $self->Message::M3UA::XFER::DATA::dissect(@_);
	}
	my $mt = $self->{mt}; $mt = SCCP_MSG_TYPE->{$mt}."($mt)";
	return [
		@{$layers},
		"$mt Message",
		[$off+0,Frame::bytes($self->{mt},0xff,1),'MT',$mt],
	];
}
use constant {
	SCCP_PT_EOP	=>  0,	# 0000 0000	End of optional parameters
	SCCP_PT_DLR	=>  1,	# 0000 0001	Destination local reference
	SCCP_PT_SLR	=>  2,	# 0000 0010	Source local reference
	SCCP_PT_CDPA	=>  3,	# 0000 0011	Called party address
	SCCP_PT_CGPA	=>  4,	# 0000 0100	Calling party address
	SCCP_PT_PCLS	=>  5,	# 0000 0101	Protocol class
	SCCP_PT_SEG	=>  6,	# 0000 0110	Segmenting/reassembling
	SCCP_PT_RSN	=>  7,	# 0000 0111	Receive sequence number
	SCCP_PT_SEQ	=>  8,	# 0000 1000	Sequencing/segmenting
	SCCP_PT_CRED	=>  9,	# 0000 1001	Credit
	SCCP_PT_RELC	=> 10,	# 0000 1010	Release cause
	SCCP_PT_RETC	=> 11,	# 0000 1011	Return cause (formerly diagnostic)
	SCCP_PT_RESC	=> 12,	# 0000 1100	Reset cause
	SCCP_PT_ERRC	=> 13,	# 0000 1101	Error cause
	SCCP_PT_REFC	=> 14,	# 0000 1110	Refusal cause
	SCCP_PT_DATA	=> 15,	# 0000 1111	Data
	SCCP_PT_SGMT	=> 16,	# 0001 0000	Segmentation
	SCCP_PT_HOPC	=> 17,	# 0001 0001	SCCP hop counter
	SCCP_PT_IMP	=> 18,	# 0001 0010	Importance (not ANSI)
	SCCP_PT_LDATA	=> 19,	# 0001 0011	Long data
	SCCP_PT_MTI	=>248,	# 1111 1000	Message type interworking
	SCCP_PT_INS	=>249,	# 1111 1001	Intermediate network selection
	SCCP_PT_ISNI	=>250,	# 1111 1010	Intermediate signalling network identification

	SCCP_PARM_TYPE=>{
		&SCCP_PT_EOP	=> 'EOP',
		&SCCP_PT_DLR	=> 'DLR',
		&SCCP_PT_SLR	=> 'SLR',
		&SCCP_PT_CDPA	=> 'CDPA',
		&SCCP_PT_CGPA	=> 'CGPA',
		&SCCP_PT_PCLS	=> 'PCLS',
		&SCCP_PT_SEG	=> 'SEG',
		&SCCP_PT_RSN	=> 'RSN',
		&SCCP_PT_SEQ	=> 'SEQ',
		&SCCP_PT_CRED	=> 'CRED',
		&SCCP_PT_RELC	=> 'RELC',
		&SCCP_PT_RETC	=> 'RETC',
		&SCCP_PT_RESC	=> 'RESC',
		&SCCP_PT_ERRC	=> 'ERRC',
		&SCCP_PT_REFC	=> 'REFC',
		&SCCP_PT_DATA	=> 'DATA',
		&SCCP_PT_SGMT	=> 'SGMT',
		&SCCP_PT_HOPC	=> 'HOPC',
		&SCCP_PT_IMP	=> 'IMP',
		&SCCP_PT_LDATA	=> 'LDATA',
		&SCCP_PT_MTI	=> 'MTI',
		&SCCP_PT_INS	=> 'INS',
		&SCCP_PT_ISNI	=> 'ISNI',
	}
};

sub unpack_SCCP_ADDR {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $off < $end;
	$self->{$lab} = {};
	$lab = $self->{$lab};
	my $ai = $lab->{ai} = unpack('C',substr(${$self->{buf}},$off,1)); $off++;
	$lab->{ni}  = ($ai >> 7) & 0x01;
	$lab->{ri}  = ($ai >> 6) & 0x01;
	$lab->{gti} = ($ai >> 2) & 0x0f;
	$lab->{ssi} = ($ai >> 1) & 0x01;
	$lab->{pci} = ($ai >> 0) & 0x01;
	if ($lab->{ni} == 0) {
		if ($lab->{pci}) {
			return unless $off + 2 <= $end;
			$lab->{pc} = unpack('v',substr(${$self->{buf}},$off,2)) & 0x3fff;
			$off += 2;
		}
		$lab->{ssn} = 0;
		if ($lab->{ssi}) {
			return unless $off + 1 <= $end;
			$lab->{ssn} = unpack('C',substr(${$self->{buf}},$off,1));
			$off += 1;
		}
		if ($lab->{gti}) {
			if (1 > $lab->{gti} or $lab->{gti} > 4) {
				return 1;
			} elsif ($lab->{gti} == 1) {
				return unless $off + 1 <= $end;
				my $nai = unpack('C',substr(${$self->{buf}},$off,1));
				$off += 1;
				$lab->{oe}  = ($nai >> 7) & 0x01;
				$lab->{nai} = ($nai >> 0) & 0x7f;
				$lab->{es}  = 2 if $lab->{oe} == 0;
				$lab->{es}  = 1 if $lab->{oe} == 1;
			} elsif ($lab->{gti} == 2) {
				return unless $off + 1 <= $end;
				$lab->{tt} = unpack('C',substr(${$self->{buf}},$off,1));
				$off += 1;
			} elsif ($lab->{gti} == 3) {
				return unless $off + 2 <= $end;
				my $ne;
				($lab->{tt},$ne) = unpack('CC',substr(${$self->{buf}},$off,2));
				$off += 2;
				$lab->{np} = ($ne>>4) & 0x0f;
				$lab->{es} = ($ne>>0) & 0x0f;
			} elsif ($lab->{gti} == 4) {
				return unless $off + 3 <= $end;
				my ($ne,$nai);
				($lab->{tt},$ne,$nai) = unpack('CCC',substr(${$self->{buf}},$off,3));
				$off += 3;
				$lab->{np}  = ($ne >>4) & 0x0f;
				$lab->{es}  = ($ne >>0) & 0x0f;
				$lab->{oe}  = ($nai>>7) & 0x01; # should always be zero
				$lab->{nai} = ($nai>>0) & 0x7f;
			}
		}
	} else {
		$lab->{ssn} = 0;
		if ($lab->{ssi}) {
			return unless $off + 1 <= $end;
			$lab->{ssn} = unpack('C',substr(${$self->{buf}},$off,1));
			$off += 1;
		}
		if ($lab->{pci}) {
			return unless $off + 3 <= $end;
			$lab->{pc} = unpack('V',substr(${$self->{buf}},$off-1,4)) & 0xffffff;
			$off += 3;
		}
		if ($lab->{gti}) {
			if (1 > $lab->{gti} or $lab->{gti} > 2) {
				return 1;
			} elsif ($lab->{gti} == 1) {
				return unless $off + 2 <= $end;
				my $ne;
				($lab->{tt},$ne) = unpack('CC',substr(${$self->{buf}},$off,2));
				$off += 2;
				$lab->{np} = ($ne>>4) & 0x0f;
				$lab->{es} = ($ne>>0) & 0x0f;
			} elsif ($lab->{gti} == 2) {
				return unless $off + 1 <= $end;
				$lab->{tt} = unpack('C',substr(${$self->{buf}},$off,1));
				$off += 1;
			}
		}
	}
	if ($lab->{gti}) {
		if ($lab->{es} == 1 or $lab->{es} == 2) {
			$lab->{add} = '';
			foreach (unpack('C*',substr(${$self->{buf}},$off,$end-$off))) {
				$lab->{add}.=sprintf('%1X',($_>>0)&0x0f);
				$lab->{add}.=sprintf('%1X',($_>>4)&0x0f);
			}
			$lab->{add} = substr($lab->{add},0,length($lab->{add})-1) if $lab->{es} == 1;
		} else {
			$lab->{add} = substr(${$self->{buf}},$off,$end-$off);
		}
	}
	return 1;
}
sub unpack_SCCP_LOCR {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len == 3;
	$self->{$lab} = unpack('V',substr(${$self->{buf}},$off-1,$len+1)) & 0xffffff;
	return 1;
}
sub unpack_SCCP_1OCT {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{$lab} = unpack('C',substr(${$self->{buf}},$off,1));
	return 1;
}

sub unpack_PT_EOP {
	my ($self,$off,$end,$len,$lab) = @_;
	return 2;
}
sub unpack_PT_DLR	{ return shift->Message::SCCP::unpack_SCCP_LOCR(@_) }
sub unpack_PT_SLR	{ return shift->Message::SCCP::unpack_SCCP_LOCR(@_) }
sub unpack_PT_CDPA	{ return shift->Message::SCCP::unpack_SCCP_ADDR(@_) }
sub unpack_PT_CGPA	{ return shift->Message::SCCP::unpack_SCCP_ADDR(@_) }
sub unpack_PT_PCLS	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_SEG	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_RSN	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_SEQ {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{$lab} = {};
	$lab = $self->{$lab};
	($lab->{ps},$lab->{pr}) = unpack('CC',substr(${$self->{buf}},$off,2));
	$lab->{m} = $lab->{pr} & 0x1;
	$lab->{pr} &= 0xfe;
	$lab->{ps} &= 0xfe;
	return 1;
}
sub unpack_PT_CRED	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_RELC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_RETC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_RESC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_ERRC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_REFC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_DATA {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $len <= 252;
	$self->{doff} = $off;
	$self->{dlen} = $len;
	$self->{dend} = $off + $len;
	return 1;
}
sub unpack_PT_SGMT {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $off + 4 <= $end;
	$self->{$lab} = {};
	$lab = $self->{$lab};
	my $val = unpack('V',substr(${$self->{buf}},$off,4));
	$lab->{rems} = ($val >> 0) & 0x0f;
	$lab->{isdo} = ($val >> 6) & 0x01;
	$lab->{flag} = ($val >> 7) & 0x01;
	$lab->{lref} = ($val >> 8) & 0xffffff;
	return 1;
}
sub unpack_PT_HOPC	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_IMP	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_LDATA {
	my ($self,$off,$end,$len,$lab) = @_;
	return unless $le <= 3904;
	$self->{doff} = $off;
	$self->{dlen} = $len;
	$self->{dend} = $off + $len;
	return 1;
}
sub unpack_PT_MTI	{ return shift->Message::SCCP::unpack_SCCP_1OCT(@_) }
sub unpack_PT_INS {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{$lab} = substr(${$self->{buf}},$off,$len); # just binary copy for now
	return 1;
}
sub unpack_PT_ISNI {
	my ($self,$off,$end,$len,$lab) = @_;
	$self->{$lab} = substr(${$self->{buf}},$off,$len); # just binary copy for now
	return 1;
}

sub unpack_oparms {
	my ($self,$ptr,$tag,$len,$parm,$sub) = @_;
	for (;;) {
		return unless $ptr < $self->{end};
		$tag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		return 1 if $tag == 0;
		return unless exists SCCP_PARM_TYPE->{$tag};
		$parm = SCCP_PARM_TYPE->{$tag};
		return unless $ptr < $self->{end};
		$len = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		return if $len == 0;
		return unless $ptr + $len <= $self->{end};
		return unless $sub = $self->can("unpack_PT_$parm");
		return unless $sub->($self,$ptr,$len);
		$ptr += $len;
	}
}

sub dissect_PT_EOP {
	my ($self,$off,$end,$len) = @_;
	return;
}
sub dissect_PT_DLR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SLR {
	my ($self,$off,$end,$len) = @_;
}
use constant {
	ITUT_GTI_TYPES=>{
		0x0=>'no global title included',
		0x1=>'global title includes nature of address indicator only',
		0x2=>'global title includes translation type only',
		0x3=>'global title includes translation type, numbering plan and encoding scheme',
		0x4=>'global title includes translation type, numbering plan, encoding scheme and nature of address indicator',
	},
	ANSI_GTI_TYPES=>{
		0x0=>'no global title included',
		0x1=>'global title includes translation type, numbering plan and encoding scheme',
		0x2=>'global title includes translation type only',
	},
	RI_TYPES=>{
		0x0=>'Route on global title',
		0x1=>'Route on subsystem number',
	},
	SSN_TYPES=>{
		0x00=>'SSN not known/not used',
		0x01=>'SCCP management',
		0x02=>'Reserved for ITU-T allocation',
		0x03=>'ISDN user part',
		0x04=>'Operation, maintenance and administration part (OMAP)',
		0x05=>'Mobile application part (MAP)',
		0x06=>'Home location register (HLR)',
		0x07=>'Visitor location register (VLR)',
		0x08=>'Mobile switching center (MSC)',
		0x09=>'Equipment identifier centre (EIC)',
		0x0a=>'Authentication center (AUC)',
		0x0b=>'Reserved for ITU-T for ISDN supplementary services/MOSMS',
		0x0c=>'Reserved for international use',
		0x0d=>'Reserved for ITU-T for broadband ISDN edge-to-edge applications',
		0x0e=>'Reserved for ITU-T for TC test responder',
		0x0f=>'Reserved for international use',
		0x1f=>'Reserved for international use',
		0x20=>'Spare',
		0xc0=>'Enhanced 911 location testing (E911)',
		0xc1=>'Enhanced 911 testing (E911)',
		0xeb=>'Voicemail (message waiting)',
		0xf7=>'Local number portability (LNP)',
		0xf8=>'Local number portability (LNP)',
		0xfa=>'LNP Wireline and Wireless/One-way outgoing CMS',
		0xfb=>'CLASS/Call Management services',
		0xfc=>'PVN/Billed Number Screening',
		0xfd=>'LIDB Query (GR-945-CORE)/Automated Calling Card Services',
		0xfe=>'800 Number Translation/800 Plus/Toll Free services',
		0xff=>'Reserved for expansion',
	},
	NAI_TYPES=>{
		0x00=>'unknown',
		0x01=>'subscriber number',
		0x02=>'reserved for national use',
		0x03=>'national significant number',
		0x04=>'international number',
	},
	OE_TYPES=>{
		0x0=>'even number of address signals',
		0x1=>'odd number of address signals',
	},
	NP_TYPES=>{
		0x0=>'unknown',
		0x1=>'ISDN/telephony numbering plan (ITU-T E.163/164)',
		0x2=>'generic numbering plan',
		0x3=>'data numbering plan (ITU-T X.121)',
		0x4=>'telex numbering plan (ITU-T F.69)',
		0x5=>'maritime mobile numbering plan (ITU-T E.210/211)',
		0x6=>'land mobile numbering plan (ITU-T E.212)',
		0x7=>'ISDN/mobile numbering plan (ITU-T E.214)',
	},
	ES_TYPES=>{
		0x0=>'unknown',
		0x1=>'BCD, odd number of digits',
		0x2=>'BCD, even number of digits',
		0x3=>'national specific',
	},
	RELC_TYPES=>{
		0x00=>'end user originated',
		0x01=>'end user busy',
		0x02=>'end user failure',
		0x03=>'SCCP user originated',
		0x04=>'remote procedure error',
		0x05=>'inconsistent connection data',
		0x06=>'access failure',
		0x07=>'access congestion',
		0x08=>'subsystem failure',
		0x09=>'subsystem congestion',
		0x0a=>'MTP failure',
		0x0b=>'network congestion',
		0x0c=>'expiration of reset timer',
		0x0d=>'expiration of receive inactivity timer',
		0x0e=>'reserved',
		0x0f=>'unqualified',
		0x10=>'SCCP failure (ITU-T only)',
	},
	RETC_TYPES=>{
		0x00=>'no translation for an address of such nature',
		0x01=>'no translation for this specific address',
		0x02=>'subsystem congestion',
		0x03=>'subsystem failure',
		0x04=>'unequipped user',
		0x05=>'MTP failure',
		0x06=>'network congestion',
		0x07=>'unqualified',
		0x08=>'error in message transport',
		0x09=>'error in local processing',
		0x0a=>'destination cannot perform reassembly',
		0x0b=>'SCCP failure (ITU-T only)',
		0x0c=>'SCCP hop counter violation',
		0x0d=>'segmentation not supported',
		0x0e=>'segmentation failure',
		0xf7=>'message change failure',
		0xf8=>'invalid INS routing request',
		0xf9=>'invalid INSI routing request',
		0xfa=>'unauthorized message',
		0xfb=>'message incompatibility',
		0xfc=>'cannot perform ISNI constrained routing',
		0xfd=>'redundant ISNI constrained routing information',
		0xfe=>'unable to perform ISNI identification',
	},
	RESC_TYPES=>{
		0x00=>'end user originated',
		0x01=>'SCCP user originated',
		0x02=>'message out of order - incorrect P(S)',
		0x03=>'message out of order - incorrect P(R)',
		0x04=>'remote procedure error - message out of window',
		0x05=>'remote procedure error - incorrect P(S) after (re)init',
		0x06=>'remote procedure error - general',
		0x07=>'remote end user operational',
		0x08=>'network operational',
		0x09=>'access operational',
		0x0a=>'network congestion',
		0x0b=>'not obtainable (reserved)',
		0x0c=>'unqualified',
	},
	ERRC_TYPES=>{
		0x00=>'LRN mismatch - unassigned destination LRN',
		0x01=>'LRN mismatch - inconsistent source LRN',
		0x02=>'point code mismatch',
		0x03=>'service class mismatch',
		0x04=>'unqualified',
	},
	REFC_TYPES=>{
		0x00=>'end user originated',
		0x01=>'end user congestion',
		0x02=>'end user failure',
		0x03=>'SCCP user originated',
		0x04=>'destination address unknown',
		0x05=>'destination inaccessible',
		0x06=>'network resource - QoS not available/non-transient',
		0x07=>'network resource - QoS not available/transient',
		0x08=>'access failure',
		0x09=>'access congestion',
		0x0a=>'subsytem failure',
		0x0b=>'subsystem congestion',
		0x0c=>'expiration of the connection establishment timer',
		0x0d=>'incompatible user data',
		0x0e=>'reserved',
		0x0f=>'unqualified',
		0x10=>'SCCP hop counter violation',
		0x11=>'SCCP faliure (ITU-T only)',
		0x12=>'no translation for an address of such nature',
		0x13=>'unequipped user',
	},
	ADIGIT_TYPES=>['0','1','2','3','4','5','6','7','8','9','a','*','#','d','e','ST'],
	ANSI_TT_VALUES=>{
		0x00=>'reserved',
		0x01=>'identification cards',
		0x02=>'reserved - 14 digit calling card (note 1)',
		0x03=>'cellular nationwide roaming service/OTA provisioning and IS41 roaming',
		0x04=>'global title = point code',
		0x05=>'calling name delivery',
		0x06=>'reserved - cal management (note 1)',
		0x07=>'message waiting',
		0x08=>'SCP assisted call processing',
		0x09=>'national and international cellulsar/PCS roaming',
		0x0a=>'PCS call delivery/network entity addressing/LNP wireline',
		0x0b=>'internetwork NP query/response (NP Q/R)/wireless LNP',
		0x0c=>'wireless MIN-based short message service/MOSMS',
		0x0d=>'wireless IMSI-based short message service',
		0x0e=>'mobile subscriber addressing/wireless services impacted by number portability',
		0x0f=>'package data interworking',
		0x10=>'cellular/PCS interworking',
		0x11=>'mobile subscriber message center addressing',
		0x12=>'internetwork applications',
		0x13=>'internetwork applications',
		0x14=>'internetwork applications',
		0x15=>'internetwork applications',
		0x16=>'internetwork applications',
		0x17=>'internetwork applications',
		0x18=>'internetwork applications',
		0x19=>'internetwork applications',
		0x1a=>'internetwork applications',
		0x1b=>'internetwork applications',
		0x1c=>'14 digit telecommunication calling cards - post 10-digit (NP) GTT (loop code) (note 3)',
		0x1d=>'calling name delivery post-10-digit (NP) GTT (loop code) (note 3)',
		0x1e=>'call management post-10-digit (NP) GTT (loop code) (note 3)',
		0x1f=>'message waiting post-10-digit (NP) GTT (loop code) (note 3)',
		0x77=>'E911 (another E911 assigment)',
		0x8c=>'GSM Voice roaming MSISDN to HLR',
		0xbf=>'E911',
		0xc0=>'E911 location testing',
		0xc1=>'E911 testing',
		0xc2=>'E911 testing',
		0xd3=>'IN type Wireline LNP query',
		0xf9=>'network specific applications',
		0xfa=>'network specific applications',
		0xfb=>'network specific applications/call management',
		0xfc=>'network specific applications',
		0xfd=>'network specific applications/14 digit calling card/LIDB GR-945-CORE',
		0xfe=>'800 service',
		0xff=>'reserved',
	},
};
sub dissect_PT_CDPA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CGPA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PCLS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SEG {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RSN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SEQ {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CRED {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RELC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RETC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RESC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ERRC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_REFC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_DATA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SGMT {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_HOPC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_IMP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_LDATA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_MTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_INS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ISNI {
	my ($self,$off,$end,$len) = @_;
}

use constant {
	SCCP_PARMS_DISSECT=>{
		&SCCP_PT_EOP	=> [\&Message::SCCP::dissect_PT_EOP,	'EOP',	'End of optional parameters'],
		&SCCP_PT_DLR	=> [\&Message::SCCP::dissect_PT_DLR,	'DLR',	'Destination local reference'],
		&SCCP_PT_SLR	=> [\&Message::SCCP::dissect_PT_SLR,	'SLR',	'Source local reference'],
		&SCCP_PT_CDPA	=> [\&Message::SCCP::dissect_PT_CDPA,	'CDPA',	'Called party address'],
		&SCCP_PT_CGPA	=> [\&Message::SCCP::dissect_PT_CGPA,	'CGPA',	'Calling party address'],
		&SCCP_PT_PCLS	=> [\&Message::SCCP::dissect_PT_PCLS,	'PCLS',	'Protocol class'],
		&SCCP_PT_SEG	=> [\&Message::SCCP::dissect_PT_SEG,	'SEG',	'Segmenting/reassembling'],
		&SCCP_PT_RSN	=> [\&Message::SCCP::dissect_PT_RSN,	'RSN',	'Receive sequence number'],
		&SCCP_PT_SEQ	=> [\&Message::SCCP::dissect_PT_SEQ,	'SEQ',	'Sequencing/segmenting'],
		&SCCP_PT_CRED	=> [\&Message::SCCP::dissect_PT_CRED,	'CRED',	'Credit'],
		&SCCP_PT_RELC	=> [\&Message::SCCP::dissect_PT_RELC,	'RELC',	'Release cause'],
		&SCCP_PT_RETC	=> [\&Message::SCCP::dissect_PT_RETC,	'RETC',	'Return cause (formerly diagnostic)'],
		&SCCP_PT_RESC	=> [\&Message::SCCP::dissect_PT_RESC,	'RESC',	'Reset cause'],
		&SCCP_PT_ERRC	=> [\&Message::SCCP::dissect_PT_ERRC,	'ERRC',	'Error cause'],
		&SCCP_PT_REFC	=> [\&Message::SCCP::dissect_PT_REFC,	'REFC',	'Refusal cause'],
		&SCCP_PT_DATA	=> [\&Message::SCCP::dissect_PT_DATA,	'DATA',	'Data'],
		&SCCP_PT_SGMT	=> [\&Message::SCCP::dissect_PT_SGMT,	'SGMT',	'Segmentation'],
		&SCCP_PT_HOPC	=> [\&Message::SCCP::dissect_PT_HOPC,	'HOPC',	'SCCP hop counter'],
		&SCCP_PT_IMP	=> [\&Message::SCCP::dissect_PT_IMP,	'IMP',	'Importance (not ANSI)'],
		&SCCP_PT_LDATA	=> [\&Message::SCCP::dissect_PT_LDATA,	'LDATA','Long data'],
		&SCCP_PT_MTI	=> [\&Message::SCCP::dissect_PT_MTI,	'MTI',	'Message type interworking'],
		&SCCP_PT_INS	=> [\&Message::SCCP::dissect_PT_INS,	'INS',	'Intermediate network selection'],
		&SCCP_PT_ISNI	=> [\&Message::SCCP::dissect_PT_ISNI,	'ISNI',	'Intermediate signalling network identification'],
	}
};

sub dissect_oparms {
	my ($self,$off,$end) = @_;
	my @oparms = ();
	for (;$off < $end;$off++) {
		my @fields = ();
		my $ptr = unpack('C',substr(${$self->{buf}},$off,1));
		push @fields, [$off,Frame::bytes($ptr,0xff,1),'Pointer',$ptr];
		my $poff = $off + $ptr;
		return unless $poff <= $end;
		my $ptag = unpack('C',substr(${$self->{buf}},$poff,1));
		my $parm;
		if (exists SCCP_PARMS_DISSECT->{$ptag}) {
			my @args = @{SCCP_PARMS_DISSECT->{$ptag}}; my $sub = shift @args;
			$parm = sprintf '%s(%d) - %s', $args[0], $ptag, $args[1];
			push @fields, [$poff,Frame::bytes($ptag,0xff,1),'Tag',$parm],
			$poff++;
			my ($plen,$pend) = (undef,$poff);
			if ($ptag) {
				$plen = unpack('C',substr(${$self->{buf}},$poff,1));
				push @fields, [$poff,Frame::bytes($plen,0xff,1),'Length',$plen];
				$poff++;
				$pend = $poff + $plen;
			}
			return unless $pend <= $self->{end};
			push @fields, @{$sub->($self,$poff,$pend,$plen,@args,@_)};
		} else {
			$parm = "Unknown($ptag) - Unknown parameter tag";
			push @fields, [$poff,Frame::bytes($ptag,0xff,1),'Tag',$parm];
			$poff++;
			my $plen = unpack('C',substr(${$self->{buf}},$poff,1));
			push @fields, [$poff,Frame::bytes($plen,0xff,1),'Length',$plen];
			$poff++;
			my $pend = $poff + $plen;
			return unless $pend <= $self->{end};
			push @fields, [$poff,' ******** ','Parameter',Frame::pdata(substr(${$self->{buf}},$poff,$plen))];
		}
		push @oparms, "Parameter $parm", \@fields;
	}
	return [ 'Optional Parameters', \@oparms ];
}

# -------------------------------------
package Message::ITUT::SCCP; our @ISA = qw(Message::SCCP);
package Message::ANSI::SCCP; our @ISA = qw(Message::SCCP);
# -------------------------------------
package Message::SCCP::CR; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(SLR) F(PCLS) V(CDPA) O(CRED CGPA DATA HOPC EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self, 'SLR', $off, 3); $off += 3;
	return unless Message::unpack_MF($self, 'PCLS',$off, 1); $off += 1;
	return unless Message::unpack_MV($self, 'CDPA',$off   ); $off += 1;
	return unless Message::unpack_OP($self,        $off   );
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self, 'SLR', $off+0, 3),
		Message::dissect_MF($self, 'PCLS',$off+3, 1),
		Message::dissect_MV($self, 'CDPA',$off+4   ),
		Mesasge::dissect_OP($self,        $off+5   ),
	);
}
package Message::ITUT::SCCP::CR; our @ISA = qw(Message::SCCP::CR Message::ITUT::SCCP);
package Message::ANSI::SCCP::CR; our @ISA = qw(Message::SCCP::CR Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::CC; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR) F(PCLS) O(CRED CDPA DATA EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self, 'DLR', $off, 3); $off += 3;
	return unless Message::unpack_MF($self, 'SLR', $off, 3); $off += 3;
	return unless Message::unpack_MF($self, 'PCLS',$off, 1); $off += 1;
	return unless Message::unpack_OP($self,        $off   );
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self, 'DLR', $off+0, 3),
		Message::dissect_MF($self, 'SLR', $off+3, 3),
		Message::dissect_MF($self, 'PCLS',$off+6, 1),
		message::dissect_OP($self,        $off+7   ),
	);
}
package Message::ITUT::SCCP::CC; our @ISA = qw(Message::SCCP::CC Message::ITUT::SCCP);
package Message::ANSI::SCCP::CC; our @ISA = qw(Message::SCCP::CC Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::CREF; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(REFC) O(CDPA DATA EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'REFC',$off,1); $off+=1;
	return unless Message::unpack_OP($self,       $off  );
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'REFC',$off+3,1),
		Message::dissect_OP($self,       $off    ),
	);
}
package Message::ITUT::SCCP::CREF; our @ISA = qw(Message::SCCP::CREF Message::ITUT::SCCP);
package Message::ANSI::SCCP::CREF; our @ISA = qw(Message::SCCP::CREF Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::RLSD;  our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR) F(RELC) O(DATA EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'SLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'RELC',$off,1); $off+=1;
	return unless Message::unpack_OP($self,       $off  );
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'SLR', $off+3,3),
		Message::dissect_MF($self,'RELC',$off+6,1),
		Message::dissect_OP($self,       $off    ),
	);
}
package Message::ITUT::SCCP::RLSD;  our @ISA = qw(Message::SCCP::RLSD Message::ITUT::SCCP);
package Message::ANSI::SCCP::RLSD;  our @ISA = qw(Message::SCCP::RLSD Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::RLC;   our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR',$off,3); $off+=3;
	return unless Message::unpack_MF($self,'SLR',$off,3); $off+=3;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR',$off+0,3),
		Message::dissect_MF($self,'SLR',$off+3,3),
	);
}
package Message::ITUT::SCCP::RLC; our @ISA = qw(Message::SCCP::RLC Message::ITUT::SCCP);
package Message::ANSI::SCCP::RLC; our @ISA = qw(Message::SCCP::RLC Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::DT1; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SEG) V(DATA)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'SEG', $off,1); $off+=1;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off+=1;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'SEG', $off+3,1),
		Message::dissect_MV($self,'DATA',$off+4  ),
	);
}
package Message::ITUT::SCCP::DT1; our @ISA = qw(Message::SCCP::DT1 Message::ITUT::SCCP);
package Message::ANSI::SCCP::DT1; our @ISA = qw(Message::SCCP::DT1 Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::DT2; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SEQ) V(DATA)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'SEQ', $off,2); $off+=2;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off+=1;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'SEQ', $off+3,1),
		Message::dissect_MV($self,'DATA',$off+4  ),
	);
}
package Message::ITUT::SCCP::DT2; our @ISA = qw(Message::SCCP::DT2 Message::ITUT::SCCP);
package Message::ANSI::SCCP::DT2; our @ISA = qw(Message::SCCP::DT2 Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::AK; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(RSN) F(CRED)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'RSN', $off,1); $off+=1;
	return unless Message::unpack_MF($self,'CRED',$off,1); $off+=1;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'RSN', $off+3,1),
		Message::dissect_MF($self,'CRED',$off+4,1),
	);
}
package Message::ITUT::SCCP::AK; our @ISA = qw(Message::SCCP::AK Message::ITUT::SCCP);
package Message::ANSI::SCCP::AK; our @ISA = qw(Message::SCCP::AK Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::UDT; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(PCLS) V(CDPA) V(CGPA) V(DATA)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'PCLS',$off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'PCLS',$off+0,1),
		Message::dissect_MV($self,'CDPA',$off+1  ),
		Message::dissect_MV($self,'CGPA',$off+2  ),
		Message::dissect_MV($self,'DATA',$off+3  ),
	);
}
package Message::ITUT::SCCP::UDT; our @ISA = qw(Message::SCCP::UDT Message::ITUT::SCCP);
package Message::ANSI::SCCP::UDT; our @ISA = qw(Message::SCCP::UDT Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::UDTS; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(RETC) V(CDPA) V(CGPA) V(DATA)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'RETC',$off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'RETC',$off+0,1),
		Message::dissect_MV($self,'CDPA',$off+1  ),
		Message::dissect_MV($self,'CGPA',$off+2  ),
		Message::dissect_MV($self,'DATA',$off+3  ),
	);
}
package Message::ITUT::SCCP::UDTS; our @ISA = qw(Message::SCCP::UDTS Message::ITUT::SCCP);
package Message::ANSI::SCCP::UDTS; our @ISA = qw(Message::SCCP::UDTS Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::ED; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) V(DATA)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MV($self,'DATA',$off+3  ),
	);
}
package Message::ITUT::SCCP::ED; our @ISA = qw(Message::SCCP::ED Message::ITUT::SCCP);
package Message::ANSI::SCCP::ED; our @ISA = qw(Message::SCCP::ED Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::EA; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR',$off,3); $off+=3;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR',$off,3),
	);
}
package Message::ITUT::SCCP::EA; our @ISA = qw(Message::SCCP::EA Message::ITUT::SCCP);
package Message::ANSI::SCCP::EA; our @ISA = qw(Message::SCCP::EA Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::RSR; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR) F(RESC)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'SLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'RESC',$off,1); $off+=1;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'SLR', $off+3,3),
		Message::dissect_MF($self,'RESC',$off+6,1),
	);
}
package Message::ITUT::SCCP::RSR; our @ISA = qw(Message::SCCP::RSR Message::ITUT::SCCP);
package Message::ANSI::SCCP::RSR; our @ISA = qw(Message::SCCP::RSR Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::RSC; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR',$off,3); $off+=3;
	return unless Message::unpack_MF($self,'SLR',$off,3); $off+=3;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR',$off+0,3),
		Mesasge::dissect_MF($self,'SLR',$off+3,3),
	);
}
package Message::ITUT::SCCP::RSC; our @ISA = qw(Message::SCCP::RSC Message::ITUT::SCCP);
package Message::ANSI::SCCP::RSC; our @ISA = qw(Message::SCCP::RSC Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::ERR; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(ERRC)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'ERRC',$off,1); $off++;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'ERRC',$off+3,1),
	);
}
package Message::ITUT::SCCP::ERR; our @ISA = qw(Message::SCCP::ERR Message::ITUT::SCCP);
package Message::ANSI::SCCP::ERR; our @ISA = qw(Message::SCCP::ERR Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::IT; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(DLR) F(SLR) F(PCLS) F(SEQ) F(CRED)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'DLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'SLR', $off,3); $off+=3;
	return unless Message::unpack_MF($self,'PCLS',$off,1); $off+=1;
	return unless Message::unpack_MF($self,'SEQ', $off,2); $off+=2;
	return unless Message::unpack_MF($self,'CRED',$off,1); $off+=1;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'DLR', $off+0,3),
		Message::dissect_MF($self,'SLR', $off+3,3),
		Message::dissect_MF($self,'PCLS',$off+6,1),
		Message::dissect_MF($self,'SEQ', $off+7,2),
		Message::dissect_MF($self,'CRED',$off+9,1),
	);
}
package Message::ITUT::SCCP::IT; our @ISA = qw(Message::SCCP::IT Message::ITUT::SCCP);
package Message::ANSI::SCCP::IT; our @ISA = qw(Message::SCCP::IT Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::XUDT; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(PCLS) F(HOPC) V(CDPA) V(CGPA) V(DATA) O(SEG ISNI INS EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'PCLS',$off,1); $off++;
	return unless Message::unpack_MF($self,'HOPC',$off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'PCLS',$off+0,1),
		Message::dissect_MF($self,'HOPC',$off+1,1),
		Message::dissect_MV($self,'CDPA',$off+2  ),
		Message::dissect_MV($self,'CGPA',$off+3  ),
		Message::dissect_MV($self,'DATA',$off+4  ),
		Message::dissect_oP($self,       $off+5  ),
	);
	my $off = $self->{sccp}{off} + 1;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
package Message::ITUT::SCCP::XUDT; our @ISA = qw(Message::SCCP::XUDT Message::ITUT::SCCP);
package Message::ANSI::SCCP::XUDT; our @ISA = qw(Message::SCCP::XUDT Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::XUDTS; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(RETC) F(HOPC) V(CDPA) V(CGPA) V(DATA) O(SGMT ISNI INS EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'RETC',$off,1); $off++;
	return unless Message::unpack_MF($self,'HOPC',$off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA',$off  ); $off++;
	return unless Message::unpack_MV($self,'DATA',$off  ); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'RETC',$off+0,1),
		Message::dissect_MF($self,'HOPC',$off+1,1),
		Message::dissect_MV($self,'CDPA',$off+2  ),
		Message::dissect_MV($self,'CGPA',$off+3  ),
		Message::dissect_MV($self,'DATA',$off+4  ),
		Message::dissect_OP($self,       $off+5  ),
	);
}
package Message::ITUT::SCCP::XUDTS; our @ISA = qw(Message::SCCP::XUDTS Message::ITUT::SCCP);
package Message::ANSI::SCCP::XUDTS; our @ISA = qw(Message::SCCP::XUDTS Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::LUDT; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(PCLS) F(HOPC) V(CDPA) V(CGPA) V(LDATA) O(SGMT ISNI INS EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'PCLS', $off,1); $off++;
	return unless Message::unpack_MF($self,'HOPC', $off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA', $off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA', $off  ); $off++;
	return unless Message::unpack_MV($self,'LDATA',$off  ); $off++;
	return unless Message::unpack_OP($self,        $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'PCLS', $off+0,1),
		Message::dissect_MF($self,'HOPC', $off+1,1),
		Message::dissect_MV($self,'CDPA', $off+2  ),
		Message::dissect_MV($self,'CGPA', $off+3  ),
		Message::dissect_MV($self,'LDATA',$off+4  ),
	);
}
package Message::ITUT::SCCP::LUDT; our @ISA = qw(Message::SCCP::LUDT Message::ITUT::SCCP);
package Message::ANSI::SCCP::LUDT; our @ISA = qw(Message::SCCP::LUDT Message::ANSI::SCCP);
# -------------------------------------
package Message::SCCP::LUDTS; our @ISA = qw(Message::SCCP);
# -------------------------------------
# F(MT) F(RETC) F(HOPC) V(CDPA) V(CGPA) V(LDATA) O(SGMT ISNI INS EOP)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'RETC', $off,1); $off++;
	return unless Message::unpack_MF($self,'HOPC', $off,1); $off++;
	return unless Message::unpack_MV($self,'CDPA', $off  ); $off++;
	return unless Message::unpack_MV($self,'CGPA', $off  ); $off++;
	return unless Message::unpack_MV($self,'LDATA',$off  ); $off++;
	return unless Message::unpack_OP($self,        $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'RETC', $off+0,1),
		Message::dissect_MF($self,'HOPC', $off+1,1),
		Message::dissect_MV($self,'CDPA', $off+2  ),
		Message::dissect_MV($self,'CGPA', $off+3  ),
		Message::dissect_MV($self,'LDATA',$off+4  ),
		Message::dissect_OP($self,        $off+5  ),
	);
}
package Message::ITUT::SCCP::LUDTS; our @ISA = qw(Message::SCCP::LUDTS Message::ITUT::SCCP);
package Message::ANSI::SCCP::LUDTS; our @ISA = qw(Message::SCCP::LUDTS Message::ANSI::SCCP);
# -------------------------------------


# -------------------------------------
package Message::ITUT::SCCP::SCMG;
our @ISA = qw(
	Message::ITUT::SCCP::UDT  Message::ITUT::SCCP:UDTS
	Message::ITUT::SCCP::XUDT Message::ITUT::SCCP:XUDTS
	Message::ITUT::SCCP::LUDT Message::ITUT::SCCP:LUDTS
);
# -------------------------------------
# F(MT) F(ASSN) F(APC) F(SMI) for SSC only F(SCL)
# -------------------------------------
sub new {
	my $type = shift;
	my $parent = shift;
	my $self = {};
	bless $self,$type;
	$self->{parent} = $parent;
	$self->{hdr} = $parent->{hdr};
	$self->{buf} = $parent->{buf};
	$self->{beg} = $parent->{off};
	$self->{end} = $parent->{end};
	return $self->unpack($self->{beg},$self->{end},@_);
}
use constant {
	SCMG_TYPES=>{
		0x01=>['SSA','Subsystem available'],
		0x02=>['SSP','Subsystem prohibited'],
		0x03=>['SST','Subsystem status test'],
		0x04=>['SOR','Subsystem out-of-service request'],
		0x05=>['SOG','Subsystem out-of-service grant'],
		0x06=>['SSC','SCCP/Subsystem congestion'],
	},
	SMI_TYPES=>[
		'affected subsystem multiplicity unknown',
		'affected subsystem is solitary',
		'affected subsystem is duplicated',
		'spare',
	]
};
sub unpack {
	my ($self,$off,$end) = (shift,shift,shift);
	(
		$self->{fi},
		$self->{assn},
		$self->{apc},
		$self->{smi},
		$self->{scl},
	) =
	unpack('CCvCC',substr(${$self->{buf}},$off,6));
	$self->{smi} &= 0x03;
	$self->{scl} &= 0x0f if $self->{fi} == 6;
	return unless exists SCMG_TYPES->{$self->{fi}};
	my $type = SCMG_TYPES->{$self->{fi}}[0];
	bless $self,ref($self)."$type";
	return $self;
}
use constant {
	SCCP_MT_CR	=>0x01,
	SCCP_MT_CC	=>0x02,
	SCCP_MT_CREF	=>0x03,
	SCCP_MT_RLSD	=>0x04,
	SCCP_MT_RLC	=>0x05,
	SCCP_MT_DT1	=>0x06,
	SCCP_MT_DT2	=>0x07,
	SCCP_MT_AK	=>0x08,
	SCCP_MT_UDT	=>0x09,
	SCCP_MT_UDTS	=>0x0a,
	SCCP_MT_ED	=>0x0b,
	SCCP_MT_EA	=>0x0c,
	SCCP_MT_RSR	=>0x0d,
	SCCP_MT_RSC	=>0x0e,
	SCCP_MT_ERR	=>0x0f,
	SCCP_MT_IT	=>0x10,
	SCCP_MT_XUDT	=>0x11,
	SCCP_MT_XUDTS	=>0x12,
	SCCP_MT_LUDT	=>0x13,
	SCCP_MT_LUDTS	=>0x14,
};
sub dissect {
	my $self = shift;
	my $parent = $self->{parent}->dissect(@_);
	my $off = $self->{scmg}{off};
	my ($mt,$desc) = (@{SCMG_TYPES->{$self->{fi}}});
	my $smi = SMI_TYPES->[$self->{smi}];
	my $apc = $self->{apc}; $apc = (($apc>>11)&0x07).'-'.(($apc>>3)&0xff).'-'.(($apc>>0)&0x07);
	my @fields = (
		[$off+0,Frame::bytes($self->{fi},0xff,1),'FI',"$mt - $desc"],
		[$off+1,Frame::bytes($self->{assn},0xff,1),'ASSN',$self->{assn}],
		[$off+2,Frame::bytes($self->{apc},0x3fff,2),'APC',$apc],
		[$off+4,Frame::bytes($self->{smi},0xff,1),'SMI',$smi],
	);
	push @fields, [$off+5,Frame::bytes($self->{scl},0xff,1),'SCL',$self->{scl}] if $mt eq 'SSC';
	return [ @{$parent}, "SCMG Message $mt $desc", \@fields ];
}

# -------------------------------------
package Message::ANSI::SCCP::SCMG;
our @ISA = qw(
	Message::ANSI::SCCP::UDT  Message::ANSI::SCCP:UDTS
	Message::ANSI::SCCP::XUDT Message::ANSI::SCCP:XUDTS
	Message::ANSI::SCCP::LUDT Message::ANSI::SCCP:LUDTS
);
# -------------------------------------
# F(MT) F(ASSN) F(APC) F(SMI) for SSC only F(SCL)
# -------------------------------------
use constant {
	SCMG_TYPES=>{
		0x01=>['SSA','Subsystem available'],
		0x02=>['SSP','Subsystem prohibited'],
		0x03=>['SST','Subsystem status test'],
		0x04=>['SOR','Subsystem out-of-service request'],
		0x05=>['SOG','Subsystem out-of-service grant'],
		0xfd=>['SBR','Subsystem backup routing'],
		0xfe=>['SNR','Subsystem normal routing'],
		0xff=>['SRC','Subsystem routing status test'],
	},
	SMI_TYPES=>[
		'affected subsystem multiplicity unknown',
		'affected subsystem is solitary',
		'affected subsystem is duplicated',
		'spare',
	]
};
sub unpack {
	my $self = shift;
	$self->{scmg}{off} = $self->{off};
	$self->{scmg}{end} = $self->{end};
	(
		$self->{fi},
		$self->{assn},
		$self->{apc},
	) =
	unpack('CCV',substr(${$self->{buf}},$self->{off},6));
	$self->{smi} = ($self->{apc} >> 24) & 0xff;
	$self->{apc} = ($self->{apc} >>  0) & 0xffffff;
	$self->{smi} &= 0x03;
	$self->{scl} &= 0x0f if $self->{fi} == 6;
	return unless exists SCMG_TYPES->{$self->{fi}};
	my $type = SCMG_TYPES->{$self->{fi}}[0];
	bless $self,ref($self)."$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $parent = $self->{parent}->dissect(@_);
	my $off = $self->{scmg}{off};
	my ($mt,$desc) = (@{SCMG_TYPES->{$self->{fi}}});
	my $smi = SMI_TYPES->[$self->{smi}];
	my $apc = $self->{apc}; $apc = (($apc>>16)&0xff).'-'.(($apc>>8)&0xff).'-'.(($apc>>0)&0xff);
	my @fields = (
		[$off+0,Frame::bytes($self->{fi},0xff,1),'FI',"$mt - $desc"],
		[$off+1,Frame::bytes($self->{assn},0xff,1),'ASSN',$self->{assn}],
		[$off+2,Frame::bytes($self->{apc},0xffffff,3),'APC',$apc],
		[$off+5,Frame::bytes($self->{smi},0xff,1),'SMI',$smi],
	);
	push @fields, [$off+6,Frame::bytes($self->{scl},0xff,1),'SCL',$self->{scl}] if $mt eq 'SSC';
	return [ @{$parent}, "SCMG Message $mt $desc", \@fields ];
}
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SSA; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SSA; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SSP; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SSP; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SST; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SST; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SOR; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SOR; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SOG; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SOG; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ITUT::SCCP::SCMG::SSC; our @ISA = qw(Message::ITUT::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SBR; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SNR; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------
package Message::ANSI::SCCP::SCMG::SRT; our @ISA = qw(Message::ANSI::SCCP::SCMG);
# -------------------------------------

# -------------------------------------
package Message::USER; our @ISA = qw(Message::MTP3);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($network,$datalink) = @_;
	$self->{mt} = unpack('C',substr(${$self->{buf}},$self->{off},1));
	return $self;
}
sub dissect {
	my $self = shift;
	return $self->Message::MTP3::dissect(@_) if exists $self->{mtp3};
	return $self->Message::M3UA::XFER::DATA::dissect(@_) if exists $self->{m3ua};
}

# -------------------------------------
package Message::ISUP; our @ISA = qw(Message::MTP3);
# -------------------------------------
use constant {
	ISUP_MT_IAM	=> 0x01,    # 0000 0001	Initial address
	ISUP_MT_SAM	=> 0x02,    # 0000 0010	Subsequent address message
	ISUP_MT_INR	=> 0x03,    # 0000 0011	Information request (national use)
	ISUP_MT_INF	=> 0x04,    # 0000 0100	Information response (national use)
	ISUP_MT_COT	=> 0x05,    # 0000 0101	Continuity
	ISUP_MT_ACM	=> 0x06,    # 0000 0110	Address complete
	ISUP_MT_CON	=> 0x07,    # 0000 0111	Connect
	ISUP_MT_FOT	=> 0x08,    # 0000 1000	Forward transfer
	ISUP_MT_ANM	=> 0x09,    # 0000 1001	Answer
	#ISUP_MT_FIXME	=> 0x0a,    # 0000 1010	Reserved (Red book)
	#ISUP_MT_FIXME	=> 0x0b,    # 0000 1011	Reserved (Red book)
	ISUP_MT_REL	=> 0x0c,    # 0000 1100	Release
	ISUP_MT_SUS	=> 0x0d,    # 0000 1101	Suspend
	ISUP_MT_RES	=> 0x0e,    # 0000 1110	Resume
	#ISUP_MT_FIXME	=> 0x0f,    # 0000 1111	Reserved (Red book)
	ISUP_MT_RLC	=> 0x10,    # 0001 0000	Release complete
	ISUP_MT_CCR	=> 0x11,    # 0001 0001	Continuity check request
	ISUP_MT_RSC	=> 0x12,    # 0001 0010	Reset circuit
	ISUP_MT_BLO	=> 0x13,    # 0001 0011	Blocking
	ISUP_MT_UBL	=> 0x14,    # 0001 0100	Unblocking
	ISUP_MT_BLA	=> 0x15,    # 0001 0101	Blocking acknowledgement
	ISUP_MT_UBA	=> 0x16,    # 0001 0110	Unblocking acknowledgement
	ISUP_MT_GRS	=> 0x17,    # 0001 0111	Circuit group reset
	ISUP_MT_CGB	=> 0x18,    # 0001 1000	Circuit group blocking
	ISUP_MT_CGU	=> 0x19,    # 0001 1001	Circuit group unblocking
	ISUP_MT_CGBA	=> 0x1a,    # 0001 1010	Circuit group blocking acknowledgement
	ISUP_MT_CGUA	=> 0x1b,    # 0001 1011	Circuit group ublocking acknowledgement
	ISUP_MT_CMR	=> 0x1c,    # 0001 1100	Call modification request
	ISUP_MT_CMC	=> 0x1d,    # 0001 1101	Call modification complete
	ISUP_MT_CMRJ	=> 0x1e,    # 0001 1110	Call modification reject
	ISUP_MT_FAR	=> 0x1f,    # 0001 1111	Facility request
	ISUP_MT_FAA	=> 0x20,    # 0010 0000	Facility accepted
	ISUP_MT_FRJ	=> 0x21,    # 0010 0001	Facility reject
	ISUP_MT_FAD	=> 0x22,    # 0010 0010	Facility deactivate
	ISUP_MT_FAI	=> 0x23,    # 0010 0011	Facility information
	ISUP_MT_LPA	=> 0x24,    # 0010 0100	Loop back acknowledgement (national)
	ISUP_MT_CSVQ	=> 0x25,    # 0010 0101	Reserved (Red book)
	ISUP_MT_CSVR	=> 0x26,    # 0010 0110	Reserved (Red book)
	ISUP_MT_DRS	=> 0x27,    # 0010 0111	Delayed release (Reserved (Blue Book))
	ISUP_MT_PAM	=> 0x28,    # 0010 1000	Pass along message (national)
	ISUP_MT_GRA	=> 0x29,    # 0010 1001	Circuit group reset acknowledgement
	ISUP_MT_CQM	=> 0x2a,    # 0010 1010	Circuit group query (national)
	ISUP_MT_CQR	=> 0x2b,    # 0010 1011	Circuit group query response (national)
	ISUP_MT_CPG	=> 0x2c,    # 0010 1100	Call progress
	ISUP_MT_USR	=> 0x2d,    # 0010 1101	User to user information
	ISUP_MT_UCIC	=> 0x2e,    # 0010 1110	Unequipped CIC (national)
	ISUP_MT_CFN	=> 0x2f,    # 0010 1111	Confusion
	ISUP_MT_OLM	=> 0x30,    # 0011 0000	Overload (national)
	ISUP_MT_CRG	=> 0x31,    # 0011 0001	Charge information (national)
	ISUP_MT_NRM	=> 0x32,    # 0011 0010	Network resource managment
	ISUP_MT_FAC	=> 0x33,    # 0011 0011	Facility
	ISUP_MT_UPT	=> 0x34,    # 0011 0100	User part test
	ISUP_MT_UPA	=> 0x35,    # 0011 0101	User part available
	ISUP_MT_IDR	=> 0x36,    # 0011 0110	Identification request
	ISUP_MT_IRS	=> 0x37,    # 0011 0111	Identification response
	ISUP_MT_SGM	=> 0x38,    # 0011 1000	Segmentation
	#ISUP_MT_FIXME	=> 0x39,    # 0011 1001	Reserved (B-ISUP)
	#ISUP_MT_FIXME	=> 0x3a,    # 0011 1010	Reserved (B-ISUP)
	#ISUP_MT_FIXME	=> 0x3b,    # 0011 1011	Reserved (B-ISUP)
	#ISUP_MT_FIXME	=> 0x3c,    # 0011 1100	Reserved (B-ISUP)
	#ISUP_MT_FIXME	=> 0x3d,    # 0011 1101	Reserved (B-ISUP)
	#ISUP_MT_FIXME	=> 0x3e,    # 0011 1110
	#ISUP_MT_FIXME	=> 0x3f,    # 0011 1111
	ISUP_MT_LPR	=> 0x40,    # 0100 0000	Loop preservation
	ISUP_MT_APT	=> 0x41,    # 0100 0001	Application transport
	ISUP_MT_PRI	=> 0x42,    # 0100 0010	Pre-releation information
	ISUP_MT_SDN	=> 0x43,    # 0100 0011	Subsequent directory number (national)
	#
	ISUP_MT_CRA	=> 0xe9,    # 1110 1001	Circuit reservation acknowledgment
	ISUP_MT_CRM	=> 0xea,    # 1110 1010	Circuit reservation
	ISUP_MT_CVR	=> 0xeb,    # 1110 1011	Circuit validation response
	ISUP_MT_CVT	=> 0xec,    # 1110 1100	Circuit validation test
	ISUP_MT_EXM	=> 0xed,    # 1110 1101	Exit message
	#
	ISUP_MT_NON	=> 0xf8,    # 1111 1000	National notification (Spain)
	#			    # 1111 1001
	#			    # 1111 1010
	#			    # 1111 1011
	ISUP_MT_LLM	=> 0xfc,    # 1111 1100	National malicioius call (Spain)
	ISUP_MT_CAK	=> 0xfd,    # 1111 1101	Charge acknowledgement (Singapore)
	ISUP_MT_TCM	=> 0xfe,    # 1111 1110	Tariff charge (Signapore)
	ISUP_MT_MCP	=> 0xff,    # 1111 1111	Malicious call print (Singapore)
};
use constant {
	ISUP_TYPES=>{
		&ISUP_MT_IAM	=> 'IAM',   # 0000 0001	Initial address
		&ISUP_MT_SAM	=> 'SAM',   # 0000 0010	Subsequent address message
		&ISUP_MT_INR	=> 'INR',   # 0000 0011	Information request (national use)
		&ISUP_MT_INF	=> 'INF',   # 0000 0100	Information response (national use)
		&ISUP_MT_COT	=> 'COT',   # 0000 0101	Continuity
		&ISUP_MT_ACM	=> 'ACM',   # 0000 0110	Address complete
		&ISUP_MT_CON	=> 'CON',   # 0000 0111	Connect
		&ISUP_MT_FOT	=> 'FOT',   # 0000 1000	Forward transfer
		&ISUP_MT_ANM	=> 'ANM',   # 0000 1001	Answer
		#0x0a		=> '???',   # 0000 1010	Reserved (Red book)
		#0x0b		=> '???',   # 0000 1011	Reserved (Red book)
		&ISUP_MT_REL	=> 'REL',   # 0000 1100	Release
		&ISUP_MT_SUS	=> 'SUS',   # 0000 1101	Suspend
		&ISUP_MT_RES	=> 'RES',   # 0000 1110	Resume
		#0x0f		=> '???',   # 0000 1111	Reserved (Red book)
		&ISUP_MT_RLC	=> 'RLC',   # 0001 0000	Release complete
		&ISUP_MT_CCR	=> 'CCR',   # 0001 0001	Continuity check request
		&ISUP_MT_RSC	=> 'RSC',   # 0001 0010	Reset circuit
		&ISUP_MT_BLO	=> 'BLO',   # 0001 0011	Blocking
		&ISUP_MT_UBL	=> 'UBL',   # 0001 0100	Unblocking
		&ISUP_MT_BLA	=> 'BLA',   # 0001 0101	Blocking acknowledgement
		&ISUP_MT_UBA	=> 'UBA',   # 0001 0110	Unblocking acknowledgement
		&ISUP_MT_GRS	=> 'GRS',   # 0001 0111	Circuit group reset
		&ISUP_MT_CGB	=> 'CGB',   # 0001 1000	Circuit group blocking
		&ISUP_MT_CGU	=> 'CGU',   # 0001 1001	Circuit group unblocking
		&ISUP_MT_CGBA	=> 'CGBA',  # 0001 1010	Circuit group blocking acknowledgement
		&ISUP_MT_CGUA	=> 'CGUA',  # 0001 1011	Circuit group ublocking acknowledgement
		&ISUP_MT_CMR	=> 'CMR',   # 0001 1100	Call modification request (Reserved (Blue book))
		&ISUP_MT_CMC	=> 'CMC',   # 0001 1101	Call modification complete (Reserved (Blue book))
		&ISUP_MT_CMRJ	=> 'CMRJ',  # 0001 1110	Call modification reject (Reserved (Blue book))
		&ISUP_MT_FAR	=> 'FAR'    # 0001 1111	Facility request,
		&ISUP_MT_FAA	=> 'FAA',   # 0010 0000	Facility accepted
		&ISUP_MT_FRJ	=> 'FRJ',   # 0010 0001	Facility reject
		&ISUP_MT_FAD	=> 'FAD',   # 0010 0010	Facility deactivate (Reserved (Red book))
		&ISUP_MT_FAI	=> 'FAI',   # 0010 0011	Facility information (Reserved (Red book))
		&ISUP_MT_LPA	=> 'LPA',   # 0010 0100	Loop back acknowledgement (national)
		&ISUP_MT_CSVQ	=> 'CSVQ',  # 0010 0101	Reserved (Red book)
		&ISUP_MT_CSVR	=> 'CSVR',  # 0010 0110	Reserved (Red book)
		&ISUP_MT_DRS	=> 'DRS',   # 0010 0111	Delayed release (Reserved (Blue book))
		&ISUP_MT_PAM	=> 'PAM',   # 0010 1000	Pass along message (national)
		&ISUP_MT_GRA	=> 'GRA',   # 0010 1001	Circuit group reset acknowledgement
		&ISUP_MT_CQM	=> 'CQM',   # 0010 1010	Circuit group query (national)
		&ISUP_MT_CQR	=> 'CQR',   # 0010 1011	Circuit group query response (national)
		&ISUP_MT_CPG	=> 'CPG',   # 0010 1100	Call progress
		&ISUP_MT_USR	=> 'USR',   # 0010 1101	User to user information
		&ISUP_MT_UCIC	=> 'UCIC',  # 0010 1110	Unequipped CIC (national)
		&ISUP_MT_CFN	=> 'CFN',   # 0010 1111	Confusion
		&ISUP_MT_OLM	=> 'OLM',   # 0011 0000	Overload (national)
		&ISUP_MT_CRG	=> 'CRG',   # 0011 0001	Charge information (national)
		&ISUP_MT_NRM	=> 'NRM',   # 0011 0010	Network resource managment
		&ISUP_MT_FAC	=> 'FAC',   # 0011 0011	Facility
		&ISUP_MT_UPT	=> 'UPT',   # 0011 0100	User part test
		&ISUP_MT_UPA	=> 'UPA',   # 0011 0101	User part available
		&ISUP_MT_IDR	=> 'IDR',   # 0011 0110	Identification request
		&ISUP_MT_IRS	=> 'IRS',   # 0011 0111	Identification response
		&ISUP_MT_SGM	=> 'SGM',   # 0011 1000	Segmentation
		#0x39		=> '???',   # 0011 1001	Reserved (B-ISUP)
		#0x3a		=> '???',   # 0011 1010	Reserved (B-ISUP)
		#0x3b		=> '???',   # 0011 1011	Reserved (B-ISUP)
		#0x3c		=> '???',   # 0011 1100	Reserved (B-ISUP)
		#0x3d		=> '???',   # 0011 1101	Reserved (B-ISUP)
		#0x3e		=> '???',   # 0011 1110
		#0x3f		=> '???',   # 0011 1111
		&ISUP_MT_LPR	=> 'LPR',   # 0100 0000	Loop preservation
		&ISUP_MT_ATP	=> 'ATP',   # 0100 0001	Application transport
		&ISUP_MT_PRI	=> 'PRI',   # 0100 0010	Pre-releation information
		&ISUP_MT_SDN	=> 'SDN',   # 0100 0011	Subsequent directory number (national)
		#
		&ISUP_MT_CRA	=> 'CRA',   # 1110 1001	Circuit reservation acknowledgment
		&ISUP_MT_CRM	=> 'CRM',   # 1110 1010	Circuit reservation
		&ISUP_MT_CVR	=> 'CVR',   # 1110 1011	Circuit validation response
		&ISUP_MT_CVT	=> 'CVT',   # 1110 1100	Circuit validation test
		&ISUP_MT_EXM	=> 'EXM',   # 1110 1101	Exit message
		#
		&ISUP_MT_NON	=> 'NON',   # 1111 1000	National notification (Spain)
		#			    # 1111 1001
		#			    # 1111 1010
		#			    # 1111 1011
		&ISUP_MT_LLM	=> 'LLM',   # 1111 1100	National malicioius call (Spain),
		&ISUP_MT_CAK	=> 'CAK',   # 1111 1101	Charge acknowledgement (Singapore)
		&ISUP_MT_TCM	=> 'TCM',   # 1111 1110	Tariff charge (Signapore)
		&ISUP_MT_MCP	=> 'MCP',   # 1111 1111	Malicious call print (Singapore)
	},
};
sub unpack {
	my $self = shift;
	$self->{isup}{off} = $self->{off};
	$self->{isup}{end} = $self->{end};
	(
		$self->{cic},
		$self->{mt},
	)
	= unpack('vC',substr(${$self->{buf}},$self->{off},3));
	my $type = ISUP_TYPES->{$self->{mt}};
	return unless defined $type;
	bless $self,ref($self)."::$type";
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off};
	my $layers;
	if (exists $self->{mtp3}) {
		$layers = $self->SUPER::dissect(@_);
	} else { # if (exists $self->{m3ua}) {
		$layers = $self->Message::M3UA::XFER::DATA::dissect(@_);
	}
	my $mt = $self->{mt}; $mt = ISUP_TYPES->{$mt}."($mt)";
	return [
		@{$layers},
		"$mt Message",
		[$off+0,Frame::bytes($self->{cic},0x00ffff,3),'CIC',$self->{cic}],
		[$off+0,Frame::bytes($self->{mt},0xff0000,3),'MT',$mt],
	];
}

use constant {
	ISUP_PT_EOP	=>  0,	# 0000 0000	End of optional parameters
	ISUP_PT_CLRF	=>  1,	# 0000 0001	Call reference @
	ISUP_PT_TMR	=>  2,	# 0000 0010	Transmission medium requirement (!A)
	ISUP_PT_ATP	=>  3,	# 0000 0011	Access transport
	ISUP_PT_CDPN	=>  4,	# 0000 0100	Called party number
	ISUP_PT_SUBN	=>  5,	# 0000 0101	Subsequent number (!A)
	ISUP_PT_NCI	=>  6,	# 0000 0110	Nature of connection indicators
	ISUP_PT_FCI	=>  7,	# 0000 0111	Forward call indicators
	ISUP_PT_OFCI	=>  8,	# 0000 1000	Optional forward call indicators
	ISUP_PT_CPC	=>  9,	# 0000 1001	Calling party's category
	ISUP_PT_CGPN	=> 10,	# 0000 1010	Calling party number
	ISUP_PT_RDGN	=> 11,	# 0000 1011	Redirecting number
	ISUP_PT_RDNN	=> 12,	# 0000 1100	Redirection number
	ISUP_PT_CONR	=> 13,	# 0000 1101	Connection request
	ISUP_PT_INRI	=> 14,	# 0000 1110	Information request indicators (N)
	ISUP_PT_INFI	=> 15,	# 0000 1111	Information indicators (N) @
	ISUP_PT_COTI	=> 16,	# 0001 0000	Continuity indicators
	ISUP_PT_BCI	=> 17,	# 0001 0001	Backward call indicators
	ISUP_PT_CAUS	=> 18,	# 0001 0010	Cause indicators
	ISUP_PT_RDI	=> 19,	# 0001 0011	Redirection information
	#			# 0001 0100	Reserved (Q.763 1984, Red Book)
	ISUP_PT_CGI	=> 21,	# 0001 0101	Circuit group supervision message type
	ISUP_PT_RS	=> 22,	# 0001 0110	Range and status
	ISUP_PT_CMI	=> 23,	# 0001 0111	Call modification indicators (Blue Book) (!A)
	ISUP_PT_FACI	=> 24,	# 0001 1000	Facility indicator (!A)
	ISUP_PT_FAII	=> 25,	# 0001 1001	Facility information indicators (Bellcore) (!A)
	ISUP_PT_CUGI	=> 26,	# 0001 1010	Closed user group interlock code (!A)
	ISUP_PT_INDEX	=> 27,	# 0001 1011	Index (Bellcore) (!A)
	ISUP_PT_CUGC	=> 28,	# 0001 1100	Closed user group check response indicators (!A)
	ISUP_PT_USI	=> 29,	# 0001 1101	User service information
	ISUP_PT_SPC	=> 30,	# 0001 1110	Signalling point code (N) (!A) (Bellcore) @
	#			# 0001 1111	Reserved (Q.763 1984, Red Book)
	ISUP_PT_UUI	=> 32,	# 0010 0000	User-to-user information
	ISUP_PT_CONN	=> 33,	# 0010 0001	Connected number (!A)
	ISUP_PT_SRIS	=> 34,	# 0010 0010	Suspend/resume indicators
	ISUP_PT_TNS	=> 35,	# 0010 0011	Transit network selection (N) @
	ISUP_PT_EVNT	=> 36,	# 0010 0100	Event information indicators
	ISUP_PT_CAM	=> 37,	# 0010 0101	Circuit assignment map
	ISUP_PT_CSI	=> 38,	# 0010 0110	Circuit state indicator @
	ISUP_PT_ACL	=> 39,	# 0010 0111	Automatic congestion level
	ISUP_PT_OCDN	=> 40,	# 0010 1000	Original called number
	ISUP_PT_OBCI	=> 41,	# 0010 1001	Optional backward call indicators
	ISUP_PT_UUIND	=> 42,	# 0010 1010	User-to-user indicators
	ISUP_PT_ISPC	=> 43,	# 0010 1011	Origination ISC point code (X)
	ISUP_PT_GNOT	=> 44,	# 0010 1100	Generic notification (X)
	ISUP_PT_CHI	=> 45,	# 0010 1101	Call history information (X)
	ISUP_PT_ADI	=> 46,	# 0010 1110	Access delivery information (X)
	ISUP_PT_NSF	=> 47,	# 0010 1111	Network specific facility (N) @
	ISUP_PT_USIP	=> 48,	# 0011 0000	User service information prime
	ISUP_PT_PROP	=> 49,	# 0011 0001	Propagation delay counter (X)
	ISUP_PT_ROPS	=> 50,	# 0011 0010	Remote operations (N) @
	ISUP_PT_SA	=> 51,	# 0011 0011	Service activation @
	ISUP_PT_UTI	=> 52,	# 0011 0100	User teleservice information (X)
	ISUP_PT_TMU	=> 53,	# 0011 0101	Transmission medium used
	ISUP_PT_CDI	=> 54,	# 0011 0110	Call diversion information (X)
	ISUP_PT_ECI	=> 55,	# 0011 0111	Echo control information (X)
	ISUP_PT_MCI	=> 56,	# 0011 1000	Message compatibility information
	ISUP_PT_PCI	=> 57,	# 0011 1001	Parameter compatibility information
	ISUP_PT_MLPP	=> 58,	# 0011 1010	MLPP precedence (Precedence ANSI)
	ISUP_PT_MCIQ	=> 59,	# 0011 1011	MCID request indicators (X)
	ISUP_PT_MCIR	=> 60,	# 0011 1100	MCID response indicators (X)
	ISUP_PT_HOPC	=> 61,	# 0011 1101	Hop counter (reserved)
	ISUP_PT_TMRP	=> 62,	# 0011 1110	Transmission medium requirement prime (X)
	ISUP_PT_LN	=> 63,	# 0011 1111	Location number (X)
	ISUP_PT_RDNR	=> 64,	# 0100 0000	Redirection number restriction (X)
	ISUP_PT_FREEP	=> 65,	# 0100 0001	Freephone indicators/Reserved (Q.763 1992, White Book) (X)
	ISUP_PT_GREF	=> 66,	# 0100 0010	Generic reference/Reserved (Q.763 1992, White Book) (X)
	ISUP_PT_CTR	=> 67,	# 0100 0011	Call transfer reference (Q.763) (X)
	ISUP_PT_LPI	=> 68,	# 0100 0100	Loop prevention indicators (Q.763) (X)
	ISUP_PT_CTN	=> 69,	# 0100 0101	Call transfer number (Q.763) (X)
	#			# 0100 0110	Reserved (B-ISUP)
	#			# 0100 0111	Reserved (B-ISUP)
	#			# 0100 1000	Reserved (B-ISUP)
	#			# 0100 1001	Reserved (B-ISUP)
	#			# 0100 1010	Reserved (B-ISUP)
	ISUP_PT_CCSS	=> 75,	# 0100 1011	CCSS (Q.763) (X)
	ISUP_PT_FGVSN	=> 76,	# 0100 1100	Forward GVNS (Q.763) (X)
	ISUP_PT_BGVNS	=> 77,	# 0100 1101	Backward GVNS (Q.763) (X)
	ISUP_PT_RDCAP	=> 78,	# 0100 1110	Redirect capability (N) (ANSI/GR-246-CORE)
	#			# 0100 1111	Reserved (B-ISUP)
	#			# 0101 0000	Reserved (B-ISUP)
	#			# 0101 0001	Reserved (B-ISUP)
	#			# 0101 0010	Reserved (B-ISUP)
	#			# 0101 0011	Reserved (B-ISUP)
	#			# 0101 0100	Reserved (B-ISUP)
	#			# 0101 0101	Reserved (B-ISUP)
	#			# 0101 0110	Reserved (B-ISUP)
	#			# 0101 0111	Reserved (B-ISUP)
	#			# 0101 1000	Reserved (B-ISUP)
	#			# 0101 1001	Reserved (B-ISUP)
	#			# 0101 1010	Reserved (B-ISUP)
	ISUP_PT_NMC	=> 91,	# 0101 1011	Network management controls (ANSI/GR-246-CORE)
	#			# 0101 1100	Reserved (B-ISUP)
	#			# 0101 1101	Reserved (B-ISUP)
	#			# 0101 1110	Reserved (B-ISUP)
	#			# 0101 1111	Reserved (B-ISUP)
	#			# 0110 0000	Reserved (B-ISUP)
	#			# 0110 0001	Reserved (B-ISUP)
	#			# 0110 0010	Reserved (B-ISUP)
	#			# 0110 0011	Reserved (B-ISUP)
	#			# 0110 0100	Reserved (B-ISUP)
	#			# 0110 0100	Reserved (B-ISUP)
	ISUP_PT_CID	=>101,	# 0110 0101	Correlation id (Q.763) (X)
	ISUP_PT_SCFID	=>102,	# 0110 0110	SCF id (Q.763) (X)
	#			# 0110 0111	Reserved (B-ISUP)
	#			# 0110 1000	Reserved (B-ISUP)
	#			# 0110 1001	Reserved (B-ISUP)
	#			# 0110 1010	Reserved (B-ISUP)
	#			# 0110 1011	Reserved (B-ISUP)
	#			# 0110 1100	Reserved (B-ISUP)
	#			# 0110 1101	Reserved (B-ISUP)
	ISUP_PT_CDTI	=>110,	# 0110 1110	Call diversion treatment indicators (Q.763) (X)
	ISUP_PT_CINN	=>111,	# 0110 1111	Called IN number (Q.763) (X)
	ISUP_PT_CTRI	=>112,	# 0111 0000	Call offering treatment indicators (Q.763) (X)
	ISUP_PT_CHPI	=>113,	# 0111 0001	Charged party identification (Q.763) (X)
	ISUP_PT_CTI	=>114,	# 0111 0010	Conference treatment indicators (Q.763) (X)
	ISUP_PT_DISP	=>115,	# 0111 0011	Display information (Q.763) (X)
	ISUP_PT_UAI	=>116,	# 0111 0100	UID action indicators (Q.763) (X)
	ISUP_PT_UCI	=>117,	# 0111 0101	UID capability indicators (Q.763) (X)
	#			# 0111 0110	Reserved (B-ISUP)
	ISUP_PT_RC	=>119,	# 0111 0111	Redirect counter (N) (ANSI/GR-246-CORE)/Reserved (B-ISUP)
	ISUP_PT_AT	=>120,	# 0111 1000	Application transport (Q.763)/Reserved (B-ISUP) (X)
	ISUP_PT_CCR	=>121,	# 0111 1001	Collect call request (Q.763)/Reserved (B-ISUP) (X)
	ISUP_PT_CPI	=>122,	# 0111 1010	CCNR possible indicator (Q.763)/Reserved (B-ISUP) (X)
	ISUP_PT_PVTCA	=>123,	# 0111 1011	Pivot capability (ANSI/Q.763)/Reserved (B-ISUP)
	ISUP_PT_PVTRI	=>124,	# 0111 1100	Pivot routing indicators (ANSI/Q.763)/Reserved (B-ISUP)
	ISUP_PT_CDDN	=>125,	# 0111 1101	Called directory number (Q.763)/Reserved (B-ISUP) (X)
	#			# 0111 1110	Reserved (B-ISUP)
	ISUP_PT_OCDIN	=>127,	# 0111 1111	Original called IN number (Q.763) (X)
	#			# 1000 0000
	ISUP_PT_GL	=>129,	# 1000 0001	Calling geodetic location (ANSI/GR-246-CORE)
	ISUP_PT_HTRI	=>130,	# 1000 0010	HTR information (Q.763) (X)
	#			# 1000 0011
	ISUP_PT_NRN	=>132,	# 1000 0100	Network routing number (N) (Q.763) (X)
	ISUP_PT_QORC	=>133,	# 1000 0101	Query on release capability (O) (Q.763) (X)
	ISUP_PT_PVTS	=>134,	# 1000 0110	Pivot status (N) (ANSI/Q.763) (!A)
	ISUP_PT_PVTC	=>135,	# 1000 0111	Pivot counter (ANSI/Q.763) (!A)
	ISUP_PT_PVTFI	=>136,	# 1000 1000	Pivot routing forward information (ANSI/Q.763) (!A)
	ISUP_PT_PVTBI	=>137,	# 1000 1001	Pivot routing backward information (ANSI/Q.763) (!A)
	ISUP_PT_RDS	=>138,	# 1000 1010	Redirect status (N)
	ISUP_PT_RDFI	=>139,	# 1000 1011	Redirect forward information (N)
	ISUP_PT_RDBI	=>140,	# 1000 1100	Redirect backward information (N)
	ISUP_PT_NPFI	=>141,	# 1000 1101	Number portability forward information (O) (X)
	#			# 1000 1110
	#			# 1000 1111	Reserved (B-ISUP)
	#			# 1001 0000	Reserved (B-ISUP)
	#			# 1001 0001	Reserved (B-ISUP)
	#			# 1001 0010	Reserved (B-ISUP)
	#			# 1001 0011	Reserved (B-ISUP)
	#			# 1001 0100	Reserved (B-ISUP)
	#			# 1001 0101	Reserved (B-ISUP)
	#			# 1001 0110	Reserved (B-ISUP)
	ISUP_PT_ARR	=>140,	# 1001 0110	Automatic re-routing
	#			# 1001 0111	Reserved (B-ISUP)
	#			# 1001 1000	Reserved (B-ISUP)
	#
	ISUP_PT_IEPS	=>166,	# 1010 0110	Internation Emergency Prefrerence Scheme call info (Q.763/Amd.4/01/2006)
	ISUP_PT_VEDI	=>168,	# 1010 1000	Voice Enhancement Device Information (Q.763/Amd.5/09/2006)
	#
	ISUP_PT_GNUM	=>192,	# 1100 0000	Generic number (Q.763)/Generic address (ANSI/GR-246-CORE)
	ISUP_PT_GDIG	=>193,	# 1100 0001	Generic digits (N)
	ISUP_PT_OSI	=>194,	# 1100 0010	Operator services information (ANSI/GR-246-CORE)
	ISUP_PT_EGRESS	=>195,	# 1100 0011	Egress (ANSI/GR-246-CORE)
	ISUP_PT_JUR	=>196,	# 1100 0100	Jurisdiction (ANSI/GR-246-CORE)
	ISUP_PT_CIDC	=>197,	# 1100 0101	Carrier identification (ANSI/GR-246-CORE)
	ISUP_PT_BGROUP	=>198,	# 1100 0110	Business group (ANSI/GR-246-CORE)
	ISUP_PT_GNAM	=>199,	# 1100 0111	Generic name (ANSI/GR-246-CORE)
	#
	ISUP_PT_NOTI	=>225,	# 1110 0001	Notification indicator (ANSI/GR-246-CORE)
	ISUP_PT_CSPI	=>226,	# 1110 0010	Carrier service provider identification (ANSI)
	ISUP_PT_TRNSRQ	=>227,	# 1110 0011	Transaction request (ANSI/GR-246-CORE)
	ISUP_PT_LSPI	=>228,	# 1110 0100	Local service provider identification (ANSI)
	ISUP_PT_CGCI	=>229,	# 1110 0101	Circuit group characteristic indicator (ANSI/GR-246-CORE)
	ISUP_PT_CVRI	=>230,	# 1110 0110	Circuit validation response indicator (ANSI/GR-246-CORE)
	ISUP_PT_OTGN	=>231,	# 1110 0111	Outgoing trunk group number (ANSI/GR-246-CORE)
	ISUP_PT_CIN	=>232,	# 1110 1000	Circuit identification name (ANSI/GR-246-CORE)
	ISUP_PT_CLLI	=>233,	# 1110 1001	Common language location identification (ANSI/GR-246-CORE)
	ISUP_PT_OLI	=>234,	# 1110 1010	Originating line information (ANSI/GR-246-CORE)
	ISUP_PT_CHGN	=>235,	# 1110 1011	Charge number (ANSI/GR-246-CORE)
	ISUP_PT_SVCD	=>236,	# 1110 1100	Service code indicator (ANSI/GR-246-CORE)
	ISUP_PT_SPR	=>237,	# 1110 1101	Special processing request (ANSI/GR-246-CORE)
	ISUP_PT_CSEL	=>238,	# 1110 1110	Carrier selection information (ANSI/GR-246-CORE)
	ISUP_PT_NT	=>239,	# 1110 1111	Network transport (ANSI/GR-246-CORE)
				# 1111 0000
				# 1111 0001
				# 1111 0010
	ISUP_PT_ORI	=>243,	# 1111 0011	Outgoing route identification (Spain)
	ISUP_PT_IRI	=>244,	# 1111 0100	Incoming route identification (Spain)
				# 1111 0101
				# 1111 0110
				# 1111 0111
	ISUP_PT_RATE	=>248,	# 1111 1000	Rate (Spain)
	ISUP_PT_IIC	=>249,	# 1111 1001	Identifier of incoming circuit (Spain)
				# 1111 1010
				# 1111 1011
				# 1111 1100
	ISUP_PT_TOI	=>253,	# 1111 1101	Trunk offering information (Singapore)
	ISUP_PT_TON	=>253,	# 1111 1101	Type of notification (Spain)
	ISUP_PT_CRI	=>254,	# 1111 1110	Charge rate information (Singapore)
	ISUP_PT_ICCI	=>255,	# 1111 1111	Call charge information (Signapore)
};

sub unpack_PT_EOP {
	my ($self,$off,$end,$len) = @_;
	return;
}
sub unpack_PT_CLRF {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TMR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ATP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CDPN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SUBN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_FCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OFCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CPC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CGPN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDGN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDNN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CONR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_INRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_INFI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_COTI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_BCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CAUS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CGI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CMI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_FACI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_FAII {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CUGI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_INDEX {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CUGC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_USI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SPC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_UUI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CONN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SRIS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TNS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_EVNT {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CAM {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CSI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ACL {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OCDN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OBCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_UUIND {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ISPC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GNOT {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CHI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ADI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NSF {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_USIP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PROP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ROPS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SA {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_UTI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TMU {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CDI {
	my ($self,$off,$end,$len) = @_; }
sub unpack_PT_ECI {
	my ($self,$off,$end,$len) = @_; }
sub unpack_PT_MCI {
	my ($self,$off,$end,$len) = @_; }
sub unpack_PT_PCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_MLPP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_MCIQ {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_MCIR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_HOPC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TMRP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_LN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDNR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_FREEP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GREF {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CTR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_LPI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CTN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CCSS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_FGVSN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_BGVNS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDCAP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NMC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CID {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SCFID {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CDTI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CINN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CTRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CHPI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CTI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_DISP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_UAI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_UCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_AT {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CCR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CPI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTCA {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CDDN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OCDIN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GL {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_HTRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NRN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_QORC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTFI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_PVTBI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDFI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RDBI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NPFI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GNUM {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GDIG {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OSI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_EGRESS {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_JUR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CIDC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_BGROUP {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_GNAM {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NOTI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CSPI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TRNSRQ {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_LSPI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CGCI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CVRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OTGN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CIN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CLLI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_OLI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CHGN {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SVCD {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_SPR {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CSEL {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_NT {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ORI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_IRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_RATE {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_IIC {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TOI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_TON {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_CRI {
	my ($self,$off,$end,$len) = @_;
}
sub unpack_PT_ICCI {
	my ($self,$off,$end,$len) = @_;
}

use constant {
	ISUP_PARM_TYPE=>{
		&ISUP_PT_EOP	=> 'EOP',
		&ISUP_PT_CLRF	=> 'CLRF',
		&ISUP_PT_TMR	=> 'TMR',
		&ISUP_PT_ATP	=> 'ATP',
		&ISUP_PT_CDPN	=> 'CDPN',
		&ISUP_PT_SUBN	=> 'SUBN',
		&ISUP_PT_NCI	=> 'NCI',
		&ISUP_PT_FCI	=> 'FCI',
		&ISUP_PT_OFCI	=> 'OFCI',
		&ISUP_PT_CPC	=> 'CPC',
		&ISUP_PT_CGPN	=> 'CGPN',
		&ISUP_PT_RDGN	=> 'RDGN',
		&ISUP_PT_RDNN	=> 'RDNN',
		&ISUP_PT_CONR	=> 'CONR',
		&ISUP_PT_INRI	=> 'INRI',
		&ISUP_PT_INFI	=> 'INFI',
		&ISUP_PT_COTI	=> 'COTI',
		&ISUP_PT_BCI	=> 'BCI',
		&ISUP_PT_CAUS	=> 'CAUS',
		&ISUP_PT_RDI	=> 'RDI',
		&ISUP_PT_CGI	=> 'CGI',
		&ISUP_PT_RS	=> 'RS',
		&ISUP_PT_CMI	=> 'CMI',
		&ISUP_PT_FACI	=> 'FACI',
		&ISUP_PT_FAII	=> 'FAII',
		&ISUP_PT_CUGI	=> 'CUGI',
		&ISUP_PT_INDEX	=> 'INDEX',
		&ISUP_PT_CUGC	=> 'CUGC',
		&ISUP_PT_USI	=> 'USI',
		&ISUP_PT_SPC	=> 'SPC',
		&ISUP_PT_UUI	=> 'UUI',
		&ISUP_PT_CONN	=> 'CONN',
		&ISUP_PT_SRIS	=> 'SRIS',
		&ISUP_PT_TNS	=> 'TNS',
		&ISUP_PT_EVNT	=> 'EVNT',
		&ISUP_PT_CAM	=> 'CAM',
		&ISUP_PT_CSI	=> 'CSI',
		&ISUP_PT_ACL	=> 'ACL',
		&ISUP_PT_OCDN	=> 'OCDN',
		&ISUP_PT_OBCI	=> 'OBCI',
		&ISUP_PT_UUIND	=> 'UUIND',
		&ISUP_PT_ISPC	=> 'ISPC',
		&ISUP_PT_GNOT	=> 'GNOT',
		&ISUP_PT_CHI	=> 'CHI',
		&ISUP_PT_ADI	=> 'ADI',
		&ISUP_PT_NSF	=> 'NSF',
		&ISUP_PT_USIP	=> 'USIP',
		&ISUP_PT_PROP	=> 'PROP',
		&ISUP_PT_ROPS	=> 'ROPS',
		&ISUP_PT_SA	=> 'SA',
		&ISUP_PT_UTI	=> 'UTI',
		&ISUP_PT_TMU	=> 'TMU',
		&ISUP_PT_CDI	=> 'CDI',
		&ISUP_PT_ECI	=> 'ECI',
		&ISUP_PT_MCI	=> 'MCI',
		&ISUP_PT_PCI	=> 'PCI',
		&ISUP_PT_MLPP	=> 'MLPP',
		&ISUP_PT_MCIQ	=> 'MCIQ',
		&ISUP_PT_MCIR	=> 'MCIR',
		&ISUP_PT_HOPC	=> 'HOPC',
		&ISUP_PT_TMRP	=> 'TMRP',
		&ISUP_PT_LN	=> 'LN',
		&ISUP_PT_RDNR	=> 'RDNR',
		&ISUP_PT_FREEP	=> 'FREEP',
		&ISUP_PT_GREF	=> 'GREF',
		&ISUP_PT_CTR	=> 'CTR',
		&ISUP_PT_LPI	=> 'LPI',
		&ISUP_PT_CTN	=> 'CTN',
		&ISUP_PT_CCSS	=> 'CCSS',
		&ISUP_PT_FGVSN	=> 'FGVSN',
		&ISUP_PT_BGVNS	=> 'BGVNS',
		&ISUP_PT_RDCAP	=> 'RDCAP',
		&ISUP_PT_NMC	=> 'NMC',
		&ISUP_PT_CID	=> 'CID',
		&ISUP_PT_SCFID	=> 'SCFID',
		&ISUP_PT_CDTI	=> 'CDTI',
		&ISUP_PT_CINN	=> 'CINN',
		&ISUP_PT_CTRI	=> 'CTRI',
		&ISUP_PT_CHPI	=> 'CHPI',
		&ISUP_PT_CTI	=> 'CTI',
		&ISUP_PT_DISP	=> 'DISP',
		&ISUP_PT_UAI	=> 'UAI',
		&ISUP_PT_UCI	=> 'UCI',
		&ISUP_PT_RC	=> 'RC',
		&ISUP_PT_AT	=> 'AT',
		&ISUP_PT_CCR	=> 'CCR',
		&ISUP_PT_CPI	=> 'CPI',
		&ISUP_PT_PVTCA	=> 'PVTCA',
		&ISUP_PT_PVTRI	=> 'PVTRI',
		&ISUP_PT_CDDN	=> 'CDDN',
		&ISUP_PT_OCDIN	=> 'OCDIN',
		&ISUP_PT_GL	=> 'GL',
		&ISUP_PT_HTRI	=> 'HTRI',
		&ISUP_PT_NRN	=> 'NRN',
		&ISUP_PT_QORC	=> 'QORC',
		&ISUP_PT_PVTS	=> 'PVTS',
		&ISUP_PT_PVTC	=> 'PVTC',
		&ISUP_PT_PVTFI	=> 'PVTFI',
		&ISUP_PT_PVTBI	=> 'PVTBI',
		&ISUP_PT_RDS	=> 'RDS',
		&ISUP_PT_RDFI	=> 'RDFI',
		&ISUP_PT_RDBI	=> 'RDBI',
		&ISUP_PT_NPFI	=> 'NPFI',
		&ISUP_PT_GNUM	=> 'GNUM',
		&ISUP_PT_GDIG	=> 'GDIG',
		&ISUP_PT_OSI	=> 'OSI',
		&ISUP_PT_EGRESS	=> 'EGRESS',
		&ISUP_PT_JUR	=> 'JUR',
		&ISUP_PT_CIDC	=> 'CIDC',
		&ISUP_PT_BGROUP	=> 'BGROUP',
		&ISUP_PT_GNAM	=> 'GNAM',
		&ISUP_PT_NOTI	=> 'NOTI',
		&ISUP_PT_CSPI	=> 'CSPI',
		&ISUP_PT_TRNSRQ	=> 'TRNSRQ',
		&ISUP_PT_LSPI	=> 'LSPI',
		&ISUP_PT_CGCI	=> 'CGCI',
		&ISUP_PT_CVRI	=> 'CVRI',
		&ISUP_PT_OTGN	=> 'OTGN',
		&ISUP_PT_CIN	=> 'CIN',
		&ISUP_PT_CLLI	=> 'CLLI',
		&ISUP_PT_OLI	=> 'OLI',
		&ISUP_PT_CHGN	=> 'CHGN',
		&ISUP_PT_SVCD	=> 'SVCD',
		&ISUP_PT_SPR	=> 'SPR',
		&ISUP_PT_CSEL	=> 'CSEL',
		&ISUP_PT_NT	=> 'NT',
		&ISUP_PT_ORI	=> 'ORI',
		&ISUP_PT_IRI	=> 'IRI',
		&ISUP_PT_RATE	=> 'RATE',
		&ISUP_PT_IIC	=> 'IIC',
		&ISUP_PT_TOI	=> 'TOI',
		&ISUP_PT_TON	=> 'TON',
		&ISUP_PT_CRI	=> 'CRI',
		&ISUP_PT_ICCI	=> 'ICCI',
	},
};

sub unpack_oparms {
	my ($self,$ptr,$tag,$len,$parm,$sub) = @_;
	for (;;) {
		return unless $ptr < $self->{end};
		$tag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		return 1 if $tag == 0;
		return unless exists ISUP_PARM_TYPE->{$tag};
		$parm = ISUP_PARM_TYPE->{$tag};
		return unless $ptr < $self->{end};
		$len = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		return if $len == 0;
		return unless $ptr + $len <= $self->{end};
		return unless $sub = $self->can("unpack_PT_$parm");
		return unless $sub->($self,$ptr,$len);
		$ptr += $len;
	}
}

sub dissect_PT_EOP {
	my ($self,$off,$end,$len) = @_;
	return;
}
sub dissect_PT_CLRF {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TMR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ATP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CDPN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SUBN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_FCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OFCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CPC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CGPN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDGN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDNN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CONR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_INRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_INFI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_COTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_BCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CAUS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CGI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CMI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_FACI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_FAII {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CUGI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_INDEX {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CUGC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_USI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SPC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_UUI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CONN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SRIS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TNS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_EVNT {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CAM {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CSI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ACL {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OCDN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OBCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_UUIND {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ISPC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GNOT {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CHI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ADI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NSF {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_USIP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PROP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ROPS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_UTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TMU {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CDI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ECI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_MCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_MLPP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_MCIQ {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_MCIR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_HOPC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TMRP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_LN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDNR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_FREEP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GREF {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CTR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_LPI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CTN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CCSS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_FGVSN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_BGVNS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDCAP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NMC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CID {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SCFID {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CDTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CINN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CTRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CHPI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_DISP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_UAI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_UCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_AT {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CCR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CPI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTCA {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CDDN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OCDIN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GL {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_HTRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NRN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_QORC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTFI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_PVTBI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDFI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RDBI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NPFI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GNUM {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GDIG {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OSI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_EGRESS {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_JUR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CIDC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_BGROUP {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_GNAM {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NOTI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CSPI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TRNSRQ {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_LSPI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CGCI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CVRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OTGN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CIN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CLLI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_OLI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CHGN {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SVCD {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_SPR {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CSEL {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_NT {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ORI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_IRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_RATE {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_IIC {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TOI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_TON {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_CRI {
	my ($self,$off,$end,$len) = @_;
}
sub dissect_PT_ICCI {
	my ($self,$off,$end,$len) = @_;
}

use constant {
	ISUP_PARMS_DISSECT=>{
		&ISUP_PT_EOP	=> [\&Message::ISUP::dissect_PT_EOP,	'EOP',	 'End of optional parameters'],
		&ISUP_PT_CLRF	=> [\&Message::ISUP::dissect_PT_CLRF,	'CLRF',	 'Call reference @'],
		&ISUP_PT_TMR	=> [\&Message::ISUP::dissect_PT_TMR,	'TMR',	 'Transmission medium requirement (not ANSI)'],
		&ISUP_PT_ATP	=> [\&Message::ISUP::dissect_PT_ATP,	'ATP',	 'Access transport'],
		&ISUP_PT_CDPN	=> [\&Message::ISUP::dissect_PT_CDPN,	'CDPN',	 'Called party number'],
		&ISUP_PT_SUBN	=> [\&Message::ISUP::dissect_PT_SUBN,	'SUBN',	 'Subsequent number (not ANSI)'],
		&ISUP_PT_NCI	=> [\&Message::ISUP::dissect_PT_NCI,	'NCI',	 'Nature of connection indicators'],
		&ISUP_PT_FCI	=> [\&Message::ISUP::dissect_PT_FCI,	'FCI',	 'Forward call indicators'],
		&ISUP_PT_OFCI	=> [\&Message::ISUP::dissect_PT_OFCI,	'OFCI',	 'Optional forward call indicators'],
		&ISUP_PT_CPC	=> [\&Message::ISUP::dissect_PT_CPC,	'CPC',	 'Calling party\'s category'],
		&ISUP_PT_CGPN	=> [\&Message::ISUP::dissect_PT_CGPN,	'CGPN',	 'Calling party number'],
		&ISUP_PT_RDGN	=> [\&Message::ISUP::dissect_PT_RDGN,	'RDGN',	 'Redirecting number'],
		&ISUP_PT_RDNN	=> [\&Message::ISUP::dissect_PT_RDNN,	'RDNN',	 'Redirection number'],
		&ISUP_PT_CONR	=> [\&Message::ISUP::dissect_PT_CONR,	'CONR',	 'Connection request'],
		&ISUP_PT_INRI	=> [\&Message::ISUP::dissect_PT_INRI,	'INRI',	 'Information request indicators (Nat use)'],
		&ISUP_PT_INFI	=> [\&Message::ISUP::dissect_PT_INFI,	'INFI',	 'Information indicators (Nat use) @'],
		&ISUP_PT_COTI	=> [\&Message::ISUP::dissect_PT_COTI,	'COTI',	 'Continuity indicators'],
		&ISUP_PT_BCI	=> [\&Message::ISUP::dissect_PT_BCI,	'BCI',	 'Backward call indicators'],
		&ISUP_PT_CAUS	=> [\&Message::ISUP::dissect_PT_CAUS,	'CAUS',	 'Cause indicators'],
		&ISUP_PT_RDI	=> [\&Message::ISUP::dissect_PT_RDI,	'RDI',	 'Redirection information'],
		&ISUP_PT_CGI	=> [\&Message::ISUP::dissect_PT_CGI,	'CGI',	 'Circuit group supervision message type'],
		&ISUP_PT_RS	=> [\&Message::ISUP::dissect_PT_RS,	'RS',	 'Range and status'],
		&ISUP_PT_CMI	=> [\&Message::ISUP::dissect_PT_CMI,	'CMI',	 'Call modification indicators (Blue Book) (not ANSI)'],
		&ISUP_PT_FACI	=> [\&Message::ISUP::dissect_PT_FACI,	'FACI',	 'Facility indicator (not ANSI)'],
		&ISUP_PT_FAII	=> [\&Message::ISUP::dissect_PT_FAII,	'FAII',	 'Facility information indicators (Bellcore) (not ANSI)'],
		&ISUP_PT_CUGI	=> [\&Message::ISUP::dissect_PT_CUGI,	'CUGI',	 'Closed user group interlock code (not ANSI)'],
		&ISUP_PT_INDEX	=> [\&Message::ISUP::dissect_PT_INDEX,	'INDEX', 'Index (Bellcore) (not ANSI)'],
		&ISUP_PT_CUGC	=> [\&Message::ISUP::dissect_PT_CUGC,	'CUGC',	 'Closed user group check response indicators (not ANSI)'],
		&ISUP_PT_USI	=> [\&Message::ISUP::dissect_PT_USI,	'USI',	 'User service information'],
		&ISUP_PT_SPC	=> [\&Message::ISUP::dissect_PT_SPC,	'SPC',	 'Signalling point code (Nat use) (not ANSI) (Bellcore) @'],
		&ISUP_PT_UUI	=> [\&Message::ISUP::dissect_PT_UUI,	'UUI',	 'User-to-user information'],
		&ISUP_PT_CONN	=> [\&Message::ISUP::dissect_PT_CONN,	'CONN',	 'Connected number (not ANSI)'],
		&ISUP_PT_SRIS	=> [\&Message::ISUP::dissect_PT_SRIS,	'SRIS',	 'Suspend/resume indicators'],
		&ISUP_PT_TNS	=> [\&Message::ISUP::dissect_PT_TNS,	'TNS',	 'Transit network selection (Nat use) @'],
		&ISUP_PT_EVNT	=> [\&Message::ISUP::dissect_PT_EVNT,	'EVNT',	 'Event information indicators'],
		&ISUP_PT_CAM	=> [\&Message::ISUP::dissect_PT_CAM,	'CAM',	 'Circuit assignment map'],
		&ISUP_PT_CSI	=> [\&Message::ISUP::dissect_PT_CSI,	'CSI',	 'Circuit state indicator @'],
		&ISUP_PT_ACL	=> [\&Message::ISUP::dissect_PT_ACL,	'ACL',	 'Automatic congestion level'],
		&ISUP_PT_OCDN	=> [\&Message::ISUP::dissect_PT_OCDN,	'OCDN',	 'Original called number'],
		&ISUP_PT_OBCI	=> [\&Message::ISUP::dissect_PT_OBCI,	'OBCI',	 'Optional backward call indicators'],
		&ISUP_PT_UUIND	=> [\&Message::ISUP::dissect_PT_UUIND,	'UUIND', 'User-to-user indicators'],
		&ISUP_PT_ISPC	=> [\&Message::ISUP::dissect_PT_ISPC,	'ISPC',	 'Origination ISC point code (not ANSI)'],
		&ISUP_PT_GNOT	=> [\&Message::ISUP::dissect_PT_GNOT,	'GNOT',	 'Generic notification (not ANSI)'],
		&ISUP_PT_CHI	=> [\&Message::ISUP::dissect_PT_CHI,	'CHI',	 'Call history information (not ANSI)'],
		&ISUP_PT_ADI	=> [\&Message::ISUP::dissect_PT_ADI,	'ADI',	 'Access delivery information (not ANSI)'],
		&ISUP_PT_NSF	=> [\&Message::ISUP::dissect_PT_NSF,	'NSF',	 'Network specific facility (Nat use) @'],
		&ISUP_PT_USIP	=> [\&Message::ISUP::dissect_PT_USIP,	'USIP',	 'User service information prime'],
		&ISUP_PT_PROP	=> [\&Message::ISUP::dissect_PT_PROP,	'PROP',	 'Propagation delay counter (not ANSI)'],
		&ISUP_PT_ROPS	=> [\&Message::ISUP::dissect_PT_ROPS,	'ROPS',	 'Remote operations (Nat use) @'],
		&ISUP_PT_SA	=> [\&Message::ISUP::dissect_PT_SA,	'SA',	 'Service activation @'],
		&ISUP_PT_UTI	=> [\&Message::ISUP::dissect_PT_UTI,	'UTI',	 'User teleservice information (not ANSI)'],
		&ISUP_PT_TMU	=> [\&Message::ISUP::dissect_PT_TMU,	'TMU',	 'Transmission medium used'],
		&ISUP_PT_CDI	=> [\&Message::ISUP::dissect_PT_CDI,	'CDI',	 'Call diversion information (not ANSI)'],
		&ISUP_PT_ECI	=> [\&Message::ISUP::dissect_PT_ECI,	'ECI',	 'Echo control information (not ANSI)'],
		&ISUP_PT_MCI	=> [\&Message::ISUP::dissect_PT_MCI,	'MCI',	 'Message compatibility information'],
		&ISUP_PT_PCI	=> [\&Message::ISUP::dissect_PT_PCI,	'PCI',	 'Parameter compatibility information'],
		&ISUP_PT_MLPP	=> [\&Message::ISUP::dissect_PT_MLPP,	'MLPP',	 'MLPP precedence (Precedence ANSI)'],
		&ISUP_PT_MCIQ	=> [\&Message::ISUP::dissect_PT_MCIQ,	'MCIQ',	 'MCID request indicators (not ANSI)'],
		&ISUP_PT_MCIR	=> [\&Message::ISUP::dissect_PT_MCIR,	'MCIR',	 'MCID response indicators (not ANSI)'],
		&ISUP_PT_HOPC	=> [\&Message::ISUP::dissect_PT_HOPC,	'HOPC',	 'Hop counter (reserved)'],
		&ISUP_PT_TMRP	=> [\&Message::ISUP::dissect_PT_TMRP,	'TMRP',	 'Transmission medium requirement prime (not ANSI)'],
		&ISUP_PT_LN	=> [\&Message::ISUP::dissect_PT_LN,	'LN',	 'Location number (not ANSI)'],
		&ISUP_PT_RDNR	=> [\&Message::ISUP::dissect_PT_RDNR,	'RDNR',	 'Redirection number restriction (not ANSI)'],
		&ISUP_PT_FREEP	=> [\&Message::ISUP::dissect_PT_FREEP,	'FREEP', 'Freephone indicators/Reserved (Q.763 1992, White Book) (not ANSI)'],
		&ISUP_PT_GREF	=> [\&Message::ISUP::dissect_PT_GREF,	'GREF',	 'Generic reference/Reserved (Q.763 1992, White Book) (not ANSI)'],
		&ISUP_PT_CTR	=> [\&Message::ISUP::dissect_PT_CTR,	'CTR',	 'Call transfer reference (Q.763) (not ANSI)'],
		&ISUP_PT_LPI	=> [\&Message::ISUP::dissect_PT_LPI,	'LPI',	 'Loop prevention indicators (Q.763) (not ANSI)'],
		&ISUP_PT_CTN	=> [\&Message::ISUP::dissect_PT_CTN,	'CTN',	 'Call transfer number (Q.763) (not ANSI)'],
		&ISUP_PT_CCSS	=> [\&Message::ISUP::dissect_PT_CCSS,	'CCSS',	 'CCSS (Q.763) (not ANSI)'],
		&ISUP_PT_FGVSN	=> [\&Message::ISUP::dissect_PT_FGVSN,	'FGVSN', 'Forward GVNS (Q.763) (not ANSI)'],
		&ISUP_PT_BGVNS	=> [\&Message::ISUP::dissect_PT_BGVNS,	'BGVNS', 'Backward GVNS (Q.763) (not ANSI)'],
		&ISUP_PT_RDCAP	=> [\&Message::ISUP::dissect_PT_RDCAP,	'RDCAP', 'Redirect capability (Nat use) (ANSI/GR-246-CORE)'],
		&ISUP_PT_NMC	=> [\&Message::ISUP::dissect_PT_NMC,	'NMC',	 'Network management controls (ANSI/GR-246-CORE)'],
		&ISUP_PT_CID	=> [\&Message::ISUP::dissect_PT_CID,	'CID',	 'Correlation id (Q.763) (not ANSI)'],
		&ISUP_PT_SCFID	=> [\&Message::ISUP::dissect_PT_SCFID,	'SCFID', 'SCF id (Q.763) (not ANSI)'],
		&ISUP_PT_CDTI	=> [\&Message::ISUP::dissect_PT_CDTI,	'CDTI',	 'Call diversion treatment indicators (Q.763) (not ANSI)'],
		&ISUP_PT_CINN	=> [\&Message::ISUP::dissect_PT_CINN,	'CINN',	 'Called IN number (Q.763) (not ANSI)'],
		&ISUP_PT_CTRI	=> [\&Message::ISUP::dissect_PT_CTRI,	'CTRI',	 'Call offering treatment indicators (Q.763) (not ANSI)'],
		&ISUP_PT_CHPI	=> [\&Message::ISUP::dissect_PT_CHPI,	'CHPI',	 'Charged party identification (Q.763) (not ANSI)'],
		&ISUP_PT_CTI	=> [\&Message::ISUP::dissect_PT_CTI,	'CTI',	 'Conference treatment indicators (Q.763) (not ANSI)'],
		&ISUP_PT_DISP	=> [\&Message::ISUP::dissect_PT_DISP,	'DISP',	 'Display information (Q.763) (not ANSI)'],
		&ISUP_PT_UAI	=> [\&Message::ISUP::dissect_PT_UAI,	'UAI',	 'UID action indicators (Q.763) (not ANSI)'],
		&ISUP_PT_UCI	=> [\&Message::ISUP::dissect_PT_UCI,	'UCI',	 'UID capability indicators (Q.763) (not ANSI)'],
		&ISUP_PT_RC	=> [\&Message::ISUP::dissect_PT_RC,	'RC',	 'Redirect counter (Nat use) (ANSI/GR-246-CORE)/Reserved (B-ISUP)'],
		&ISUP_PT_AT	=> [\&Message::ISUP::dissect_PT_AT,	'AT',	 'Application transport (Q.763)/Reserved (B-ISUP) (not ANSI)'],
		&ISUP_PT_CCR	=> [\&Message::ISUP::dissect_PT_CCR,	'CCR',	 'Collect call request (Q.763)/Reserved (B-ISUP) (not ANSI)'],
		&ISUP_PT_CPI	=> [\&Message::ISUP::dissect_PT_CPI,	'CPI',	 'CCNR possible indicator (Q.763)/Reserved (B-ISUP) (not ANSI)'],
		&ISUP_PT_PVTCA	=> [\&Message::ISUP::dissect_PT_PVTCA,	'PVTCA', 'Pivot capability (ANSI/Q.763)/Reserved (B-ISUP)'],
		&ISUP_PT_PVTRI	=> [\&Message::ISUP::dissect_PT_PVTRI,	'PVTRI', 'Pivot routing indicators (ANSI/Q.763)/Reserved (B-ISUP)'],
		&ISUP_PT_CDDN	=> [\&Message::ISUP::dissect_PT_CDDN,	'CDDN',	 'Called directory number (Q.763)/Reserved (B-ISUP) (not ANSI)'],
		&ISUP_PT_OCDIN	=> [\&Message::ISUP::dissect_PT_OCDIN,	'OCDIN', 'Original called IN number (Q.763) (not ANSI)'],
		&ISUP_PT_GL	=> [\&Message::ISUP::dissect_PT_GL,	'GL',	 'Calling geodetic location (ANSI/GR-246-CORE)'],
		&ISUP_PT_HTRI	=> [\&Message::ISUP::dissect_PT_HTRI,	'HTRI',	 'HTR information (Q.763) (not ANSI)'],
		&ISUP_PT_NRN	=> [\&Message::ISUP::dissect_PT_NRN,	'NRN',	 'Network routing number (Nat use) (Q.763) (not ANSI)'],
		&ISUP_PT_QORC	=> [\&Message::ISUP::dissect_PT_QORC,	'QORC',	 'Query on release capability (Network option) (Q.763) (not ANSI)'],
		&ISUP_PT_PVTS	=> [\&Message::ISUP::dissect_PT_PVTS,	'PVTS',	 'Pivot status (Nat use) (ANSI/Q.763) (not ANSI)'],
		&ISUP_PT_PVTC	=> [\&Message::ISUP::dissect_PT_PVTC,	'PVTC',	 'Pivot counter (ANSI/Q.763) (not ANSI)'],
		&ISUP_PT_PVTFI	=> [\&Message::ISUP::dissect_PT_PVTFI,	'PVTFI', 'Pivot routing forward information (ANSI/Q.763) (not ANSI)'],
		&ISUP_PT_PVTBI	=> [\&Message::ISUP::dissect_PT_PVTBI,	'PVTBI', 'Pivot routing backward information (ANSI/Q.763) (not ANSI)'],
		&ISUP_PT_RDS	=> [\&Message::ISUP::dissect_PT_RDS,	'RDS',	 'Redirect status (Nat use)'],
		&ISUP_PT_RDFI	=> [\&Message::ISUP::dissect_PT_RDFI,	'RDFI',	 'Redirect forward information (Nat use)'],
		&ISUP_PT_RDBI	=> [\&Message::ISUP::dissect_PT_RDBI,	'RDBI',	 'Redirect backward information (Nat use)'],
		&ISUP_PT_NPFI	=> [\&Message::ISUP::dissect_PT_NPFI,	'NPFI',	 'Number portability forward information (Network option) (not ANSI)'],
		&ISUP_PT_GNUM	=> [\&Message::ISUP::dissect_PT_GNUM,	'GNUM',	 'Generic number (Q.763)/Generic address (ANSI/GR-246-CORE)'],
		&ISUP_PT_GDIG	=> [\&Message::ISUP::dissect_PT_GDIG,	'GDIG',	 'Generic digits (Nat use)'],
		&ISUP_PT_OSI	=> [\&Message::ISUP::dissect_PT_OSI,	'OSI',	 'Operator services information (ANSI/GR-246-CORE)'],
		&ISUP_PT_EGRESS	=> [\&Message::ISUP::dissect_PT_EGRESS,	'EGRESS','Egress (ANSI/GR-246-CORE)'],
		&ISUP_PT_JUR	=> [\&Message::ISUP::dissect_PT_JUR,	'JUR',	 'Jurisdiction (ANSI/GR-246-CORE)'],
		&ISUP_PT_CIDC	=> [\&Message::ISUP::dissect_PT_CIDC,	'CIDC',	 'Carrier identification (ANSI/GR-246-CORE)'],
		&ISUP_PT_BGROUP	=> [\&Message::ISUP::dissect_PT_BGROUP,	'BGROUP','Business group (ANSI/GR-246-CORE)'],
		&ISUP_PT_GNAM	=> [\&Message::ISUP::dissect_PT_GNAM,	'GNAM',	 'Generic name (ANSI/GR-246-CORE)'],
		&ISUP_PT_NOTI	=> [\&Message::ISUP::dissect_PT_NOTI,	'NOTI',	 'Notification indicator (ANSI/GR-246-CORE)'],
		&ISUP_PT_CSPI	=> [\&Message::ISUP::dissect_PT_CSPI,	'CSPI',	 'Carrier service provider identification (ANSI)'],
		&ISUP_PT_TRNSRQ	=> [\&Message::ISUP::dissect_PT_TRNSRQ,	'TRNSRQ','Transaction request (ANSI/GR-246-CORE)'],
		&ISUP_PT_LSPI	=> [\&Message::ISUP::dissect_PT_LSPI,	'LSPI',	 'Local service provider identification (ANSI)'],
		&ISUP_PT_CGCI	=> [\&Message::ISUP::dissect_PT_CGCI,	'CGCI',	 'Circuit group characteristic indicator (ANSI/GR-246-CORE)'],
		&ISUP_PT_CVRI	=> [\&Message::ISUP::dissect_PT_CVRI,	'CVRI',	 'Circuit validation response indicator (ANSI/GR-246-CORE)'],
		&ISUP_PT_OTGN	=> [\&Message::ISUP::dissect_PT_OTGN,	'OTGN',	 'Outgoing trunk group number (ANSI/GR-246-CORE)'],
		&ISUP_PT_CIN	=> [\&Message::ISUP::dissect_PT_CIN,	'CIN',	 'Circuit identification name (ANSI/GR-246-CORE)'],
		&ISUP_PT_CLLI	=> [\&Message::ISUP::dissect_PT_CLLI,	'CLLI',	 'Common language location identification (ANSI/GR-246-CORE)'],
		&ISUP_PT_OLI	=> [\&Message::ISUP::dissect_PT_OLI,	'OLI',	 'Originating line information (ANSI/GR-246-CORE)'],
		&ISUP_PT_CHGN	=> [\&Message::ISUP::dissect_PT_CHGN,	'CHGN',	 'Charge number (ANSI/GR-246-CORE)'],
		&ISUP_PT_SVCD	=> [\&Message::ISUP::dissect_PT_SVCD,	'SVCD',	 'Service code indicator (ANSI/GR-246-CORE)'],
		&ISUP_PT_SPR	=> [\&Message::ISUP::dissect_PT_SPR,	'SPR',	 'Special processing request (ANSI/GR-246-CORE)'],
		&ISUP_PT_CSEL	=> [\&Message::ISUP::dissect_PT_CSEL,	'CSEL',	 'Carrier selection information (ANSI/GR-246-CORE)'],
		&ISUP_PT_NT	=> [\&Message::ISUP::dissect_PT_NT,	'NT',	 'Network transport (ANSI/GR-246-CORE)'],
		&ISUP_PT_ORI	=> [\&Message::ISUP::dissect_PT_ORI,	'ORI',	 'Outgoing route identification (Spain)'],
		&ISUP_PT_IRI	=> [\&Message::ISUP::dissect_PT_IRI,	'IRI',	 'Incoming route identification (Spain)'],
		&ISUP_PT_RATE	=> [\&Message::ISUP::dissect_PT_RATE,	'RATE',	 'Rate (Spain)'],
		&ISUP_PT_IIC	=> [\&Message::ISUP::dissect_PT_IIC,	'IIC',	 'Identifier of incoming circuit (Spain)'],
		&ISUP_PT_TOI	=> [\&Message::ISUP::dissect_PT_TOI,	'TOI',	 'Trunk offering information (Singapore)'],
		&ISUP_PT_TON	=> [\&Message::ISUP::dissect_PT_TON,	'TON',	 'Type of notification (Spain)'],
		&ISUP_PT_CRI	=> [\&Message::ISUP::dissect_PT_CRI,	'CRI',	 'Charge rate information (Singapore)'],
		&ISUP_PT_ICCI	=> [\&Message::ISUP::dissect_PT_ICCI,	'ICCI',	 'Call charge information (Signapore)'],
	},
};

sub dissect_oparms {
	my ($self,$off,$end) = @_;
	my @oparms = ();
	for (;$off < $end;$off++) {
		my @fields = ();
		my $ptr = unpack('C',substr(${$self->{buf}},$off,1));
		push @fields, [$off,Frame::bytes($ptr,0xff,1),'Pointer',$ptr];
		my $poff = $off + $ptr;
		return unless $poff <= $end;
		my $ptag = unpack('C',substr(${$self->{buf}},$poff,1));
		my $parm;
		if (exists ISUP_PARMS_DISSECT->{$ptag}) {
			my @args = @{ISUP_PARMS_DISSECT->{$ptag}}; my $sub = shift @args;
			$parm = sprintf '%s(%d) - %s', $args[0], $ptag, $args[1];
			push @fields, [$poff,Frame::bytes($ptag,0xff,1),'Tag',$parm],
			$poff++;
			my ($plen,$pend) = (undef,$poff);
			if ($ptag) {
				$plen = unpack('C',substr(${$self->{buf}},$poff,1));
				push @fields, [$poff,Frame::bytes($plen,0xff,1),'Length',$plen];
				$poff++;
				$pend = $poff + $plen;
			}
			return unless $pend <= $self->{end};
			push @fields, @{$sub->($self,$poff,$pend,$plen,@args,@_)};
		} else {
			$parm = "Unknown($ptag) - Unknown parameter tag";
			push @fields, [$poff,Frame::bytes($ptag,0xff,1),'Tag',$parm];
			$poff++;
			my $plen = unpack('C',substr(${$self->{buf}},$poff,1));
			push @fields, [$poff,Frame::bytes($plen,0xff,1),'Length',$plen];
			$poff++;
			my $pend = $poff + $plen;
			return unless $pend <= $self->{end};
			push @fields, [$poff,' ******** ','Parameter',Frame::pdata(substr(${$self->{buf}},$poff,$plen))];
		}
		push @oparms, "Parameter $parm", \@fields;

	}
	return [ 'Optional Parameters', \@oparms ];
}

sub pandl {
	my $self = shift;
	my ($p,$l) = ($self->{off},0);
	if ($p < $self->{end}) {
		$p = unpack('C',substr($$self->{buf},$self->{off},1)) + $self->{off}; $self->{off}++;
		if ($p < $self->{end}) {
			$l = unpack('C',substr($$self->{buf},$p,1)); $p++;
			$l = 0 unless $p + $l <= $self->{end};
		}
	}
	return $p,$l;
}

# -------------------------------------
package Message::ISUP::NONE; our @ISA = qw(Message::ISUP);
# -------------------------------------
sub unpack {
	return shift;
}
# -------------------------------------
package Message::ISUP::OPARMS; our @ISA = qw(Message::ISUP);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_OP($self,$off);
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_OP($self,$off),
	);
}

# -------------------------------------
package Message::ITUT::ISUP; our @ISA = qw(Message::ISUP);
# -------------------------------------
use constant {
	# This is not really just ITUT but anything with a 14-bit point code.
	ISUP_ITUT_TYPES=>{
		&ISUP_MT_IAM	=> 'IAM',   # 0000 0001	Initial address
		&ISUP_MT_SAM	=> 'SAM',   # 0000 0010	Subsequent address message
		&ISUP_MT_INR	=> 'INR',   # 0000 0011	Information request (national use)
		&ISUP_MT_INF	=> 'INF',   # 0000 0100	Information response (national use)
		&ISUP_MT_COT	=> 'COT',   # 0000 0101	Continuity
		&ISUP_MT_ACM	=> 'ACM',   # 0000 0110	Address complete
		&ISUP_MT_CON	=> 'CON',   # 0000 0111	Connect
		&ISUP_MT_FOT	=> 'FOT',   # 0000 1000	Forward transfer
		&ISUP_MT_ANM	=> 'ANM',   # 0000 1001	Answer
		#0x0a		=> '???',   # 0000 1010	Reserved (Red book)
		#0x0b		=> '???',   # 0000 1011	Reserved (Red book)
		&ISUP_MT_REL	=> 'REL',   # 0000 1100	Release
		&ISUP_MT_SUS	=> 'SUS',   # 0000 1101	Suspend
		&ISUP_MT_RES	=> 'RES',   # 0000 1110	Resume
		#0x0f		=> '???',   # 0000 1111	Reserved (Red book)
		&ISUP_MT_RLC	=> 'RLC',   # 0001 0000	Release complete
		&ISUP_MT_CCR	=> 'CCR',   # 0001 0001	Continuity check request
		&ISUP_MT_RSC	=> 'RSC',   # 0001 0010	Reset circuit
		&ISUP_MT_BLO	=> 'BLO',   # 0001 0011	Blocking
		&ISUP_MT_UBL	=> 'UBL',   # 0001 0100	Unblocking
		&ISUP_MT_BLA	=> 'BLA',   # 0001 0101	Blocking acknowledgement
		&ISUP_MT_UBA	=> 'UBA',   # 0001 0110	Unblocking acknowledgement
		&ISUP_MT_GRS	=> 'GRS',   # 0001 0111	Circuit group reset
		&ISUP_MT_CGB	=> 'CGB',   # 0001 1000	Circuit group blocking
		&ISUP_MT_CGU	=> 'CGU',   # 0001 1001	Circuit group unblocking
		&ISUP_MT_CGBA	=> 'CGBA',  # 0001 1010	Circuit group blocking acknowledgement
		&ISUP_MT_CGUA	=> 'CGUA',  # 0001 1011	Circuit group ublocking acknowledgement
		#&ISUP_MT_CMR	=> 'CMR',   # 0001 1100	Call modification request (Reserved (Blue book))
		#&ISUP_MT_CMC	=> 'CMC',   # 0001 1101	Call modification complete (Reserved (Blue book))
		#&ISUP_MT_CMRJ	=> 'CMRJ',  # 0001 1110	Call modification reject (Reserved (Blue book))
		&ISUP_MT_FAR	=> 'FAR'    # 0001 1111	Facility request,
		&ISUP_MT_FAA	=> 'FAA',   # 0010 0000	Facility accepted
		&ISUP_MT_FRJ	=> 'FRJ',   # 0010 0001	Facility reject
		#&ISUP_MT_FAD	=> 'FAD',   # 0010 0010	Facility deactivate (Reserved (Red book))
		#&ISUP_MT_FAI	=> 'FAI',   # 0010 0011	Facility information (Reserved (Red book))
		&ISUP_MT_LPA	=> 'LPA',   # 0010 0100	Loop back acknowledgement (national)
		#&ISUP_MT_CSVQ	=> 'CSVQ',  # 0010 0101	Reserved (Red book)
		#&ISUP_MT_CSVR	=> 'CSVR',  # 0010 0110	Reserved (Red book)
		#&ISUP_MT_DRS	=> 'DRS',   # 0010 0111	Delayed release (Reserved (Blue book))
		&ISUP_MT_PAM	=> 'PAM',   # 0010 1000	Pass along message (national)
		&ISUP_MT_GRA	=> 'GRA',   # 0010 1001	Circuit group reset acknowledgement
		&ISUP_MT_CQM	=> 'CQM',   # 0010 1010	Circuit group query (national)
		&ISUP_MT_CQR	=> 'CQR',   # 0010 1011	Circuit group query response (national)
		&ISUP_MT_CPG	=> 'CPG',   # 0010 1100	Call progress
		&ISUP_MT_USR	=> 'USR',   # 0010 1101	User to user information
		&ISUP_MT_UCIC	=> 'UCIC',  # 0010 1110	Unequipped CIC (national)
		&ISUP_MT_CFN	=> 'CFN',   # 0010 1111	Confusion
		&ISUP_MT_OLM	=> 'OLM',   # 0011 0000	Overload (national)
		&ISUP_MT_CRG	=> 'CRG',   # 0011 0001	Charge information (national)
		&ISUP_MT_NRM	=> 'NRM',   # 0011 0010	Network resource managment
		&ISUP_MT_FAC	=> 'FAC',   # 0011 0011	Facility
		&ISUP_MT_UPT	=> 'UPT',   # 0011 0100	User part test
		&ISUP_MT_UPA	=> 'UPA',   # 0011 0101	User part available
		&ISUP_MT_IDR	=> 'IDR',   # 0011 0110	Identification request
		&ISUP_MT_IRS	=> 'IRS',   # 0011 0111	Identification response
		&ISUP_MT_SGM	=> 'SGM',   # 0011 1000	Segmentation
		#0x39		=> '???',   # 0011 1001	Reserved (B-ISUP)
		#0x3a		=> '???',   # 0011 1010	Reserved (B-ISUP)
		#0x3b		=> '???',   # 0011 1011	Reserved (B-ISUP)
		#0x3c		=> '???',   # 0011 1100	Reserved (B-ISUP)
		#0x3d		=> '???',   # 0011 1101	Reserved (B-ISUP)
		#0x3e		=> '???',   # 0011 1110
		#0x3f		=> '???',   # 0011 1111
		&ISUP_MT_LPR	=> 'LPR',   # 0100 0000	Loop preservation
		&ISUP_MT_ATP	=> 'ATP',   # 0100 0001	Application transport
		&ISUP_MT_PRI	=> 'PRI',   # 0100 0010	Pre-releation information
		&ISUP_MT_SDN	=> 'SDN',   # 0100 0011	Subsequent directory number (national)
		#
		#&ISUP_MT_CRA	=> 'CRA',   # 1110 1001	Circuit reservation acknowledgment
		#&ISUP_MT_CRM	=> 'CRM',   # 1110 1010	Circuit reservation
		#&ISUP_MT_CVR	=> 'CVR',   # 1110 1011	Circuit validation response
		#&ISUP_MT_CVT	=> 'CVT',   # 1110 1100	Circuit validation test
		#&ISUP_MT_EXM	=> 'EXM',   # 1110 1101	Exit message
		#
		&ISUP_MT_NON	=> 'NON',   # 1111 1000	National notification (Spain)
		#			    # 1111 1001
		#			    # 1111 1010
		#			    # 1111 1011
		&ISUP_MT_LLM	=> 'LLM',   # 1111 1100	National malicioius call (Spain),
		&ISUP_MT_CAK	=> 'CAK',   # 1111 1101	Charge acknowledgement (Singapore)
		&ISUP_MT_TCM	=> 'TCM',   # 1111 1110	Tariff charge (Signapore)
		&ISUP_MT_MCP	=> 'MCP',   # 1111 1111	Malicious call print (Singapore)
	},
};
# -------------------------------------
package Message::ANSI::ISUP; our @ISA = qw(Message::ISUP);
# -------------------------------------
use constant {
	# This is not really just ANSI but anything with a 24-bit point code.
	ISUP_ANSI_TYPES=>{
		&ISUP_MT_IAM	=> 'IAM',   # 0000 0001	Initial address
		#&ISUP_MT_SAM	=> 'SAM',   # 0000 0010	Subsequent address message
		&ISUP_MT_INR	=> 'INR',   # 0000 0011	Information request (national use)
		&ISUP_MT_INF	=> 'INF',   # 0000 0100	Information response (national use)
		&ISUP_MT_COT	=> 'COT',   # 0000 0101	Continuity
		&ISUP_MT_ACM	=> 'ACM',   # 0000 0110	Address complete
		#&ISUP_MT_CON	=> 'CON',   # 0000 0111	Connect
		&ISUP_MT_FOT	=> 'FOT',   # 0000 1000	Forward transfer
		&ISUP_MT_ANM	=> 'ANM',   # 0000 1001	Answer
		#0x0a		=> '???',   # 0000 1010	Reserved (Red book)
		#0x0b		=> '???',   # 0000 1011	Reserved (Red book)
		&ISUP_MT_REL	=> 'REL',   # 0000 1100	Release
		&ISUP_MT_SUS	=> 'SUS',   # 0000 1101	Suspend
		&ISUP_MT_RES	=> 'RES',   # 0000 1110	Resume
		#0x0f		=> '???',   # 0000 1111	Reserved (Red book)
		&ISUP_MT_RLC	=> 'RLC',   # 0001 0000	Release complete
		&ISUP_MT_CCR	=> 'CCR',   # 0001 0001	Continuity check request
		&ISUP_MT_RSC	=> 'RSC',   # 0001 0010	Reset circuit
		&ISUP_MT_BLO	=> 'BLO',   # 0001 0011	Blocking
		&ISUP_MT_UBL	=> 'UBL',   # 0001 0100	Unblocking
		&ISUP_MT_BLA	=> 'BLA',   # 0001 0101	Blocking acknowledgement
		&ISUP_MT_UBA	=> 'UBA',   # 0001 0110	Unblocking acknowledgement
		&ISUP_MT_GRS	=> 'GRS',   # 0001 0111	Circuit group reset
		&ISUP_MT_CGB	=> 'CGB',   # 0001 1000	Circuit group blocking
		&ISUP_MT_CGU	=> 'CGU',   # 0001 1001	Circuit group unblocking
		&ISUP_MT_CGBA	=> 'CGBA',  # 0001 1010	Circuit group blocking acknowledgement
		&ISUP_MT_CGUA	=> 'CGUA',  # 0001 1011	Circuit group ublocking acknowledgement
		#&ISUP_MT_CMR	=> 'CMR',   # 0001 1100	Call modification request (Reserved (Blue book))
		#&ISUP_MT_CMC	=> 'CMC',   # 0001 1101	Call modification complete (Reserved (Blue book))
		#&ISUP_MT_CMRJ	=> 'CMRJ',  # 0001 1110	Call modification reject (Reserved (Blue book))
		#&ISUP_MT_FAR	=> 'FAR'    # 0001 1111	Facility request,
		#&ISUP_MT_FAA	=> 'FAA',   # 0010 0000	Facility accepted
		#&ISUP_MT_FRJ	=> 'FRJ',   # 0010 0001	Facility reject
		#&ISUP_MT_FAD	=> 'FAD',   # 0010 0010	Facility deactivate (Reserved (Red book))
		#&ISUP_MT_FAI	=> 'FAI',   # 0010 0011	Facility information (Reserved (Red book))
		&ISUP_MT_LPA	=> 'LPA',   # 0010 0100	Loop back acknowledgement (national)
		#&ISUP_MT_CSVQ	=> 'CSVQ',  # 0010 0101	Reserved (Red book)
		#&ISUP_MT_CSVR	=> 'CSVR',  # 0010 0110	Reserved (Red book)
		#&ISUP_MT_DRS	=> 'DRS',   # 0010 0111	Delayed release (Reserved (Blue book))
		&ISUP_MT_PAM	=> 'PAM',   # 0010 1000	Pass along message (national)
		&ISUP_MT_GRA	=> 'GRA',   # 0010 1001	Circuit group reset acknowledgement
		&ISUP_MT_CQM	=> 'CQM',   # 0010 1010	Circuit group query (national)
		&ISUP_MT_CQR	=> 'CQR',   # 0010 1011	Circuit group query response (national)
		&ISUP_MT_CPG	=> 'CPG',   # 0010 1100	Call progress
		#&ISUP_MT_USR	=> 'USR',   # 0010 1101	User to user information
		&ISUP_MT_UCIC	=> 'UCIC',  # 0010 1110	Unequipped CIC (national)
		&ISUP_MT_CFN	=> 'CFN',   # 0010 1111	Confusion
		#&ISUP_MT_OLM	=> 'OLM',   # 0011 0000	Overload (national)
		#&ISUP_MT_CRG	=> 'CRG',   # 0011 0001	Charge information (national)
		#&ISUP_MT_NRM	=> 'NRM',   # 0011 0010	Network resource managment
		&ISUP_MT_FAC	=> 'FAC',   # 0011 0011	Facility
		#&ISUP_MT_UPT	=> 'UPT',   # 0011 0100	User part test
		#&ISUP_MT_UPA	=> 'UPA',   # 0011 0101	User part available
		#&ISUP_MT_IDR	=> 'IDR',   # 0011 0110	Identification request
		#&ISUP_MT_IRS	=> 'IRS',   # 0011 0111	Identification response
		&ISUP_MT_SGM	=> 'SGM',   # 0011 1000	Segmentation
		#0x39		=> '???',   # 0011 1001	Reserved (B-ISUP)
		#0x3a		=> '???',   # 0011 1010	Reserved (B-ISUP)
		#0x3b		=> '???',   # 0011 1011	Reserved (B-ISUP)
		#0x3c		=> '???',   # 0011 1100	Reserved (B-ISUP)
		#0x3d		=> '???',   # 0011 1101	Reserved (B-ISUP)
		#0x3e		=> '???',   # 0011 1110
		#0x3f		=> '???',   # 0011 1111
		#&ISUP_MT_LPR	=> 'LPR',   # 0100 0000	Loop preservation
		#&ISUP_MT_ATP	=> 'ATP',   # 0100 0001	Application transport
		#&ISUP_MT_PRI	=> 'PRI',   # 0100 0010	Pre-releation information
		#&ISUP_MT_SDN	=> 'SDN',   # 0100 0011	Subsequent directory number (national)
		#
		&ISUP_MT_CRA	=> 'CRA',   # 1110 1001	Circuit reservation acknowledgment
		&ISUP_MT_CRM	=> 'CRM',   # 1110 1010	Circuit reservation
		&ISUP_MT_CVR	=> 'CVR',   # 1110 1011	Circuit validation response
		&ISUP_MT_CVT	=> 'CVT',   # 1110 1100	Circuit validation test
		&ISUP_MT_EXM	=> 'EXM',   # 1110 1101	Exit message
		#
		#&ISUP_MT_NON	=> 'NON',   # 1111 1000	National notification (Spain)
		#			    # 1111 1001
		#			    # 1111 1010
		#			    # 1111 1011
		#&ISUP_MT_LLM	=> 'LLM',   # 1111 1100	National malicioius call (Spain),
		#&ISUP_MT_CAK	=> 'CAK',   # 1111 1101	Charge acknowledgement (Singapore)
		#&ISUP_MT_TCM	=> 'TCM',   # 1111 1110	Tariff charge (Signapore)
		#&ISUP_MT_MCP	=> 'MCP',   # 1111 1111	Malicious call print (Singapore)
	},
};

# -------------------------------------
package Message::ITUT::ISUP::NONE; our @ISA = qw(Message::ISUP::NONE);
package Message::ITUT::ISUP::OPARMS; our @ISA = qw(Message::ISUP::OPARMS);
# -------------------------------------
package Message::ANSI::ISUP::NONE; our @ISA = qw(Message::ISUP::NONE);
package Message::ANSI::ISUP::OPARMS; our @ISA = qw(Message::ISUP::OPARMS);
# -------------------------------------

# -------------------------------------
package Message::ITUT::ISUP::IAM;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: F(NCI FCI CPC TMR) V(CDPN) O(VEDI IEPS)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'NCI',  $off,1); $off++;
	return unless Message::unpack_MF($self,'FCI',  $off,2); $off+=2;
	return unless Message::unpack_MF($self,'CPC',  $off,1); $off++;
	return unless Message::unpack_MF($self,'TMR',  $off,1); $off++;
	return unless Message::unpack_MV($self,'CDPN', $off  ); $off++;
	return unless Message::unpack_OP($self,        $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'NCI',  $off+0,1),
		Message::dissect_MF($self,'FCI',  $off+1,2),
		Message::dissect_MF($self,'CPC',  $off+3,1),
		Message::dissect_MF($self,'TMR',  $off+4,1),
		Message::dissect_MV($self,'CDPN', $off+5  ),
		Message::dissect_OP($self,        $off+6  ),
	);
}
# -------------------------------------
package Message::ANSI::ISUP::IAM;  our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
#  ANSI: F(NCI FCI CPC) V(USI CDPN) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	return unless $self->unpack_MF('NCI', $self->{off},1); $self->{off} += 1;
	return unless $self->unpack_MF('FCI', $self->{off},2); $self->{off} += 2;
	return unless $self->unpack_MF('CPC', $self->{off},1); $self->{off} += 1;
	return unless $self->unpack_MV('USI', $self->{off}  ); $self->{off} += 1;
	return unless $self->unpack_MV('CDPN',$self->{off}  ); $self->{off} += 1;
	return unless $self->unpack_OP(       $self->{off}  );
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		$self->dissect_MF('NCI', $off+0,1),
		$self->dissect_MF('FCI', $off+1,2),
		$self->dissect_MF('CPC', $off+3,1),
		$self->dissect_MV('USI', $off+4),
		$self->dissect_MV('CDPN',$off+5),
		$self->dissect_OP(       $off+6),
	);
}
# -------------------------------------
package Message::ITUT::ISUP::SAM;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: V(SUBN) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'SUBN',$off); $off++;
	return unless Message::unpack_OP($self,       $off); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'SUBN',$off+0),
		Message::dissect_OP($self,       $off+1),
	);
}
# -------------------------------------
package Message::ISUP::INM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(INRI) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'INRI',$off,2); $off+=2;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'INRI',$off+0,2),
		Message::dissect_OP($self,       $off+1  ),
	);
}
package Message::ITUT::ISUP::INR; our @ISA = qw(Message::ISUP::INM Message::ITUT::ISUP);
package Message::ANSI::ISUP::INR; our @ISA = qw(Message::ISUP::INM Message::ANSI::ISUP);
package Message::ITUT::ISUP::INF; our @ISA = qw(Message::ISUP::INM Message::ITUT::ISUP);
package Message::ANSI::ISUP::INF; our @ISA = qw(Message::ISUP::INM Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::COT; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(COTI)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'COTI',$off,1); $off++;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::unpack_MF($self,'COTI',$off,1),
	);
}
package Message::ITUT::ISUP::COT; our @ISA = qw(Message::ISUP::COT Message::ITUT::ISUP);
package Message::ANSI::ISUP::COT; our @ISA = qw(Message::ISUP::COT Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::ACM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(BCI) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'BCI',$off,2); $off+=2;
	return unless Message::unpack_OP($self,      $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'BCI',$off+0,2),
		Message::dissect_OP($self,      $off+1  ),
	);
}
package Message::ITUT::ISUP::ACM; our @ISA = qw(Message::ISUP::ACM Message::ITUT::ISUP);
package Message::ANSI::ISUP::ACM; our @ISA = qw(Message::ISUP::ACM Message::ANSI::ISUP);
# -------------------------------------
package Message::ITUT::ISUP::CON;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: F(BCI) O(VEDI)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'BCI',$off,2); $off+=2;
	return unless Message::unpack_OP($self,      $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'BCI',$off+0,2),
		Message::dissect_OP($self,      $off+2  ),
	);
}
# -------------------------------------
package Message::ITUT::ISUP::FOT; our @ISA = qw(Message::ITUT::ISUP::OPARMS);
package Message::ANSI::ISUP::FOT; our @ISA = qw(Message::ANSI::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::ANM; our @ISA = qw(Message::ITUT::ISUP::OPARMS);
package Message::ANSI::ISUP::ANM; our @ISA = qw(Message::ANSI::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::REL;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ANSI::ISUP::REL;  our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
#  ANSI: V(CAUS) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'CAUS',$off); $off++;
	return unless Message::unpack_OP($self,       $off); $Off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'CAUS',$off+0),
		Message::dissect_OP($self,       $off+1),
	);
}
# -------------------------------------
package Message::ISUP::SRM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(SRIS) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'SRIS',$off,1); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'SRIS',$off+0,1),
		Message::dissect_OP($self,       $off+1  ),
	);
}
package Message::ITUT::ISUP::SUS; our @ISA = qw(Message::ISUP::SRM Message::ITUT::ISUP);
package Message::ANSI::ISUP::SUS; our @ISA = qw(Message::ISUP::SRM Message::ANSI::ISUP);
package Message::ITUT::ISUP::RES; our @ISA = qw(Message::ISUP::SRM Message::ITUT::ISUP);
package Message::ANSI::ISUP::RES; our @ISA = qw(Message::ISUP::SRM Message::ANSI::ISUP);
# -------------------------------------
package Message::ITUT::ISUP::RLC;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
package Message::ANSI::ISUP::RLC;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::CCR;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::CCR;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::RSC;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::RSC;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::BLO;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::BLO;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::UBL;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::UBL;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::BLA;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::BLA;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::UBA;  our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::UBA;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ISUP::GCM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: V(RS)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'RS',$off); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'RS',$off),
	);
}
package Message::ITUT::ISUP::GRS; our @ISA = qw(Message::ISUP::GCM Message::ITUT::ISUP);
package Message::ANSI::ISUP::GRS; our @ISA = qw(Message::ISUP::GCM Message::ANSI::ISUP);
package Message::ITUT::ISUP::CQM; our @ISA = qw(Message::ISUP::GCM Message::ITUT::ISUP);
package Message::ANSI::ISUP::CQM; our @ISA = qw(Message::ISUP::GCM Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::CGM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(CGI) V(RS)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'CGI',$off,1); $off++;
	return unless Message::unpack_MV($self,'RS'  $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'CGI',$off+0,1),
		Message::dissect_MV($self,'RS'  $off+1  ),
	);
}
package Message::ITUT::ISUP::CGB;  our @ISA = qw(Message::ISUP::CGM Message::ITUT::ISUP);
package Message::ANSI::ISUP::CGB;  our @ISA = qw(Message::ISUP::CGM Message::ANSI::ISUP);
package Message::ITUT::ISUP::CGU;  our @ISA = qw(Message::ISUP::CGM Message::ITUT::ISUP);
package Message::ANSI::ISUP::CGU;  our @ISA = qw(Message::ISUP::CGM Message::ANSI::ISUP);
package Message::ITUT::ISUP::CGBA; our @ISA = qw(Message::ISUP::CGM Message::ITUT::ISUP);
package Message::ANSI::ISUP::CGBA; our @ISA = qw(Message::ISUP::CGM Message::ANSI::ISUP);
package Message::ITUT::ISUP::CGUA; our @ISA = qw(Message::ISUP::CGM Message::ITUT::ISUP);
package Message::ANSI::ISUP::CGUA; our @ISA = qw(Message::ISUP::CGM Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::FAM; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(FACI) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'FACI',$off,1); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::unpack_MF($self,'FACI',$off+0,1),
		Message::unpack_OP($self,       $off+1  ),
	};
}
package Message::ITUT::ISUP::FAR; our @ISA = qw(Message::ISUP::FAM Message::ITUT::ISUP);
package Message::ANSI::ISUP::FAR; our @ISA = qw(Message::ISUP::FAM Message::ANSI::ISUP);
package Message::ITUT::ISUP::FAA; our @ISA = qw(Message::ISUP::FAM Message::ITUT::ISUP);
package Message::ANSI::ISUP::FAA; our @ISA = qw(Message::ISUP::FAM Message::ANSI::ISUP);
# -------------------------------------
package Message::ITUT::ISUP::FRJ;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: F(FACI) V(CAUS) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'FACI',$off,1); $off++;
	return unless Message::unpack_MV($self,'CAUS',$off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'FACI',$off+0,1),
		Message::dissect_MV($self,'CAUS',$off+1  ),
	);
}
# -------------------------------------
package Message::ITUT::ISUP::LPA;  our @ISA = qw(Message::ITUT::ISUP::NONE);
# -------------------------------------
package Message::ANSI::ISUP::LPA;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::PAM;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: (encapsultated ISUP message)
# -------------------------------------
sub unpack {
	my $self = shift;

	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off} + 3;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
# -------------------------------------
package Message::ANSI::ISUP::PAM;  our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
#  ANSI: (encapsultated ISUP message)
# -------------------------------------
sub unpack {
	my $self = shift;

	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off} + 3;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
# -------------------------------------
package Message::ISUP::GRA; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: V(RS)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'RS',$off); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'RS',$off),
	);
}
package Message::ITUT::ISUP::GRA; our @ISA = qw(Message::ISUP::GRA Message::ITUT::ISUP);
package Message::ANSI::ISUP::GRA; our @ISA = qw(Message::ISUP::GRA Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::CQR; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: V(RS) V(CSI)
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'RS', $off  ); $off++;
	return unless Message::unpack_MV($self,'CSI',$off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'RS', $off+0),
		Message::dissect_MV($self,'CSI',$off+1),
	);
}
# -------------------------------------
package Message::ITUT::ISUP::CQR; our @ISA = qw(Message::ISUP::CQR Message::ITUT::ISUP);
package Message::ANSI::ISUP::CQR; our @ISA = qw(Message::ISUP::CQR Message::ANSI::ISUP);
# -------------------------------------
package Message::ISUP::CPG;  our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: F(EVNT) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'EVNT',$off,1); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'EVNT',$off+0,1),
		Message::dissect_OP($self,       $off+1  ),
	);
}
package Message::ITUT::ISUP::CPG; our @ISA = qw(Message::ISUP::CPG Message::ITUT::ISUP);
package Message::ANSI::ISUP::CPG; our @ISA = qw(Message::ISUP::CPG Message::ANSI::ISUP);
# -------------------------------------
package Message::ITUT::ISUP::USR;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: V(UUI) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'UUI',$off); $off++;
	return unless Message::unpack_OP($self,      $off); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'UUI',$off+0),
		Message::dissect_OP($self,      $off+1),
	);
}
# -------------------------------------
package Message::ITUT::ISUP::UCIC; our @ISA = qw(Message::ITUT::ISUP::NONE);
package Message::ANSI::ISUP::UCIC; our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ISUP::CFN; our @ISA = qw(Message::ISUP);
# -------------------------------------
#  BOTH: V(CAUS) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MV($self,'CAUS',$off); $off++;
	return unless Message::unpack_OP($self,       $off); $off++;
	return $self;
}
sub dissect{
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MV($self,'CAUSE',$off+0),
		Message::dissect_OP($self,        $off+1),
	);
}
package Message::ITUT::ISUP::CFN; our @ISA = qw(Message::ITUT::ISUP);
package Message::ANSI::ISUP::CFN; our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
package Message::ITUT::ISUP::OLM;  our @ISA = qw(Message::ITUT::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::CRG;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: (none)
#  SING: ICCI O()
#  SPAIN: O()
# -------------------------------------
sub unpack {
	my $self = shift;

	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off} + 3;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
# -------------------------------------
package Message::ITUT::ISUP::NRM;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::FAC;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
package Message::ANSI::ISUP::FAC;  our @ISA = qw(Message::ANSI::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::UPT;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::UPA;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::IDR;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::IRS;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::SGM;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
package Message::ANSI::ISUP::SGM;  our @ISA = qw(Message::ANSI::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::LPR;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::ATP;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::PRI;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::SDN;  our @ISA = qw(Message::ITUT::ISUP::OPARMS);
# -------------------------------------
package Message::ANSI::ISUP::CRA;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ANSI::ISUP::CRM;  our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
#  ANSI: NCI
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'NCI',$off,1); $off++;
	return unless $off == $self->{end};
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'NCI',$off,1),
	);
}
# -------------------------------------
package Message::ANSI::ISUP::CVR;  our @ISA = qw(Message::ANSI::ISUP);
# -------------------------------------
#  ANSI: F(CVRI CGCI) O()
# -------------------------------------
sub unpack {
	my $self = shift;
	my $off = $self->{off};
	return unless Message::unpack_MF($self,'CVRI',$off,1); $off++;
	return unless Message::unpack_MF($self,'CGCI',$off,1); $off++;
	return unless Message::unpack_OP($self,       $off  ); $off++;
	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{off};
	return $self->SUPER::dissect(
		Message::dissect_MF($self,'CVRI',$off+0,1),
		Message::dissect_MF($self,'CGCI',$off+1,1),
		Message::dissect_OP($self,       $off+2  ),
	);
}
# -------------------------------------
package Message::ANSI::ISUP::CVT;  our @ISA = qw(Message::ANSI::ISUP::NONE);
# -------------------------------------
package Message::ANSI::ISUP::EXM;  our @ISA = qw(Message::ANSI::ISUP::OPARMS);
# -------------------------------------
package Message::ITUT::ISUP::NON;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: (no such message)
#  SPAN: TON
# -------------------------------------
sub unpack {
	my $self = shift;

	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off} + 3;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
# -------------------------------------
package Message::ITUT::ISUP::LLM;  our @ISA = qw(Message::ITUT::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::CAK;  our @ISA = qw(Message::ITUT::ISUP::NONE);
# -------------------------------------
package Message::ITUT::ISUP::TCM;  our @ISA = qw(Message::ITUT::ISUP);
# -------------------------------------
#  ITUT: (no such message)
#  SING: CRI
# -------------------------------------
sub unpack {
	my $self = shift;

	return $self;
}
sub dissect {
	my $self = shift;
	my $off = $self->{isup}{off} + 3;
	return [
		@{$self->SUPER::dissect(@_)},
	];
}
# -------------------------------------
package Message::ITUT::ISUP::MCP;  our @ISA = qw(Message::ITUT::ISUP::NONE);
# -------------------------------------



# -------------------------------------
package Message::TCAP; our @ISA = qw(Message);
# -------------------------------------

use constant {
	TCAP_TAG_UNIV_INT	=>  2,	# UNIV Integer
	TCAP_TAG_UNIV_OSTR	=>  4,	# UNIV Octet String
	TCAP_TAG_UNIV_OID	=>  6,	# UNIV Object Id
	TCAP_TAG_UNIV_PSEQ	=> 16,	# UNIV Parameter Sequence
	TCAP_TAG_UNIV_UTC	=> 17,	# UNIV Universal Time
	TCAP_TAG_UNIV_TS	=> 23,	# UNIV Timestamp
	TCAP_TAG_UNIV_SEQ	=> 48,	# UNIV Sequence

	# TCAP (application-context/national) parameter type tag values
	TCAP_TAG_TCAP_ACG	=>  1,	# TCAP ACG Indicators
	TCAP_TAG_TCAP_STA	=>  2,	# TCAP Standard Announcement
	TCAP_TAG_TCAP_CUA	=>  3,	# TCAP Customized Announcment
	TCAP_TAG_TCAP_TDIG	=>  4,	# TCAP Digits
	TCAP_TAG_TCAP_SUEC	=>  5,	# TCAP Standard User Error Code
	TCAP_TAG_TCAP_PDTA	=>  6,	# TCAP Problem Data
	TCAP_TAG_TCAP_TCGPA	=>  7,	# TCAP SCCP Calling Party Address
	TCAP_TAG_TCAP_TRSID	=>  8,	# TCAP Transaction ID
	TCAP_TAG_TCAP_PCTY	=>  9,	# TCAP Package Type
	TCAP_TAG_TCAP_SKEY	=> 10,	# TCAP Service Key (Constructor)
	TCAP_TAG_TCAP_BISTAT	=> 11,	# TCAP Busy/Idle Status
	TCAP_TAG_TCAP_CFSTAT	=> 12,	# TCAP Call Forwarding Status
	TCAP_TAG_TCAP_ORIGR	=> 13,	# TCAP Origination Restrictions
	TCAP_TAG_TCAP_TERMR	=> 14,	# TCAP Terminating Restrictions
	TCAP_TAG_TCAP_DNMAP	=> 15,	# TCAP DN to Line Service TYpe Mapping
	TCAP_TAG_TCAP_DURTN	=> 16,	# TCAP Duration
	TCAP_TAG_TCAP_RETD	=> 17,	# TCAP Return Data (Constructor)
	TCAP_TAG_TCAP_BCRQ	=> 18,	# TCAP Bearer Capability Requested
	TCAP_TAG_TCAP_BCSUP	=> 19,	# TCAP Bearer Capability Supported
	TCAP_TAG_TCAP_REFID	=> 20,	# TCAP Reference Id
	TCAP_TAG_TCAP_BGROUP	=> 21,	# TCAP Business Group
	TCAP_TAG_TCAP_SNI	=> 22,	# TCAP Signalling Networks Identifier
	TCAP_TAG_TCAP_GNAME	=> 23,	# TCAP Generic Name
	TCAP_TAG_TCAP_MWIT	=> 24,	# TCAP Message Waiting Indicator Type
	TCAP_TAG_TCAP_LAFB	=> 25,	# TCAP Locall Ahead for Busy
	TCAP_TAG_TCAP_CIC	=> 26,	# TCAP Circuit Identification Code
	TCAP_TAG_TCAP_PRECI	=> 27,	# TCAP Precedence Identifier
	TCAP_TAG_TCAP_CRI	=> 28,	# TCAP Call Reference Identifier
	TCAP_TAG_TCAP_AUTH	=> 29,	# TCAP Authorization
	TCAP_TAG_TCAP_INTEG	=> 30,	# TCAP Integrity
	TCAP_TAG_TCAP_SEQN	=> 31,	# TCAP Sequence Number
	TCAP_TAG_TCAP_NMSGS	=> 32,	# TCAP Number of Messages
	TCAP_TAG_TCAP_DTEXT	=> 33,	# TCAP Display Text
	TCAP_TAG_TCAP_KEYX	=> 34,	# TCAP Key Exchange
	TCAP_TAG_TCAP_TCDPA	=> 35,	# TCAP SCCP Called Party Address

	#TCAP (private/private) parameter type tags
	TCAP_TAG_PRIV_BLGI	=>  1,	# TCAP Billing indicators
	TCAP_TAG_PRIV_CTIME	=>  2,	# TCAP Connect time
	TCAP_TAG_PRIV_ECHOD	=>  3,	# TCAP Echo data
	TCAP_TAG_PRIV_ONC	=>  4,	# TCAP Originating node capabilities
	TCAP_TAG_PRIV_OST	=>  5,	# TCAP Originating station type
	TCAP_TAG_PRIV_TERMIND	=>  6,	# TCAP Terminating indicators
	TCAP_TAG_PRIV_AGCP	=>  7,	# TCAP AGC indicators (private)
	TCAP_TAG_PRIV_ALPHAS	=>  8,	# TCAP Alphanumeric string
	TCAP_TAG_PRIV_PDIG	=>  9,	# TCAP Digits (private)
	TCAP_TAG_PRIV_ASYNCTP	=> 10,	# TCAP Asynchronous terminal profile
	TCAP_TAG_PRIV_ACBI	=> 11,	# TCAP Automated callback indicator
	TCAP_TAG_PRIV_CSDI	=> 12,	# TCAP CC account number service denial indicator
	TCAP_TAG_PRIV_CCSAN	=> 13,	# TCAP Calling card subaccount number
	TCAP_TAG_PRIV_CHGII	=> 14,	# TCAP Charging information indicator
	TCAP_TAG_PRIV_CUGFC	=> 15,	# TCAP CUG facility code
	TCAP_TAG_PRIV_CUGI	=> 16,	# TCAP CUG index
	TCAP_TAG_PRIV_CUGP	=> 17,	# TCAP CUG parameters
	TCAP_TAG_PRIV_COLL	=> 18,	# TCAP Collect acceptance indicator
	TCAP_TAG_PRIV_COMPID	=> 19,	# TCAP Company ID
	TCAP_TAG_PRIV_DTPCL	=> 20,	# TCAP Default throughput class
	TCAP_TAG_PRIV_FCNFI	=> 21,	# TCAP Flow control negotiation facility indicator
	TCAP_TAG_PRIV_ICDR	=> 22,	# TCAP IXC denial check response
	TCAP_TAG_PRIV_CARI	=> 23,	# TCAP IXC (carrier) indicators
	TCAP_TAG_PRIV_ICPRE	=> 24,	# TCAP IXC (carrier) preselect
	TCAP_TAG_PRIV_ICPREI	=> 25,	# TCAP IXC (carrier) preselect indicator
	TCAP_TAG_PRIV_ICPT	=> 26,	# TCAP Intercept indicator
	TCAP_TAG_PRIV_NSDPS	=> 27,	# TCAP Non-standard default packet size
	TCAP_TAG_PRIV_NSDWS	=> 28,	# TCAP Non-standard default window size
	TCAP_TAG_PRIV_MATCH	=> 29,	# TCAP Match
	TCAP_TAG_PRIV_OBILL	=> 30,	# TCAP Originating billing indicators
	TCAP_TAG_PRIV_CCVI2	=> 31,	# TCAP Calling card verification information 2
	TCAP_TAG_PRIV_PIN	=> 32,	# TCAP Personal identification number
	TCAP_TAG_PRIV_PINR	=> 33,	# TCAP PIN restriction indicator
	TCAP_TAG_PRIV_PSDI	=> 34,	# TCAP PIN service denial indicator
	TCAP_TAG_PRIV_PPSN	=> 35,	# TCAP PPSN data services indicator
	#
	#
	TCAP_TAG_PRIV_PSNP	=> 38,	# TcAP Private subscribe network preselect
	TCAP_TAG_PRIV_RSI	=> 39,	# TCAP Record status indicator
	TCAP_TAG_PRIV_SST	=> 40,	# TCAP service or equipment indicator (service station type)
	TCAP_TAG_PRIV_TERMLI	=> 41,	# TCAP Terminating line indicators
	TCAP_TAG_PRIV_TN	=> 42,	# TCAP Third number acceptance indicator
	TCAP_TAG_PRIV_TCNFI	=> 43,	# TCAP Throughput class negotiation
	TCAP_TAG_PRIV_TREAT	=> 44,	# TCAP Treatment indicator
	TCAP_TAG_PRIV_RPOASB	=> 45,	# TCAP RPOA selection barred facility indicator
	TCAP_TAG_PRIV_INTINF	=> 46,	# TCAP Intercept information
	TCAP_TAG_PRIV_OLNSI	=> 47,	# TCAP Originating line number screening information
	TCAP_TAG_PRIV_CCVI1	=> 48,	# TCAP Calling card verification information 1 (Automated calling card service)
	TCAP_TAG_PRIV_BNS	=> 49,	# TCAP Billed number screening information
	TCAP_TAG_PRIV_TLNSI	=> 50,	# TCAP Terminating line number screening information
	TCAP_TAG_PRIV_NUI	=> 51,	# TCAP NUI profile screening information
	TCAP_TAG_PRIV_SPAR1	=> 52,	# TCAP Spare indicator 1
	TCAP_TAG_PRIV_SPAR2	=> 53,	# TCAP Spare indicator 2
	TCAP_TAG_PRIV_TSPAR1	=> 54,	# TCAP Telco spare indicator 1
	TCAP_TAG_PRIV_TSPAR2	=> 55,	# TCAP Telco spare indicator 2
	TCAP_TAG_PRIV_TSPAR3	=> 56,	# TCAP Telco spare indicator 3
	TCAP_TAG_PRIV_TSPAR4	=> 57,	# TCAP Telco spare indicator 4
	TCAP_TAG_PRIV_OLSI1	=> 58,	# TCAP Originating listing services indicator 1
	TCAP_TAG_PRIV_OLSI2	=> 59,	# TCAP Originating listing services indicator 2
	TCAP_TAG_PRIV_LOCAL	=> 60,	# TCAP Locality
	TCAP_TAG_PRIV_SPECA	=> 61,	# TCAP Special announcement

	TCAP_TAG_PRIV_OST_VALUES=>{
		0x00=>'identified line, no special treatment',
		0x01=>'ONI (multiparty)',
		0x02=>'ANI failure',
		0x06=>'hotel with room ID',
		0x07=>'coinless, hospital, etc',
		0x08=>'interLATA restricted',
		0x0a=>'test call (1)',
		0x14=>'AIOD (automatic inward/outward dialing) listed DN sent',
		0x17=>'coin or non-coin identified line',
		0x18=>'800 call',
		0x44=>'interLATA restricted-hotel',
		0x4e=>'interLATA restricted-coinless, etc',
		0x5f=>'test call (2)',
	},
	TCAP_TAG_PRIV_SPECA_VALUES=>{
		0x00=>'not used',
		0x01=>'change 800 number (T1)',
		0x02=>'change 800 number (T2)',
		0x03=>'security failure, invalid SO',
	},

	#TCAP (private/national) parameter type tags
	TCAP_TAG_PRIV_UNI	=>  1,	# ANSI Unidirectional
	TCAP_TAG_PRIV_QWP	=>  2,	# ANSI Query w/ permission
	TCAP_TAG_PRIV_QWOP	=>  3,	# ANSI Query w/o permission
	TCAP_TAG_PRIV_RESP	=>  4,	# ANSI Response
	TCAP_TAG_PRIV_CWP	=>  5,	# ANSI Conversaion w/ permission
	TCAP_TAG_PRIV_CWOP	=>  6,	# ANSI Conversaion w/o permission
	TCAP_TAG_PRIV_TRSID	=>  7,	# ANSI Transaction Id
	TCAP_TAG_PRIV_CSEQ	=>  8,	# ANSI Component Sequence
	TCAP_TAG_PRIV_INKL	=>  9,	# ANSI Invoke (Last)
	TCAP_TAG_PRIV_RRL	=> 10,	# ANSI Return Result (Last)
	TCAP_TAG_PRIV_RER	=> 11,	# ANSI Return Error
	TCAP_TAG_PRIV_REJ	=> 12,	# ANSI Reject
	TCAP_TAG_PRIV_INK	=> 13,	# ANSI Invoke (Not Last)
	TCAP_TAG_PRIV_RR	=> 14,	# ANSI Result (Not Last)
	TCAP_TAG_PRIV_CORID	=> 15,	# ANSI Correlation Id(s)
	TCAP_TAG_PRIV_NOPCO	=> 16,	# ANSI National Operation Code
	TCAP_TAG_PRIV_POPCO	=> 17,	# ANSI Private Operation Code
	TCAP_TAG_PRIV_PSET	=> 18,	# ANSI Parameter Set
	TCAP_TAG_PRIV_NECODE	=> 19,	# ANSI National Error Code
	TCAP_TAG_PRIV_PECODE	=> 20,	# ANSI Private Error Code
	TCAP_TAG_PRIV_PBCODE	=> 21,	# ANSI Reject Problem Code
	TCAP_TAG_PRIV_PSEQ	=> 21,	# ANSI Parameter Sequence
	TCAP_TAG_PRIV_ABORT	=> 22,	# ANSI Abort
	TCAP_TAG_PRIV_PCAUSE	=> 23,	# ANSI P-Abort Cause
	TCAP_TAG_PRIV_U_ABORT	=> 24,	# ANSI User Abort Information
	TCAP_TAG_PRIV_DLGP	=> 25,	# ANSI Dialog Portion
	TCAP_TAG_PRIV_VERSION	=> 26,	# ANSI Protocol Version
	TCAP_TAG_PRIV_CONTEXT	=> 27,	# ANSI Integer Application Context
	TCAP_TAG_PRIV_CTX_OID	=> 28,	# ANSI OID Application Context
	TCAP_TAG_PRIV_UINFO	=> 29,	# ANSI User Information

	TCAP_TAG_APPL_UNI	=>  1,	# ITUT Unidirectional
	TCAP_TAG_APPL_BEGIN	=>  2,	# ITUT Begin Transaction
	TCAP_TAG_APPL_END	=>  4,	# ITUT End Transaction
	TCAP_TAG_APPL_CONT	=>  5,	# ITUT Continue Transaction
	TCAP_TAG_APPL_ABORT	=>  7,	# ITUT Abort Transaction
	TCAP_TAG_APPL_ORIGID	=>  8,	# ITUT Origination Transaction Id
	TCAP_TAG_APPL_DESTID	=>  9,	# ITUT Destination Transaction Id
	TCAP_TAG_APPL_PCAUSE	=> 10,	# ITUT P-Abort Cause
	TCAP_TAG_APPL_DLGP	=> 11,	# ITUT Dialog Portion
	TCAP_TAG_APPL_CSEQ	=> 12,	# ITUT Component Portion

	TCAP_TAG_APPL_AUDT_PDU	=>  0,	# ITUT AUDT APDU
	TCAP_TAG_APPL_AARQ_PDU	=>  0,	# ITUT AARQ APDU
	TCAP_TAG_APPL_AARE_PDU	=>  1,	# ITUT AARE APDU
	TCAP_TAG_APPL_RLRQ_PDU	=>  2,	# ITUT RLRQ APDU
	TCAP_TAG_APPL_RLRE_PDU	=>  3,	# ITUT RLRE APDU
	TCAP_TAG_APPL_ABRT_PDU	=>  4,	# ITUT ABRT APDU

	TCAP_TAG_CNTX_LID	=>  0,	# Linked Id
	TCAP_TAG_CNTX_INK	=>  1,	# Invoke
	TCAP_TAG_CNTX_RRL	=>  2,	# Return Result (Last)
	TCAP_TAG_CNTX_RER	=>  3,	# Return Error
	TCAP_TAG_CNTX_REJ	=>  4,	# Reject
	TCAP_TAG_CNTX_RR	=>  7,	# Return Result (Not Last)

	TCAP_NAT_OP_FAMILY=>{
		0x00=>'All Families',
		0x01=>'Parameter',
		0x02=>'Charging',
		0x03=>'Provide Instructions',
		0x04=>'Connection Control',
		0x05=>'Caller Interaction',
		0x06=>'Send Notification',
		0x07=>'Network Management',
		0x08=>'Procedural',
		0x09=>'Operation Control',
		0x0a=>'Report Event',
		0x7e=>'Miscellaneous',
	},
	# Note: the high bit indicates whether a reply is required (1) or not (0)
	TCAP_NAT_OPERATIONS=>{
		0x00ff=>'Reserved',
		0x0000=>'Spare',
		0x0101=>'Provide Value',
		0x0102=>'Set Value',
		0x0201=>'Bill Call',
		0x0301=>'Start',
		0x0302=>'Assist',
		0x0401=>'Connect',
		0x0402=>'Temporary Connect',
		0x0403=>'Disconnect',
		0x0404=>'Forward Disconnect',
		0x0501=>'Play Announcement',
		0x0502=>'Play Announcement and Collect Digits',
		0x0503=>'Indicate Information Waiting',
		0x0504=>'Indicate Information Provided',
		0x0601=>'When Party Free',
		0x0701=>'Automatic Code Gap',
		0x0801=>'Temporary Handover',
		0x0802=>'Report Assist Termination',
		0x0803=>'Security',
		0x0901=>'Cancel',
		0x0a01=>'Voice Message Available',
		0x0a02=>'Voice Message Retrieved',
		0x7e01=>'Queue Call',
		0x7e02=>'Dequeue Call',
	},
	TCAP_NAT_ERRORS=>{
		0x00=>'not used',
		0x01=>'unexpected component sequence',
		0x02=>'unexpected data value',
		0x03=>'unavailable resource',
		0x04=>'missing customer record',
		0x05=>'spare',
		0x06=>'data unavailable',
		0x07=>'task refused',
		0x08=>'queue full',
		0x09=>'no queue',
		0x0a=>'timer expired',
		0x0b=>'data already exists',
		0x0c=>'unauthorized request',
		0x0d=>'not queued',
		0x0e=>'unassigned directory number',
		0x0f=>'spare',
		0x10=>'notification unavailable for destination directory number',
		0x11=>'VMSR system identification did not match user profile',
		0x12=>'security error',
		0x13=>'missing parameter',
		0x14=>'unexpected parameter sequence',
		0x15=>'unexpected message',
		0x16=>'unexpected package type',
	},
	TCAP_ACG_CTRL_CAUSE=>{
		0x01=>'vacant code',
		0x02=>'out-of-band',
		0x03=>'database overload',
		0x04=>'destination mass calling',
		0x05=>'operation support system initiated',
	},
	TCAP_ACG_DURATION=>{
		0x00=>'not used',
		0x01=>'1 second',
		0x02=>'2 seconds',
		0x03=>'4 seconds',
		0x04=>'8 seconds',
		0x05=>'16 seconds',
		0x06=>'32 seconds',
		0x07=>'64 seconds',
		0x08=>'128 seconds',
		0x09=>'256 seconds',
		0x0a=>'512 seconds',
		0x0b=>'1024 seconds',
		0x0c=>'2048 seconds',
	},
	TCAP_ACG_GAP=>{
		0x00=>'remove gap control',
		0x01=>'0.00 seconds',
		0x02=>'0.10 seconds',
		0x03=>'0.25 seconds',
		0x04=>'0.50 seconds',
		0x05=>'1.00 seconds',
		0x06=>'2.00 seconds',
		0x07=>'5.00 seconds',
		0x08=>'10.00 seconds',
		0x09=>'15.00 seconds',
		0x0a=>'30.00 seconds',
		0x0b=>'60.00 seconds',
		0x0c=>'120.00 seconds',
		0x0d=>'300.00 seconds',
		0x0e=>'600.00 seconds',
		0x0f=>'stop all calls',
	},
	TCAP_STA_STAI=>{
		0x00=>'not used',
		0x01=>'out-of-band',
		0x02=>'vacant code',
		0x03=>'disconnected number',
		0x04=>'reorder (120 ipm)',
		0x05=>'busy (60 ipm)',
		0x06=>'no circuit available',
		0x07=>'reorder',
		0x08=>'audible ring',
	},
	TCAP_DIG_TOD=>{
		0x00=>'not used',
		0x01=>'called party number',
		0x02=>'calling party number',
		0x03=>'caller interaction',
	},
};
use constant {
	TCAP_TYPE=>{
		0xa0 => {
			0x01	=> 'APPL::UNI',
			0x02	=> 'APPL::BEGIN',
			0x04	=> 'APPL::END',
			0x05	=> 'APPL::CONT',
			0x07	=> 'APPL::ABORT',
		},
		0xe0 => {
			0x01	=> 'PRIV::UNI',
			0x02	=> 'PRIV::QWP',
			0x03	=> 'PRIV::QWOP',
			0x04	=> 'PRIV::RESP',
			0x05	=> 'PRIV::CWP',
			0x06	=> 'PRIV::CWOP',
			0x16	=> 'PRIV::ABORT',
		},
	},
};
sub unpack {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls;
	return unless exists TCAP_TYPE->{$cls};
	return unless exists TCAP_TYPE->{$cls}{$tag};
	my $type = TCAP_TYPE->{$cls}{$tag};
	$self->{off} = $ptr;
	$self->{end} = $end;
	$self->{cls} = $cls;
	$self->{tag} = $tag;
	bless $self,ref($self)."::$type";
	return $self->unpack(@_);
}

sub unpack_taglen {
	my ($self,$ptr,$end,$cls,$tag,$len) = @_;
	return unless $ptr < $end;
	$cls = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
	if (($cls & 0x1f) != 0x1f) { # tag is not extended
		$tag = $cls & 0x1f;
	} else {
		my $ptag;
		$tag = 0;
		return unless $ptr < $end;
		$ptag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		$tag |= $ptag & 0x7f;
		if ($ptag & 0x80) {
			return unless $ptr < $end;
			$ptag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
			$tag <<= 7;
			$tag |= $ptag & 0x7f;
			if ($ptag & 0x80) {
				return unless $ptr < $end;
				$ptag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
				$tag <<= 7;
				$tag |= $ptag & 0x7f;
				if ($ptag & 0x80) {
					return unless $ptr < $end;
					$ptag = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
					$tag <<= 7;
					$tag |= $ptag & 0x7f;
					return if $ptag & 0x80; 
				}
			}


		}

	}
	return unless $ptr < $end;
	$len = unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
	if (($len & 0x80) != 0x00) { # extended length
		my $plen;
		$plen = $len & 0x7f;
		return if $plen > 4 or $plen == 0;
		$len = 0;
		while ($plen--) {
			return unless $ptr < $end;
			$len <<= 8;
			$len |= unpack('C',substr($$self->{buf},$ptr,1)); $ptr++;
		}
		return unless $ptr + $len <= $end;
		$end = $ptr + $len;
	}
	$cls &= 0x1f;
	return $cls,$tag,$ptr,$end;
}

# -------------------------------------
package Message::TCAP::APPL; our @ISA = qw(Message::TCAP);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{dialportion} = new Message::TCAP::APPL::DLGP($self);
	$self->{compportion} = new Message::TCAP::APPL::CSEQ($self);
	return $self;
}

# -------------------------------------
package Message::TCAP::APPL::DLGP; our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls and $cls & 0xe0 == 0x60 and $tag == TCAP_TAG_APPL_DLGP;
	$self->{cls} = $cls;
	$self->{tag} = $tag;
	$self->{off} = $ptr;
	$self->{end} = $end;
	return $self;
}
# -------------------------------------
package Message::TCAP::APPL::CSEQ; our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls and $cls & 0xe0 == 0x60 and $tag == TCAP_TAG_APPL_CSEQ;
	$self->{cls} = $cls;
	$self->{tag} = $tag;
	$self->{off} = $ptr;
	$self->{end} = $end;
	return $self;
}

# -------------------------------------
package Message::TCAP::APPL::UNI;   our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::APPL::BEGIN; our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::APPL::END;   our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::APPL::CONT;  our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::APPL::ABORT; our @ISA = qw(Message::TCAP::APPL);
# -------------------------------------
sub unpack {
	my $self = shift;
	$self->{dialportion} = new Message::TCAP::APPL::DLGP($self);
	# FIXME: needs to be cause info
#	$self->{compportion} = new Message::TCAP::APPL::CSEQ($self);
	return $self;
}


# -------------------------------------
package Message::TCAP::PRIV::DLGP; our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls and $cls & 0xe0 == 0xe0 and $tag == TCAP_TAG_PRIV_DLGP;
	$self->{cls} = $cls;
	$self->{tag} = $tag;
	$self->{off} = $ptr;
	$self->{end} = $end;
	return $self;
}
# -------------------------------------
package Message::TCAP::PRIV::CSEQ; our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls and $cls & 0xe0 == 0xe0 and $tag == TCAP_TAG_PRIV_CSEQ;
	$self->{cls} = $cls;
	$self->{tag} = $tag;
	$self->{off} = $ptr;
	$self->{end} = $end;
	return $self;
}

# -------------------------------------
package Message::TCAP::PRIV; our @ISA = qw(Message::TCAP);
# -------------------------------------
sub unpack_prim {
	my $self = shift;
	my ($cls,$tag,$ptr,$end) = $self->unpack_taglen($self->{off},$self->{end});
	return unless defined $cls;
	my $val = substr($$self->{buf},$ptr,$end-$ptr);
	return $cls,$tag,$end,$val;
}
sub unpack_TRSID {
	my $self = shift;
	my ($cls,$tag,$end,$val) = $self->unpack_prim($self->{off},$self->{end});
	return unless defined $cls and $cls & 0xe0 == 0xc0 and $tag == TCAP_TAG_PRIV_TRSID;
	$self->{off} = $end;
	return $val;
}
sub unpack {
	my $self = shift;
	$self->{dialportion} = new Message::TCAP::PRIV::DLGP($self);
	$self->{compportion} = new Message::TCAP::PRIV::CSEQ($self);
	return $self;
}
# -------------------------------------
package Message::TCAP::PRIV::UNI;   our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 0;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::QWP;   our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 4;
	$self->{origid} = unpack('N',$trnsid);
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::QWOP;  our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 4;
	$self->{origid} = unpack('N',$trnsid);
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::RESP;  our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 4;
	$self->{origid} = unpack('N',$trnsid);
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::CWP;   our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 8;
	($self->{origid},$self->{termid}) = unpack('NN',$trnsid);
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::CWOP;  our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	($self->{origid},$self->{termid}) = unpack('NN',$trnsid);
	return unless defined $trsid and length $trsid == 8;
	return $self->SUPER::unpack(@_);
}
# -------------------------------------
package Message::TCAP::PRIV::ABORT; our @ISA = qw(Message::TCAP::PRIV);
# -------------------------------------
sub unpack {
	my $self = shift;
	my $trsid = $self->unpack_TRSID($self->{off},$self->{end});
	return unless defined $trsid and length $trsid == 4;
	$self->{origid} = unpack('N',$trnsid);
	$self->{dialportion} = new Message::TCAP::PRIV::DLGP($self);
	# FIXME: needs to be cause info
#	$self->{compportion} = new Message::TCAP::PRIV::CSEQ($self);
	return $self;
}

