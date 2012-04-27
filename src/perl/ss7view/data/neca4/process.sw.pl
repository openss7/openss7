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
my $fn;

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
	for (my $v=$gv-6000;$v<=$gv+6000;$v+=2000) {
		for (my $h=$gh-6000;$h<=$gh+6000;$h+=2000) {
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
	for (my $v=$gv-3000;$v<=$gv+3000;$v+=1000) {
		for (my $h=$gh-3000;$h<=$gh+3000;$h+=1000) {
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
	for (my $v=$gv-1500;$v<=$gv+1500;$v+=500) {
		for (my $h=$gh-1500;$h<=$gh+1500;$h+=500) {
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
	for (my $v=$gv-600;$v<=$gv+600;$v+=200) {
		for (my $h=$gh-600;$h<=$gh+600;$h+=200) {
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
	for (my $v=$gv-300;$v<=$gv+300;$v+=100) {
		for (my $h=$gh-300;$h<=$gh+300;$h+=100) {
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
	for (my $v=$gv-150;$v<=$gv+150;$v+=50) {
		for (my $h=$gh-150;$h<=$gh+150;$h+=50) {
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
	for (my $v=$gv-60;$v<=$gv+60;$v+=20) {
		for (my $h=$gh-60;$h<=$gh+60;$h+=20) {
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
	for (my $v=$gv-30;$v<=$gv+30;$v+=10) {
		for (my $h=$gh-30;$h<=$gh+30;$h+=10) {
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
	for (my $v=$gv-15;$v<=$gv+15;$v+=5) {
		for (my $h=$gh-15;$h<=$gh+15;$h+=5) {
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
	for (my $v=$gv-6;$v<=$gv+6;$v+=2) {
		for (my $h=$gh-6;$h<=$gh+6;$h+=2) {
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
	for (my $v=$gv-3;$v<=$gv+3;$v+=1) {
		for (my $h=$gh-3;$h<=$gh+3;$h+=1) {
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

my %nanpst = ();

$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	my ($npa,$cc,$st,$name) = split(/\t/,$_);
	$nanpst{$npa} = $st;
}
close($fh);

my %lergcc = ();
my %lergst = ();
my %cllist = ();
my %cllicc = ();
my %cllirg = ();
my %statrg = ();
my %countries = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^\s*$/;
	my @tokens = split(/\t/,$_);
	$statrg{$tokens[1]} = $tokens[2];
	$lergcc{$tokens[2]} = $tokens[0];
	$lergst{$tokens[2]} = $tokens[1];
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
	$countries{$tokens[0]}++;
}
close($fh);

my @gn_fields = (
	'geonameid',
	'name',
	'asciiname',
	'alternatenames',
	'latitude',
	'longitude',
	'feature class',
	'feature code',
	'country code',
	'cc2',
	'admin1 code',
	'admin2 code',
	'admin3 code',
	'admin4 code',
	'population',
	'elevation',
	'dem',
	'timezone',
	'modification date',
);

my %gn_dontneed = (
	'alternatenames'=>1,
	'admin2 code'=>1,
	'admin3 code'=>1,
	'admin4 code'=>1,
	'population'=>1,
	'elevation'=>1,
	'dem'=>1,
	'timezone'=>1,
);

sub normalize {
	my $nm = shift;
	$nm =~ s/-/ /g;
	$nm =~ s/[[:punct:]]//g;
	$nm =~ s/^\s+$//;
	$nm =~ s/^\s+//;
	$nm =~ s/\s+$//;
	$nm =~ s/ \([^\)]*\)//g;
	$nm =~ s/ \(historical\)//i;
	$nm =~ s/\b(The )?Villages? of (the )?//i;
	$nm =~ s/\bIsle of //i;
	$nm =~ s/\bCity of //i;
	$nm =~ s/\bEstates of //i;
	$nm =~ s/\bUnorganized Territory of //i;
	$nm =~ s/\bTown(ship)? of //i;
	$nm =~ s/\bBorough of //i;
	$nm =~ s/\bPoint of //i;
	$nm =~ s/\BLakes? of the //i;
	$nm =~ s/\bHead of (the )?//i;
	#$nm =~ s/ Subdivision\b//i;
	#$nm =~ s/ County$//i;
	#$nm =~ s/ (Military|Indian)? Reservation$//i;
	#$nm =~ s/ Town(ship)?$//i;
	#$nm =~ s/ City$//i;
	#$nm =~ s/ Village$//i;
	#$nm =~ s/^New //i;
	#$nm =~ s/^Old //i unless $nm =~ /Old Rural Town/i;
	$nm =~ s/\bD\s+C\b/DC/;
	$nm =~ s/\bSaint\b/St/gi;
	$nm =~ s/\bFort\b/Ft/gi;
	$nm =~ s/\bSainte\b/Ste/gi;
	$nm =~ s/\bRoad\b/Rd/gi;
	$nm =~ s/\bCenter\b/Ctr/gi;
	$nm =~ s/\bMount\b/Mt/gi;
	$nm =~ s/\bJunction\b/Jct/gi;
	$nm =~ s/\bCourt\b/Ct/gi;
	$nm =~ s/\bCompany\b/Co/gi;
	$nm =~ s/\bFalls\b/Fls/gi;
	$nm =~ s/\bSprints\b/Spg/gi;
	$nm =~ s/\bBuilding\b/Bldg/gi;
	$nm =~ s/\bCreek\b/Cr/gi;
	$nm =~ s/\bLake\b/Lk/gi;
	$nm =~ s/\bValey\b/Vly/gi;
	$nm =~ s/\bNorth.?East\b/NE/gi;
	$nm =~ s/\bNorth.?West\b/NW/gi;
	$nm =~ s/\bSouth.?East\b/SE/gi;
	$nm =~ s/\bSouth.?West\b/SW/gi;
	$nm =~ s/\bEast\b/E/gi;
	$nm =~ s/\bWest\b/W/gi;
	$nm =~ s/\bNorth\b/N/gi;
	$nm =~ s/\bSouth\b/S/gi;
	$nm =~ s/\bAir Force Base\b/AFB/gi;
	$nm =~ s/\bAir Force Station\b/AFS/gi;
	$nm =~ s/\bAir Station\b/AS/gi;
	$nm = "\U$nm\E";
	return $nm;
}

sub cmpnames {
	my ($nm1,$nm2) = @_;
	my $n1 = normalize($nm1);
	my $n2 = normalize($nm2);
	return 1 if $n1 eq $n2;
	($n1,$n2) = ($n2,$n1) if length($n2) < length($n1);
	my $dist = distance($n1,$n2);
	return 1 if $dist <= 3 and $dist < length($n1)/3;
	#return 1 if $dist < 3 or $dist < length($n2)/3 or $dist < length($n2) - length($n1) + 3;
	{
		my $a1 = $n1; $a1 =~ s/[AEIOU ]//gi; $a1 =~ s/([A-Z])\1/\1/gi;
		my $a2 = $n2; $a2 =~ s/[AEIOU ]//gi; $a2 =~ s/([A-Z])\1/\1/gi;
		($a1,$a2) = ($a2,$a1) if length($a2) < length($a1);
		if (length($a1) > 4) {
			return 1 if $a1 eq $a2;
			return 1 if index($a2,$a1) != -1 and length($a1) >= length($a2)/3;
		}
	}
	my @t1 = split(/\s+/,$n1);
	my @t2 = split(/\s+/,$n2);
	my (@c1,@c2);
	if (scalar @t1 < scalar @t2) {
		@c1 = @t1; @c2 = @t2;
	} else {
		@c1 = @t2; @c2 = @t1;
	}
	my $cnt = scalar @c1;
	my $mtc = 0;
	foreach my $k1 (@c1) {
		foreach my $k2 (@c2) {
			if ($k1 eq $k2) {
				$mtc++;
				last;
			} else {
				my ($i1,$i2,$good) = (0,0,0);
				my ($s1,$s2);
				if (length($k1)<length($k2)) {
					($s1,$s2) = ($k1,$k2);
				} else {
					($s1,$s2) = ($k2,$k1);
				}
				while ($i1<length($s1)) {
					if (substr($s1,$i1,1) eq substr($s2,$i2,1)) {
						$good++; $i1++; $i2++;
					} elsif (substr($s1,$i1,1) eq substr($s2,$i2+1,1)) {
						$good++; $i1++; $i2+=2;
					} elsif (substr($s1,$i1+1,1) eq substr($s2,$i2,1)) {
						$good++; $i1+=2; $i2++;
					} elsif (substr($s1,$i1,1) eq substr($s2,$i2+2,1)) {
						$good++; $i1++; $i2+=3;
					} else {
						$i1++; $i2++;
					}
				}
				if ($good/length($s1) > 0.8 and $good/length($s2) > 0.3) {
					$mtc += 0.5;
					last;
				}
			}
		}
	}
	return 1 if $mtc > $cnt/2;
	#print STDERR "I: no match: '$nm1' '$nm2'\n";
	return 0;
}

sub clliize {
	my $nm = shift;
	$nm =~ s/-/ /g;
	$nm =~ s/[[:punct:]]//g;
	$nm =~ s/^\s+$//;
	$nm =~ s/^\s+//;
	$nm =~ s/\s+$//;
	$nm =~ s/ \([^\)]*\)//g;
	$nm =~ s/ \(historical\)//i;
	$nm =~ s/\b(The )?Villages? of (the )?//i;
	$nm =~ s/\bIsle of //i;
	$nm =~ s/\bCity of //i;
	$nm =~ s/\bEstates of //i;
	$nm =~ s/\bUnorganized Territory of //i;
	$nm =~ s/\bTown(ship)? of //i;
	$nm =~ s/\bBorough of //i;
	$nm =~ s/\bPoint of //i;
	$nm =~ s/\BLakes? of the //i;
	$nm =~ s/\bHead of (the )?//i;
	#$nm =~ s/ Subdivision\b//i;
	#$nm =~ s/ County$//i;
	#$nm =~ s/ (Military|Indian)? Reservation$//i;
	#$nm =~ s/ Town(ship)?$//i;
	#$nm =~ s/ City$//i;
	#$nm =~ s/ Village$//i;
	#$nm =~ s/^New //i;
	#$nm =~ s/^Old //i unless $nm =~ /Old Rural Town/i;
	$nm =~ s/\bD\s+C\b/DC/;
	#$nm =~ s/\bSaint\b/St/gi;
	#$nm =~ s/\bFort\b/Ft/gi;
	#$nm =~ s/\bSainte\b/Ste/gi;
	#$nm =~ s/\bRoad\b/Rd/gi;
	#$nm =~ s/\bCenter\b/Ctr/gi;
	#$nm =~ s/\bMount\b/Mt/gi;
	#$nm =~ s/\bJunction\b/Jct/gi;
	#$nm =~ s/\bCourt\b/Ct/gi;
	#$nm =~ s/\bCompany\b/Co/gi;
	#$nm =~ s/\bFalls\b/Fls/gi;
	#$nm =~ s/\bSprints\b/Spg/gi;
	#$nm =~ s/\bBuilding\b/Bldg/gi;
	#$nm =~ s/\bCreek\b/Cr/gi;
	#$nm =~ s/\bLake\b/Lk/gi;
	#$nm =~ s/\bValey\b/Vly/gi;
	$nm =~ s/\bNorth.?East\b/NE/gi;
	$nm =~ s/\bNorth.?West\b/NW/gi;
	$nm =~ s/\bSouth.?East\b/SE/gi;
	$nm =~ s/\bSouth.?West\b/SW/gi;
	#$nm =~ s/\bEast\b/E/gi;
	#$nm =~ s/\bWest\b/W/gi;
	#$nm =~ s/\bNorth\b/N/gi;
	#$nm =~ s/\bSouth\b/S/gi;
	$nm =~ s/\bAir Force Base\b/AFB/gi;
	$nm =~ s/\bAir Force Station\b/AFS/gi;
	$nm =~ s/\bAir Station\b/AS/gi;
	$nm = "\U$nm\E";
	return $nm;
}

sub cmpcllis {
	my ($ct,$nm) = @_;
	my $c = $ct; $c =~ s/[ _]//g;
	my $n = clliize($nm); $n =~ s/ //g;
	my ($ic,$in) = (0,-1);
	if (substr($c,0,1) eq substr($n,0,1)) {
		while ($ic<length($c) and $in<length($n)) {
			$in = index($n,substr($c,$ic,1),$in+1);
			last if $in < 0;
			$ic++;
		}
		# ok if last unmatched letter is X, Y or Z or a number
		if ($ic==3 and $ic==length($c)-1 and substr($c,$ic,1) =~ /[XYZ0-9]/) { return 1; }
		if ($ic>=length($c)) { return 1; }
	}
	return 0;
}

my %a1codes = (
	'01'=>'AB',
	'02'=>'BC',
	'03'=>'MB',
	'04'=>'NB',
	'13'=>'NT',
	'07'=>'NS',
	'14'=>'NU',
	'08'=>'ON',
	'09'=>'PE',
	'10'=>'QC',
	'11'=>'SK',
	'12'=>'YT',
	'05'=>'NL',
);

my $maxradius = 5; # maximum of 100 miles (50 mile radius)

my %geonames = ();
my %features = ();

foreach my $code (qw/cities1000 AG AI AS BB BM BS CA DM DO GD GU JM KN KY LC MP MS PR SX TC TT US VC VG VI/) {
	my ($good,$bad) = (0,0);
	$fn = "$geondir/$code.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $totals = `wc -l $fn`; chomp $totals; $totals = int($totals); $totals--;
	my $heading = 1;
	my @fields = ();
	my $count = 0;
	while (<$fh>) { chomp; s/\r//g;
		s/^"//; s/"$//; my @tokens = split('","',$_);
		if ($heading) { @fields = @tokens; $heading = undef; next; }
		my %data = ();
		for (my $i=0;$i<@fields;$i++) {
			next if exists $gn_dontneed{$fields[$i]};
			$data{$fields[$i]} = $tokens[$i] if $tokens[$i] or length($tokens[$i]);
		}
		$count++;
		print STDERR "I: uniq: $good, dupl: $bad, totl: $count/$totals\r" if $count % 1000 == 0;
		my $cc = $data{'country code'};
		my $st = $cc; $st = $data{'admin1 code'} if $cc eq 'US' or $cc eq 'CA';
		$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
		my $fc = $data{'feature class'};
		$fc = 'C' if $code eq 'cities1000';
		my $nm = normalize($data{asciiname});
		if (exists $geonames{$cc}{$st}{$nm}{$fc}) {
			my $rec = $geonames{$cc}{$st}{$nm}{$fc};
			if (ref $rec eq 'ARRAY') {
				if ($rec->[0]{'modification date'} le $data{'modification date'}) {
					unshift @{$rec}, \%data;
				} else {
					push @{$rec}, \%data;
				}
			} else {
				if ($rec->{'modification date'} le $data{'modification date'}) {
					$geonames{$cc}{$st}{$nm}{$fc} = [ \%data, $rec ];
				} else {
					$geonames{$cc}{$st}{$nm}{$fc} = [ $rec, \%data ];
				}
				$good--; $bad++;
			}
		} else {
			$geonames{$cc}{$st}{$nm}{$fc} = \%data; $good++;
		}
		my ($v,$h) = ($data{vertical},$data{horizontal});
		for (my $lev=-4;$lev<=0;$lev++) {
			my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
			my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
			$features{$fc}{$lev}{"$vf,$hf"} = [] unless exists $features{$fc}{$lev}{"$vf,$hf"};
			push @{$features{$fc}{$lev}{"$vf,$hf"}}, \%data;
		}
	}
	close($fh);
	print STDERR "I: uniq: $good, dupl: $bad, totl: $count/$totals\n";
}

sub closestname {
	my ($v,$h,$nm) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-4;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/C P A L S T/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				next unless cmpnames($nm,$_->{asciiname}) or cmpnames($nm,$_->{name});
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub closestclli {
	my ($v,$h,$ct) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-4;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/C P A L S T/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				next unless cmpcllis($ct,$_->{asciiname}) or cmpcllis($ct,$_->{name});
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub closestcity {
	my ($v,$h) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-4;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/C/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub closestfeat {
	my ($v,$h) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-4;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/P A L S T/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub bestname {
	my ($cc,$st,$nm) = @_;
	my $geo;
	foreach my $fc (qw/C P A L S T/) {
		foreach my $ln (keys %{$geonames{$cc}{$st}}) {
			next unless exists $geonames{$cc}{$st}{$ln}{$fc};
			$_ = $geonames{$cc}{$st}{$ln}{$fc};
			if (ref $_ ne 'ARRAY' or $_ = $_->[0]) {
				unless (cmpnames($nm,$_->{asciiname}) or cmpnames($nm,$_->{name})) {
					#print STDERR "I: $cc-$st '$nm' does not match name $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n" unless defined $geo;
					next;
				} else {
					#printf STDERR "I: $cc-$st '$nm' matches %3.6f miles $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n", $d;
				}
				$geo = $_;
			}
			last if $geo;
		}
		last if $geo;
	}
	return $geo;
}

sub bestclli {
	my ($cc,$st,$ct) = @_;
	my $geo;
	foreach my $fc (qw/C P A L S T/) {
		foreach my $ln (keys %{$geonames{$cc}{$st}}) {
			next unless exists $geonames{$cc}{$st}{$ln}{$fc};
			$_ = $geonames{$cc}{$st}{$ln}{$fc};
			if (ref $_ ne 'ARRAY' or $_ = $_->[0]) {
				unless (cmpcllis($ct,$_->{asciiname}) or cmpcllis($ct,$_->{name})) {
					#print STDERR "I: $cc-$st '$ct' does not match name $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n" unless defined $geo;
					next;
				} else {
					#printf STDERR "I: $cc-$st '$ct' matches %3.6f miles $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n", $d;
				}
				$geo = $_;
			}
			last if $geo;
		}
		last if $geo;
	}
	return $geo;
}

my ($found,$unusable,$failed,$recovered,$toofar);

sub lookupgeo {
	my ($cc,$st,$nm,$v,$h) = @_;
	my $ln = normalize($nm);
	my $geo;
	if (exists $geonames{$cc}{$st}{$ln}) {
		foreach my $fc (qw/C P A L S T/) {
			next unless exists $geonames{$cc}{$st}{$ln}{$fc};
			if (ref $geonames{$cc}{$st}{$ln}{$fc} ne 'ARRAY') {
				$geo = $geonames{$cc}{$st}{$ln}{$fc};
				if ($v and $h) {
					my ($vp,$hp) = ($geo->{vertical},$geo->{horizontal});
					my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
					if ($d > $maxradius) {
						#printf STDERR "I: $cc-$st '$nm' too far %3.6f miles $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}'\n", $d;
						$geo = undef;
						$toofar++;
					}
				}
			} else {
				if ($v and $h) {
					my $delta;
					foreach (@{$geonames{$cc}{$st}{$ln}{$fc}}) {
						my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
						my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
						if ($d > $maxradius) {
							#printf STDERR "I: $cc-$st '$nm' too far %3.6f miles $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n", $d;
							next;
						}
						if (defined $delta) {
							if ($d < $delta) { $delta = $d; $geo = $_; }
						} else { $delta = $d; $geo = $_; }
					}
				} else {
					# just have to take the first one on the list
					$geo = $geonames{$cc}{$st}{$ln}{$fc}[0];
				}
				unless (defined $geo) {
						print STDERR "W: cannot use geoname for $cc-$st-$nm\n";
					$unusable++;
				}
			}
			if (defined $geo) { $found++; last; }
		}
	} else {
		#print STDERR "W: cannot find geoname for $cc-$st-$nm\n";
		$failed++;
	}
	return $geo;
}

my ($nolook,$noname,$nocity,$nofeat,$noclli);
my ($alook,$aname,$acity,$afeat,$aclli);

sub geoassign {
	my ($data,$geo) = @_;
	$data->{WCGEOID} = $geo->{geonameid};
	$data->{WCGN} = $geo->{name};
	$data->{WCGEOVH} = sprintf('%05d,%05d', $geo->{vertical}, $geo->{horizontal});
	$data->{WCGEOLL} = "$geo->{latitude},$geo->{longitude}";
	$data->{WCCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
	$data->{WCNAME} = $geo->{name} unless $data->{WCNAME};
	$data->{WCCC} = $geo->{'country code'} if $geo->{'country code'};
	$data->{WCST} = $geo->{'admin1 code'} if length($geo->{'admin1 code'});
	$data->{WCST} = $a1codes{$data->{WCST}} if $data->{WCCC} eq 'CA' and exists $a1codes{$data->{WCST}};
}

sub geoshowfail {
	my ($cc,$st,$ct,$nm,$data,$geo,$kind,$dist) = @_;
	my $vh = sprintf('%05d,%05d',$geo->{vertical},$geo->{horizontal});
	my $ll = "$geo->{latitude},$geo->{longitude}";
	my $gn = $geo->{asciiname};
	my $fc = "$geo->{'feature class'}.$geo->{'feature code'}";
	$cc = $geo->{'country code'};
	$st = $geo->{'admin1 code'};
	$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
	my $mi = sprintf(' %.2f mi', $dist) if defined $dist;
	printf STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' $kind: $fc$mi $ll ($vh $gn)\n", $dist;
}

sub geofound {
	my ($cc,$st,$ct,$nm,$data,$geo,$kind) = @_;
	geoassign($data,$geo);
	print STDERR "I: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' found as $kind $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}' ($geo->{asciiname})\n";
}

sub georecover {
	my ($cc,$st,$ct,$nm,$data,$geo,$kind,$dist) = @_;
	geoassign($data,$geo);
	geoshowfail($cc,$st,$ct,$nm,$data,$geo,$kind,$dist);
	$found++; $failed--; $recovered++;
	print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' recovered as $kind $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}' ($geo->{asciiname})\n";
}

sub getll {
	my $data = shift;
	my ($v,$h) = split(/,/,$data->{WCVH});
	my $ll = join(',',vh2ll($v,$h)) if $v and $h;
	return $ll;
}

sub getvh {
	my $data = shift;
	my ($v,$h) = split(/,/,$data->{WCVH});
	my $vh = sprintf('%05d,%05d',$v,$h) if $v and $h;
	return $vh;
}

my %sws = ();
my %wcs = ();
my %pls = ();
my %dbsw = ();

my @sw_keys = qw/SWCLLI NPA NXX X LATA OCN NAME TYPE WCVH WCLL SPC SECT/;
my @wc_keys = qw/WCCLLI NPA NXX X LATA OCN SWCLLI WCVH WCLL WCADDR WCCITY WCCOUNTY SECT WCGEOID WCGN WCGEOVH WCGEOLL WCCODE WCNAME WCCC WCST/;
my @pl_keys = qw/PLCLLI NPA NXX X LATA OCN WCCLLI/;

my @dbsw_keys = qw/NPA NXX X SWCLLI LATA OCN/;

my %skipcllis = (
	'XXXXXXXXXXX'=>1,
	'__VARIOUS__'=>1,
);

my $olddata;

$fn = "$progdir/db.csv";
print STDERR "I: processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	next unless /^"/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{"\U$fields[$i]\E"} = $tokens[$i] if length($tokens[$i]);
	}
	if ($olddata ne substr($data->{CLLI},4,2)) {
		$olddata = substr($data->{CLLI},4,2);
		print STDERR "I: processing $olddata\n";
	}
	#print STDERR "I: processing $data->{NPA} $data->{NXX} $data->{X}\n";
	my $sw = $data->{CLLI};
	next unless length($sw) == 11;
	next if exists $skipcllis{$sw};
	my $wc = substr($sw,0,8);
	my $pl = substr($sw,0,6);
	my $cs = substr($sw,4,2);
	my $ct = substr($sw,0,4);
	my $rg = $cllirg{$cs} if exists $cllirg{$cs};
	my $cc = $cllicc{$cs} if exists $cllicc{$cs};
	my $st = $cllist{$cs} if exists $cllist{$cs};
	$data->{NAME} = $data->{LOC};
	$data->{SWCLLI} = $sw;
	$data->{WCCLLI} = $wc;
	$data->{PLCLLI} = $pl;
	$data->{CTCLLI} = $ct;
	$data->{WCVH} = getvh($data);
	$data->{WCLL} = getll($data);
	$data->{CC} = $cc;
	$data->{ST} = $st;
	$data->{RG} = $rg;
	my $nm = $data->{LOC};
	if ($cc and $st and $sw) {
		my $rec;
		unless ($rec or exists $data->{WCGEOID}) {
			if ($wc and exists $wcs{$cc}{$st}{$wc}) {
				$rec = $wcs{$cc}{$st}{$wc};
				foreach my $k (qw/WCGEOID WCGN WCGEOVH WCGEOLL WCCODE WCNAME/) {
					$data->{$k} = $rec->{$k} if $rec->{$k} or length($rec->{$k});
				}
			}
		}
		unless ($rec or exists $data->{WCGEOID}) {
			if ($nm and not cmpcllis($ct,$nm)) {
				print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc Location '$nm' is not consistent with CLLI '$pl'\n";
				$nm = undef;
			}
			if ($nm) {
				foreach my $vh (split(/;/,$data->{WCVH})) {
					my ($v,$h) = split(/,/,$vh);
					if (my $geo = lookupgeo($cc,$st,$nm,$v,$h)) { $alook++;
						geofound($cc,$st,$ct,$nm,$data,$geo); last;
					} else {
						$nolook++;
					}
				}
			}
		}
		foreach my $vh (split(/;/,$data->{WCVH})) {
			my ($v,$h) = split(/,/,$vh);
			if ($nm) {
				unless ($rec or exists $data->{WCGEOID}) {
					my ($vh,$ll) = ($data->{WCVH},$data->{WCLL});
					print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' targ: $ll ($vh $nm)\n";
				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestname($v,$h,$nm);
					if ($geo) { $aname++;
						if ($dist < $maxradius) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'name',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'name',$dist);
						}
					} else {
						$noname++;
					}
				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestcity($v,$h);
					if ($geo) { $acity++;
						if ($dist < $maxradius and (cmpnames($nm,$geo->{asciiname}) or cmpnames($nm,$geo->{name}))) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'city',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'city',$dist);
						}
					} else {
						$nocity++;
					}
				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestfeat($v,$h);
					if ($geo) { $afeat++;
						if ($dist < $maxradius and (cmpnames($nm,$geo->{asciiname}) or cmpnames($nm,$geo->{name}))) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'feat',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'feat',$dist);
						}
					} else {
						$nofeat++;
					}
				}
			} else {
				unless ($rec or exists $data->{WCGEOID}) {
					my ($vh,$ll) = ($data->{WCVH},$data->{WCLL});
					print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' targ: $ll ($vh $ct)\n";
				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestclli($v,$h,$ct);
					if ($geo) { $aclli++;
						if ($dist < $maxradius) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'clli',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'clli',$dist);
						}
					} else {
						$noclli++;
					}
				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestcity($v,$h);
					if ($geo) { $acity++;
						if ($dist < $maxradius and (cmpcllis($ct,$geo->{asciiname}) or cmpcllis($ct,$geo->{name}))) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'city',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'city',$dist);
						}
					} else {
						$nocity++;
					}

				}
				unless ($rec or exists $data->{WCGEOID}) {
					my ($geo,$dist) = closestfeat($v,$h);
					if ($geo) { $afeat++;
						if ($dist < $maxradius and (cmpcllis($ct,$geo->{asciiname}) or cmpcllis($ct,$geo->{name}))) {
							georecover($cc,$st,$ct,$nm,$data,$geo,'feat',$dist); last;
						} else {
							geoshowfail($cc,$st,$ct,$nm,$data,$geo,'feat',$dist);
						}
					} else {
						$nofeat++;
					}
				}
			}
		}
		if ($nm) {
			unless ($rec or exists $data->{WCGEOID}) {
				if (my $geo = bestname($cc,$st,$nm)) { $aname++;
					georecover($cc,$st,$ct,$nm,$data,$geo,'best');
				} else {
					$noname++;
				}
			}
		} else {
			unless ($rec or exists $data->{WCGEOID}) {
				if (my $geo = bestclli($cc,$st,$ct)) { $aclli++;
					georecover($cc,$st,$ct,$nm,$data,$geo,'best');
				} else {
					$noclli++;
				}
			}
		}
		unless ($rec or exists $data->{WCGEOID}) {
			print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st-$ct '$nm' cannot find geoname\n";
		}
	}
	{
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}} = {} unless exists
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		my $rec = $dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		foreach my $k (@dbsw_keys) {
			if ($k eq 'SWCLLI') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} else {
				if ($data->{$k}) {
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
			if ($k =~ /^(NPA|LATA|OCN|SECT|WCLL|NAME)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'WCVH') {
				foreach my $vh (split(/;/,$data->{$k})) {
					if (exists $rec->{$k}) {
						if (ref $rec->{$k} eq 'HASH') {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$vh' ($data->{SECT})\n"
							unless exists $rec->{$k}{$vh};
							$rec->{$k}{$vh}++;
						} elsif ($rec->{$k} ne $vh) {
							my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$rec->{$k}' ($sects)\n";
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$vh' ($data->{SECT})\n";
							$rec->{$k} = { $rec->{$k}=>1, $vh=>1 };
						}
					} else {
						$rec->{$k} = $vh;
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
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
			if ($k =~ /^(NPA|LATA|OCN|SECT|SWCLLI|WCLL)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'WCVH') {
				foreach my $vh (split(/;/,$data->{$k})) {
					if (exists $rec->{$k}) {
						if (ref $rec->{$k} eq 'HASH') {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$vh' ($data->{SECT})\n"
							unless exists $rec->{$k}{$vh};
							$rec->{$k}{$vh}++;
						} elsif ($rec->{$k} ne $vh) {
							my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$rec->{$k}' ($sects)\n";
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$vh' ($data->{SECT})\n";
							$rec->{$k} = { $rec->{$k}=>1, $vh=>1 };
						}
					} else {
						$rec->{$k} = $vh;
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
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
			if ($k =~ /^(NPA|LATA|OCN|WCCLLI)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
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
printf STDERR "I: %8d matches found\n", $found;
printf STDERR "I: %8d matches unusable\n", $unusable;
printf STDERR "I: %8d matches failed\n", $failed;
printf STDERR "I: %8d matches recovered\n", $recovered;
printf STDERR "I: %8d matches too far\n", $toofar;
printf STDERR "I: --------------------------\n";
printf STDERR "I: %8d/%8d found/unfound look\n", $alook,$nolook;
printf STDERR "I: %8d/%8d found/unfound name\n", $aname,$noname;
printf STDERR "I: %8d/%8d found/unfound clli\n", $aclli,$noclli;
printf STDERR "I: %8d/%8d found/unfound city\n", $acity,$nocity;
printf STDERR "I: %8d/%8d found/unfound feat\n", $afeat,$nofeat;
printf STDERR "I: --------------------------\n";

$fn = "$datadir/db.sw.csv";
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

$fn = "$datadir/sw.csv";
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

$fn = "$datadir/wc.csv";
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

$fn = "$datadir/pl.csv";
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

exit;

1;

__END__

     1	"NPA"
     2	"NXX"
     3	"X"
     4	"rng"
     5	"lines"
     6	"total"
     7	"loc"
     8	"state"
     9	"clli"
    10	"wcvh"
    11	"ocn"
    12	"lata"
    13	"feat"
    14	"sect"
