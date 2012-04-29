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
	'Type'=>sub{
	},
	'Carrier'=>sub{
	},
	'Switch'=>sub{
	},
	'Rate Center'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{RCSHORT} = substr("\U$val\E",0,10) if length($val)<=10;
			$dat->{RCNAME} = $val if not $dat->{RCSHORT} or $val =~ /[a-z]/;
		}
	},
	'State'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my $rg = $val;
			if ($rg eq 'NT') {
				$rg = $nxxst{$dat->{NXX}} if exists $nxxst{$dat->{NXX}};
			}
			if ($rg eq 'NS') {
				$rg = $nsxst{$dat->{NXX}} if exists $nsxst{$dat->{NXX}};
			}
			# State is LERG except for the following:
			$rg = 'NF' if $rg eq 'NL';
			$rg = 'PQ' if $rg eq 'QC';
			$rg = 'VU' if $rg eq 'NU';
			$rg = 'NN' if $rg eq 'MP';
			$rg = 'PQ' if $rg eq 'ON' and $dat->{RCNAME} eq 'St-Regis';
			if ($rg ne $val) {
				my $numb = "$dat->{NPA}-$dat->{NXX}";
				$numb .= "-$dat->{X}" if length($dat->{X});
				print STDERR "C: $numb State: $val \-> $rg\n";
			}
			$dat->{REGION} = $rg;
			$dat->{RCCC} = $::lergcc{$rg} if exists $::lergcc{$rg};
			$dat->{RCST} = $::lergst{$rg} if exists $::lergst{$rg};
		}
	},
);
