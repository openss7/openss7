#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;

use strict;
use Data::Dumper;

my $fh = \*INFILE;

my $db = {};

sub close_record {
	my $data = shift;
	if ($data->{nxx}) {
		foreach my $npa (keys %{$data->{nxx}}) {
			foreach my $nxx (keys %{$data->{nxx}{$npa}}) {
				foreach my $blk (keys %{$data->{nxx}{$npa}{$nxx}}) {
					foreach my $rng (keys %{$data->{nxx}{$npa}{$nxx}{$blk}}) {
						my $nxx = {};
						$nxx = $db->{nxx}{$npa}{$nxx}{$blk}{$rng} if exists $db->{nxx}{$npa}{$nxx}{$blk}{$rng};
						$nxx->{lines} += $data->{nxx}{$npa}{$nxx}{$blk}{$rng};
						$nxx->{st}   = $data->{st}   if $data->{st};
						$nxx->{loc}  = $data->{loc}  if $data->{loc};
						$nxx->{ccli} = $data->{clli} if $data->{clli};
						$nxx->{lata} = $data->{lata} if $data->{lata};
						$db->{nxx}{$npa}{$nxx}{$blk}{$rng} = $nxx;
					}
				}
			}
		}
	}
	if ($data->{lata}) {
		my $lata = {};
		$lata = $db->{lata}{$data->{lata}} if exists $db->{lata}{$data->{lata}};
		$lata->{lata} = $data->{lata};
		if ($data->{st}) {
			$lata->{state}{$data->{st}} += $data->{lines};
		}
		if ($data->{npa}) {
			foreach my $npa (keys %{$data->{npa}}) {
				$lata->{npa}{$npa} += $data->{npa}{$npa};
			}
		}
		$lata->{lines} += $data->{lines};
		$db->{lata}{$data->{lata}} = $lata;
	}
	if ($data->{clli}) {
		my $clli = {};
		$clli = $db->{clli}{$data->{clli}} if exists $db->{clli}{$data->{clli}};
		$clli->{clli} = $data->{clli};
		if ($data->{loc}) {
			if ($clli->{loc} and $clli->{loc} ne $data->{loc}) {
				warn "CLLI location changing from $clli->{loc} to $data->{loc}";
			}
			$clli->{loc} = $data->{loc};
		}
		if ($data->{st}) {
			$clli->{state}{$data->{st}} += $data->{lines};
		}
		if ($data->{nxx}) {
			foreach my $npa (keys %{$data->{nxx}}) {
				foreach my $nxx (keys %{$data->{nxx}{$npa}}) {
					foreach my $blk (keys %{$data->{nxx}{$npa}{$nxx}}) {
						foreach my $rng (keys %{$data->{nxx}{$npa}{$nxx}{$blk}}) {
							$clli->{nxx}{$npa}{$nxx}{$blk} += $data->{nxx}{$npa}{$nxx}{$blk}{$rng};
						}
					}
				}
			}
		}
		if ($data->{lata}) {
			$clli->{lata}{$data->{lata}} += $data->{lines};
		}
		$clli->{lines} += $data->{lines};
		$db->{clli}{$data->{clli}} = $clli;
	}
	if ($data->{ocn}) {
		my $ocn = {};
		$ocn = $db->{ocn}{$data->{ocn}} if exists $db->{ocn}{$data->{ocn}};
		$ocn->{ocn} = $data->{ocn};
		if ($data->{st}) {
			$ocn->{state}{$data->{st}} += $data->{lines};
		}
		if ($data->{lata}) {
			$ocn->{lata}{$data->{lata}} += $data->{lines};
		}
		if ($data->{npa}) {
			foreach my $npa (keys %{$data->{npa}}) {
				$ocn->{npa}{$npa} += $data->{npa}{$npa};
			}
		}
		$ocn->{lines} += $data->{lines};
		$db->{ocn}{$data->{ocn}} = $ocn;
	}
}

my $state = undef;

my %stnames = (
	'ALABAMA'=>'AL',
	'ALASKA'=>'AK',
	'AMERICAN SAMOA'=>'AS',
	'ARIZONA'=>'AZ',
	'ARKANSAS'=>'AR',
	'CALIFORNIA'=>'CA',
	'COLORADO'=>'CO',
	'CONNECTICUT'=>'CT',
	'DELAWARE'=>'DE',
	'DISTRICT OF COLUMBIA'=>'DC',
	'FLORIDA'=>'FL',
	'GEORGIA'=>'GA',
	'GUAM'=>'GU',
	'HAWAII'=>'HI',
	'IDAHO'=>'ID',
	'ILLINOIS'=>'IL',
	'INDIANA'=>'IN',
	'IOWA'=>'IA',
	'KANSAS'=>'KS',
	'KENTUCKY'=>'KY',
	'LOUISIANA'=>'LA',
	'MAINE'=>'ME',
	'MARYLAND'=>'MD',
	'MASSACHUSETTS'=>'MA',
	'MICHIGAN'=>'MI',
	'MINNESOTA'=>'MN',
	'MISSISSIPPI'=>'MS',
	'MISSOURI'=>'MO',
	'MONTANA'=>'MT',
	'NEBRASKA'=>'NE',
	'NEVADA'=>'NV',
	'NEW HAMPSHIRE'=>'NH',
	'NEW JERSEY'=>'NJ',
	'NEW MEXICO'=>'NM',
	'NEW YORK'=>'NY',
	'NORTH CAROLINA'=>'NC',
	'NORTH DAKOTA'=>'ND',
	'OHIO'=>'OH',
	'OKLAHOMA'=>'OK',
	'OREGON'=>'OR',
	'PENNSYLVANIA'=>'PA',
	'PUERTO RICO'=>'PR',
	'RHODE ISLAND'=>'RI',
	'SOUTH CAROLINA'=>'SC',
	'SOUTH DAKOTA'=>'SD',
	'TENNESSEE'=>'TN',
	'TEXAS'=>'TX',
	'UTAH'=>'UT',
	'VERMONT'=>'VT',
	'VIRGINIA'=>'VA',
	'VIRGIN ISLANDS'=>'VI',
	'WASHINGTON'=>'WA',
	'WEST VIRGINIA'=>'Wv',
	'WISCONSIN'=>'WI',
	'WYOMING'=>'wY',
);

sub checkstate {
	my ($data,$line) = @_;
	if ($line =~ /SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
		my $name = $1;
		if (exists $stnames{$name}) {
			$state = $stnames{$name};
		} else {
			$state = undef;
		}
	}
}


foreach my $fn (qw/NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt/) {
open($fh,"<","$datadir/$fn");

my $inheader = 1;
my $infooter = 0;
my $lineno = 0;
my $data = {};

while (<$fh>) { chomp;
	#print "HEADER: $_\n" if $inheader;
	if ($inheader) {
		checkstate($data,$_) if /^SERVING WIRE/;
		next unless /^---/;
		$inheader = 0;
		next;
	}
	if (/^\.\.\.\.\./) {
		#print "CLOSING RECORD: $_\n";
		# close record
		print "--------\n";
		$db->{clli}{$data->{clli}} = $data;
		$data = {};
		$lineno = 0;
		next;
	}
	if (/^\(x\) / or /\=====/ or /PRINTED IN U.S.A./ or /END OF SECTION/) {
		#print "FOOTER: $_\n";
		$inheader = 1;
		next;
	}
	next if /^\s*$/;
	if (/^[A-Z]/) {
		if ($lineno) {
			# close record
			print "--------\n";
			$db->{clli}{$data->{clli}} = $data;
			$data = {};
			$lineno = 0;
		}
	} else {
		unless ($lineno) {
			checkstate($data,$_) if /^SERVING WIRE/;
			#print "SKIPPING: $_\n";
			next;
		}
	}
	$lineno++;
	printf "DATA %2d: %s\n", $lineno, $_;
	if ($lineno >= 1) {
		while (/\b([A-Z][A-Z0-9])\b/g) {
			$data->{feat}{$1} = 1;
		}
	}
	if ($lineno == 1) {
		if (/^([A-Z]+.*?)\s+([A-Z][A-Z0-9][A-Z0-9][A-Z0-9 ][A-Z0-9][A-Z0-9][A-Z0-9][A-Z0-9][A-Z0-9][A-Z0-9][A-Z0-9])\s+([0-9][0-9][0-9][0-9][0-9])\s+([0-9][0-9][0-9][0-9][0-9])\s+([0-9][0-9][0-9][A-Z0-9])\s+([0-9][0-9][0-9]+)/) {
			$data->{st} = $state if $state;
			$data->{loc} = $1;
			$data->{clli} = $2;
			$data->{wcv} = $3;
			$data->{wch} = $4;
			$data->{cc} = $5;
			$data->{lata} = $6;
		} else {
			print "ERROR: $_\n";
		}
	} elsif ($lineno > 1) {
		if (/^\s*([2-9][0-9][0-9])\//) {
			my $npa = $1;
			s/^\s*[2-9][0-9][0-9]\///;
			s/\s+[A-Z][A-Z0-9]\b//g;
			s/\s*\([A-Za-z]\)\b//g;
			s/\s+.*$//;
			my @nxxs = split(/,/,$_);
			foreach my $nxx (@nxxs) {
				my @blks = ();
				my @rngs = ();
				if ($nxx =~ /^[1-9][0-9][0-9]$/) {
					push @blks, 'A';
					push @rngs, '0000-9999';
				} elsif ($nxx =~ /^([2-9][0-9][0-9])\((([0-9])[0-9][0-9][0-9])-(([0-9])[0-9][0-9][0-9])\)$/) {
					$nxx = $1;
					my ($s,$e) = ($2,$4);
					my ($f,$l) = ($3,$5);
					for (my $i=$f;$i<=$l;$i++) {
						push @blks, $i;
						push @rngs, sprintf("%04d-%04d",$s,$e);
					}
				} else {
					print "BAD NXX: $nxx\n";
				}
				foreach my $blk (@blks) {
					my $rng = shift @rngs;
					#if (exists $db->{nxx}{$npa}{$nxx}{$blk} and $db->{nxx}{$npa}{$nxx}{$blk} ne $data->{clli}) {
					#	printf "OVERWRITE: %3s-%3s-%1s %s -> %s\n",
					#	$npa, $nxx, $blk, $db->{nxx}{$npa}{$nxx}{$blk},
					#	$data->{clli};
					#}
					#$db->{nxx}{$npa}{$nxx}{$blk} = $data->{clli};
					if (exists $db->{nxx}{$npa}{$nxx}{$blk}) {
						print "SHARED: $npa-$nxx-$blk\n";
					}
					$db->{nxx}{$npa}{$nxx}{$blk}{$rng} = $data->{clli};
					my ($f,$t) = split(/-/,$rng);
					$f = int($f); $t = int($t);
					my $lines = $t - $f + 1;
					$data->{npa}{$npa} += $lines;
					$data->{lines} += $lines;
				}
			}
		} else {
			printf "UGLY %2d: %s\n", $lineno,$_;
		}
	}
}

close($fh);
}

open($fh,">","$datadir/cllis.pm");
my $dumper = Data::Dumper->new([$db]);
print $fh $dumper->Dump;
close($fh);

my @cllis = keys %{$db->{clli}};
my $num = scalar @cllis;
print "There are $num CLLIs\n";

sub printnotice {
	my $file = shift;
	print $file <<'EOF';
#----------------------------------------------------------------------------
# NOTICE:  This file contains simple data gleened from public sources.  As
# such, the data is not subject to provisions under copyright law.  You can
# make as many copies of or manipulate the data in any way you see fit.  The
# OpenSS7 Project does not charge any fee for obtaining or using this data.  It
# is provided for our customers to use with OpenSS7 application software.  The
# data is defective and might not even be 80% accurate and might not be even
# 30% complete.
#----------------------------------------------------------------------------
# THERE IS NO WARRANTY FOR THE DATA, TO THE EXTENT PERMITTED BY APPLICABLE LAW.
# EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
# PARTIES PROVIDE THE DATA "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
# TO THE QUALITY AND PERFORMANCE OF THE DATA IS WITH YOU.  SHOULD THE DATA
# PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
# CORRECTION.
#----------------------------------------------------------------------------
# IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
# WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
# THE DATA AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
# GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
# USE OR INABILITY TO USE THE DATA (INCLUDING BUT NOT LIMITED TO LOSS OF DATA
# OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
# PARTIES OR A FAILURE OF THE DATA TO OPERATE WITH ANY OTHER PROGRAMS),
# EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGES.
#----------------------------------------------------------------------------
# Note that this is not the Telcordia LERG product.  If you have license to
# Telcordia Technologies LERG data, you should substitute the corresponding
# file from Telcordia for this file.  See the LERGINFO.DOC and LERGSPEC.DOC
# documents available from Telcordia at www.trainfo.com for further information
# about the formatting of these files and for information on obtaining the
# Telcordia LERG Routing Guide.  Telcordia is a registered trademark and LERG
# Routing Guide is a trademark of Telcordia Technologies, Inc.
EOF
}

open($fh,"<","$datadir/LERG5.DAT");
while (<$fh>) { chomp;
	next if /^#/;
	my $hist = substr($_, 0,40); $hist =~ s/^\s+//; $hist =~ s/\s+$//;
	my $lata = substr($_,40, 5); $lata =~ s/^\s+//; $lata =~ s/\s+$//;
	my $name = substr($_,45,20); $name =~ s/^\s+//; $name =~ s/\s+$//;
	$db->{lata}{$lata}{lata} = $lata;
	$db->{lata}{$lata}{name} = $name;
	$db->{lata}{$lata}{hist} = $hist;
}
close($fh);

my $of = \*OUTFILE;

print STDERR "writing $datadir/LERG6.DAT\n";
open($of,">","$datadir/LERG6.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG6.DAT
# ---------
# The LERG6 file provides NPA NXX (central office code) information and, where
# applicable, BLOCK assignments within the NPA NXX.  NPA NXX assignments (those
# made by the CO Code Administrator) are represented with a BLOCK value of "A".
# Thousands Block Pooling assignments (those made by the Pooling Admnistrator)
# are represented by numeric BLOCK values (0-9).  Only those BLOCKs actually
# assigned are provided (i.e., not all blocks 0-9, may appear).  This file
# includes amoung other data, the Rate Center associated with each record, the
# OCN (company assigned the record), the serving switch/POI with an associated
# switch homing arrangement (SHA) indicator, the identified "use" of the record
# (e.g. cellular, wireline -- see COCTYPE/SSC), and LATA (both switch and Rate
# Center LATA).
# ---------
# NPA NXX Block Assignments (Key field 5,6,7)
# ---------
#   1-  5  5 LATA (of Switch Location)
#   6- 25 20 LATA Name
#  26- 26  1 Status ( |E|M|D)
#  27- 32  6 Effective Date (mmddyy)
#  33- 35  3 NPA
#  36- 38  3 NXX (Central Office Code)
#  39- 39  1 Block ID (A, 0-9)
#  40- 43  4 filler
#  44- 46  3 COC Type
#  47- 50  4 Special Service Code (SSC)
#  51- 51  1 Dialable Indicator (DIND)(Y/N)
#  52- 53  2 Terminating Digits - End Office (EO)
#  54- 55  2 Terminating Digits - Access Tandem (AT)
#  56- 56  1 Portable Indicator (Y/N)
#  57- 60  4 Administrative OCN (AOCN)
#  61- 61  1 filler
#  62- 65  4 Operating Company Number (OCN)
#  66- 75 10 Locality Name
#  76- 77  2 Locality Country
#  78- 79  2 Locality State/Province/Territory/Country
#  80- 89 10 RC Name Abbreviation
#  90- 90  1 RC Type
#  91- 94  4 Lines from (0000,1000,...) (1212 for NXX 555)
#  95- 98  4 Lines to   (0999,1999,...) (1212 for NXX 555)
#  99-109 11 Switch
# 110-111  2 Switch Homing Arrangement (SHA) Indicator
# 112-115  4 filler
# 116-119  4 Test line number (XXXX) (i.e. NPA NXX XXXX)
# 120-120  1 Response (A)nnouncement/(M)illiwat Tone
# 121-126  6 Expiration Date (mmddyy)
# 127-127  1 Thousands Block Pooling Indicator
# 128-132  5 LATA (of Rate Center Location)
# 133-133  1 filler
# 134-139  6 Creation Date in BIRRDS (mmddyy)
# 140-140  1 filler
# 141-146  6 "E" STATUS date (mmddyy)
# 147-147  1 filler
# 148-153  6 Last Modification Date (mmddyy)
# 154-170 17 filler
# ---------
EOF
foreach my $npa (sort keys %{$db->{nxx}}) {
foreach my $nxx (sort keys %{$db->{nxx}{$npa}}) {
foreach my $blk (sort keys %{$db->{nxx}{$npa}{$nxx}}) {
	my @rngs = (keys %{$db->{nxx}{$npa}{$nxx}{$blk}});
	my $tpi = scalar(@rngs) > 1 ? 'N' : 'Y';
foreach my $rng (sort @rngs) {
	my $clli = $db->{nxx}{$npa}{$nxx}{$blk}{$rng};
	my $data = $db->{clli}{$clli};
	my ($f,$t) = split('-',$rng);
	my $name = $db->{lata}{$data->{lata}}{name} if exists $db->{lata}{$data->{lata}}{name};
	printf $of "%-5.5s", $data->{lata};
	printf $of "%-20.20s", $name; # LATA Name
	printf $of "%-1.1s", ''; # Status
	printf $of "%-6.6s", ''; # Effective Date
	printf $of "%-3.3s", $npa;
	printf $of "%-3.3s", $nxx;
	printf $of "%-1.1s", $blk;
	printf $of "%-4.4s", ''; # filler
	printf $of "%-3.3s", 'EOC'; # COC Type
	printf $of "%-4.4s", 'N'; # SSC
	printf $of "%-1.1s", 'Y'; # Dialable Indicator
	printf $of "%-2.2s", 'NA'; # Digs EO
	printf $of "%-2.2s", 'NA'; # Digs AT
	printf $of "%-1.1s", ''; # Portabile Indicator
	printf $of "%-4.4s", ''; # AOCN
	printf $of "%-1.1s", ''; # filler
	printf $of "%-4.4s", $data->{cc};
	printf $of "%-10.10s", $data->{loc};
	printf $of "%-2.2s", ''; # Locality County
	printf $of "%-2.2s", substr($clli,4,2); # Locality State/Terr/Prov/Cntry
	printf $of "%-10.10s", ''; # RC Name Abbreviation
	printf $of "%-1.1s", ''; # RC Type
	printf $of "%-4.4s", $f; # Lines From
	printf $of "%-4.4s", $t; # Lines to
	printf $of "%-11.11s", $clli;
	printf $of "%-2.2s", ''; # SHA
	printf $of "%-4.4s", ''; # filler
	printf $of "%-4.4s", ''; # Test line number
	printf $of "%-1.1s", ''; # Response
	printf $of "%-6.6s", ''; # Expiration Date
	printf $of "%-1.1s", ''; # $tpi Thousands block pooling indicator
	printf $of "%-5.5s", ''; # RC LATA
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # Creation date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # E Status Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # last mod date
	printf $of "%-17.17s", ''; # filler
	print $of "\n";
}
}
}
}
close($of);

my $feats = {
	AA=>'FGA OFFICE',
	AB=>'FGA CALL SCREENING',
	BA=>'FGB OFFICE',
	BC=>'FGB OFFICE W/ ANI',
	CA=>'FGC OFFICE',
	CC=>'FGC w/ SS7 PROTOCOL',
	D1=>'CENTRALIZED EQUAL ACCESS TANDEM OFFICE',
	DA=>'FGD OFFICE',
	DC=>'FGD OFFICE W/O 101XXXX DIALING',
	DD=>'CENTRALIZED EQUAL ACCESS END OFFICE',
	EA=>'CENTREX INTERNET PROTOCOL',
	G1=>'DIRECTORY ASSISTANCE LOCATION NPA 555',
	G2=>'OUTWARD OPERATOR SERVICES LOCATION',
	G3=>'INWARD OPERATOR SERVICES LOCATION',
	G4=>'1+ COIN CAPABLE EQUAL ACCESS END OFFICE - MOSS SIGNALLING',
	G5=>'1+ COIN CAPABLE EQUAL ACESSS END OFFICE - DIRECT TRUNK  EAOSS SIGNALLING',
	G6=>'1+ COIN CAPABLE EQUAL ACCESS END OFFICE - TANDEM MOSS SIGNALING',
	G7=>'1+ COIN CAPABLE EQUAL ACCESS END OFFICE - TANDEM EAOSS SIGNALING',
	G8=>'DESIGNATED OPERATOR SERVICE (DOS) LOCATION',
	G9=>'OPERATOR SERVICES AUTOMATIC CALL DISTRIBUTION (OS-ACD) SERVICE LOCATION',
	GA=>'TOLL STATION LOCATION',
	GB=>'ACCESS TANDEM OFFICE',
	GC=>'WATS OR WATS TYPE OFFICE',
	GD=>'800 "SERIES" OR 800 "SERIES" TYPE OFFICE',
	GE=>'PUBLIC SWITCHED 56KBPS DIGITAL DATA SERVICE (PSDS)',
	GF=>'INTERNATIONAL BOUNDARY POINT',
	GG=>'LIDB ACCESS SERVICE LOCATION',
	GH=>'ELECTRONIC WHITE PAGES (EWP) LOCATION',
	GI=>'500 ACCESS OFFICE',
	GJ=>'PUBLIC SWITCHED 64 KBPS CLEAR CHANNEL CAPABILITY',
	GK=>'0+900 ACCESS OFFICE',
	GL=>'INTERNATIONAL BLOCKING',
	GM=>'WIRE CENTER INCAPABLE OF MEASURING AND RECORDING',
	GN=>'900 BLOCKING',
	GP=>'PRE-PAID CALLING SERVICE ACCESS',
	GQ=>'CARRIER IDENTIFICATION PARAMETER (CIP)',
	GR=>'LOCAL NUMBER PORTABILITY CAPABILITY',
	HA=>'SIGNAL POINT (SP)',
	HB=>'SERVICE SWITCHING POINT (SSP)',
	HC=>'SIGNAL TRANSFER POINT (STP)',
	HD=>'800 "SERIES" QUERY CHARGE BILLING LOCATION',
	HE=>'NON-SSP EQUIPPED ORIGINATING 800 "SERIES" LOCATION',
	HG=>'A WIRE CENTER INCAPABLE OF DISTINGUISHING BETWEEN TANDEM SWITCHED AND DIRECT ROUTED TRAFFIC',
	HH=>'800 "ONLY" QUERY CHARGE BILLING LOCATION',
	HI=>'SSP EQUIPPED BUT INCAPABLE OF HANDLING DIRECT ROUTING OF 888',
	HJ=>'ORIGINATING LINE SCREENING (OLS) PROVISIONED BY FLEX ANI',
	HK=>'ORIGINATING LINE SCREENING (OLS) PROVISIONED BY LIDB',
	HL=>'FACILITY SIGNALING POINT OF INTERCONNECTION (FSPOI)',
	IA=>'ALTERNATE ROUTING',
	IB=>'CALLING BILLING NUMBER DELIVERY - FGD PROTOCOL',
	IC=>'MULTILINE HUNT GROUP',
	ID=>'MULTILINE HUNT GROUP - INDIVIDUAL ACCESS TO EACH PORT IN HUNT GROUP',
	IE=>'MULTILINE HUNT GROUP - UCD WITH QUEUING',
	IF=>'MULTILINE HUNT GROUP - C.O. ANNOUNCEMENTS',
	IG=>'MULTILINE HUNT GROUP - UNIFORM CALL DISTRIBUTION LINE HUNTING',
	IH=>'MAKE BUSY KEY',
	II=>'FLEXIBLE AUTOMATIC NUMBER IDENTIFICATION (ANI)',
	IP=>'IP GATEWAY OFFICE                                                         (N)',
	JA=>'SWITCH TO COMPUTER APPLICATIONS INTERFACE',
	JB=>'CUSTOMER NETWORK MANAGEMENT',
	JC=>'REAL TIME PIC (PRIMARY INTEREXCHANGE CARRIER) PROCESSING SERVICE',
	JD=>'PRIMARY INTEREXCHANGE CARRIER (PIC) VERIFICATION SERVICE',
	JE=>'TRANSPARENT LAN SERVICE',
	OE=>'OPTICAL ETHERNET SERVICE',
	OH=>'OPTICAL ETHERNET HUB - INTERMEDIATE#',
	OR=>'OPTICAL ETHERNET SUBTENDING WIRE CENTER',
	PA=>'SPECIAL ACCESS - PRICING ZONE 1',
	PB=>'SPECIAL ACCESS - PRICING ZONE 2',
	PC=>'SPECIAL ACCESS - PRICING ZONE 3',
	PD=>'SPECIAL ACCESS - PRICING ZONE 4',
	PE=>'BASE RENT AREA 1',
	PF=>'BASE RENT AREA 2',
	PG=>'BASE RENT AREA 3',
	PH=>'BASE RENT AREA 4',
	PJ=>'SWITCHED ACCESS - PRICING ZONE 1',
	PK=>'SWITCHED ACCESS - PRICING ZONE 2',
	PL=>'SWITCHED ACCESS - PRICING ZONE 3',
	PM=>'SWITCHED ACCESS - PRICING ZONE 4',
	PN=>'TERM AND VOLUME DISCOUNT',
	PO=>'SPECIAL ACCESS - PRICING ZONE 5',
	PQ=>'SWITCHED ACCESS - PRICING ZONE 5',
	PR=>'PHASE I MSA PRICING FLEXIBILITY (FULL CONTRACTUAL RELIEF AREA)',
	PS=>'PHASE II MSA PRICING FLEXIBILITY (FULL SERVICE RELIEF AREA)',
	PT=>'PHASE I MSA PRICING FLEXIBILITY (LIMITED CONTRACTUAL RELIEF AREA)',
	PU=>'PHASE II MSA PRICING FLEXIBILITY (LIMITED SERVICE RELIEF AREA)',
	QA=>'SWITCHED ACCESS INTERCONNECTION - PHYSICAL COLLOCATION FIBER',
	QB=>'SWITCHED ACCESS INTERCONNECTION - VIRTUAL COLLOCATION FIBER',
	QC=>'SWITCHED ACCESS INTERCONNECTION - PHYSICAL COLLOCATION MICROWAVE',
	QD=>'SWITCHED ACCESS INTERCONNECTION - VIRTUAL COLLOCATION MICROWAVE',
	QE=>'SPECIAL ACCESS INTERCONNECTION - PHYSICAL COLLOCATION FIBER',
	QF=>'SPECIAL ACCESS INTERCONNECTION - VIRTUAL COLLOCATION FIBER',
	QG=>'SPECIAL ACCESS INTERCONNECTION - PHYSICAL COLLOCATION MICROWAVE',
	QH=>'SPECIAL ACCESS INTERCONNECTION - VIRTUAL COLLOCATION MICROWAVE',
	RA=>'METALLIC BRIDGING (THREE PREMISES AND SERIES)',
	RB=>'TELEGRAPH BRIDGING (TWO-WIRE AND FOUR-WIRE)',
	RC=>'VOICE BRIDGING (TWO-WIRE AND FOUR-WIRE)',
	RD=>'VOICE GRADE DATA BRIDGING (TWO-WIRE AND FOUR-WIRE)',
	RE=>'TELEPHOTO BRIDGING (TWO-WIRE AND FOUR-WIRE)',
	RF=>'TELEMETRY AND ALARM BRIDGING SPLIT BAND (ACTIVE)',
	RG=>'TELEMETRY AND ALARM BRIDGING SUMMATION (ACTIVE)',
	RH=>'TELEMETRY AND ALARM BRIDGING (PASSIVE)',
	RJ=>'DATAPHONE SELECT-A-STATION BRIDGING-SEQUENTIAL ARRANGEMENT (TWO-WIRE AND FOUR-WIRE)',
	RK=>'DATAPHONE SELECT-A-STATION BRIDGING-ADDRESSABLE ARRANGEMENT (TWO-WIRE AND FOUR-WIRE)',
	RL=>'DIGITAL DATA BRIDGING - INTERMEDIATE',
	RM=>'PROGRAM AUDIO BRIDGING',
	RN=>'VOICE BRIDGING - TERMINUS (TWO-WIRE AND FOUR-WIRE)',
	RO=>'VOICE GRADE DATA BRIDGING - TERMINUS (TWO-WIRE AND FOUR- WIRE)',
	RP=>'VOICE BRIDGING - INTERMEDIATE (TWO-WIRE AND FOUR-WIRE)',
	RQ=>'VOICE GRADE DATA BRIDGING - INTERMEDIATE (TWO-WIRE AND FOUR-WIRE)',
	RS=>'SYNCHRONOUS DIGITAL DATA SERVICE BRIDGING CAPABILITY',
	RT=>'DATA OVER VOICE DIGITAL BRIDGING CAPABILITY',
	RU=>'METALLIC BRIDGING - INTERMEDIATE (THREE PREMISES AND SERIES)',
	RW=>'TELEGRAPH BRIDGING - INTERMEDIATE (TWO-WIRE AND FOUR-WIRE)',
	RX=>'PROGRAM AUDIO BRIDGING - INTERMEDIATE',
	SA=>'VOICE GRADE MULTIPLEXING',
	SC=>'WIDEBAND ANALOG MULTIPLEXING (MASTERGROUP TO SUPERGROUP)',
	SD=>'WIDEBAND ANALOG MULTIPLEXING (SUPERGROUP TO GROUP)',
	SE=>'WIDEBAND ANALOG MULTIPLEXING (GROUP TO VOICE)',
	SF=>'WIDEBAND ANALOG MULTIPLEXING (GROUP TO DS1)',
	SG=>'HIGH CAPACITY MULTIPLEXING (DS4 TO DS1)',
	SH=>'HIGH CAPACITY MULTIPLEXING (DS3 TO DS1)',
	SI=>'SWITCHED FRACTIONALIZED OFFICE',
	SJ=>'HIGH CAPACITY MULTIPLEXING - TERMINUS (DS3 TO DS1)',
	SK=>'HIGH CAPACITY MULTIPLEXING - INTERMEDIATE (DS3 TO DS1)',
	SL=>'HIGH CAPACITY MULTIPLEXING - SUPER-INTERMEDIATE (DS3 TO DS1)',
	SM=>'HIGH CAPACITY MULTIPLEXING (DS2 TO DS1)',
	SN=>'HIGH CAPACITY MULTIPLEXING (DS1C TO DS1)',
	SP=>'DIGITAL DATA MULTIPLEXING (DS1 TO DIGITAL)',
	SQ=>'HIGH CAPACITY MULTIPLEXING (DS1 TO VOICE)',
	SR=>'HIGH CAPACITY MULTIPLEXING - INTERMEDIATE (DS1 TO VOICE)',
	SS=>'HIGH CAPACITY MULTIPLEXING - SUPER-INTERMEDIATE (DS1 TO VOICE)',
	ST=>'OPTICAL LINE TERMINATING MULTIPLEXER',
	SU=>'HIGH CAPACITY MUX - DS1 TO SYNCHRONOUS DIGITAL DATA/DATA OVER VOICE AND VOICE GRADE SERVICE',
	SV=>'HIGH CAPACITY MULTIPLEXING - TERMINUS (DS1 TO VOICE)',
	SW=>'HIGH CAPACITY MULTIPLEXING - INTERMEDIATE (DS1 TO DS0)',
	SX=>'DIGITAL DATA MULTIPLEXING (DS1 TO DIGITAL) - TERMINUS',
	SY=>'DIGITAL DATA MULTIPLEXING (DS1 TO DIGITAL) - INTERMEDIATE',
	T1=>'DS3 FRAME RELAY SERVICE - PACKET SWITCH',
	T2=>'DS3 FRAME RELAY SERVICE - ACCESS POINT',
	T3=>'DIGITAL SUBSCRIBER LINE (DSL) SPECIAL ACCESS CONNECTION POINT - INTERMEDIATE#',
	T4=>'DIGITAL SUBSCRIBER LINE (DSL) FRAME RELAY CONNECTION POINT',
	T5=>'DIGITAL SUBSCRIBER LINE (DSL) FRAME RELAY CONNECTION POINT - INTERMEDIATE',
	T6=>'DIGITAL SUBSCRIBER LINE (DSL) FRAME RELAY CONNECTION POINT - SUPER INTERMEDIATE',
	T7=>'DIGITAL SUBSCRIBER LINE (DSL) ASYNCHRONOUS TRANSFER MODE (ATM) CONNECTION POINT',
	T8=>'INTERNET DIAL ACCESS',
	T9=>'DIGITAL SUBSCRIBER LINE USING SYMMETRICAL BIT RATES (DATA ONLY)',
	TA=>'ACCESS CONCENTRATOR',
	TB=>'PACKET SWITCH',
	TC=>'FRAME RELAY SERVICE ACCESS POINT - ACCESS CONCENTRATOR',
	TD=>'DS1 & BELOW FRAME RELAY SERVICE - PACKET SWITCH',
	TE=>'PUBLIC DATA NETWORK ACCESS POINT - X.25 PROTOCOL',
	TF=>'PUBLIC DATA NETWORK ACCESS POINT - X.75 PROTOCOL',
	TG=>'SWITCHED MULTI-MEGABIT DATA SERVICE (SMDS)',
	TH=>'FRAME RELAY SERVICE ACCESS POINT - FRAME SWITCH',
	TI=>'DIGITAL SUBSCRIBER LINE USING INTEGRATED SERVICES DIGITAL NETWORK (ISDN)',
	TJ=>'DIGITAL SUBSCRIBER LINE USING ASYMMETRICAL BIT RATES',
	TK=>'DIGITAL SUBSCRIBER LINE USING SYMMETRICAL BIT RATES (DATA AND VOICE)',
	TL=>'MESSAGE WAITING INDICATOR - PACKET ACCESS',
	TM=>'FAST SELECT ACCEPTANCE - PACKET',
	TN=>'FAST SELECT REQUEST - PACKET',
	TO=>'MENU SERVER - PACKET',
	TP=>'CALL REDIRECTION - PACKET',
	TQ=>'HUNT GROUPS - PACKET',
	TR=>'CALL DETAIL RECORDING REPORTS (PACKET)',
	TS=>'CLOSED USER GROUPS - PACKET',
	TT=>'REVERSE CHARGE ACCEPTANCE - PACKET',
	TU=>'ASYNCHRONOUS TRANSFER MODE (ATM)',
	TW=>'DS1 & BELOW FRAME RELAY SERVICE - ACCESS POINT',
	TX=>'FRAME RELAY SERVICE-SUPER INTERMEDIATE',
	TY=>'FRAME RELAY SERVICE - INTERMEDIATE',
	TZ=>'DIGITAL SUBSCRIBER LINE (DSL) ETHERNET TRANSPORT CONNECTION POINT',
	UA=>'SPECIAL ACCESS FLAT-RATED DS1 TRANSPORT OVER FIBER',
	UB=>'SECONDARY CHANNEL CAPABILITY',
	UC=>'VIDEO HUB',
	UD=>'SPECIAL ACCESS OFFICE',
	UE=>'NETWORK RECONFIGURATION OFFICE - DS1 AND LOWER RATES',
	UF=>'VOICE GATEWAY OFFICE',
	UG=>'CLEAR CHANNEL CAPABILITY - DS1 WITH B8ZS',
	UH=>'DISASTER RECOVERY - HUB REDUNDANCY',
	UJ=>'DISASTER RECOVERY - SERVING WIRE CENTER AND FACILITY REDUNDANCY',
	UK=>'DISASTER RECOVERY - LOOP REDUNDANCY',
	UL=>'SYNCHRONOUS DIGITAL DATA SERVICE OFFICE (2-WIRE)',
	UM=>'SYNCHRONOUS DIGITAL DATA SERVICE OFFICE (4-WIRE)',
	UN=>'DATA OVER VOICE OFFICE',
	UP=>'56/64KBPS CLEAR CHANNEL DIGITAL DATA SERVICE',
	UQ=>'SELF-HEALING TRANSPORT NETWORK ACCESS NODE',
	UR=>'DS3 CAPABLE WIRE CENTER',
	US=>'FIBER CROSS-CONNECTION HUB',
	UT=>'AUTOMATIC PROTECTION SWITCHING',
	UU=>'ACCESS TO CLEAR CHANNEL TRANSMISSION',
	UV=>'TRANSPORT RESOURCE MANAGEMENT',
	UW=>'CONDITIONING',
	UX=>'EXTENDED SUPERFRAME CONDITIONING',
	UY=>'ROUTE DIVERSITY',
	UZ=>'SECONDARY CHANNEL CAPABILITY',
	VA=>'DIGITAL DATA SERVICE - SUBRATES:2.4-19.2 KBPS',
	VB=>'NETWORK RECONFIGURATION OFFICE - DS3 AND DS1',
	VC=>'SIMPLIFIED MESSAGE DESK INTERFACE (SMDI)',
	VD=>'SIMPLIFIED MESSAGE DESK INTERFACE-EXPANDED(SMDI-E)',
	VE=>'56/64 KBPS CLEAR CHANNEL DIGITAL DATA SERVICE-TERMINUS',
	VF=>'56/64 KBPS CLEAR CHANNEL DIGITAL DATA SERVICE - INTERMEDIATE',
	VG=>'SHARED HIGH CAPACITY NETWORK-INTERMEDIATE',
	VH=>'NETWORK RECONFIGURATION OFFICE - DS3 TO DS3',
	VI=>'NETWORK RECONFIGURATION OFFICE - DS3-DS1-DS0',
	VJ=>'ON-NET METROPOLITAN AREA MANAGED SERVICE - INTERMEDIATE',
	VK=>'OFF-NET METROPOLITAN AREA MANAGED SERVICE - INTERMEDIATE',
	VL=>'56/64 KBPS CLEAR CHANNEL DIGITAL DATA SERVICE -SUPER INTERMEDIATE',
	VM=>'FRACTIONAL T-1 DIGITAL DATA SERVICE - TERMINUS',
	VN=>'INTERSWITCH SIMPLIFIED MESSAGE DESK INTERFACE - ISMDI HOST OFFICE',
	VO=>'INTERSWITCH SIMPLIFIED MESSAGE DESK INTERFACE - ISMDI REMOTE OFFICE',
	VP=>'FRACTIONAL T-1 DIGITAL DATA SERVICE - INTERMEDIATE',
	VQ=>'FRACTIONAL T-1 DIGITAL DATA SERVICE - SUPER INTERMEDIATE',
	WA=>'DS3 HUB WITH REMOTE MANAGEMENT CAPABILITIES',
	WB=>'DS3 GROOMING HUB WITH REMOTE MANAGEMENT CAPABILITIES',
	WC=>'DS0/FRACTIONAL DS1 HUB WITH REMOTE MANAGEMENT CAPABILITIES',
	WD=>'SHARED SONET NETWORK ACCESS CONNECTION',
	WE=>'DS1 CAPABLE WIRE CENTER',
	WF=>'OC1 CAPABLE WIRE CENTER',
	WG=>'OC3 CAPABLE WIRE CENTER',
	WH=>'OC12 CAPABLE WIRE CENTER',
	WI=>'OC24 CAPABLE WIRE CENTER',
	WJ=>'OC48 CAPABLE WIRE CENTER',
	WK=>'OC96 CAPABLE WIRE CENTER',
	WL=>'OC192 CAPABLE WIRE CENTER',
	XA=>'DATA SESSION MANAGEMENT DIGITAL SUBSCRIBER LINE USING ASYMMETRICAL BIT RATES',
	XB=>'DATA SESSION MANAGEMENT DIGITAL SUBSCRIBER LINE USING SYMMETRICAL BIT RATES (DATA AND VOICE)',
	XC=>'DATA SESSION MANAGEMENT DIGITAL SUBSCRIBER LINE USING SYMMETRICAL BIT RATES (DATA ONLY)',
	XD=>'DIGITAL SUBSCRIBER LINE (DSL) DATA SESSION MANAGEMENT CONNECTION POINT',
	XE=>'DIGITAL SUBSCRIBER LINE (DSL) 10 MBPS ETHERNET CONNECTION POINT - INTERMEDIATE#',
	XF=>'DIGITAL SUBSCRIBER LINE (DSL) 100 MBPS ETHERNET CONNECTION POINT - INTERMEDIATE#',
	XG=>'DSL TRANSPORT HUB SERVING WIRE CENTER (SWC): - INTERMEDIATE#',
	ZW=>'DIGITAL DATA HUB - TERMINUS',
	ZX=>'DIGITAL DATA HUB - INTERMEDIATE',
	ZY=>'DIGITAL DATA HUB - SUPER-INTERMEDIATE',
	ZZ=>'DIGITAL DATA HUB',
};

print STDERR "writing $datadir/LERG7.DAT\n";
open($of,">","$datadir/LERG7.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG7.DAT
# ---------
# The LERG7 file contains information regarding switches.  Such switches are
# identified by an eleven-character code that, with few exceptions, should be a
# CLLI codes as is trademarked and supported by the COMMON LANGUAGE
# organization in Telcordia Technologies, Inc.  Although the field is termed a
# "switch," there may be multiple CLLI codes for a single physical switch for
# various reaons.  Such reasons can include the switch performing multiple
# functions (e.g. as an end office and as a tandem).  CLLIs may also reflect a
# "Point of Interface (POI)" established as the inteconnection point between
# two carriers.  Although POIs may, at times, be at the same location as an
# actual physical switch, they do not have to be.  All assigned CLLIs are not
# listed in the LERG -- only those that are relevant to accessing the local
# network and routing calls within the local network are included.  This file
# provides high-level switch information such as address, V&H coordinates,
# switch functionalities, equipment type, etc.
# ---------
# Switching Entity data (Key field 5)
# ---------
#   1-  5  5 LATA
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 43 11 Switch
#  44- 48  5 Equipment Type
#  49- 52  4 Administrative OCN (AOCN)
#  53- 53  1 filler
#  54- 57  4 Operating Company Number (OCN)
#  58- 62  5 Vertical Coordinate (VC)
#  63- 67  5 Horizontal Coordinate (HC)
#  68- 68  1 Intenational DDD (IDDD) (Y/N)
#  69-128 60 Street (Switch)
# 129-158 30 City (Switch)
# 159-160  2 State/Province/Territory/Country (Switch)
# 161-169  9 Zip/Postal Code
# 170-170  1 Point Code Flag
# 171-181 11 CLASS 4/5 Switch
# 182-182  1 Functionality 1 - END OFC
# 183-183  1 Functionality 2 - HOST
# 184-184  1 Functionality 3 - REMOTE
# 185-185  1 Functionality 4 - DA OFC
# 186-186  1 Functionality 5 - CLASS 4/5
# 187-187  1 Functionality 6 - WIRELESS OFC
# 188-188  1 Functionality 7 - FG D ADJ EO
# 189-189  1 Functionality 8 - filler
# 190-190  1 Functionality 9 - filler
# 191-191  1 Functionality 10 - filler
# 192-192  1 Functionality 11 - filler
# 193-193  1 Functionality 12 - FG B TDM
# 194-194  1 Functionality 13 - FG C TDM
# 195-195  1 Functionality 14 - FG D TDM
# 196-196  1 Functionality 15 - OS TDM
# 197-197  1 Functionality 16 - ITERMED OFC
# 198-198  1 Functionality 17 - DA TDM
# 199-199  1 Functionality 18 - 911 TDM
# 200-200  1 Functionality 19 - FG D AJD TDM
# 201-201  1 Functionality 20 - LOCAL TDM
# 202-202  1 Functionality 21 - INTRA TDM
# 203-203  1 Functionality 22 - CS DATA TDM
# 204-204  1 Functionality 23 - BCR5
# 205-205  1 Functionality 24 - BCR6
# 206-206  1 Functionality 25 - PRI 64
# 207-207  1 Functionality 26 - ISDN MULTIRT
# 208-208  1 Functionality 27 - ISDN FS OFC
# 209-209  1 Functionality 28 - X.75 GATEWAY
# 210-210  1 Functionality 29 - PACKET X.121
# 211-211  1 Functionality 30 - PACKET E.164
# 212-212  1 Functionality 31 - filler
# 213-213  1 Functionality 32 - filler
# 214-214  1 Functionality 33 - filler
# 215-215  1 Functionality 34 - STP
# 216-216  1 Functionality 35 - CCS AC OFC
# 217-217  1 Functionality 36 - filler
# 218-218  1 Functionality 37 - 800 SSP
# 219-219  1 Functionality 38 - LNP CAPABLE
# 220-220  1 Functionality 39 - filler
# 221-221  1 Functionality 40 - filler
# 222-222  1 Functionality 41 - filler
# 223-223  1 Functionality 42 - CIP
# 224-224  1 Functionality 43 - CSP
# 225-225  1 Functionality 44 - filler
# 226-226  1 Functionality 45 - SW 56
# 227-227  1 Functionality 46 - FGD 56
# 228-228  1 Functionality 47 - FGD 64
# 229-229  1 Functionality 48 - INTRA PRSUB
# 230-230  1 Functionality 49 - CALL AGENT
# 231-231  1 Functionality 50 - TRUNK GATEWAY
# 232-232  1 Functionality 51 - ACCESS GATEWAY
# 233-233  1 Functionality 52 - filler
# 234-234  1 Functionality 53 - filler
# 235-235  1 Functionality 54 - filler
# 236-236  1 Functionality 55 - filler
# 237-272 36 filler
# 273-278  6 Creation Date in BIRRDS (mmddyy)
# 279-279  1 filler
# 280-285  6 "E" STATUS date (mmddyy)
# 286-286  1 filler
# 287-292  6 Last Modification Date (mmddyy)
# 293-293  1 filler
# 294-294  1 CLN Indicator
# 295-297  3 Bill-to RAO
# 298-300  3 Send-to RAO
# 301-312 12 filler
# ---------
EOF
my $flag;
foreach my $key (sort keys %{$db->{clli}}) {
	my $clli = $db->{clli}{$key};
	my $name = $db->{lata}{$clli->{lata}}{name} if exists $db->{lata}{$clli->{lata}}{name};
	printf $of "%-5.5s", $clli->{lata};
	printf $of "%-20.20s", $name;
	printf $of "%-1.1s", ''; # Status
	printf $of "%-6.6s", ''; # Effective Date
	printf $of "%-11.11s", $clli->{clli};
	printf $of "%-5.5s", ''; # Equiment type
	printf $of "%-4.4s", ''; # Administrative OCN
	printf $of "%-1.1s", ''; # filler
	printf $of "%-4.4s", $clli->{cc}; # OCN
	printf $of "%-5.5s", $clli->{wcv};
	printf $of "%-5.5s", $clli->{wch};
	printf $of "%-1.1s", ''; # IDDD
	printf $of "%-60.60s", ''; # Street
	printf $of "%-30.30s", ''; # City
	printf $of "%-2.2s", $clli->{st};
	printf $of "%-9.9s", '';
	printf $of "%-1.1s", # Point Code Flag
		($clli->{feat}{HA} or $clli->{feat}{HB} or $clli->{feat}{HC} or $clli->{feat}{HI})
		? 'X' : '';
	printf $of "%-11.11s", ''; # Class 4/5 Switch
	printf $of "%-1.1s", # End OFC
		($clli->{feat}{DD})
		? 'X' : '';
	printf $of "%-1.1s", ''; # Host
	printf $of "%-1.1s", ''; # Remote
	printf $of "%-1.1s", # DA OFC
		($clli->{feat}{G1})
		? 'X' : '';
	printf $of "%-1.1s", ''; # CLASS 4/5
	printf $of "%-1.1s", ''; # WIRELESS OFC
	printf $of "%-1.1s", ''; # FG D ADJ EO
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # FG B TDM
		($clli->{feat}{GB} and ($clli->{feat}{BA} or $clli->{feat}{BC}))
		? 'X' : '';
	printf $of "%-1.1s", ''; # FG C TDM
		($clli->{feat}{GB} and ($clli->{feat}{CA} or $clli->{feat}{CC}))
		? 'X' : '';
	printf $of "%-1.1s", ''; # FG D TDM
		($clli->{feat}{GB} and ($clli->{feat}{DA} or $clli->{feat}{DC}))
		? 'X' : '';
	printf $of "%-1.1s", ''; # OS TDM
	printf $of "%-1.1s", ''; # INTERMED OFC
	printf $of "%-1.1s", ''; # DA TDM
	printf $of "%-1.1s", ''; # 911 TDM
	printf $of "%-1.1s", ''; # FG D ADJ TDM
	printf $of "%-1.1s", ''; # LOCAL TDM
	printf $of "%-1.1s", ''; # INTRA TDM
	printf $of "%-1.1s", ''; # CS DATA TDM
	printf $of "%-1.1s", ''; # BCR5
	printf $of "%-1.1s", ''; # BCR6
	printf $of "%-1.1s", ''; # PRI 64
	printf $of "%-1.1s", ''; # ISDN MULTIRT
	printf $of "%-1.1s", ''; # ISDN FS OFC
	printf $of "%-1.1s", ''; # X.75 GATEWAY
	printf $of "%-1.1s", ''; # PACKET X.121
	printf $of "%-1.1s", ''; # PACKET E.164
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # STP
	printf $of "%-1.1s", ''; # CCS AC OFC
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # 800 SSP
	printf $of "%-1.1s", ''; # LNP CAPABLE
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # CIP
	printf $of "%-1.1s", ''; # CSP
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # SW56
	printf $of "%-1.1s", ''; # FGD 56
	printf $of "%-1.1s", ''; # FGD 64
	printf $of "%-1.1s", ''; # INTRA PRSUB
	printf $of "%-1.1s", ''; # CALL AGENT
	printf $of "%-1.1s", # TRUNK GATEWAY
		($clli->{feat}{IP})
		? 'X' : '';
	printf $of "%-1.1s", ''; # ACCESS GATEWAY
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-36.36s", ''; # filler
	printf $of "%-6.6s", ''; # Creation Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # "E" STATUS Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # Last Modificaiton Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # CLN Indicator
	printf $of "%-3.3s", ''; # Bill-to RAO
	printf $of "%-3.3s", ''; # Send-to RAO
	printf $of "%-12.12s", ''; # filler
	print $of "\n";
}
close($of);

