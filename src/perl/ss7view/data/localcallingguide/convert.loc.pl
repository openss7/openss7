#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";
my $geondir = "$progdir/../geonames";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %lergst = ();
my %lergco = ();
my %lergcn = ();
my %countries = ();

{
$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	next if /^$/;
	next if /^#/;
	my @tokens = split(/\t/,$_);
	next unless scalar @tokens == 5;
	if ($heading and /^ISO2/) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{$fields[$i]} = $tokens[$i] if $tokens[$i];
	}
	if (my $k = $data->{CLLI}) {
		$lergst{$k} = $data;
	}
	if (my $k = $data->{ISO2}) {
		$countries{$k}++;
	}
	if (my $k = $data->{'ISO2/3'}) {
		$lergco{$k} = $data->{ISO2};
	}
	if (my $k = $data->{LERG}) {
		$lergcn{$k} = $data->{ISO2};
	}
}
close($fh);
}

sub getstate {
	my $clli = shift;
	my $st = substr($clli,4,2);
	my $cc;
	if (exists $lergst{$st}) {
		$cc = $lergst{$st}{'ISO2'};
		if ($cc eq 'US') {
			$st = $lergst{$st}{'ISO2/3'};
		} else {
			$st = $cc;
		}
	}
	return ($cc,$st);
}

sub normalize {
	my $nm = shift;
	$nm =~ s/-/ /g;
	$nm =~ s/\.//g;
	$nm =~ s/'//g;
	$nm =~ s/^\s+$//;
	$nm =~ s/ \([^\)]*\)//g;
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
	my $dist = distance($n1,$n2);
	return 1 if $dist < 4 or $dist < length($n2)/3;
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
					} elsif (substr($s1,$i1+2,1) eq substr($s2,$i2,1)) {
						$good++; $i1+=3; $i2++;
					} else {
						$i1++; $i2++;
					}
				}
				if ($good/length($s1) > 0.7 and $good/length($s2) > 0.2) {
					$mtc++;
					last;
				}
			}
		}
	}
	return 1 if $mtc > $cnt/2;
	return 0;
}

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

my %cities = ();

{
	$fn = "$geondir/cities1000.zip";
	print STDERR "I: processing $fn\n";
	open($fh,"unzip -p $fn cities1000.txt |") or die "can't process $fn";
	while (<$fh>) { chomp;
		my @tokens = split(/\t/,$_);
		my $data = {};
		for (my $i=0;$i<@gn_fields;$i++) {
			$data->{$gn_fields[$i]} = $tokens[$i] if $tokens[$i] or length($tokens[$i]);
		}
		my $cc = $data->{'country code'};
		next unless exists $countries{$cc};
		my $la = $data->{latitude};
		my $lo = $data->{longitude};
		for (my $lev=-2;$lev<4;$lev++) {
			my $laf = POSIX::floor($la*(10**$lev))/(10**$lev);
			my $lof = POSIX::floor($lo*(10**$lev))/(10**$lev);
			$cities{$lev}{"$laf,$lof"} = [] unless exists $cities{$lev}{"$laf,$lof"};
			push @{$cities{$lev}{"$laf,$lof"}}, $data;
		}
	}
	close($fh);
}

sub closestcity {
	my ($la,$lo,$cc,$st) = @_;
	my $geo;
	for (my $lev=3;$lev>-3;$lev--) {
		my $laf = POSIX::floor($la*(10**$lev))/(10**$lev);
		my $lof = POSIX::floor($lo*(10**$lev))/(10**$lev);
		next unless exists $cities{$lev}{"$laf,$lof"};
		my $delta;
		foreach (@{$cities{$lev}{"$laf,$lof"}}) {
			next unless $_->{'country code'} eq $cc;
			if ($cc eq 'CA') {
				next unless $a1codes{$_->{'admin1 code'}} eq $st;
			} elsif ($cc eq 'US') {
				next unless $_->{'admin1 code'} eq $st;
			}
			my $lati = $_->{latitude};
			my $long = $_->{longitude};
			my $d = sqrt((($lati-$la)**2)+(($long-$lo)**2));
			if (defined $delta) {
				if ($d < $delta) {
					$delta = $d;
					$geo = $_;
				}
			} else {
				$delta = $d;
				$geo = $_;
			}
		}
		last if defined $geo;
	}
	return $geo;
}

my %geonames = ();
my %features = ();




foreach my $code (qw/AG AI AS BB BM BS CA DM DO GD GU JM KN KY LC MP MS MX PR SX TC TT US VC VG VI/) {
	my ($good,$bad) = (0,0);
	$fn = "$geondir/$code.zip";
	print STDERR "I: processing $fn\n";
	open($fh,"unzip -p $fn $code.txt |") or die "can't process $fn";
	while (<$fh>) { chomp; s/\r//g;
		my @tokens = split(/\t/,$_);
		my $data = {};
		for (my $i=0;$i<@gn_fields;$i++) {
			$data->{$gn_fields[$i]} = $tokens[$i] if $tokens[$i] or length($tokens[$i]);
		}
		my $fs = $data->{'feature class'};
		next unless $fs =~ /^(P|L|S|T|A)$/;
		my $fc = $data->{'feature code'};
		next unless $fc =~ /^(PPL|AREA|RESV|ISL|MILB|INSM|AIR|ADM)/;
		my $on = $data->{asciiname};
		my $nm = normalize($on);
		my $cc = $data->{'country code'};
		my $st = $cc; $st = $data->{'admin1 code'} if $cc eq 'US' or $cc eq 'CA';
		$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
		my $dt = $data->{'modification date'};
		if (exists $geonames{$cc}{$st}{$nm}) {
			my $rec = $geonames{$cc}{$st}{$nm};
			if (ref $rec eq 'ARRAY') {
#				if ($rec->[0]{'feature class'} ne 'P' and $fs eq 'P') {
#					$geonames{$cc}{$st}{$nm} = $data; $bad--; $good++;
#				} else {
					push @{$rec}, $data;
#					#printf STDERR "W: %d conflicting records for $cc-$st-$on\n", scalar(@{$rec});
#				}
			} else {
#				if ($rec->{'feature class'} ne $fs) {
#					if ($rec->{'feature class'} ne 'P' and $fs eq 'P') {
#						$geonames{$cc}{$st}{$nm} = $data;
#					}
#				} elsif ($rec->{'modification date'} eq $dt) {
#					unless ($rec->{latitude} == $data->{latitude} and $rec->{longitude} == $data->{longitude}) {
						$geonames{$cc}{$st}{$nm} = [ $rec, $data ];
						$good--; $bad++;
#						#print STDERR "W: 2 conflicting records for $cc-$st-$on\n";
#					}
#				} elsif ($rec->{'modification date'} lt $dt) {
#					$geonames{$cc}{$st}{$nm} = $data;
#				}
			}
		} else {
			$geonames{$cc}{$st}{$nm} = $data; $good++;
		}
		my $la = $data->{latitude};
		my $lo = $data->{longitude};
		for (my $lev=-2;$lev<4;$lev++) {
			my $laf = POSIX::floor($la*(10**$lev))/(10**$lev);
			my $lof = POSIX::floor($lo*(10**$lev))/(10**$lev);
			$features{$lev}{"$laf,$lof"} = [] unless exists $features{$lev}{"$laf,$lof"};
			push @{$features{$lev}{"$laf,$lof"}}, $data;
		}
	}
	close($fh);
	print STDERR "I: $code: $good good records; $bad bad records\n";
}

sub closestfeat {
	my ($la,$lo,$cc,$st) = @_;
	my $geo;
	for (my $lev=3;$lev>-3;$lev--) {
		my $laf = POSIX::floor($la*(10**$lev))/(10**$lev);
		my $lof = POSIX::floor($lo*(10**$lev))/(10**$lev);
		next unless exists $features{$lev}{"$laf,$lof"};
		my $delta;
		foreach (@{$features{$lev}{"$laf,$lof"}}) {
			next unless $_->{'country code'} eq $cc;
			if ($cc eq 'CA') {
				next unless $a1codes{$_->{'admin1 code'}} eq $st;
			} elsif ($cc eq 'US') {
				next unless $_->{'admin1 code'} eq $st;
			}
			my $lati = $_->{latitude};
			my $long = $_->{longitude};
			my $d = sqrt((($lati-$la)**2)+(($long-$lo)**2));
			if (defined $delta) {
				if ($d < $delta) {
					$delta = $d;
					$geo = $_;
				}
			} else {
				$delta = $d;
				$geo = $_;
			}
		}
		last if defined $geo;
	}
	return $geo;
}

sub closestname {
	my ($la,$lo,$cc,$st,$nm) = @_;
	my $geo;
	for (my $lev=3;$lev>0;$lev--) {
		foreach my $db (\%cities,\%features) {
			my $laf = POSIX::floor($la*(10**$lev))/(10**$lev);
			my $lof = POSIX::floor($lo*(10**$lev))/(10**$lev);
			next unless exists $db->{$lev}{"$laf,$lof"};
			my $delta;
			foreach (@{$db->{$lev}{"$laf,$lof"}}) {
				next unless $_->{'country code'} eq $cc;
				if ($cc eq 'CA') {
					next unless $a1codes{$_->{'admin1 code'}} eq $st;
				} elsif ($cc eq 'US') {
					next unless $_->{'admin1 code'} eq $st;
				}
				next unless cmpnames($nm,$_->{asciiname}) or cmpnames($nm,$_->{name});
				my $lati = $_->{latitude};
				my $long = $_->{longitude};
				my $d = sqrt((($lati-$la)**2)+(($long-$lo)**2));
				if (defined $delta) {
					if ($d < $delta) {
						$delta = $d;
						$geo = $_;
					}
				} else {
					$delta = $d;
					$geo = $_;
				}
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return $geo;
}

my ($found,$failed,$unusable,$recovered);

sub lookupgeo {
	my ($cc,$st,$loc,$la,$lo) = @_;
	my $ln = normalize($loc);
	my $geo;
	if (exists $geonames{$cc}{$st}{$ln}) {
		if (ref $geonames{$cc}{$st}{$ln} ne 'ARRAY') {
			$geo = $geonames{$cc}{$st}{$ln};
			$found++;
		} else {
			if ($la and $lo) {
				my $delta;
				foreach (@{$geonames{$cc}{$st}{$ln}}) {
					my $lati = $_->{latitude};
					my $long = $_->{longitude};
					my $d = sqrt((($lati-$la)**2)+(($long-$lo)**2));
					if (defined $delta) {
						if ($d < $delta) {
							$delta = $d;
							$geo = $_;
						}
					} else {
						$delta = $d;
						$geo = $_;
					}
				}
			}
			if (defined $geo) {
				$found++;
			} else {
				print STDERR "W: cannot use geoname for $cc-$st-$loc\n";
				$unusable++;
			}
		}
	} else {
		#print STDERR "W: cannot find geoname for $cc-$st-$loc\n";
		$failed++;
	}
	return $geo;
}

my %locs = ();
my %lcas = ();

{
$fn = "$datadir/rc.csv";
print STDERR "I: processing $fn\n";
open($fh,"<",$fn) or die "can't process $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	next unless /^"/;
	last if /^$/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{$fields[$i]} = $tokens[$i] if $tokens[$i];
	}
	my $loc = $data->{RC};
	my $rg = $data->{REGION};
	$rg = 'SX' if $rg eq 'SM';
	my $cc = $lergco{$rg} if exists $lergco{$rg};
	$cc = $lergcn{$rg} unless $cc;
	my $st = $rg;
	if ($loc and $cc and $st) {
		my ($rec,$la,$lo,$vh);
		my ($v,$h) = ($data->{'RC-V'},$data->{'RC-H'}); ($v,$h) = (int($v),int($h));
		if ($v and $h) {
			($la,$lo) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$lo = -$lo; $lo += 360 if $lo < -180; $lo -= 360 if $lo > 180;
			$vh = sprintf('%05d,%05d',$v,$h);
		} else {
			($la,$lo) = ($data->{'RC-LAT'},$data->{'RC-LON'});
		}
		if (exists $locs{$cc}{$st}{$loc}) {
			$rec = $locs{$cc}{$st}{$loc};
		} else {
			$rec = $locs{$cc}{$st}{$loc} = { LOCCC=>$cc, LOCST=>$st, LOCNAME=>$loc };
			if (my $geo = lookupgeo($cc,$st,$loc,$la,$lo)) {
				$rec->{LOCGEOID} = $geo->{geonameid};
				$rec->{LOCGN} = $geo->{name};
				$rec->{LOCLL} = "$geo->{latitude},$geo->{longitude}";
				$rec->{LOCCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
			}
		}
		if ($la and $lo) {
			my $nm = normalize($loc);
			if ($vh) {
				$rec->{RCVH}{$vh}++;
				if (exists $lcas{$vh}) {
					if (ref $lcas{$vh} eq 'ARRAY') {
						push @{$lcas{$vh}}, $rec;
					} else {
						$lcas{$vh} = [ $lcas{$vh}, $rec ];
					}
				} else {
					$lcas{$vh} = $rec;
				}
			}
			$rec->{RCLL} = "$la,$lo";
			unless (exists $rec->{NMGEOID}) {
				if (my $geo = closestname($la,$lo,$cc,$st,$nm)) {
					$rec->{NMGEOID} = $geo->{geonameid};
					$rec->{NMLL} = "$geo->{latitude},$geo->{longitude}";
					$rec->{NMNAME} = $geo->{asciiname};
					$rec->{NMCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
					unless ($rec->{LOCGEOID}) {
						$rec->{LOCGEOID} = $rec->{NMGEOID};
						$rec->{LOCGN} = $geo->{name};
						$rec->{LOCCODE} = $rec->{NMCODE};
						$rec->{LOCLL} = $rec->{NMLL};
						$found++; $failed--; $recovered++;
						print STDERR "W: $cc-$st '$loc' recovered as $rec->{LOCCODE} '$geo->{name}' ($geo->{asciiname})\n";
					}
				}
			}
			unless (exists $rec->{CTGEOID}) {
				if (my $geo = closestcity($la,$lo,$cc,$st)) {
					$rec->{CTGEOID} = $geo->{geonameid};
					$rec->{CTLL} = "$geo->{latitude},$geo->{longitude}";
					$rec->{CTNAME} = $geo->{asciiname};
					$rec->{CTCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
					unless ($rec->{LOCGEOID}) {
						if (cmpnames($nm,$geo->{name}) or
						    cmpnames($nm,$geo->{asciiname})) {
						    $rec->{LOCGEOID} = $rec->{CTGEOID};
						    $rec->{LOCGN} = $geo->{name};
						    $rec->{LOCCODE} = $rec->{CTCODE};
						    $rec->{LOCLL} = $rec->{CTLL};
						    $found++; $failed--; $recovered++;
						    print STDERR "W: $cc-$st '$loc' recovered as $rec->{LOCCODE} '$geo->{name}' ($geo->{asciiname})\n";
						}
					}
				}
			}
			unless (exists $rec->{FTGEOID}) {
				if (my $geo = closestfeat($la,$lo,$cc,$st)) {
					$rec->{FTGEOID} = $geo->{geonameid};
					$rec->{FTLL} = "$geo->{latitude},$geo->{longitude}";
					$rec->{FTNAME} = $geo->{asciiname};
					$rec->{FTCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
					unless ($rec->{LOCGEOID}) {
						if (cmpnames($nm,$geo->{name}) or
						    cmpnames($nm,$geo->{asciiname})) {
						    $rec->{LOCGEOID} = $rec->{FTGEOID};
						    $rec->{LOGGN} = $geo->{name};
						    $rec->{LOCCODE} = $rec->{FTCODE};
						    $rec->{LOCLL} = $rec->{FTLL};
						    $found++; $failed--; $recovered++;
						    print STDERR "W: $cc-$st '$loc' recovered as $rec->{LOCCODE} '$geo->{name}' ($geo->{asciiname})\n";
						}
					}
				}
			}
			unless (exists $rec->{LOCGEOID}) {
				print STDERR "W: cannot find geoname for $cc-$st-$loc\n";
			}
		}
		$rec->{LOCNAME} = $data->{RC};
		$rec->{NPA} = $data->{NPA};
		$rec->{NXX} = $data->{NXX};
		$rec->{OCN} = $data->{OCN};
		$rec->{LATA} = $data->{LATA};
		$rec->{RCSHORT} = $data->{RCSHORT};
	} else {
		print STDERR "E: skipping loc: '$loc', cc: '$cc', st: '$st'\n";
	}
}
close($fh);
}

printf STDERR "I: %-8.8d matches found\n", $found;
printf STDERR "I: %-8.8d matches unusable\n", $unusable;
printf STDERR "I: %-8.8d matches failed\n", $failed;
printf STDERR "I: %-8.8d matches recovered\n", $recovered;

my @keys = (
	'LOCCC',
	'LOCST',
	'LOCNAME',
	'LOCGN',
	'LOCGEOID',
	'LOCCODE',
	'LOCLL',
	'NMNAME',
	'NMGEOID',
	'NMCODE',
	'NMLL',
	'CTNAME',
	'CTGEOID',
	'CTCODE',
	'CTLL',
	'FTNAME',
	'FTGEOID',
	'FTCODE',
	'FTLL',
	'RCVH',
	'RCLL',
	'NPA',
	'NXX',
	'OCN',
	'LATA',
	'RCSHORT',
);

$fn = "$datadir/loc.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@keys),'"',"\n";
foreach my $cc (sort keys %locs) {
	foreach my $st (sort keys %{$locs{$cc}}) {
		foreach my $loc (sort keys %{$locs{$cc}{$st}}) {
			my $rec = $locs{$cc}{$st}{$loc};
			my @values = ();
			foreach (@keys) {
				if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
					if ($_ eq 'RCVH') {
						push @values, join(';',sort keys %{$rec->{$_}});
					} else {
						push @values, join(',',sort keys %{$rec->{$_}});
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

foreach my $vh (sort keys %lcas) {
	my $lca = $lcas{$vh};
	if (ref $lca eq 'ARRAY') {
		print STDERR "W: $vh has multiple rate centers:\n";
		foreach my $rec (@{$lca}) {
			print STDERR "W: $rec->{LOCCC}-$rec->{LOCST}-$rec->{LOCNAME} [$rec->{LOCGEOID}] '$rec->{LOCGN}'\n";
		}
	}
}

exit;

1;

__END__
