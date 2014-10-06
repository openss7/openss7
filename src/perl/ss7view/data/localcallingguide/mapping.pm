package mapping;

our %fieldsource = (
	'EXCH'=>'EXCH',
	'RCNAME'=>'RC',
	'RCSHORT'=>'RCSHORT',
	'REGION'=>'REGION',
	'SWCLLI'=>'SWITCH',
	'SWNAME'=>'SWITCHNAME',
	'SWTYPE'=>'SWITCHTYPE',
	'LATA'=>'LATA',
	'OCN'=>'OCN',
	'FDATE'=>'FDATE',
);

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
	'XXXX'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'YYYY'=>sub{
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
				::correct(undef,$dat,$fld,$val,$rg,'autocorrect by NPA-NXX','DB',$dat->{NPA},$dat->{NXX});
			}
		} elsif (length($val)) {
			$dat->{"\U$fld\E"} = $val;
			$rg = $val;
			if (exists $::lergcc{$rg} and exists $::lergst{$rg}) {
				$dat->{REGION} = $rg;
				$cc = $dat->{RCCC} = $::lergcc{$rg};
				$st = $dat->{RCST} = $::lergst{$rg};
			} else {
				::correct(undef,$dat,$fld,$val,$val,'FIXME: bad state','DB',$dat->{NPA},$dat->{NXX});
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
			return unless ::checkclli($dat,'SWCLLI',$val);
			$dat->{SWCLLI} = $val;
			$dat->{WCCLLI} = substr($val,0,8);
			$dat->{PLCLLI} = substr($val,0,6);
			$dat->{CTCLLI} = substr($val,0,4);
			my $cs = $dat->{STCLLI} = substr($val,4,2);
			$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
			$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
			$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no CC for CLLI region $cs\n" unless exists $::cllicc{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no ST for CLLI region $cs\n" unless exists $::cllist{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no RG for CLLI region $cs\n" unless exists $::cllirg{$cs};
		}
	},
	'SWITCHNAME'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{SWNAME} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: ",::getpfx($dat),": switch equipment name is '$val'\n";
			}
		}
	},
	'SWITCHTYPE'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{SWTYPE} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: ",::getpfx($dat),": switch equipment type is '$val'\n";
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
			$dat->{RCVH} = sprintf('%05d,%05d',$v,$h) if ($v and $h);
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
			#$dat->{RCLL} = join(',',$la,$lo) if $la and $lo;
		}
	},
	'EFFDATE'=>sub{
	},
	'DISCDATE'=>sub{
	},
	'UDATE'=>sub{
	},
	'FDATE'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
);
