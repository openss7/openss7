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

my %db = ();

my @dirs = (
	'npanxxsource',
	'nanpa',
	'telcodata',
	'localcallingguide',
	'pinglo',
	'neca4',
);

my %fields = ();

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.rc.nogeo.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<",$fn) or die "can't read $fn";
	my $heading = 1;
	my $npa = 100;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			$fields{$dir} = \@tokens;
			$heading = undef;
			next;
		}
		my %data = ();
		for (my $i=0;$i<@{$fields{$dir}};$i++) {
			$data{$fields{$dir}[$i]} = $tokens[$i] if $tokens[$i];
		}
		push @{$db{$dir}{$data{NPA}}{$data{NXX}}}, \%data;
		$db{mark}{$data{NPA}}{$data{NXX}}++;
		if ($npa != int($data{NPA}/100)*100) {
			$npa = int($data{NPA}/100)*100;
			print STDERR "I: reading NPA $npa for $dir\n";
		}
	}
	close($fh);
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

sub cmpnamesnew {
	my ($nm1,$nm2) = @_;
	my $n1 = normalize($nm1);
	my $n2 = normalize($nm2);
	($n1,$n2) = ($n2,$n1) if length($n2)<length($n1);
	return undef if $n1 eq $n2;
	return undef if distance($n1,$n2) < length($n2)/3;
	return 1;
}

sub cmpnames {
	my ($nm1,$nm2) = @_;
	my $n1 = normalize($nm1);
	my $n2 = normalize($nm2);
	return undef if $n1 eq $n2;
	my $dist = distance($n1,$n2);
	return undef if $dist < 4 or $dist < length($n2)/3;
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
	return undef if $mtc > $cnt/2;
	#print STDERR "I: no match: '$nm1' '$nm2'\n";
	return 1;
}

my %compares = (
	'npanxxsource'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCSHORT=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			my %skiplist = (
				'BILLING'=>1,
				'INFO PROVD'=>1,
				'RESERVED'=>1,
				'BROADBAND'=>1,
				'PLANT TEST'=>1,
				'HIGHVOLUME'=>1,
				'ROUTING'=>1,
			);
			return 1 if exists $skiplist{$tst};
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
	'nanpa'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCSHORT=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
	'telcodata'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCSHORT=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
	'localcallingguide'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCSHORT=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
	'pinglo'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCSHORT=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
	'neca4'=>{
		REGION=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCCC=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCST=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val ne $tst) { return undef } } else { $$val = $tst }
			return 1;
		},
		RCGN=>sub{
			my ($val,$tst,$npa,$nxx) = @_;
			my $result = 1;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { $result = undef } } else { $$val = "\U$tst\E" }
			return $result;
		},
		RCSHORT=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and $$val ne "\U$tst\E") { return undef } } else { $$val = "\U$tst\E" }
			return 1;
		},
		RCNAME=>sub{
			return 1;
			my ($val,$tst,$npa,$nxx) = @_;
			if (defined $$val) { if ($$val and $tst and cmpnames($$val,$tst)) { return undef } } else { $$val = normalize($tst) }
			return 1;
		},
	},
);

foreach my $field (qw/REGION RCCC RCST RCSHORT RCGN/) {
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
			my $all = 1;
			my $good = 1;
			my $val;
			my @vals;
			foreach my $dir (@dirs) {
				my $goodone = undef;
				my @items = ();
				foreach my $rec (@{$db{$dir}{$npa}{$nxx}}) {
					if (exists $compares{$dir}{$field}) {
						$goodone = 1 if &{$compares{$dir}{$field}}(\$val,$rec->{$field},$npa,$nxx);
					}
					push @items,$rec->{$field};
				}
				if (scalar @items) {
					$good = undef unless $goodone;
					push @vals,join(';',@items);
				} else {
					$all = undef;
					push @vals,undef;
				}
			}
			next if $good;
			print STDERR "E: $npa-$nxx $field mismatch: ",'"',join('","',@vals),'"',"\n";
		}
	}
}

print STDERR "I: key ",'"',join('","',@dirs),'"',"\n";

exit;

1;

__END__


