package mapping;

our %fieldsource = (
	'OCN'=>'OCN',
	'WCLATA'=>'Wire Center LATA',
	'WCADDR'=>'Wire Center Address',
	'WCCOUNTY'=>'Wire Center County',
	'WCCITY'=>'Wire Center City',
	'WCST'=>'Wire Center State',
	'SPC'=>'Point Code',
	'SWTYPE'=>'Switch type',
	'TDMILT'=>'Tandem IntraLATA',
	'TDMFGB'=>'Tandem Feature Group B',
	'TDMFGC'=>'Tandem Feature Group C',
	'TDMFGD'=>'Tandem Feature Group D',
	'TDMLCL'=>'Tandem Local',
	'TDMOPS'=>'Tandem Operator Services',
	'WCZIP'=>'Wire Center Zip',
	'STP1'=>'SS7 STP 1',
	'STP2'=>'SS7 STP 2',
	'WCLL'=>'Wire Center Latitude/Longitude',
	'WCVH'=>'Wire Center V&H',
	'WCCLLI'=>'Wire Center CLLI',
	'SWCLLI'=>'Switch',
	'ACTUAL'=>'Actual Switch',
	'WCCC'=>'Wire Center Country',
	'AGENT'=>'Call Agent',
	'HOST'=>'Host',
	'WCNAME'=>'Wire Center Name',
	'LATA'=>'LATA',
	'RCLATA'=>'Rate Center LATA',
	'RCNAME'=>'Rate Center Name',
	'RCST'=>'Rate Center State',
	'RCCC'=>'Rate Center Country',
	'RCSHORT'=>'LERG Abbreviation',
	'RCVH'=>'Major V&H',
	'RCLL'=>'Rate Center Latitude/Longitude',
	'RCCOUNTY'=>'County or Equivalent',
	'FDATE'=>'File Updated',
);

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
	'XXXX'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'YYYY'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'NPA NXX Record'=>sub{
	},
	'Line From'=>sub{
	},
	'Line To'=>sub{
	},
	'Numbers From'=>sub{
	},
	'Numbers To'=>sub{
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
		my ($dat,$fld,$val) = @_;
		$dat->{UDATE} = $val if length($val);
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
		my ($dat,$fld,$val) = @_;
		::addclli($dat,undef,$val) if length($val);
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
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'CLS45SW',$val,'CLS45SW') if length($val);
		$dat->{SWFUNC}{CLASS45}++ if length($val);
	},
	'Wire Center State'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCST} = $val if length($val);
	},
	'Trunk Gateway'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{SWFUNC}{TRUNKGW}++ if length($val);
	},
	'Point Code'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{SPC} = $val if length($val);
	},
	'Switch type'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			$dat->{SWTYPE} = $val;
			if ($dat->{BADCLLI}) {
				print STDERR "W: ",::getpfx($dat),": switch equipment type is '$val'\n";
			}
		}
	},
	'Tandem IntraLATA'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMILT',$val,'TDMILT') if length($val);
	},
	'Tandem Feature Group C'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMFGC',$val,'TDMFGC') if length($val);
	},
	'Tandem Feature Group D'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMFGD',$val,'TDMFGD') if length($val);
	},
	'Tandem Local'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMLCL',$val,'TDMLCL') if length($val);
	},
	'Tandem Feature Group B'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMFGB',$val,'TDMFGB') if length($val);
	},
	'Tandem Operator Services'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'TDMOPS',$val,'TDMOPS') if length($val);
	},
	'Wire Center Zip'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCZIP} = $val if length($val);
	},
	'SS7 STP 1'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			::addclli($dat,'STP1',$val,'STP1');
			$dat->{SWFUNC}{SSP}++;
		}
	},
	'Wire Center Google maps'=>sub{
	},
	'Wire Center Latitude/Longitude'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = split(/,/,$val);
			$dat->{WCLL} = join(',',$la,$lo) if $la and $lo;
		}
	},
	'Wire Center V&H'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = split(/,/,$val);
			$dat->{WCVH} = sprintf('%05d,%05d',$v,$h) if $v and $h;
		}
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
			return unless ::checkclli($dat,'SWCLLI',$val);
			$dat->{SWCLLI} = $val;
			if (length($dat->{WCCLLI}) and $dat->{WCCLLI} ne substr($val,0,8)) {
				print STDERR "E: ",::getpfx($dat),": CLLI $val: wire center mismatch '$dat->{WCCLLI}' and '", substr($val,0,8), "'\n";
			} else {
				$dat->{WCCLLI} = substr($val,0,8);
				$dat->{PLCLLI} = substr($val,0,6);
				$dat->{CTCLLI} = substr($val,0,4);
			}
			my $cs = $dat->{STCLLI} = substr($val,4,2);
			$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
			$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
			$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no CC for CLLI region $cs\n" unless exists $::cllicc{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no ST for CLLI region $cs\n" unless exists $::cllist{$cs};
			print STDERR "E: ",::getpfx($dat),": CLLI $val: no RG for CLLI region $cs\n" unless exists $::cllirg{$cs};
		}
	},
	'SS7 STP 2'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			::addclli($dat,'STP2',$val,'STP2');
			$dat->{SWFUNC}{SSP}++;
		}
	},
	'Actual Switch'=>sub{
		my ($dat,$fld,$val) = @_;
		::addclli($dat,'ACTUAL',$val,'ACTUAL') if length($val);
	},
	'Wire Center Country'=>sub{
		my ($dat,$fld,$val) = @_;
		my ($cc,$st,$rg,$pc,$ps);
		if (length($dat->{WCRG})) {
			$rg = $dat->{WCRG};
			($pc,$ps) = @{$::lergps{$rg}} if exists $::lergps{$rg};
			if ($pc) {
				if ($pc ne 'US' and $pc ne 'CA') {
					$pc = $rg;
				}
				if ($pc ne $val) {
					::correct(undef,$dat,$fld,$val,$pc,'autocorrect','DB',$dat->{NPA},$dat->{NXX});
				}
			}
		} elsif (length($val) and length($dat->{WCST})) {
			($pc,$cc) = ($val,$val);
			($ps,$st,$rg) = ($dat->{WCST},$dat->{WCST},$dat->{WCST});
			($pc,$ps,$rg) = split(/\./,$cor{"$pc.$ps"}) if exists $cor{"$pc.$ps"};
			($cc,$st,$rg) = @{$::pcpsda{$pc}{$ps}} if exists $::pcpsda{$pc}{$ps};
			$rg = $::ccstrg{$cc}{$st} if exists $::ccstrg{$cc}{$st};
			$cc = $::lergcc{$rg} if exists $::lergcc{$rg};
			$st = $::lergst{$rg} if exists $::lergst{$rg};
			if ($pc ne $val) {
				::correct(undef,$dat,$fld,$val,$pc,'autocorrect','DB',$dat->{NPA},$dat->{NXX});
			}
			if ($ps ne $dat->{WCST}) {
				::correct(undef,$dat,'Wire Center State',$dat->{WCST},$ps,'autocorrect','DB',$dat->{NPA},$dat->{NXX});
			}
		}
		$dat->{WCRG} = $rg if $rg;
		$dat->{WCCC} = $cc if $cc;
		$dat->{WCST} = $st if $st;
	},
	'Call Agent'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			::addclli($dat,'AGENT',$val,'AGENT');
			$dat->{SWFUNC}{GATEWAY}++;
		}
	},
	'Host'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			::addclli($dat,'HOST',$val,'HOST');
			$dat->{SWFUNC}{REMOTE}++;
		}
	},
	'Wire Center Name'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{WCNAME} = $val if length($val);
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
				::correct(undef,$dat,$fld,$val,$pc,'autocorrect by NPA-NXX','DB',$dat->{NPA},$dat->{NXX});
			}
			if ($ps ne $dat->{RCST}) {
				::correct(undef,$dat,'Rate Center State',$dat->{RCST},$ps,'autocorrect by NPA-NXX','DB',$dat->{NPA},$dat->{NXX});
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
				::correct(undef,$dat,$fld,$val,$pc,'autocorrect','DB',$dat->{NPA},$dat->{NXX});
			}
			if ($ps ne $dat->{RCST}) {
				::correct(undef,$dat,'Rate Center State',$dat->{RCST},$ps,'autocorrect','DB',$dat->{NPA},$dat->{NXX});
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
			$dat->{RCVH} = sprintf('%05d,%05d',$v,$h) if $v and $h;
		}
	},
	'Minor V&H'=>sub{
	},
	'Rate Center Latitude/Longitude'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = split(/,/,$val);
			$dat->{RCLL} = join(',',$la,$lo) if $la and $lo;
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
	'File Updated'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{FDATE} = $val if length($val);
	},
);

