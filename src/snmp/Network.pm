# ------------------------------------------
package UNIVERSAL;
use warnings;
# ------------------------------------------
#package UNIVERSAL;
sub _init {
	my $self = shift;
	my $type = shift;
	if (exists *{"$type\::"}->{only}) {
		my $sub = "$type\::only";
		$self->$sub(@_);
	} else {
		if (exists *{"$type\::"}->{ISA}) {
			foreach my $clas (@{*{"$type\::"}->{ISA}}) {
				$self->_init($clas,@_);
			}
		}
		if (exists *{"$type\::"}->{init}) {
			my $sub = "$type\::init";
			$self->$sub(@_);
		}
	}
}
#package UNIVERSAL;
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
#package UNIVERSAL;
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
#package UNIVERSAL;
sub new {
	my $type = shift;
	Carp::carp "Creating instance of $type";
	my $self = $type->_make($type,@_);
	$self->_init($type,@_);
	return $self;
}
#package UNIVERSAL;
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
			foreach my $clas (reverse @{*{"$type\::"}->{ISA}}) {
				$self->_fini($clas,@_);
			}
		}
	}
}
#package UNIVERSAL;
sub put {
	my $self = shift;
	my $type = ref $self;
	Carp::carp "Removing instance of $type, $self";
	$self->_fini($type,@_);
}
#package UNIVERSAL;
sub get {
	my $type = shift;
	Carp::carp "Getting object of $type";
	my $self;
	if ($type->can('find')) {
		$self = $type->find(@_);
	}
	return $self if $self;
	$self = $type->new(@_);
	return $self;
}
#package UNIVERSAL;
sub _dump {
	my $self = shift;
	my $type = shift;
	if (exists *{"$type\::"}->{info}) {
		my $sub = "$type\::info";
		$self->$sub(@_);
	} else {
		if (exists *{"$type\::"}->{dump}) {
			my $sub = "$type\::dump";
			$self->$sub(@_);
		}
		if (exists *{"$type\::"}->{ISA}) {
			foreach my $clas (@{*{"$type\::"}->{ISA}}) {
				$self->_dump($clas,@_);
			}
		}
	}
}
#package UNIVERSAL;
sub show {
	my $self = shift;
	my $type = ref $self;
	warn "Bad object of type $type, $self\n" if $self->{bad};
	$self->_dump($type,@_);
}

# ------------------------------------------
package Base;
use strict; use warnings;
# ------------------------------------------

# ------------------------------------------
package Root; our @ISA = qw(Base);
use strict; use warnings;
# ------------------------------------------
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
	return undef unless exists $self->{children}{$form}{$key};
	return $self->{children}{$form}{$key};
}
#package Root;
sub children {
	my ($self,$form) = @_;
	return values %{$self->{children}{$form}};
}
#package Root;
sub count {
	my ($self,$form) = @_;
	return 0 unless exists $self->{children}{$form};
	return scalar values %{$self->{children}{$form}};
}
#package Root;
sub addchild {
	my ($self,$child,$dir,$key,$form) = @_;
	$self->{children}{$form}{$key} = $child;
	$self->addfrom($child,$dir) if $self->can('addrom');
}
#package Root;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Root $othr into $self";
	return unless $othr->isa('Root');
	foreach my $form (keys %{$othr->{children}}) {
		while (my ($k,$v) = each %{$othr->{children}{$form}}) {
			$self->{children}{$form}{$k} = $v;
		}
	}
	$othr->{children} = {};
}

# ------------------------------------------
package Leaf; our @ISA = qw(Base);
use strict; use warnings;
# ------------------------------------------
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
	my ($ptyp,$key,@args) = ($type->skey(@_));
	my $parent = $ptyp->get(@args);
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
	Carp::carp "Adding child $self to parent $parent";
	$self->{parent} = $parent;
	$self->{dir} = $dir;
	$self->{key}{p} = $key;
	$self->{form} = $form;
	Carp::confess "Parent is $parent" unless $parent->can('addchild');
	$parent->addchild($self,$dir,$key,$form) if $parent;
}
#package Leaf;
sub init {
	my $self = shift;
	$self->addtoparent($self->pkey(@_),$self->form());
}
#package Leaf;
sub parent { return shift->{parent} }
#package Leaf;
sub childid { return shift->{key}{p} }
#package Leaf;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Leaf $othr into $self";
	return unless $othr->isa('Leaf');
	return if $self->{parent};
	return unless my $parent = $othr->{parent};
	my $dir = 0;
	if ($self->isa('Path') and $parent->isa('Path')) {
		$dir = ($self->obja == $parent->objb) ? 1 : 0;
	}
	my $key = $othr->{key}{p};
	my $form = $self->form;
	$self->addtoparent($parent,$dir,$key,$form);
}

# ------------------------------------------
package Tree; our @ISA = qw(Root Leaf);
use strict; use warnings;
# ------------------------------------------

# ------------------------------------------
package Type; our @ISA = qw(Base);
use strict; use warnings;
# ------------------------------------------
#package Type;
sub init { shift->{subs} = {} }
#package Type;
sub fini { delete shift->{subs} }
#package Type;
sub hasa { return exists shift->{subs}{shift} }
#package Type;
sub sub {
	my $self = shift;
	my ($subtype) = @_;
	return $self->{subs}{$subtype} if $self->hasa($subtype);
}

# ------------------------------------------
package Subtype; our @ISA = qw(Base);
use strict; use warnings;
# ------------------------------------------
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
	$type->{subs}{$subtype} = $self;
}
#package Subtype;
sub fini {
	my $self = shift;
	my $type = delete $self->{type};
	my $subtype = delete $self->{subtype};
	delete $type->{subs}{$subtype} if $type and $subtype;
}

# ------------------------------------------
package Network; our @ISA = qw(Base);
use strict; use warnings;
use threads;
use threads::shared;
use Thread::Queue;
use SNMP;
use Data::Dumper;
# ------------------------------------------
# There is only really one network object.  It is the collection of the single
# global network plus all the private and local subnetworks.  This is the root
# of all other objects in the network.
# ------------------------------------------
#package Network;
sub launch_thread {
	my $self = shift;
	my $rq :shared = Thread::Queue->new;
	my $wq :shared = Thread::Queue->new;
	my $thread = threads->create(sub{
			while (my $item = $rq->dequeue()) {
				threads->create(sub{
						my ($session,$error) = Net::SNMP->session(%$item);
						if (defined $session and $session) {
							foreach my $table (qw/
							systemScalars
							sysORTable
							ifTable
							ipAddrTable
							ipAddressTable
							atTable
							ipNetToPhysicalTable
							ipRouteTable
							lldpPortConfigTable
							lldpConfigManAddrTable
							lldpStatsTxPortTable
							lldpStatsRxPortTable
							lldpLocPortTable
							lldpLocManAddrTable
							lldpRemTable
							lldpRemManAddrTable
							lldpRemUnknownTLVTable
							lldpRemOrgDefInfoTable
							lldpXdot1ConfigPortVlanTable
							lldpXdot1ConfigVlanNameTable
							lldpXdot1ConfigProtoVlanTable
							lldpXdot1ConfigProtocolTable
							lldpXdot1LocTable
							lldpXdot1LocProtoVlanTable
							lldpXdot1LocVlanNameTable
							lldpXdot1LocProtocolTable
							lldpXdot1RemTable
							lldpXdot1RemProtoVlanTable
							lldpXdot1RemVlanNameTable
							lldpXdot1RemProtocolTable
							lldpXdot3PortConfigTable
							lldpXdot3LocPortTable
							lldpXdot3LocPowerTable
							lldpXdot3LocLinkAggTable
							lldpXdot3LocMaxFrameSizeTable
							lldpXdot3RemPortTable
							lldpXdot3RemPowerTable
							lldpXdot3RemLinkAggTable
							lldpXdot3RemMaxFrameSizeTable
							lldpXMedPortConfigTable
							lldpXMedLocMediaPolicyTable
							lldpXMedLocLocationTable
							lldpXMedLocXPoEPSEPortTable
							lldpXMedRemCapabilitiesTable
							lldpXMedRemMediaPolicyTable
							lldpXMedRemInventoryTable
							lldpXMedRemLocationTable
							lldpXMedRemXPoETable
							lldpXMedRemXPoEPSETable
							lldpXMedRemXPoEPDTable
							/) {
								my ($oid,$result,$error);
								my $key = $table;
								if ($key =~ /Table$/) {
									$oid = sub_dot(SNMP::translateObj($key));
									$result = $session->get_table(-baseoid=>$oid);
									$error = $session->error() unless defined $result;
									$wq->enqueue([$item,$table,$result,$error]);
									unless (defined $result) {
										if ($error =~ /No response/) { last }
										if ($error =~ /Requested table is empty/) { next }
										Carp::carp "SNMP error $error\n";
										next;
									}
								} elsif ($key =~ /Scalars$/) {
									$key =~ s/Scalars$//;
									$oid = sub_dot(SNMP::translateObj($key));
									$result = $session->get_bulk_request(-nonrepeaters=>0,-maxrepetitions=>8,-varbindlist=>[$oid]);
									$error = $session->error() unless defined $result;
									$wq->enqueue([$item,$table,$result,$error]);
								}
							}
						} else {
							$wq->enqueue([$item,undef,$error]);
						}
				})->detach();
			}
	})->detach();
	$self->{queue}{host}{request} = $rq;
	$self->{queue}{host}{results} = $wq;
	$self->{thread}{host} = $thread;
}
#package Network;
sub snmp_results {
	my $self = shift;
	my $count = 10;
	for (my $i=0;$i<100;$i++) {
		while (my $item = $self->{queue}{host}{results}->dequeue_nb()) {
			my ($snmpargs,$table,$result,$error) = @$item;
			my $sub = $self->can($table);
			&{$sub}($self,@$item) if $sub;
		}
		sleep 1;
		last unless --$count;
	}
}
#package Network;
sub init {
	my $self = shift;
	$self->{numbs} = {};
	$self->{items} = {};
	$self->launch_thread;
}
#package Network;
sub fini {
	my $self = shift;
	delete $self->{numbs};
	delete $self->{items};
	delete ($self->{thread}{host})->kill('TERM');
	delete $self->{queue}{host}{request};
	delete $self->{queue}{host}{results};
}
#package Network;
sub item {
	my ($self,$kind,$ref) = @_;
	my $keys = $ref;
	if (ref $ref ne 'ARRAY') {
		$keys = [ $ref ];
	}
	my $result = undef;
	foreach my $key (@$keys) {
		Carp::carp "Searching $kind key ".Item::showkey($key);
		if (exists $self->{items}{$kind}{$key}) {
			$result = $self->{items}{$kind}{$key};
			last if $result;
		}
	}
	Carp::carp "Found $result" if $result;
	return $result;
}
#package Network;
sub getno {
	my ($self,$kind) = @_;
	return --$self->{numbs}{$kind};
}
#package Network;
sub added {
	my ($self,$obj) = @_;
	Carp::carp "Added object $obj number $obj->{no} of kind $obj->{kind}";
}
#package Network;
sub removed {
	my ($self,$obj) = @_;
	Carp::carp "Removed object $obj number $obj->{no} of kind $obj->{kind}";
}
#package Network;
sub xformed {
	my ($self,$obj,$oldtype,$newtype) = @_;
	Carp::carp "Transformed $obj number $obj->{no} of kind $obj->{kind} from $oldtype to $newtype";
}

sub sub_dot {
	my $oid = shift;
	$oid =~ s/^\.+//;
	return $oid;
}
sub add_dot {
	my $oid = shift;
	$oid = sub_dot($oid);
	return ".$oid";
}

#package Network;
sub xlate_table {
	my ($snmpargs,$table,$result,$error) = @_;
	return undef unless defined $result;
	my $xlate = {};
	foreach (keys %$result) {
		my $val = $result->{$_};
		my $tag = SNMP::translateObj(add_dot($_));
		my ($label,$index) = split(/\./,$tag,2);
		my $mib = $SNMP::MIB{$label};
		if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
			$val =~ s/^0x//;
			$val = pack('H*',$val);
			$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
		}
		if ($mib->{enums}) {
			foreach my $k (keys %{$mib->{enums}}) {
				$val = "$k($val)" if $mib->{enums}{$k} eq $val;
			}
		}
		if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
			if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
				$val =~ s/://g;
				$val = join('.',unpack('C*',pack('H*',$val)));
			} else {
				while ($val =~ s/00:/:/) { }
				while ($val =~ s/:::+/::/) { }
				$val =~ s/^:([^:])/::$1/;
				$val =~ s/^0+//;
				while ($val =~ s/:0/:/) { }
			}
		}
		if ($mib->{type} =~ /OBJECTID|OPAQUE/) {
			$val = SNMP::translateObj(add_dot($val));
		}
		$xlate->{$index}{$label} = $val;
	}
	my $entry = $SNMP::MIB{$table}{children}[0];
	my @indexes = @{$entry->{indexes}};
	foreach my $index (keys %$xlate) {
		my @inds = split(/\./,$index);
		foreach my $i (@indexes) {
			my $mib = $SNMP::MIB{$i};
			next if exists $xlate->{$index}{$i};
			if ($mib->{type} =~ /INTEGER|UNSIGNED|COUNTER|GAUGE|TICKS/) {
				my $val = shift @inds;
				if (my $enums = $SNMP::MIB{$i}{enums}) {
					foreach my $k (keys %$enums) {
						$val = "$k($val)" if $enums->{$k} eq $val;
					}
				}
				$xlate->{$index}{$i} = $val;
			} elsif ($mib->{type} =~ /OCTETSTR/) {
				my @vals = ();
				my $len = shift @inds;
				while ($len>0) { push @vals, shift @inds; $len-- } 
				my $val = join(':',map {sprintf('%02x',$_)} @vals);
				if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
					$val =~ s/^0x//;
					$val = pack('H*',$val);
					$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
				}
				if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
					if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
						$val =~ s/://g;
						$val = join('.',unpack('C*',pack('H*',$val)));
					} else {
						while ($val =~ s/00:/:/) { }
						while ($val =~ s/:::+/::/) { }
						$val =~ s/^:([^:])/::$1/;
						$val =~ s/^0+//;
						while ($val =~ s/:0/:/) { }
					}
				}
				$xlate->{$index}{$i} = $val;
			} elsif ($mib->{type} =~ /OBJECTID|OPAQUE/) {
				my @vals = ();
				my $len = shift @inds;
				while ($len>0) { push @vals, shift @inds; $len-- }
				my $val = join('.',@vals);
				$val = SNMP::translateObj(add_dot($val));
				$xlate->{$index}{$i} = join('.',@vals);
			} elsif ($mib->{type} =~ /IPADDR|NETADDR/) {
				my @vals = ();
				my $len = 4;
				while ($len>0) { push @vals, shift @inds; $len-- }
				$xlate->{$index}{$i} = join('.',@vals);
			} else {
				Carp::carp "Unhandled index type for $i is $mib->{type}\n";
			}
		}
	}
	print STDERR "Table $table:\n";
	print STDERR Dumper($xlate);
	return $xlate;
}
#package Network;
sub xlate_scalars {
	my ($snmpargs,$scalars,$result,$error) = @_;
	return undef unless defined $result;
	my $xlate = {};
	foreach (keys %$result) {
		my $val = $result->{$_};
		my $tag = SNMP::translateObj(add_dot($_));
		my ($label,$index) = split(/\./,$tag,2);
		($label,$index) = ('sysUpTime',0) if $label eq 'sysUpTimeInstance';
		my $mib = $SNMP::MIB{$label};
		if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
			$val =~ s/^0x//;
			$val = pack('H*',$val);
			$val = join('.',map {sprintf('%02x',$_)} unpack('C*',$val));
		}
		if ($mib->{enums}) {
			foreach my $k (keys %{$mib->{enums}}) {
				$val = "$k($val)" if $mib->{enums}{$k} eq $val;
			}
		}
		if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
			if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
				$val =~ s/://g;
				$val = join('.',unpack('C*',pack('H*',$val)));
			} else {
				while ($val =~ s/00:/:/) { }
				while ($val =~ s/:::+/::/) { }
				$val =~ s/^:([^:])/::$1/;
				$val =~ s/^0+//;
				while ($val =~ s/:0/:/) { }
			}
		}
		if ($mib->{type} =~ /OBJECTID|OPAQUE/) {
			$val = SNMP::translateObj(add_dot($val));
		}
		$xlate->{$index}{$label} = $val;
	}
	print STDERR "Scalar $scalars:\n";
	print STDERR Dumper($xlate);
	return $xlate;
}

#package Network;
sub systemScalars {
	my $self = shift;
	#Carp::cluck "check";
	my ($snmpargs,$scalars,$result,$error) = @_;
	my $xlated = Network::xlate_scalars(@_);
	my $row = $xlated->{0};
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	my $host = Point::Host->get($self,$hkey);
	foreach (qw{sysLocation sysObjectID sysDescr sysORLastChange sysUpTime sysServices sysName sysContact}) {
		$host->{$_} = $row->{$_} if exists $row->{$_};
	}
}
#package Network;
sub sysORTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub ifTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		my $idx = $row->{ifIndex};
		my $hwa = $row->{ifPhysAddress};
		$hwa = "\0\0\0\0\0\0" unless $hwa;
		my $dsc = $row->{ifDescr};
		my $vid = 0;
		if ($dsc =~ /\d+\.(\d+)(:\d+)?$/) { $vid = $1 }
		$vid = pack('n',$vid);
		$hwa = Item::makekey($hwa);
		Point::Host->get($self,$hkey)->add_hwa($hwa);
		Vprt->get($self,$hwa,$vid)->{ifIndex} = $idx;
		Port->get($self,$hwa)->{ifIndex} = $idx;
		Vlan->get($self,$hwa,$vid);
		Point::Host->get($self,$hkey)->{indexes}{$idx} = [ $hwa, $vid ];
	}
}
#package Network;
sub ipAddrTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		my $idx = $row->{ipAdEntIfIndex};
		my $ipa = Item::makekey($row->{ipAdEntAddr});
		my $host = Point::Host->get($self,$hkey); $host->add_ipa($ipa);
		next unless exists $host->{indexes}{$idx};
		my ($hwa,$vid) = @{$host->{indexes}{$idx}};
		Vprt->get($self,$hwa,$vid)->add_ipa($ipa);
		Vlan->get($self,$hwa,$vid)->add_ipa($ipa);
		my $pfx = Item::makekey($row->{ipAdEntNetMask});
		my $sub = Subnet->get($self,$pfx); $sub->add_ipa($ipa);
		Support->get($self,$sub,Vlan->get($self,$hwa,$vid));
	}
}
#package Network;
sub ipAddressTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		next unless $row->{ipAddressRowStatus} eq 'active(1)';
		next unless $row->{ipAddressAddrType} eq 'ipv4(1)';
		next unless $row->{ipAddressType} eq 'unicast(1)';
		my $idx = $row->{ipAddressIfIndex};
		my $ipa = Item::makekey($row->{ipAddressAddr});
		my $host = Point::Host->get($self,$hkey); $host->add_ipa($ipa);
		next unless exists $host->{indexes}{$idx};
		my ($hwa,$vid) = @{$host->{indexes}{$idx}};
		Vprt->get($self,$hwa,$vid)->add_ipa($ipa);
		Vlan->get($self,$hwa,$vid)->add_ipa($ipa);
	}
}
#package Network;
sub atTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		my $idx = $row->{atIfIndex};
		my $hwa = Item::makekey($row->{atPhysAddress});
		my $ipa = Item::makekey($row->{atNetAddress});
		my $host = Point::Host->get($self,$hkey);
		next unless exists $host->{indexes}{$idx};
		my ($dummy,$vid) = @{$host->{indexes}{$idx}};
		Point::Host->get($self,$ipa)->add_hwa($hwa);
		Vprt->get($self,$hwa,$vid)->add_ipa($ipa);
		Vlan->get($self,$hwa,$vid)->add_ipa($ipa);
	}
}
#package Network;
sub ipNetToPhysicalTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		next unless $row->{ipNetToPhysicalRowStatus} eq 'active(1)';
		next unless $row->{ipNetToPhysicalNetAddressType} eq 'ipv4(1)';
		my $idx = $row->{ipNetToPhysicalIfIndex};
		my $hwa = Item::makekey($row->{ipNetToPhysicalPhysAddress});
		my $ipa = Item::makekey($row->{ipNetToPhysicalNetAddress});
		my $host = Point::Host->get($self,$hkey);
		next unless exists $host->{indexes}{$idx};
		my ($dummy,$vid) = @{$host->{indexes}{$idx}};
		Point::Host->get($self,$ipa)->add_hwa($hwa);
		Vprt->get($self,$hwa,$vid)->add_ipa($ipa);
		Vlan->get($self,$hwa,$vid)->add_ipa($ipa);
	}
}
#package Network;
sub ipRouteTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
	foreach my $row (values %$xlated) {
		my $idx = $row->{ipRouteIfIndex};
		my $pfx = Item::makekey($row->{ipRouteDest}.'/'.$row->{ipRouteMask});
		my $ipa = Item::makekey($row->{ipRouteNextHop}) if $row->{ipRouteType} eq 'indirect(4)';
		my $sub = Subnet->get($self,$pfx);
		my $host = Point::Host->get($self,$hkey);
		next unless exists $host->{indexes}{$idx};
		my ($hwa,$vid) = @{$host->{indexes}{$idx}};
		my $vprt = Vprt->get($self,$hwa,$vid);
		my $vlan = Vlan->get($self,$hwa,$vid); $vlan->add_ipa($ipa) if $ipa;
		Support->get($self,$sub,$vlan) if $row->{ipRouteType} eq 'direct(3)';
		Route->get($self,$vprt,$sub,$ipa);
	}
}
#package Network;
sub lldpPortConfigTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpConfigManAddrTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpStatsTxPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpStatsRxPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpLocPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpLocManAddrTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpRemTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpRemManAddrTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpRemUnknownTLVTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpRemOrgDefInfoTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1ConfigPortVlanTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1ConfigVlanNameTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1ConfigProtoVlanTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1ConfigProtocolTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1LocTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1LocProtoVlanTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1LocVlanNameTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1LocProtocolTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1RemTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1RemProtoVlanTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1RemVlanNameTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot1RemProtocolTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3PortConfigTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3LocPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3LocPowerTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3LocLinkAggTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3LocMaxFrameSizeTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3RemPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3RemPowerTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3RemLinkAggTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXdot3RemMaxFrameSizeTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedPortConfigTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedLocMediaPolicyTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedLocLocationTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedLocXPoEPSEPortTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemCapabilitiesTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemMediaPolicyTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemInventoryTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemLocationTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemXPoETable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemXPoEPSETable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}
#package Network;
sub lldpXMedRemXPoEPDTable {
	my $self = shift;
	my $xlated = Network::xlate_table(@_);
	my ($snmpargs,$table,$result,$error) = @_;
	my $hkey = Item::makekey($snmpargs->{'-hostname'});
}

# ------------------------------------------
package Item; our @ISA = qw(Base);
use strict; use warnings;
use Carp qw(cluck);
# ------------------------------------------
#package Item;
sub makekey {
	my $key = shift;
	unless (defined $key) {
		cluck "passed null key";
		return '';
	}
	my ($made,@parts) = ($key);
	if ($key =~ /^-?\d+$/) {
		$made = pack('s',$key);
	} elsif ($key =~ /^0x([0-9a-fA-F]{2})+$/) {
		$key =~ s/^0x//;
		$made = pack('H*',$key);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*$/) {
		$key =~ s/://g;
		$made = pack('H*',$key);
	} elsif ($key =~ /^\d+(\.\d+){3}$/) {
		@parts = split(/\./,$key);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$made = pack('C*',@parts);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
		$key =~ s/://g;
		$made = pack('H*',$key);
	} elsif ($key =~ /^\d+(\.\d+){0,3}\/\d+$/) {
		my ($pfx,$len) = split(/\//,$key);
		@parts = split(/\./,$pfx);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$made = pack('C*',@parts,$len);
	} elsif ($key =~ /^\d+(\.\d+){0,3}\/\d+(\.\d+){0,3}$/) {
		my ($add,$msk) = split(/\//,$key);
		@parts = split(/\./,$add);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$add = pack('C*',@parts);
		$add = unpack('N',$add);
		@parts = split(/\./,$msk);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$msk = pack('C*',@parts);
		$msk = unpack('N',$msk);
		my $pfx = $add & $msk;
		my $len = 32;
		while (($msk & 1) == 0 and $len > 0) { $len--; $msk <<= 1 }
		$made = pack('NC',$pfx,$len);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*[(]\d+[)]$/) {
		my ($hwa,$vlan) = split(/[()]/,$key);
		$hwa =~ s/://g;
		$made = pack('H*',$hwa).pack('n',$vlan);
	}
	return $made;
}
#package Item;
sub showkey {
	my $key = shift;
	unless (defined $key) {
		cluck "passed null key";
		return '(null)';
	}
	my ($show,@parts);
	if (length($key) == 2) { # item number
		$show = unpack('s',$key);
	} elsif (length($key) == 4) { # IP address
		@parts = unpack('C*',$key);
		$show = join('.',@parts);
	} elsif (length($key) == 5) { # IP prefix
		@parts = unpack('C*',$key);
		$show = join('/',join('.',@parts[0..3]),$parts[4]);
	} elsif (length($key) == 6) { # HW address
		@parts = unpack('C*',$key);
		$show = join(':',map {sprintf('%02x',$_)} @parts);
	} elsif (length($key) == 8) { # HW address + VLAN
		@parts = unpack('C6n',$key);
		$show = join(':',map {sprintf('%02x',$_)} @parts[0..5])."($parts[6])";
	} else {
		@parts = unpack('C*',$key);
		$show = "0x".join('',map {sprintf('%02x',$_)} @parts);
	}
	return $show;
}
#package Item;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	$type =~ s/::.*//;
	return "\L$type\Es";
}
#package Item;
sub key  { return join('',shift->gkey(@_)) }
#package Item;
sub keys { return shift->key(@_) }
#package Item;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($network) = @_;
	$self->{network} = $network;
	my $keys = $self->{key}{n} = [ $type->keys(@_) ];
	Carp::carp "Creating item $self with key ".Item::showkey($keys->[0]);
	my $kind = $self->{kind} = $self->kind;
	$self->{no} = $network->getno($kind);
	push @{$keys}, Item::makekey($self->{no});
	foreach my $key (@$keys) {
		$network->{items}{$kind}{$key} = $self;
	}
	$network->added($self);
	$self->{new} = 1;
}
#package Item;
sub dump {
	my $self = shift;
	print STDERR "Item $self has number $self->{no}\n";
}
#package Item;
sub find {
	my $type = shift;
	my ($network) = @_;
	Carp::carp "Finding object of $type";
	my $self = $network->item(Item::kind($type),$type->key(@_));
	delete $self->{new} if $self and exists $self->{new};
	return $self;
}
#package Item;
sub fini {
	my $self = shift;
	my $network = $self->{network};
	my $keys = $self->{key}{n};
	my $kind = $self->{kind};
	foreach my $key (@$keys) {
		delete $network->{items}{$kind}{$key};
	}
	$network->removed($self);
	delete $self->{key}{n};
	delete $self->{no};
	$self->{bad} = 1;
}
#package Item;
sub add_key {
	my $self = shift;
	my ($key) = @_;
	my $kind = $self->{kind};
	foreach (@{$self->{key}{n}}) {
		return if $_ eq $key;
	}
	my $othr = $self->{network}->item($kind,$key);
	if (defined $othr) {
		return if $othr eq $self;
		$self->mergefrom($othr);
		$othr->put;
	} else {
		push @{$self->{key}{n}}, $key;
		$self->{network}{items}{$kind}{$key} = $self;
	}
}
#package Item;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Item $othr into $self";
	return unless $othr->isa('Item');
	my ($s_network,$s_kind) = ($self->{network},$self->{kind});
	my ($o_network,$o_kind) = ($othr->{network},$othr->{kind});
	foreach (@{$othr->{key}{n}}) {
		Carp::carp "Merging Item key ".Item::showkey($_);
		push @{$self->{key}{n}}, $_;
		delete $o_network->{items}{$o_kind}{$_};
		$s_network->{items}{$s_kind}{$_} = $self;
	}
	print STDERR "Item $self keys are now:\n";
	foreach (sort @{$self->{key}{n}}) {
		print STDERR "\t",Item::showkey($_),"\n";
	}
	$self->Root::mergefrom($othr) if $self->isa('Root');
	$self->Leaf::mergefrom($othr) if $self->isa('Leaf');
	$self->HasIpas::mergefrom($othr) if $self->isa('HasIpas');
	$self->HasHwas::mergefrom($othr) if $self->isa('HasHwas');
}
#package Item;
sub xform {
	my ($type,$self) = @_;
	my $oldtype = ref $self;
	bless $self,$type;
	$self->{network}->xformed($self,$oldtype,$type);
}

# ------------------------------------------
package Point; our @ISA = qw(Item);
use strict; use warnings;
use Carp qw(cluck);
# ------------------------------------------
#package Point;
sub gkey {
	my $typeorself = shift;
	my ($network,$id,@args) = @_;
	$id = Item::makekey($id);
	return $id;
}
#package Point;
sub init {
	my $self= shift;
	$self->{end} = { a => {}, b => {} };
}
#package Point;
sub fini {
	my $self = shift;
	foreach my $side (values %{$self->{end}}) {
		foreach my $kind (values %$side) {
			foreach my $edge (values %$kind) {
				$edge->put(@_);
			}
		}
	}
	delete $self->{end};
}
#package Point;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Point $othr into $self";
	$self->SUPER::mergefrom($othr);
	# move all the edges from $othr to attach instead to $self
	unless ($othr->isa('Point')) {
		cluck "Cannot merge $self and $othr";
		return undef;
	}
	my @tomerge = ();
	my @puts = ();
	foreach my $side (keys %{$othr->{end}}) {
		foreach my $kind (keys %{$othr->{end}{$side}}) {
			foreach my $key (keys %{$othr->{end}{$side}{$kind}}) {
				my $edgeo = $othr->{end}{$side}{$kind}{$key};
				my $edges = $self->{end}{$side}{$kind}{$key}
				if exists $self->{end}{$side}{$kind}{$key};
				if ($edges) {
					warn "cannot merge $edgeo and $edges";
					#does not work for overlapping edges
					push @tomerge,$edges,$edgeo;
				} else {
					$edgeo->{obj}{$side} = $self;
					$self->{end}{$side}{$kind}{$key} = $edgeo;
					delete $othr->{end}{$side}{$kind}{$key};
				}
			}
		}
	}
	while (my $edges = shift @tomerge) {
		my $edgeo = shift @tomerge;
		$edges->mergefrom($edgeo);
		push @puts,$edgeo;
	}
	foreach (@puts) { $_->put }
	# note: any edges that went from $othr to $self will wind up going
	# from $self to $self.
	$self->addrom($othr,$self->{dir}^$othr->{dir})
	if $self->isa('Counts') or $self->isa('Collector');
	# FIXME: add notification of the merge
	return $self;
}
#package Point;
sub count {
	my ($self,$side,$kind) = @_;
	return undef unless exists $self->{end}{$side}{$kind};
	return scalar values %{$self->{end}{$side}{$kind}};
}
#package Point;
sub counts {
	my ($self,$kind) = @_;
	return undef unless exists $self->{end}{a}{$kind} or
			    exists $self->{end}{b}{$kind};
	return (scalar values %{$self->{end}{a}{$kind}}) +
	       (scalar values %{$self->{end}{b}{$kind}});
}

# ------------------------------------------
package Path; our @ISA = qw(Item);
use strict; use warnings;
use Carp qw(cluck);
# ------------------------------------------
#package Path;
sub typpair { return 'Point','Point'; }
#package Path;
sub gkey {
	my $type = shift;
	my ($network,$keya,$keyb,@args) = @_;
	my ($okeya,$okeyb) = ($keya,$keyb);
	my ($typea,$typeb) = $type->typpair();
	$keya = $typea->key($network,$keya);
	cluck "Key A is null for type $typea, originally $okeya" unless $keya;
	$keyb = $typeb->key($network,$keyb);
	cluck "Key B is null for type $typeb, originally $okeyb" unless $keyb;
	return $keya, $keyb;
}
#package Path;
sub gobj {
	my $type = shift;
	my ($network,$obja,$objb,@args) = @_;
	my ($typea,$typeb) = ($type->typpair(@_));
	Carp::confess "Object $obja must be of type $typea" if ref $obja and not $obja->isa($typea);
	Carp::confess "Object $objb must be of type $typeb" if ref $objb and not $objb->isa($typeb);
	$obja = $typea->get($network,$obja,@args) unless ref $obja;
	$objb = $typeb->get($network,$objb,@args) unless ref $objb;
	return $obja,$objb;
}
#package Path;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($obja,$objb) = ($type->gobj(@_));
	$self->{obj}{a} = $obja;
	$self->{obj}{b} = $objb;
	my ($keya,$keyb) = ($type->gkey(@_));
	$self->{key}{a} = $keya;
	$self->{key}{b} = $keyb;
	print STDERR "Key A for $obja is ",Item::showkey($keya),"\n";
	print STDERR "Key B for $objb is ",Item::showkey($keyb),"\n";
	$obja->{end}{a}{$self->{kind}}{$keyb} = $self;
	$objb->{end}{b}{$self->{kind}}{$keya} = $self;
	$self->{duplex} = 0;
}
#package Path;
sub dump {
	my $self = shift;
	print STDERR "Path $self object A is $self->{obj}{a}\n";
	print STDERR "Path $self object B is $self->{obj}{b}\n";
}
#package Path;
sub fini {
	my $self = shift;
	my ($keya,$keyb) = ($self->{key}{a},$self->{key}{b});
	my ($obja,$objb) = ($self->{obj}{a},$self->{obj}{b});
	delete $obja->{end}{a}{$self->{kind}}{$keyb} if $obja;
	delete $objb->{end}{b}{$self->{kind}}{$keya} if $objb;
	foreach (qw(key obj)) { delete $self->{$_} }
}
#package Path;
sub identify {
	my $self = shift;
	my $type = ref $self;
	my $id = "$type ";
	$id .= $self->{obj}{a}->shortid;
	$id .= $self->{duplex} ? ', ' : ' -> ';
	$id .= $self->{obj}{b}->shortid;
	my $cid = $self->can('childid');
	$cid = ' : '.$cid if $cid and $cid = &{$cid}($self);
	$id .= $cid;
	return ($self->{id} = $id);
}
#package Path;
sub shortid {
	my $self = shift;
	my $conn = $self->{duplex} ? '::' : '->';
	my $cid = $self->can('childid');
	$cid = ' : '.$cid if $cid and $cid = &{$cid}($self);
	return $self->{obj}{a}->shortid.$conn.$self->{obj}{b}->shortid.$cid;
}
#package Path;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Path $othr into $self";
	$self->SUPER::mergefrom($othr);
	# merge path $othr into $self.  It does not matter between which
	# nodes $othr exists.
	unless (ref $self eq ref $othr) {
		cluck "Cannot merge $self and $othr";
		return undef;
	}
	$self->addfrom($othr,$self->{dir}^$othr->{dir})
	if $self->isa('Counts') or $self->isa('Collector');
	# FIXME: add notification of merge
	return $self;
}
#package Path;
sub obja { my $self = shift; return $self->{obj}{a} }
#package Path;
sub objb { my $self = shift; return $self->{obj}{b} }
#package Path;
sub objs { my $self = shift; return $self->{obj}{a},$self->{obj}{b} }

# -------------------------------------
package Path::Duplex; our @ISA = qw(Path);
use strict; use warnings;
# -------------------------------------
# A Path::Duplex is a bidirectional concept.  There are two directional keys
# used to index the Path.
# -------------------------------------
#package Path::Duplex;
sub init {
	my $self = shift;
	$self->{duplex} = 1;
}
#package Path::Duplex;
sub keys {
	my $type = shift;
	my ($network,$keya,$keyb,@args) = @_;
	return	$type->key($network,$keya,$keyb,@args),
		$type->key($network,$keyb,$keya,@args);
}

# -------------------------------------
package Traceable; our @ISA = qw(Base);
use strict; use warnings;
# -------------------------------------
#package Traceable;
sub init {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->{trace}{$which} = [];
	$self->{tracing}{$which} = $what;
}
#package Traceable;
sub fini {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->dotrace('put',${$self->{tracing}{$which}});
	delete $self->{trace}{$which};
	delete $self->{tracing}{$which};
}
#package Traceable;
sub trace {
	my ($self,$which,$sub) = @_;
	my $index = scalar @{$self->{trace}{$which}};
	push @{$self->{trace}{$which}}, $sub;
	return $index;
}
#package Traceable;
sub untrace {
	my ($self,$which,$index) = @_;
	if ($self->{trace}{$which}->[$index]) {
		$self->{trace}{$which}->[$index] = undef;
		while (!defined $self->{trace}{$which}->[0]) {
			shift @{$self->{trace}{$which}};
		}
		while (!defined $self->{trace}{$which}->[-1]) {
			pop @{$self->{trace}{$which}};
		}
		return 1;
	}
}
#package Traceable;
sub dotrace {
	my ($self,$which,$what) = @_;
	foreach (@{$self->{trace}{$which}}) {
		my $val = ${$self->{tracing}{$which}};
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
use strict; use warnings;
# -------------------------------------
#package Showable;
sub init {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->{show}{$which} = undef;
}
#package Showable;
sub fini {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->{show}{$which}->put(@_) if $self->{show}{$which};
	delete $self->{show}{$which};

}
#package Showable;
sub update {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->Traceable::dotrace($which,'changed');
	$self->{show}{$which}->update(@_) if $self->{show}{$which};
}

# -------------------------------------
package HasIpas; our @ISA = qw(Base);
use strict; use warnings;
# -------------------------------------
#package HasIpas;
sub init {
	my $self = shift;
	$self->{ipas} = {};
}
#package HasIpas;
sub dump {
	my $self = shift;
	foreach (sort keys %{$self->{ipas}}) {
		print STDERR "HasIpas $self has IP address ",Item::showkey($_),"\n";
	}
}
#package HasIpas;
sub fini {
	my $self = shift;
	delete $self->{ipas};
	delete $self->{ipa};
}
#package HasIpas;
sub add_ipa {
	my ($self,$ipa) = @_;
	return if defined $ipa and exists $self->{ipas}{$ipa};
	Carp::carp "Adding IP address ".Item::showkey($ipa)." to $self";
	$self->add_key($ipa) unless unpack('C',$ipa) == 127;
	if ($self->{parent} and $self->{parent}->can('add_ipa')) {
		$self->{parent}->add_ipa($ipa);
	}
	$self->{ipas}{$ipa} = $ipa if defined $ipa;
	$self->{ipa} = $ipa if defined $ipa and $ipa and not exists $self->{ipa};
}
#package HasIpas;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging HasIpas $othr into $self";
	return unless $othr->isa('HasIpas');
	$self->{ipa} = $othr->{ipa} unless $self->{ipa};
	delete $othr->{ipa};
	foreach (keys %{$othr->{ipas}}) { $self->{ipas}{$_} = $_ }
	$othr->{ipas} = {};
}

# -------------------------------------
package HasHwas; our @ISA = qw(Base);
use strict; use warnings;
# -------------------------------------
#package HasHwas;
sub init {
	my $self = shift;
	$self->{hwas} = {};
}
#package HasHwas;
sub dump {
	my $self = shift;
	foreach (sort keys %{$self->{hwas}}) {
		print STDERR "HasHwas $self has HW address ",Item::showkey($_),"\n";
	}
}
#package HasHwas;
sub fini {
	my $self = shift;
	delete $self->{hwas};
	delete $self->{hwa};
}
#package HasHwas;
sub add_hwa {
	my ($self,$hwa,$vlan) = @_;
	return if defined $hwa and exists $self->{hwas}{$hwa};
	$vlan = $self->{vlan} if exists $self->{vlan};
	$vlan = '' unless defined $vlan;
	Carp::carp "Adding HW address ".Item::showkey($hwa.$vlan)." to $self";
	$self->add_key($hwa.$vlan) unless $hwa eq "\0\0\0\0\0\0";
	if ($self->{parent} and $self->{parent}->can('add_hwa')) {
		$self->{parent}->add_hwa($hwa);
	}
	$self->{hwas}{$hwa} = $hwa if defined $hwa;
	$self->{hwa} = $hwa if defined $hwa and $hwa and not exists $self->{hwa};
}
#package HasHwas;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging HasHwas $othr into $self";
	return unless $othr->isa('HasHwas');
	$self->{hwa} = $othr->{hwa} unless $self->{hwa};
	delete $othr->{hwa};
	foreach (keys %{$othr->{hwas}}) { $self->{hwas}{$_} = $_ }
	$othr->{hwas} = {};
}


# -------------------------------------
package SubNetwork; our @ISA = qw(Point HasHwas HasIpas Root);
use strict; use warnings;
# -------------------------------------
# A subnetwork is a scoping of IP addresses.  There are either local, link,
# private or global subnetworks.  There is only a single global subnetwork.
# -------------------------------------
#package SubNetwork;

# -------------------------------------
package SubNetwork::Local; our @ISA = qw(SubNetwork);
use strict; use warnings;
# -------------------------------------
# Every host has a local subnetwork.  This is the 127.0.0.0/8 subnet.
# -------------------------------------
#package SubNetwork::Local;

# -------------------------------------
package SubNetwork::Link; our @ISA = qw(SubNetwork);
use strict; use warnings;
# -------------------------------------
# Every port has a link local subnetwork.  This is only an IPv6 concept.
# -------------------------------------
#package SubNetwork::Link;

# -------------------------------------
package SubNetwork::Private; our @ISA = qw(SubNetwork);
use strict; use warnings;
# -------------------------------------
# Some hosts may belong to a private subnetwork.  Private subnetworks are
# distiguished from other private subnetworks by the global scoped IP addresses
# that belong to them.
# -------------------------------------
#package SubNetwork::Private;

# -------------------------------------
package SubNetwork::Global; our @ISA = qw(SubNetwork);
use strict; use warnings;
# -------------------------------------
# There is a single global subnetwork to which all global IP addresses belong.
# -------------------------------------
#package SubNetwork::Global;

# -------------------------------------
package Point::Station; our @ISA = qw(Point HasHwas HasIpas Root);
use strict; use warnings;
# -------------------------------------
# Stations are an unidentified IP host or gateway.  We know that there are two
# stations on the edge of a single ethernet, we just do no know which stations
# yet.  A bare Point::Station object is an unknown IP host or gateway.  It just
# acts as a place holder for when the station is bound to an actual IP host or
# gateway.  Stations need a put method so that the ethernet can put these
# stations once they are no longer necessary.  Stations are created in reference
# to an ethernet, the edge of the ethernet on which they exist.
#
# When a Point::Station is created, we know possibly one of its Ethernet
# hardware addresses (Port), but we have no idea which IP addreses belong to it
# unti an ARP or LLDP packet is processed.  When we are live (we are viewing
# captured files or performing capture on the monitoring point itself), we can
# query ARP, LLDP or SNMP for further information.  In particualr LLDP can help
# us find the management IP address associated with the station so that we can
# query it directly with SNMP.
#
# The addchild() method of the Root package is overridden so that the key
# associated with the child Port can be added to the station and stations can
# be properly merged once their hardware and IP addresses are discovered.
# -------------------------------------
#package Point::Station;
sub gkey {
	my $typeorself = shift;
	return Port->gkey(@_);
}
#package Point::Station;
sub addchild {
	my $self = shift;
	my ($child,$dir,$hwa,$form) = @_;
	$self->SUPER::addchild(@_);
	$self->add_hwa($hwa);
}
#package Point::Station;
sub add_ipa {
	my ($self,$ipa) = @_;
	my $type = ref $self;
	Point::Host->xform($self);
	$self->add_ipa($ipa);
	$self->query($self->{network},$ipa);
}
#package Point::Station;
sub init {
	my $self = shift;
	my $hwa = $self->gkey(@_);
	$self->add_hwa($hwa);
	$self->{indexes} = {};
}
#package Point::Station;
sub mergefrom {
	my ($self,$othr) = @_;
	Carp::carp "Merging Point::Station $othr into $self";
	$self->SUPER::mergefrom($othr);
	foreach (keys %{$othr->{indexes}}) {
		$self->{indexes}{$_} = $othr->{indexes}{$_};
	}
	delete $othr->{indexes};
}

# -------------------------------------
package Point::Host; our @ISA = qw(Point HasHwas HasIpas Root);
use strict; use warnings;
# -------------------------------------
#package Point::Host;
sub gkey {
	my $typeorself = shift;
	my ($network,$ipa) = @_;
	$ipa = $ipa->{ipa} if ref $ipa;
	$ipa = Item::makekey($ipa);
	return $ipa;
}
#package Point::Host;
sub query {
	my $self = shift;
	my ($network,$ipa) = @_;
	my $snmpargs = {
		-hostname=>Item::showkey($ipa),
		-version=>'snmpv2c',
		-community=>'public',
	};
	$network->{queue}{host}{request}->enqueue($snmpargs);
}
#package Point::Host;
sub init {
	my $self = shift;
	my $ipa = $self->gkey(@_);
	$self->add_ipa($ipa);
	$self->query(@_);
}
#package Point::Host;
sub add_ipa {
	my ($self,$ipa) = @_;
	$self->HasIpas::add_ipa($ipa);
}

# -------------------------------------
package Point::Host::LLDP; our @ISA = qw(Point::Host);
use strict; use warnings;
# -------------------------------------

# -------------------------------------
package Point::Router; our @ISA = qw(Point::Station);
use strict; use warnings;
# -------------------------------------

# -------------------------------------
package Point::Router::LLDP; our @ISA = qw(Point::Router);
use strict; use warnings;
# -------------------------------------

# -------------------------------------
package Lan; our @ISA = qw(Point HasHwas HasIpas Root);
use strict; use warnings;
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
sub gkey {
	my $typeorself = shift;
	return Port->gkey(@_);
}
#package Lan;
sub init {
	my $self = shift;
	my $hwa = $self->gkey(@_);
	$self->add_hwa($hwa);
}

# -------------------------------------
package Vlan; our @ISA = qw(Point HasHwas HasIpas Leaf);
use strict; use warnings;
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
#
# The addchild() method of the Root package is overridden so that the key
# associated with the child Subnet can be added to the Vlan
# -------------------------------------
#package Vlan;
sub gkey {
	my $typeorself = shift;
	return Vprt->gkey(@_);
}
#package Vlan;
sub skey {
	my $type = shift;
	my ($hwa,$vlan) = $type->gkey(@_);
	return 'Lan',$vlan,@_;
}
#package Vlan;
sub init {
	my $self = shift;
	my ($hwa,$vlan) = $self->gkey(@_);
	$self->{vlan} = $vlan;
	$self->add_hwa($hwa,$vlan);
}
#package Vlan;
sub dump {
	my $self = shift;
	print STDERR "Vlan $self has VLAN ID ",Item::showkey($self->{vlan}),"\n";
}
#package Vlan;
sub fini {
	my $self = shift;
	delete $self->{vlan};
}

# -------------------------------------
package Subnet; our @ISA = qw(Point HasIpas);
use strict; use warnings;
# -------------------------------------
# A Subnet is an IP subnet (address prefix) that is associated with a Vlan.
# Note that 0.0.0.0/0 is the default subnet.
# -------------------------------------
#package Subnet;
sub gkey {
	my $typeorself = shift;
	my ($network,$pfx) = @_;
	$pfx = $pfx->{pfx} if ref $pfx;
	$pfx = Item::makekey($pfx);
	return $pfx;
}
#package Subnet;
sub init {
	my $self = shift;
	my $pfx = $self->gkey(@_);
	$self->{pfx} = $pfx;
}
#package Subnet;
sub fini {
	my $self = shift;
	delete $self->{pfx};
}

# -------------------------------------
package Support; our @ISA = qw(Path);
use strict; use warnings;
# -------------------------------------
# A Support represents the support for a subnet on a Vlan;
# -------------------------------------
#package Support;
sub typpair { return 'Subnet','Vlan' }
#package Support;
sub dump {
	my $self = shift;
	print STDERR "Support $self supports subnet ",Item::showkey($self->{key}{a})," on vlan ",Item::showkey($self->{key}{b}),"\n";
}

# -------------------------------------
package Route; our @ISA = qw(Path);
use strict; use warnings;
# -------------------------------------
# A route is an IP routing path from a Port to a Subnet via a gateway
# (Point::Router).
# -------------------------------------
#package Route;
sub typpair { return 'Vprt','Subnet'; }
#package Route;
sub gkey {
	my ($type,$network,$vprt,$dest,$gw) = @_;
	$gw = Point::Host->key($network,$gw) if defined $gw;
	$gw = '' unless defined $gw;
	($vprt,$dest) = Path::gkey(@_);
	return $vprt, $dest.$gw;
}
#package Route;
sub dump {
	my $self = shift;
	print STDERR "Route $self is from vprt ",Item::showkey($self->{key}{a})," to subnet ",Item::showkey($self->{key}{b}),"\n";
}

# -------------------------------------
package Loopback; our @ISA = qw(Point Leaf);
use strict; use warnings;
# -------------------------------------
# A Loopback object represents the loopback port.
# -------------------------------------
#package Loopback;

# -------------------------------------
package Port; our @ISA = qw(Point HasIpas Tree);
use strict; use warnings;
# -------------------------------------
# A Port is a LAN link layer interface.  Fundamentally, a Port belongs to the
# Lan to which it is attached  (with which it shares its hardware address key).
# Also, it fundamentally belongs to the Point::Station to which it is attached
# (and also shares its hardware address key).  The Lan to which a Port belongs
# can be found by simply lookup up the Lan with the Port hardware address.  The
# same is true of the Point::Station.
# -------------------------------------
#package Port;
sub gkey {
	my $typeorself = shift;
	my ($network,$hwa) = @_;
	$hwa = $hwa->{hwa} if ref $hwa;
	$hwa = Item::makekey($hwa);
	return $hwa;
}
#package Port;
sub skey {
	my $type = shift;
	return 'Point::Station', $type->key(@_),@_;
}
#package Port;
sub init {
	my $self = shift;
	my $hwa = $self->gkey(@_);
	$self->{hwa} = $hwa;
}
#package Port;
sub dump {
	my $self = shift;
	print STDERR "Port $self has HW address ",Item::showkey($self->{hwa}),"\n";
}
#package Port;
sub fini {
	my $self = shift;
	delete $self->{hwa};
	delete $self->{ipas};
}

# -------------------------------------
package Vprt; our @ISA = qw(Point HasIpas Leaf);
use strict; use warnings;
# -------------------------------------
# A Vprt is a VLAN link layer interface.  Fundamentally, a Vprt belongs to the
# Vlan to which it is attached (with which it shares its hardware address and
# VLAN id key).  Also, a Vprt is a child of a Port.  The Vlan to which a Vprt
# belongs can be found by simply lookup up the Vlan with the Vprt hardware
# address and VLAN id.
#
# To avoid unnecessary complexity, we deal with Vprt packages only.  The parent
# Port package is just for automatically tracking membership of Vprt to Port
# relationships.
# -------------------------------------
#package Vprt;
sub gkey {
	my $typeorself = shift;
	my ($network,$hwa,$vlan) = @_;
	$hwa = Port->key($network,$hwa);
	$vlan = '' unless defined $vlan;
	$vlan = Item::makekey($vlan);
	return $hwa,$vlan;
}
#package Vprt;
sub skey {
	my $type = shift;
	my ($hwa,$vlan) = $type->gkey(@_);
	return 'Port',$vlan,@_;
}
#package Vprt;
sub init {
	my $self = shift;
	my ($hwa,$vlan) = $self->gkey(@_);
	$self->{hwa} = $hwa;
	$self->{vlan} = $vlan;
}
#package Vprt;
sub fini {
	my $self = shift;
	delete $self->{hwa};
	delete $self->{vlan};
}
#package Vprt;
sub dump {
	my $self = shift;
	print STDERR "Vprt $self has HW address ",Item::showkey($self->{hwa}),"\n";
	print STDERR "Vprt $self has VLAN ID ",Item::showkey($self->{vlan}),"\n";
}
