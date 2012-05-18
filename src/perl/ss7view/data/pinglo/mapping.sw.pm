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
	'Rate Center'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{RCSHORT} = substr("\U$val\E",0,10) if length($val)<=10;
			$dat->{RCNAME} = $val if not $dat->{RCSHORT} or $val =~ /[a-z]/;
		}
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
			$rg = $val;
			$dat->{"\U$fld\E"} = $val;
			if (exists $::lergcc{$rg} and exists $::lergst{$rg}) {
				$dat->{REGION} = $rg;
				$cc = $dat->{RCCC} = $::lergcc{$rg};
				$st = $dat->{RCST} = $::lergst{$rg};
			} else {
				::correct($dat,$fld,$val,$val,'FIXME: bad state');
				$dat->{REGION} = $val;
			}
		}
	},
);
