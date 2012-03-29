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

require "$codedir/database.pm";

my @files = `find . -name '*-city.html' | sort`;

my $fh = \*INFILE;

foreach my $file (@files) { chomp $file;
	my $data = {};
	print STDERR "processing $file\n";
	open($fh,"<",$file) or die "can't open $file";
	$file = /([0-9]{3})-city.html/;
	my $npa = $1;
	my $header = 1;
	my $rowno = 0;
	my $row = {};
	my $searching = 1;
	while (<$fh>) { chomp;
		if ($searching) {
			$searching = undef if /<tr align="CENTER">/;
			next;
		}
		if (/<tr align="CENTER">/) {
			next;
		} elsif (/<td /) {
			$rowno++;
			next;
		} elsif (/<\/td>/) {
			next;
		} elsif (/<\/tr>/) {
			$header = 0;
			$rowno = 0;
			next;
		}
		next if $header;
		next unless $rowno;
		s/^\s+//; s/\s+$//;
		if ($rowno == 1) {
			$row->{npa} = $_;
		} elsif ($rowno == 2) {
			$row->{city} = $_;
		} elsif ($rowno == 3) {
			$row->{county} = $_;
		} elsif ($rowno == 4) {
			$row->{st} = $_;
			push @{$data->{npa}{$npa}{cities}}, $row;
			print "$row->{npa}\t$row->{city}\t$row->{county}\t$row->{st}\n";
		}
	}
	close($fh);
}
