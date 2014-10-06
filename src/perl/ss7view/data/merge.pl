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
use Time::gmtime;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

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

my @dirs = (
	'atandt',
	'moredata',
	'areacodes',
	'neca4',
	'pinglo',
	'telcodata',
	'localcallingguide',
	'npanxxsource',
);

my %db = ();
my %fields = ();
my %keys = (
	sw=>[qw/SWCLLI NPA NXX X LATA OOCN OCN SWNAME SWTYPE SWABBV SWDESC SWFUNC WCVH WCLL RCVH RCLL TGCLLI CLS45SW SPC APC FEAT STP1 STP2 ACTUAL AGENT HOST MATE TDM TDMILT TDMFGC TDMFGD TDMLCL TDMFGB TDMOPS TDM911 REMOTES SECT FDATE/],
	wc=>[qw/WCCLLI NPA NXX X LATA OOCN OCN WCNAME SWCLLI WCVH WCLL RCVH RCLL WCADDR WCZIP WCCITY WCCOUNTY WCST WCCC SECT FDATE/],
	pl=>[qw/PLCLLI NPA NXX X LATA OOCN OCN WCCLLI WCCITY WCCOUNTY WCST WCCC SECT FDATE/],
	pc=>[qw/SPC APC SWCLLI HOST CLS45SW STP1 STP2 MATE SECT FDATE/],
);

sub assign {
	my ($dir,$fld,$rec,$val) = @_;
	$rec->{$fld} = $val;
	$rec->{source}{$fld} = [] unless exists $rec->{source}{$fld};
	push @{$rec->{source}{$fld}}, $dir;
}

my %merging = (
	sw=>{
		areacodes=>{
			SWTYPE=>sub{
				my ($type,$dir,$fld,$rec,$val) = @_;
				assign($dir,'SWABBV',$rec,$val) if length($val);
			},
		},
		localcallingguide=>{
			SWTYPE=>sub{
				my ($type,$dir,$fld,$rec,$val) = @_;
				assign($dir,'SWABBV',$rec,$val) if length($val);
			},
		},
		neca4=>{
		},
		npanxxsource=>{
		},
		pinglo=>{
		},
		telcodata=>{
			SWTYPE=>sub{
				my ($type,$dir,$fld,$rec,$val) = @_;
				assign($dir,'SWDESC',$rec,$val) if length($val);
			},
			SWNAME=>sub{
			},
		},
		atandt=>{
			SWTYPE=>sub{
				my ($type,$dir,$fld,$rec,$val) = @_;
				assign($dir,'SWABBV',$rec,$val) if length($val);
			},
		},
	},
	wc=>{
		areacodes=>{
		},
		localcallingguide=>{
		},
		neca4=>{
		},
		npanxxsource=>{
		},
		pinglo=>{
		},
		telcodata=>{
		},
		atandt=>{
		},
	},
	pl=>{
		areacodes=>{
		},
		localcallingguide=>{
		},
		neca4=>{
		},
		npanxxsource=>{
		},
		pinglo=>{
		},
		telcodata=>{
		},
		atandt=>{
		},
	},
);

foreach my $type (qw/pc pl wc sw/) {
	$db{$type} = {};
	foreach my $dir (@dirs) {
		$fn = "$dir/$type.nogeo.csv";
		if (-f $fn) {
			print STDERR "I: reading $fn\n";
			open($fh,"<:utf8",$fn) or die "can't open $fn";
			my $heading = 1;
			my $lineno = 0;
			while (<$fh>) { chomp; $lineno++;
				s/^"//; s/"$//; my @tokens = split(/","/,$_);
				if ($heading) {
					$fields{$type} = \@tokens;
					$heading = undef;
					#print STDERR "I: setting headings for \U$type\E to ",join(',',@{$fields{$type}}),"\n";
					next;
				}
				my $key = $tokens[0];
				unless (length($key)) {
					print STDERR "E: bad key '$key' at line $lineno\n";
					next;
				}
				$db{$type}{$key} = {} unless exists $db{$type}{$key};
				my $rec = $db{$type}{$key};
				#print STDERR "I: processing $dir: $key\n";
				for (my $i=0;$i<@{$fields{$type}};$i++) {
					my $fld = $fields{$type}[$i];
					#print STDERR "I: process $dir: $key: $fld\n";
					my $val = $tokens[$i];
					if (exists $merging{$type}{$dir}{$fld}) {
						&{$merging{$type}{$dir}{$fld}}($type,$dir,$fld,$rec,$val);
					} else {
						if ($fld =~ /^(NPA|NXX|X|LATA|OCN|SWFUNC)$/ or
							($fld eq 'SWCLLI' and $type ne 'sw') or
							($fld eq 'WCCLLI' and $type ne 'wc') or
							($fld eq 'PLCLLI' and $type ne 'pl')
						) {
							foreach my $m (split(/,/,$val)) { $rec->{$fld}{$m}++ }
						} elsif ($fld eq 'SECT') {
							foreach my $m (split(/,/,$val)) { $rec->{$fld}{substr($dir,0,4)."($m)"}++ }
						} elsif ($fld =~ /^(WCVH|WCLL)$/) {
							foreach my $m (split(/;/,$val)) { $rec->{$fld}{$m}++ }
						} elsif ($fld eq 'FEAT') {
							foreach my $m (split(/\s/,$val)) { $rec->{$fld}{$m}++ }
						} elsif ($fld eq 'FDATE') {
							assign($dir,$fld,$rec,$val) if $rec->{$fld} < $val;
						} elsif ($fld =~ /^(RCVH|RCLL)$/) {
						} elsif ($fld eq 'LATA') {
							if (length($val)) {
								if (length($rec->{$fld}) and $rec->{$fld} ne $val) {
									if (index($rec->{$fld},$val,0) == 0 or index($val,$rec->{$fld} == 0)) {
										if (length($val) == 5) {
											assign($dir,$fld,$rec,$val);
										}
									} else {
										print STDERR "E: \U$type\E: $key: $fld changing from $rec->{$fld} by $rec->{source}{$fld}[-1] to $val by $dir\n";
										assign($dir,$fld,$rec,$val);
									}
								} else {
									assign($dir,$fld,$rec,$val);
								}
							}
						} elsif ($fld =~ /^(SWNAME|WCCITY|WCCOUNTY)$/) {
							if (length($val)) {
								if (length($rec->{$fld}) and $rec->{$fld} ne $val) {
									if (cmpnames($rec->{$fld},$val)) {
										if (length($val) >= length($rec->{$fld})) {
											assign($dir,$fld,$rec,$val);
										}
									} else {
										print STDERR "E: \U$type\E: $key: $fld changing from $rec->{$fld} by $rec->{source}{$fld}[-1] to $val by $dir\n";
										assign($dir,$fld,$rec,$val);
									}
								} else {
									assign($dir,$fld,$rec,$val);
								}
							}
						} else {
							if (length($val)) {
								if (length($rec->{$fld}) and $rec->{$fld} ne $val) {
									if ($fld eq 'STP1') {
										unless ($rec->{STP2} eq $val) {
											print STDERR "E: \U$type\E: $key: $fld changing from $rec->{$fld} by $rec->{source}{$fld}[-1] to $val by $dir\n";
											assign($dir,$fld,$rec,$val);
										}
									} elsif ($fld eq 'STP2') {
										unless ($rec->{STP1} eq $val) {
											print STDERR "E: \U$type\E: $key: $fld changing from $rec->{$fld} by $rec->{source}{$fld}[-1] to $val by $dir\n";
											assign($dir,$fld,$rec,$val);
										}
									} else {
										print STDERR "E: \U$type\E: $key: $fld changing from $rec->{$fld} by $rec->{source}{$fld}[-1] to $val by $dir\n";
										assign($dir,$fld,$rec,$val);
									}
								} else {
									assign($dir,$fld,$rec,$val);
								}
							}
						}
					}
				}
			}
			close($fh);
		}
	}
}

foreach my $type (qw/sw wc pl pc/) {
	$fn = "$type.nogeo.csv";
	print STDERR "I: writing $fn\n";
	open($of,">:utf8",$fn) or die "can't write $fn";
	print $of '"',join('","',@{$keys{$type}}),'"',"\n";
	foreach my $key (sort keys %{$db{$type}}) {
		my $rec = $db{$type}{$key};
		my @values = ();
		foreach (@{$keys{$type}}) {
			if (exists $rec->{$_}) {
				if (ref $rec->{$_} eq 'HASH') {
					push @values, join(',',sort keys %{$rec->{$_}});
				} elsif (ref $rec->{$_} eq 'ARRAY') {
					push @values, join(',',@{$rec->{$_}});
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

exit;

1;

__END__
