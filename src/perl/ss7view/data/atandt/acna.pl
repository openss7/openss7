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
use Geo::Coordinates::VandH;
use POSIX;
use File::stat;
use Time::gmtime;
use DBI;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

my %states = ();

$fn = "$progdir/../lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<:utf8",$fn) or die "can't read $fn";
while (<$fh>) { chomp;
	next unless /^[A-Z]{2}\t/;
	my @fields = split(/\t/,$_);
	$states{$fields[6]} = $fields[2]; 
}
close($fh);


my %db = ();

$fn = "$progdir/ACNA_BNA.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't read $fn";
my $leader = 1;
my $insection = undef;
my $inheader = undef;
my $infgb = undef;
my $infgd = undef;
my %data = ();
sub closerecord {
	return unless ($data{acna} and $data{state} and $data{clli});
	$data{state} =~ s/Louisianna/Louisiana/;
	$data{state} =~ s/Kentuckey/Kentucky/;
	$data{state} =~ s/N\./North/;
	$data{state} =~ s/S\./South/;
	$data{state} = $states{$data{state}} if exists $states{$data{state}};
	$db{$data{acna}}{$data{state}}{$data{clli}} = {} unless exists $db{$data{acna}}{$data{state}}{$data{clli}};
	my $rec = $db{$data{acna}}{$data{state}}{$data{clli}};
	$rec->{state} = $data{state};
	$rec->{clli} = $data{clli};
	$rec->{acna} = $data{acna};
	$rec->{entity} = $data{entity};
	$rec->{addrs} = $data{addrs};
	$data{fgbcics} =~ s/\s+//g;
	foreach (split(/\s*,\s*/,$data{fgbcics})) {
		next unless $_;
		s/^950//;
		#next unless /^950([0-9]{3,4})$/;
		$rec->{fgbcics}{$_}++;
	}
	$data{fgdcics} =~ s/\s+//g;
	foreach (split(/\s*,\s*/,$data{fgdcics})) {
		next unless $_;
		s/^101//;
		#next unless /^1010([0-9]{3,4})$/;
		$rec->{fgdcics}{$_}++;
	}
}
$data{page} = 1;
while (<$fh>) { chomp;
	if ($leader) {
		next unless /^[1-9][0-9]*\.[A-Z]/;
		$leader = undef;
	}
	next if /^\s*$/;
	if ($inheader) {
		$inheader = undef;
		next;
	} elsif (/^\s*([1-9][0-9]*)\s*$/) {
		$data{page} = $1 + 1;
		next;
	} elsif (/^ACNA\s*\/\s*BNA\s+List/) {
		$inheader = 1;
		next;
	} elsif (/^([1-9][0-9]*)\.([A-Z].*)/) {
		$data{section} = $1;
		$data{state} = $2;
		$insection = undef;
		next;
	} elsif (/^([1-9][0-9]*)\.([1-9][0-9]*)([A-Z][A-Z0-9 _]{10})$/) {
		$data{section} = $1;
		$data{subsect} = $2;
		$data{clli} = $3;
		$insection = undef;
		next;
	} elsif (/^([1-9][0-9]*)\.([1-9][0-9]*)\.([1-9][0-9]*)ACNA:\s+([A-Z]{3})\s+-\s+(.*)$/) {
		closerecord;
		$data{section} = $1;
		$data{subsect} = $2;
		$data{subsubs} = $3;
		$data{acna} = $4;
		$data{entity} = $5;
		$insection = 1; $data{addrs} = [];
		$infgb = undef; delete $data{fgbcics};
		$infgd = undef; delete $data{fgdcics};
		next;
	} elsif ($insection) {
		if (/^FGB CICS$/) {
			$infgb = 1;
			$infgd = undef;
			next;
		} elsif (/^FGD CICS$/) {
			$infgb = undef;
			$infgd = 1;
			next;
		} elsif ($infgb) {
			$data{fgbcics} = join(' ',$data{fgbcics},$_);
			next;
		} elsif ($infgd) {
			$data{fgdcics} = join(' ',$data{fgdcics},$_);
			next;
		} else {
			push @{$data{addrs}}, $_;
		}
	} else {
		print STDERR "E: cannot grok line '$_'\n";
	}
}
closerecord;
close($fh);

my @keys = qw/ACNA STATE CLLI ENTITY ADDR1 ADDR2 ADDR3 ADDR4 ADDR5 ADDR6 FGBCICS FGDCICS/;
$fn = "$progdir/ACNA_BNA.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@keys),'"',"\n";
foreach my $acna (sort keys %db) {
	foreach my $state (sort keys %{$db{$acna}}) {
		foreach my $clli (sort keys %{$db{$acna}{$state}}) {
			my $rec = $db{$acna}{$state}{$clli};
			my @values = ();
			push @values, $acna, $state, $clli;
			push @values, $rec->{entity};
			if ($rec->{addrs} and ref $rec->{addrs} eq 'ARRAY') {
				push @values, $rec->{addrs}[0];
				push @values, $rec->{addrs}[1];
				push @values, $rec->{addrs}[2];
				push @values, $rec->{addrs}[3];
				push @values, $rec->{addrs}[4];
				push @values, $rec->{addrs}[5];
			} else {
				push @values, '','','','','','';
			}
			if ($rec->{fgbcics} and ref $rec->{fgbcics} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{fgbcics}});
			} else {
				push @values, '';
			}
			if ($rec->{fgdcics} and ref $rec->{fgdcics} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{fgdcics}});
			} else {
				push @values, '';
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

exit;

1;

__END__
