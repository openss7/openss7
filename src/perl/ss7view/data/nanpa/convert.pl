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
	'OCN',
	'Company',
	'Status',
	'RateCenter',
	'Remarks',
	'State',
	'EffectiveDate',
	'Use',
	'AssignDate',
	'Initial/Growth',
);

my @ocn_keys = (
	'OCN',
	'Company',
);

my %ocns = ();

my @rc_keys = (
	'RCSHORT',
	'REGION',
	'RCVH',
	'NPA',
);

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
	if (my $rc = $data->{RateCenter}) {
		$rc = substr("\U$rc\E",0,10); $rc =~ s/\s+$//;
		if (my $st = $data->{State}) {
			$rcs{$st}{$rc} = {} unless exists $rcs{$st}{$rc};
			my $rec = $rcs{$st}{$rc};
			$rec->{RCSHORT} = $rc;
			$rec->{REGION} = $st;
			$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		}
	}
}

my %mapping = (
	'NPA'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'NXX'=>sub{
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
	'Company'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Status'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'RateCenter'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Remarks'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","', @keys), '"', "\n";
{
$fn = "$datadir/NPANXX.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn cygdrive/d/cna/COCode-ESRDData/NPANXX.csv |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp; s/\r//g;
	s/^"//;
	s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		#print STDERR "I: fields are '", join('|',@fields), "'\n";
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
}
{
$fn = "$datadir/allutlzd.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn allutlzd.txt | expand |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($header) {
		$header = undef;
		next;
	}
	my @tokens = ();
	$tokens[0] = substr($_,0,8); # State
	$tokens[1] = substr($_,8,8); # NPA-NXX
	$tokens[2] = substr($_,16,8); # Ocn
	$tokens[3] = substr($_,24,64); # Company
	$tokens[4] = substr($_,88,16); # RateCenter
	$tokens[5] = substr($_,104,16); # EffectiveDate
	$tokens[6] = substr($_,120,8); # Use
	$tokens[7] = substr($_,128,16); # AssignDate
	$tokens[8] = substr($_,144,16); # Intial/Growth
	for (my $i=0;$i<@tokens;$i++) {
		$tokens[$i] =~ s/^\s*"?//;
		$tokens[$i] =~ s/"?\s*$//;
	}
	my $data = {};
	$data->{State} = $tokens[0];
	($data->{NPA},$data->{NXX}) = split(/-/,$tokens[1]);
	$data->{OCN} = $tokens[2];
	$data->{Company} = $tokens[3];
	$data->{RateCenter} = $tokens[4];
	$data->{EffectiveDate} = $tokens[5];
	$data->{Use} = $tokens[6];
	$data->{AssignDate} = $tokens[7];
	$data->{'Initial/Growth'} = $tokens[8];
	closerecord($data);
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

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach my $s (sort keys %rcs) {
	foreach my $k (sort keys %{$rcs{$s}}) {
		my $rc = $rcs{$s}{$k};
		my @values = ();
		foreach (@rc_keys) {
			if (ref $rc->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rc->{$_}});
			} else {
				push @values, $rc->{$_};
			}
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);

