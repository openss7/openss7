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

my %fcount = ();

my @keys = (
	'NPA',
	'NXX',
	'X',
	'COUNTRY',
	'STATE',
	'CITY',
	'COUNTY',
	'LATITUDE',
	'LONGITUDE',
	'LATA',
	'TIMEZONE',
	'OBSERVES_DST',
	'COUNTY_POPULATION',
	'FIPS_COUNTY_CODE',
	'MSA_COUNTY_CODE',
	'PMSA_COUNTY_CODE',
	'CBSA_COUNTY_CODE',
	'ZIPCODE_POSTALCODE',
	'ZIPCODE_COUNT',
	'ZIPCODE_FREQUENCY',
	'NEW_NPA',
	'NXX_USE_TYPE',
	'NXX_INTRO_VERSION',
	'OVERLAY',
	'OCN',
	'COMPANY',
	'RATE_CENTER',
	'SWITCH_CLLI',
	'RC_VERTICAL',
	'RC_HORIZONTAL',
);

my @ocn_keys = (
	'OCN',
	'COMPANY',
);

my @sw_keys = (
	'CLLI',
	'NPA',
	'NXX',
	'SWVH',
	'SWLL',
	'LATA',
	'OCN',
);

my @wc_keys = (
	'WC',
	'NPA',
	'NXX',
	'WCVH',
	'WCLL',
	'LATA',
	'OCN',
);

my @rc_keys = (
	'REGION',
	'RCSHORT',
	'NPA',
	'NXX',
	'RCVH',
	'RCLL',
	'LATA',
	'OCN',
);

my %ocns = ();
my %sws = ();
my %wcs = ();
my %rcs = ();

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
					print STDERR "E: $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	if (my $sw = $data->{SWITCH_CLLI}) {
		$sws{$sw} = {} unless exists $sws{$sw};
		my $rec = $sws{$sw};
		$data->{CLLI} = $sw;
		$data->{SWVH} = sprintf('%05d,%05d',$data->{RC_VERTICAL},$data->{RC_HORIZONTAL});
		$data->{SWLL} = "$data->{LATITUDE},$data->{LONGITUDE}";
		foreach my $k (@sw_keys) {
			if ($k eq 'NPA' or $k eq 'LATA' or $k eq 'OCN') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}->$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: SW $sw $k changing from $rec->{$k} to $data->{$k}\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if (my $wc = substr($data->{SWITCH_CLLI},0,8)) {
		$wcs{$wc} = {} unless exists $wcs{$wc};
		my $rec = $wcs{$wc};
		$data->{WC} = $wc;
		$data->{WCVH} = sprintf('%05d,%05d',$data->{RC_VERTICAL},$data->{RC_HORIZONTAL});
		$data->{WCLL} = "$data->{LATITUDE},$data->{LONGITUDE}";
		foreach my $k (@wc_keys) {
			if ($k eq 'NPA' or $k eq 'LATA' or $k eq 'OCN') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}->$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: WC $wc $k changing from $rec->{$k} to $data->{$k}\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if (my $rc = $data->{RATE_CENTER}) {
		$rc = substr("\U$rc\E",0,10);
		if (my $st = $data->{STATE}) {
			$rcs{$st}{$rc} = {} unless exists $rcs{$st}{$rc};
			my $rec = $rcs{$st}{$rc};
			$data->{RCSHORT} = $rc;
			$data->{REGION} = $st;
			$data->{RCVH} = sprintf('%05d,%05d',$data->{RC_VERTICAL},$data->{RC_HORIZONTAL});
			$data->{RCLL} = "$data->{LATITUDE},$data->{LONGITUDE}";
			foreach my $k (@rc_keys) {
				if ($k eq 'NPA' or $k eq 'LATA' or $k eq 'OCN') {
					$rec->{$k}{$data->{$k}}++ if $data->{$k};
				} elsif ($k eq 'NXX') {
					$rec->{$k}{"$data->{NPA}->$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
				} else {
					if ($data->{$k}) {
						if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
							print STDERR "E: RC $st-$rc $k changing from $rec->{$k} to $data->{$k}\n";
						}
						$rec->{$k} = $data->{$k};
					}
				}
			}
		}
	}
}

my %mapping = (
	NPA=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	NXX=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	COUNTRY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	STATE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	CITY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	COUNTY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	LATITUDE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	LONGITUDE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	LATA=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	TIMEZONE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	OBSERVES_DST=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	COUNTY_POPULATION=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	FIPS_COUNTY_CODE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	MSA_COUNTY_CODE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	PMSA_COUNTY_CODE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	CBSA_COUNTY_CODE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	ZIPCODE_POSTALCODE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	ZIPCODE_COUNT=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	ZIPCODE_FREQUENCY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	NEW_NPA=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	NXX_USE_TYPE=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	NXX_INTRO_VERSION=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	OVERLAY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	OCN=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	COMPANY=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	RATE_CENTER=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	SWITCH_CLLI=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	RC_VERTICAL=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	RC_HORIZONTAL=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
$fn = "$datadir/AREACODEWORLD-GOLD-SAMPLES.zip";
print STDERR "I: processing $fn...\n";
open($fh,"unzip -p $fn AREACODEWORLD-GOLD-SAMPLE.CSV |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp; s/\r//g;
	s/^"//;
	s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		$header = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$tokens[$i],$fields[$i]);
		} else {
			print STDERR "W: no mapping for $fields[$i]\n";
		}
	}
	closerecord($data);
}
close($fh);
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
	print $of '"', join('","',@values), '"', "\n";
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
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach my $k (sort keys %rcs) {
	foreach my $k2 (sort keys %{$rcs{$k}}) {
		my $rec = $rcs{$k}{$k2};
		my @values = ();
		foreach (@rc_keys) {
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{$_}});
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);

