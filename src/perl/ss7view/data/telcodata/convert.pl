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

my $names = [
	'dummy',
	'NPA NXX',
	'State',
	'Company',
	'OCN',
	'Rate Center',
	'Switch',
	'Date',
	'Prefix Type',
	'Switch Name',
	'Switch Type',
	'LATA',
	'Tandem',
];

my @keys = (
	'NPA',
	'NXX',
	'X',
	'State',
	'Company',
	'OCN',
	'Rate Center',
	'Switch',
	'Date',
	'Prefix Type',
	'Switch Name',
	'Switch Type',
	'LATA',
	'Tandem',
);

my @ocn_keys = (
	'OCN',
	'Company',
);

my @clli_keys = (
	'CLLI',
	'LATA',
	'Switch Name',
	'Switch Type',
	'Tandem',
	'Is Tandem',
);

my @rc_keys = (
	'RCSHORT',
	'REGION',
	'RCVH',
	'NPA',
	'LATA',
);

my %feat_keys = (
	'Tandem'=>'Is Tandem',
);

my %ocns = ();
my %cllis = ();
my %rcs = ();

my %nanpst = ();

$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	my ($npa,$cc,$st,$name) = split(/\t/,$_);
	$nanpst{$npa} = $st;
}
close($fh);

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
	my $clli = $data->{Switch};
	if ($clli and $clli ne 'NOCLLIKNOWN' and $clli ne 'XXXXXXXXXXX') {
		$cllis{$clli} = {} unless exists $cllis{$clli};
		my $rec = $cllis{$clli};
		$rec->{CLLI} = $clli;
		for (my $i=1;$i<@clli_keys;$i++) {
			my $k = $clli_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: CLLI $clli $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
		foreach my $k (keys %feat_keys) {
			if ($data->{$k} and length($data->{$k}) == 11) {
				$cllis{$data->{$k}}{CLLI} = $data->{$k};
				$cllis{$data->{$k}}{$feat_keys{$k}} = 'X';
			}
		}
	}
	if (my $rc = $data->{'Rate Center'}) {
		$rc =~ s/\.//g;
		$rc = substr("\U$rc\E",0,10); $rc =~ s/\s+$//;
		if (my $st = $data->{'State'}) {
			if ($st eq 'CN' and $data->{NPA}) {
				if (exists $nanpst{$data->{NPA}}) {
					$st = $nanpst{$data->{NPA}};
				}
				$data->{LATA} = 'Canada (888)';
			}
			$st = 'NF' if $st eq 'NL';
			$st = 'PQ' if $st eq 'QC';
			$st = 'VU' if $st eq 'NU';
			if ($data->{LATA} eq 'Manitoba (888)') {
				$data->{LATA} = 'Canada (888)';
			}
			$rcs{$st}{$rc} = {} unless exists $rcs{$st}{$rc};
			my $rec = $rcs{$st}{$rc};
			$data->{RCSHORT} = $rc;
			$data->{REGION} = $st;
			for (my $i=0;$i<@rc_keys;$i++) {
				my $k = $rc_keys[$i];
				if ($k eq 'NPA') {
					if ($data->{$k}) {
						$rec->{$k}{$data->{$k}}++;
					}
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
}

my %mapping = (
	'NPA NXX'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		return if $val eq 'N/A';
		my ($npa,$nxx,$blk) = split(/-/,$val);
		if ($npa) {
			$fcount{NPA}{count}++;
			$dat->{NPA} = $npa;
		}
		if ($nxx) {
			$fcount{NXX}{count}++;
			$dat->{NXX} = $nxx;
		}
		if (defined $blk) {
			$fcount{X}{count}++;
			$dat->{X} = $blk;
		}
	},
	'State'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Company'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'OCN'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Rate Center'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Switch'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Date'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Prefix Type'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Switch Name'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Switch Type'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'LATA'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Tandem'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},

);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";

my @files = `find $datadir -name '*.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn...\n";
	open($fh,"xzcat $fn|") or die "can't process $fn";
	while (<$fh>) { chomp;
		next unless /<tr class="results"/;
		/<tr class="results">(.*?)<\/tr>/;
		$_ = $1;
		my $dat = {};
		my $fno = 0;
		while (/<td.*?>(.*?)<\/td>/g) {
			my $fld = $1; $fno++;
			$fld =~ s/<a href=.*?>//g;
			$fld =~ s/<\/a>//g;
			if ($fld =~ /<br>/) {
				my @fields = split(/<br>/,$fld);
				foreach my $datum (@fields) {
					next unless $datum =~ /: +/;
					my ($key,$val) = split(/: +/,$datum);
					&{$mapping{$key}}($dat,$val,$key) if exists $mapping{$key};
				}
			} else {
				my $key = $names->[$fno];
				&{$mapping{$key}}($dat,$fld,$key) if exists $mapping{$key};
			}
		}
		closerecord($dat);
	}
	close($fh);
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

$fn = "$datadir/clli.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@clli_keys), '"', "\n";
foreach my $k (sort keys %cllis) {
	my $clli = $cllis{$k};
	my @values = ();
	foreach (@clli_keys) { push @values, $clli->{$_} }
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach my $s (sort keys %rcs) {
	foreach my $k (sort keys %{$rcs{$s}}) {
		my $rc = $rcs{$s}{$k};
		my @values = ();
		foreach (@rc_keys) {
			if (exists $rc->{$_} and ref $rc->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rc->{$_}});
			} else {
				push @values, $rc->{$_};
			}
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);


