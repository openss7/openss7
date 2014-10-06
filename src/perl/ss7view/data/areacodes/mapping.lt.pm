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
	'COUNTRY'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'STATE'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps) = ::getnxxccst($dat);
		if ($rg) {
			$dat->{REGION} = $rg if $rg;
			$dat->{RCCC} = $cc if $cc;
			$dat->{RCST} = $st if $st;
			if ($rg ne $val) {
				::correct($dat,$fld,$val,$rg,'autocorrect by NPA-NXX');
			}
			if ($cc and $cc ne $dat->{COUNTRY}) {
				::correct($dat,'COUNTRY',$dat->{COUNTRY},$cc,'autocorrect by NPA-NXX');
			}
		} elsif (length($val)) {
			my $rg = $val;
			if (exists $::lergcc{$rg} and exists $::lergst{$rg}) {
				$dat->{REGION} = $rg;
				$cc = $dat->{RCCC} = $::lergcc{$rg};
				$st = $dat->{RCST} = $::lergst{$rg};
			} else {
				::correct($dat,$fld,$val,$val,'FIXME: bad state');
			}
			if ($cc and $cc ne $dat->{COUNTRY}) {
				::correct($dat,'COUNTRY',$dat->{COUNTRY},$cc,'autocorrect');
			}
		}
	},
	'CITY'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{RCCITY} = $val;
			if (length($val)<=10 and $val !~ /[a-z]/) {
				$dat->{RCSHORT} = substr("\U$val\E",0,10);
			} else {
				$dat->{RCNAME} = $val;
			}
		}
	},
	'COUNTY'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCCOUNTY} = $val if length($val);
	},
	'LATITUDE'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCLA} = $val if length($val);
	},
	'LONGITUDE'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = (delete $dat->{RCLA},$val);
			if ($la and $lo) {
				#$dat->{VH} = $dat->{RCVH} = sprintf('%05d,%05d',::ll2vh($la,$lo));
				$dat->{LL} = $dat->{RCLL} = join(',',$la,$lo);
			}
		}
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'TIMEZONE'=>sub{
	},
	'OBSERVES_DST'=>sub{
	},
	'COUNTY_POPULATION'=>sub{
	},
	'FIPS_COUNTY_CODE'=>sub{
	},
	'MSA_COUNTY_CODE'=>sub{
	},
	'PMSA_COUNTY_CODE'=>sub{
	},
	'CBSA_COUNTY_CODE'=>sub{
	},
	'ZIPCODE_POSTALCODE'=>sub{
	},
	'ZIPCODE_COUNT'=>sub{
	},
	'ZIPCODE_FREQUENCY'=>sub{
	},
	'NEW_NPA'=>sub{
	},
	'NXX_USE_TYPE'=>sub{
	},
	'NXX_INTRO_VERSION'=>sub{
	},
	'OVERLAY'=>sub{
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'COMPANY'=>sub{
	},
	'RATE_CENTER'=>sub{
		my ($dat,$fld,$val) = @_;
		#print STDERR "I: $dat->{NPA}-$dat->{NXX}($dat->{X}) RATE_CENTER mapping called '$dat','$fld','$val'\n";
		if (length($val)) {
			if (length($val)<=10 and $val !~ /[a-z]/) {
				$dat->{RCSHORT} = substr("\U$val\E",0,10);
				#print STDERR "I: $dat->{NPA}-$dat->{NXX}($dat->{X}) RCSHORT assigned as '$dat->{RCSHORT}'";
			} else {
				$dat->{RCNAME} = $val;
				#print STDERR "I: $dat->{NPA}-$dat->{NXX}($dat->{X}) RCNAME assigned as '$dat->{RCNAME}'";
			}
		}
	},
	'SWITCH_CLLI'=>sub{
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
	'RC_VERTICAL'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCV} = $val if length($val);
	},
	'RC_HORIZONTAL'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = (delete $dat->{RCV},$val);
			if ($v and $h) {
				$dat->{RCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{RCLL} = join(',',::vh2ll($v,$h));
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
);

