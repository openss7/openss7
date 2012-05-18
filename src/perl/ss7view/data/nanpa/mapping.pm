package mapping;

our %fieldsource = (
	'OCN'=>'OCN',
	'RCSHORT'=>'RateCenter',
	'RCNAME'=>'RateCenter',
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
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'Company'=>sub{
	},
	'Status'=>sub{
	},
	'RateCenter'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{RCSHORT} = substr("\U$val\E",0,10) if length($val)<=10;
			$dat->{RCNAME} = $val if not $dat->{RCSHORT} or $val =~ /[a-z]/;
		}
	},
	'Remarks'=>sub{
	},
	'State'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps) = ::getnxxccst($dat);
		if ($rg) {
			$dat->{REGION} = $rg if $rg;
			$dat->{RCCC} = $cc if $cc;
			$dat->{RCST} = $st if $st;
			if ($cc eq 'CA') {
				if ($ps ne $val) {
					::correct(undef,$dat,$fld,$val,$ps,"autocorrect by NPA-NXX",'DB',$dat->{NPA},$dat->{NXX});
				}
			} else {
				if ($rg ne $val) {
					::correct(undef,$dat,$fld,$val,$rg,"autocorrect by NPA-NXX",'DB',$dat->{NPA},$dat->{NXX});
				}
			}
		} elsif (length($val)) {
			$rg = $::statrg{$val} if exists $::statrg{$val};
			if ($rg) {
				$dat->{REGION} = $rg;
				$dat->{RCCC} = $::lergcc{$rg} if exists $::lergcc{$rg};
				$dat->{RCST} = $::lergst{$rg} if exists $::lergst{$rg};
			} else {
				::correct(undef,$dat,$fld,$val,$val,'FIXME: bad state','DB',$dat->{NPA},$dat->{NXX});
			}
			$dat->{LATA} = '888' if $dat->{RCCC} eq 'CA';
		}
	},
	'EffectiveDate'=>sub{
	},
	'Use'=>sub{
	},
	'AssignDate'=>sub{
	},
	'Initial/Growth'=>sub{
	},
	'FDATE'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
);
