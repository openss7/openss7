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
		$dat->{TYPE} = $val if length($val);
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
	},
	'Wire Center Google maps'=>sub{
	},
	'Wire Center Latitude/Longitude'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = split(/,/,$val);
			if ($la and $lo) {
				$dat->{LL} = $dat->{WCLL} = join(',',$la,$lo);
				$dat->{VH} = $dat->{WCVH} = sprintf('%05d,%05d',::ll2vh($la,$lo));
			}
		}
	},
	'Wire Center V&H'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = split(/,/,$val);
			($v,$h) = (int($v),int($h));
			if ($v and $h) {
				$dat->{WCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{WCLL} = join(',',::vh2ll($v,$h));
				if ($dat->{VH}) {
					if ($dat->{VH} ne $dat->{WCVH}) {
						$dat->{VH} = $dat->{WCVH} = join(';',$dat->{VH},$dat->{WCVH});
					}
				} else {
					$dat->{VH} = $dat->{WCVH};
				}
				if ($dat->{LL}) {
					if ($dat->{LL} ne $dat->{WCLL2}) {
						$dat->{LL} = $dat->{WCLL} = join(';',$dat->{LL},$dat->{WCLL2});
					}
				} else {
					$dat->{LL} = $dat->{WCLL};
				}
			}
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
			$dat->{SWCLLI} = $val;
			unless (exists $skipcllis{$val}) {
				$dat->{WCCLLI} = substr($val,0,8);
				$dat->{PLCLLI} = substr($val,0,6);
				$dat->{STCLLI} = substr($val,4,2);
				$dat->{CTCLLI} = substr($val,0,4);
				my $cs = $dat->{STCLLI};
				unless (exists $::cllicc{$cs}) {
					print STDERR "E: $dat->{NPA}-$dat->{NXX}($dat->{X}) CLLI '$val' has invalid state code '$cs'\n";
				}
				$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
				$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
				$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			}
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
		$dat->{LATA} = $val if length($val);
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
	},
	'Rate Center Name'=>sub{
	},
	'Rate Center State'=>sub{
	},
	'Rate Center Country'=>sub{
	},
	'LERG Abbreviation'=>sub{
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
	},
	'Minor V&H'=>sub{
	},
	'Rate Center Latitude/Longitude'=>sub{
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

