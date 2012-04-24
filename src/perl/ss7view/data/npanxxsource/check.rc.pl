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

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

sub closest {
	my ($lat,$lon) = @_;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=0;$v<=10000;$v+=1000) {
		for (my $h=-5000;$h<=27000;$h+=1000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=100) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=100) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-200;$v<=$gv+200;$v+=10) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=10) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-20;$v<=$gv+20;$v+=1) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=1) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	return ($cv,$ch);
}

$fn = "$progdir/rc.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
my $oldst = '';
while (<$fh>) { chomp;
	next if /^#/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $rc = $tokens[0];
	my $st = $tokens[1];
	if ($st ne $oldst) {
		$oldst = $st;
		print STDERR "I: processing $st...\n";
	}
	my $rec = {};
	for (my $i=0;$i<@fields;$i++) {
		$rec->{$fields[$i]} = $tokens[$i] if $tokens[$i];
	}
	my ($v1,$h1) = split(/,/,$rec->{'Major V&H'});
	my ($y2,$x2) = split(/,/,$rec->{'Rate Center Latitude/Longitude'});
	if ($v1 and $h1 and $v1 != 0 and $h1 != 0) {
		my ($y1,$x1) = Geo::Coordinates::VandH->vh2ll($v1,$h1);
		if ($y2 and $x2 and $y2 != 0 and $x2 != 0) {
			my ($v2,$h2) = closest($y2,$x2);
			my $ad = sqrt(((($v2-$v1)**2)+(($h2-$h1)**2))/10);
			next if $ad == 0;
			if ($ad > 0.708) {
				printf STDERR "E: %-2.2s %-10.10s %02.11f,%03.12f (%05d,%05d) (%03.9f miles)\n",
				$st,$rc,$y2,$x2,$v1,$h1,$ad;
			}
		} else {
			printf STDERR "E: %-2.2s %-10.10s missing RCLL\n", $st, $rc;
		}
	} else {
		if ($y2 and $x2 and $y2 != 0 and $x2 != 0) {
			my ($v2,$h2) = closest($y2,$x2);
			printf STDERR "E: %-2.2s %-10.10s %02.11f,%03.12f (%05d,%05d) missing RCVH\n",
			$st,$rc,$y2,$x2,$v2,$h2;
		} else {
			printf STDERR "E: %-2.2s %-10.10s missing RCVH/RCVLL\n", $st, $rc;
		}
	}
}
close($fh);

exit;

1;

__END__

"RCSHORT"
"REGION"
"NPA"
"NXX"
"LATA"
"RCVH"
"Zone"
"Rate Center LATA"
"Rate Center Name"
"Rate Center State"
"Rate Center Country"
"Zone Type"
"Number Pooling"
"Point Identifier"
"Rate Step"
"Area Codes in Rate Center"
"Embedded Overlays"
"Major V&H"
"Minor V&H"
"Rate Center Latitude/Longitude"
"Rate Center Google maps"
"Time Zone"
"DST Recognized"
