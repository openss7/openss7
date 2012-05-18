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

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.all.nogeo.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $heading = 1;
	my $npa = 100;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			$db_fields{$dir} = \@tokens;
			$heading = undef;
			next;
		}
		my %data = ();
		for (my $i=0;$i<@{$db_fields{$dir}};$i++) {
			$data{$db_fields{$dir}[$i]} = $tokens[$i] if $tokens[$i];
		}
		my $clli = $data{SWCLLI};
		if (length($clli) == 11) {
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
		$data{WCCLLI} = substr($data{SWCLLI},0,8);
		$data{PLCLLI} = substr($data{SWCLLI},0,6);
		$data{STCLLI} = substr($data{SWCLLI},4,2);
		push @{$db{$dir}{$data{NPA}}{$data{NXX}}{$data{X}}}, \%data;
		$db{mark}{$data{NPA}}{$data{NXX}}{$data{X}}++;
		$db{cnts}{$data{NPA}}{$data{NXX}}{$dir}++;
		if ($npa != int($data{NPA}/100)*100) {
			$npa = int($data{NPA}/100)*100;
			print STDERR "I: reading NPA $npa for $dir\n";
		}
	}
	close($fh);
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

my %sws = ();

my %sw_fields = ();

foreach my $dir (@dirs) {
	$fn ="$codedir/$dir/sw.nogeo.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $heading = 1;
	my $rg = '';
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			$sw_fields{$dir} = \@tokens;
			$heading = undef;
			next;
		}
		my %data = ();
		for (my $i=0;$i<@{$sw_fields{$dir}};$i++) {
			$data{$sw_fields{$dir}[$i]} = $tokens[$i] if $tokens[$i];
		}
		$sws{$dir}{$data{SWCLLI}} = \%data;
		$sws{mark}{$data{SWCLLI}}++;
		if ($rg ne substr($data{SWCLLI},4,2)) {
			$rg = substr($data{SWCLLI},4,2);
			print STDERR "I: reading region $rg for $dir\n";
		}
	}
	close($fh);
}

sub compareothers {
	my ($d1,$d2,$r1,$r2,$f) = @_;
	my ($off,$len) = (0,11);
	if ($f eq 'STCLLI') {
		($off,$len) = (4,2);
	} elsif ($f eq 'PLCLLI') {
		($off,$len) = (0,6);
	} elsif ($f eq 'WCCLLI') {
		($off,$len) = (0,8);
	} elsif ($f eq 'SWCLLI') {
		($off,$len) = (0,11);
	}
	my @s1 = ();
	push @s1, substr($r1->{SWCLLI},$off,$len) if $r1->{SWCLLI};
	if (exists $sws{$d1}{$r1->{SWCLLI}}) {
		my $sw = $sws{$d1}{$r1->{SWCLLI}};
		push @s1, substr($sw->{ACTUAL},$off,$len) if $sw->{ACTUAL};
		push @s1, substr($sw->{AGENT} ,$off,$len) if $sw->{AGENT};
		push @s1, substr($sw->{HOST}  ,$off,$len) if $sw->{HOST};
	}
	my @s2 = ();
	push @s2, substr($r2->{SWCLLI},$off,$len) if $r2->{SWCLLI};
	if (exists $sws{$d2}{$r2->{SWCLLI}}) {
		my $sw = $sws{$d2}{$r2->{SWCLLI}};
		push @s2, substr($sw->{ACTUAL},$off,$len) if $sw->{ACTUAL};
		push @s2, substr($sw->{AGENT} ,$off,$len) if $sw->{AGENT};
		push @s2, substr($sw->{HOST}  ,$off,$len) if $sw->{HOST};
	}
	foreach my $c1 (@s1) {
		foreach my $c2 (@s2) {
			return 1 if $c1 eq $c2;
		}
	}
	return 0;
}

my %compares = (
	'neca4'=>{
		'npanxxsource'=>{
			'STCLLI'=>\&compareothers,
			'PLCLLI'=>\&compareothers,
			'WCCLLI'=>\&compareothers,
			'SWCLLI'=>\&compareothers,
		},
	},
	'npanxxsource'=>{
		'telcodata'=>{
			'STCLLI'=>\&compareothers,
			'PLCLLI'=>\&compareothers,
			'WCCLLI'=>\&compareothers,
			'SWCLLI'=>\&compareothers,
		},
		'localcallingguide'=>{
			'STCLLI'=>\&compareothers,
			'PLCLLI'=>\&compareothers,
			'WCCLLI'=>\&compareothers,
			'SWCLLI'=>\&compareothers,
		},
		'pinglo'=>{
			'STCLLI'=>\&compareothers,
			'PLCLLI'=>\&compareothers,
			'WCCLLI'=>\&compareothers,
			'SWCLLI'=>\&compareothers,
		},
		'areacodes'=>{
			'STCLLI'=>\&compareothers,
			'PLCLLI'=>\&compareothers,
			'WCCLLI'=>\&compareothers,
			'SWCLLI'=>\&compareothers,
		},
	},
);

sub comparem {
	my ($d1,$d2,$r1,$r2,$f) = @_;
	return 1 if ($r1->{$f} eq $r2->{$f});
	return 0 unless exists $compares{$d1}{$d2}{$f};
	return &{$compares{$d1}{$d2}{$f}}($d1,$d2,$r1,$r2,$f);
}

my %isolated = ();
print STDERR "I: checking isolated records...\n";
foreach my $npa (sort keys %{$db{cnts}}) {
	foreach my $nxx (sort keys %{$db{cnts}{$npa}}) {
		next if $nxx eq '555';
		next if $nxx =~ /[2-9]11/;
		next if $nxx eq '950';
		next if $nxx eq '955';
		next if $nxx eq '958';
		next if $nxx eq '959';
		next if $nxx eq '976';
		if (exists $db{cnts}{$npa}{$nxx}) {
			my @recs = (keys %{$db{cnts}{$npa}{$nxx}});
			if (@recs < 2) {
				my $dir = $recs[0];
				print STDERR "E: $npa-$nxx only has records for $dir\n";
				$isolated{$recs[0]}++;
				foreach my $x (keys %{$db{$dir}{$npa}{$nxx}}) {
					foreach my $rec (@{$db{$dir}{$npa}{$nxx}{$x}}) {
						my @values = ();
						foreach (@{$db_fields{$dir}}) {
							push @values,$rec->{$_};
						}
						print STDERR "E: $npa-$nxx($x): ",'"',join('","',@values),'"',"\n";
					}
				}
			}
		}
	}
}
print STDERR "I: ---------------------\n";
foreach my $d (@dirs) {
	my $cnt = 0;
	$cnt = $isolated{$d} if exists $isolated{$d};
	printf STDERR "I: there are %10d isolated records for $d\n", $cnt;
}
print STDERR "I: ---------------------\n";

exit;

__END__

foreach my $field (qw/STCLLI PLCLLI WCCLLI SWCLLI WCVH/) {
	my %extra = ();
	my %missd = ();
	my %commn = ();
	my %count = ();
	my %match = ();
	my %missm = ();
	my %confl = ();
	print STDERR "I: checking $field...\n";
	foreach my $npa (sort keys %{$db{mark}}) {
		foreach my $nxx (sort keys %{$db{mark}{$npa}}) {
			next if $nxx eq '555';
			next if $nxx =~ /[2-9]11/;
			next if $nxx eq '950';
			next if $nxx eq '955';
			next if $nxx eq '958';
			next if $nxx eq '959';
			next if $nxx eq '976';
			for (my $i1=0;$i1<@dirs;$i1++) {
				my $d1 = $dirs[$i1];
				my $c1 = exists $db{$d1}{$npa}{$nxx};
				if ($c1) {
					my $have = undef;
					foreach my $a1 (values %{$db{$d1}{$npa}{$nxx}}) {
						foreach my $r1 (@{$a1}) {
							if ($r1->{$field}) {
								$have = 1;
								last;
							}
						}
						last if $have;
					}
					$count{$d1}++ if $have;
				}
				for (my $i2=$i1+1;$i2<@dirs;$i2++) {
					my $d2 = $dirs[$i2];
					my $c2 = exists $db{$d2}{$npa}{$nxx};
					if ($c1 and not $c2) {
						$extra{$d1}{$d2}++;
						$missd{$d2}{$d1}++;
					} elsif ($c2 and not $c1) {
						$missd{$d1}{$d2}++;
						$extra{$d2}{$d1}++;
					} elsif ($c1 and $c2) {
						$commn{$d1}{$d2}++;
						$commn{$d2}{$d1}++;
						my $mat = undef;
						my $mis = undef;
						foreach my $a1 (values %{$db{$d1}{$npa}{$nxx}}) {
							foreach my $r1 (@{$a1}) {
								foreach my $a2 (values %{$db{$d2}{$npa}{$nxx}}) {
									foreach my $r2 (@{$a2}) {
										if ($r1->{$field} and $r2->{$field}) {
											if (comparem($d1,$d2,$r1,$r2,$field)) {
												$mat = 1;
											} else {
												$mis = 1;
											}
										}
									}
									last if $mat and $mis;
								}
								last if $mat and $mis;
							}
							last if $mat and $mis;
						}
						if ($mat and $mis) {
							$confl{$d1}{$d2}++;
							$confl{$d2}{$d1}++;
						} elsif ($mat) {
							$match{$d1}{$d2}++;
							$match{$d2}{$d1}++;
						} elsif ($mis) {
							$missm{$d1}{$d2}++;
							$missm{$d2}{$d1}++;
						}
					}
				}
			}
		}
	}
	foreach my $d1 (@dirs) {
		print STDERR "I: ---------------------\n";
		print STDERR "I: $d1\n";
		printf STDERR "I: there were %10d counts\n", $count{$d1};
		foreach my $d2 (@dirs) {
			printf STDERR "I: there were %10d (%7.2f%%) extras over $d2\n", $extra{$d1}{$d2}, 100*$extra{$d1}{$d2}/$count{$d1} if $count{$d1};
			printf STDERR "I: there were %10d (%7.2f%%) missed over $d2\n", $missd{$d1}{$d2}, 100*$missd{$d1}{$d2}/$count{$d2} if $count{$d2};
			printf STDERR "I: there were %10d (%7.2f%%) common with $d2\n", $commn{$d1}{$d2}, 100*$commn{$d1}{$d2}/$count{$d1} if $count{$d1};
			printf STDERR "I: there were %10d (%7.2f%%) matchd with $d2\n", $match{$d1}{$d2}, 100*$match{$d1}{$d2}/$commn{$d1}{$d2} if $commn{$d1}{$d2};
			printf STDERR "I: there were %10d (%7.2f%%) missma with $d2\n", $missm{$d1}{$d2}, 100*$missm{$d1}{$d2}/$commn{$d1}{$d2} if $commn{$d1}{$d2};
			printf STDERR "I: there were %10d (%7.2f%%) confli with $d2\n", $confl{$d1}{$d2}, 100*$confl{$d1}{$d2}/$commn{$d1}{$d2} if $commn{$d1}{$d2};
		}
	}
	print STDERR "I: ---------------------\n";
}

print STDERR "I: key ",'"',join('","',@dirs),'"',"\n";

exit;

1;

__END__


