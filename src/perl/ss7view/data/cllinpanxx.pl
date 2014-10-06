#!/usr/bin/perl -w

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @files = `ls */db.csv`;

$fn = "cllinpanxx.txt";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "E: cannot open $fn";
foreach $fn (@files) { chomp $fn;
	$fn =~ /(.*?)\//; my $dir = $1;
	print STDERR "I: processing $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $header = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i] if $tokens[$i]
		}
		my $clli;
		$clli = $data->{SWITCH_CLLI} if $data->{SWITCH_CLLI};
		$clli = $data->{SWITCH} if $data->{SWITCH};
		$clli = $data->{clli} if $data->{clli};
		$clli = $data->{'Wire Center CLLI'} if $data->{'Wire Center CLLI'};
		$clli = $data->{Switch} if $data->{Switch};
		next unless $clli;
		printf $of "$clli\t$data->{NPA}\t$data->{NXX}\n";
	}
}
close($of);

exit;

1;

__END__

==> areacodes/db.csv <==
"NPA","NXX","X","COUNTRY","STATE","CITY","COUNTY","LATITUDE","LONGITUDE","LATA","TIMEZONE","OBSERVES_DST","COUNTY_POPULATION","FIPS_COUNTY_CODE","MSA_COUNTY_CODE","PMSA_COUNTY_CODE","CBSA_COUNTY_CODE","ZIPCODE_POSTALCODE","ZIPCODE_COUNT","ZIPCODE_FREQUENCY","NEW_NPA","NXX_USE_TYPE","NXX_INTRO_VERSION","OVERLAY","OCN","COMPANY","RATE_CENTER","SWITCH_CLLI","RC_VERTICAL","RC_HORIZONTAL"

==> localcallingguide/db.csv <==
"NPA","NXX","X","EXCH","RC","RCSHORT","REGION","SEE-EXCH","SEE-RC","SEE-REGION","SWITCH","SWITCHNAME","SWITCHTYPE","LATA","OCN","COMPANY-NAME","COMPANY-TYPE","ILEC-OCN","ILEC-NAME","RC-V","RC-H","RC-LAT","RC-LON","EFFDATE","DISCDATE","UDATE"

==> nanpa/db.csv <==
"NPA","NXX","X","OCN","Company","Status","RateCenter","Remarks","State","EffectiveDate","Use","AssignDate","Initial/Growth"

==> neca4/db.csv <==
"NPA","NXX","X","rng","loc","state","clli","wcvh","ocn","lata","feat"

==> npanxxsource/db.csv <==
"NPA","NXX","X","NPA NXX Record","Line From","Line To","Wireless Block","NXX Type","Portable Block","1,000 Block Pooling","Block Contaminated","Block Retained","Date Assigned","Effective Date","Date Activated","Last Modified","Common Name","OCN","OCN Type","Carrier Name","Abbreviation","DBA","FKA","Carrier Address","Carrier City","Carrier State","Carrier Zip","Carrier Country","Wire Center LATA","Other switches in WC","Wire Center Address","Wire Center County","Wire Center City","Class 4/5 Switch","Wire Center State","Trunk Gateway","Point Code","Switch type","Tandem IntraLATA","Tandem Feature Group C","Tandem Feature Group D","Tandem Local","Tandem Feature Group B","Tandem Operator Services","Wire Center Zip","SS7 STP 1","Wire Center Google maps","Wire Center Latitude/Longitude","Wire Center V&H","Wire Center CLLI","Switch","SS7 STP 2","Actual Switch","Wire Center Country","Call Agent","Host","Wire Center Name","LATA","LATA Name","Historical Region","Area Codes in LATA","Remark","Rate Center LATA","Rate Center Name","Rate Center State","Rate Center Country","LERG Abbreviation","Zone","Zone Type","Number Pooling","Point Identifier","Rate Step","Area Codes in Rate Center","Embedded Overlays","Major V&H","Minor V&H","Rate Center Latitude/Longitude","Rate Center Google maps","Time Zone","DST Recognized","Location within Rate Center Type","Location within Rate Center Name","County or Equivalent","Federal Feature ID","FIPS County Code","FIPS Place Code","Land Area","Water Area","Population 2000","Population 2009","Rural-Urban Continuum Code","CBSA","CBSA Level","County Relation to CBSA","Metro Division","Part of CSA","Cellular Market","Major Trading Area","Basic Trading Area"

==> pinglo/db.csv <==
"NPA","NXX","X","Type","Carrier","Switch","Rate Center","State"

==> telcodata/db.csv <==
"NPA","NXX","X","State","Company","OCN","Rate Center","Switch","Date","Prefix Type","Switch Name","Switch Type","LATA","Tandem"
