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
		if (length($val)) {
			my $rg = $val;
			$rg = 'PQ' if $rg eq 'ON' and $dat->{RCNAME} eq 'St-Regis';
			$dat->{REGION} = $rg;
			$dat->{RCCC} = $::lergcc{$rg} if exists $::lergcc{$rg};
			$dat->{RCST} = $::lergst{$rg} if exists $::lergst{$rg};
		}
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'SEE-EXCH'=>sub{
	},
	'SEE-RC'=>sub{
	},
	'SEE-REGION'=>sub{
	},
	'SWITCH'=>sub{
	},
	'SWITCHNAME'=>sub{
	},
	'SWITCHTYPE'=>sub{
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

