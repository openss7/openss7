#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $rsltdir = "$progdir/results";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use File::stat;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'NPA',
	'NXX',
	'X',
	'XXXX',
	'YYYY',
	'Type',
	'Carrier',
	'Switch',
	'Rate Center',
	'State',
);


$fn = "$datadir/db.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't open $fn";
print $of '"',join('","',@keys),'"',"\n";

print STDERR "I: reading results...\n";
print STDERR "I: finding results files...\n";
my @files = `find $rsltdir -name '*.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn...\n";
	open($fh,"xzgrep '^<tr>' $fn | grep -v '<th' | grep '<a href' | cut -f2- -d: |") or die "can't process files";
	while (<$fh>) { chomp;
		my $data = {};
		$data->{FDATE} = stat($fn)->mtime;
		next unless /^<tr><td><a href=[0-9]{3}-[0-9]{3}>([0-9]{3})-([0-9]{3})-0000<\/a><\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td>/;
		print $of '"',join('","',$1,$2,'','','',$3,$4,$5,$6,$7),'"',"\n";
	}
	close($fh);
}

close($of);

exit;

1;

__END__
