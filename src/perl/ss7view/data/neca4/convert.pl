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

my @keys = (
	'NPA',
	'NXX',
	'X',
	'rng',
	'lines',
	'total',
	'loc',
	'state',
	'clli',
	'wcvh',
	'ocn',
	'lata',
	'feat',
	'sect',
);

my @ocn_keys = (
	'OCN',
	'company',
	'overall',
	'neca',
	'trgt',
);

my @sw_keys = (
	'CLLI',
	'wcvh',
	'ocn',
	'lata',
	'feat',
	'NPA',
	'NXX',
);

my @wc_keys = (
	'WC',
	'wcvh',
	'clli',
	'ocn',
	'lata',
	'feat',
	'state',
	'NPA',
	'NXX',
);

my @rc_keys = (
	'RCSHORT',
	'REGION',
	'RCVH',
	'RCLL',
	'NPA',
	'NXX',
	'RCNAME',
);

my %ocns = ();
my %sws = ();
my %wcs = ();
my %dbsw = ();
my %dbwc = ();

my @swcor_keys = ();
my %swcor = ();
$fn = "$datadir/swcor.csv";
if (-f $fn) {
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		next if /^#/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@swcor_keys = @tokens;
			$heading = undef;
			next;
		}
		if (my $sw = $tokens[0]) {
			$swcor{$sw} = {} unless exists $swcor{$sw};
			my $rec = $swcor{$sw};
			for (my $i=0;$i<@swcor_keys;$i++) {
				$swcor{$sw}{$swcor_keys[$i]} = $tokens[$i] if $tokens[$i];
			}
		}
	}
	close($fh);
}


my @wccor_keys = ();
my %wccor = ();
$fn = "$datadir/wccor.csv";
if (-f $fn) {
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		next if /^#/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@wccor_keys = @tokens;
			$heading = undef;
			next;
		}
		if (my $wc = $tokens[0]) {
			$wccor{$wc} = {} unless exists $wccor{$wc};
			my $rec = $wccor{$wc};
			for (my $i=0;$i<@wccor_keys;$i++) {
				$wccor{$wc}{$wccor_keys[$i]} = $tokens[$i] if $tokens[$i];
			}
		}
	}
	close($fh);
}

sub closerecord {
	my $data = shift;
	if (exists $data->{feat} and ref $data->{feat} eq 'HASH') {
		$data->{feat} = join(' ',sort keys %{$data->{feat}});
	}
	$data->{clli} = 'SXCTIAXO1MD' if $data->{clli} eq 'SXCTIZXO1MD';
	$data->{clli} = 'STCDMNTOXUX' if $data->{clli} eq 'STCDMITOXUX';
	$data->{clli} = 'WCHSSCXARS0' if $data->{clli} eq 'SALDSCXARS0';
	$data->{clli} = ''            if $data->{clli} eq 'TBAYARXARS0';
	$data->{clli} = ''            if $data->{clli} eq 'BREWARXARS0';
	$data->{clli} = ''            if $data->{clli} eq 'WDRWARXARS0';
	if (my $sw = $data->{clli}) {
		if (exists $swcor{$sw}) {
			foreach my $k (@swcor_keys) {
				next if $k eq 'CLLI';
				if ($swcor{$sw}{$k} and $swcor{$sw}{$k} ne $data->{$k}) {
					printf STDERR "W: SW  %-7.7s %-11.11s %-8.8s corrected from %-24.24s to %-24.24s\n",
						$data->{sect}, $sw, $k, $data->{$k}, $swcor{$sw}{$k};
					$data->{$k} = $swcor{$sw}{$k};
				}
			}
		}
		if (my $wc = substr($sw,0,8)) {
			if (exists $wccor{$wc}) {
				foreach my $k (@wccor_keys) {
					next if $k eq 'WC';
					if ($wccor{$wc}{$k} and $wccor{$wc}{$k} ne $data->{$k}) {
						printf STDERR "W: WC  %-7.7s %-11.11s %-8.8s corrected from %-24.24s to %-24.24s\n",
							$data->{sect}, $wc, $k, $data->{$k}, $wccor{$wc}{$k};
						$data->{$k} = $wccor{$wc}{$k};
					}
				}
			}
		}
	}
#	printf STDERR "I: %-32.32s %-11.11s %-5.5s %-5.5s %-4.4s %-5.5s %s\n",
#		$data->{loc}, $data->{clli}, $data->{wcv}, $data->{wch},
#		$data->{ocn}, $data->{lata}, $data->{feat};
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
	if (my $ocn = $data->{ocn}) {
		$ocns{$ocn} = {} unless exists $ocns{$ocn};
		my $rec = $ocns{$ocn};
		$data->{OCN} = $ocn;
		foreach my $k (@ocn_keys) {
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					printf STDERR "E: OCN %-11.11s %-8.8s changing from %-24.24s to %-24.24s\n",
						$ocn, $k, $rec->{$k}, $data->{$k};
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	if (my $sw = $data->{clli}) {
		$sws{$sw} = {} unless exists $sws{$sw};
		my $rec = $sws{$sw};
		$data->{CLLI} = $sw;
		foreach my $k (@sw_keys) {
			if ($data->{$k}) {
				if ($k eq 'CLLI') {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						printf STDERR "E: SW  %-11.11s %-8.8s changing from %-24.24s to %-24.24s\n",
							$sw, $k, $rec->{$k}, $data->{$k};
					}
					$rec->{$k} = $data->{$k};
				} elsif ($k eq 'feat') {
					foreach my $f (split(/\s+/,$data->{$k})) {
						$rec->{$k}{$f}++;
					}
				} elsif ($k eq 'NXX') {
					$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
				} else {
					$rec->{$k}{$data->{$k}}++ if $data->{$k};
				}
			}
		}
		if ($data->{NPA} and $data->{NXX}) {
			$dbsw{$data->{NPA}}{$data->{NXX}} = {} unless exists
			$dbsw{$data->{NPA}}{$data->{NXX}};
			my $rec = $dbsw{$data->{NPA}}{$data->{NXX}};
			$rec->{NPA} = $data->{NPA};
			$rec->{NXX} = $data->{NXX};
			$rec->{CLLI} = $sw;
		}
		if (my $wc = substr($sw,0,8)) {
			$wcs{$wc} = {} unless exists $wcs{$wc};
			my $rec = $wcs{$wc};
			$data->{WC} = $wc;
			foreach my $k (@wc_keys) {
				if ($data->{$k}) {
					if ($k eq 'WC') {
						if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
							printf STDERR "E: WC  %-11.11s %-8.8s changing from %-24.24s to %-24.24s\n",
								$wc, $k, $rec->{$k}, $data->{$k};
						}
						$rec->{$k} = $data->{$k};
					} elsif ($k eq 'feat') {
						foreach my $f (split(/\s+/,$data->{$k})) {
							$rec->{$k}{$f}++;
						}
					} elsif ($k eq 'NXX') {
						$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
					} else {
						$rec->{$k}{$data->{$k}}++ if $data->{$k};
					}
				}
			}
			if ($data->{NPA} and $data->{NXX}) {
				$dbwc{$data->{NPA}}{$data->{NXX}} = {} unless exists
				$dbwc{$data->{NPA}}{$data->{NXX}};
				my $rec = $dbwc{$data->{NPA}}{$data->{NXX}};
				$rec->{NPA} = $data->{NPA};
				$rec->{NXX} = $data->{NXX};
				$rec->{WC} = $wc;
			}
		}
	}
}

$fn = "$datadir/NECA-4-1.txt";
print STDERR "processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";

my $start = 0;

my $frag = '';

while (<$fh>) { chomp;
	my $data = {};
	unless ($start) {
		#print "IGNORED: $_\n";
		next unless /^\s+COMPANY CODES FOR/;
		$start = 1;
		next;
	}
	if ($frag) {
		$_ = $frag.' '.$_;
		$frag = '';
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
	s/\([CDSN]\)//g; s/^\s+//; s/\s+$//;
	next if $_ eq '';
	if (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+(.*?)\s+\(OVERALL\)$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{company} = $3;
		$data->{overall} = 1;
		closerecord($data);
	} elsif (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+([YN])\s+(CAP|[A-Z]+ )\s*(.*?)$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{neca} = $3;
		$data->{category} = $4;
		$data->{company} = $5;
		closerecord($data);
	} elsif (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])(\s+([YN])\s+([A-Z]+))?$/) {
		$frag = $_;
	} else {
		warn "cannot parse line '$_'";
	}
}
close($fh);

my $state = undef;
my $section = undef;
my $page = undef;

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

$fn = "$datadir/$fname";
print STDERR "processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";

my $flineno = 0;
my $inheader = 1;
my $infooter = 0;
my $lineno = 0;
my $data = {};
my $changing = 0;
my $willchange = 0;
my $couldchange = 0;
my $changenxx = 0;
my $pending = 0;

while (<$fh>) { chomp; $flineno++;
	#print STDERR "HEADER: $fname($flineno) $_\n" if $inheader;
	if ($inheader) {
		if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
			$state = $1;
		}
		if (/^\s+SECTION\s+([0-9]+)/) {
			$section = $1;
			$page = 1;
		}
		if (/\sPAGE\s+([0-9]+)/) {
			$page = $1;
		}
		if (/PENDING CHANGES/) {
			$pending = 1;
		}
		next unless /^\s?-----/;
		$inheader = 0;
		next;
	}
	if (/^\s?\.\.\.\.\./) {
		#print STDERR "CLOSING RECORD: $fname($flineno) $_\n";
		# close record
		#print STDERR "-------- /^...../\n";
		my $line = $_;
		mydoneit($data);
		$_ = $line;
		$data = {};
		$lineno = 0;
		$changing = 0;
		$couldchange = 0;
		$willchange = 0;
		$changenxx = 0;
		next;
	}
	if (/^\s*\(x\) / or /\=====/ or /PRINTED IN U.S.A./ or /END OF SECTION/) {
		#print STDERR "FOOTER: $fname($flineno) $_\n";
		$inheader = 1;
		next;
	}
	if (/END OF REPORT/) {
		if ($pending) {
			$pending = 0;
			next;
		}
	}
	if ($pending) {
		#print STDERR "PENDING: $fname($flineno): $_\n";
		next;
	}
	next if /^\s*$/;
	next if /--- Continued/;
	if (/^\s?[A-Z]/) {
		if ($lineno) {
			# close record
			#print STDERR "-------- /^[A-Z]/\n";
			$lineno = 0;
			$changing = 0;
			$couldchange = 0;
			$willchange = 0;
			$changenxx = 0;
			mydoneit($data);
			$data = {};
		}
	} else {
		unless ($lineno or /\(D\)/) {
			if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
				$state = $1;
			}
			print STDERR "SKIPPING: $fname($flineno) $_\n";
			next;
		}
	}
	if (/\(D\)/) {
		#print STDERR "DELETED: $fname($flineno) $_\n";
		next;
	}
	$lineno++;
	#printf STDERR "DATA %2d: $fname($flineno) %s\n", $lineno, $_;
	my $ugly = 1;
	if (/\(S\)/) {
		$willchange = 0;
		$couldchange = 1;
		$changing = 0;
		$changenxx = 0;
	} elsif (/\(C\)/) {
		$willchange = 1;
		$couldchange = 0;
		$changing = 0;
		$changenxx = 0;
	} elsif ($willchange) {
		$willchange = 0;
		$couldchange = 0;
		$changing = 1;
		$changenxx = 0;
		$lineno = 1;
		if (/\b[A-Z][A-Z0-9]\b/) {
			# feature codes are changing too, clear old codes
			$data->{feat} = {};
		}
	} elsif ($couldchange) {
		if (/"/) {
			$willchange = 0;
			$couldchange = 0;
			$changing = 1;
			$changenxx = 0;
			$lineno = 1;
			if (/\b[A-Z][A-Z0-9]\b/) {
				# feature codes are changing too, clear old codes
				$data->{feat} = {};
			}
		} else {
			$willchange = 0;
			$couldchange = 0;
			$changing = 0;
			$changenxx = 0;
		}
	}
	if ($lineno >= 1) {
		while (/\b([A-Z][A-Z0-9])\b/g) {
			$data->{feat}{$1} = 1;
			$ugly = undef;
		}
	}
	if ($lineno == 1) {
		if ($changing) {
			if (/^\s?([A-Z]+.*?|\s*")\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7}|")\s+([-0-9][0-9]{4}|")\s+([-0-9][0-9]{4}|")\s+([0-9]{3}[A-Z0-9]|")\s+([0-9]{3,}|")/) {
				$data->{state} = $state if $state;
				$data->{loc} = $1 unless $1 =~ /"/;
				$data->{stloc} = "$state: $1" if $state and $1 !~ /"/;
				$data->{clli} = $2 unless $2 eq '"';
				$data->{wcv} = $3 unless $3 eq '"';
				$data->{wch} = $4 unless $4 eq '"';
				$data->{wcvh} = "$data->{wcv},$data->{wch}" if $data->{wcv} and $data->{wch};
				$data->{ocn} = $5 unless $5 eq '"';
				$data->{lata} = $6 unless $6 eq '"';
			} elsif (/--- Continued/) {
				# skip these lines
			} else {
				print STDERR "E: $fname($flineno) can't grok line '$_'\n";
			}
		} else {
			if (/^\s?([A-Z]+.*?)\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7})\s+([-0-9][0-9]{4})\s+([-0-9][0-9]{4})\s+([0-9]{3}[A-Z0-9])\s+([0-9]{3,})/) {
				$data->{state} = $state if $state;
				$data->{loc} = $1;
				$data->{stloc} = "$state: $1" if $state;
				$data->{clli} = $2;
				$data->{wcv} = $3;
				$data->{wch} = $4;
				$data->{wcvh} = "$3,$4";
				$data->{ocn} = $5;
				$data->{lata} = $6;
				$data->{sect} = "$section-$page" if $section and $page;
# these are only formatted this way for deleted (D) records
#			} elsif (/^\s?([A-Z]+.*?)\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7})\s+XXXXX\s+XXXXX\s+XXXX\s+XXX\s+/) {
#				$data->{state} = $state if $state;
#				$data->{loc} = $1;
#				$data->{stloc} = "$state: $1" if $state;
#				$data->{clli} = $2;
			} elsif (/--- Continued/) {
				# skip these lines
			} else {
				print STDERR "E: $fname($flineno) can't grok line '$_'\n";
			}
		}
	} elsif ($lineno > 1) {
		if (/^\s*([2-9][0-9][0-9])\//) {
			my $npa = $1;
			if ($changing) {
				unless ($changenxx) {
					# changing NXXs, must clear old data
					$data->{npas} = [];
					$data->{nxxs} = [];
					$data->{blks} = [];
					$data->{rngs} = [];
					$data->{lins} = [];
					$data->{total} = 0;
					$changenxx = 1;
				}
			}
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
					push @{$data->{rngs}}, '0000-9999';
					push @{$data->{lins}}, 10000;
					$data->{total} += 10000;
				} elsif ($nxx =~ /^([2-9][0-9]{2})\((([0-9])[0-9]{3})-(([0-9])[0-9]{3})\)$/) {
					$nxx = $1;
					my ($s,$e) = ($2,$4);
					my ($f,$l) = ($3,$5);
					for (my $i=$f;$i<=$l;$i++) {
						push @{$data->{npas}}, $npa;
						push @{$data->{nxxs}}, $nxx;
						push @{$data->{blks}}, $i;
						my $bs = $i * 1000;
						my $be = $bs + 999;
						my ($ns,$ne) = ($s,$e);
						$ns = $bs if $s < $bs;
						$ne = $be if $e > $be;
						push @{$data->{rngs}}, sprintf("%04d-%04d",$ns,$ne);
						push @{$data->{lins}}, $ne - $ns + 1;
						$data->{total} += $ne - $ns + 1;
					}
				} else {
					print STDERR "E: $fname($flineno) BAD NXX: $nxx\n";
				}
			}
		} elsif (/COMPANY:\s+([0-9]{3}[0-9A-Z])\s+HAS A PRESENCE ON THIS SWITCH/) {
			$data->{ocn} = $1;
		} elsif (/\b___\/___\b/) {
			# skip these lines
			if ($changing) {
				unless ($changenxx) {
					# changing NXXs, must clear old data
					$data->{npas} = [];
					$data->{nxxs} = [];
					$data->{blks} = [];
					$data->{rngs} = [];
					$data->{lins} = [];
					$data->{total} = 0;
					$changenxx = 1;
				}
			}
		} elsif (/"\s+\/\s+"/) {
			# skip these lines
			if ($changing) {
				$changenxx = 1;
			} else {
				if ($ugly) {
					printf STDERR "E: $fname($flineno) UGLY(A) %2d: %s\n", $lineno,$_;
					printf STDERR "E: $fname($flineno) UGLY(A) %2d: %s\n", $lineno,"changing: $changing, willchange: $willchange, couldchange: $couldchange, changenxx: $changenxx";
				}
			}
		} elsif (/--- Continued/) {
			# skip these lines
# these lines should be handled by changes now...
#		} elsif (/"/) {
#			# skip these lines
		} else {
			if ($ugly) {
				printf STDERR "E: $fname($flineno) UGLY(B) %2d: %s\n", $lineno,$_;
				printf STDERR "E: $fname($flineno) UGLY(B) %2d: %s\n", $lineno,"changing: $changing, willchange: $willchange, couldchange: $couldchange, changenxx: $changenxx";
			}
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
	foreach (@ocn_keys) {
		if (exists $ocn->{$_} and ref $ocn->{$_} eq 'HASH') {
			push @values,join(',',sort keys %{$ocn->{$_}});
		} else {
			push @values, $ocn->{$_};
		}
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

my %bads;

%bads = ();

$fn = "$datadir/sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@sw_keys), '"', "\n";
foreach my $k (sort keys %sws) {
	my $rec = $sws{$k};
	my @values = ();
	foreach (@sw_keys) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/db.sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/NPA NXX CLLI/),'"',"\n";
foreach my $npa (sort keys %dbsw) {
	foreach my $nxx (sort keys %{$dbsw{$npa}}) {
		my $rec = $dbsw{$npa}{$nxx};
		print $of '"',join('","',($npa,$nxx,$rec->{CLLI})),'"',"\n";
	}
}
close($of);

$fn = "$datadir/swfix.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/CLLI wcvh/),'"',"\n";
foreach my $k (sort keys %bads) {
	my $rec = $sws{$k};
	my @values = ();
	foreach (qw/CLLI wcvh/) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

%bads = ();

$fn = "$datadir/wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@wc_keys), '"', "\n";
foreach my $k (sort keys %wcs) {
	my $rec = $wcs{$k};
	my @values = ();
	foreach (@wc_keys) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/db.wc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/NPA NXX WC/),'"',"\n";
foreach my $npa (sort keys %dbwc) {
	foreach my $nxx (sort keys %{$dbwc{$npa}}) {
		my $rec = $dbwc{$npa}{$nxx};
		print $of '"',join('","',($npa,$nxx,$rec->{WC})),'"',"\n";
	}
}
close($of);

$fn = "$datadir/wcfix.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/WC wcvh/),'"',"\n";
foreach my $k (sort keys %bads) {
	my $rec = $wcs{$k};
	my @values = ();
	foreach (qw/WC wcvh/) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

