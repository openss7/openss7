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
		if (length($val)) {
			my $rg = $::statrg{$val} if exists $::statrg{$val};
			if ($rg) {
				$dat->{REGION} = $rg;
				$dat->{RCCC} = $::lergcc{$rg} if exists $::lergcc{$rg};
				$dat->{RCST} = $::lergst{$rg} if exists $::lergst{$rg};
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
);
