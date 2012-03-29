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

my @keys = (
	'NPA',
	'NXX',
	'X',
	'rng',
	'loc',
	'state',
	'clli',
	'wcvh',
	'ocn',
	'lata',
	'feat',
);

my @ocn_keys = (
	'OCN',
	'company',
	'overall',
	'neca',
	'trgt',
);

my @clli_keys = (
	'CLLI',
	'wcvh',
	'ocn',
	'lata',
	'feat',
);

my %ocns = ();
my %cllis = ();

sub closerecord {
	my $data = shift;
	if (exists $data->{feat} and ref $data->{feat} eq 'HASH') {
		$data->{feat} = join(' ',sort keys %{$data->{feat}});
	}
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
	if (my $ocn = $data->{ocn}) {
		$ocns{$ocn} = {} unless exists $ocns{$ocn};
		my $rec = $ocns{$ocn};
		$rec->{OCN} = $ocn;
		for (my $i=1;$i<@ocn_keys;$i++) {
			my $k = $ocn_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					printf STDERR "E: OCN %-4.4s %-8.8s changing from %-24.24s to %-24.24s\n",
						$ocn, $k, $rec->{$k}, $data->{$k};
					#print STDERR "E: $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	if (my $clli = $data->{clli}) {
		$cllis{$clli} = {} unless exists $cllis{$clli};
		my $rec = $cllis{$clli};
		$rec->{CLLI} = $clli;
		for (my $i=1;$i<@clli_keys;$i++) {
			my $k = $clli_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					printf STDERR "E: CLLI %-11.11s %-8.8s changing from %-24.24s to %-24.24s\n",
						$clli, $k, $rec->{$k}, $data->{$k};
					#print STDERR "E: $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
}

$fn = "$codedir/NECA-4-1.txt";
print STDERR "processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";

my $start = 0;

while (<$fh>) { chomp;
	my $data = {};
	unless ($start) {
		#print "IGNORED: $_\n";
		next unless /^\s+COMPANY CODES FOR/;
		$start = 1;
		next;
	}
	last if /^\s+END OF SECTION/;
	#next unless /^\s/;
	next unless /^\s/ or /^[0-9][0-9][0-9][A-Z0-9]\s/;
	next if /^\s*-\s*THE\s*COMPANY HAS/;
	next if /^\s*-\s*THE\s*LEGAL NAME/;
	next if /^\s*-\s*THE\s*ACQUIRED CODE/;
	next if /PRINTED IN U.S.A./;
	next if /^====/;
	next if /^\f/;
	next if /^DIRECTOR - ACCESS TARIFFS/;
	next if /^80 S. JEFFERSON/;
	next if /^WHIPPANY, N.J./;
	next if /^ISSUED: /;
	next if /WIRE CENTER AND INTERCONNECTION INFORMATION/;
	next if /COMPANY CODES FOR/;
	next if /^\s*CODE\s*$/;
	next if /^\s*$/;
	#print "USING: $_\n";
	$_ =~ s/\([CDSN]\)//g; $_ =~ s/^\s+//; $_ =~ s/\s+$//;
	next if $_ eq '';
	if (/^\s*([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+(.*?)\s+\(OVERALL\)\s*$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{company} = $3;
		$data->{overall} = 1;
		closerecord($data);
	} elsif (/^\s*([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+([YN])\s+([A-Z]+)\s+(.*?)\s*$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{neca} = $3;
		$data->{category} = $4;
		$data->{company} = $5;
		closerecord($data);
	} else {
		warn "cannot parse line '$_'";
	}
}
close($fh);

my $state = undef;

sub mydoneit {
	my $data = shift;
	my $state = $data->{state} if exists $data->{state};
	if ($data->{blks}) {
		for (my $i=0;$i<@{$data->{blks}};$i++) {
			$data->{NPA}   = $data->{npas}[$i];
			$data->{NXX}   = $data->{nxxs}[$i];
			$data->{X}     = $data->{blks}[$i];
			$data->{rng}   = $data->{rngs}[$i];
			$data->{lines} = $data->{lins}[$i];
			#my $npa = $data->{npa};
			#my $st = $nanpst{$npa} if exists $nanpst{$npa};
			#my $st = $db->{npa}{$npa}{state} if exists $db->{npa}{$npa}{state};
#			if (defined $st) {
#				$data->{state} = $st;
#				if (exists $data->{loc}) {
#					$data->{stloc} = "$st: $data->{loc}";
#				} else {
#					delete $data->{stloc};
#				}
#			} elsif ($state) {
				$data->{state} = $state;
#				if (exists $data->{loc}) {
#					$data->{stloc} = "$state: $data->{loc}";
#				} else {
#					delete $data->{stloc};
#				}
#				print STDERR "W: assigning state $state to npa $npa\n";
#			} else {
#				delete $data->{state};
#				delete $data->{stloc};
#				#print STDERR "E: no state for npa $npa\n";
#			}
			closerecord($data);
		}
	} else {
		closerecord($data);
	}
}

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";

foreach my $fname (qw/NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt/) {

$fn = "$codedir/$fname";
print STDERR "processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";

my $inheader = 1;
my $infooter = 0;
my $lineno = 0;
my $data = {};

while (<$fh>) { chomp;
	#print STDERR "HEADER: $_\n" if $inheader;
	if ($inheader) {
		if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
			$state = $1;
		}
		next unless /^---/;
		$inheader = 0;
		next;
	}
	if (/^\.\.\.\.\./) {
		#print STDERR "CLOSING RECORD: $_\n";
		# close record
		#print STDERR "-------- /^...../\n";
		my $line = $_;
		mydoneit($data);
		$_ = $line;
		$data = {};
		$lineno = 0;
		next;
	}
	if (/^\s*\(x\) / or /\=====/ or /PRINTED IN U.S.A./ or /END OF SECTION/) {
		#print STDERR "FOOTER: $_\n";
		$inheader = 1;
		next;
	}
	next if /^\s*$/;
	if (/^[A-Z]/) {
		if ($lineno) {
			# close record
			#print STDERR "-------- /^[A-Z]/\n";
			$lineno = 0;
			mydoneit($data);
			$data = {};
		}
	} else {
		unless ($lineno) {
			if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
				$state = $1;
			}
			#print STDERR "SKIPPING: $_\n";
			next;
		}
	}
	$lineno++;
	#printf STDERR "DATA %2d: %s\n", $lineno, $_;
	my $ugly = 1;
	if ($lineno >= 1) {
		while (/\b([A-Z][A-Z0-9])\b/g) {
			$data->{feat}{$1} = 1;
			$ugly = undef;
		}
	}
	if ($lineno == 1) {
		if (/^([A-Z]+.*?)\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7})\s+([-0-9][0-9]{4})\s+([-0-9][0-9]{4})\s+([0-9]{3}[A-Z0-9])\s+([0-9]{3,})/) {
			$data->{state} = $state if $state;
			$data->{loc} = $1;
			$data->{stloc} = "$state: $1" if $state;
			$data->{clli} = $2;
			$data->{wcv} = $3;
			$data->{wch} = $4;
			$data->{wcvh} = "$3,$4";
			$data->{ocn} = $5;
			$data->{lata} = $6;
		} elsif (/^([A-Z]+.*?)\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7})\s+XXXXX\s+XXXXX\s+XXXX\s+XXX\s+/) {
			$data->{state} = $state if $state;
			$data->{loc} = $1;
			$data->{stloc} = "$state: $1" if $state;
			$data->{clli} = $2;
		} else {
			print STDERR "E: can't grok line '$_'\n";
		}
	} elsif ($lineno > 1) {
		if (/^\s*([2-9][0-9][0-9])\//) {
			my $npa = $1;
			s/^\s*[2-9][0-9]{2}\///;
			s/\s+[A-Z][A-Z0-9]\b//g;
			s/\s*\([A-Za-z]\)\b//g;
			s/\s+.*$//;
			my @nxxs = split(/,/,$_);
			foreach my $nxx (@nxxs) {
				if ($nxx =~ /^[1-9][0-9][0-9]$/) {
					push @{$data->{npas}}, $npa;
					push @{$data->{nxxs}}, $nxx;
					push @{$data->{blks}}, '';
					push @{$data->{rngs}}, '';
					push @{$data->{lins}}, 10000;
				} elsif ($nxx =~ /^([2-9][0-9]{2})\((([0-9])[0-9]{3})-(([0-9])[0-9]{3})\)$/) {
					$nxx = $1;
					my ($s,$e) = ($2,$4);
					my ($f,$l) = ($3,$5);
					for (my $i=$f;$i<=$l;$i++) {
						push @{$data->{npas}}, $npa;
						push @{$data->{nxxs}}, $nxx;
						push @{$data->{blks}}, $i;
						push @{$data->{rngs}}, sprintf("%04d-%04d",$s,$e);
						push @{$data->{lins}}, $e - $s + 1;
					}
				} else {
					print STDERR "E: BAD NXX: $nxx\n";
				}
			}
		} elsif (/COMPANY:\s+([0-9]{3}[0-9A-Z])\s+HAS A PRESENCE ON THIS SWITCH/) {
			$data->{ocn} = $1;
		} elsif (/\b___\/___\b/) {
			# skip these lines
		} elsif (/--- Continued/) {
			# skip these lines
		} elsif (/"/) {
			# skip these lines
		} else {
			printf STDERR "E: UGLY %2d: %s\n", $lineno,$_ if $ugly;
		}
	}
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

