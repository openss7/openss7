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

foreach my $file (qw/AR-Jonesboro-EO.csv AR-Littlerock-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 2;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'LOCATION'};
	$data->{host}   = delete $data->{'HOST'};
	delete $data->{host} if $data->{host} eq 'N/A';
	$data->{pcs}    = delete $data->{'Point Codes'};
	delete $data->{pcs} if $data->{pcs} =~ /see host p.c./i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'TYPE'};
	$data->{street} = delete $data->{'ADDRESS'};
	$data->{city}   = delete $data->{'CITY'};
	$data->{zip}    = delete $data->{'ZIP'};
	$data->{st}     = 'AR';
	$data->{lata}   = delete $data->{'LATA'};
	$data->{name}   = delete $data->{'LATA_NAME'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
foreach (values %{$db->{clli}}) {
	if ($_->{host} and exists $db->{locs}{$_->{host}}) {
		$_->{host} = $db->{locs}{$_->{host}}{clli};
	}
}
delete $db->{locs};
}

foreach my $file (qw/CA-Lata722-EO.csv CA-Lata726-EO.csv CA-Lata728-EO.csv CA-Lata730-EO.csv CA-Lata736-EO.csv CA-Lata740-EO.csv/) {
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	next unless /\t[A-Z0-9]{11}\t/;
	next unless /^[A-Z]/;
	my $data = {};
	my @tokens = split(/\t/,$_);
	$data->{region} = $tokens[0];
	$data->{area}   = $tokens[1];
	$data->{lata}   = $tokens[2];
	$data->{clli}   = $tokens[3];
	$data->{kind}   = $tokens[4] if $tokens[4];
	$data->{type}   = $tokens[5];
	$data->{cat}    = $tokens[6] if $tokens[6];
	$data->{aka}    = $tokens[7] if $tokens[7];
	$data->{pcs}    = "$tokens[8]-$tokens[9]-$tokens[10]";
	$data->{pair}   = $tokens[11];
	$data->{npa}    = $tokens[12];
	$data->{remark} = $tokens[13] unless $tokens[13] =~ /^\s*$/;
	$data->{tandem}{access} = $tokens[14] if $tokens[14];
	$data->{tandem}{local}  = $tokens[15] if $tokens[15];
	$data->{cc}     = $tokens[16];
	$data->{wcv}    = $tokens[17];
	$data->{wch}    = $tokens[18];
	$data->{street} = $tokens[19];
	$data->{city}	= $tokens[20];
	$data->{st}     = substr($tokens[21],0,2);
	$data->{zip}    = substr($tokens[21],2);
	$data->{other}  = $tokens[22] unless $tokens[22] =~ /^\s*$/;
	$db->{clli}{$data->{clli}} = $data;
}
close($fh);
}

foreach my $file (qw/CA-Lata722-EO.csv CA-Lata724-EO.csv CA-Lata726-EO.csv CA-Lata728-EO.csv CA-Lata730-EO.csv CA-Lata732-EO.csv CA-Lata734-EO.csv CA-Lata736-EO.csv CA-Lata738-EO.csv CA-Lata740-EO.csv/) {
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	next unless /\t[A-Z0-9]{11}\t/;
	next unless /^[A-Z]/;
	my $data = {};
	my @tokens = split(/\t/,$_);
	$data->{region} = $tokens[0];
	$data->{area}   = $tokens[1];
	$data->{lata}   = $tokens[2];
	$data->{clli}   = $tokens[3];
	$data->{kind}   = $tokens[4] if $tokens[4];
	$data->{type}   = $tokens[5];
	$data->{cat}    = $tokens[6] if $tokens[6];
	$data->{aka}    = $tokens[7] if $tokens[7];
	$data->{pcs}    = "$tokens[8]-$tokens[9]-$tokens[10]";
	$data->{pair}   = $tokens[11];
	$data->{npa}    = $tokens[12];
	$data->{remark} = $tokens[13] unless $tokens[13] =~ /^\s*$/;
	$data->{tandem}{access} = $tokens[14] if $tokens[14];
	$data->{tandem}{local}  = $tokens[15] if $tokens[15];
	$data->{cc}     = $tokens[16];
	$data->{wcv}    = $tokens[17];
	$data->{wch}    = $tokens[18];
	$data->{street} = $tokens[19];
	$data->{city}	= $tokens[20];
	$data->{st}     = substr($tokens[21],0,2);
	$data->{zip}    = substr($tokens[21],2);
	$data->{other}  = $tokens[22] unless $tokens[22] =~ /^\s*$/;
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/CT-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	next unless $data->{'Office Location'};
	$data->{loc}            = delete $data->{'Office Location'};
	$data->{host}           = delete $data->{'Host'};
	$data->{clli}           = delete $data->{'CLLI'};
	$data->{pcs}            = delete $data->{'POINT CODE'};
	$data->{type}           = delete $data->{'TYPE'};
	$data->{wcv}            = delete $data->{'V'};
	$data->{wch}            = delete $data->{'H'};
	$data->{tandem}{access} = delete $data->{'FGD Tandem'};
	$data->{npa}            = delete $data->{'HNPA'};
	$data->{street}         = delete $data->{'Address'};
	if ($data->{street} =~ /^(.*?),\s+([^,]*?),\s+([A-Z]{2})\s+([0-9]{5})$/) {
		$data->{street} = $1;
		$data->{city}   = $2;
		$data->{st}     = $3;
		$data->{zip}    = $4;
	}
	$data->{loc} =~ s/\s+\(Tandem Only\)\s*$//;
	$data->{loc} =~ s/\s+\(Remote\)\s*$//;
	$data->{loc} =~ s/\s*,\s*/ /g;
	$data->{loc} =~ s/\s*\.\s*/ /g;
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

open($fh,"<","$datadir/KS-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

foreach my $file (qw/IL-EO.csv IN-EO.csv MI-EO.csv OH-EO.csv WI-EO.csv/) {
my $heading = 1;
my @fields = ();
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{st}   = delete $data->{'STATE'};
	$data->{lata} = delete $data->{'LATA'};
	$data->{clli} = delete $data->{'END OFFICE CLLI'};
	$data->{deot} = delete $data->{'DEOT MODEL PRIORITY'};
	$data->{loc}  = delete $data->{'ENGLISH NAME'};
	$data->{host} = delete $data->{'HOST CLLI'};
	$data->{sort} = delete $data->{'SORT CLLI'};
	$data->{tandem}{access} = delete $data->{'CURRENT / PLANNED INTERLATA TANDEM CLLI'};
	$data->{tandem}{local}  = delete $data->{'CURRENT / PLANNED LOCAL/INTRALATA TANDEM CLLI'};
	$data->{tandem}{osda}   = delete $data->{'CURRENT / PLANNED OS/DA TANDEM CLLI'};
	$data->{tandem}{e911}   = delete $data->{'CURRENT E-911 SELECTIVE ROUTER CLLI'};
	$data->{stp1}           = delete $data->{'SIGNAL TRANSFER POINT CLLI 1'};
	$data->{stp2}           = delete $data->{'SIGNAL TRANSFER POINT CLLI 2'};
	$data->{status}         = delete $data->{'END OFFICE CLLI STATUS'};
	$data->{func}           = delete $data->{'NETWORK FUNCTION'};
	$data->{pcs}            = delete $data->{'SS7 POINT CODE'};
	$data->{type}           = delete $data->{'SWITCH TYPE'};
	$data->{effdate}        = delete $data->{'IN SERVICE DATE'};
	$data->{edate}          = delete $data->{'TERMINATION DATE'};
	$data->{newclli}        = delete $data->{'REPLACEMENT CLLI'};
	$data->{wcclli}         = delete $data->{'WIRE CENTER CLLI'};
	$data->{geo}            = delete $data->{'GEO LOC'};
	$data->{bdclli}         = delete $data->{'BUILDING CLLI'};
	$data->{wcv}            = delete $data->{'V COORDINATE'};
	$data->{wch}            = delete $data->{'H COORDINATE'};
	$data->{street}         = delete $data->{'STREET ADDRESS'};
	$data->{city}           = delete $data->{'CITY'};
	$data->{county}         = delete $data->{'COUNTY'};
	$data->{zip}            = delete $data->{'ZIP CODE'};
	$data->{zone}           = delete $data->{'RATE DISTRICT'};
	$data->{rc1}            = delete $data->{'RATE CENTER 1'};
	$data->{rc2}            = delete $data->{'RATE CENTER 2'};
	$data->{rc3}            = delete $data->{'RATE CENTER 3'};
	$data->{tl105}          = delete $data->{'TESTLINE 105'};
	$data->{tl108}          = delete $data->{'TESTLINE 108'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/KS-EO.csv MO-KansasCity-EO.csv MO-Springfield-EO.csv MO-StLouis-EO.csv/) {
my $heading = 1;
my @fields = ();
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	delete $data->{'HOST OFFICE NAME'};
	$data->{loc}   = delete $data->{'REMOTE OFFICE NAME'};
	$data->{clli}  = delete $data->{'CLLI'};
	$data->{type}  = delete $data->{'TYPE'};
	$data->{pcs}   = delete $data->{'POINTCODE'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}   = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{kind}  = delete $data->{'Host / Remote / ISDN Remote / TDM / STP '};
	$data->{lata}  = delete $data->{'LATA'};
	$data->{class} = delete $data->{'CLASS'};
	delete $data->{'Address'};
	$data->{tandem}{intra}  = delete $data->{'Intralata / Local Tandem'};
	$data->{tandem}{access} = delete $data->{'Interlata Tandem'};
	delete $data->{''};
	delete $data->{''};
	delete $data->{''};
	$data->{tandem}{local} = delete $data->{'TP&E Notes and / or Local Tandem CLLI'};
	delete $data->{tandem}{local} unless $data->{tandem}{local} and $data->{tandem}{local} ne 'No Local Tandem';
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/OK-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	delete $data->{'OFFICE NAME'};
	$data->{clli}   = delete $data->{'SW CLLI'};
	$data->{host}   = delete $data->{'HOST'};
	delete $data->{host} if $data->{host} eq 'NA';
	$data->{pcs}    = delete $data->{'PT CODE'};
	delete $data->{pcs} if $data->{pcs} =~ /see host/i;
	$data->{pcs}    = s/ /-/g if $data->{pcs};
	$data->{type}   = delete $data->{'SW TYPE'};
	$data->{tandem}{local}  = delete $data->{'Local Tandem'};
	$data->{tandem}{access} = delete $data->{'Access Tandem'};
	$data->{street} = delete $data->{'Address'};
	if ($data->{street} =~ /^(.*?)\s+\@\s+\([0-9]{5}\)\s*$/) {
		$data->{street} = $1;
		$data->{zip}    = $2;
	}
	$data->{st}     = 'OK';
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/OK-Tulsa-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'SW CLLI'};
	$data->{loc}    = delete $data->{'Location'};
	$data->{host}   = delete $data->{'Host'};
	delete $data->{host} if $data->{host} eq 'NA';
	$data->{tandem}{local}  = delete $data->{'Local TDM'};
	delete $data->{tandem}{local} if $data->{tandem}{local} eq 'NA';
	$data->{tandem}{access} = delete $data->{'IAL/IEL TDM'};
	$data->{pcs}    = delete $data->{'Point Code'};
	delete $data->{pcs} if $data->{pcs} =~ /see host/i;
	$data->{type}   = delete $data->{'Type'};
	$data->{street} = delete $data->{'Address'};
	$data->{st}     = 'OK';
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Abilene-EO.csv TX-Amarillo-EO.csv TX-Austin-EO.csv/) {
my $heading = 3;
my @fields = ();
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{access} = delete $data->{'Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City  ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Beaumont-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}  = delete $data->{'Local Tandem'};
	$data->{tandem}{intra}  = delete $data->{'IAL Tandem'};
	$data->{tandem}{inter}  = delete $data->{'IEL Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City Zip'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{tl105}  = delete $data->{'105 Test'};
	$data->{tl108}  = delete $data->{'108 Test'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Brownsville-EO.csv TX-CorpusChristi-EO.csv/) {
my $heading = 1;
my @fields = ();
open($fh,"<","$datadir/$file");
while (<$fh>) { chomp;
	if ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'LOCATION'};
	$data->{host}   = delete $data->{'HOST'};
	$data->{pcs}    = delete $data->{'Point Codes'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'TYPE'};
	$data->{street} = delete $data->{'ADDRESS'};
	$data->{zip}    = delete $data->{'ZIP'};
	$data->{st}     = 'TX';
	$data->{lata}   = delete $data->{'LATA '};
	$data->{name}   = delete $data->{'LATA NAME'};
	$data->{tandem}{local}  = delete $data->{'LOC TAND'};
	$data->{tandem}{access} = delete $data->{'IXC/IAL TAND'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Dallas-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}  = delete $data->{'Local Tandem'};
	$data->{tandem}{intra}  = delete $data->{'IAL Tandem'};
	$data->{tandem}{access} = delete $data->{'FGD Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-ElPaso-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	delete $data->{pcs} if $data->{pcs} =~ /SEE HOST PC/i;
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City  ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-FortSmith-EO.csv TX-Fayetteville-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 2;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'LOCATION'};
	$data->{host}   = delete $data->{'HOST'};
	delete $data->{host} if $data->{host} eq 'N/A';
	$data->{pcs}    = delete $data->{'Point Codes'};
	delete $data->{pcs} if $data->{pcs} eq 'SEE HOST PC';
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'TYPE'};
	$data->{street} = delete $data->{'ADDRESS'};
	$data->{city}   = delete $data->{'CITY'};
	$data->{st}     = 'TX';
	$data->{zip}    = delete $data->{'ZIP'};
	$data->{lata}   = delete $data->{'LATA'};
	$data->{name}   = delete $data->{'LATA_NAME'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-FtWorth-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}     = delete $data->{'Local Tandem'};
	$data->{tandem}{intra} = delete $data->{'IAL Tandem'};
	$data->{tandem}{access}    = delete $data->{'FGD Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Houston-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}     = delete $data->{'Local Tandem'};
	$data->{tandem}{intra} = delete $data->{'IAL Tandem'};
	$data->{tandem}{interlata} = delete $data->{'IEL Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City Zip'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{tl105}  = delete $data->{'105 Test'};
	$data->{tl108}  = delete $data->{'108 Test'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Longview-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}     = delete $data->{'Local Tandem'};
	$data->{tandem}{intra} = delete $data->{'IAL Tandem'};
	$data->{tandem}{access}    = delete $data->{'FGD Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Lubbock-EO.csv TX-Midland-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City  ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-PineBluf-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 2;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'LOCATION'};
	$data->{host}   = delete $data->{'HOST'};
	delete $data->{host} if $data->{host} eq 'N/A';
	$data->{pcs}    = delete $data->{'Point Codes'};
	delete $data->{pcs}  if $data->{pcs}  eq 'see host PC';
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'TYPE'};
	$data->{street} = delete $data->{'ADDRESS'};
	$data->{city}   = delete $data->{'CITY'};
	$data->{zip}    = delete $data->{'ZIP'};
	$data->{lata}   = delete $data->{'LATA'};
	$data->{name}   = delete $data->{'LATA_NAME'};
	$data->{st}     = 'TX';
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-SanAntonio-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{loc}    = delete $data->{'LOCATION'};
	$data->{host}   = delete $data->{'HOST'};
	$data->{pcs}    = delete $data->{'Point Codes'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{type}   = delete $data->{'TYPE'};
	$data->{tandem}{local}  = delete $data->{'LOC TANDEM'};
	$data->{tandem}{access} = delete $data->{'IEL/IAL Tandem'};
	$data->{street} = delete $data->{'ADDRESS'};
	$data->{zip}    = delete $data->{'ZIP'};
	$data->{lata}   = delete $data->{'LATA '};
	$data->{name}   = delete $data->{'LATA NAME'};
	$data->{st}     = 'TX';
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Tyler-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}  = delete $data->{'Local Tandem'};
	$data->{tandem}{intra}  = delete $data->{'IAL Tandem'};
	$data->{tandem}{access} = delete $data->{'FGD Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-Waco-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City  ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

foreach my $file (qw/TX-WichitaFalls-EO.csv/) {
open($fh,"<","$datadir/$file");
my $heading = 3;
my @fields = ();
while (<$fh>) { chomp;
	if ($heading > 1) {
		$heading--;
		next;
	} elsif ($heading == 1) {
		@fields = split(/\t/,$_);
		$heading = undef;
		next;
	}
	my @tokens = split(/\t/,$_);
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] }
	$data->{loc}    = delete $data->{'Office'};
	$data->{host}   = delete $data->{'Host'};
	$data->{pcs}    = delete $data->{'Point Code'};
	$data->{pcs}    = substr($data->{pcs},0,3).'-'.substr($data->{pcs},3,3).'-'.substr($data->{pcs},6,3) if $data->{pcs};
	$data->{clli}   = delete $data->{'CLLI'};
	$data->{type}   = delete $data->{'Type'};
	$data->{tandem}{local}  = delete $data->{'Local Tandem'};
	$data->{tandem}{intra}  = delete $data->{'IAL Tandem'};
	$data->{tandem}{access} = delete $data->{'FGD Tandem'};
	$data->{street} = delete $data->{'Address'};
	$data->{city}   = delete $data->{'City ZIP'};
	if ($data->{city} =~ /^(.*?)\s+([0-9]{5})\s*$/) {
		$data->{city} = $1;
		$data->{zip}  = $2;
	}
	$data->{st}     = 'TX';
	$data->{wcv}    = delete $data->{'Vertical'};
	$data->{wch}    = delete $data->{'Horizontal'};
	$data->{data}   = delete $data->{'Data'};
	$data->{npa}    = delete $data->{'HNPA'};
	$data->{mwatt}  = delete $data->{'Milliwat'};
	$db->{clli}{$data->{clli}} = $data if $data->{clli};
}
close($fh);
}

my $of = \*OUTFILE;
open($of,">","$datadir/eos.pm");
my $dumper = Data::Dumper->new([$db]);
print $of $dumper->Dump;
close($of);
