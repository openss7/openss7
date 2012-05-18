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

my %db = ();

my @dirs = (
	'neca4',
	'npanxxsource',
	'telcodata',
	'localcallingguide',
	'pinglo',
	'areacodes',
);

my %db_fields = ();

my %patterns = ();
my %matching = ();

sub addclli {
	my $clli = shift;
	return unless length($clli) == 11;
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

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/swx.csv";
	if (-f $fn) {
		print STDERR "I: reading $fn\n";
		open($fh,"<:utf8",$fn) or die "can't read $fn";
		my $heading = 1;
		my @fields = ();
		while (<$fh>) { chomp;
			s/^"//; s/"$//; my @tokens = split(/","/,$_);
			if ($heading) {
				@fields = @tokens;
				$heading = undef;
				next;
			}
			for (my $i=0;$i<@fields;$i++) {
				if ($fields[$i] =~ /^(SWCLLI|STP.*|ACTUAL|AGENT|HOST|TDM.*)$/) {
					addclli($tokens[$i]) if $tokens[$i];
				}
			}
		}
		close($fh);
	}
}

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.all.nogeo.csv";
	if (-f $fn) {
		print STDERR "I: reading $fn\n";
		open($fh,"<:utf8",$fn) or die "can't read $fn";
		my $heading = 1;
		my @fields = ();
		my $oldnpa = 100;
		while (<$fh>) { chomp;
			s/^"//; s/"$//; my @tokens = split(/","/,$_);
			if ($heading) {
				@fields = @tokens;
				$heading = undef;
				next;
			}
			my $npa = $tokens[0];
			for (my $i=0;$i<@fields;$i++) {
				if ($fields[$i] =~ /^(SWCLLI|STP.*|ACTUAL|AGENT|HOST|TDM.*)$/) {
					addclli($tokens[$i]) if $tokens[$i];
				}
			}
			if ($oldnpa != int($npa/100)*100) {
				$oldnpa = int($npa/100)*100;
				print STDERR "I: reading NPA $oldnpa for $dir\n";
			}
		}
		close($fh);
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

exit;

1;

__END__
