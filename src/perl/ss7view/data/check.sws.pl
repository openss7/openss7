#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = $progdir;

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(STDERR,':utf8');

my @dirs = (
	'areacodes',
	'localcallingguide',
	'neca4',
	'npanxxsource',
	'pinglo',
	'telcodata',
);

my @types = qw/sw wc pl/;

my %db = ();
my %sts = ();

my %fields = ();
my %fieldnos = ();
my %counts = ();

foreach my $type (@types) {
	foreach my $dir (@dirs) {
		$fn = "$codedir/$dir/$type.nogeo.csv";
		next unless -f $fn;
		print STDERR "I: reading $fn\n";
		open($fh,"<:utf8",$fn) or die "can't read $fn";
		my $heading = 1;
		my $oldrg = '';
		while (<$fh>) { chomp;
			s/^"//; s/"$//; my @tokens = split(/","/,$_);
			if ($heading) {
				$fields{$type}{$dir} = \@tokens;
				for (my $i=0;$i<@tokens;$i++) {
					$fieldnos{$dir}{$tokens[$i]} = $i;
				}
				$heading = undef;
				next;
			}
			my $key = "\U$type\ECLLI";
			my $fld = $tokens[$fieldnos{$dir}{$key}];
			next unless $fld;
			$counts{$dir}{$type}++;
			push @{$db{$type}{$dir}{$fld}}, \@tokens;
			$db{cnts}{$type}{$fld}{$dir}++;
			if (my $rg = substr($fld,4,2)) {
				if ($oldrg ne $rg) {
					$oldrg = $rg;
					print STDERR "I: reading REGION $rg for $dir\r";
				}
			}
		}
		print STDERR "\n";
		close($fh);
	}
}
foreach my $dir (@dirs) {
	foreach my $type (@types) {
		printf STDERR "I: there are %10d \U$type\E records for $dir\n", $counts{$dir}{$type} if exists $counts{$dir}{$type};
	}
}

my %patterns = ();
my %matching = ();

print STDERR "I: generating CLLI patterns...\n";
foreach my $clli (keys %{$db{cnts}{sw}}) {
	my $code = substr($clli,8,3);
	my $pat;
	if ($code =~ /[A-Z0-9]MD/) {
		$pat = 'xMD'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /[XYZ][A-Z0-9][XYZ]/) {
		$pat = 'zxz'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /CM[A-Z0-9]/) {
		$pat = 'CMx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /BB[A-Z0-9]/) {
		$pat = 'BBx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /[A-Z0-9]XD/) {
		$pat = 'xXD'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /CA[A-Z0-9]/) {
		$pat = 'CAx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /GT[A-Z0-9]/) {
		$pat = 'GTx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /HA[A-Z0-9]/) {
		$pat = 'HAx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /HG[A-Z0-9]/) {
		$pat = 'HGx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /RS[A-Z0-9]/) {
		$pat = 'RSx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /RL[A-Z0-9]/) {
		$pat = 'RLx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /WA[A-Z0-9]/) {
		$pat = 'WAx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /WC[A-Z0-9]/) {
		$pat = 'WCx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /DS[A-Z0-9]/) {
		$pat = 'DSx'; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	} elsif ($code =~ /[A-Z0-9]/) {
		if ($code =~ /[0-9]/) {
			$pat = $code; $pat =~ s/[0-9]/n/g; $patterns{$pat}++;
			for (my $i=0;$i<3;$i++) {
				$matching{$pat}{$i}{substr($code,$i,1)}++;
			}
			if ($code =~ /[A-Z]/) {
				$pat =~ s/[A-Z]/a/g; $patterns{$pat}++;
				for (my $i=0;$i<3;$i++) {
					$matching{$pat}{$i}{substr($code,$i,1)}++;
				}
			}
		} else {
			$pat = $code; $patterns{$pat}++;
			for (my $i=0;$i<3;$i++) {
				$matching{$pat}{$i}{substr($code,$i,1)}++;
			}
			$pat = $code; $pat =~ s/[A-Z]/a/g; $patterns{$pat}++;
			for (my $i=0;$i<3;$i++) {
				$matching{$pat}{$i}{substr($code,$i,1)}++;
			}
		}
	} else {
		$pat = $code; $patterns{$pat}++;
		for (my $i=0;$i<3;$i++) {
			$matching{$pat}{$i}{substr($code,$i,1)}++;
		}
	}
}
print STDERR "I: writing CLLI matching\n";
open($of,"| sort -n > matching.log") or die "can't generate matching.log";
foreach my $pat (sort keys %matching) {
	print $of "$patterns{$pat}\t$pat\t";
	for (my $i=0;$i<3;$i++) {
		print $of "[";
		foreach my $c (reverse sort {$matching{$pat}{$i}{$a} <=> $matching{$pat}{$i}{$b}} keys %{$matching{$pat}{$i}}) {
			print $of "$c";
		}
		print $of "]";
	}
	print $of "\n";
}
close($of);

foreach my $type (@types) {
	print STDERR "I: counting \U$type\E records...\n";
	foreach my $fld (keys %{$db{cnts}{$type}}) {
		my @ds = (keys %{$db{cnts}{$type}{$fld}});
		if (scalar @ds < 2) {
			$db{only}{$type}{$fld} = $ds[0];
		}
	}
}


foreach my $type (@types) {
	my %isolated = ();
	print STDERR "I: checking isolated \U$type\E records...\n";
	foreach my $fld (sort keys %{$db{only}{$type}}) {
		my $dir = $db{only}{$type}{$fld};
		print STDERR "E: $fld: only has \U$type\E records for $dir\n";
		$isolated{$dir}++;
		print STDERR "E: $fld: ",'"',join('","',@{$fields{$type}{$dir}}),'"',"\n";
		foreach my $rec (@{$db{$type}{$dir}{$fld}}) {
			print STDERR "E: $fld: ",'"',join('","',@{$rec}),'"',"\n";
		}
	}
	print STDERR "I: ---------------------\n";
	foreach my $dir (@dirs) {
		printf STDERR "I: there are %10d %7.2f%% isolated \U$type\E records for $dir\n", $isolated{$dir}, 100*$isolated{$dir}/$counts{$dir}{$type} if exists $isolated{$dir} and $counts{$dir}{$type};
	}
	print STDERR "I: ---------------------\n";
}

exit;

1;

__END__


