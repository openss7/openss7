package mapping;

our %cor = (
#	'AG.AN'=>'AG.AG.AN',
#	'BS.BA'=>'BS.BS.BA',
#	'BB.BD'=>'BB.BB.BD',
#	'VG.BV'=>'VG.VG.BV',
#	'KY.CQ'=>'KY.KY.CQ',
#	'DO.DR'=>'DO.DO.DR',
#	'GD.GN'=>'GD.GD.GN',
#	'US.GU'=>'GU.GU.GU',
#	'KN.KA'=>'KN.KN.KA',
#	'US.MP'=>'MP.MP.NN',
#	'CA.NL'=>'CA.NL.NF',
#	'CA.NU'=>'CA.NU.VU',
#	'US.PR'=>'PR.PR.PR',
#	'CA.QC'=>'CA.QC.PQ',
#	'MS.RT'=>'MS.MS.RT',
#	'LC.SA'=>'LC.LC.SA',
#	'SX.SF'=>'SX.SX.SF',
	'US.SK'=>'CA.SK.SK',
#	'TT.TR'=>'TT.TT.TR',
#	'US.AS'=>'AS.AS.AS',
#	'US.VI'=>'VI.VI.VI',
#	'VC.ZF'=>'VC.VC.ZF',
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
	'NPA NXX Record'=>sub{
	},
	'Line From'=>sub{
	},
	'Line To'=>sub{
	},
	'Wireless Block'=>sub{
	},
	'NXX Type'=>sub{
	},
	'Portable Block'=>sub{
	},
	'1,000 Block Pooling'=>sub{
	},
	'Block Contaminated'=>sub{
	},
	'Block Retained'=>sub{
	},
	'Date Assigned'=>sub{
	},
	'Effective Date'=>sub{
	},
	'Date Activated'=>sub{
	},
	'Last Modified'=>sub{
	},
	'Common Name'=>sub{
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'OCN Type'=>sub{
	},
	'Carrier Name'=>sub{
	},
	'Abbreviation'=>sub{
	},
	'DBA'=>sub{
	},
	'FKA'=>sub{
	},
	'Carrier Address'=>sub{
	},
	'Carrier City'=>sub{
	},
	'Carrier State'=>sub{
	},
	'Carrier Zip'=>sub{
	},
	'Carrier Country'=>sub{
	},
	'Wire Center LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCLATA} = $val if length($val);
	},
	'Other switches in WC'=>sub{
	},
	'Wire Center Address'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCADDR} = $val if length($val);
	},
	'Wire Center County'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCCOUNTY} = $val if length($val);
	},
	'Wire Center City'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCCITY} = $val if length($val);
	},
	'Class 4/5 Switch'=>sub{
	},
	'Wire Center State'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{ST} = $val if length($val);
	},
	'Trunk Gateway'=>sub{
	},
	'Point Code'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{SPC} = $val if length($val);
	},
	'Switch type'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{TYPE} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}): switch equipment type is '$val'\n";
			}
		}
	},
	'Tandem IntraLATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMILT} = $val if length($val);
	},
	'Tandem Feature Group C'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMFGC} = $val if length($val);
	},
	'Tandem Feature Group D'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMFGD} = $val if length($val);
	},
	'Tandem Local'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMLCL} = $val if length($val);
	},
	'Tandem Feature Group B'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMFGB} = $val if length($val);
	},
	'Tandem Operator Services'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TDMOPS} = $val if length($val);
	},
	'Wire Center Zip'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCZIP} = $val if length($val);
	},
	'SS7 STP 1'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{STP1} = $val if length($val);
	},
	'Wire Center Google maps'=>sub{
	},
	'Wire Center Latitude/Longitude'=>sub{
#		my ($dat,$fld,$val) = @_;
#		if (length($val)) {
#			my ($la,$lo) = split(/,/,$val);
#			if ($la and $lo) {
#				$dat->{LL} = $dat->{WCLL} = join(',',$la,$lo);
#				$dat->{VH} = $dat->{WCVH} = sprintf('%05d,%05d',::ll2vh($la,$lo));
#			}
#		}
	},
	'Wire Center V&H'=>sub{
#		my ($dat,$fld,$val) = @_;
#		if (length($val)) {
#			my ($v,$h) = split(/,/,$val);
#			($v,$h) = (int($v),int($h));
#			if ($v and $h) {
#				$dat->{WCVH} = sprintf('%05d,%05d',$v,$h);
#				$dat->{WCLL} = join(',',::vh2ll($v,$h));
#				if ($dat->{VH}) {
#					if ($dat->{VH} ne $dat->{WCVH}) {
#						$dat->{VH} = $dat->{WCVH} = join(';',$dat->{VH},$dat->{WCVH});
#					}
#				} else {
#					$dat->{VH} = $dat->{WCVH};
#				}
#				if ($dat->{LL}) {
#					if ($dat->{LL} ne $dat->{WCLL2}) {
#						$dat->{LL} = $dat->{WCLL} = join(';',$dat->{LL},$dat->{WCLL2});
#					}
#				} else {
#					$dat->{LL} = $dat->{WCLL};
#				}
#			}
#		}
	},
	'Wire Center CLLI'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{WCCLLI} = $val;
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
	'SS7 STP 2'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{STP2} = $val if length($val);
	},
	'Actual Switch'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{ACTUAL} = $val if length($val);
	},
	'Wire Center Country'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{CC} = $val if length($val);
	},
	'Call Agent'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{AGENT} = $val if length($val);
	},
	'Host'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{HOST} = $val if length($val);
	},
	'Wire Center Name'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{NAME} = $val if length($val);
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'LATA Name'=>sub{
	},
	'Historical Region'=>sub{
	},
	'Area Codes in LATA'=>sub{
	},
	'Remark'=>sub{
	},
	'Rate Center LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCLATA} = $val if length($val);
	},
	'Rate Center Name'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCNAME} = $val if length($val);
	},
	'Rate Center State'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCST} = $val if length($val);
	},
	'Rate Center Country'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps) = ::getnxxccst($dat);
		if ($rg) {
			if ($pc ne $val) {
				::correct($dat,$fld,$val,$pc,'autocorrect by NPA-NXX');
			}
			if ($ps ne $dat->{RCST}) {
				::correct($dat,$fld,$dat->{RCST},$ps,'autocorrect by NPA-NXX');
			}
		} elsif (length($val) and length($dat->{RCST})) {
			($pc,$cc) = ($val,$val);
			($ps,$st,$rg) = ($dat->{RCST},$dat->{RCST},$dat->{RCST});
			($pc,$ps,$rg) = split(/\./,$cor{"$pc.$ps"}) if exists $cor{"$pc.$ps"};
			($cc,$st,$rg) = @{$::pcpsda{$pc}{$ps}} if exists $::pcpsda{$pc}{$ps};
			$rg = $::ccstrg{$cc}{$st} if exists $::ccstrg{$cc}{$st};
			$cc = $::lergcc{$rg} if exists $::lergcc{$rg};
			$st = $::lergst{$rg} if exists $::lergst{$rg};
			if ($pc ne $val) {
				::correct($dat,'Rate Center Country',$val,$pc,'autocorrect');
			}
			if ($ps ne $dat->{RCST}) {
				::correct($dat,'Rate Center State',$dat->{RCST},$ps,'autocorrect');
			}
		}
		$dat->{REGION} = $rg if $rg;
		$dat->{RCCC} = $cc if $cc;
		$dat->{RCST} = $st if $st;
	},
	'LERG Abbreviation'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCSHORT} = $val if length($val);
	},
	'Zone'=>sub{
	},
	'Zone Type'=>sub{
	},
	'Number Pooling'=>sub{
	},
	'Point Identifier'=>sub{
	},
	'Rate Step'=>sub{
	},
	'Area Codes in Rate Center'=>sub{
	},
	'Embedded Overlays'=>sub{
	},
	'Major V&H'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = split(/,/,$val);
			if ($v and $h) {
				$dat->{VH} = $dat->{RCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{LL} = $dat->{RCLL} = join(',',::vh2ll($v,$h));
			}
		}
	},
	'Minor V&H'=>sub{
	},
	'Rate Center Latitude/Longitude'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = split(/,/,$val);
			if ($la and $lo) {
				$dat->{RCVH} = sprintf('%05d,%05d',::ll2vh($la,$lo));
				$dat->{RCLL} = join(',',$la,$lo);
			}
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
	},
	'Rate Center Google maps'=>sub{
	},
	'Time Zone'=>sub{
	},
	'DST Recognized'=>sub{
	},
	'Location within Rate Center Type'=>sub{
	},
	'Location within Rate Center Name'=>sub{
	},
	'County or Equivalent'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCCOUNTY} = $val if length($val);
	},
	'Federal Feature ID'=>sub{
	},
	'FIPS County Code'=>sub{
	},
	'FIPS Place Code'=>sub{
	},
	'Land Area'=>sub{
	},
	'Water Area'=>sub{
	},
	'Population 2000'=>sub{
	},
	'Population 2009'=>sub{
	},
	'Rural-Urban Continuum Code'=>sub{
	},
	'CBSA'=>sub{
	},
	'CBSA Level'=>sub{
	},
	'County Relation to CBSA'=>sub{
	},
	'Metro Division'=>sub{
	},
	'Part of CSA'=>sub{
	},
	'Cellular Market'=>sub{
	},
	'Major Trading Area'=>sub{
	},
	'Basic Trading Area'=>sub{
	},
);

