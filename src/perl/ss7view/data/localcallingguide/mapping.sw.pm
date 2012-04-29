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
	},
	'RC'=>sub{
	},
	'RCSHORT'=>sub{
	},
	'REGION'=>sub{
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
			$dat->{SWCLLI} = $val;
			unless (exists $skipcllis{$val}) {
				$dat->{WCCLLI} = substr($val,0,8);
				$dat->{PLCLLI} = substr($val,0,6);
				$dat->{STCLLI} = substr($val,4,2);
				$dat->{CTCLLI} = substr($val,0,4);
				my $cs = $dat->{STCLLI};
				unless (exists $::cllicc{$cs}) {
					print STDERR "E: $dat->{NPA}-$dat->{NXX}($dat->{X}) CLLI '$val' has invalid state code '$cs'\n";
				}
				$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
				$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
				$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			}
		}
	},
	'SWITCHNAME'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{NAME} = $val if length($val);
	},
	'SWITCHTYPE'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TYPE} = $val if length($val);
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
				$dat->{RCLL} = join(',',$la,$lo);
				if ($dat->{VH}) {
					if ($dat->{VH} ne $dat->{RCVH}) {
						$dat->{VH} = $dat->{RCVH} = join(';',$dat->{VH},$dat->{RCVH});
					}
				} else {
					$dat->{VH} = $dat->{RCVH};
				}
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
