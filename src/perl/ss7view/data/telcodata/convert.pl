#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $rsltdir = "$datadir/results";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(STDERR,':utf8');

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

my %mapping = (
	'NPA NXX'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		return if $val eq 'N/A';
		my ($npa,$nxx,$blk) = split(/-/,$val);
		if ($npa) {
			$dat->{NPA} = $npa;
		}
		if ($nxx) {
			$dat->{NXX} = $nxx;
		}
		if (defined $blk) {
			$dat->{X} = $blk;
		}
	},
	'State'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Company'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'OCN'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Rate Center'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Switch'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Date'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Prefix Type'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Switch Name'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'Switch Type'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},
	'LATA'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$dat->{$fld} = $val;
	},
	'Tandem'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{$fld} = $val;
	},

);

$fn = "$datadir/db.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";

print STDERR "I: finding results files...\n";
my @files = `find $rsltdir -name '*.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn...\n";
	open($fh,"xzcat $fn|") or die "can't process $fn";
	while (<$fh>) { chomp;
		next unless /<tr class="results"/;
		/<tr class="results">(.*?)<\/tr>/;
		$_ = $1;
		my $data = {};
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
					&{$mapping{$key}}($data,$val,$key) if exists $mapping{$key};
				}
			} else {
				my $key = $names->[$fno];
				&{$mapping{$key}}($data,$fld,$key) if exists $mapping{$key};
			}
		}
		if (exists $data->{NPA} and exists $data->{NXX}) {
			my @values = ();
			foreach (@keys) { push @values, $data->{$_}; }
			print $of '"', join('","', @values), '"', "\n";
		}
	}
	close($fh);
}
close($of);

exit;

1;

__END__
