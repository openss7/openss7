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

my %db = ();
my %rcs = ();
my %sws = ();
my %wcs = ();
my %dbrc = ();
my %dbsw = ();
my %dbwc = ();

my @db_keys = (
	'NPA',
	'NXX',
	'X',
	'Type',
	'Carrier',
	'Switch',
	'Rate Center',
	'State',
);
my @rc_keys = (
	'RCCC',
	'RCST',
	'RCNAME',
	'REGION',
	'RCSHORT',
	'RCVH',
	'NPA',
	'NXX',
	'LATA',
);
my @dbrc_keys = (
	'NPA',
	'NXX',
	'REGION',
	'RCSHORT',
	'RCCC',
	'RCST',
	'RCNAME',
);
my @sw_keys = (
	'CLLI',
	'NPA',
	'NXX',
);
my @dbsw_keys = (
	'NPA',
	'NXX',
	'CLLI',
);
my @wc_keys = (
	'WC',
	'NPA',
	'NXX',
);
my @dbwc_keys = (
	'NPA',
	'NXX',
	'WC',
);

my %cllist = ();
my %cllicc = ();
my %cllirg = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^$/;
	my @tokens = split(/\t/,$_);
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
}
close($fh);

#$fn = "$datadir/db.csv";
#open($of,">",$fn) or die "can't open $fn";
#print $of '"NPA","NXX","X","Type","Carrier","Switch","Rate Center","State"',"\n";
print STDERR "I: reading results...\n";
open($fh,"xzgrep '^<tr>' results/*.xz | grep -v '<th' | grep '<a href' | cut -f2- -d: |") or die "can't process files";
while (<$fh>) { chomp;
	my $data = {};
	next unless /^<tr><td><a href=[0-9]{3}-[0-9]{3}>([0-9]{3})-([0-9]{3})-0000<\/a><\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td>/;
	#printf $of '"%s","%s","%s","%s","%s","%s","%s","%s"'."\n",$1,$2,'',$3,$4,$5,$6,$7;
	$data->{NPA} = $1;
	$data->{NXX} = $2;
	$data->{X} = '';
	$data->{Type} = $3;
	$data->{Carrier} = $4;
	$data->{Switch} = $5;
	$data->{'Rate Center'} = $6;
	$data->{State} = $7;
	if ($data->{NPA} and $data->{NXX}) {
		$db{$1}{$2} = {} unless exists $db{$1}{$2};
		my $rec = $db{$1}{$2};
		foreach my $k (@db_keys) {
			$rec->{$k} = $data->{$k} if $data->{$k};
		}
	}
	next if $data->{Switch} eq '__VARIOUS__';
	my $sw = $data->{Switch};
	my $wc = substr($data->{Switch},0,8);
	my $cl = substr($data->{Switch},4,2);
	my $rn = $data->{'Rate Center'};
	my $rc = substr("\U$rn\E",0,10) if length($rn)<=10;
	my ($cc,$st,$rg);
	if (exists $cllicc{$cl}) { $cc = $cllicc{$cl} } else { $cc = $cl }
	if (exists $cllist{$cl}) { $st = $cllist{$cl} } else { $st = $cl }
	if (exists $cllirg{$cl}) { $rg = $cllirg{$cl} } else { $rg = $cl }
	$data->{RCCC} = $cc;
	$data->{RCST} = $st;
	$data->{RCNAME} = $rn;
	$data->{REGION} = $rg;
	$data->{RCSHORT} = $rc;
	if ($sw) {
		$sws{$sw} = {} unless exists $sws{$sw};
		my $rec = $sws{$sw};
		$rec->{CLLI} = $sw;
		$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		$rec->{NXX}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		if ($data->{NPA} and $data->{NXX}) {
			$dbsw{$data->{NPA}}{$data->{NXX}} = {} unless exists $dbsw{$data->{NPA}}{$data->{NXX}};
			$rec = $dbsw{$data->{NPA}}{$data->{NXX}};
			foreach my $k (@dbsw_keys) {
				$rec->{$k} = $data->{$k} if $data->{$k};
			}
		}
	}
	if ($wc) {
		$wcs{$wc} = {} unless exists $wcs{$wc};
		my $rec = $wcs{$wc};
		$rec->{WC} = $wc;
		$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		$rec->{NXX}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		if ($data->{NPA} and $data->{NXX}) {
			$dbwc{$data->{NPA}}{$data->{NXX}} = {} unless exists $dbwc{$data->{NPA}}{$data->{NXX}};
			$rec = $dbwc{$data->{NPA}}{$data->{NXX}};
			foreach my $k (@dbwc_keys) {
				$rec->{$k} = $data->{$k} if $data->{$k};
			}
		}

	}
	if ($rn and $rg and $rg ne 'XX') {
		$rcs{$cc}{$st}{$rn} = {} unless exists $rcs{$cc}{$st}{$rn};
		my $rec = $rcs{$cc}{$st}{$rn};
		$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		$rec->{NXX}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		$rec->{RCCC} = $cc;
		$rec->{RCST} = $st;
		$rec->{RCNAME} = $rn;
		$rec->{REGION} = $rg;
		$rec->{RCSHORT} = $rc;
		if ($data->{NPA} and $data->{NXX}) {
			$dbrc{$data->{NPA}}{$data->{NXX}} = {} unless exists $dbrc{$data->{NPA}}{$data->{NXX}};
			$rec = $dbrc{$data->{NPA}}{$data->{NXX}};
			foreach my $k (@dbrc_keys) {
				$rec->{$k} = $data->{$k} if $data->{$k};
			}

		}
	}
}
close($fh);
#close($of);

$fn = "$datadir/db.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@db_keys),'"',"\n";
foreach my $npa (sort keys %db) {
	foreach my $nxx (sort keys %{$db{$npa}}) {
		my $rec = $db{$npa}{$nxx};
		my @values = ();
		foreach (@db_keys) {
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

$fn = "$datadir/db.sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@dbsw_keys),'"',"\n";
foreach my $npa (sort keys %dbsw) {
	foreach my $nxx (sort keys %{$dbsw{$npa}}) {
		my $rec = $dbsw{$npa}{$nxx};
		my @values = ();
		foreach (@dbsw_keys) {
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

$fn = "$datadir/db.wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@dbwc_keys),'"',"\n";
foreach my $npa (sort keys %dbwc) {
	foreach my $nxx (sort keys %{$dbwc{$npa}}) {
		my $rec = $dbwc{$npa}{$nxx};
		my @values = ();
		foreach (@dbwc_keys) {
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

$fn = "$datadir/sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@sw_keys),'"',"\n";
foreach my $sw (sort keys %sws) {
	my $rec = $sws{$sw};
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

$fn = "$datadir/wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@wc_keys),'"',"\n";
foreach my $wc (sort keys %wcs) {
	my $rec = $wcs{$wc};
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

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@rc_keys),'"',"\n";
foreach my $cc (sort keys %rcs) {
	foreach my $st (sort keys %{$rcs{$cc}}) {
		foreach my $rn (sort keys %{$rcs{$cc}{$st}}) {
			my $rec = $rcs{$cc}{$st}{$rn};
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
}
close($of);

