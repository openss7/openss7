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

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @ocn_keys = (
	'OCN',
	'company',
	'overall',
	'neca',
	'trgt',
	'oocn',
	'st',
	'companyname',
	'name',
	'title',
	'org',
	'careof',
	'street',
	'country',
	'city',
	'suite',
	'pobox',
	'pocity',
	'pocountry',
);

my %ocns = ();

sub closerecord {
	my $data = shift;
	if (my $ocn = $data->{ocn}) {
		$ocns{$ocn} = {} unless exists $ocns{$ocn};
		my $rec = $ocns{$ocn};
		$rec->{OCN} = $ocn;
		for (my $i=1;$i<@ocn_keys;$i++) {
			my $k = $ocn_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
}


$fn = "$datadir/NECA-4-1.txt";
open($fh,"<",$fn) or die "can't open $fn";
my $start = 0;
while (<$fh>) { chomp;
	my $data = {};
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
	if (/^\s*([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+(.*?)\s+\(OVERALL\)\s*$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{company} = $3;
		$data->{overall} = 1;
		closerecord($data);
	} elsif (/^\s*([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+([YN])\s+([A-Z]+)\s+(.*?)\s*$/) {
		$data->{trgt} = $1 if $1;
		$data->{ocn} = $2;
		$data->{neca} = $3;
		$data->{category} = $4;
		$data->{company} = $5;
		closerecord($data);
	} else {
		warn "cannot parse line '$_'";
	}
}
close($fh);

$fn = "$datadir/NECA-4-1.txt";
open($fh,"<",$fn) or die "can't open $fn";
my @lines1 = (); my $wasblank1 = 1;
my @lines2 = (); my $wasblank2 = 1;
my @holdln = ();
$start = 0;
my $holding = 0;
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
my @lines = (@lines1, '', @lines2);
my $data = {};
my $lno = 0;
my $continue = 0;
while (@lines) {
	$_ = shift @lines;
	#printf STDERR "line %d: %s\n", $lno, $_;
	if (/^$/) {
		if ($lno) {
			closerecord($data);
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
			$data->{oocn} = $data->{ocn};
			$data->{ocn} = $ocn;
			$data->{st} = $states{$1};
			next;
		}
		warn "cannot parse state '$_'";
		next;
	}
}
if ($lno) {
	closerecord($data);
	$lno = 0;
}

$fn = "$datadir/ocn.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@ocn_keys), '"', "\n";
foreach my $k (sort keys %ocns) {
	my $ocn = $ocns{$k};
	my @values = ();
	foreach (@ocn_keys) { push @values, $ocn->{$_} }
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

