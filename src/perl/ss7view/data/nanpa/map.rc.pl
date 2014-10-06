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

my %nanpst = ();
$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my @tokens = split(/\t/,$_);
	my $cc = $tokens[1];
	my $st = $tokens[2];
	$nanpst{$st} = $cc if $cc and $st;
}
close($fh);

my %stcc = ();
my %lergcc = ();
my %cllicc = ();
my %cclerg = ();
$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^$/;
	next if /^ISO2/;
	my @tokens = split(/\t/,$_);
	my $rec = {};
	my $cc = $rec->{cc} = $tokens[0];
	my $st = $rec->{st} = $tokens[1];
	my $lg = $rec->{lerg} = $tokens[2];
	my $cl = $rec->{clli} = $tokens[3];
	$rec->{loc} = $tokens[4];
	$stcc{$st} = $rec;
	$lergcc{$lg} = $rec;
	$cllicc{$cl} = $rec;
	$cclerg{$cc} = $rec if $cc ne 'US' and $cc ne 'CA';
}
close($fh);

my %rcmap = ();
my %rnmap = ();
$fn = "$datadir/rc.csv";
my $heading = 1;
my @fields = ();
print STDERR "I: processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
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
	my $rc = $data->{RCSHORT};
	my $rg = $data->{REGION};
	my $cc = $nanpst{$rg} if exists $nanpst{$rg};
	$cc = $data->{RCCC} if $data->{RCCC};
	my $st = $rg unless $cc eq $rg;
	$st = $data->{RCST} if $data->{RCST};
	my $rn;
	if ($data->{RCNAME}) {
		$rn = $data->{RCNAME};
	} else {
		$rn = $rc;
	}
	if ($cc and exists $cclerg{$cc}) {
		$st = $cclerg{$cc}{st};
	}
	if (0) {
	# for the purpose of comparison
	$rn =~ s/-/ /g;
	$rn =~ s/'//g;
	$rn =~ s/\.//g;
	$rn =~ s/\bSAINT\b/ST/g;
	$rn =~ s/\bSAINTE\b/STE/g;
	$rn =~ s/\bSTE\b/ST/g;
	if ($cc eq 'CA') {
		if ($st eq 'AB') {
			$rn = 'MULHURST'			if $rn eq 'MULHURST BAY';
			$rn = 'SPRUCE VIEW'			if $rn eq 'SPRUCEVIEW';
			$rn = 'WABASCA'				if $rn eq 'WABASCA DESMARAIS';
			$rn = 'ZAMA CITY'			if $rn eq 'ZAMA';
		} elsif ($st eq 'BC') {
			$rn = '100 MILE HOUSE'			if $rn eq 'ONE HUNDRED MILE HOUSE';
			$rn = 'NEW AIYANSH'			if $rn eq 'AIYANSH';
			$rn = 'BELLA BELLA'			if $rn eq 'NEW BELLA BELLA';
			$rn = 'NEW HAZELTON'			if $rn eq 'HAZELTON';
			$rn = 'HARTLEY BAY'			if $rn eq 'KULKAYU (HARTLEY BAY)';
			$rn = 'LAKELSE LAKE'			if $rn eq 'LAKELSE';
			$rn = 'PUNTZI LAKE'			if $rn eq 'PUNTZI';
			$rn = 'QUEEN CHARLOTTE CITY'		if $rn eq 'QUEEN CHARLOTTE';
			$rn = 'VAN ANDA'			if $rn eq 'VANANDA';
		} elsif ($st eq 'MB') {
			$rn = 'DOUGLAS'				if $rn eq 'DOUGLAS STATION';
			$rn = 'MEDORA, NAPINKA'			if $rn eq 'MEDORA' or $rn eq 'MEDORA NAPINKA';
			$rn = 'ST PIERRE JOLYS'			if $rn eq 'ST PIERRE';
			$rn = 'WASAGAMACK'			if $rn eq 'WAASAGAMACH' or $rn eq 'WAASAGOMACH';
			$rn = 'DOUGLAS'				if $rn eq 'DOUGLAS STATION';
			$rn = 'BLOODVEIN'			if $rn eq 'BLOODVEIN RIVER';
		} elsif ($st eq 'NB') {
			$rn = 'GRAND BAY WESTFIELD'		if $rn eq 'GRAND BAY WESTFIELD CENTRE';
			$rn = 'RIVERSIDE ALBERT'		if $rn eq 'ALBERT';
			$rn = 'TRACADIE SHEILA'			if $rn eq 'TRACADIE';
		} elsif ($st eq 'NL') {
			$rn = 'DE GRAU'				if $rn eq 'DEGRAS';
			$rn = 'CHURCHILL FALLS'			if $rn eq 'CHURCHILL FALLS (LABRADOR)';
			$rn = 'CARTWRIGHT'			if $rn eq 'CARTWRIGHT (LABRADOR)';
			$rn = 'CHARLOTTETOWN (BONAVISTA BAY)'	if $rn eq 'CHARLOTTETOWN BONAVISTA BAY';
			$rn = 'FAIR HAVEN'			if $rn eq 'FAIRHAVEN';
			$rn = 'FORTEAU'				if $rn eq 'FORTEAU (LABRADOR)';
			$rn = 'FRENCHMANS ISLAND'		if $rn eq 'FRENCH ISLAND';
			$rn = 'FRESHWATER PLACENTIA BAY'	if $rn eq 'FRESHWATER';
			$rn = 'GRAND FALLS'			if $rn eq 'GRAND FALLS WINDSOR';
		}
	}
	}
	if ($rc and $cc) {
		$rcmap{$cc}{$st}{$rn} = {} unless exists $rcmap{$cc}{$st}{$rn};
		my $rec = $rcmap{$cc}{$st}{$rn};
		$rec->{RCSHORT} = $rc;
		$rec->{RCNAME} = $rn;
		$rec->{RCCC} = $cc;
		$rec->{RCST} = $st;
		$rec->{REGION} = $rg;
	}
	if ($rc and $rg) {
		$rnmap{$rg}{$rc} = {} unless exists $rnmap{$rg}{$rc};
		my $rec = $rnmap{$rg}{$rc};
		$rec->{RCSHORT} = $rc;
		$rec->{RCNAME} = $rn;
		$rec->{RCCC} = $cc;
		$rec->{RCST} = $st;
		$rec->{REGION} = $rg;
	}
}
close($fh);

$fn = "$datadir/rcmap.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/RCCC RCST RCNAME REGION RCSHORT/),'"',"\n";
foreach my $cc (sort keys %rcmap) {
	foreach my $st (sort keys %{$rcmap{$cc}}) {
		foreach my $rn (sort keys %{$rcmap{$cc}{$st}}) {
			my $rec = $rcmap{$cc}{$st}{$rn};
			my @values = ();
			foreach (qw/RCCC RCST RCNAME REGION RCSHORT/) {
				push @values, $rec->{$_};
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/rcmap.ca.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/RCCC RCST RCNAME REGION RCSHORT/),'"',"\n";
foreach my $cc (sort keys %rcmap) {
	next unless $cc eq 'CA';
	foreach my $st (sort keys %{$rcmap{$cc}}) {
		foreach my $rn (sort keys %{$rcmap{$cc}{$st}}) {
			my $rec = $rcmap{$cc}{$st}{$rn};
			my @values = ();
			foreach (qw/RCCC RCST RCNAME REGION RCSHORT/) {
				push @values, $rec->{$_};
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/rcmap.us.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/RCCC RCST RCNAME REGION RCSHORT/),'"',"\n";
foreach my $cc (sort keys %rcmap) {
	next unless $cc eq 'US';
	foreach my $st (sort keys %{$rcmap{$cc}}) {
		foreach my $rn (sort keys %{$rcmap{$cc}{$st}}) {
			my $rec = $rcmap{$cc}{$st}{$rn};
			my @values = ();
			foreach (qw/RCCC RCST RCNAME REGION RCSHORT/) {
				push @values, $rec->{$_};
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/rcmap.xx.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/RCCC RCST RCNAME REGION RCSHORT/),'"',"\n";
foreach my $cc (sort keys %rcmap) {
	next if $cc eq 'CA' or $cc eq 'US';
	foreach my $st (sort keys %{$rcmap{$cc}}) {
		foreach my $rn (sort keys %{$rcmap{$cc}{$st}}) {
			my $rec = $rcmap{$cc}{$st}{$rn};
			my @values = ();
			foreach (qw/RCCC RCST RCNAME REGION RCSHORT/) {
				push @values, $rec->{$_};
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

my @rnmap_keys = qw/REGION RCSHORT RCCC RCST RCNAME/;
$fn = "$datadir/rnmap.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@rnmap_keys),'"',"\n";
foreach my $rg (sort keys %rnmap) {
	foreach my $rc (sort keys %{$rnmap{$rg}}) {
		my $rec = $rnmap{$rg}{$rc};
		my @values = ();
		foreach (@rnmap_keys) {
			push @values, $rec->{$_};
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

my @rnmap_keys = qw/REGION RCSHORT RCCC RCST RCNAME/;
$fn = "$datadir/rnmap.ca.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@rnmap_keys),'"',"\n";
foreach my $rg (sort keys %rnmap) {
	foreach my $rc (sort keys %{$rnmap{$rg}}) {
		my $rec = $rnmap{$rg}{$rc};
		next unless $rec->{RCCC} eq 'CA';
		my @values = ();
		foreach (@rnmap_keys) {
			push @values, $rec->{$_};
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

my @rnmap_keys = qw/REGION RCSHORT RCCC RCST RCNAME/;
$fn = "$datadir/rnmap.us.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@rnmap_keys),'"',"\n";
foreach my $rg (sort keys %rnmap) {
	foreach my $rc (sort keys %{$rnmap{$rg}}) {
		my $rec = $rnmap{$rg}{$rc};
		next unless $rec->{RCCC} eq 'US';
		my @values = ();
		foreach (@rnmap_keys) {
			push @values, $rec->{$_};
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

my @rnmap_keys = qw/REGION RCSHORT RCCC RCST RCNAME/;
$fn = "$datadir/rnmap.xx.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@rnmap_keys),'"',"\n";
foreach my $rg (sort keys %rnmap) {
	foreach my $rc (sort keys %{$rnmap{$rg}}) {
		my $rec = $rnmap{$rg}{$rc};
		next if $rec->{RCCC} eq 'CA' or $rec->{RCCC} eq 'US';
		my @values = ();
		foreach (@rnmap_keys) {
			push @values, $rec->{$_};
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

exit;

1;

__END__
