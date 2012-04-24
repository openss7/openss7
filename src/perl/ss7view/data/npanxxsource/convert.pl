#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my $counts = {};

my @keys = (
	'NPA',
	'NXX',
	'X',
	'NPA NXX Record',
	'Line From',
	'Line To',
	'Wireless Block',
	'NXX Type',
	'Portable Block',
	'1,000 Block Pooling',
	'Block Contaminated',
	'Block Retained',
	'Date Assigned',
	'Effective Date',
	'Date Activated',
	'Last Modified',
	'Common Name',
	'OCN',
	'OCN Type',
	'Carrier Name',
	'Abbreviation',
	'DBA',
	'FKA',
	'Carrier Address',
	'Carrier City',
	'Carrier State',
	'Carrier Zip',
	'Carrier Country',
	'Wire Center LATA',
	'Other switches in WC',
	'Wire Center Address',
	'Wire Center County',
	'Wire Center City',
	'Class 4/5 Switch',
	'Wire Center State',
	'Trunk Gateway',
	'Point Code',
	'Switch type',
	'Tandem IntraLATA',
	'Tandem Feature Group C',
	'Tandem Feature Group D',
	'Tandem Local',
	'Tandem Feature Group B',
	'Tandem Operator Services',
	'Wire Center Zip',
	'SS7 STP 1',
	'Wire Center Google maps',
	'Wire Center Latitude/Longitude',
	'Wire Center V&H',
	'Wire Center CLLI',
	'Switch',
	'SS7 STP 2',
	'Actual Switch',
	'Wire Center Country',
	'Call Agent',
	'Host',
	'Wire Center Name',
	'LATA',
	'LATA Name',
	'Historical Region',
	'Area Codes in LATA',
	'Remark',
	'Rate Center LATA',
	'Rate Center Name',
	'Rate Center State',
	'Rate Center Country',
	'LERG Abbreviation',
	'Zone',
	'Zone Type',
	'Number Pooling',
	'Point Identifier',
	'Rate Step',
	'Area Codes in Rate Center',
	'Embedded Overlays',
	'Major V&H',
	'Minor V&H',
	'Rate Center Latitude/Longitude',
	'Rate Center Google maps',
	'Time Zone',
	'DST Recognized',
	'Location within Rate Center Type',
	'Location within Rate Center Name',
	'County or Equivalent',
	'Federal Feature ID',
	'FIPS County Code',
	'FIPS Place Code',
	'Land Area',
	'Water Area',
	'Population 2000',
	'Population 2009',
	'Rural-Urban Continuum Code',
	'CBSA',
	'CBSA Level',
	'County Relation to CBSA',
	'Metro Division',
	'Part of CSA',
	'Cellular Market',
	'Major Trading Area',
	'Basic Trading Area',
);

my @ocn_keys = (
	'OCN',
	'OCN Type',
	'Common Name',
	'Carrier Name',
	'Abbreviation',
	'DBA',
	'FKA',
	'Carrier Address',
	'Carrier City',
	'Carrier State',
	'Carrier Zip',
	'Carrier Country',
);

my @sw_keys = (
	'CLLI',
	'NPA',
	'NXX',
	'LATA',
	'OCN',
	'Class 4/5 Switch',
	'Trunk Gateway',
	'Point Code',
	'Switch type',
	'SS7 STP 1',
	'SS7 STP 2',
	'Actual Switch',
	'Call Agent',
	'Host',
	'Wire Center LATA',
	'Other switches in WC',
	'Wire Center Address',
	'Wire Center County',
	'Wire Center City',
	'Wire Center State',
	'Wire Center Zip',
	'Wire Center Latitude/Longitude',
	'Wire Center V&H',
	'Wire Center CLLI',
	'Wire Center Country',
	'Wire Center Name',
	'Tandem IntraLATA',
	'Tandem Feature Group C',
	'Tandem Feature Group D',
	'Tandem Local',
	'Tandem Feature Group B',
	'Tandem Operator Services',
	'Is Call Agent',
	'Is Trunk Gateway',
	'Is Class 4/5 Switch',
	'Is SS7 STP',
	'Is Host',
	'Is Tandem Local',
	'Is Tandem IntraLATA',
	'Is Tandem Feature Group B',
	'Is Tandem Feature Group C',
	'Is Tandem Feature Group D',
	'Is Tandem Operator Services',
	'Is Remote',
	'Is Feature Group B EO',
	'Is Feature Group C EO',
	'Is Feature Group D EO',
);

my @wc_keys = (
	'WC',
	'NPA',
	'NXX',
	'LATA',
	'OCN',
	'Wire Center Address',
	'Wire Center County',
	'Wire Center City',
	'Wire Center State',
	'Wire Center Zip',
	'Wire Center Latitude/Longitude',
	'Wire Center V&H',
	'Wire Center Country',
	'Wire Center Name',
);

my @rc_keys = (
	'RCCC',
	'RCST',
	'RCNAME',
	'REGION',
	'RCSHORT',
	'RCVH',
	'RCLL',
	'NPA',
	'NXX',
	'LATA',
	'OCN',
	'Zone',
	'Rate Center LATA',
	'Rate Center Name',
	'Rate Center State',
	'Rate Center Country',
	'Zone Type',
	'Number Pooling',
	'Point Identifier',
	'Rate Step',
	'Area Codes in Rate Center',
	'Embedded Overlays',
	'Major V&H',
	'Minor V&H',
	'Rate Center Latitude/Longitude',
	'Rate Center Google maps',
	'Time Zone',
	'DST Recognized',
);

my %feat_keys = (
	'Class 4/5 Switch'=>'Is Class 4/5 Switch',
	'Trunk Gateway'=>'Is Trunk Gateway',
	'SS7 STP 1'=>'Is SS7 STP',
	'SS7 STP 2'=>'Is SS7 STP',
	'Call Agent'=>'Is Call Agent',
	'Host'=>'Is Host',
	'Tandem IntraLATA'=>'Is Tandem IntraLATA',
	'Tandem Feature Group B'=>'Is Tandem Feature Group B',
	'Tandem Feature Group C'=>'Is Tandem Feature Group C',
	'Tandem Feature Group D'=>'Is Tandem Feature Group D',
	'Tandem Local'=>'Is Tandem Local',
	'Tandem Operator Services'=>'Is Tandem Operator Services',
);

my %this_keys = (
	'Class 4/5 Switch'=>'Is Class 4/5 Switch',
	'Host'=>'Is Remote',
	'Tandem Feature Group B'=>'Is Feature Group B EO',
	'Tandem Feature Group C'=>'Is Feature Group C EO',
	'Tandem Feature Group D'=>'Is Feature Group D EO',
);

my %ocns = ();
my %sws = ();
my %wcs = ();
my %rcs = ();

my %dbsw = ();
my %dbwc = ();
my %dbrc = ();

sub closerecord {
	my $data = shift;
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
	if (my $ocn = $data->{OCN}) {
		$ocns{$ocn} = {} unless exists $ocns{$ocn};
		my $rec = $ocns{$ocn};
		$rec->{OCN} = $ocn;
		for (my $i=1;$i<@ocn_keys;$i++) {
			my $k = $ocn_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: OCN $ocn $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	if (my $sw = $data->{Switch}) {
		$sws{$sw} = {} unless exists $sws{$sw};
		my $rec = $sws{$sw};
		$data->{CLLI} = $sw;
		foreach my $k (@sw_keys) {
			if ($k eq 'NPA' or $k eq 'LATA' or $k eq 'OCN') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: SW $sw $k changing from $rec->{$k} to $data->{$k}\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
		foreach my $k (keys %feat_keys) {
			if ($data->{$k} and length($data->{$k}) == 11) {
				$sws{$data->{$k}}{CLLI} = $data->{$k};
				$sws{$data->{$k}}{$feat_keys{$k}} = 'X';
			}
		}
		foreach my $k (keys %this_keys) {
			if ($data->{$k} and length($data->{$k}) == 11) {
				$rec->{$this_keys{$k}} = 'X';
			}
		}
		if ($data->{NPA} and $data->{NXX}) {
			$dbsw{$data->{NPA}}{$data->{NXX}} = {} unless exists
			$dbsw{$data->{NPA}}{$data->{NXX}};
			my $rec = $dbsw{$data->{NPA}}{$data->{NXX}};
			$rec->{NPA} = $data->{NPA};
			$rec->{NXX} = $data->{NXX};
			$rec->{CLLI} = $sw;
		}
	}
	if (my $wc = substr($data->{Switch},0,8)) {
		$wcs{$wc} = {} unless exists $wcs{$wc};
		my $rec = $wcs{$wc};
		$data->{WC} = $wc;
		$data->{LATA} = $data->{'Wire Center LATA'};
		foreach my $k (@wc_keys) {
			if ($k eq 'NPA' or $k eq 'LATA' or $k eq 'OCN') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: WC $wc $k changing from $rec->{$k} to $data->{$k}\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
		if ($data->{NPA} and $data->{NXX}) {
			$dbwc{$data->{NPA}}{$data->{NXX}} = {} unless exists
			$dbwc{$data->{NPA}}{$data->{NXX}};
			my $rec = $dbwc{$data->{NPA}}{$data->{NXX}};
			$rec->{NPA} = $data->{NPA};
			$rec->{NXX} = $data->{NXX};
			$rec->{WC} = $wc;
		}
	}
	my %cor = (
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
	my $st = $data->{'Rate Center State'};
	my $cc = $data->{'Rate Center Country'};
	my $rg = $st;
	($cc,$st,$rg) = split(/\./,$cor{"$cc.$st"}) if exists $cor{"$cc.$st"};
	my $rc = $data->{'LERG Abbreviation'};
	if ($rc and $rc ne 'XXXXXXXXXX') {
		if ($rg) {
			$rcs{$rg}{$rc} = {} unless exists $rcs{$rg}{$rc};
			my $rec = $rcs{$rg}{$rc};
			$data->{RCSHORT} = $rc;
			$data->{REGION} = $rg;
			$data->{RCCC} = $cc;
			$data->{RCST} = $st;
			$rec->{RCNAME} = $data->{'Rate Center Name'};
			$data->{LATA} = $data->{'Rate Center LATA'};
			if ($data->{'Major V&H'}) {
				$data->{RCVH} = sprintf "%05d,%05d", split(/,/,$data->{'Major V&H'});
			}
			$data->{RCLL} = $data->{'Rate Center Latitude/Longitude'};
			foreach my $k (@rc_keys) {
				if ($k eq 'NPA') {
					$rec->{$k}{$data->{$k}}++ if $data->{$k};
				} elsif ($k eq 'NXX') {
					$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
				} else {
					if ($data->{$k}) {
						if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
							print STDERR "E: RC $rc $k changing from $rec->{$k} to $data->{$k}\n";
						}
						$rec->{$k} = $data->{$k};
					}
				}
			}
		}
	}
	if ($data->{NPA} and $data->{NXX}) {
		$dbrc{$data->{NPA}}{$data->{NXX}} = {} unless exists $dbrc{$data->{NPA}}{$data->{NXX}};
		my $rec = $dbrc{$data->{NPA}}{$data->{NXX}};
		$rec->{NPA} = $data->{NPA};
		$rec->{NXX} = $data->{NXX};
		$rec->{REGION} = $rg;
		$rec->{RCSHORT} = $data->{'LERG Abbreviation'};
		$rec->{RCNAME} = $data->{'Rate Center Name'};
		$rec->{RCCC} = $cc;
		$rec->{RCST} = $st;
	}
}

my %fcount = {};

my %mapping = (
	'NPA NXX'=>{
		'NPA NXX'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			if ($val =~ /([0-9]{3})-([0-9]{3})(-([0-9A]))?/) {
				$fcount{NPA}{count}++;
				$data->{NPA} = $1;
				$fcount{NXX}{count}++;
				$data->{NXX} = $2;
				if ($4) {
					$fcount{X}{count}++;
					$data->{X} = $4;
				}
			}
		},
		'NPA NXX Record'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
		},
		'Line From'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Line To'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Wireless Block'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'NXX Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Portable Block'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'1,000 Block Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Block Contaminated'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Block Retained'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Date Assigned'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Effective Date'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Date Activated'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Last Modified'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
	},
	'Carrier'=>{
		'Common Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'OCN'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'OCN Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'DBA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'FKA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
	},
	'Wire Center'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$fcount{$fld}{value}{$val}++;
			$data->{$fld} = $val;
		},
		'Other switches in WC'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'County'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Operator Services'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Class 4/5 Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Trunk Gateway'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Point Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$fcount{$fld}{value}{$val}++;
			$data->{$fld} = $val;
		},
		'Switch type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Tandems'=>{
			'IntraLATA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Feature Group C'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Feature Group D'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Local'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Feature Group B'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Operator Services'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'SS7 STP 1'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Coordinates'=>{
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
		'Wire Center CLLI'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'SS7 STP 2'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Actual Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Call Agent'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Host'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Wire Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
	},
	'LATA'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'LATA '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Historical Region'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Area Codes in LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Remark'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
	},
	'Rate Center - Locality'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$fcount{$fld}{count}++;
			$fcount{$fld}{value}{$val}++;
			$data->{$fld} = $val;
		},
		'Rate Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'LERG Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Zone'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Zone Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Number Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Point Identifier'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		}, # Canada only
		'Rate Step'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Area Codes in Rate Center'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Embedded Overlays'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Coordinates'=>{
			'Major V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Minor V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Rate Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Rate Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
		'Time Zone'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'DST Recognized'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Location within Rate Center'=>{
			'Type'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Location within Rate Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Name'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Location within Rate Center '.$fld;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
		'County or Equivalent'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Federal Feature ID'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'FIPS County Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'FIPS Place Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Land Area'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Water Area'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Latitude/Longitude'=>sub{
		},
		'Population 2000'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Population 2009'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Rural-Urban Continuum Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fcount{$fld}{count}++;
			$data->{$fld} = $val;
		},
		'Core Based Statistical Area'=>{
			'CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'CBSA Level'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'County Relation to CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Metro Division'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Part of CSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
		'Market Area'=>{
			'Cellular Market'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Major Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
			'Basic Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fcount{$fld}{count}++;
				$data->{$fld} = $val;
			},
		},
	},
);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","', @keys), '"', "\n";
my @files = `find $datadir -name '*.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn\n";
	open($fh,"xzcat $fn |") or die "can't open $fn";
	my @lines = ();
	while (<$fh>) { chomp; s/\r//g;
		push @lines, $_;
	}
	close($fh);
	my $entry = join('',@lines);
	my $data = {};
	my ($hdr,$key,$val);
	while ($entry =~ /(<h3>(.*?)<\/h3>|<div class="n30">(.*?)<\/div>[[:space:]]*<div class="n70">(.*?)<\/div>|<div class="n100">(&nbsp;)<\/div>)/g) {
		if ($2) {
			my $tit = $2;
			my $npa;
			if ($tit =~ /^NPA NXX ([0-9]+-[0-9]+)$/) {
				$tit = 'NPA NXX';
				$npa = $1;
			}
			$key = $tit;
			$hdr = undef;
			if ($npa) {
				if (exists $mapping{$key}{$tit}) {
					&{$mapping{$key}{$tit}}($data,$npa,$tit);
				} else {
					print STDERR "E: no mapping for '$key', '$tit'\n";
				}
			}
		} elsif ($5) {
			$hdr = undef;
		} else {
			my $fld = $3;
			my $val = $4;
			my $sub;
			$fld =~ s/\&amp;/\&/g;
			$val =~ s/\&amp;/\&/g;
			if ($fld =~ /^CBSA ([0-9]+)$/) {
				$fld = 'CBSA';
				$sub = $1;
			} elsif ($fld =~ /^Part of CSA ([0-9]+)$/) {
				$fld = 'Part of CSA';
				$sub = $1;
			} elsif ($fld =~ /^Metro Division ([0-9]+)$/) {
				$fld = 'Metro Division';
				$sub = $1;
			} elsif ($fld =~ /^Rural-Urban Continuum Code ([0-9]+)$/) {
				$fld = 'Rural-Urban Continuum Code';
				$sub = $1;
#			} elsif ($fld =~ /^Population ([0-9]+)$/) {
#				$fld = 'Population';
#				$sub = $1;
			}
			if ($fld eq '&nbsp;') {
				if ($val =~ /<em>(.*?)<\/em>/) {
					$hdr = $1;
				} elsif ($val =~ /<a href/) {
					$fld = 'Google maps';
				} else {
					$fld = 'Remark';
				}
			}
			unless ($fld eq '&nbsp;') {
				$val = "$sub $val" if $sub;
				if ($hdr) {
					if (exists $mapping{$key}{$hdr}{$fld}) {
						&{$mapping{$key}{$hdr}{$fld}}($data,$val,$fld);
					} else {
						print STDERR "E: no mapping for '$key', '$hdr', '$fld'\n";
					}
				} else {
					if (exists $mapping{$key}{$fld}) {
						&{$mapping{$key}{$fld}}($data,$val,$fld);
					} else {
						print STDERR "E: no mapping for '$key', '$fld'\n";
					}
				}
			}
		}
	}
	closerecord($data);
}
close($of);

$fn = "$datadir/ocn.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@ocn_keys), '"', "\n";
foreach my $k (sort keys %ocns) {
	my $ocn = $ocns{$k};
	my @values = ();
	foreach (@ocn_keys) { push @values, $ocn->{$_} }
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@sw_keys), '"', "\n";
foreach my $k (sort keys %sws) {
	my $rec = $sws{$k};
	my @values = ();
	foreach (@sw_keys) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			push @values, join(',',sort keys %{$rec->{$_}});
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

$fn = "$datadir/db.sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/NPA NXX CLLI/),'"',"\n";
foreach my $npa (sort keys %dbsw) {
	foreach my $nxx (sort keys %{$dbsw{$npa}}) {
		my $rec = $dbsw{$npa}{$nxx};
		print $of '"',join('","',($npa,$nxx,$rec->{CLLI})),'"',"\n";
	}
}
close($of);

$fn = "$datadir/wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@wc_keys), '"', "\n";
foreach my $k (sort keys %wcs) {
	my $rec = $wcs{$k};
	my @values = ();
	foreach (@wc_keys) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			push @values, join(',',sort keys %{$rec->{$_}});
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

$fn = "$datadir/db.wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/NPA NXX WC/),'"',"\n";
foreach my $npa (sort keys %dbwc) {
	foreach my $nxx (sort keys %{$dbwc{$npa}}) {
		my $rec = $dbwc{$npa}{$nxx};
		print $of '"',join('","',($npa,$nxx,$rec->{WC})),'"',"\n";
	}
}
close($of);

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach my $rg (sort keys %rcs) {
	foreach my $rc (sort keys %{$rcs{$rg}}) {
		my $rec = $rcs{$rg}{$rc};
		my @values = ();
		foreach (@rc_keys) {
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{$_}});
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

my @dbrc_keys = (
	'NPA',
	'NXX',
	'REGION',
	'RCSHORT',
	'RCCC',
	'RCST',
	'RCNAME',
);

$fn = "$datadir/db.rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@dbrc_keys),'"',"\n";
foreach my $npa (sort keys %dbrc) {
	foreach my $nxx (sort keys %{$dbrc{$npa}}) {
		my $rec = $dbrc{$npa}{$nxx};
		my @values = ();
		foreach (@dbrc_keys) {
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{$_}});
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);


foreach (@keys) {
	my $count = $fcount{$_}{count} if exists $fcount{$_}{count};
	printf STDERR "F: %9d '$_'\n", $count;
	if (exists $fcount{$_}{value}) {
		foreach my $val (sort keys %{$fcount{$_}{value}}) {
			printf STDERR "V:           %9d '%s'\n", $fcount{$_}{value}{$val}, $val;
		}
	}
}
