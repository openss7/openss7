#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $geondir = "$progdir/../geonames";
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

sub vh2ll {
	my ($v,$h) = @_;
	my ($la,$lo) = Geo::Coordinates::VandH->vh2ll($v,$h);
	$lo = -$lo;
	$lo += 360 if $lo < -180;
	$lo -= 360 if $lo >  180;
	return ($la,$lo);
}

sub ll2vh {
	my ($lat,$lon) = @_;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=-5000;$v<=10000;$v+=5000) {
		for (my $h=-5000;$h<=30000;$h+=5000) {
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
	for (my $v=$gv-8000;$v<=$gv+8000;$v+=2000) {
		for (my $h=$gh-8000;$h<=$gh+8000;$h+=2000) {
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
	for (my $v=$gv-4000;$v<=$gv+4000;$v+=1000) {
		for (my $h=$gh-4000;$h<=$gh+4000;$h+=1000) {
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
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=500) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=500) {
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
	for (my $v=$gv-800;$v<=$gv+800;$v+=200) {
		for (my $h=$gh-800;$h<=$gh+800;$h+=200) {
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
	for (my $v=$gv-400;$v<=$gv+400;$v+=100) {
		for (my $h=$gh-400;$h<=$gh+400;$h+=100) {
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
	for (my $v=$gv-200;$v<=$gv+200;$v+=50) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=50) {
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
	for (my $v=$gv-80;$v<=$gv+80;$v+=20) {
		for (my $h=$gh-80;$h<=$gh+80;$h+=20) {
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
	for (my $v=$gv-40;$v<=$gv+40;$v+=10) {
		for (my $h=$gh-40;$h<=$gh+40;$h+=10) {
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
	for (my $v=$gv-20;$v<=$gv+20;$v+=5) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=5) {
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
	for (my $v=$gv-8;$v<=$gv+8;$v+=2) {
		for (my $h=$gh-8;$h<=$gh+8;$h+=2) {
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
	for (my $v=$gv-4;$v<=$gv+4;$v+=1) {
		for (my $h=$gh-4;$h<=$gh+4;$h+=1) {
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

my %npaccst = ();
our %nanpst = ();

$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	my ($npa,$cc,$st,$name) = split(/\t/,$_);
	$nanpst{$npa} = $st;
	$npaccst{$npa} = [ $cc, $st ];
}
close($fh);

my %nxxccst = ();

$fn = "$codedir/nxxst.txt";
print STDER "I: reading $fn\n";
open ($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next unless /^[0-9]+/;
	my ($npa,$nxx,$cc,$st,$name) = split(/\t/,$_);
	$nxxccst{$npa}{$nxx} = [ $cc, $st ];
}
close($fh);

our %lergcc = ();
our %lergst = ();
our %lergps = ();
our %cllist = ();
our %cllicc = ();
our %cllirg = ();
our %statrg = ();
our %ccstrg = ();
our %pcpsda = ();
our %countries = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^\s*$/;
	my @tokens = split(/\t/,$_);
	$pcpsda{$tokens[4]}{$tokens[5]} = [ $tokens[0], $tokens[1], $tokens[2], $tokens[3] ];
	$ccstrg{$tokens[0]}{$tokens[1]} = $tokens[2];
	$statrg{$tokens[1]} = $tokens[2];
	$lergcc{$tokens[2]} = $tokens[0];
	$lergst{$tokens[2]} = $tokens[1];
	$lergps{$tokens[2]} = [ $tokens[4], $tokens[5] ];
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
	$countries{$tokens[0]}++;
}
close($fh);

sub getnxxccst {
	my $dat = shift;
	my ($npa,$nxx) = ($dat->{NPA},$dat->{NXX});
	my ($cc,$st,$rg,$pc,$ps);
	($cc,$st) = @{$npaccst{$npa}} if exists $npaccst{$npa};
	($cc,$st) = @{$nxxccst{$npa}{$nxx}} if exists $nxxccst{$npa}{$nxx};
	if ($cc and $st) {
		if (exists $ccstrg{$cc}{$st}) {
			$rg = $ccstrg{$cc}{$st};
			if (exists $lergps{$rg}) {
				($pc,$ps) = @{$lergps{$rg}};
			} else {
				print STDERR "E: $npa-$nxx: no PC/PS for REGION $rg\n";
			}
		} else {
			print STDERR "E: $npa-$nxx: no REGION for CC/ST $cc-$st\n";
		}
	}
	return ($cc,$st,$rg,$pc,$ps);
}

my $corrected = 0;
my %corrections = ();

$fn = "$datadir/db.cor.csv";
if (-f $fn) {
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $heading = 1;
	my @cfields = ();
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@cfields = @tokens;
			$heading = undef;
			next;
		}
		my ($npa,$nxx,$x,$fld,$old,$new,$com) = @tokens;
		$corrections{$npa}{$nxx}{$x} = [] unless exists $corrections{$npa}{$nxx}{$x};
		push @{$corrections{$npa}{$nxx}{$x}}, [$fld,$old,$new,$com];
	}
	close($fh);
}

sub correct {
	my ($dat,$fld,$old,$new,$com) = @_;
	my ($npa,$nxx,$x) = ($dat->{NPA},$dat->{NXX},$dat->{X});
	my $numb = "$npa-$nxx"; $numb .= "($x)" if exists $dat->{X} and length($dat->{X});
	$corrections{$npa}{$nxx}{$x} = [] unless exists $corrections{$npa}{$nxx}{$x};
	foreach (@{$corrections{$npa}{$nxx}{$x}}) {
		if ($_->[0] eq $fld and $_->[2] eq $new) { return; }
	}
	print STDERR "C: $numb: $fld: $old \-> $new ($com)\n";
	push @{$corrections{$npa}{$nxx}{$x}}, [$fld,$old,$new,$com];
	$corrected++;
}

my $pattern = '^........(';

{
$fn = "$codedir/matching.txt";
print STDERR "I: reading $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
my $sep = '';
while (<$fh>) { chomp;
	my ($numb,$pat,$regex) = split(/\t/,$_);
	next unless $numb >= 50 or $pat =~ /[x]/ or ($pat =~ /[an]/ and $numb >= 5);
	next if $pat =~ /[a]/;
	$pattern .= "$sep$regex";
	$sep = '|';
}
close($fh);
$pattern .= ')$';
}

sub checkclli {
	my ($clli,$pfx,$dat) = @_;
	my $result = 1;
	my %skipcllis = (
		'XXXXXXXXXXX'=>1,
		'NOCLLIKNOWN'=>1,
		'__VARIOUS__'=>1,
		'99999999999'=>1,
		'ALL SWITCHE'=>1,
		'ALLSWITCHES'=>1,
	);
	return 0 if exists $skipcllis{$clli};
	if (length($clli) != 11) {
		print STDERR "E: $pfx CLLI $clli is not 11 characters long\n";
		$dat->{BADCLLI} = 'FIXME: wrong length';
		return 0;
	}
	unless (exists $cllicc{substr($clli,4,2)}) {
		print STDERR "E: $pfx CLLI $clli has bad region ",substr($clli,4,2),"\n";
		$dat->{BADCLLI} = 'FIXME: bad region';
		$result = 0;
	}
	unless ($clli =~ /$pattern/) {
		print STDERR "W: $pfx CLLI $clli has equip ",substr($clli,8,3)," that is poorly formatted\n";
		$dat->{BADCLLI} = 'FIXME: poor equip';
	}
	return $result;
}

my %sws = ();
my %wcs = ();
my %pls = ();
my %dbsw = ();

my @sw_keys = qw/SWCLLI NPA NXX X LATA OCN NAME TYPE WCVH WCLL RCVH RCLL SPC SECT FEAT STP1 STP2 ACTUAL AGENT HOST/;
my @wc_keys = qw/WCCLLI NPA NXX X LATA OCN SWCLLI WCVH WCLL RCVH RCLL WCADDR WCCITY WCCOUNTY WCZIP SECT WCNAME WCCC WCST/;
my @pl_keys = qw/PLCLLI NPA NXX X LATA OCN WCCLLI/;
my @dbsw_keys = qw/NPA NXX X SWCLLI LATA OCN/;

require "$progdir/mapping.sw.pm";

my ($oldnpa,$oldrg);

$fn = "$progdir/db.csv";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
my %fieldnos = ();
while (<$fh>) { chomp;
	next unless /^"/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		for (my $i=0;$i<@fields;$i++) {
			$fieldnos{$fields[$i]} = $i;
		}
		$heading = undef;
		next;
	}
	my ($npa,$nxx,$x) = @tokens;
	if (exists $corrections{$npa}{$nxx}{$x}) {
		foreach my $cor (@{$corrections{$npa}{$nxx}{$x}}) {
			if ($cor->[0] eq '**DELETE**') { $npa = undef; last; }
			if (exists $fieldnos{$cor->[0]}) {
				$tokens[$fieldnos{$cor->[0]}] = $cor->[2];
			}
		}
	}
	next unless defined $npa;
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping::mapping{$fields[$i]}) {
			&{$mapping::mapping{$fields[$i]}}($data,$fields[$i],$tokens[$i]);
		} else {
			print STDERR "E: no mapping for '$fields[$i]'\n";
		}
	}
	#print STDERR "I: processing $data->{NPA} $data->{NXX} $data->{X}\n";
	my $rg = $data->{REGION};
	if ($npa ne $oldnpa and $rg ne $oldrg) {
		print STDERR "I: processing $npa $rg...\n";
	}
	($oldnpa,$oldrg) = ($npa,$rg);
	my $cc = $data->{WCCC};
	my $st = $data->{WCST};
	my $sw = $data->{SWCLLI};
	next unless length($sw) == 11;
	my $wc = $data->{WCCLLI};
	my $pl = $data->{PLCLLI};
	my $ct = $data->{CTCLLI};
	{
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}} = {} unless exists
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		my $rec = $dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		foreach my $k (@dbsw_keys) {
			if ($k eq 'SWCLLI') {
				$rec->{$k}{$data->{$k}}++ if length($data->{$k});
			} else {
				if (length($data->{$k})) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) DBRC $data->{NPA}-$data->{NXX}-$data->{X} $k changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $sw) {
		$sws{$cc}{$st}{$sw} = {} unless exists $sws{$cc}{$st}{$sw};
		my $rec = $sws{$cc}{$st}{$sw};
		foreach my $k (@sw_keys) {
			if ($k =~ /^(NPA|LATA|OCN|SECT|NAME)$/) {
				$rec->{$k}{$data->{$k}}++ if length($data->{$k});
			} elsif ($k eq 'FEAT') {
				foreach my $f (split(/\s+/,$data->{$k})) {
					$rec->{$k}{$f}++ if $f;
				}
			} elsif ($k =~ /^(WCVH|WCLL|RCVH|RCLL)$/) {
				if (length($data->{$k})) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$data->{$k})) {
						next if not $m or exists $rec->{$K}{$m};
						$rec->{$K}{$m}++;
						if ($rec->{$k}) {
							unless ($K eq 'LL' or substr($k,0,2) eq 'RC') {
								if ($rec->{$k} =~ /;/) {
									print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$m' ($data->{SECT})\n";
								} else {
									#my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
									#print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k old value '$rec->{$k}' ($sects)\n";
									#print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k new value '$m' ($data->{SECT})\n";
								}
							}
							$rec->{$k} = join(';',$rec->{$k},$m);
						} else {
							$rec->{$k} = $m;
						}
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
			} else {
				if (length($data->{$k})) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $wc) {
		$wcs{$cc}{$st}{$wc} = {} unless exists $wcs{$cc}{$st}{$wc};
		my $rec = $wcs{$cc}{$st}{$wc};
		foreach my $k (@wc_keys) {
			if ($k =~ /^(NPA|LATA|OCN|SECT|SWCLLI)$/) {
				$rec->{$k}{$data->{$k}}++ if length($data->{$k});
			} elsif ($k =~ /^(WCVH|WCLL|RCVH|RCLL)$/) {
				if (length($data->{$k})) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$data->{$k})) {
						next if not $m or exists $rec->{$K}{$m};
						$rec->{$K}{$m}++;
						if ($rec->{$k}) {
							unless ($K eq 'LL' or substr($k,0,2) eq 'RC') {
								if ($rec->{$k} =~ /;/) {
									print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$m' ($data->{SECT})\n";
								} else {
									#my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
									#print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k old value '$rec->{$k}' ($sects)\n";
									#print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k new value '$m' ($data->{SECT})\n";
								}
							}
							$rec->{$k} = join(';',$rec->{$k},$m);
						} else {
							$rec->{$k} = $m;
						}
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
			} else {
				if (length($data->{$k})) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $pl) {
		$pls{$cc}{$st}{$pl} = {} unless exists $pls{$cc}{$st}{$pl};
		my $rec = $pls{$cc}{$st}{$pl};
		foreach my $k (@pl_keys) {
			if ($k =~ /^(NPA|LATA|OCN|SECT|WCCLLI)$/) {
				$rec->{$k}{$data->{$k}}++ if length($data->{$k});
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
			} else {
				if (length($data->{$k})) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) PL $pl $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
}
close($fh);

printf STDERR "I: --------------------------\n";

$fn = "$datadir/db.sw.nogeo.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@dbsw_keys),'"',"\n";
foreach my $npa (sort keys %dbsw) {
	foreach my $nxx (sort keys %{$dbsw{$npa}}) {
		foreach my $x (sort keys %{$dbsw{$npa}{$nxx}}) {
			my $rec = $dbsw{$npa}{$nxx}{$x};
			my @values = ();
			foreach (@dbsw_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/sw.nogeo.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@sw_keys),'"',"\n";
foreach my $cc (sort keys %sws) {
	foreach my $st (sort keys %{$sws{$cc}}) {
		foreach my $sw (sort keys %{$sws{$cc}{$st}}) {
			my $rec = $sws{$cc}{$st}{$sw};
			my @values = ();
			foreach (@sw_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						if ($_ eq 'FEAT') {
							$sep = ' ';
						} elsif ($_ eq 'NAME') {
							$sep = ';';
						} else {
							foreach my $k (keys %{$rec->{$_}}) {
								if ($k =~ /,/) { $sep = ';'; last; }
							}
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/wc.nogeo.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@wc_keys),'"',"\n";
foreach my $cc (sort keys %wcs) {
	foreach my $st (sort keys %{$wcs{$cc}}) {
		foreach my $wc (sort keys %{$wcs{$cc}{$st}}) {
			my $rec = $wcs{$cc}{$st}{$wc};
			my @values = ();
			foreach (@wc_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/pl.nogeo.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@pl_keys),'"',"\n";
foreach my $cc (sort keys %pls) {
	foreach my $st (sort keys %{$pls{$cc}}) {
		foreach my $pl (sort keys %{$pls{$cc}{$st}}) {
			my $rec = $pls{$cc}{$st}{$pl};
			my @values = ();
			foreach (@pl_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

if ($corrected > 0) {
	my @cor_keys = qw/NPA NXX X FIELD OLD NEW COMMENT/;
	$fn = "$datadir/db.cor.csv";
	print STDERR "I: writing $fn\n";
	open($of,">:utf8",$fn) or die "can't write $fn";
	print $of '"',join('","',@cor_keys),'"',"\n";
	foreach my $npa (sort keys %corrections) {
		foreach my $nxx (sort keys %{$corrections{$npa}}) {
			foreach my $x (sort keys %{$corrections{$npa}{$nxx}}) {
				foreach my $rec (@{$corrections{$npa}{$nxx}{$x}}) {
					print $of '"',join('","',$npa,$nxx,$x,@{$rec}),'"',"\n";
				}
			}
		}
	}
	close($of);
}

exit;

1;

__END__

