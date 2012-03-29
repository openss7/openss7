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

open($fh,"<","$datadir/NECA-4-1.txt");

my $start = 0;

while (<$fh>) { chomp;
	unless ($start) {
		#print "IGNORED: $_\n";
		next unless /^\s+COMPANY CODES FOR/;
		$start = 1;
		next;
	}
	last if /^\s+END OF SECTION/;
	#next unless /^\s/;
	next unless /^\s/ or /^[0-9][0-9][0-9][A-Z0-9]\s/;
	next if /^\s*-\s*THE\s*COMPANY HAS/;
	next if /^\s*-\s*THE\s*LEGAL NAME/;
	next if /^\s*-\s*THE\s*ACQUIRED CODE/;
	next if /PRINTED IN U.S.A./;
	next if /^====/;
	next if /^\f/;
	next if /^DIRECTOR - ACCESS TARIFFS/;
	next if /^80 S. JEFFERSON/;
	next if /^WHIPPANY, N.J./;
	next if /^ISSUED: /;
	next if /WIRE CENTER AND INTERCONNECTION INFORMATION/;
	next if /COMPANY CODES FOR/;
	next if /^\s*CODE\s*$/;
	next if /^\s*$/;
	#print "USING: $_\n";
	$_ =~ s/\([CDSN]\)//g; $_ =~ s/^\s+//; $_ =~ s/\s+$//;
	next if $_ eq '';
	if (/^\s*([0-9][0-9][0-9][A-Z0-9])\s+(.*?)\s+\(OVERALL\)\s*$/) {
		my ($ocn,$cpn) = ($1,$2);
		$db->{$ocn}{ocn} = $ocn;
		$db->{$ocn}{companyname} = $cpn;
		$db->{$ocn}{overall} = 1;
		#print "USING: ocn=>$ocn,companyname=>$cpn\n";
		next;
	} elsif (/^\s*([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+([YN])\s+([A-Z]+)\s+(.*?)\s*$/) {
		my ($tar,$ocn,$mem,$cat,$cpn) = ($1,$2,$3,$4,$5);
		$db->{$ocn}{ocn} = $ocn;
		$db->{$ocn}{member} = $mem;
		$db->{$ocn}{category} = $cat;
		$db->{$ocn}{companyname} = $cpn;
		#print "USING: ocn=>$ocn,companyname=>$cpn,member=>$mem,catetory=>$cat\n";
		next;
	} else {
		warn "cannot parse line '$_'";
	}
}
close($fh);

open($fh,"<","$datadir/NECA-4-1.txt");

my @lines1 = (); my $wasblank1 = 1;
my @lines2 = (); my $wasblank2 = 1;
my @holdln = ();

$start = 0;
my $holding = 0;
#my $wasblank = 1;

while (<$fh>) { chomp;
	unless ($start) {
		#print "IGNORED: $_\n";
		next unless /^\s+ISSUING CARRIERS/;
		$start = 1;
		next;
	}
	#if (/^$/) {
	#	next if $wasblank;
	#	$wasblank = 1;
	#	print "\n";
	#	next;
	#}
	last if /^\s+END OF SECTION/;
	next if /THE CARRIERS ON WHOSE/;
	next if /THERE ARE ANY CONCURRING/;
	next if /TARIFF, THEY ARE NOT/;
	next if /PRINTED IN U.S.A./;
	next if /^====/;
	next if /^\f/;
	next if /^DIRECTOR - ACCESS TARIFFS/;
	next if /^80 S. JEFFERSON/;
	next if /^WHIPPANY, N.J./;
	next if /^ISSUED: /;
	next if /WIRE CENTER AND INTERCONNECTION INFORMATION/;
	next if /ISSUING CARRIERS/;
	#$wasblank = 0;
	#print "$_\n";
	my $line1 = substr($_,0,40);
	$line1 =~ s/\([CDSN]\)//g;
	$line1 =~ s/^\s+//; $line1 =~ s/\s+$//;
	$line1 =~ s/  +/ /g;
	my $line2 = substr($_,41);
	$line2 =~ s/\([CDSN]\)//g;
	$line2 =~ s/^\s+//; $line2 =~ s/\s+$//;
	$line2 =~ s/  +/ /g;
	if ($line1 eq '') {
		unless ($wasblank1) {
			$wasblank1 = 1;
			if (@holdln) {
				push @lines1, @holdln;
				@holdln = ();
			}
			push @lines1, $line1;
		}
#	} elsif ($line1 eq "-cont'd-") {
#		if ($wasblank1) {
#			$wasblank1 = 0;
#			pop @lines1;
#		}
	} else {
		$wasblank1 = 0;
		push @lines1, $line1;
	}
	if ($line2 eq '') {
		if ($holding) {
			$holding = 0;
			next;
		}
		unless ($wasblank2) {
			$wasblank2 = 2;
			push @lines2, $line2;
		}
	} elsif ($line2 eq "-cont'd-") {
		$holding = 1;
		next;
	} else {
		if ($holding) {
			push @holdln, $line2;
			next;
		}
		$wasblank2 = 0;
		push @lines2, $line2;
	}
}

close($fh);

my %states = (
	'ALABAMA'=>'AL',
	'ALASKA'=>'AK',
	'ARIZONA'=>'AZ',
	'ARKANSAS'=>'AR',
	'CALIFORNIA'=>'CA',
	'COLORADO'=>'CO',
	'CONNECTICUT'=>'CT',
	'DELAWARE'=>'DE',
	'DISTRICT OF COLUMBIA'=>'DC',
	'FLORIDA'=>'FL',
	'GEORGIA'=>'GA',
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
	'WASHINGTON'=>'WA',
	'WEST VIRGINIA'=>'WV',
	'WISCONSIN'=>'WI',
	'WYOMING'=>'WY',
);
my @lines;

@lines = (@lines1, '', @lines2);
my $data = {};
my $lno = 0;
my $continue = 0;
while (@lines) {
	$_ = shift @lines;
	#printf STDERR "line %d: %s\n", $lno, $_;
	if (/^$/) {
		if ($lno) {
			foreach my $key (keys %{$data}) {
				$db->{$data->{ocn}}{$key} = $data->{$key};
			}
			$data = {};
			$lno = 0;
		}
		next;
	}
	if ($lno == 0) {
		if (/^([0-9][0-9][0-9][A-Z0-9])$/) {
			$data->{ocn} = $1;
			#$data->{overall} = 0;
			$lno++;
			next;
		} elsif (/^\(([0-9][0-9][0-9][A-Z0-9])\)$/) {
			$data->{ocn} = $1;
			$data->{overall} = 1;
			$lno++;
			next;
		}
		die "cannot find ocn in line '$_'";
	} elsif ($lno == 1) {
		$data->{name} = $_;
		$lno++;
		next;
	} elsif ($lno == 2) {
		$data->{title} = $_;
		$lno++;
		next;
	} elsif ($lno == 3) {
		if (/PO BOX ADDRESS/) {
			# missing title
			$data->{org} = delete $data->{title};
			$lno = 8;
			next;
		} elsif (/^%/) {
			# missing title
			$data->{org} = delete $data->{title};
			$data->{careof} = $_;
			$data->{careof} =~ s/^%//;
			$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
			$lno += 2;
			next;
		} else {
			$data->{org} = $_;
			$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
			$lno++;
			next;
		}
	} elsif ($lno == 4) {
		if (/^%/) {
			$data->{careof} = $_;
			$data->{careof} =~ s/^%//;
			$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
			next;
		}
		if ($continue
		    or /^Overall$/
		    or /Inc/
		    or /^Communications/
		    or /^Adak/
		    or /Cooperative/
		    or /Authority/
		    or /Telecommunications/
	            or /dba/
		    or /^- /
		    or /LLC/
		    or /L\.P\./
		    or /\bCo\b/
		    or /COTC Connections/
		    or /Corp/
		    or /Company/
		    or /Services/
		    or /Telecom/
		    or /Telcom/
		    or /Fulton/
		    or /Coop/
		    or /Dept/
		    or /Communications/
		    or /\(Contel\)/
		    or /\(Gte\)/
		    or /\(Quaker St\)/
		    or /^Springs$/
		    or /^Jamestown$/
		    or /^Red Jacket$/
		    or /^[A-Z][A-Z]$/
		    or /^- *[A-Z][A-Z]$/
		)
		{
			if (exists $data->{careof}) {
				$data->{careof} .= " $_";
				$data->{careof} =~ s/^%//;
			} else {
				$data->{org} .= " $_";
			}
			$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
			next;
		}
		if (/PO BOX ADDRESS/) {
			$lno = 8;
			next;
		}
		$data->{street} = $_;
		$lno++;
		next;
	} elsif ($lno == 5) {
		if (/^USA$/) {
			# missing {title}
			$data->{country} = $_;
			if (exists $data->{careof}) {
				$data->{city} = delete $data->{careof};
				$data->{careof} = delete $data->{org};
				$data->{org}  = delete $data->{title};
			} else {
				$data->{city} = delete $data->{org};
				$data->{org}  = delete $data->{title};
			}
			$lno += 2;
			next;
		}
		if (/\b(Mailstop|Route|Suite|Ste|Box|Floor)\b/) {
			$data->{suite} = $_;
			next;
		}
		$data->{city} = $_;
		$lno++;
		next;
	} elsif ($lno == 6) {
		$data->{country} = $_;
		$lno++;
		next;
	} elsif ($lno == 7) {
		if (/PO BOX ADDRESS/) {
			$lno++;
			next;
		}
		if (/FOR THE STATES OF:/) {
			$lno = 12;
			next;
		}
		$lno = 11;
		next;
	} elsif ($lno == 8) {
		$data->{pobox} = $_;
		$lno++;
		next;
	} elsif ($lno == 9) {
		$data->{pocity} = $_;
		$lno++;
		next;
	} elsif ($lno == 10) {
		$data->{pocountry} = $_;
		$lno++;
		next;
	} elsif ($lno == 11) {
		if (/FOR THE STATES OF:/) {
			$lno++;
			next;
		}
		warn "excess lines '$_'";
		next;
	} elsif ($lno == 12) {
		if (/^([A-Z].*?)\(([0-9][0-9][0-9][A-Z0-9])\)$/) {
			push @{$data->{states}}, {st=>$states{$1},ocn=>$2};
			my $ocn = $2;
			$db->{$ocn}{ocn} = $ocn;
			$db->{$ocn}{oocn} = $data->{ocn};
			$db->{$ocn}{st} = $states{$1};
			foreach my $key (qw/companyname name title org careof street country city suite pobox pocity pocountry/) {
				$db->{$ocn}{$key} = $data->{$key} if $data->{$key} and !$db->{$ocn}{$key};
			}
			next;
		}
		warn "cannot parse state '$_'";
		next;
	}
}
if ($lno) {
	foreach my $key (keys %{$data}) {
		$db->{$data->{ocn}}{$key} = $data->{$key};
	}
	$data = {};
	$lno = 0;
}

my $of = \*OUTFILE;
open($of,">","$datadir/ocns.pm");
my $dumper = Data::Dumper->new([$db]);
print $of $dumper->Dump;
close($of);

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

open($of,">","$datadir/LERG1.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG1.DAT
# ---------
# Telcordia LERGINFO.DOC says: "The LERG1 file provides high-level information
# about Operating Company Numbers (OCNs) such as the name of, and type of,
# company.  This includes contact information (for purposes of LERG data) as
# has been provided to TRA.  Note that contact information is maintained as
# best as possible; however, due to constant changes in personnel in a company,
# reorganizations, mergers, acquisitions, etc., some information will not be up
# to date.  In addition, the contact noted may not necessarily be familiar with
# all aspects of a company's operation (network, billing, etc.).  The contact
# name defaults to the party requesting the assignment of the number used as
# the OCN unless the company has specifically requested another party be listed
# for purposes of addressing LERG data.
#
# Note that the maintenance of company names are the purview of the company
# itself.  Although a company may change its name for various reasons (e.g.
# mergers and acquisitions), at times, the name used at the consumer level may
# not be the legal name of the company.  If you have any questions regarding a
# company's name as listed in LERG1, please contact the company directly to
# either receive an explanation or to prompt the company to have the data
# changed through the proper channels."
# ---------
# You will never find any contact information (personal identifying
# information) in this file.  If you wish to contact a responsible person,
# duckduckgo the operating company name.  If you would like to correct
# information contained in this file, use the ss7view utility to submit your
# corrections to the OpenSS7 project.  Also remember to submit changes to NECA
# or Telcordia as necessary to change other datasets.
# ---------
# OCN/Company Names (Key field 1)
# ---------
#   1-  4  4 Operating Company Number (OCN)
#   4- 54 50 Operating Company Name
#  55- 74 20 Abbreviated OCN Name
#  75- 76  2 State/Province/Territory/Country of OCN
#  77- 86 10 Category
#            CAP        - competitive access provider
#            CLEC       - competitive local exchange carrier
#            GENERAL    - various miscellaneous cases
#            INTL       - international
#            L RESELLER - local reseller
#            IC         - interexchange carrier
#            ILEC       - incumbent local exchange carrier
#            IPES       - internet protocol enabled services
#            P RESELLER - peronsal communication services reseller
#            PCS        - personal communications service
#            RBOC       - regional Bell operating company
#            ULEC       - unbundled local exchange carrier
#            WIRELESS   - wireless provided (non-PCS cellular, paging, radio)
#            W RESELLER - wireless reseller
#  87- 90  4 Overall OCN
#  91- 95  5 filler
#  96- 96  1 Last Known Address indicatory (X=reported address supposedly invalid)
#  97-116 20 Last Name
# 117-126 10 First Name
# 127-127  1 Middle Initial
# 128-177 50 Company Name (contact specific)
# 178-207 30 Title
# 208-237 30 Address 1
# 238-267 30 Address 2
# 268-277 10 Floor
# 278-297 20 Room
# 298-317 20 City
# 318-319  2 State/Province/Territory/Country
# 320-328  9 Zip/Postal Code
# 329-340 12 Phone
# 341-350 10 filler
# ---------
EOF
foreach my $key (sort keys %{$db}) {
	my $ocn = $db->{$key};
	printf $of "%-4.4s", $ocn->{ocn};
	printf $of "%-50.50s", $ocn->{companyname};
	printf $of "%-20.20s", '';
	my $cp = $ocn->{companyname};
	my $st = $ocn->{st};
	unless ($st) {
		if ($cp =~ /^(.*?) *- *([A-Z][A-Z])$/) {
			$st = $2;
		} elsif ($cp =~ / *- *([A-Z][A-Z]) *- */) {
			$st = $1;
		} elsif ($cp =~ /  *([A-Z][A-Z])$/) {
			$st = $1;
			$cp =~ s/  *[A-Z][A-Z]$//;
		} elsif ($cp =~ / ([A-Z][A-Z])-CCO/) {
			$cp = $1;
		} elsif ($cp =~ / *- *([A-Z][A-Z])  *[A-Z]+$/) {
			$cp = $1;
		} else {
			my $CP = "\U$cp\E";
			foreach my $tok (keys %states) {
				if (index($CP,$tok) >= 0) {
					$st = $states{$tok};
					last;
				}
			}
		}
	}
	my ($city,$state,$zip);
	$city = $ocn->{city};
	$city = $ocn->{pocity} unless $city;
	if ($city =~ /^(.*?) ([A-Z][A-Z]) ([0-9-]+)$/) {
		$city = $1;
		$state = $2;
		$zip = $3;
		$zip =~ s/-//g;
	}
	unless ($st) {
		$st = $state;
	}
	printf $of "%-2.2s", $st;
	printf $of "%-10.10s", $ocn->{category};
	printf $of "%-4.4s", $ocn->{oocn};
	printf $of "%-5.5s", '';
	printf $of "%-1.1s", '';
	my $name = $ocn->{name};
	$name =~ s/^Mr\.? //;
	$name =~ s/^Mrs\.? //;
	$name =~ s/^Ms\.? //;
	$name =~ s/ Jr\.?$//;
	$name =~ s/ Sr\.?$//;
	$name =~ s/ II$//;
	$name =~ s/ III$//;
	my @names = split(/  */,$name);
	printf $of "%-20.20s", $names[-1];
	printf $of "%-10.10s", $names[0];
	if (@names > 2) {
		printf $of "%-1.1s", substr($names[1],0,1);
	} else {
		printf $of "%-1.1s", '';
	}
	printf $of "%-50.50s", $ocn->{careof} ? $ocn->{careof} : $ocn->{org};
	printf $of "%-30.30s", $ocn->{title};
	if ($ocn->{street}) {
		printf $of "%-30.30s", $ocn->{street};
		printf $of "%-30.30s", '';
		printf $of "%-10.10s", ($ocn->{suite} =~ /Floor|Flr/) ? $ocn->{suite} : '';
		printf $of "%-20.20s", ($ocn->{suite} !~ /Floor|Flr/) ? $ocn->{suite} : '';
		printf $of "%-20.20s", $city;
	} else {
		printf $of "%-30.30s", $ocn->{pobox};
		printf $of "%-30.30s", '';
		printf $of "%-10.10s", '';
		printf $of "%-20.20s", '';
		printf $of "%-20.20s", $city;
	}
	printf $of "%-2.2s", $state;
	printf $of "%-9.9s", $zip;
	printf $of "%-12.12s", '';
	printf $of "%-10.10s", '';
	print $of "\n";
}
close($of);
