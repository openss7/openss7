package mapping;

our %cor = (
	'AG.AN'=>'AG.AG.AN',
	'BS.BA'=>'BS.BS.BA',
	'BB.BD'=>'BB.BB.BD',
	'VG.BV'=>'VG.VG.BV',
	'KY.CQ'=>'KY.KY.CQ',
	'DO.DR'=>'DO.DO.DR',
	'GD.GN'=>'GD.GD.GN',
	'US.GU'=>'GU.GU.GU',
	'KN.KA'=>'KN.KN.KA',
	'US.MP'=>'MP.MP.NN',
	'CA.NL'=>'CA.NL.NF',
	'CA.NU'=>'CA.NU.VU',
	'US.PR'=>'PR.PR.PR',
	'CA.QC'=>'CA.QC.PQ',
	'MS.RT'=>'MS.MS.RT',
	'LC.SA'=>'LC.LC.SA',
	'SX.SF'=>'SX.SX.SF',
	'US.SK'=>'CA.SK.SK',
	'TT.TR'=>'TT.TT.TR',
	'US.AS'=>'AS.AS.AS',
	'US.VI'=>'VI.VI.VI',
	'VC.ZF'=>'VC.VC.ZF',
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
	},
	'Other switches in WC'=>sub{
	},
	'Wire Center Address'=>sub{
	},
	'Wire Center County'=>sub{
	},
	'Wire Center City'=>sub{
	},
	'Class 4/5 Switch'=>sub{
	},
	'Wire Center State'=>sub{
	},
	'Trunk Gateway'=>sub{
	},
	'Point Code'=>sub{
	},
	'Switch type'=>sub{
	},
	'Tandem IntraLATA'=>sub{
	},
	'Tandem Feature Group C'=>sub{
	},
	'Tandem Feature Group D'=>sub{
	},
	'Tandem Local'=>sub{
	},
	'Tandem Feature Group B'=>sub{
	},
	'Tandem Operator Services'=>sub{
	},
	'Wire Center Zip'=>sub{
	},
	'SS7 STP 1'=>sub{
	},
	'Wire Center Google maps'=>sub{
	},
	'Wire Center Latitude/Longitude'=>sub{
	},
	'Wire Center V&H'=>sub{
	},
	'Wire Center CLLI'=>sub{
	},
	'Switch'=>sub{
	},
	'SS7 STP 2'=>sub{
	},
	'Actual Switch'=>sub{
	},
	'Wire Center Country'=>sub{
	},
	'Call Agent'=>sub{
	},
	'Host'=>sub{
	},
	'Wire Center Name'=>sub{
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
		$dat->{LATA} = $val if length($val);
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
		my $cc = $val;
		my $st = $dat->{RCST};
		my $rg = $st;
		($cc,$st,$rg) = split(/\./,$cor{"$cc.$st"}) if exists $cor{"$cc.$st"};
		$cc = $::lergcc{$rg} if exists $::lergcc{$rg};
		$st = $::lergst{$rg} if exists $::lergst{$rg};
		if ($cc and $val and $cc ne $val) {
			print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}) Country: $val \-> $cc\n";
		}
		if ($st and $dat->{RCST} and $st ne $dat->{RCST}) {
			print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}) State: $dat->{RCST} \-> $st\n";
		}
		$dat->{RCCC} = $cc;
		$dat->{RCST} = $st;
		$dat->{REGION} = $rg;
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

