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

my @dirs = (
	'areacodes',
	'localcallingguide',
	'nanpa',
	'neca4',
	'npanxxsource',
	'pinglo',
	'telcodata',
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

my %rcs = ();
my %rns = ();

my %fields = ();
my %fieldnos = ();

my %counts = ();

foreach my $type (qw/rc rn/) {
	foreach my $dir (@dirs) {
		$fn = "$codedir/$dir/$type.nogeo.csv";
		print STDERR "I: reading $fn\n";
		open($fh,"<:utf8",$fn) or die "can't read $fn";
		my $heading = 1;
		my $oldrg = '';
		while (<$fh>) { chomp;
			s/^"//; s/"$//; my @tokens = split(/","/,$_);
			if ($heading) {
				$fields{$dir} = \@tokens;
				for (my $i=0;$i<@tokens;$i++) {
					$fieldnos{$dir}{$tokens[$i]} = $i;
				}
				$heading = undef;
				next;
			}
			my $rc = $tokens[$fieldnos{$dir}{RCSHORT}];
			my $rg = $tokens[$fieldnos{$dir}{REGION}];
			my $rn = $tokens[$fieldnos{$dir}{RCNAME}];
			my $cc = $tokens[$fieldnos{$dir}{RCCC}];
			my $st = $tokens[$fieldnos{$dir}{RCST}];
			if ($rc and $rg) {
				$counts{$dir}{RC}++;
				push @{$rcs{$dir}{$rg}{$rc}}, \@tokens;
				$rcs{cnts}{$rg}{$rc}{$dir}++;
			}
			if ($rn and $cc and $st) {
				$counts{$dir}{RN}++;
				my $RN = normalize($rn);
				push @{$rns{$dir}{$cc}{$st}{$RN}}, \@tokens;
				$rns{cnts}{$cc}{$st}{$RN}{$dir}++;
			}
			if ($rg) {
				if ($oldrg ne $rg) {
					$oldrg = $rg;
					print STDERR "I: reading REGION $rg for $dir\r";
				}
			}
		}
		print STDERR "\n";
		close($fh);
	}
}
foreach my $dir (@dirs) {
	printf STDERR "I: there are %10d RC records for $dir\n", $counts{$dir}{RC} if exists $counts{$dir}{RC};
	printf STDERR "I: there are %10d RN records for $dir\n", $counts{$dir}{RN} if exists $counts{$dir}{RN};
}

print STDERR "I: counting RC records...\n";
foreach my $rg (keys %{$rcs{cnts}}) {
	foreach my $rc (keys %{$rcs{cnts}{$rg}}) {
		my @ds = (keys %{$rcs{cnts}{$rg}{$rc}});
		if (scalar @ds < 2) {
			$rcs{only}{$rg}{$rc} = $ds[0];
		}
	}
}

print STDERR "I: counting RN records...\n";
foreach my $cc (keys %{$rns{cnts}}) {
	foreach my $st (keys %{$rns{cnts}{$cc}}) {
		foreach my $rn (keys %{$rns{cnts}{$cc}{$st}}) {
			my @ds = (keys %{$rns{cnts}{$cc}{$st}{$rn}});
			if (scalar @ds < 2) {
				$rns{only}{$cc}{$st}{$rn} = $ds[0];
			}
		}
	}
}

{
	my %isolated = ();
	print STDERR "I: checking isolated RC records...\n";
	foreach my $rg (sort keys %{$rcs{only}}) {
		foreach my $rc (sort keys %{$rcs{only}{$rg}}) {
			my $dir = $rcs{only}{$rg}{$rc};
			printf STDERR "E: %-2.2s-%-10.10s: ", $rg, $rc;
			print STDERR "only has RC records for $dir\n";
			$isolated{$dir}++;
			printf STDERR "E: %-2.2s-%-10.10s: ", $rg, $rc;
			print STDERR '"',join('","',@{$fields{$dir}}),'"',"\n";
			foreach my $rec (@{$rcs{$dir}{$rg}{$rc}}) {
				printf STDERR "E: %-2.2s-%-10.10s: ", $rg, $rc;
				print STDERR '"',join('","',@{$rec}),'"',"\n";
			}
		}
	}
	print STDERR "I: ---------------------\n";
	foreach my $dir (@dirs) {
		my $cnt = 0;
		$cnt = $isolated{$dir} if exists $isolated{$dir};
		printf STDERR "I: there are %10d isolated RC records for $dir\n", $cnt;
	}
	print STDERR "I: ---------------------\n";
}

{
	my %isolated = ();
	print STDERR "I: checking isolated RN records...\n";
	foreach my $cc (sort keys %{$rns{only}}) {
		foreach my $st (sort keys %{$rns{only}{$cc}}) {
			foreach my $rn (sort keys %{$rns{only}{$cc}{$st}}) {
				my $dir = $rns{only}{$cc}{$st}{$rn};
				printf STDERR "E: %-2.2s-%-2.2s-%-24.24s: ", $cc, $st, $rn;
				print STDERR "only has RN records for $dir\n";
				$isolated{$dir}++;
				printf STDERR "E: %-2.2s-%-2.2s-%-24.24s: ", $cc, $st, $rn;
				print STDERR '"',join('","',@{$fields{$dir}}),'"',"\n";
				foreach my $rec (@{$rns{$dir}{$cc}{$st}{$rn}}) {
					print STDERR '"',join('","',@{$rec}),'"',"\n";
				}
			}
		}
	}
	print STDERR "I: ---------------------\n";
	foreach my $dir (@dirs) {
		my $cnt = 0;
		$cnt = $isolated{$dir} if exists $isolated{$dir};
		printf STDERR "I: there are %10d isolated RN records for $dir\n", $cnt;
	}
	print STDERR "I: ---------------------\n";
}

exit;

1;

__END__


