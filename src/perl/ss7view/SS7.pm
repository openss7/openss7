use constant {
	HALF_LEFT	=>-1,
	HALF_RIGHT	=> 1,

	COL_NOD => 5,
	COL_SSP => 4,
	COL_SCP => 3,
	COL_GTT => 2,
	COL_ADJ => 1,

	CTS_UNINIT	=> 0,
	CTS_IDLE	=> 1,
	CTS_WAIT_ACM	=> 2,
	CTS_WAIT_ANM	=> 3,
	CTS_ANSWERED	=> 4,
	CTS_SUSPENDED	=> 5,
	CTS_WAIT_RLC	=> 6,
	CTS_SEND_RLC	=> 7,
	CTS_COMPLETE	=> 8,

	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4,	# also RL length in octets
	RT_24BIT_PC => 7,	# also RL length in octets
	RT_NONE => -1,

	DX_UNKNOWN => 0,
	DX_HALF_DUPLEX => 1,
	DX_FULL_DUPLEX => 2,

	HT_UNKNOWN => 0,
	HT_BASIC => 3,		# also link level header length
	HT_EXTENDED => 6,	# also link level header length
	HT_NONE => -1,

	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3,
	MP_NONE => -1,

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
package UNIVERSAL;
# -------------------------------------

sub _init {
	my $self = shift;
	my $type = shift;
	if (exists *{"$type\::"}->{only}) {
		my $sub = "$type\::only";
		$self->$sub(@_);
	} else {
		if (exists *{"$type\::"}->{ISA}) {
			foreach $clas (@{*{"$type\::"}->{ISA}}) {
				$self->_init($clas,@_);
			}
		}
		if (exists *{"$type\::"}->{init}) {
			my $sub = "$type\::init";
			$self->$sub(@_);
		}
	}
}

sub _new {
	my $clas = shift;
	my $type = shift;
	my @isas = ();
	@isas = @{*{"$clas\::"}->{ISA}} if exists *{"$clas\::"}->{ISA};
	my $self;
	if (my $isa = shift @isas) {
		$self = $isa->_make($type,@_);
	} else {
		$self = {};
		bless $self,$type;
	}
	return $self;
}

sub _make {
	my $clas = shift;
	my $type = shift;
	my $self;
	if (exists *{"$clas\::"}->{make}) {
		$self = &{*{"$clas\::"}->{make}}($type,@_);
	} else {
		$self = $clas->_new($type,@_);
	}
	return $self;
}

sub new {
	my $type = shift;
	my $self = $type->_make($type,@_);
	$self->_init($type,@_);
	return $self;
}

sub _fini {
	my $self = shift;
	my $type = shift;
	if (exists *{"$type\::"}->{cull}) {
		my $sub = "$type\::cull";
		$self->$sub(@_);
	} else {
		if (exists *{"$type\::"}->{fini}) {
			my $sub = "$type\::fini";
			$self->$sub(@_);
		}
		if (exists *{"$type\::"}->{ISA}) {
			foreach $clas (reverse @{*{"$type\::"}->{ISA}}) {
				$self->_fini($clas,@_);
			}
		}
	}
}

sub put {
	my $self = shift;
	my $type = ref $self;
	$self->_fini($type,@_);
}

sub get {
	my $type = shift;
	my $self;
	if ($type->can('find')) {
		$self = $type->find(@_);
	}
	return $self if $self;
	$self = $type->new(@_);
	return $self;
}

# -------------------------------------
package Root;
# -------------------------------------
#package Root;
#package Root;
sub init {
	my $self = shift;
	$self->{children} = {} unless exists $self->{children};
}
#package Root;
sub fini {
	my $self = shift;
	return unless exists $self->{children};
	foreach my $form (values %{$self->{children}}) {
		foreach my $child (values %{$form}) {
			$child->put(@_);
		}
	}
	delete $self->{children};
}
#package Root;
sub child {
	my ($self,$form,$key) = @_;
	return undef unless exists $self->{children}->{$form}->{$key};
	return $self->{children}->{$form}->{$key};
}
#package Root;
sub children {
	my ($self,$form) = @_;
	return values %{$self->{children}->{$form}};
}
#package Root;
sub count {
	my ($self,$form) = @_;
	return 0 unless exists $self->{children}->{$form};
	return scalar values %{$self->{children}->{$form}};
}
#package Root;
sub addchild {
	my ($self,$child,$dir,$key,$form) = @_;
	$self->{children}->{$form}->{$key} = $child;
	$self->addfrom($child,$dir) if $self->can('addfrom');
}
#package Root;
sub mergefrom {
	my ($self,$othr) = @_;
	return unless $othr->isa('Root');
	foreach my $form %{$othr->{children}} {
		while (my ($k,$v) = each %{$othr->{children}->{$form}}) {
			$self->{children}->{$form}->{$k} = $v;
		}
	}
	$othr->{children} = {};
}

# -------------------------------------
package Leaf;
# -------------------------------------
#package Leaf;
sub form {
	my $self = shift;
	my $type = ref $self;
	$type =~ s/::.*//;
	return "\L$type\Es";
}
#package Leaf;
sub pkey {
	my $self = shift;
	my $type = ref $self;
	my ($ptyp,$key) = ($type->skey(@_));
	my $parent = $ptyp->get(@_);
	my $dir = 0;
	if ($self->isa('Path') and $parent->isa('Path')) {
		$dir = ($self->obja eq $parent->objb) ? 1 : 0;
	}
	$key = $dir + 1 unless $key;
	return $parent,$dir,$key;
}
#package Leaf;
sub addtoparent {
	my $self = shift;
	my ($parent,$dir,$key,$form) = @_;
	$self->{parent} = $parent;
	$self->{dir} = $dir;
	$self->{key}->{p} = $key;
	$self->{form} = $form;
	$parent->addchild($self,$dir,$key,$form) if $parent;
}
#package Leaf;
sub init {
	my $self = shift;
	$self->addtoparent($self->pkey(@_),$self->form());
}
#package Leaf;
sub parent {
	return shift->{parent};
}
#package Leaf;
sub childid {
	return shift->{key}->{p};
}
#package Leaf;
sub mergefrom {
	my ($self,$othr) = @_;
	return unless $othr->isa('Leaf');
	return if $self->{parent};
	return unless my $parent = $othr->{parent};
	my $dir = 0;
	if ($self->isa('Path') and $parent->isa('Path')) {
		$dir = ($self->obja == $parent->objb) ? 1 : 0;
	}
	my $key = $othr->{key}->{p};
	my $form = $self->form;
	$self->addtoparent($parent,$dir,$key,$form);
}

# -------------------------------------
package Tree; our @ISA = qw(Root Leaf);
# -------------------------------------

# -------------------------------------
package Type;
# -------------------------------------
#package Type;
sub init {
	my $self = shift;
	$self->{subs} = {};
}
#package Type;
sub fini {
	my $self = shift;
	delete $self->{subs};
}
#package Type;
sub hasa {
	my $self = shift;
	my ($subtype) = @_;
	return exists $self->{subs}->{$subtype};
}
#package Type;
sub sub {
	my $self = shift;
	my ($subtype) = @_;
	return $self->{subs}->{$subtype} if $self->hasa($subtype);
}

# -------------------------------------
package Subtype;
# -------------------------------------
#package Subtype;
sub subtype {
	my $self = shift;
	my $type = ref $self;
	$type =~ s/::.*//;
	return "\L$type\E";
}
#package Subtype;
sub init {
	my $self = shift;
	my $subtype = $self->{subtype} = $self->subtype(@_);
	my $class = $self->type(@_);
	my $type = $self->{type} = $class->get(@_);
	$type->{subs}->{$subtype} = $self;
}
#package Subtype;
sub fini {
	my $type = delete $self->{type};
	my $subtype = delete $self->{subtype};
	delete $type->{subs}->{$subtype} if $type and $subtype;
}

# -------------------------------------
package Network; our @ISA = qw(Decoder Log TripleStats);
# -------------------------------------
#package Network;
sub init {
	my $self = shift;
	$self->{items} = {};
	$self->{items}->{new} = [];
	$self->{items}->{chg} = [];
	$self->{items}->{num} = {};
}
#package Network;
sub fini {
	my $self = shift;
	delete $self->{items};
}
#package Network;
sub item {
	my ($self,$kind,$ref) = @_;
	my $keys = $ref;
	if (ref $ref ne 'ARRAY') {
		$keys = [ $ref ];
	}
	foreach my $key (@{$keys}) {
		return $self->{items}->{$kind}->{$key}
			if exists $self->{items}->{$kind}->{$key};
	}
	return undef;
}
#package Network;
sub getno {
	my ($self,$kind) = @_;
	return --$self->{items}->{num}->{$kind};
}
#package Network;
sub add_pdu_Pcap {
	my $self = shift;
	my ($pcap) = @_;
	return unless my $pdu = $pcap->unpack($self);
	$self->add_pdu($pdu);
}
#package Network;
sub add_pdu_Mtp2WithPhdr {
	my ($self,$msg) = @_;
	my $datalink = Datalink::Channel->get($self,$msg);
	return unless my $pdu = $msg->decode($datalink);
	return $datalink->add_pdu($pdu,$pdu->{dir});
}
#package Network;
sub add_pdu_SCTP {
	my $self = shift;
	my ($pkt) = @_;
	my $ip = $pkt->{pdu};
	my $lan = $ip->{pdu};

	Vprt->get($self,$lan->{shwa},$lan)->add_pkt($self,$lan,0);
	Vlan->get($self,$lan->{shwa},$lan)->add_pkt($self,$lan,0);

	Flow->get($self,$lan->{shwa},$ip->{saddr},$ip)->add_pkt($self,$ip,1);
	Flow->get($self,$lan->{dhwa},$ip->{daddr},$ip)->add_pkt($self,$ip,0) if $lan->{dhwa};

	my $type = 'Association::'.Frame::SCTP::PPIS->{$pkt->{ppi}};
	return $type->get($self,$pkt)->add_pdu_SCTP($self,$pkt);
}
#package Network;
sub add_pdu_ARP {
	my ($self,$pkt) = @_;
	if (my $host = Point::Host->get($self,$pkt->{arpspa},$pkt)) {
		$host->add_key($pkt->{arpsha});
	}
	if (my $host = Point::Host->get($self,$pkt->{arptpa},$pkt)) {
		$host->add_key($pkt->{arptha}) if $pkt->{arpop} == 2;
	}
	return 1;
}

# -------------------------------------
package Item; our @ISA = qw(Log);
# -------------------------------------
#package Item;
sub kind {
	my $self = shift;
	my $type = ref $self;
	$type =~ s/::.*//;
	return "\L$type\Es";
}
#package Item;
sub keys {
	return shift->key(@_);
}
#package Item;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($network) = @_;
	$self->{network} = $network;
	my $keys = $self->{key}->{n} = [ $type->keys(@_) ];
	my $kind = $self->{kind} = $self->kind();
	$self->{no} = $network->getno($kind);
	push @{$keys}, pack('N',$self->{no});
	foreach my $key (@{$keys}) {
		$network->{items}->{$kind}->{$key} = $self;
	}
	push @{$network->{items}->{new}->{$kind}}, $self;
	$self->{new} = 1;
}
#package Item;
sub find {
	my $type = shift;
	my ($network) = @_;
	my $self = $network->item(Item::kind($type),$type->key(@_));
	delete $self->{new} if exists $self->{new};
	return $self;
}
#package Item;
sub fini {
	my $self = shift;
	my $network = $self->{network};
	my $keys = $self->{key}->{n};
	my $kind = $self->{kind};
	foreach my $key (@{$keys}) {
		delete $network->{items}->{$kind}->{$key};
	}
	delete $self->{key}->{n};
	delete $self->{no};
}
#package Item;
sub add_key {
	my $self = shift;
	my ($key) = @_;
	my $kind = $self->{kind};
	foreach (@{$self->{key}->{n}}) {
		return if $_ eq $key;
	}
	my $othr = $self->{network}->item($kind,$key);
	if (defined $othr) {
		return if $othr eq $self;
		$self->mergefrom($othr);
		#FIXME: indicate merge to $self from $othr
	} else {
		#FIXME: indicate primary configuration change on $self
		push @{$self->{key}->{n}}, $key;
		$self->{network}->{items}->{$kind}->{$key} = $self;
	}
}
#package Item;
sub mergefrom {
	my ($self,$othr) = @_;
	return unless $othr->isa('Item');
	my ($s_network,$s_kind) = ($self->{network},$self->{kind});
	my ($o_network,$o_kind) = ($othr->{network},$othr->{kind});
	foreach (@{$othr->{key}->{n}}) {
		push @{$self->{key}->{n}}, $_;
		delete $o_network->{items}->{$o_kind}->{$_};
		$s_network->{items}->{$s_kind}->{$_} = $self;
	}
	$self->Root::mergefrom($othr) if $self->isa('Root');
	$self->Leaf::mergefrom($othr) if $self->isa('Leaf');
}
#package Item;
sub xform {
	my ($type,$self) = @_;
	my $oldtype = ref $self;
	bless $self,$type;
	$self->{network}->xformed($self,$oldtype,$type);
}

# -------------------------------------
package Point; our @ISA = qw(Item);
use Carp qw(cluck);
# -------------------------------------
#package Point;
sub key {
	my ($type,$network,$id,$msg) = @_;
	return [] unless defined $id;
	return pack('N',$id) if int($id);
	return $id;
}
#package Point;
sub init {
	my $self = shift;
	$self->{end} = { a => {}, b => {} };
}
#package Point;
sub fini {
	my $self = shift;
	my $self = shift;
	foreach my $side (values %{$self->{end}}) {
		foreach my $kind (values %{$side}) {
			foreach my $edge (values %{$kind}) {
				$edge->put(@_);
			}
		}
	}
	delete $self->{end};
}
#package Point;
sub mergefrom {
	my ($self,$othr) = @_;
	$self->SUPER::mergefrom($othr);
	# move all the edges from $othr to attach instead to $self.
	unless ($othr->isa('Point')) {
		cluck "Cannot merge $self and $othr";
		return undef;
	}
	my @tomerge = ();
	foreach my $side (keys %{$othr->{end}}) {
		foreach my $kind (keys %{$othr->{end}->{$side}}) {
			foreach my $key (keys %{$othr->{end}->{$side}->{$kind}}) {
				my $edgeo = $othr->{end}->{$side}->{$kind}->{$key};
				my $edges = $self->{end}->{$side}->{$kind}->{$key}
				 if exists $self->{end}->{$side}->{$kind}->{$key};
				if ($edges) {
					warn "cannot merge $edgeo and $edges";
					#does not work for overlapping edges
					push @tomerge, $edges, $edgeo;
				} else {
					$edgeo->{obj}->{$side} = $self;
					$self->{end}->{$side}->{$kind}->{$key} = $edgeo;
					delete $othr->{end}->{$side}->{$kind}->{$key};
				}
			}
		}
	}
	while (my $edges = shift @tomerge) {
		my $edgeo = shift @tomerge;
		$edges->mergefrom($edgeo);
	}
	# note: any edges that went from $othr to $self will wind up going
	# from $self to $self.
	$self->addrom($othr,$self->{dir}^$othr->{dir})
		if $self->isa('Counts') or $self->isa('Collector');
	#FIXME: add notification of merge
	return $self;
}
#package Point;
sub count {
	my ($self,$side,$kind) = @_;
	return undef unless exists $self->{end}->{$side}->{$kind};
	return scalar values %{$self->{end}->{$side}->{$kind}};
}
#package Point;
sub counts {
	my ($self,$kind) = @_;
	return undef unless exists $self->{end}->{a}->{$kind} or
	                    exists $self->{end}->{b}->{$kind};
	return (scalar values %{$self->{end}->{a}->{$kind}}) +
	       (scalar values %{$self->{end}->{b}->{$kind}});
}

# -------------------------------------
package Path; our @ISA = qw(Item);
use Carp qw(cluck);
# -------------------------------------
#package Path;
sub key {
	my ($type,$network,$keya,$keyb,$msg) = @_;
	if (ref $keya) {
		$keya = $keya->{key}->{p}->[0];
	} else {
		$keya = -1 unless defined $keya;
		$keya = pack('N',$keya) if int $keya;
	}
	if (ref $keyb) {
		$keyb = $keyb->{key}->{p}->[0];
	} else {
		$keyb = -1 unless defined $keyb;
		$keyb = pack('N',$keyb) if int $keyb;
	}
	return $keya.$keyb;
}
#package Path;
sub keypair {
	my ($objb,$obja) = (pop,pop);
	return $obja->{key}->{p}->[0],$objb->{key}->{p}->[0];
}
#package Path;
sub objpair {
	my $type = shift;
	my ($network,$obja,$objb,$msg) = @_;
	unless (ref $obja and ref $objb) {
		my ($typea,$typeb) = ($type->typpair(@_));
		$obja = $typea->get($network,$obja,$msg) unless ref $obja;
		$objb = $typeb->get($network,$objb,$msg) unless ref $objb;
	}
	return $obja,$objb;
}
#package Path;
sub typpair {
	return 'Point::Node','Point::Node';
}
#package Path;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($obja,$objb) = ($type->objpair(@_));
	$self->{obj}->{a} = $obja;
	$self->{obj}->{b} = $objb;
	my ($keya,$keyb) = ($type->keypair(@_,$obja,$objb));
	$self->{key}->{a} = $keya;
	$self->{key}->{b} = $keyb;
	$obja->{end}->{a}->{$self->{kind}}->{$keyb} = $self;
	$objb->{end}->{b}->{$self->{kind}}->{$keya} = $self;
	$self->{duplex} = 0;
}
#package Path;
sub fini {
	my $self = shift;
	my ($keya,$keyb) = ($self->{key}->{a},$self->{key}->{b});
	my ($obja,$objb) = ($self->{obj}->{a},$self->{obj}->{b});
	delete $obja->{end}->{a}->{$self->{kind}}->{$keyb} if $obja;
	delete $objb->{end}->{b}->{$self->{kind}}->{$keya} if $objb;
	foreach (qw(key obj)) { delete $self->{$_} }
}
#package Path;
sub identify {
	my $type = ref $self;
	my $id = "$type ";
	$id .= $self->{obj}->{a}->shortid;
	$id .= $self->{duplex} ? ', ' : ' -> ';
	$id .= $self->{obj}->{b}->shortid;
	my $cid = ' : '.$cid if $cid = $self->can('childid') and $cid = &{$cid}($self);
	$id .= $cid;
	return ($self->{id} = $id);
}
#package Path;
sub shortid {
	my $self = shift;
	my $conn = $self->{duplex} ? '::' : '->';
	my $cid = ' : '.$cid if $cid = $self->can('childid') and $cid = &{$cid}($self);
	return $self->{obj}->{a}->shortid.$conn.$self->{obj}->{b}->shortid.$cid;
}
#package Path;
sub mergefrom {
	my ($self,$othr) = @_;
	$self->SUPER::mergefrom($othr);
	# merge path $othr into $self  It does not matter between which
	# nodes $othr exists.
	unless (ref $self eq ref $othr) {
		cluck "Cannot merge $self and $othr";
		return undef;
	}
	$self->addfrom($othr,$self->{dir}^$othr->{dir})
		if $self->isa('Counts') or $self->isa('Collector');
	#FIXME: add notification of merge
	return $self;
}
#package Path;
sub obja { return $self->{obj}->{a} }
#package Path;
sub objb { return $self->{obj}->{b} }
#package Path;
sub objs { return $self->{obj}->{a},$self->{obj}->{b} }

# -------------------------------------
package Path::Duplex; our @ISA = qw(Path);
# -------------------------------------
# A Path::Duplex is a bidirectional concept.  There are two directional keys
# used to index the Path.
# -------------------------------------
sub init {
	my $self = shift;
	$self->{duplex} = 1;
}
sub keys {
	my $type = shift;
	my ($network,$keya,$keyb,$msg) = @_;
	return	$type->key($network,$keya,$keyb,$msg),
		$type->key($network,$keyb,$keya,$msg);
}


# -------------------------------------
package Traceable;
# -------------------------------------
#package Traceable;
sub init {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->{trace}->{$which} = [];
	$self->{tracing}->{$which} = $what;
}
#package Traceable;
sub fini {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->dotrace('put',${$self->{tracing}->{$which}});
	delete $self->{trace}->{$which};
	delete $self->{tracing}->{$which};
}
#package Traceable;
sub trace {
	my ($self,$which,$sub) = @_;
	my $index = scalar @{$self->{trace}->{$which}};
	push @{$self->{trace}->{$which}}, $sub;
	return $index;
}
#package Traceable;
sub untrace {
	my ($self,$which,$index) = @_;
	if ($self->{trace}->{$which}->[$index]) {
		$self->{trace}->{$which}->[$index] = undef;
		while (!defined $self->{trace}->{$which}->[0]) {
			shift @{$self->{trace}->{$which}};
		}
		while (!defined $self->{trace}->{$which}->[-1]) {
			pop @{$self->{trace}->{$which}};
		}
		return 1;
	}
}
#package Traceable;
sub dotrace {
	my ($self,$which,$what) = @_;
	foreach (@{$self->{trace}->{$which}}) {
		my $val = ${$self->{tracing}->{$which}};
		if (ref $_ eq 'SUB') {
			&{$_}($what,$val);
		} elsif (ref $_ eq 'ARRAY') {
			my $sub = shift @{$_};
			&{$sub}($what,$val,@{$_});
		}
	}
}

# -------------------------------------
package Showable; our @ISA = qw(Traceable);
# -------------------------------------
#package Showable;
sub init {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->{show}->{$which} = undef;
}
#package Showable;
sub fini {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->{show}->{$which}->put(@_) if $self->{show}->{$which};
	delete $self->{show}->{$which};

}
#package Showable;
sub update {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->Traceable::dotrace($which,'changed');
	$self->{show}->{$which}->update(@_) if $self->{show}->{$which};
}

# -------------------------------------
package Log; our @ISA = qw(Showable);
# -------------------------------------
#package Log;
sub init {
	my $self = shift;
	$self->{logs} = [];
}
#package Log;
sub fini {
	my $self = shift;
	delete $self->{logs};
}
#package Log;
sub log {
	my ($self,$text,$origin) = @_;
	my $fulltext = $text;
	if ($origin) {
		$fulltext .= $origin->identify.": $text";
	} else {
		$origin = $self;
	}
	push @{$self->{logs}}, $fulltext;
	if ($origin eq $self) {
		if ($self->isa('Path')) {
			foreach my $end ($self->{end}->{a},$self->{end}->{b}) {
				if ($end and $end->isa('Log')) {
					$end->log($text,$origin) if $end->isa('Log');
				}
			}
		}
	}
	if ($self->isa('Tree')) {
		my $parent = $self->{parent};
		if ($parent and $parent->isa('Log')) {
			$parent->log($text,$origin);
		}
	}
}

# -------------------------------------
package Addable;
# -------------------------------------
#package Addable;
sub addto {
	my $self = shift;
	my $othr = shift;
	return $othr->addrom($self,@_) if $othr->isa('Addable');
}

# -------------------------------------
package Counts; our @ISA = qw(Addable);
# -------------------------------------
#package Counts;
sub init {
	my ($self,$interval) = @_;
	$self->{interval} = $interval;
}
#package Counts;
sub fini {
	my ($self) = @_;
	delete $self->{interval};
}

# -------------------------------------
package MsgCounts; our @ISA = qw(Counts);
# -------------------------------------
#package MsgCounts;
sub init {
	my $self = shift;
	$self->{incs} = [{},{},{}];
}
#package MsgCounts;
sub fini {
	my $self = shift;
	delete $self->{incs};
}
#package MsgCounts;
sub inc {
	my ($self,$msg,$dir) = @_;
	$self->{incs}->[$dir]->{sus} += 1;
	$self->{incs}->[2]->{sus} += 1 unless $dir == 2;
	if ($msg->{li} == 0) {
		$self->{incs}->[$dir]->{fisus} += 1;
		$self->{incs}->[2]->{fisus} += 1 unless $dir == 2;
		return;
	}
	if ($msg->{li} == 1) {
		$self->{incs}->[$dir]->{lssus} += 1;
		$self->{incs}->[2]->{lssus} += 1 unless $dir == 2;
		return;
	}
	if ($msg->{li} == 2) {
		$self->{incs}->[$dir]->{lss2s} += 1;
		$self->{incs}->[2]->{lss2s} += 1 unless $dir == 2;
		return;
	}
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	my $mp = $msg->{mp};
	$self->{incs}->[$dir]->{msus} += 1;
	$self->{incs}->[$dir]->{sis}->{$si}->{msus} += 1;
	$self->{incs}->[$dir]->{sis}->{$si}->{$mt}->{msus} += 1;
	$self->{incs}->[$dir]->{sis}->{$si}->{$mt}->{$mp} += 1;
	$self->{incs}->[$dir]->{mp}->{$mp} += 1;
	unless ($dir ==[2) {
		$self->{incs}->[2]->{msus} += 1;
		$self->{incs}->[2]->{sis}->{$si}->{msus} += 1;
		$self->{incs}->[2]->{sis}->{$si}->{$mt}->{msus} += 1;
		$self->{incs}->[2]->{sis}->{$si}->{$mt}->{$mp} += 1;
		$self->{incs}->[2]->{mp}->{$mp} += 1;
	}
}
#package MsgCounts;
sub addfrom {
	my ($self,$othr,$flip) = @_;

	return unless $othr->isa('MsgCounts');

	foreach my $odir (qw(0 1 2)) {
		my $sdir = $odir ^ $flip unless $odir == 2;
		foreach my $su (qw(sus fisus lssus lss2s msus)) {
			next unless exists $othr->{incs}->[$odir]->{$su};
			$self->{incs}->[$sdir]->{$su} +=
			$othr->{incs}->[$odir]->{$su};
		}
		if (exists $othr->{incs}->[$odir]->{mp}) {
			foreach my $mp (keys %{$othr->{incs}->[$odir]->{mp}}) {
				$self->{incs}->[$sdir]->{mp}->{$mp} +=
				$othr->{incs}->[$odir]->{mp}->{$mp};
			}
		}
		if (exists $othr->{incs}->[$odir]->{sis}) {
			foreach my $si (keys %{$othr->{incs}->[$odir]->{sis}}) {
				foreach my $mt (keys %{$othr->{incs}->[$odir]->{sis}->{$si}}) {
					if ($mt eq 'msus') {
						$self->{incs}->[$sdir]->{sis}->{$si}->{msus} +=
						$othr->{incs}->[$odir]->{sis}->{$si}->{msus};
						next;
					}
					foreach my $mp (keys %{$othr->{incs}->[$odir]->{sis}->{$si}->{$mt}}) {
						$self->{incs}->[$sdir]->{sis}->{$si}->{$mt}->{$mp} +=
						$othr->{incs}->[$odir]->{sis}->{$si}->{$mt}->{$mp};
					}

				}
			}
		}
	}
}

# -------------------------------------
package CallCounts; our @ISA = qw(Counts);
# -------------------------------------
#package CallCounts;
sub init {
	my $self = shift;
	$self->{sess} = [0,0,0];
	$self->{pegs} = [{},{},{}];
	$self->{sims} = [{},{},{}];
	$self->{durs} = [{},{},{},[{},{},{}]];
	$self->{iats} = [{},{},{}];
	$self->{itvs} = [{},{},{}];
	$self->{ciccnt} = 0;
	$self->{actcnt} = [{},{},{},{}];
}
#package CallCounts;
sub fini {
	my $self = shift;
	foreach (qw(pegs sims durs iats itvs ciccnt actcnt)) { delete $self->{$_}; }
}
#package CallCounts;
sub ses {
	my ($self,$call,$dir) = @_;
	$self->{sess}->[$dir]++;
	$self->{sess}->[2]++ unless $dir == 2;
}
#package CallCounts;
sub peg {
	my ($self,$event,$ts,$dir) = @_;
	$self->{pegs}->[$dir]->{$event} += 1;
	$self->{pegs}->[2]->{$event} += 1 unless $dir == 2;
}
#package CallCounts;
sub act {
	my ($self) = @_;
	$self->{actcnt}->[0]++;
}
#package CallCounts;
sub deact {
	my ($self) = @_;
	$self->{actcnt}->[0]--;
}
#package CallCounts;
sub cnt {
	my ($self) = @_;
	$self->{ciccnt}++;
}
#package CallCounts;
sub uncnt {
	my ($self) = @_;
	$self->{ciccnt}--;
}
#package CallCounts;
sub sim_up {
	my ($self,$event,$dir) = @_;
	if (my $new = $self->{sims}->[$dir]->{$event}) {
		$new->{curr}++;
		$new->{hiwa} = $new->{curr} if $new->{curr} > $new->{hiwa};
	} else {
		$self->{sims}->[$dir]->{$event} = { curr=>1,hiwa=>1,lowa=>0 };
	}
}
#package CallCounts;
sub sim_dn {
	my ($self,$event,$dir) = @_;
	if (my $old = $self->{sims}->[$dir]->{$event}) {
		$old->{curr}--;
		$old->{lowa} = $old->{curr} if $old->{curr} < $old->{lowa} || $old->{lowa} <= 0;
		if ($old->{curr} < 0) {
			$old->{hiwa} += -$old->{curr};
			$old->{lowa} += -$old->{curr};
			$old->{curr} = 0;
		}
	} else {
		$self->{sims}->[$dir]->{$event} = { curr=>0,hiwa=>0,lowa=>0 };
	}
}
#package CallCounts;
sub sim {
	my ($self,$oldevent,$olddir,$newevent,$newdir) = @_;
	$self->sim_up($newevent,$newdir);
	$self->sim_up($newevent,2) unless $newdir == 2;
	$self->sim_dn($oldevent,$olddir);
	$self->sim_dn($oldevent,2) unless $olddir == 2;
}
#package CallCounts;
sub itv_dir {
	my ($self,$event,$itv,$dir) = @_;
	if (my $itvs = $self->{itvs}->[$dir]->{$event}) {
		push @{$itvs}, $itv;
	} else {
		$self->{itvs}->[$dir]->{$event} = [ $itv ];
	}
}
#package CallCounts;
sub itv {
	my ($self,$event,$beg,$end,$dir) = @_;
	my $itv = { beg=>$beg, end=>$end };
	$self->itv_dir($event,$itv,$dir);
	$self->itv_dir($event,$itv,2) unless $dir == 2;
	$self->dur($event,$beg,$end,$dir);
}
#package CallCounts;
sub dur {
	my ($self,$event,$beg,$end,$dir) = @_;
	my $dsecs = $end->{tv_sec} - $beg->{tv_sec};
	my $dusec = $end->{tv_usec} - $beg->{tv_usec};
	while ($dusec < 0 ) { $dsecs++; $dusec += 1000000; }
	my $dur = $dsecs + $dusec/1000000;
	$self->{durs}->[$dir]->{$event} += $dur;
	$self->{durs}->[2]->{$event} += $dur unless $dir == 2;
	my $int = $dsecs;
	$int++ if $dusec > 0;
	$self->{durs}->[3]->[$dir]->{$event}->{$int} += 1;
	$self->{durs}->[3]->[2]->{$event}->{$int} += 1 unless $dir == 2;
}
#package CallCounts;
sub iat_dir {
	my ($self,$event,$ts,$dir) = @_;
	if (my $iats = $self->{iats}->[$dir]->{$event}) {
		push @{$iats}, $ts;
	} else {
		$self->{iats}->[$dir]->{$event} = [ $ts ];
	}
}
#package CallCounts;
sub iat {
	my ($self,$event,$ts,$dir) = @_;
	$self->iat_dir($event,$ts,$dir);
	$self->iat_dir($event,$ts,2) unless $dir == 2;
}
#package CallCounts;
sub addfrom {
	my ($self,$othr) = @_;

	return unless $othr->isa('CallCounts');

	# FIXME: complete this
}

# -------------------------------------
package PktCounts; our @ISA = qw(Counts);
# -------------------------------------
#package PktCounts;
sub init {
	my $self = shift;
	$self->{hits} = [{},{},{}];
}
#package PktCounts;
sub fini {
	my $self = shift;
	delete $self->{hits};
}
#package PktCounts;
sub hit {
	my ($self,$msg,$dir) = @_;
	$self->{hits}->[$dir]++;
	$self->{hits}->[2]++ unless $dir == 2;
}
#package PktCounts;
sub addfrom {
	my ($self,$othr,$flip) = @_;

	return unless $othr->isa('PktCounts');

	# FIXME: complete this
}

# -------------------------------------
package History; our @ISA = qw(Counts);
# -------------------------------------
#package History;
sub init {
	my $self = shift;
	$self->{hist} = {};
}
#package History;
sub fini {
	my $self = shift;
	foreach (values %{$self->{hist}}) { $_->put(@_); }
	delete $self->{hist};
}
#package History;
sub interval {
	my ($self,$ts) = @_;
	my $int = $ts->{tv_sec};
	$int++ if ($ts->{tv_usec} > 0);
	$int = int(($int + 299)/300);
	return $int;
}
#package History;
sub hist {
	my ($self,$int) = @_;
	my $obj;
	unless ($obj = $self->{hist}->{$int}) {
		$obj = $self->{hist}->{$int} = $self->newcounts($int);
	}
	return $obj;
}
#package History;
sub addfrom {
	my ($self,$othr) = (shift,shift);

	return unless $othr->isa('History');

	foreach my $int (keys %{$othr->{hist}}) {
		$self->{hist}->{$int} = $self->newcounts($int) unless $self->{hist}->{$int};
		$self->{hist}->{$int}->addrom($othr->{hist}->{$int},@_);
	}
}

# -------------------------------------
package Passer;
# -------------------------------------

sub pass {
	my ($self,$sub,$type,$noprop) = (shift,shift,shift,pop);
	unless ($noprop) {
		if ($self->isa('Path')) {
			if (my $end = $self->{end}->{a} and $end->isa($type)) {
				$noprop = 1;
				$end->$sub(@_,$noprop);
			}
			if (my $end = $self->{end}->{b} and $end->isa($type)) {
				$noprop = 1;
				$end->$sub(@_,$noprop);
			}
		}
	}
	if ($self->isa('Leaf')) {
		if (my $parent = $self->{parent} and $parent->isa($type)) {
			$parent->$sub(@_,$noprop);
		}
	}
}

# -------------------------------------
package DirPasser; our @ISA = qw(Passer);
# -------------------------------------

sub pass {
	my ($self,$sub,$type,$noprop,$dir) = (shift,shift,shift,pop,pop);
	unless ($noprop) {
		if ($self->isa('Path')) {
			if (my $end = $self->{end}->{a} and $end->isa($type)) {
				my $enddir = $dir; $enddir ^= 0 unless $enddir == 2;
				$noprop = 1;
				$end->$sub(@_,$enddir,$noprop);
			}
			if (my $end = $self->{end}->{b} and $end->isa($type)) {
				my $enddir = $dir; $enddir ^= 1 unless $enddir == 2;
				$noprop = 1;
				$end->$sub(@_,$enddir,$noprop);
			}
		}
	}
	if ($self->isa('Leaf')) {
		if (my $parent = $self->{parent} and $parent->isa($type)) {
			my $pardir = $dir; $pardir ^= $self->{dir} unless $pardir == 2;
			$parent->$sub(@_,$pardir,$noprop);
		}
	}
}

# -------------------------------------
package EvtPasser; our @ISA = qw(DirPasser);
# -------------------------------------

sub pass {
	my ($self,$sub,$type,$evt1,$dir1,$evt2,$dir2,$noprop) = @_;
	unless ($noprop) {
		if ($self->isa('Path')) {
			if (my $end = $self->{end}->{a} and $end->isa($type)) {
				my $enddir1 = $dir1; $enddir1 ^= 0 unless $enddir1 == 2;
				my $enddir2 = $dir2; $enddir2 ^= 0 unless $enddir2 == 2;
				$noprop = 1;
				$end->$sub($evt1,$enddir1,$evt2,$enddir2,$noprop);
			}
			if (my $end = $self->{end}->{b} and $end->isa($type)) {
				my $enddir1 = $dir1; $enddir1 ^= 1 unless $enddir1 == 2;
				my $enddir2 = $dir2; $enddir2 ^= 1 unless $enddir2 == 2;
				$noprop = 1;
				$end->$sub($evt1,$enddir1,$evt2,$enddir2,$noprop);
			}
		}
	}
	if ($self->isa('Leaf')) {
		if (my $par = $self->{parent} and $par->isa($type)) {
			my $pardir1 = $dir1; $pardir1 ^= $self->{dir} unless $pardir1 == 2;
			my $pardir2 = $dir2; $pardir2 ^= $self->{dir} unless $pardir2 == 2;
			$end->$sub($evt1,$pardir1,$evt2,$pardir2,$noprop);
		}
	}
}

# -------------------------------------
package MsgHistory; our @ISA = qw(History MsgCounts DirPasser);
# -------------------------------------
#package MsgHistory;
sub newcounts {
	my $self = shift;
	return MsgCounts->new(@_);
}
#package MsgHistory;
sub inc {
	my ($self,$msg,$dir,$noprop) = @_;
	my $int = $self->interval($msg->{hdr});
	my $hist = $self->hist($int);
	$hist->inc($msg,$dir,$noprop);
	$self->MsgCounts::inc($msg,$dir,$noprop);
	#$self->DirPasser::pass('inc','MsgCounts',$msg,$dir,$noprop);
}
#package MsgHistory;
sub addfrom {
	my $self = shift;
	$self->MsgCounts::addfrom(@_);
	$self->History::addfrom(@_);
}

# -------------------------------------
package CallHistory; our @ISA = qw(History CallCounts EvtPasser);
# -------------------------------------
#package CallHistory;
sub newcounts {
	my $self = shift;
	return CallCounts->new(@_);
}
#package CallHistory;
sub ses {
	my ($self,$call,$dir,$noprop) = @_;
	my $int = $self->interval($ts);
	my $hist = $self->hist($int);
	$hist->ses($call,$dir,$noprop));
	$self->CallCounts::ses($call,$dir,$noprop);
	#$self->DirPasser::pass('ses','CallCounts',$call,$dir,$noprop);
}
#package CallHistory;
sub peg {
	my ($self,$event,$ts,$dir,$noprop) = @_;
	my $int = $self->interval($ts);
	my $hist = $self->hist($int);
	$hist->peg($event,$ts,$dir,$noprop));
	$self->CallCounts::peg($event,$ts,$dir,$noprop);
	$self->DirPasser::pass('peg','CallCounts',$event,$ts,$dir,$noprop);
}
#package CallHistory;
sub act {
	my $self = shift;
	$self->CallCounts::act(@_);
	$self->Passer::pass('act','CallCounts',@_);
}
#package CallHistory;
sub deact {
	my $self = shift;
	$self->CallCounts::deact(@_);
	$self->Passer::pass('deact','CallCounts',@_);
}
#package CallHistory;
sub cnt {
	my $self = shift;
	$self->CallCounts::cnt(@_);
	$self->Passer::pass('cnt','CallCounts',@_);
}
#package CallHistory;
sub uncnt {
	my $self = shift;
	$self->CallCounts::uncnt(@_);
	$self->Passer::pass('uncnt','CallCounts',@_);
}
#package CallHistory;
sub sim {
	my $self = shift;
	$self->CallCounts::sim(@_);
	$self->EvtPasser::pass('sim','CallCounts',@_,0);
}
#package CallHistory;
sub itv {
	my $self = shift;
	$self->CallCounts::itv(@_);
	$self->DirPasser::pass('itv','CallCounts',@_);
}
#package CallHistory;
sub dur {
	my ($self,$event,$beg,$end,$dir,$noprop) = @_;
#	my $s = $self->interval($beg);
#	my $e = $self->interval($end);
#	for (my $t = $s; $t <= $e; $t += 300) {
#		my $hist = $self->hist($t);
#		if ($t == $s) {
#			$hist->dur($event,$beg,{tv_sec=>$t+300,tv_usec=>0},$dir,$noprop);
#		} elsif ($t == $e) {
#			$hist->dur($event,{tv_sec=>$t,tv_usec=>0},$end,$dir,$noprop);
#		} else {
#			$hist->dur($event,{tv_sec=>$t,tv_usec=>0},{tv_sec=>$t+300,tv_usec=>0},$dir,$noprop);
#		}
#	}
	$self->CallCounts::dur($event,$beg,$end,$dir,$noprop);
	$self->DirPasser::pass('dur','CallCounts',$event,$beg,$end,$dir,$noprop);
}
#package CallHistory;
sub iat {
	my $self = shift;
	$self->CallCounts::iat(@_);
	$self->DirPasser::pass('iat','CallCounts',@_);
}
#package CallHistory;
sub addfrom {
	my $self = shift;
	$self->CallCounts::addfrom(@_);
	$self->History::addfrom(@_);
}

# -------------------------------------
package PktHistory; our @ISA = qw(History PktCounts DirPasser);
# -------------------------------------
#package PktHistory;
sub newcounts {
	my $self = shift;
	return PktCounts->new(@_);
}
#package PktHistory;
sub hit {
	my ($self,$pkt,$dir,$noprop) = @_;
	my $int = $self->interval($pkt->{hdr});
	my $hist = $self->hist($int);
	$hist->hit($pkt,$dir,$noprop);
	$self->PktCounts::hit($pkt,$dir,$noprop);
	#$self->DirPasser::pass('hit','PktCounts',$pkt,$dir,$noprop);
}
#package PktHistory;
sub addfrom {
	my $self = shift;
	$self->PktCounts::addfrom(@_);
	$self->History::addfrom(@_);
}

# -------------------------------------
package Collector; our @ISA = qw(Showable);
# -------------------------------------
#package Collector;
sub init {
	my ($self,$which) = (shift,shift);
	$self->{collection}->{$which} = [];
	$self->{collect}->{$which} = undef;
	$self->{collectcnt}->{$which} = 0;
}
#package Collector;
sub fini {
	my $self = shift;
	my ($which) = @_;
	delete $self->{collection}->{$which};
	delete $self->{collect}->{$which};
	delete $self->{collectcnt}->{$which};
}
#package Collector;
sub push {
	my ($self,$which,$what) = @_;
	push @{$self->{collection}->{$which}}, $what;
	$self->{collect}->{$which} = undef;
	$self->{collectcnt}->{$which}++;
	$self->Showable::update($which,$what);
}
#package Collector;
sub delete {
	my ($self,$which,$what) = @_;
	my @things = ();
	while (my $m = shift @{$self->{collection}->{$which}}) {
		if ($m eq $what) {
			$self->{collectcnt}->{$which}--;
			next;
		}
		push @things, $m;
	}
	push @{$self->{collection}->{$which}}, @things;
}
#package Collector;
sub pop {
	my ($self,$which) = @_;
	$self->{collectcnt}->{$which}--;
	return pop @{$self->{collection}->{$which}};
}
#package Collector;
sub clear {
	my ($self,$which) = @_;
	$self->{collection}->{$which} = [];
	$self->{collect}->{$which} = undef;
	$self->{collectcnt}->{$which} = 0;
}
#package Collector;
sub count {
	my ($self,$which) = @_;
	return scalar @{$self->{collection}->{$which}};
}
#package Collector;
sub addfrom {
	my ($self,$which,$othr) = (shift,shift,shift);
	splice(@{$self->{collection}->{$which}},$self->count,0,@{$othr->{collection}->{$which}});
}

# -------------------------------------
package MsgCollector; our @ISA = qw(Collector);
# -------------------------------------
#package MsgCollector;
sub only {
	my $self = shift;
	$self->_init('Collector','msgs',@_);
}
#package MsgCollector;
sub cull {
	my $self = shift;
	$self->_fini('Collector','msgs',@_);
}
#package MsgCollector;
sub push {
	return shift->Collector::push('msgs',@_);
}
#package MsgCollector;
sub delete {
	return shift->Collector::delete('msgs',@_);
}
#package MsgCollector;
sub pop {
	return shift->Collector::pop('msgs',@_);
}
#package MsgCollector;
sub clear {
	return shift->Collector::clear('msgs',@_);
}
#package MsgCollector;
sub count {
	return shift->Collector::count('msgs',@_);
}
#package MsgCollector;
sub addfrom {
	my $self = shift;
	return unless $_[0]->isa('MsgCollector');
	$self->Collector::addfrom('msgs',@_);
}

# -------------------------------------
package CallCollector; our @ISA = qw(Collector);
# -------------------------------------
#package CallCollector;
sub only {
	my $self = shift;
	$self->_init('Collector','calls',@_);
}
#package CallCollector;
sub cull {
	my $self = shift;
	$self->_fini('Collector','calls',@_);
}
#package CallCollector;
sub push {
	return shift->Collector::push('calls',@_);
}
#package CallCollector;
sub delete {
	return shift->Collector::delete('calls',@_);
}
#package CallCollector;
sub pop {
	return shift->Collector::pop('calls',@_);
}
#package CallCollector;
sub clear {
	return shift->Collector::clear('calls',@_);
}
#package CallCollector;
sub count {
	return shift->Collector::count('calls',@_);
}
#package CallCollector;
sub addfrom {
	my $self = shift;
	return unless $_[0]->isa('CallCollector');
	$self->Collector::addfrom('calls',@_);
}

# -------------------------------------
package PktCollector; our @ISA = qw(Collector);
# -------------------------------------
#package PktCollector;
sub only {
	my $self = shift;
	$self->_init('Collector','pkts',@_);
}
#package PktCollector;
sub cull {
	my $self = shift;
	$self->_fini('Collector','pkts',@_);
}
#package PktCollector;
sub push {
	return shift->Collector::push('pkts',@_);
}
#package PktCollector;
sub delete {
	return shift->Collector::delete('pkts',@_);
}
#package PktCollector;
sub pop {
	return shift->Collector::pop('pkts',@_);
}
#package PktCollector;
sub clear {
	return shift->Collector::clear('pkts',@_);
}
#package PktCollector;
sub count {
	return shift->Collector::count('pkts',@_);
}
#package PktCollector;
sub addfrom {
	my $self = shift;
	return unless $_[0]->isa('PktCollector');
	$self->Collector::addfrom('pkts',@_);
}

# -------------------------------------
package MsgStats; our @ISA = qw(MsgHistory MsgCollector Showable);
# -------------------------------------
#package MsgStats;
sub what {
	my $self = shift;
	return 'mstats',\$self->{incs}->[2]->{sus};
}
#package MsgStats;
sub init {
	my $self = shift;
	$self->{incs}->[2]->{sus} = 0;
}
#package MsgStats;
sub trace {
	return shift->Traceable::trace('mstats',@_);
}
#package MsgStats;
sub untrace {
	return shift->Traceable::untrace('mstats',@_);
}
#package MsgStats;
sub dotrace {
	return shift->Traceable::dotrace('mstats',@_);
}
#package MsgStats;
sub inc {
	my $self = shift;
	$self->SUPER::inc(@_);
	$self->dotrace('changed');
}
#package MsgStats;
sub addfrom {
	my $self = shift;
	$self->SUPER::addfrom(@_);
	$self->dotrace('changed');
}
#package MsgStats;
sub add_msg {
	my $self = shift;
	my ($msg,$dir,$noprop) = @_;
	$self->hit(@_);
	$self->MsgCollector::push($msg);
	$self->DirPasser::pass('add_msg','MsgStats',@_);
}

# -------------------------------------
package CallStats; our @ISA = qw(CallHistory CallCollector Showable);
# -------------------------------------
#package CallStats;
sub what {
	my $self = shift;
	return 'cstats',\$self->{ciccnt};
}
#package CallStats;
sub init {
	my $self = shift;
	$self->{ciccnt} = 0;
}
#package CallStats;
sub ses {
	my $self = shift;
	$self->SUPER::ses(@_);
	$self->dotrace('changed');
}
#package CallStats;
sub cnt {
	my $self = shift;
	$self->SUPER::cnt(@_);
	$self->dotrace('changed');
}
#package CallStats;
sub uncnt {
	my $self = shift;
	$self->SUPER::uncnt(@_);
	$self->dotrace('changed');
}
#package CallStats;
sub addfrom {
	my $self = shift;
	$self->SUPER::addfrom(@_);
	$self->dotrace('changed');
}
#package CallStats;
sub add_call {
	my $self = shift;
	my ($call,$dir,$noprop) = @_;
	$self->ses(@_);
	$self->CallCollector::push($call);
	$self->DirPasser::pass('add_call','CallStats',@_);
}

# -------------------------------------
package PktStats; our @ISA = qw(PktHistory PktCollector Showable);
# -------------------------------------
#package PktStats;
sub what {
	my $self = shift;
	return 'pstats',\$self->{hits}->[2];
}
#package PktStats;
sub init {
	my $self = shift;
	$self->{hits}->[2] = 0;
}
#package PktStats;
sub hit {
	my $self = shift;
	$self->SUPER::hit(@_);
	$self->dotrace('changed');
}
#package PktStats;
sub addfrom {
	my $self = shift;
	$self->SUPER::addfrom(@_);
	$self->dotrace('changed');
}
#package PktStats;
sub add_pkt {
	my $self = shift;
	my ($pkt,$dir,$noprop) = @_;
	$self->hit(@_);
	$self->PktCollector::push($pkt);
	$self->DirPasser::pass('add_pkt','PktStats',@_);
}

# -------------------------------------
package PairStats; our @ISA = qw(PktStats MsgStats);
# -------------------------------------
#package PairStats;
sub addfrom {
	my $self = shift;
	$self->PktStats::addfrom(@_);
	$self->MsgStats::addfrom(@_);
}

# -------------------------------------
package DualStats; our @ISA = qw(MsgStats CallStats);
# -------------------------------------
#package DualStats;
sub addfrom {
	my $self = shift;
	$self->MsgStats::addfrom(@_);
	$self->CallStats::addfrom(@_);
}

# -------------------------------------
package TripleStats; our @ISA = qw(PktStats MsgStats CallStats);
# -------------------------------------
#package TripleStats;
sub addfrom {
	my $self = shift;
	$self->PktStats::addfrom(@_);
	$self->MsgStats::addfrom(@_);
	$self->CallStats::addfrom(@_);
}

# -------------------------------------
package Buffer;
# -------------------------------------
#package Buffer;
sub init {
	my ($self,$which) = @_;
	$self->{buf}->{$which} = [];
}
#package Buffer;
sub fini {
	my ($self,$which) = @_;
	delete $self->{buf}->{$which};
}
#package Buffer;
sub pushbuf {
	my ($self,$which,$what) = @_;
	return push @{$self->{buf}->{$which}}, $what;
}
#package Buffer;
sub shiftbuf {
	my ($self,$which) = @_;
	return shift @{$self->{buf}->{$which}};
}
#package Buffer;
sub peekbuf {
	my ($self,$which) = @_;
	return $self->{buf}->{$which}->[0];
}

# -------------------------------------
package MsgBuffer; our @ISA = qw(Buffer);
# -------------------------------------
#package MsgBuffer;
sub only { shift->_init('Buffer','msg',@_); }
#package MsgBuffer;
sub cull { shift->_fini('Buffer','msg',@_); }
#package MsgBuffer;
sub pushbuf  { return shift->Buffer::pushbuf('msg',@_); }
#package MsgBuffer;
sub shiftbuf { return shift->Buffer::shiftbuf('msg',@_); }
#package MsgBuffer;
sub peekbuf  { return shift->Buffer::peekbuf('msg',@_); }

# -------------------------------------
package PktBuffer; our @ISA = qw(Buffer);
# -------------------------------------
#package PktBuffer;
sub only { shift->_init('Buffer','pkt',@_); }
#package PktBuffer;
sub cull { shift->_fini('Buffer','pkt',@_); }
#package PktBuffer;
sub pushbuf  { return shift->Buffer::pushbuf('pkt',@_); }
#package PktBuffer;
sub shiftbuf { return shift->Buffer::shiftbuf('pkt',@_); }
#package PktBuffer;
sub peekbuf  { return shift->Buffer::peekbuf('pkt',@_); }

# -------------------------------------
package CallBuffer; our @ISA = qw(Buffer);
# -------------------------------------
#package CallBuffer;
sub only { shift->_init('Buffer','call',@_); }
#package CallBuffer;
sub cull { shift->_fini('Buffer','call',@_); }
#package CallBuffer;
sub pushbuf  { return shift->Buffer::pushbuf('call',@_); }
#package CallBuffer;
sub shiftbuf { return shift->Buffer::shiftbuf('call',@_); }
#package CallBuffer;
sub peekbuf  { return shift->Buffer::peekbuf('call',@_); }

# -------------------------------------
package Stateful; our @ISA = qw(Traceable);
# -------------------------------------
#package Stateful;
sub what {
	return 'state',\shift->{state};
}
#package Stateful;
sub init {
	my $self = shift;
	my ($state,$options) = ($self->settings);
	$self->{oldstate}   = $state;
	$self->{state}      = $state;
	$self->{options}    = $options;
	$self->{statetext}  = $options->[$state]->[0];
}
#package Stateful;
sub fini {
	foreach (qw(options state oldstate)) { delete $self->{$_} }
}
#package Stateful;
sub trace {
	return shift->Traceable::trace('state',@_);
}
#package Stateful;
sub untrace {
	return shift->Traceable::untrace('state',@_);
}
#package Stateful;
sub dotrace {
	return shift->Traceable::dotrace('state',@_);
}
#package Stateful;
sub updatestate {
	my $self = shift;
	my @others = ();
	if ($self->isa('Path')) {
		push @others,$self->obja,$self->objb;
	}
	if ($self->isa('Leaf')) {
		push @others,$self->{parent} if exists $self->{parent};
	}
	foreach my $other (@others) {
		next unless ref $other;
		next unless my $other->can('chkstate');
		$other->chkstate($self,$self->{state});
	}
}
#package Stateful;
sub setstate {
	my ($self,$state) = @_;
	return if $self->{state} == $state;
	$self->{state} = $state;
	$self->{statetext} = $self->{options}->[$state]->[0];
	$self->dotrace('changed');
}
#package Stateful;
sub chkstate {
	my ($self,$obj,$state) = @_;
}

# -------------------------------------
package Tristate; our @ISA = qw(Stateful);
# -------------------------------------
#package Tristate;
use constant {
	TS_AVAILABLE => 0,
	TS_DEGRADED => 1,
	TS_UNAVAILABLE => 2,
};
#package Tristate;
$Tristate::options = [
	['Available'	=> TS_AVAILABLE    ],
	['Degraded'	=> TS_DEGRADED	   ],
	['Unavailable'	=> TS_UNAVAILABLE  ],
];
#package Tristate;
sub settings {
	return TS_AVAILABLE,$Tristate::options;
}

# -------------------------------------
package Spstate; our @ISA = qw(Stateful);
# -------------------------------------
#package Spstate;
use constant {
	SS_AVAILABLE	=> 0,
	SS_DEGRADED	=> 1,
	SS_CONGESTED_1	=> 2,
	SS_CONGESTED_2	=> 3,
	SS_CONGESTED_3	=> 4,
	SS_UNAVAILABLE	=> 5,
	SS_RESTARTING	=> 6,
};
#package Spstate;
$Spstate::options = {
	['Available'	=> SS_AVAILABLE	    ],
	['Degraded'	=> SS_DEGRADED	    ],
	['Congested(1)'	=> SS_CONGESTED_1   ],
	['Congested(2)'	=> SS_CONGESTED_2   ],
	['Congested(3)'	=> SS_CONGESTED_3   ],
	['Unavailable'	=> SS_UNAVAILABLE   ],
	['Restarting'	=> SS_RESTARTING    ],
};
#package Spstate;
sub settings {
	return SS_AVAILABLE,$Spstate::options;
}

# -------------------------------------
package Trunkstate; our @ISA = qw(Stateful);
# -------------------------------------
#package Trunkstate;
use constant {
	XS_INACTIVE => 0,
	XS_ACTIVE => 1,
	XS_DEGRADED => 2,
	XS_CONGESTED => 3,
	XS_FAILED => 4,
};
#package Trunkstate;
$Trunkstate::options = [
	['Inactive'	=> XS_INACTIVE	    ],
	['Active'	=> XS_ACTIVE	    ],
	['Degraded'	=> XS_DEGRADED	    ],
	['Congested'	=> XS_CONGESTED	    ],
	['Failed'	=> XS_FAILED	    ],
];
#package Trunkstate;
sub settings {
	return XS_INACTIVE,$Trunkstate::options;
}
#package Trunkstate;

# -------------------------------------
package Hexstate; our @ISA = qw(Stateful);
# -------------------------------------
#package Hexstate;
use constant {
	HS_AVAILABLE => 0,
	HS_DEGRADED => 1,
	HS_CONGESTED_1 => 2,
	HS_CONGESTED_2 => 3,
	HS_CONGESTED_3 => 4,
	HS_UNAVAILABLE => 5,
};
#package Hexstate;
$Hexstate::options = [
	['Available'	=> HS_AVAILABLE	    ],
	['Degraded'	=> HS_DEGRADED	    ],
	['Congested(1)'	=> HS_CONGESTED_1   ],
	['Congested(2)'	=> HS_CONGESTED_2   ],
	['Congested(3)'	=> HS_CONGESTED_3   ],
	['Unavailable' 	=> HS_UNAVAILABLE   ],
];
#package Hexstate;
sub settings {
	return HS_AVAILABLE,$Hexstate::options;
}

# -------------------------------------
package Pentastate; our @ISA = qw(Stateful);
# -------------------------------------
#package Pentastate;
use constant {
	PS_INSERVICE => 0,
	PS_BUSY => 1,
	PS_INHIBITED => 2,
	PS_BLOCKED => 3,
	PS_OUTOFSERVICE => 4,
};
#package Pentastate;
$Pentastate::options = [
	['Inservice'	  => PS_INSERVICE	],
	['Busy'		  => PS_BUSY		],
	['Inhibited'	  => PS_INHIBITED	],
	['Blocked'	  => PS_BLOCKED		],
	['Out of service' => PS_OUTOFSERVICE	],
];
#package Pentastate;
sub settings {
	return PS_INSERVICE,$Pentastate::options;
}

# -------------------------------------
package Linkstate; our @ISA = qw(Stateful);
# -------------------------------------
#package Linkstate;
use constant {
	LS_UNINIT	    => 0,
	LS_ALIGNING	    => 1,
	LS_PROVING	    => 2,
	LS_IN_SERVICE	    => 3,
	LS_OUTAGE	    => 4,
	LS_CONGESTED	    => 5,
	LS_OUT_OF_SERVICE   => 6,
	LS_INHIBITED	    => 7,
}
#package Linkstate;
$Linkstate::options = {
	['Uninitialized'    => LS_UNINIT	    ],
	['Aligning'	    => LS_ALIGNING	    ],
	['Proving'	    => LS_PROVING	    ],
	['In service'	    => LS_IN_SERVICE	    ],
	['Processor outage' => LS_OUTAGE	    ],
	['Busy'		    => LS_CONGESTED	    ],
	['Out of service'   => LS_OUT_OF_SERVICE    ],
	['Inhibited'	    => LS_INHIBITED	    ],
}
#package Linkstate;
sub settings {
	return LS_UNINIT,$Linkstate::options;
}

# -------------------------------------
package Circuitstate; our @ISA = qw(Stateful);
# -------------------------------------
#package Circuitstate;
use constant {
	CTS_UNINIT	=> 0,
	CTS_IDLE	=> 1,
	CTS_WAIT_ACM	=> 2,
	CTS_WAIT_ANM	=> 3,
	CTS_ANSWERED	=> 4,
	CTS_SUSPENDED	=> 5,
	CTS_WAIT_RLC	=> 6,
	CTS_SEND_RLC	=> 7,
	CTS_COMPLETE	=> 8,
};
#package Circuitstate;
$Circuitstate::options = [
	['Uninitialized'    => CTS_UNINIT	],
	['Idle'		    => CTS_IDLE		],
	['Setup'	    => CTS_WAIT_ACM	],
	['Alerting'	    => CTS_WAIT_ANM	],
	['Connected'	    => CTS_ANSWERED	],
	['Suspended'	    => CTS_SUSPENDED	],
	['Releasing'	    => CTS_WAIT_RLC	],
	['Released'	    => CTS_SEND_RLC	],
	['Completed'	    => CTS_COMPLETE	],
];
#package Circuitstate;
sub settings {
	return CTS_UNINIT,$Circuitstate::options;
}

# -------------------------------------
package Point::Node; our @ISA = qw(Point);
# -------------------------------------
# Nodes are unidentified signalling points.  We know that there are two nodes on
# either side of a datalink, we just do not know which node yet.  A bare
# Point::Node object is an unknown signalling point.  It just acts as a place
# holder for when the data link is bound to an actual node.  Notes need a put
# method so that the datalink can put these nodes once they are no longer
# necessary.  Nodes are created in reference to a datalink, the side of the
# datalink on which they exist.
# -------------------------------------
package Point::AS; our @ISA = qw(Point::Node);
# -------------------------------------
# Point::AS are unidentified signalling points acting as an application server
# instance.  These are primarily for M3UA and SUA where the node is never
# directly assigned a signalling point code, but just remains unidentified to
# represent the application server.
# -------------------------------------
package Point::SG; our @ISA = qw(Point::Node);
# -------------------------------------
# Point::SG are unidentified signalling points acting as a signalling gateway
# instance.  These are primarily for M3UA and SUA where the node is never
# directly assigned a signalling point code, but just remains unidentified to
# represent the signalling gateway.
# -------------------------------------

# -------------------------------------
package Point::Cluster; our @ISA = qw(Point::Node Tristate MsgStats);
# -------------------------------------
# A Point::Cluster is a signalling cluster with an identified cluster code.
# -------------------------------------
#package Point::Cluster;
sub cluster {
	my $pc = shift;
	if ($pc & (~0x00003fff)) {
		$pc &= 0x00ffff00;
	} else {
		$pc &= 0x00003ff8;
	}
	return $pc;
}
#package Point::Cluster;
sub key {
	my $type = shift;
	my ($network,$pc,$msg) = @_;
	$pc = $pc->{pc} if ref $pc;
	$pc = Point::Cluster::cluster($pc);
	return pack('N',$pc);
}
#package Point::Cluster;
sub init {
	my ($self,$network,$pc,$msg) = @_;
	$pc = Point::Cluster::cluster($pc);
	$self->{pc} = $pc;
}

# -------------------------------------
package Point::SP; our @ISA = qw(Point::Node Spstate MsgStats);
# -------------------------------------
# An Point::SP is a signalling point with an identified point code.
# -------------------------------------
#package Point::SP;
sub init {
	my ($self,$network,$pc,$msg) = @_;
	$self->{pc} = $pc;
	$self->{primary} = undef;
	$self->{aliases} = [];
}
#package Point::SP;
sub fini {
	my $self = shift;
	delete $self->{aliases};
	delete $self->{primary};
	delete $self->{pc};
}
#package Point::SP;
sub add_msg {
	my $self = shift;
	$self->SUPER::add_msg(@_);
	my $xchg_sltm = $self->counts('linksets') > 0;
	my $xchg_isup = $self->counts('trunkgroups') > 0;
	my $orig_tcap = $self->count('a','transacts') > 0;
	my $term_tcap = $self->count('b','transacts') > 0;
	if ($xchg_isup || ($orig_tcap and $term_tcap)) {
		Point::SSP->xform($self) unless $self->isa('Point::SSP');
	} elsif ($orig_tcap and not $term_tcap) {
		Point::SCP->xform($self) unless $self->isa('Point::SCP');
	} elsif (not $orig_tcap and $term_tcap) {
		Point::GTT->xform($self) unless $self->isa('Point::GTT');
	} elsif ($xchg_sltm) {
		Point::STP->xform($self) unless $self->isa('Point::STP');
	}
}
#package Point::SP;
sub xform {
	my ($type,$self) = @_;
	$self->CallStats::fini if (not $type->isa('CallStats') and $self->isa('CallStats'));
	$self->XactStats::fini if (not $type->isa('XactStats') and $self->isa('XactStats'));
	$type = ref $self; Item::xform(@_);
	$self->CallStats::init if ($self->isa('CallStats') and not $type->isa('CallStats'));
	$self->XactStats::init if ($self->isa('XactStats') and not $type->isa('XactStats'));
	$self->findaliases;
}
#package Point::SP;
sub makereal {
	my $self = shift;
	return if $self->{real};
	$self->{real} = 1;
	$self->findaliases;
}
#package Point::SP;
sub findaliases {
	my $self = shift;
	return unless $self->{real};
	return unless $self->isa('Point::SEP');
	$self->{aliases} = [];
	foreach my $route (values %{$self->{end}->{a}->{routes}}) {
		my $alias = $route->objb;
		$alias->{alias} = 1;
		$alias->{primary} = $self;
		push @{$self->{aliases}}, $alias;
	}
}
#package Point::SP;
#
# When a signalling point itself is marked congested (it sent a TFC or SCON) all
# of the routesets to that signalling point are also marked congested.
#
sub setstate {
	my $self = shift;
	my ($state) = @_;
	return if $self->{state} == $state;
	$self->SUPER::setstate(@_);
	return unless Spstate::SS_DEGRADED <= $state and $state <= Spstate::SS_CONGESTED_3;
	$state = $state - Spstate::SS_DEGRADED + Hexstate::HS_DEGRADED;
	foreach my $side (qw/a b/) {
		foreach (values %{$self->{end}->{$side}->{routesets}}) {
			$_->setstate($state);
		}
	}
}
#package Point::SP;
#
# Whenever a signalling routeset from the node is marked available, the
# signalling point is also marked available if it was previously marked
# congested.
#
sub chkstate {
	my ($self,$obj,$state) = @_;
	return unless $obj->isa('Routeset');
	return unless $state == Hexstate::HS_AVAILABLE;
	return if $self->{state} == Spstate::SS_AVAILABLE;
	return if $self->{state} == Spstate::SS_RESTARTING;
	$self->setstate(Spstate::SS_AVAILABLE);
}

# -------------------------------------
package Point::SEP; our @ISA = qw(Point::SP);
# -------------------------------------
# -------------------------------------
package Point::SSP; our @ISA = qw(Point::SEP CallStats XactStats);
# -------------------------------------
# -------------------------------------
package Point::SCP; our @ISA = qw(Point::SEP XactStats);
# -------------------------------------
# -------------------------------------
package Point::STP; our @ISA = qw(Point::SP);
# -------------------------------------
# -------------------------------------
package Point::GTT; our @ISA = qw(Point::SP XactStats);
# -------------------------------------

# -------------------------------------
package Point::Station; our @ISA = qw(Point PktStats);
# -------------------------------------
# Stations are an unidentified IP host or gateway.  We know that there are two
# stations on the edge of a single ethernet, we just do not know which stations
# yet.  A bare Point::Station object is an unknown IP host or gateway.  It just acts as
# a place holder for when the station is bound to an actual IP host or gateway.
# Stations need a put method so that the ethernet can put these stations once
# they are no longer necessary.  Stations are created in reference to an
# ethernet, the edge of the ethernet on which they exist.
#
# When a Point::Station is created, we know possibly one of its Ethernet hardware
# addresses (Port), but we have no idea which IP addresses belong to it until an
# ARP or LLDP packet is processed.  When we are live (we are viewing captured
# files or performing capture on the monitoring point itself), we can query ARP,
# LLDP and SNMP for further information.  In particular LLDP can help us find
# the management IP address associated with the station so that we can we can
# query it directly with SNMP.
#
# The addchild() method of the Root package is overridden so that the key
# associated with the child Port can be added to the station and stations can
# be properly merged once their hardware and IP addresses are discovered.
# -------------------------------------
#package Point::Station;
sub addchild {
	my $self = shift;
	my ($child,$dir,$key,$form) = @_;
	$self->SUPER::addchild(@_);
	$self->add_key($key);
}

# -------------------------------------
package Point::Host; our @ISA = qw(Point::Station);
# -------------------------------------
#package Point::Host;
sub add_pkt {
	my $self = shift;
	my ($pkt,$dir,$noprop) = @_;
	my $pdu = $pkt->{pdu};
	my $ip = $dir ? $pdu->{daddr} : $pdu->{saddr};
	$self->add_key($ip);
	$self->SUPER::add_pkt(@_);
}

# -------------------------------------
package Point::Gateway; our @ISA = qw(Point::Host);
# -------------------------------------

# -------------------------------------
package Point::ASP; our @ISA = qw(Point::Host);
# -------------------------------------
# A Point::ASP represents an application server process instance for SIGTRAN
# protocols.
# -------------------------------------
package Point::SGP; our @ISA = qw(Point::Host);
# -------------------------------------
# A Point::SGP represents a signalling gateway process instance for SIGTRAN
# protocols.
# -------------------------------------
package Point::SPP; our @ISA = qw(Point::Host);
# -------------------------------------
# A Point::SPP represents an IP signalling point process instances for SIGTRAN
# protocols (not M2UA).
# -------------------------------------

# -------------------------------------
package Relation; our @ISA = qw(Path::Duplex Root Type Hexstate MsgStats);
# -------------------------------------
# A relation is an association between signalling points that communicate with
# each other.  This object is used to track these interactions, primarily for
# identifying the role of nodes.
# -------------------------------------
#package Relation;
sub add_msg {
	my $self = shift;
	$self->SUPER::add_msg(@_);
	my ($msg,$dir,$noprop) = @_;
	if ($msg->isa('Message::ISUP')) {
		my $trunkgroup = Trunkgroup->get($self->{network},$self->objs,$msg)
			unless $trunkgroup = $self->{trunkgroup};
		$trunkgroup->add_msg(@_);
	} elsif ($msg->isa('Message::SCCP')) {
		my $service = Service->get($self->{network},$self->objs,$msg)
			unless $service = $self->{service};
		$service->add_msg(@_);
	}
}

# -------------------------------------
package Routeset; our @ISA = qw(Path Leaf Hexstate MsgStats);
# -------------------------------------
# A routeset is a collection of routes from one node (node a) to another (node b).
# In contrast to relations, a routeset is a unidirectional concept and is not
# concerned about calls or circuits.
# -------------------------------------
#package Routeset;
sub skey { return 'Relation',0 }

# -------------------------------------
package Cluster; our @ISA = qw(Path Root Tristate MsgStats);
# -------------------------------------
#package Cluster;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	$nodea = $nodea->{no} if ref $nodea;
	$nodeb = $nodeb->{pc} if ref $nodeb;
	$nodeb = Point::Cluster::cluster($nodeb);
	return pack('NN',$nodea,$nodeb);
}
#package Cluster;
sub objpair {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	$nodea = Point::Node->get($network,$nodea,$msg) unless ref $nodea;
	$nodeb = Point::Cluster->get($network,$nodeb,$msg);
	return $nodea,$nodeb;
}
#package Cluster;
sub setstate {
	my ($self,$state) = @_;
	$self->SUPER::setstate($state);
	foreach (values %{$self->{children}->{routes}}) {
		$_->setstate($state);
	}
}

# -------------------------------------
package Route; our @ISA = qw(Path Leaf Tristate MsgStats);
# -------------------------------------
#package Route;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	$nodea = $nodea->{no} if ref $nodea;
	$nodeb = $nodeb->{pc} if ref $nodeb;
	return pack('NN',$nodea,$nodeb);
}
#package Route;
sub typpair {
	return 'Point::Node','Point::SP';
}
#package Route;
sub skey {
	my ($type,$network,$nodea,$nodeb,$msg) = @_;
	$nodeb = $nodeb->{pc} if ref $nodeb;
	return 'Cluster',$nodeb;
}

# -------------------------------------
package Trunkgroup; our @ISA = qw(Path::Duplex Root Subtype Trunkstate CallStats);
# -------------------------------------
# A Trunkgroup is the set of all trunks between two signalling points.  It is a
# specialization of a Relation.
# -------------------------------------
#package Trunkgroup;
sub type { 'Relation' }
#package Trunkgroup;
sub add_msg {
	my $self = shift;
	my ($msg,$dir,$noprop) = @_;
	my $meth = 'add_pdu_'.(split(/::/,ref $msg))[-1];
	if (Trunkgroup->can($meth)) {
		$self->$meth(@_);
	} elsif (Call->can($meth)) {
		Circuit->get($self->{network},$self->objs,$msg)->getcall->$meth(@_);
	} elsif (Circuit->can($meth)) {
		Circuit->get($self->{network},$self->objs,$msg)->$meth(@_);
	} else {
		# ignoring this kind of message
	}
}
#package Trunkgroup;
sub add_pdu_UCIC {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}

# -------------------------------------
package Trunk; our @ISA = qw(Path Tree Trunkstate CallStats);
# -------------------------------------
# A Trunk is a distinct set of circuits within a Trunkgroup that share a common
# characteristic.  Default trunks are 1W-O/G, 1W-I/C and 2W trunks.  The user should be
# able to define new trunks and move circuits to and from these default trunks.
# -------------------------------------
#package Trunk;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	my $fordir = pack('C',Circuit::fordir($msg));
	return	Path->key(@_).$fordir;
}
#package Trunk;
sub keys {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	my $fordir = pack('C',Circuit::fordir($msg));
	my $revdir = pack('C',Circuit::revdir($msg));
	return	Path->key($network,$nodea,$nodeb,$msg).$fordir,
		Path->key($network,$nodeb,$nodea,$msg).$revdir;
}
#package Trunk;
sub keypair {
	my ($type,$network,$nodea,$nodeb,$msg) = @_;
	return (
		pack('NC',$nodea->{pc},Circuit::fordir($msg)),
		pack('NC',$nodeb->{pc},Circuit::revdir($msg)),
	);
}
#package Trunk;
sub skey {
	my ($type,$newtork,$nodea,$nodeb,$msg) = @_;
	return 'Trunkgroup',Circuit::fordir($msg);
}

# -------------------------------------
package Circuit; our @ISA = qw(Decoder Tree Circuitstate CallStats MsgBuffer);
# -------------------------------------
#package Circuit;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	return	Path->key(@_).pack('n',$msg->{cic});
}
#package Circuit;
sub keys {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	my $cic = pack('n',$msg->{cic});
	return	Path->key($network,$nodea,$nodeb,$msg).$cic,
		Path->key($network,$nodeb,$nodea,$msg).$cic;
}
#package Circuit;
sub fordir {
	return shift->{calldir};
}
#package Circuit;
sub revdir {
	my $calldir = shift->{calldir};
	return CD_INACT if $calldir == CD_INACT;
	return CD_1W_IC if $calldir == CD_1W_OG;
	return CD_1W_OG if $calldir == CD_1W_IC;
	return CD_BOTHW;
}
#package Circuit;
sub skey {
	my ($type,$network,$nodea,$nodeb,$msg) = @_;
	return 'Trunk',$msg->{calldir};
}
#package Circuit;
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	$self->{ts}->{tv_sec}  = $network->{begtime}->{tv_sec};
	$self->{ts}->{tv_usec} = $network->{begtime}->{tv_usec};
	$self->{active} = 0;
	$self->cnt();
}
#package Circuit;
sub fini {
	my $self = shift;
	$self->deact() if $self->{active};
	$self->uncnt();
	foreach (qw(ts active)) { delete $self->{$_} }
}
#package Circuit;
sub identify {
	my $self = shift;
	my $id = "Circuit $self->{cic} ";
	$id .= $self->parent->obja->shortid;
	$id .= ', ';
	$id .= $self->parent->objb->shortid;
	return ($self->{id} = $id);
}
#package Circuit;
sub shortid {
	my $self = shift;
	return "$self->{parent}->obja->{pcode},$self->{parent}->objb->{pcode}:$self->{cic}";
}
#package Circuit;
sub getcall {
	my $self = shift;
	my $call = $self->{call} = Call->new($self) unless $call = $self->{call};
	return $call;
}
#package Circuit;
sub add_pdu_COT {
	# COT can be circuit-related or call-related
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_RLC {
	# RLC can be circuit-related or call-related
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CCR {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_RSC {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_BLO {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_UBL {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_BLA {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_UBA {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_GRS {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CGB {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CGU {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CGBA {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CGUA {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_LPA {
	# LPA can be circuit-related
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CSVQ {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CSVR {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_DRS {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_PAM {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_GRA {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
	#FIXME
}
#package Circuit;
sub add_pdu_CQM {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CQR {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_UCIC {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
	if (my $call = $self->{call}) {
		$call->UCIC(@_,$self);
	}
}
#package Circuit;
sub add_pdu_CFN {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
	if (my $call = $self->{call}) {
		$call->CFN(@_,$self);
	}
}
#package Circuit;
sub add_pdu_UPT {
	# actually relation related
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_UPA {
	# actually relation related
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CVR {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_CVT {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}
#package Circuit;
sub add_pdu_MCP {
	my $self = shift;
	my ($network,$msg,$dir) = @_;
}

# -------------------------------------
package Call;
# -------------------------------------
# When a call processes an ISUP messages using one of the add_pdu_* methods, it
# must also invoke the equivalent Circuit method if necessary because the
# circuit has not yet seen this message.
# -------------------------------------
#package Call;
sub add_pdu_IAM {
	my $self = shift;
	$self->{circuit}->add_pdu_IAM(@_) if Circuit->can('add_pdu_IAM');
}
#package Call;
sub add_pdu_SAM {
	my $self = shift;
	$self->{circuit}->add_pdu_SAM(@_) if Circuit->can('add_pdu_SAM');
}
#package Call;
sub add_pdu_INR {
	my $self = shift;
	$self->{circuit}->add_pdu_INR(@_) if Circuit->can('add_pdu_INR');
}
#package Call;
sub add_pdu_INF {
	my $self = shift;
	$self->{circuit}->add_pdu_INF(@_) if Circuit->can('add_pdu_INF');
}
#package Call;
sub add_pdu_COT {
	my $self = shift;
	$self->{circuit}->add_pdu_COT(@_) if Circuit->can('add_pdu_COT');
}
#package Call;
sub add_pdu_ACM {
	my $self = shift;
	$self->{circuit}->add_pdu_ACM(@_) if Circuit->can('add_pdu_ACM');
}
#package Call;
sub add_pdu_CON {
	my $self = shift;
	$self->{circuit}->add_pdu_CON(@_) if Circuit->can('add_pdu_CON');
}
#package Call;
sub add_pdu_FOT {
	my $self = shift;
	$self->{circuit}->add_pdu_FOT(@_) if Circuit->can('add_pdu_FOT');
}
#package Call;
sub add_pdu_ANM {
	my $self = shift;
	$self->{circuit}->add_pdu_ANM(@_) if Circuit->can('add_pdu_ANM');
}
#package Call;
sub add_pdu_REL {
	my $self = shift;
	$self->{circuit}->add_pdu_REL(@_) if Circuit->can('add_pdu_REL');
}
#package Call;
sub add_pdu_SUS {
	my $self = shift;
	$self->{circuit}->add_pdu_SUS(@_) if Circuit->can('add_pdu_SUS');
}
#package Call;
sub add_pdu_RES {
	my $self = shift;
	$self->{circuit}->add_pdu_RES(@_) if Circuit->can('add_pdu_RES');
}
#package Call;
sub add_pdu_RLC {
	my $self = shift;
	$self->{circuit}->add_pdu_RLC(@_) if Circuit->can('add_pdu_RLC');
}
#package Call;
sub add_pdu_CMR {
	my $self = shift;
	$self->{circuit}->add_pdu_CMR(@_) if Circuit->can('add_pdu_CMR');
}
#package Call;
sub add_pdu_CMC {
	my $self = shift;
	$self->{circuit}->add_pdu_CMC(@_) if Circuit->can('add_pdu_CMC');
}
#package Call;
sub add_pdu_CMRJ {
	my $self = shift;
	$self->{circuit}->add_pdu_CMRJ(@_) if Circuit->can('add_pdu_CMRJ');
}
#package Call;
sub add_pdu_FAR {
	my $self = shift;
	$self->{circuit}->add_pdu_FAR(@_) if Circuit->can('add_pdu_FAR');
}
#package Call;
sub add_pdu_FRJ {
	my $self = shift;
	$self->{circuit}->add_pdu_FRJ(@_) if Circuit->can('add_pdu_FRJ');
}
#package Call;
sub add_pdu_FAD {
	my $self = shift;
	$self->{circuit}->add_pdu_FAD(@_) if Circuit->can('add_pdu_FAD');
}
#package Call;
sub add_pdu_FAI {
	my $self = shift;
	$self->{circuit}->add_pdu_FAI(@_) if Circuit->can('add_pdu_FAI');
}
#package Call;
sub add_pdu_LPA {
	my $self = shift;
	$self->{circuit}->add_pdu_LPA(@_) if Circuit->can('add_pdu_LPA');
}
#package Call;
sub add_pdu_DRS {
	my $self = shift;
	$self->{circuit}->add_pdu_DRS(@_) if Circuit->can('add_pdu_DRS');
}
#package Call;
sub add_pdu_CPG {
	my $self = shift;
	if ($self->{state} == ::CTS_WAIT_ACM or $self->{state} == ::CTS_WAIT_ANM) {
		return;
	}
	$self->add_msg(@_);
	$self->{circuit}->add_pdu_CPG(@_) if Circuit->can('add_pdu_CPG');
}
#package Call;
sub add_pdu_USR {
	my $self = shift;
	$self->{circuit}->add_pdu_USR(@_) if Circuit->can('add_pdu_USR');
}
#package Call;
sub add_pdu_UCIC {
	my $self = shift;
	$self->clear_call(@_);
	$self->{circuit}->add_pdu_UCIC(@_) if Circuit->can('add_pdu_UCIC');
}
#package Call;
sub add_pdu_CFN {
	my $self = shift;
	if ($self->{state} < ::CTS_ANSWERED) {
		$self->clear_call(@_);
		return;
	}
	$self->add_msg(@_);
	$self->{circuit}->add_pdu_CFN(@_) if Circuit->can('add_pdu_CFN');
}
#package Call;
sub add_pdu_OLM {
	my $self = shift;
	$self->{circuit}->add_pdu_OLM(@_) if Circuit->can('add_pdu_OLM');
}
#package Call;
sub add_pdu_CRG {
	my $self = shift;
	$self->{circuit}->add_pdu_CRG(@_) if Circuit->can('add_pdu_CRG');
}
#package Call;
sub add_pdu_NRM {
	my $self = shift;
	$self->{circuit}->add_pdu_NRM(@_) if Circuit->can('add_pdu_NRM');
}
#package Call;
sub add_pdu_FAC {
	my $self = shift;
	$self->{circuit}->add_pdu_FAC(@_) if Circuit->can('add_pdu_FAC');
}
#package Call;
sub add_pdu_IDR {
	my $self = shift;
	$self->{circuit}->add_pdu_IDR(@_) if Circuit->can('add_pdu_IDR');
}
#package Call;
sub add_pdu_IRS {
	my $self = shift;
	$self->{circuit}->add_pdu_IRS(@_) if Circuit->can('add_pdu_IRS');
}
#package Call;
sub add_pdu_EXM {
	my $self = shift;
	$self->{circuit}->add_pdu_EXM(@_) if Circuit->can('add_pdu_EXM');
}
#package Circuit;
sub add_pdu_SGM {
	my $self = shift;
	$self->add_msg(@);
	$self->{circuit}->add_pdu_SGM(@_) if Circuit->can('add_pdu_SGM');
}
#package Circuit;
sub add_pdu_CRA {
	my $self = shift;
	if ($self->{state} == ::CTS_WAIT_ACM) {
		$self->setstate(@_,::CTS_WAIT_ACM);
		$self->add_msg(@_);
	}
	$self->{circuit}->add_pdu_CRA(@_) if Circuit->can('add_pdu_CRA');
}
#package Circuit;
sub add_pdu_CRM {
	my $self = shift;
	if ($self->{state} != ::CTS_IDLE and $self->{state} != ::CTS_WAIT_ACM) {
		$self->end_of_call(@_);
	}
	$self->setstate(@_,::CTS_WAIT_ACM);
	$self->add_msg(@_);
	$self->{circuit}->add_pdu_CRM(@_) if Circuit->can('add_pdu_CRM');
}
#package Call;
sub add_pdu_NON {
	my $self = shift;
	$self->{circuit}->add_pdu_NON(@_) if Circuit->can('add_pdu_NON');
}
#package Call;
sub add_pdu_LLM {
	my $self = shift;
	$self->{circuit}->add_pdu_LLM(@_) if Circuit->can('add_pdu_LLM');
}
#package Call;
sub add_pdu_CAK {
	my $self = shift;
	$self->{circuit}->add_pdu_CAK(@_) if Circuit->can('add_pdu_CAK');
}
#package Call;
sub add_pdu_TCM {
	my $self = shift;
	$self->{circuit}->add_pdu_TCM(@_) if Circuit->can('add_pdu_TCM');
}
#package Call;
sub add_pdu_MCP {
	my $self = shift;
	$self->{circuit}->add_pdu_MCP(@_) if Circuit->can('add_pdu_MCP');
}

# -------------------------------------
package Service; our @ISA = qw(Path::Duplex Root Servicestate XactStats);
# -------------------------------------
#package Service;
sub type { 'Relation' }

# -------------------------------------
package Transact; our @ISA = qw(Tree Servicestate XactStats);
# -------------------------------------
#package Transact;
sub xform {
	my ($type,$self) = @_;
	Item::xform(@_);
}

# -------------------------------------
package Transact::Queries; our @ISA = qw(Transact);
# -------------------------------------
# Transact::Queries is a characterisation of a transaction route.  The
# transactions that follow this route are queries only.
# -------------------------------------

# -------------------------------------
package Transact::Replies; our @ISA = qw(Transact);
# -------------------------------------
# Transact::Replies is a characterisation of a transaction route.  The
# transactions that follow this route are replies only.
# -------------------------------------

# -------------------------------------
package Transact::Both; our @ISA = qw(Transact);
# -------------------------------------
# Transact::Both is a characterisation of a transaction route.  Both queries and
# replies follow this transaction route.
# -------------------------------------

# -------------------------------------
package Combined; our @ISA = qw(Path::Duplex Root Linkstate MsgStats);
# -------------------------------------

# -------------------------------------
package Linkset; our @ISA = qw(Path::Duplex Root Subtype Linkstate MsgStats);
# -------------------------------------
#package Linkset;
sub type { 'Relation' }

# -------------------------------------
package Link; our @ISA = qw(Path Tree Linkstate MsgStats);
# -------------------------------------
#package Link;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	return	Path->key(@_).pack('C',$msg->{slc});
}
#package Link;
sub keys {
	my $type = shift;
	my ($network,$nodea,$nodeb,$msg) = @_;
	my $slc = pack('C',$msg->{slc});
	return	Path->key($network,$nodea,$nodeb,$msg).$slc,
		Path->key($network,$nodeb,$nodea,$msg).$slc;
}
#package Link;
sub skey {
	my ($type,$network,$nodea,$nodeb,$msg) = @_;
	return 'Linkset',$msg->{slc};
}

# -------------------------------------
package Datalink; our @ISA = qw(Decoder Path Leaf Linkstate MsgStats MsgBuffer);
# -------------------------------------
#package Datalink;
sub skey {
	my $type = shift;
	return 'Link',$type->key(@_);
}
#package Datalink;
sub only {
	my $self = shift;
	$self->_init('Path',@_);
	#$self->_init('Leaf',@_); # skip this
	$self->_init('Linkstate',@_);
	$self->_init('MsgStats',@_);
	$self->_init('MsgBuffer',@_);
	$self->init(@_);
}
#package Datalink;
sub init {
	my ($type,$network,$nodea,$nodeb,$msg) = @_;
	$self->{ht} = $msg->{ht};
	$self->{ts} = $msg->{hdr};
	$self->{pr} = ::MP_UNKNOWN;
	$self->{rt} = ::RT_UNKNOWN;
	$self->{dx} = ::DX_UNKNOWN;
}
#package Datalink;
sub fini {
	my $self = shift;
	foreach (qw(ht ts pr rt)) { delete $self->{$_} }
}
#package Datalink;
sub detecting {
	my $self = shift;
	return $self->{ht} == ::HT_UNKNOWN or
	       $self->{pr} == ::MP_UNKNOWN or
	       $self->{rt} == ::RT_UNKNOWN or
	       $self->{dx} == ::DX_UNKNOWN;
}
#package Datalink;
sub add_msg {
	my $self = shift;
	$self->SUPER::add_msg(@_);
	my ($msg,$dir,$noprop) = @_;
	if ($msg->isa('Message::MTP3')) {
		my $network = $self->{network};
		my $sidea = $dir ? 'b' : 'a';
		my $sideb = $dir ? 'a' : 'b';
		my $dira = $dir;
		my $dirb = $dir ^ 0x1;
		Route->get($network,$self->{obj}->{$sidea},$msg->{opc},$msg)->add_msg($msg,$dira,1);
		Route->get($network,$self->{obj}->{$sideb},$msg->{dpc},$msg)->add_msg($msg,$dirb,1);
		Routeset->get($network,$msg->{opc},$msg->{dpc},$msg)->add_msg($msg);
	}
}
#package Datalink;
sub add_pdu { # Detecting version
	my ($self,$pdu,$dir,@args) = @_;
	my $meth = 'add_pdu_'.(split(/::/,ref $pdu))[-1];
	return $sub->(@_) if my $sub = $self->can($meth);
	return if $pdu->detect($self,$dir,@args) < 0;
	$pdu->{dir} = $dir;
	$self->MsgBuffer::push($pdu);
	return if $self->detecting;
	bless $self,ref($self).'::Detected';
	while ($pdu = $self->MsgBuffer::unshift()) {
		$self->add_pdu($pdu,$pdu->{dir},@args);
	}
}
#package Datalink;
use constant {
	DX_TYPES=>{
		&::DX_UNKNOWN=>'unknown',
		&::DX_HALF_DUPLEX=>'half duplex',
		&::DX_FULL_DUPLEX=>'full duplex',
	}
};
sub setdx {
	my ($self,$dx) = @_;
	$self->log('Duplex changed from '.DX_TYPES->{$self->{dx}}.' to '.DX_TYPES->{$dx},$self) if $self->{dx} != $dx;
	$self->{dx} = $dx;
}
#package Datalink;
use constant {
	HT_TYPES=>{
		&::HT_UNKNOWN=>'unknown',
		&::HT_BASIC=>'basic',
		&::HT_EXTENDED=>'extended',
		&::HT_NONE=>'none',
	}
};
sub setht {
	my ($self,$ht) = @_;
	$self->log('Header type changed from '.HT_TYPES->{$self->{dx}}.' to '.HT_TYPES->{$dx},$self) if $self->{dx} != $dx;
	$self->{ht} = $ht;
}
#package Datalink;
use constant {
	MP_TYPES=>{
		&::MP_UNKNOWN=>'unknown',
		&::MP_JAPAN=>'japan',
		&::MP_NATIONAL=>'national',
		&::MP_INTERNATIONAL=>'international',
		&::MP_NONE=>'none',
	}
};
sub setpr {
	my ($self,$pr) = @_;
	$self->log('Msg priority changed from '.MP_TYPES->{$self->{dx}}.' to '.MP_TYPES->{$dx},$self) if $self->{dx} != $dx;
	$self->{pr} = $pr;
}
#package Datalink;
use constant {
	RT_TYPES=>{
		&::RT_UNKNOWN=>'unknown',
		&::RT_14BIT_PC=>'14-bit point codes',
		&::RT_24BIT_PC=>'24-bit point codes',
		&::RT_NONE=>'none',
	}
};
sub setrt {
	my ($self,$rt) = @_;
	$self->log('Routing label changed from '.RT_TYPES->{$self->{dx}}.' to '.RT_TYPES->{$dx},$self) if $self->{dx} != $dx;
	$self->{rt} = $rt;
}
#package Datalink;
#
# COO/COA/ECO/ECA, CBD/CBA, LLT/LRT/LIA/LUA says that the link {slc} between
# {opc} and {dpc} is out of service, restored, inhibited.  That is not
# necessarily the current datalink so we cannot do much more than identify the
# link and adjust its state.
#
sub add_SLM {
	my ($self,$state,$pdu,$dir) = @_;
	my $link = Link->get($self->{network},$pdu->{opc},$pdu->{dpc},$pdu);
	$link->setstate($state) if defined $state;
	$self->add_msg($pdu,$dir);

}
sub add_pdu_COO { shift->add_SLM(Linkstate::TS_OUT_OF_SERVICE,	@_) }
sub add_pdu_COA { shift->add_SLM(Linkstate::TS_OUT_OF_SERVICE,	@_) }
sub add_pdu_ECO { shift->add_SLM(Linkstate::TS_OUT_OF_SERVICE,	@_) }
sub add_pdu_ECA { shift->add_SLM(Linkstate::TS_OUT_OF_SERVICE,	@_) }
sub add_pdu_CBD { shift->add_SLM(Linkstate::TS_IN_SERVICE,	@_) }
sub add_pdu_CBA { shift->add_SLM(Linkstate::TS_IN_SERVICE,	@_) }
sub add_pdu_LLT { shift->add_SLM(Linkstate::LS_INHIBITED,	@_) }
sub add_pdu_LRT { shift->add_SLM(Linkstate::LS_INHIBITED,	@_) }
sub add_pdu_LIA { shift->add_SLM(Linkstate::LS_INHIBITED,	@_) }
sub add_pdu_LUA { shift->add_SLM(Linkstate::LS_IN_SERVICE,	@_) }
sub add_pdu_LIN { shift->add_SLM(undef,				@_) }
sub add_pdu_LFU { shift->add_SLM(undef,				@_) }
sub add_pdu_LUN { shift->add_SLM(undef,				@_) }
sub add_pdu_LID { shift->add_SLM(undef,				@_) }

#package Datalink;
#
# TRA/TRW says that {opc} has restarted/is restarting and that {opc} is adjacent
# to {dpc} on the linkset upon which the TRA/TRW message was sent.  These
# messages can be used to bind the nodes at each end of a datalink if it has not
# been done already.  TRA/TRW also says that there exists a linkset between the
# two signalling points.
#
sub add_TRM {
	my ($self,$state) = (shift,shift);
	my ($pdu,$dir) = @_;
	my ($obja,$objb) = ($self->objs);
	if ($dir) {
		# will merge nodes as necessary
		$obja->add_key($pdu->{dpc});
		$objb->add_key($pdu->{opc});
		$objb->setstate($state);
	} else {
		# will merge nodes as necessary
		$objb->add_key($pdu->{dpc});
		$obja->add_key($pdu->{opc});
		$obja->setstate($state);
	}
	$obja->makereal;
	$objb->makereal;
	Linkset->get($network,$obja,$objb,$pdu)->add_msg($pdu,$dir,0) unless $self->{parent};
	$self->add_msg(@_);
}
sub add_pdu_TRA { shift->add_TRM(Spstate::SS_AVAILABLE,	@_) }
sub add_pdu_TRW { shift->add_TRM(Spstate::SS_RESTARTING,@_) }

#package Datalink;
#
# TFC says that the route to {dest} via {opc} from {dpc} is congested at level
# {stat}.  When {dest} and {opc} are the same, it was the endpoint sending the
# TFC; otherwise, {opc} is an STP.
#
sub add_pdu_TFC {
	my ($self,$pdu,$dir) = @_;
	my $network = $self->{network};
	my $state = Hexstate::HS_DEGRADED + $pdu->{stat};
	my $node = Point::SP->get($network,$pdu->{opc},$pdu);
	if ($pdu->{opc} == $pdu->{dest}) {
		$node->setstate($state);
	} else {
		Point::STP->xform($node) unless $node->isa('Point::STP');
		Routeset->get($network,$node,$pdu->{dest},$pdu)->setstate($state);
	}
	$self->add_msg($pdu,$dir);
}
#package Datalink;
#
# RCT says that the route to {dpc} from {opc} is congested at level {mp},
# however, when the routeset is no longer congested, there will be no response,
# and when it is congested another TFC will be returned, so set the state back
# to available.
#
sub add_pdu_RCT {
	my ($self,$pdu,$dir) = @_;
	my $network = $self->{network};
	#my $state = Hexstate::HS_DEGRADED + $pdu->{mp};
	my $state = Hexstate::HS_AVAILABLE;
	Routeset->get($network,$pdu->{opc},$pdu->{dpc},$pdu)->setstate($state);
	$self->add_msg($pdu,$dir);
}
#package Datalink;
#
# TFP/RST, TFR/RSR, or TFA says that the route to {dest} via {opc/dpc} is
# prohibited, restricted or available on the linkset from {dpc/opc} to
# {opc/dpc}.  It also ways that {opc/dpc} is an STP.  Because TFP, TFR, TFA are
# only sent to an adjacent signalling point (and not an alias), the destination
# of the TFP, TFR, TFA is a real (non-alias) point code.  Also, because
# TFP/RST,TFR/RSR,TFA are only sent between a pair of adjacent signalling
# points, there exists a linkset between the two signalling points.
#
sub add_TFM {
	my ($self,$stp,$adj,$state,$pdu,$dir) = @_;
	my $network = $self->{network};
	$stp = Point::STP->get($network,$stp,$pdu);
	Point::STP->xform($stp) unless $stp->isa('Point::STP');
	$adj = Point::SP->get($network,$adj,$pdu); $adj->makereal;
	Route->get($network,$stp,$pdu->{dest},$pdu)->setstate($state);
	Linkset->get($network,$adj,$stp,$pdu)->add_msg($pdu,$dir,0) unless $self->{parent};
	$self->add_msg($pdu,$dir);
}
sub add_pdu_TFP { shift->add_TFM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_UNAVAILABLE,@_); }
sub add_pdu_TFR { shift->add_TFM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_DEGRADED,@_); }
sub add_pdu_TFA { shift->add_TFM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_AVAILABLE,@_); }
sub add_pdu_RST { shift->add_TFM($_[0]->{dpc},$_[0]->{opc},Tristate::TS_UNAVAILABLE,@_); }
sub add_pdu_RSR { shift->add_TFM($_[0]->{dpc},$_[0]->{opc},Tristate::TS_DEGRADED,@_); }

#package Datalink;
#
# TCP/RCP, TCR/RCR, or TCA says that the cluster route to {dest} via {opc/dpc}
# is prohibited, restricted or available on the linkset from {dpc/opc} to
# {opc/dpc}.  It also ways that {opc/dpc} is an STP.  Because TCP, TCR, TCA are
# only sent to an adjacent signalling point (and not an alias), the destination
# of the TCP, TCR, TCA is a real (non-alias) point code.  Also, because
# TCP/RCP,TCR/RCR,TCA are only sent between a pair of adjacent signalling
# points, there exists a linkset between the two signalling points.
#
sub add_TCM {
	my ($self,$stp,$adj,$state,$pdu,$dir) = @_;
	my $network = $self->{network};
	$stp = Point::STP->get($network,$stp,$pdu);
	Point::STP->xform($stp) unless $stp->isa('Point::STP');
	$adj = Point::SP->get($network,$adj,$pdu); $adj->makereal;
	Cluster->get($network,$stp,$pdu->{dest},$pdu)->setstate($state);
	Linkset->get($network,$adj,$stp,$pdu)->add_msg($pdu,$dir,0) unless $self->{parent};
	$self->add_msg($pdu,$dir);
}
sub add_pdu_TCP { shift->add_TCM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_UNAVAILABLE,@_); }
sub add_pdu_TCR { shift->add_TCM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_DEGRADED,@_); }
sub add_pdu_TCA { shift->add_TCM($_[0]->{opc},$_[0]->{dpc},Tristate::TS_AVAILABLE,@_); }
sub add_pdu_RCP { shift->add_TCM($_[0]->{dpc},$_[0]->{opc},Tristate::TS_UNAVAILABLE,@_); }
sub add_pdu_RCR { shift->add_TCM($_[0]->{dpc},$_[0]->{opc},Tristate::TS_DEGRADED,@_); }

#package Datalink;
#
# TODO: Should probably do more with these....
#
sub add_pdu_UPU { shift->add_msg(@_) }
sub add_pdu_UPA { shift->add_msg(@_) }
sub add_pdu_UPT { shift->add_msg(@_) }

#package Datalink;
#
# Nobody uses these messages any more.
#
sub add_pdu_DLC { shift->add_msg(@_) }
sub add_pdu_CSS { shift->add_msg(@_) }
sub add_pdu_CNS { shift->add_msg(@_) }
sub add_pdu_CNP { shift->add_msg(@_) }

#package Datalink;
#
# SLTA/SLTM says that {opc} and {dpc} are on either end of the link {slc} of
# which this is a datalink.  These messages can be used to bind the nodes at
# each end of a datalink and assign the datalink to a Link if not already done
# so.
#
sub add_SNTM {
	my $self = shift;
	return $self->add_msg(@_) if $self->{parent}; # only when not already bound
	my ($pdu,$dir) = @_;
	my ($obja,$objb) = ($self->objs);
	if ($dir) {
		# will merge nodes as necessary
		$obja->add_key($pdu->{dpc});
		$objb->add_key($pdu->{opc});
	} else {
		# will merge nodes as necessary
		$obja->add_key($pdu->{opc});
		$objb->add_key($pdu->{dpc});
	}
	$obja->makereal;
	$objb->makereal;
	$self->Leaf::init($self->{network},$pdu);
	$self->add_msg(@_);
}
sub add_pdu_SLTM { shift->add_SNTM(@_) }
sub add_pdu_SLTA { shift->add_SNTM(@_) }


# -------------------------------------
package Datalink::Channel; our @ISA = qw(Datalink);
# -------------------------------------
#package Datalink::Channel;
use constant {
	MTP2_ANNEX_A_NOT_USED	    => 0,
	MTP2_ANNEX_A_USED	    => 1,
	MTP2_ANNEX_A_USED_UNKNOWN   => 2,
};
#package Datalink::Channel;
sub key {
	my $type = shift;
	my ($network,$msg) = @_;
	return pack('n',$msg->{ppa});
}
#package Datalink::Channel;
sub ppa {
	my ($card,$span,$slot) = @_;
	return (($card & 0x03) << 7) | (($span & 0x03) << 5) | (($slot & 0x1f) << 0);
}
#package Datalink::Channel;
sub chan {
	my $ppa = shift;
	return (($ppa >> 0) & 0x1f), (($ppa >> 5) & 0x03), (($ppa >> 7) & 0x03);
}
#package Datalink::Channel;
sub keys {
	my $type = shift;
	my ($network,$msg) = @_;
	my ($way,$ppa) = ($msg->{way},$msg->{ppa});
	if ($way & 0x2) {
		# monitored half-duplex channel in a known direction
		my ($card,$span,$slot) = Datalink::Channel::chan($ppa);
		# tell messages from the other span how to find us
		my $ppa2 = Datalink::Channel::ppa($card,$span^0x1,$slot);
		($ppa,$ppa2) = ($ppa2,$ppa) unless $way & 0x1;
		return pack('n',$ppa), pack('n',$ppa2);
	}
	return pack('n',$ppa);
}
#package Datalink::Channel;
sub keypair {
	my ($type,$network,$msg) = @_;
	return $msg->{ppa}, $msg->{ppa};
}
#package Datalink::Channel;
sub objpair {
	my $type = shift;
	my ($network,$msg) = @_;
	return Point::Node->new($network,undef,$msg), Point::Node->new($network,undef,$msg);
}
#package Datalink::Channel;
sub init {
	my $self = shift;
	my ($network,$msg) = @_;
	my $ppa = $self->{ppa} = $msg->{ppa};
	my $way = $self->{way} = $msg->{way};
	my $xsn = $self->{xsn} = $msg->{xsn};
	my ($card,$span,$slot) = Datalink::Channel::chan($ppa);
	$self->{ht} = ::HT_BASIC if $slot != 0;
	$self->{ht} = [ ::HT_UNKNOWN, ::HT_BASIC, ::HT_EXTENDED ]->[$self->{xsn}] if $self->{ht} == ::HT_UNKNOWN;
	$self->{dx} = ::DX_HALF_DUPLEX if $msg->{dir} & 0x6;
	bless $self,ref($self).'::Detected' unless $self->detecting;
}
#package Datalink::Channel;
sub fini {
	my $self = shift;
	delete $self->{ppa};
}
#package Datalink::Channel;
sub identify {
	my $self = shift;
	my ($card,$span,$slot) = Datalink::Channel::chan($self->{dli});
	my $id = "Channel $card:$span:$slot";
	$id .= ", $self->obja->{pcode} -> $self->objb->{pcode} link $self->{slc}"
		if $self->obja and $self->obja->isa('Point::SP');
	return ($self->{id} = $id);
}
#package Datalink::Channel;
sub shortid {
	my $self = shift;
	my ($card,$span,$slot) = Datalink::Channel::chan($self->{dli});
	return "$card:$span:$slot";
}
# -------------------------------------
package Datalink::Channel::Detected; our @ISA = qw(Datalink::Channel);
# -------------------------------------
# The only difference between the base package and the Detected package is that
# the base package uses a more extensive add_pdu() method to perform the
# detection and otherwise queue messages until detection occurs.
# -------------------------------------
#package Datalink::Channel::Detected;
sub add_pdu {
	shift->Decoder::add_pdu(@_);
}
#package Datalink::Channel::Detected;
sub add_pdu_FISU {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIO {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_ALIGNING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIN {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_PROVING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIE {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_PROVING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIOS {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIPO {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUTAGE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_SIB {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_CONGESTED);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Channel::Detected;
sub add_pdu_MSU {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
	return unless my $sdu = $pdu->decode($self);
	$self->add_pdu($sdu,$dir);
}


# -------------------------------------
package Bind; our @ISA = qw(Path Tristate MsgStats);
# -------------------------------------
# A Bind is a path between a host and a node (for a protocol).  It models the
# AS/ASP and SG/SGP relationship where the AS or SG are signalling points and
# the ASP/SGP are internet hosts.  With this approach we never need to merge
# hosts and nodes.
#
# For M2PA and M2UA, creating a new Node a the end of every new bind is ok
# because eventually SLTM/SLTA messages are going to identify the adjacent
# signalling points and they will be merged into these unidentified nodes at the
# ends of the datalink.  This is precisely how we do it for channels now (except
# that no Bind structure is necessary because the nodes are directly attached to
# the channels).
# 
# Note that for M3UA and SUA, the signalling nodes at the end of bind remain as
# unidentified adjacent signalling points and can therefore be characterized as
# AS, SG or SP.   The problem becomes trying to associate ASP with an AS, SGP
# with an SG and IPSP with an SP.  We do know that numbering of RCs is
# consistent across SGP belonging to the same SG and between SG/STP.  For a
# full-mesh, two ASP that connect to the same SGP must belong belong to the same
# AS for RC values that traverse the associations.  We also know that the only
# rational basis for loadshare in signalling link selection for M3UA and
# sequence control for SUA.  We also know that only one ASP can be active for an
# AS for override.  Unfortunately we are likely to never see ASPTM messages.
# -------------------------------------
#package Bind;
sub key {
	my $type = shift;
	my ($network,$host,$msg) = @_;
	$host = $host->{no} if ref $host;
	return pack('NN',$msg->{ppi},$host).$msg->{ctx};
}
#package Bind;
sub keypair {
	my $type = shift;
	my ($network,$host,$msg,$obja,$objb) = @_;
	return (
		pack('NN',$msg->{ppi},$obja->{no}).$msg->{ctx},
		pack('NN',$msg->{ppi},$objb->{no}).$msg->{ctx},
	);
}
#package Bind;
sub objpair {
	my $type = shift;
	my ($network,$host,$msg) = @_;
	# this happens to never create a host
	$host = Point::Host->get($network,$host,$msg) unless ref $host;
	my $type;
	$type = 'Point::Node' if $host->isa('Point::SPP');
	$type = 'Point::AS'   if $host->isa('Point::ASP');
	$type = 'Point::SG'   if $host->isa('Point::SGP');
	my $node = $type->get($network,undef,$msg);
	return $node,$point;
}
#package Bind;
sub init {
	my $self = shift;
	my ($network,$host,$msg) = @_;
	$self->{ppi} = $msg->{ppi};
	$self->{ctx} = $msg->{ctx};
}
#package Bind;
sub fini {
	my $self = shift;
	delete $self->{ppi};
	delete $self->{ctx};
}

# -------------------------------------
package Bind::M2PA; our @ISA = qw(Bind);
# -------------------------------------
# M2PA is special in that it does not really have a context (RC/IID).  Only one
# signalling link per SCTP association.
# -------------------------------------
#package Bind::M2PA;
sub key {
	my $type = shift;
	my ($network,$host,$msg) = @_;
	$host = $host->{no} if ref $host;
	return pack('NN',$msg->{ppi},$host);
}
#package Bind::M2PA;
sub keypair {
	my $type = shift;
	my ($network,$host,$msg,$obja,$objb) = @_;
	return (
		pack('NN',$msg->{ppi},$obja->{no}),
		pack('NN',$msg->{ppi},$objb->{no}),
	);
}

# -------------------------------------
package Datalink::Context; our @ISA = qw(Datalink ChkBuffer);
# -------------------------------------
# This is a datalink.  Contexts exist between AS and SG (signalling points) and
# are independent of a given SCTP association.  The Datalink::Context can be
# found by using the Bind path from the hosts at either end of an SCTP
# association.  These paths are indexed by PPI and CTX (IID/RC) and lead to a
# node.
# -------------------------------------
#package Datalink::Context;
sub key {
	my $type = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$nodea = Point::Node->get($network,$nodea,$Pkt) unless ref $nodea;
	$nodeb = Point::Node->get($network,$nodeb,$Pkt) unless ref $nodeb;
	my $ctx = pack('N',$pkt->{ppi}).$pkt->{ctx};
	return $ctx.pack('NN',$nodea->{no},$nodeb->{no});
}
#package Datalink::Context;
sub keys {
	my $type = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$nodea = Point::Node->get($network,$nodea,$Pkt) unless ref $nodea;
	$nodeb = Point::Node->get($network,$nodeb,$Pkt) unless ref $nodeb;
	my $ctx = pack('N',$pkt->{ppi}).$pkt->{ctx};
	return	$ctx.pack('NN',$nodea->{no},$nodeb->{no}),
		$ctx.pack('NN',$nodeb->{no},$nodea->{no});
}
#package Datalink::Context;
sub typpair {
	return 'Point::Node','Point::Node';
}
#package Datalink::Context;
sub keypair {
	my ($type,$network,$noa,$nob,$pkt,$assoc,$nodea,$nodeb) = @_;
	my $ctx = pack('N',$pkt->{ppi}).$pkt->{ctx};
	return	$ctx.pack('N', $nodea->{no}),
		$ctx.pack('N', $nodeb->{no});
}
#package Datalink::Context;
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$self->{ppi} = $pkt->{ppi};
	$self->{ctx} = $pkt->{ctx};
}
# -------------------------------------
package Datalink::Context::M2PA; our @ISA = qw(Datalink::Context);
# -------------------------------------
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$self->{dx} = ::DX_FULL_DUPLEX;
	$self->{ht} = ::HT_EXTENDED;
	$self->{pr} = ::MP_UNKNOWN;
	$self->{rt} = ::RT_UNKNOWN;
}

# -------------------------------------
package Datalink::Context::M2PA::Detected; our @ISA = qw(Datalink::Context::M2PA);
# -------------------------------------
# The only difference between the base package and the Detected package is that
# the base package uses a more extensive add_pdu() method to perform the
# detection and otherwise queue messages until detection occurs.
# -------------------------------------
#package Datalink::Context::M2PA::Detected;
sub add_pdu {
	return shift->Decoder::add_pdu(@_);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_ACK { # FISU
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_ALIGNMENT { # SIO
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_ALIGNING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_PROVING_NORMAL { # SIN
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_PROVING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_PROVING_EMERGENCY { # SIE
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_PROVING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_IN_SERVICE { # FISU
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_PROCESSOR_OUTAGE { # SIPO
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUTAGE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_PROCESSOR_OUTAGE_ENDED { # SIPO stops
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_BUSY { # SIB
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_CONGESTED);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_BUSY_ENDED { # SIB stops
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_OUT_OF_SERVICE { # SIOS
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2PA::Detected;
sub add_pdu_DATA {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
	return unless my $sdu = $pdu->decode($self);
	$self->add_pdu($sdu,$dir);
}

# -------------------------------------
package Datalink::Context::M2UA; our @ISA = qw(Datalink::Context);
# -------------------------------------
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$self->{dx} = ::DX_FULL_DUPLEX;
	$self->{ht} = ::HT_NONE;
	$self->{pr} = ::MP_UNKNOWN;
	$self->{rt} = ::RT_UNKNOWN;
}

# -------------------------------------
package Datalink::Context::M2UA::Detected; our @ISA = qw(Datalink::Context::M2UA);
# -------------------------------------
# The only difference between the base package and the Detected package is that
# the base package uses a more extensive add_pdu() method to perform the
# detection and otherwise queue messages until detection occurs.
# -------------------------------------
#package Datalink::Context::M2UA::Detected;
sub add_pdu {
	return shift->Decoder::add_pdu(@);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
	return unless my $sdu = $pdu->decode($self);
	$self->add_pdu($sdu,$dir);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Establish_Request {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_ALIGNING);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Establish_Confirm {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Release_Request {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Release_Confirm {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Release_Indication {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_State_Request {
	my ($self,$pdu,$dir) = @_;
	     if ($pdu->{state} == 0x0) { # STATUS_LPO_SET
		$self->setstate(Linkstate::LS_OUTAGE);
	} elsif ($pdu->{state} == 0x1) { # STATUS_LPO_CLEAR
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x2) { # STATUS_EMER_SET
	} elsif ($pdu->{state} == 0x3) { # STATUS_EMER_CLEAR
	} elsif ($pdu->{state} == 0x4) { # STATUS_FLUSH_BUFFERS
	} elsif ($pdu->{state} == 0x5) { # STATUS_CONTINUE
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x6) { # STATUS_CLEAR_RTB
	} elsif ($pdu->{state} == 0x7) { # STATUS_AUDIT
	} elsif ($pdu->{state} == 0x8) { # STATUS_CONG_CLEAR
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x9) { # STATUS_CONG_ACCEPT
		$self->setstate(Linkstate::LS_CONGESTED);
	} elsif ($pdu->{state} == 0xa) { # STATUS_CONG_DISCARD
		$self->setstate(Linkstate::LS_CONGESTED);
	}
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_State_Confirm {
	my ($self,$pdu,$dir) = @_;
	my ($self,$pdu,$dir) = @_;
	     if ($pdu->{state} == 0x0) { # STATUS_LPO_SET
		$self->setstate(Linkstate::LS_OUTAGE);
	} elsif ($pdu->{state} == 0x1) { # STATUS_LPO_CLEAR
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x2) { # STATUS_EMER_SET
	} elsif ($pdu->{state} == 0x3) { # STATUS_EMER_CLEAR
	} elsif ($pdu->{state} == 0x4) { # STATUS_FLUSH_BUFFERS
	} elsif ($pdu->{state} == 0x5) { # STATUS_CONTINUE
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x6) { # STATUS_CLEAR_RTB
	} elsif ($pdu->{state} == 0x7) { # STATUS_AUDIT
	} elsif ($pdu->{state} == 0x8) { # STATUS_CONG_CLEAR
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x9) { # STATUS_CONG_ACCEPT
		$self->setstate(Linkstate::LS_CONGESTED);
	} elsif ($pdu->{state} == 0xa) { # STATUS_CONG_DISCARD
		$self->setstate(Linkstate::LS_CONGESTED);
	}
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_State_Indication {
	my ($self,$pdu,$dir) = @_;
	if ($pdu->{state} == 0x1) { # EVENT_RPO_ENTER
		$self->setstate(Linkstate::LS_OUTAGE);
	} elsif ($pdu->{state} == 0x2) { # EVENT_RPO_EXIT
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{state} == 0x3) { # EVENT_LPO_ENTER
		$self->setstate(Linkstate::LS_OUTAGE);
	} elsif ($pdu->{state} == 0x4) { # EVENT_LPO_EXIT
		$self->setstate(Linkstate::LS_IN_SERVICE);
	}
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data_Retrieval_Request {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data_Retrieval_Confirm {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data_Retrieval_Indication {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data_Retrieval_Complete_Indication {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_OUT_OF_SERVICE);
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Congestion_Indication {
	my ($self,$pdu,$dir) = @_;
	if ($pdu->{congest} eq 0x0) { # LEVEL_NONE
		$self->setstate(Linkstate::LS_IN_SERVICE);
	} elsif ($pdu->{congest} eq 0x1) { # LEVEL_1
		$self->setstate(Linkstate::LS_CONGESTED);
	} elsif ($pdu->{congest} eq 0x2) { # LEVEL_2
		$self->setstate(Linkstate::LS_CONGESTED);
	} elsif ($pdu->{congest} eq 0x3) { # LEVEL_3
		$self->setstate(Linkstate::LS_CONGESTED);
	}
	$self->add_msg($pdu,$dir,0);
}
#package Datalink::Context::M2UA::Detected;
sub add_pdu_Data_Acknowledge {
	my ($self,$pdu,$dir) = @_;
	$self->setstate(Linkstate::LS_IN_SERVICE);
	$self->add_msg($pdu,$dir,0);
}

# -------------------------------------
package Datalink::Context::M3UA; our @ISA = qw(Datalink::Context);
# -------------------------------------
#package Datalink::Context::M3UA;
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$self->{dx} = ::DX_FULL_DUPLEX;
	$self->{ht} = ::HT_NONE;
	$self->{pr} = ::MP_NONE;
	$self->{rt} = ::RT_UNKNOWN;
}
# -------------------------------------
package Datalink::Context::M3UA::Detected; our @ISA = qw(Datalink::Context::M3UA);
# -------------------------------------
# The only difference between the base package and the Detected package is that
# the base package uses a more extensive add_pdu() method to perform the
# detection and otherwise queue messages until detection occurs.
# -------------------------------------
#package Datalink::Context::M3UA::Detected;
sub add_pdu {
	shift->Decoder::add_pdu(@_);
}
#package Datalink::Context::M3UA::Detected;
sub add_pdu_DATA {
}
#package Datalink::Context::M3UA::Detected;
#
# Interesting fact is that if a DUNA/DAVA/DRST/SCON is sent in the SG/STP
# configuraiton that it also applies to the SG/STP mate (unless in the
# unexpected case that the C-links are down).  Therefore, we can mark all of the
# routesets between all of the point codes to which the AS has a route and each
# of the affected point code(s) with the appropriate status.
#
# DRST can be sent by one SG/STP only and does not concern its mate.  However,
# DRST is only sent when one of the pair cannot reach the destination directly.
# This always indicates a degraded routeset from all of the point codes to which
# the AS has a route and the affected point code(s).  So we mark those routesets
# degraded.
#
sub add_from_SG {
	my ($self,$state,$pdu) = @_;
	my $network = $self->{network};
	foreach my $route (values %{$self->obja->{end}->{a}->{routes}}) {
		my $nodea = $route->objb;
		foreach my $pair (@{$pdu->{apcs}}) {
			my ($msk,$spc) = @$pair;
			if ($msk == 0) { # point code
				# just set routeset to $spc
				my $nodeb = Point::SP->get($network,$spc,$pdu);
				next unless my $routeset = Routeset->find($network,$nodea,$nodeb,$pdu);
				$routeset->setstate($state);
			} elsif ($msk == 8 or $msg == 3) { # cluster/region code
				# set all routesets to point codes in the cluster
				my $clus = Point::Cluster->get($network,$spc,$pdu);
				foreach my $nodeb ($clus->children) {
					next unless my $routeset = Routeset->find($network,$nodea,$nodeb,$pdu);
					$routeset->setstate($state);
				}
			} elsif ($msg >= 14) {
				# wildcard, set all routesets
				foreach my $side (qw/a b/) {
					foreach my $routeset (values %{$nodea->{end}->{$side}->{routesets}}) {
						$routeset->setstate($state);
					}
				}
			} else {
				# arggggh!
			}
		}
	}
}
#package Datalink::Context::M3UA::Detected;
sub add_from_AS {
	my ($self,$state,$pdu) = @_;
	my $network = $self->{network};
	my $node = Point::SP->get($network,$pdu->{cdest},$pdu);
	$node->setstate($state);
}
#package Datalink::Context::M3UA::Detected;
sub add_pdu_DUNA {
	shift->add_from_SG(Hexstate::HS_UNAVAILABLE,$state,@_);
}
#package Datalink::Context::M3UA::Detected;
sub add_pdu_DAVA {
	shift->add_from_SG(Hexstate::HS_AVAILABLE,$state,@_);
}
#package Datalink::Context::M3UA::Detected;
#
# There is nothing really to do here: the SG will respond with
# DUNA/DAVA/DRST/SCON as necessary.  We use those messages to affect state
# changes.
#
sub add_pdu_DAUD {
	my $self = shift;
	my ($pdu) = @_;
}
#package Datalink::Context::M3UA::Detected;
#
# SCON is normally only sent from an SGP to an ASP and never between IPSP;
# however, under special circumstances a SCON can be sent from an ASP to an SGP.
# The optional Concerned DPC parameter is only used if the SCON message is sent
# from an ASP to an SGP.  It contains the point code of the originator of the
# message that triggered the SCON message.
#
sub add_pdu_SCON {
	my $self = shift;
	my ($pdu) = @_;
	if (exists $pdu->{cdest}) {
		$self->add_from_AS( Spstate::SS_DEGRADED+$pdu->{cong},@_);
	} else {
		$self->add_from_SG(Hexstate::HS_DEGRADED+$pdu->{cong},@_);
	}
}
#package Datalink::Context::M3UA::Detected;
#
# DUPU is only sent from an SGP to an ASP and never between IPSP.  This only
# affects the state of the user-part at the affected point code.
#
sub add_pdu_DUPU {
	my $self = shift;
	my ($pdu) = @_;
	#FIXME: do more
}
#package Datalink::Context::M3UA::Detected;
sub add_pdu_DRST {
	shift->add_from_SG(Hexstate::HS_DEGRADED,$state,@_);
}

# -------------------------------------
package Datalink::Context::SUA; our @ISA = qw(Datalink::Context);
# -------------------------------------
#package Datalink::Context::SUA;
sub init {
	my $self = shift;
	my ($network,$nodea,$nodeb,$pkt) = @_;
	$self->{dx} = ::DX_FULL_DUPLEX;
	$self->{ht} = ::HT_NONE;
	$self->{pr} = ::MP_NONE;
	$self->{rt} = ::RT_NONE;
}
#package Datalink::Context::SUA;
sub add_pdu {
	# Note that SUA datalinks are always fully detected at creation.
	my ($self,$pdu) = @_;
	$self->Decoder::add_pdu($pdu);
}
#package Datalink::Context::SUA;
sub add_pdu_CLDT {
}
#package Datalink::Context::SUA;
sub add_pdu_CLDR {
}
#package Datalink::Context::SUA;
sub add_pdu_CORE {
}
#package Datalink::Context::SUA;
sub add_pdu_COAK {
}
#package Datalink::Context::SUA;
sub add_pdu_COREF {
}
#package Datalink::Context::SUA;
sub add_pdu_RELRE {
}
#package Datalink::Context::SUA;
sub add_pdu_RELCO {
}
#package Datalink::Context::SUA;
sub add_pdu_RESCO {
}
#package Datalink::Context::SUA;
sub add_pdu_RESRE {
}
#package Datalink::Context::SUA;
sub add_pdu_CODT {
}
#package Datalink::Context::SUA;
sub add_pdu_CODA {
}
#package Datalink::Context::SUA;
sub add_pdu_COERR {
}
#package Datalink::Context::SUA;
sub add_pdu_COIT {
}

# -------------------------------------
package Peering; our @ISA = qw(Path::Duplex Root PktStats);
# -------------------------------------
# A peering is a relationship between hosts that communicate with each other.
# This object is used to trace these interactions, primarily for identifying the
# role of hosts.
# -------------------------------------
#package Peering;
sub typpair {
	return 'Point::Host','Point::Host';
}

# -------------------------------------
package Flow; our @ISA = qw(Path Tristate PktStats);
# -------------------------------------
# A Flow is a path between a Point::Station and a Point::Host.  It  models the
# Point::Station to Point::Host relationship where the Point::Station is an
# ethernet station and the Point::Host is an IP host.  It is similar to a
# 'Route' but for Ethernet not SS7.  Also, a Flow is bidirectional whereas a
# Route is not.
# -------------------------------------
#package Flow;
sub typpair {
	return 'Point::Station','Point::Host';
}

# -------------------------------------
package Stream;
# -------------------------------------
#package Stream;
sub init {
	my $self = shift;
	my ($network,$assoc,$dir,$chunk) = @_;
	$self->{assoc} = $assoc;
	$self->{chunks} = [];
	$assoc->{streams}->{$dir}->{$chunk->{stream}} = $self;
	return $self;
}

sub add_chunk {
	my $self = shift;
	my ($network,$assoc,$dir,$oos,$chunk) = @_;
	return if $chunk->{flags} & 0x4;
	my $beg = (($chunk->{flags} & 0x2) != 0);
	my $end = (($chunk->{flags} & 0x1) != 0);
	if ($beg) {
		$self->{chunks} = [];
		$self->{ssn} = $chunk->{ssn};
		if ($end) {
			my $msg = Message::ULP->new($network,$assoc,$dir,[$chunk]);
			$msg->unpack($network,$assoc,$dir);
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
		my $msg = Message::ULP->new($network,$assoc,$dir,$self->{chunks});
		$msg->unpack($network,$assoc,$dir);
		$self->{chunks} = [];
	}
}

# -------------------------------------
package Association; our @ISA = qw(Decoder::SCTP Path Tree PktStats PktBuffer);
# -------------------------------------
#package Association;
sub key {
	my $type = shift;
	my ($network,$pkt) = @_;
	return pack('NnnN', $pkt->{ppi}, $pkt->{sport},$pkt->{dport},$pkt->{vtag});
}
#package Association;
sub keys {
	my $type = shift;
	my ($network,$pkt) = @_;
	return	pack ('NnnN', $pkt->{ppi},$pkt->{sport},$pkt->{dport},$pkt->{vtag}),
		pack ('NnnN', $pkt->{ppi},$pkt->{dport},$pkt->{sport},$pkt->{vtag});
}
#package Association;
sub pkey {
	my $self = shift;
	my ($network,$pkt) = @_;
	my $ip = $pkt->{pdu};
	my $parent = Peering->get($network,$ip->{saddr},$ip->{daddr},$pkt);
	my $dir = $self->obja eq $parent->objb ? 1 : 0;
	return $parent,$dir,$pkt->{vtag};
}
#package Association;
use constant {
	PPI_TO_PORT=>{
		&::PPI_M2PA => ::PORT_M2PA,
		&::PPI_M2UA => ::PORT_M2UA,
		&::PPI_M3UA => ::PORT_M3UA,
		&::PPI_SUA  => ::PORT_SUA,
	},
};
#package Association;
#
# We always want to create associations with the ASP as $hosta and the SGP as
# $hostb.  This way Datalink::Contexts will always be created with the AS as
# $hosta and the SG as $hostb.
#
sub objpair {
	my $type = shift;
	my ($network,$pkt) = @_;
	my $ip = $pkt->{pdu};
	my ($adda,$addb);
	if ($pkt->{sport} == PPI_TO_PORT->{$pkt->{ppi}}) {
		$adda = $ip->{daddr};
		$addb = $ip->{saddr};
	} else {
		$adda = $ip->{saddr};
		$addb = $ip->{daddr};
	}
	my ($hosta,$hostb);
	if ($pkt->{sport} == $pkt->{dport}) {
		$hosta = Point::SPP->get($network,$adda,$pkt);
		$hostb = Point::SPP->get($network,$addb,$pkt);
	} else {
		$hosta = Point::ASP->get($network,$adda,$pkt);
		$hostb = Point::SGP->get($network,$addb,$pkt);
	}
	return $hosta,$hostb;
}
#package Association;
sub add_pdu_SCTP {
	#
	# Direct entry (does not come through add_pdu() but directly called from
	# the Network package's add_pdu_SCTP() method.
	#
	my $self = shift;
	my ($pkt) = @_;
	my $ip = $pkt->{pdu};
	if ($self->{sport} != $self->{dport}) {
		$pkt->{dir} = ($pkt->{sport} == $self->{dport}) ? 1 : 0;
	} else {
		my $hosta = Host->get($self->{network},$ip->{saddr},$ip);
		my $hostb = Host->get($self->{network},$ip->{daddr},$ip);
		if ($self->obja eq $hosta or $self->objb eq $hostb) {
			$pkt->{dir} = 0;
		} elsif ($self->objb eq $hosta or $self->obja eq $hostb) {
			$pkt->{dir} = 1;
		} else {
#
#	This might happen all the time for M2PA.  The problem there is that the
#	source and destination ports are usually the same and the SCTP header
#	offers no help in determining the direction of the message.  Because any
#	new IP address of a multihomed host cannot be attributed to the host at
#	either end by direction, the data could become unprocessable.  The
#	approach taken here is to use the decoder to test the TSNs or
#	cummulative acks in the chunks contained in the message to determine
#	whether the message rationally fits one direction and not another.
#	Otherwise, we must discard the message.  Once we discover that two IP
#	addresses belong to the same Point::Host, the Point::Host objects are
#	merged and one of the rules above will work instead.
#
			return unless $self->SUPER::tst_pdu_SCTP(@_);
			# will set $pkt->{dir} on success
		}
	}
	$self->SUPER::add_pdu_SCTP(@_); # Decoder::SCTP
	$self->add_pkt($self->{network},$pkt,$pkt->{dir},0);
}
#package Association;
sub add_pdu_Context {
	my ($self,$pdu) = @_;
	my $network = $self->{network};
	my $type = ref $self; $type =~ s/Association/Datalink::Context/;
	my $dir = $pdu->{dir};
	foreach $pdu->{ctx} (@{$pdu->{ctxs}}) {
		my $nodea = Bind->get($network,$self->obja,$pdu)->obja;
		my $nodeb = Bind->get($network,$self->objb,$pdu)->obja;
		my $datalink = $type->get($network,$nodea,$nodeb,$pdu,$self)
		$dir ^= 0x1 if $nodea eq $datalink->objb;
		$datalink->add_pdu($pdu,$dir);
	}
}
#package Association;
sub add_pdu {
	# called by Decoder::Stream with ULP pdu
	my ($self,$pdu) = @_;
	return $self->add_pdu_Context($pdu) if exists $pdu->{ctxs};
	return $self->SUPER::add_pdu($pdu);
}

#package Association;
sub add_pdu_ERR { }
#package Association;
sub add_pdu_NTFY { }
#package Association;
sub add_pdu_ASPUP_REQ { }
#package Association;
sub add_pdu_ASPDN_REQ { }
#package Association;
sub add_pdu_HBEAT_REQ { }
#package Association;
sub add_pdu_ASPUP_ACK { }
#package Association;
sub add_pdu_ASPDN_ACK { }
#package Association;
sub add_pdu_HBEAT_ACK { }
#package Association;
sub add_pdu_ASPAC_REQ { }
#package Association;
sub add_pdu_ASPIA_REQ { }
#package Association;
sub add_pdu_ASPAC_ACK { }
#package Association;
sub add_pdu_ASPIA_ACK { }

# -------------------------------------
package Association::M2PA; our @ISA = qw(Association);
# -------------------------------------
#package Association::M2PA;
sub add_pdu_Context {
	my ($self,$pdu) = @_;
	my $network = $self->{network};
	my $dir = $pdu->{dir};
	my $nodea = Bind::M2PA->get($network,$self->obja,$pdu)->obja;
	my $nodeb = Bind::M2PA->get($network,$self->objb,$pdu)->obja;
	my $datalink = Datalink::Context::M2PA->get($network,$nodea,$nodeb,$pdu,$self);
	$dir ^= 0x1 if $nodea eq $datalink->objb;
	$datalink->add_pdu($pdu,$dir);
}
#package Association::M2PA;
#
# All M2PA messages are context based
#
sub add_pdu { shift->add_pdu_Context(@_) }

# -------------------------------------
package Association::M2UA; our @ISA = qw(Association);
# -------------------------------------
#package Association::M2UA;
sub add_pdu_REG_REQ { }
#package Association::M2UA;
sub add_pdu_REG_RSP { }
#package Association::M2UA;
sub add_pdu_DEREG_REQ { }
#package Association::M2UA;
sub add_pdu_DEREG_RSP { }


# -------------------------------------
package Association::M3UA; our @ISA = qw(Association);
# -------------------------------------
#package Association::M3UA;
sub add_pdu_REG_REQ { }
#package Association::M3UA;
sub add_pdu_REG_RSP { }
#package Association::M3UA;
sub add_pdu_DEREG_REQ { }
#package Association::M3UA;
sub add_pdu_DEREG_RSP { }

# -------------------------------------
package Association::SUA; our @ISA = qw(Association);
# -------------------------------------
#package Association::SUA;
sub add_pdu_REG_REQ { }
#package Association::SUA;
sub add_pdu_REG_RSP { }
#package Association::SUA;
sub add_pdu_DEREG_REQ { }
#package Association::SUA;
sub add_pdu_DEREG_RSP { }

# -------------------------------------
package Lan; our @ISA = qw(Point Root PktStats);
# -------------------------------------
# An Lan is a relationship between any number of Port packages.  All ports that
# share a hardware address with a Lan are on that LAN.
#
# When we add a packet to a Lan both hardware addresses are discovered to also
# belong to the same Lan.  The Lan was looked up by source hardware address, so
# we add the destination hardware address when it is available.  A destination
# hardware addresses is not available when the packet comes from a Linux cooked
# header capture.  When another Lan exists that already has the destination
# hardware address as one of its addresses, the two Lan packages will be merged.
# In this way membership of hardware addresses (Port and Point::Station
# packages) to Lan packages is discovered.
# -------------------------------------
#package Lan;
sub key {
	my $type = shift;
	my ($network,$hwa,$pkt) = @_;
	$hwa = $hwa->{hwa} if ref $hwa;
}
#package Lan;
sub add_pkt {
	my $self = shift;
	my ($pkt,$dir,$noprop) = @_;
	$self->add_key($pkt->{dhwa}) if $pkt->{dhwa};
	$self->SUPER::add_pkt(@_);
}

# -------------------------------------
package Vlan; our @ISA = qw(Point Leaf PktStats);
# -------------------------------------
# A Vlan is an IEEE 802.1P/Q virtual LAN.  It is a relationship between any
# number of Vprt packages.  All virtual ports that share a hardware address and
# VLAN id with a Vlan are on that VLAN.
#
# When we add a package to a Vlan both hardware addresses are discovered to also
# belong to the same Vlan.  The Vlan was looked up by source hardware address
# and VLAN id, so we add the destination hardware address when it is available.
# A destination hardware address is not available when the packet comes from a
# Linux cooked header capture.  When another Vlan exists that already has this
# destination hardware address as on of its addresses, the two Vlan packages
# will be merged.  In this way membership of hardware addresses (Port and
# Point::Station packages) to Vlan packages is discovered.
#
# To avoid unnecessary complexity, we deal with Vlan packages only.  The parent
# Lan package is just for automatically tracking membership of Vlan to Lan
# relationships.
# -------------------------------------
#package Vlan;
sub key {
	my $type = shift;
	my ($network,$hwa,$pkt) = @_;
	$hwa = $hwa->{hwa} if ref $hwa;
	my $vlan = pack('n',$pkt->{vlan} ? $pkt->{vlan} : 0);
	return $hwa.$vlan;
}
#package Vlan;
sub skey {
	my ($type,$network,$hwa,$pkt) = @_;
	my $vlan = $pkt->{vlan} ? $pkt->{vlan} : 0;
	return 'Lan',$vlan;
}
#package Vlan;
sub init {
	my $self = shift;
	my ($network,$hwa,$pkt) = @_;
	$self->{vlan} = $pkt->{vlan};
}
#package Vlan;
sub fini {
	my $self = shift;
	delete $self->{vlan};
}
#package Vlan;
sub add_pkt {
	my $self = shift;
	my ($pkt,$dir,$noprop) = @_;
	$self->SUPER::add_pkt(@_);
	$self->add_key($pkt->{dhwa}) if $pkt->{dhwa};
}

# -------------------------------------
package Port; our @ISA = qw(Point Root PktStats);
# -------------------------------------
# A Port is a LAN link layer interface.  Fundamentally, a Port belongs to the
# Lan to which it is attached (with which it shares its hardware address key).
# Also, it fundamentally belongs to the Point::Station to which it is attached
# (and also shares its hardware address key).  The Lan to which a Port belongs
# can be found by simply looking up the Lan with the Port hardware address.  The
# same is true of the Point::Station.
# -------------------------------------
#package Port;
sub key {
	my $type = shift;
	my ($network,$hwa,$pkt) = @_;
	$hwa = $hwa->{hwa} if ref $hwa;
	return $hwa;
}
#pacakge Port;
sub init {
	my $type = shift;
	my ($network,$hwa,$pkt) = @_;
	$self->{hwa} = $hwa;
}


# -------------------------------------
package Vprt; our @ISA = qw(Point Leaf PktStats);
# -------------------------------------
# A Vprt is a VLAN link layer interface.  Fundamentall, a Vprt belongs to the
# Vlan to which it is attached (with which it shares its hardware address and
# VLAN id key).  Also, a Vprt is a child of a Port.  The Vlan to which a Vprt
# belongs can be found by simply looking up the Vlan with the Vprt hardware
# address and VLAN id.
#
# To avoid unnecessary complexity, we deal with Vprt packages only.  The parent
# Port package is just for automatically tracking membership of Vprt to Port
# relationships.
# -------------------------------------
#package Vprt;
sub key {
	my $type = shift;
	my ($network,$hwa,$pkt) = @_;
	$hwa = $hwa->{hwa} if ref $hwa;
	my $vlan = pack('n',$pkt->{vlan} ? $pkt->{vlan} : 0);
	return $hwa.$vlan;
}
#package Vprt;
sub skey {
	my ($type,$network,$hwa,$pkt) = @_;
	my $vlan = $pkt->{vlan} ? $pkt->{vlan} : 0;
	return 'Port',$vlan;
}
#package Vprt;
sub init {
	my ($self,$network,$hwa,$pkt) = @_;
	$self->{hwa} = $hwa;
	$self->{vlan} = $pkt->{vlan};
}
