package mapping;

our %mapping = (
	'NPA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'NXX'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'X'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'EXCH'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'RC'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCNAME} = $val if length($val);
	},
	'RCSHORT'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'REGION'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps) = ::getnxxccst($dat);
		if ($rg) {
			$dat->{REGION} = $rg;
			$dat->{RCCC} = $cc if $cc;
			$dat->{RCST} = $st if $st;
			if ($rg ne $val) {
				::correct($dat,$fld,$val,$rg,'autocorrect by NPA-NXX');
			}
		} elsif (length($val)) {
			$dat->{"\U$fld\E"} = $val;
			$rg = $val;
			if (exists $::lergcc{$rg} and exists $::lergst{$rg}) {
				$dat->{REGION} = $rg;
				$cc = $dat->{RCCC} = $::lergcc{$rg};
				$st = $dat->{RCST} = $::lergst{$rg};
			} else {
				::correct($dat,$fld,$val,$val,'FIXME: bad state');
			}
		}
	},
	'SEE-EXCH'=>sub{
	},
	'SEE-RC'=>sub{
	},
	'SEE-REGION'=>sub{
	},
	'SWITCH'=>sub{
		my ($dat,$fld,$val) = @_;
		if ($val and length($val) == 11) {
			my $pfx = "$dat->{NPA}-$dat->{NXX}($dat->{X}):";
			my $result = ::checkclli($val,$pfx,$dat);
			::correct($dat,$fld,$val,$val,$dat->{BADCLLI}) if $dat->{BADCLLI};
			return unless $result;
			$dat->{SWCLLI} = $val;
			$dat->{WCCLLI} = substr($val,0,8);
			$dat->{PLCLLI} = substr($val,0,6);
			$dat->{CTCLLI} = substr($val,0,4);
			my $cs = $dat->{STCLLI} = substr($val,4,2);
			$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
			$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
			$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			print STDERR "E: $pfx CLLI $val: no CC for CLLI region $cs\n" unless exists $::cllicc{$cs};
			print STDERR "E: $pfx CLLI $val: no ST for CLLI region $cs\n" unless exists $::cllist{$cs};
			print STDERR "E: $pfx CLLI $val: no RG for CLLI region $cs\n" unless exists $::cllirg{$cs};
		}
	},
	'SWITCHNAME'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{NAME} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}): switch equipment name is '$val'\n";
			}
		}
	},
	'SWITCHTYPE'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{TYPE} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}): switch equipment type is '$val'\n";
			}
		}
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'COMPANY-NAME'=>sub{
	},
	'COMPANY-TYPE'=>sub{
	},
	'ILEC-OCN'=>sub{
	},
	'ILEC-NAME'=>sub{
	},
	'RC-V'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCV} = $val if length($val);
	},
	'RC-H'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = (delete $dat->{RCV},$val);
			if ($v and $h) {
				$dat->{VH} = $dat->{RCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{LL} = $dat->{RCLL} = join(',',::vh2ll($v,$h));
			}
		}
	},
	'RC-LAT'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCLA} = $val if length($val);
	},
	'RC-LON'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = (delete $dat->{RCLA},$val);
			if ($la and $lo) {
				$dat->{RCVH} = sprintf('%05d,%05d',::ll2vh($la,$lo));
				if ($dat->{VH}) {
					if ($dat->{VH} ne $dat->{RCVH}) {
						$dat->{VH} = $dat->{RCVH} = join(';',$dat->{VH},$dat->{RCVH});
					}
				} else {
					$dat->{VH} = $dat->{RCVH};
				}
				$dat->{RCLL} = join(',',$la,$lo);
				if ($dat->{LL}) {
					if ($dat->{LL} ne $dat->{RCLL}) {
						$dat->{LL} = $dat->{RCLL} = join(';',$dat->{LL},$dat->{RCLL});
					}
				} else {
					$dat->{LL} = $dat->{RCLL};
				}
			}
		}
	},
	'EFFDATE'=>sub{
	},
	'DISCDATE'=>sub{
	},
	'UDATE'=>sub{
	},
);
