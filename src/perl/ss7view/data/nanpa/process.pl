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

require "$codedir/database.pm";

my @files = `find . -name '*.html' | sort`;

my $fh = \*INFILE;
my $fn;

my $db = {};

$fn = "$codedir/db.pm";
if (-f $fn) {
	print STDERR "reading $fn\n";
	unless ($db = do "$fn") {
		die "can't parse file $fn: $@" if $@;
		die "can't do file $fn: $!" unless defined $db;
		die "can't run file $fn" unless $db;
	}
	die "no db" unless ref $db eq 'HASH';
}

my %nanpast = ();
$fn = "$datadir/states.txt";
print STDERR "reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my ($iso,$lerg,$sw,$name) = split(/\t/,$_);
	$nanpast{$name} = {iso=>$iso,lerg=>$lerg,sw=>$sw};
}
close($fh);

my %nanpacc = ();
$fn = "$datadir/countries.txt";
print STDERR "reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my ($iso,$lerg,$name) = split(/\t/,$_);
	$nanpacc{$name} = {iso=>$iso,lerg=>$lerg};
}
close($fh);

my %nanpatz = ();
$fn = "$datadir/tzones.txt";
print STDERR "reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my ($code,$tzs,$desc) = split(/\t/,$_);
	$nanpatz{$code} = {tzs=>$tzs,desc=>$desc};
}
close($fh);

sub do_oldnpa {
my %headings = (
	'Type of Code'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Is this code assignable'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'If not, why'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Geographic(G) or non-geographic(N)'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val eq 'G') {
			delete $data->{nongeo};
		} elsif ($val eq 'N') {
			$data->{nongeo} = 1;
		}
		return;
	},
	' or non-geographic(N)'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val eq 'G') {
			delete $data->{nongeo};
		} elsif ($val eq 'N') {
			$data->{nongeo} = 1;
		}
		return;
	},
	'If non-geographic, usage'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{explain} = $val;
		return;
	},
	'Is this code assigned'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val =~ /^N/i) {
			delete $data->{assigned};
		} elsif ($val =~ /^Y/i) {
			$data->{assigned} = 1;
		} else {
			print STDERR "can't grok assigned: '$val'\n";
		}
		return;
	},
	'Is this code reserved for future use'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val =~ /^N/i) {
			delete $data->{reserved};
		} elsif ($val =~ /^Y/i) {
			$data->{reserved} = 1;
		} else {
			print STDERR "can't grok reserved: '$val'\n";
		}
		return;
	},
	'Is this code in use'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val =~ /^N/i) {
			delete $data->{used};
		} elsif ($val =~ /^Y/i) {
			$data->{used} = 1;
		} else {
			print STDERR "can't grok used: '$val'\n";
		}
		return;
	},
	'NPA Relief Status'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'In service date'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return unless $val =~ /^([0-9]{2})\/([0-9]{2})\/([0-9]{4})$/;
		$data->{date} = "$3-$2-$1";
		return;
	},
	'Planning Letter(s)'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Location'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if (exists $nanpast{$val}) {
			$data->{state} = $nanpast{$val}{lerg};
			$data->{country} = $nanpast{$val}{iso};
			$data->{country} =~ s/-.*//;
			if ($data->{state} eq 'AS') {
				$data->{tzs} = [ -11 ];
			}
		} else {
			print STDERR "E: cannot grok Location: '$val'\n";
		}
		return;
	},
	'Country'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if (exists $nanpacc{$val}) {
			unless ($data->{country}) {
				$data->{country} = $nanpacc{$val}{lerg};
			}
		} else {
			print STDERR "E: cannot grok Country: '$val'\n";
		}
		return;
	},
	'Time Zone'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if (exists $nanpatz{$val}) {
			$data->{tzs} = [ split(/,/,$nanpatz{$val}{tzs}) ];
		} else {
			print STDERR "E: cannot grok Time Zone: '$val'\n";
		}
		return;
	},
	'Parent NPA'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{parent} = $val;
		return;
	},
	'Is this an overlay code'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val =~ /^Y/) {
			$data->{ovly} = 1;
		} else {
			delete $data->{ovly};
		}
		return;
	},
	'Overlay Complex'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{complex} = [ split(/\//,$val) ];
		return;
	},
	'Jeopardy'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Relief Planning in Progress'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Home NPA Local Calls'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{hnpa}{local}{stnd} = $val;
		return;
	},
	'Home NPA Local Calls+'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{hnpa}{local}{perm} = $val;
		return;
	},
	'Foreign NPA Local Calls'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{fnpa}{local}{stand} = $val;
		return;
	},
	'Foreign NPA Local Calls+'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{fnpa}{local}{perm} = $val;
		return;
	},
	'Home NPA Toll Calls'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{hnpa}{toll}{stnd} = $val;
		return;
	},
	'Home NPA Toll Calls+'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{hnpa}{toll}{perm} = $val;
		return;
	},
	'Foreign NPA Toll Calls'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{fnpa}{toll}{stnd} = $val;
		return;
	},
	'Foreign NPA Toll Calls+'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$data->{dial}{fnpa}{toll}{perm} = $val;
		return;
	},
	'Area Served'=>sub{ # commented out in HTML
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Date'=>sub{ # commented out in HTML
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Monthly Account'=>sub{ # commented out in HTML
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'Rationing'=>sub{ # commented out in HTML
		my ($data,$val) = @_;
		return unless $val;
		return;
	},
	'For a map of this NPA, please consult this planning letter'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{map} = $val;
		return;
	},
);

FILE: foreach my $file (@files) { chomp $file;
	next unless $file =~ /([0-9]{3})\.html/;
	my $npa = $1;
	my $data = {};
	$data->{npa} = $npa;
	print STDERR "processing $file\n";
	open($fh,"<",$file) or die "can't open $file";
	my $hdr = undef;
	while (<$fh>) { chomp;
		next unless /<TD/;
		if (/>([^<]+):</) {
			$hdr = $1;
			$hdr =~ s/<BR>//g;
			unless (exists $headings{$hdr}) {
				print STDERR "no procedure for '$hdr'\n";
				$hdr = undef;
			}
		} elsif ($hdr and />([^<]+)</) {
			my $val = $1;
			$val =~ s/&nbsp;//g;
			#print STDERR "I: $hdr: $val\n";
			if (&{$headings{$hdr}}($data,$val)) {
				#print STDERR "aborting processing of $file\n";
				#close($fh);
				#next FILE;
			}
			if ($hdr =~ /Calls$/) {
				$hdr .= '+';
				next;
			}
			$hdr = undef;
		}
	}
	close($fh);
	if (keys($data) > 1) {
		closerecord($data,$db);
	}
}
}

{
my %mapping = (
	'NPA'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{npa} = $val;
	},
	'Type of Code'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Easily Recognizable Code') {
			$dat->{type} = 'E';
		} elsif ($val eq 'General Purpose Code') {
			$dat->{type} = 'G';
		} else {
			print STDERR "W: can't grok Type of Code '$val'\n";
		}
	},
	'Assignable'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{assignable} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{assignable};
		} else {
			print STDERR "W: can't grok Assignable '$val'\n";
		}
	},
	'Explanation'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{explain} = $val;
	},
	'Reserved'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{reserved} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{reserved};
		} else {
			print STDERR "W: can't grok Reserved '$val'\n";
		}
	},
	'Assigned?'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{assigned} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{assigned};
		} else {
			print STDERR "W: can't grok Assigned '$val'\n";
		}
	},
	'Asgt Date'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{adate} = $val;
	},
	'Use'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'G') {
			delete $dat->{nongeo};
		} elsif ($val eq 'N') {
			$dat->{nongeo} = 1;
		} else {
			print STDERR "W: can't grok Use '$val'\n";
		}
	},
	'Service'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Inbound International') {
			$dat->{service} = 'I';
		} elsif ($val eq 'Personal Communication Service') {
			$dat->{service} = 'P';
		} elsif ($val eq 'Canadian Services') {
			$dat->{service} = 'C';
		} elsif ($val eq 'Interexchange Carrier Services') {
			$dat->{service} = 'X';
		} elsif ($val eq 'US Government') {
			$dat->{service} = 'G';
		} elsif ($val eq 'Toll-Free') {
			$dat->{service} = 'T';
		} elsif ($val eq 'Premium Services') {
			$dat->{service} = 'R';
		} else {
			print STDERR "W: can't grok Service '$val'\n";
		}
	},
	'Location'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if (exists $nanpast{$val}) {
			$dat->{state} = $nanpast{$val}{lerg};
			$dat->{country} = $nanpast{$val}{iso};
			$dat->{country} =~ s/-.*//;
			if ($dat->{state} eq 'AS') {
				$dat->{tzs} = [ -11 ];
			}
		} else {
			print STDERR "E: can't grok Location '$val'\n";
		}
	},
	'Country'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if (exists $nanpacc{$val}) {
			unless ($dat->{country}) {
				$dat->{country} = $nanpacc{$val}{lerg};
			}
		} else {
			print STDERR "E: can't grok Country '$val'\n";
		}
	},
	'In Service?'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{inservice} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{inservice};
		} else {
			print STDEERR "E: can't grok In Service '$val'\n";
		}
	},
	'In Svc Date'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{date} = $val;
	},
	'Status'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Active') {
			$dat->{status} = 'A';
		} elsif ($val eq 'Suspended') {
			$dat->{status} = 'S';
		} else {
			print STDERR "E: can't grok Status '$val'\n";
		}
	},
	'PL'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		# space separated list of planning letters
	},
	'Overlay'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{ovly} = 'O';
		} elsif ($val eq 'No') {
			$dat->{ovly} = 'S';
		} else {
			print STDERR "E: can't grok Overlay '$val'\n";
		}
	},
	'Overlay Complex'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{complex} = [split(/\//,$val)];
	},
	'Parent'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{parent} = $val;
	},
	'Time Zone'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if (exists $nanpatz{$val}) {
			$dat->{tzs} = [ split(/,/,$nanpatz{$val}{tzs}) ];
		} else {
			print STDERR "E: can't grok Time Zone '$val'\n";
		}
	},
	'Map'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{map} = $val;
	},
	'Is NPA in Jeoparady?'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{jeopardy} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{jeopardy};
		} else {
			print STDERR "E: can't grok Jeopardy '$val'\n";
		}
	},
	'Is Relief Planning in Progress'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		if ($val eq 'Yes') {
			$dat->{relief} = 1;
		} elsif ($val eq 'No') {
			delete $dat->{relief};
		} else {
			print STDERR "E: can't grok Relief '$val'\n";
		}
	},
	'Home NPA Local Calls'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{hnpa}{local}{stand} = $val;
	},
	'Home NPA Toll Calls'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{hnpa}{toll}{stand} = $val;
	},
	'Foreign NPA Local Calls'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{fnpa}{local}{stand} = $val;
	},
	'Foreign NPA Toll Calls'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{fnpa}{toll}{stand} = $val;
	},
	'perm HNPA local'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{hnpa}{local}{perm} = $val;
	},
	'perm HNPA toll'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{hnpa}{toll}{perm} = $val;
	},
	'perm FNPA local'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		return if $val eq 'NA';
		$dat->{dial}{fnpa}{local}{perm} = $val;
	},
	'dp Notes'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{remark} = $val;
	},
);

$fn = "$datadir/AllNPAs.mdb";
print STDERR "processing $fn\n";
open($fh,"mdb-export -Q \"-d\\t\" '-D%F' $fn 'Public NPA Database Table' |") or die "can't process $fn";
{
my $header = 1;
my @fields = ();
while (<$fh>) { chomp;
	my @tokens = split(/\t/,$_);
	if ($header) {
		@fields = @tokens;
		$header = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$tokens[$i]);
		} else {
			print STDERR "W: no mapping for $fields[$i]\n";
		}
	}
	closerecord($data,$db);
}
}
close($fh);
}

{
my %mapping = (
	'NPA'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{npa} = $val;
	},
	'NXX'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{nxx} = $val;
		$dat->{blk} = 'A';
		$dat->{rng} = '0000-9999';
		$dat->{lines} = 10000;
	},
	'OCN'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{ocn} = $val;
	},
	'Company'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{company} = $val;
	},
	'Status'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		# FIXME complete more fields here
		if ($val eq 'In Service') {
		} elsif ($val eq 'Available') {
			$dat->{atype} = 'VC';
		} elsif ($val eq 'Not Available') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'For Special Use') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'Recovered/Aging') {
		} elsif ($val eq 'Assigned') {
			$dat->{atype} = 'AS';
		} elsif ($val eq 'New Entrants Reserved') {
			$dat->{atype} = 'VC';
		} elsif ($val eq 'Available as Initial Code only') {
			$dat->{atype} = 'VC';
		} elsif ($val eq 'Protected') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'Being Recovered') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'New NPA Testing') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'Relief NPA') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'Plant Test') {
			$dat->{atype} = 'UA';
		} elsif ($val eq 'Moratorium on Assignment') { # usually 310
			$dat->{atype} = 'UA';
		} else {
			print STDERR "E: can't grok Status '$val'\n";
		}
	},
	'RateCenter'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{rate} = $val;
	},
	'Remarks'=>sub{
		my ($dat,$val) = @_;
		return unless $val;
		$dat->{note} = $val;
	},
);

$fn = "$datadir/NPANXX.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn cygdrive/d/cna/COCode-ESRDData/NPANXX.csv |") or die "can't process $fn";
{
my $header = 1;
my @fields = ();
while (<$fh>) { chomp; s/\r//g;
	s/^"//;
	s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		#print STDERR "I: fields are '", join('|',@fields), "'\n";
		$header = undef;
		next;
	}
	my $data = {lata=>'888'};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$tokens[$i]);
		} else {
			print STDERR "W: no mapping for $fields[$i]\n";
		}
	}
	closerecord($data,$db);
}
}
close($fh);
}

{
$fn = "$datadir/allutlzd.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn allutlzd.txt | expand |") or die "can't process $fn";
while (<$fh>) { chomp;
	next unless /^[A-Z]{2}[ ]{6}/;
	my $data = {};
	$data->{state} = substr($_,0,2);
	$data->{npa} = substr($_,8,3);
	$data->{nxx} = substr($_,12,3);
	$data->{blk} = 'A';
	$data->{rng} = '0000-9999';
	$data->{lines} = 10000;
	$data->{ocn} = substr($_,16,4);
	delete $data->{ocn} if $data->{ocn} =~ /^\s*$/;
	$data->{company} = substr($_,24,62);
	$data->{company} =~ s/^"//;
	$data->{company} =~ s/"\s*$//;
	delete $data->{company} if $data->{company} =~ /^\s*$/;
	$data->{rate} = substr($_,88,10);
	$data->{rate} =~ s/\s+$//;
	delete $data->{rate} if $data->{rate} =~ /^\s*$/;
	$data->{edate} = substr($_,104,10); # MM/DD/YYYY
	if ($data->{edate} =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
		$data->{edate} = "$3-$1-$2";
	} else {
		delete $data->{edate};
	}
	$data->{atype} = substr($_,120,2); # (VC|AS|PR|RV|UA)
	delete $data->{atype} unless $data->{atype} =~ /^(VC|AS|PR|RV|UA)$/;
	$data->{adate} = substr($_,128,10); # MM/DD/YYYY
	if ($data->{adate} =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
		$data->{adate} = "$3-$1-$2";
	} else {
		delete $data->{adate};
	}
	$data->{growth} = substr($_,144,1); # (I|G)
	delete $data->{growth} unless $data->{growth} =~ /^(I|G)$/;
	closerecord($data,$db);
}
close($fh);
}

dumpem($datadir,$db);
dumpem($codedir,$db);
