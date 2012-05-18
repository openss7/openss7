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
	'State'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps) = ::getnxxccst($dat);
		if ($rg) {
			$dat->{REGION} = $rg if $rg;
			$dat->{RCCC} = $cc if $cc;
			$dat->{RCST} = $st if $st;
			if ($cc eq 'CA') {
				if ($ps ne $val) {
					::correct($dat,$fld,$val,$ps,"autocorrect by NPA-NXX");
				}
			} else {
				if ($rg ne $val) {
					::correct($dat,$fld,$val,$rg,"autocorrect by NPA-NXX");
				}
			}
		} elsif (length($val)) {
			::correct($dat,$fld,$val,$val,'FIXME: bad state');
			$dat->{REGION} = $val;
			$dat->{RCCC} = $::lergcc{$val} if exists $::lergcc{$val};
			$dat->{RCST} = $::lergst{$val} if exists $::lergst{$val};
		}
	},
	'Company'=>sub{
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'Rate Center'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{RCSHORT} = substr("\U$val\E",0,10) if length($val)<=10;
			$dat->{RCNAME} = $val if not $dat->{RCSHORT} or $val =~ /[a-z]/;
			if ($dat->{REGION} eq 'ON' and $val eq 'St-Regis') {
				$dat->{RCCC} = 'CA';
				$dat->{RCST} = 'QC';
				$dat->{REGION} = 'PQ';
			}
		}
	},
	'Switch'=>sub{
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
	'Date'=>sub{
	},
	'Prefix Type'=>sub{
	},
	'Switch Name'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val) and $val ne 'N/A') {
			$dat->{NAME} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}): switch equipment name is '$val'\n";
			}
		}
	},
	'Switch Type'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val) and $val ne 'N/A') {
			$dat->{TYPE} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}): switch equipment type is '$val'\n";
			}
		}
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
		if ($dat->{RCCC} eq 'CA' or $val eq 'Manitoba (888)') {
			unless ($dat->{LATA} eq 'Canada (888)') {
				::correct($dat,'LATA',$dat->{LATA},'Canada (888)','autocorrect');
				$dat->{LATA} = 'Canada (888)';
			}
		}
		$dat->{LATA} =~ s/^.*?\(([0-9]+)\).*$/\1/;
	},
	'Tandem'=>sub{
	},
);
