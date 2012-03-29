#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/data";
my $npaxdir = "$datadir/npanxx";
my $telcdir = "$datadir/telcodata";
my $areadir = "$datadir/areacodes";

my $npaxdata = do "$npaxdir/dump.pl" or die;
my $telcdata = do "$telcdir/dump.pl" or die;
my $areadata = do "$areadir/dump.pl" or die;

my $f1 = \*OUTFILE1;
my $f2 = \*OUTFILE2;

sub comparem {
	my $srcsdata = shift;
	my $chekdata = shift;
	my $corrections = 0;
	my $additions   = 0;
	my $gooddata    = 0;
	foreach my $npa (sort keys %{$srcsdata}) {
		foreach my $nxx (sort keys %{$srcsdata->{$npa}}) {
			my $differs = 0;
			my $src = $srcsdata->{$npa}{$nxx} if exists $srcsdata->{$npa}{$nxx};
			my $tel = $chekdata->{$npa}{$nxx} if exists $chekdata->{$npa}{$nxx};
			if ($src and !$tel) {
				#print "no checked data for $npa-$nxx\n";
				$additions += 5;
			}
			if ($src and $tel) {
				if ($src->{state} ne $tel->{state}) {
					print "$npa-$nxx: state: '$tel->{state}' -> '$src->{state}'\n";
					if ($tel->{state}) {
						$corrections++;
					} else {
						$additions++;
						$tel->{state} = $src->{state};
					}
					$differs++;
				} else {
					$gooddata++;
				}
				if ($src->{lata} ne $tel->{lata}) {
					unless ($tel->{lata} eq '' and $src->{lata} eq '99999') {
						print "$npa-$nxx: lata: '$tel->{lata}' -> '$src->{lata}'\n";
						if ($tel->{lata}) {
							$corrections++;
						} else {
							$additions++;
							$tel->{lata} = $src->{lata};
						}
						$differs++;
					} else {
						$gooddata++;
					}
				} else {
					$gooddata++;
				}
				if ($src->{clli} ne $tel->{clli}) {
					$tel->{clli} = '' if $tel->{clli} eq 'NOCLLIKNOWN';
					print "$npa-$nxx: clli: '$tel->{clli}' -> '$src->{clli}'\n";
					if ($tel->{clli}) {
						$corrections++;
					} else {
						$additions++;
						$tel->{clli} = $src->{clli};
					}
					$differs++;
				} else {
					$gooddata++;
				}
				if ($src->{wire} ne $tel->{wire}) {
					$tel->{wire} = '' if $tel->{wire} eq 'NOCLLIKN';
					print "$npa-$nxx: wire: '$tel->{wire}' -> '$src->{wire}'\n";
					if ($tel->{wire}) {
						$corrections++;
					} else {
						$additions++;
						$tel->{wire} = $src->{wire};
					}
					$differs++;
				} else {
					$gooddata++;
				}
				if ($src->{city} ne $tel->{city}) {
					$tel->{city} = '' if $tel->{city} eq 'NOCLLI';
					print "$npa-$nxx: city: '$tel->{city}' -> '$src->{city}'\n";
					if ($tel->{city}) {
						$corrections++;
					} else {
						$additions++;
						$tel->{city} = $src->{city};
					}
					$differs++;
				} else {
					$gooddata++;
				}
				if ($src->{rate} ne "\U$tel->{rate}\E") {
					print "$npa-$nxx: rate: '\U$tel->{rate}\E' -> '$src->{rate}'\n";
					if ($tel->{rate}) {
						if (0) {
							$corrections++
						}
					} else {
						$additions++;
						$tel->{rate} = $src->{rate};
					}
					$differs++;
				} else {
					$gooddata++;
				}
				if ($src->{ocn} ne $tel->{ocn}) {
					print "$npa-$nxx: ocn: '$tel->{ocn}' -> '$src->{ocn}'\n";
					if ($tel->{ocn}) {
						$corrections++;
					} else {
						$additions++;
						$tel->{ocn} = $src->{ocn};
					}
					$differs++;
				} else {
					$gooddata++;
				}
			}
			if (1) {
				if ($src and ($src->{state} or $src->{lata} or $src->{clli} or $src->{wire} or $src->{city} or $src->{rate} or $src->{ocn})) {
				printf $f1 "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
					$npa,
					$nxx,
					$src->{state},
					$src->{lata},
					$src->{rate},
					$src->{clli},
					$src->{wire},
					$src->{city},
					$src->{ocn};
				}
				if ($tel and ($tel->{state} or $tel->{lata} or $tel->{clli} or $tel->{wire} or $tel->{city} or $tel->{rate} or $tel->{ocn})) {
				printf $f2 "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
					$npa,
					$nxx,
					$tel->{state},
					$tel->{lata},
					$tel->{rate},
					$tel->{clli},
					$tel->{wire},
					$tel->{city},
					$tel->{ocn};
				}
			}
		}
	}
	my $corr = int($corrections * 100 / ($corrections + $gooddata));
	my $addi = int($additions   * 100 / ($additions   + $gooddata));
	print "There are $corrections corrections, or $corr percent\n";
	print "There are $additions additions, or $addi percent\n";
	print "There are $gooddata good data points\n";
}

open($f1,">","$datadir/outf5.txt");
open($f2,">","$datadir/outf6.txt");
print "Comparing npanxxsource.com and areacodes.com\n";
print $f1 "Comparing npanxxsource.com\n";
print $f2 "Comparing areacodes.com\n";
comparem($npaxdata,$areadata);
close($f1);
close($f2);

open($f1,">","$datadir/outf1.txt");
open($f2,">","$datadir/outf2.txt");
print "Comparing npanxxsource.com and telcodata.us\n";
print $f1 "Comparing npanxxsource.com\n";
print $f2 "Comparing telcodata.us\n";
comparem($npaxdata,$telcdata);
close($f1);
close($f2);

open($f1,">","$datadir/outf3.txt");
open($f2,">","$datadir/outf4.txt");
print "Comparing areacodes.com and telcodata.us\n";
print $f1 "Comparing areacodes.com\n";
print $f2 "Comparing telcodata.us\n";
comparem($areadata,$telcdata);
close($f1);
close($f2);

