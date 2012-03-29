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
while (<$fh>) { chomp;
	next unless /^[A-Z0-9]{11}\t/;
	my $data = {};
	my @tokens = split(/\t/,$_);
	$data->{clli}   = $tokens[0];
	$data->{loc}    = $tokens[1];
	$data->{host}   = $tokens[2] unless $tokens[2] eq 'N/A';
	$data->{pcs}    = $tokens[3] unless $tokens[3] =~ /SEE HOST/;
	$data->{type}   = $tokens[4];
	$data->{street} = $tokens[5];
	$data->{city}   = $tokens[6];
	$data->{zip}    = $tokens[7];
	$data->{lata}   = $tokens[8];
	$data->{name}   = $tokens[9];
	$db->{clli}{$data->{clli}} = $data;
	$db->{locs}{$data->{loc}}  = $data unless $data->{host};
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
	$db->{clli}{$data->{clli}} = $data;
}
close($fh);
}

open($fh,"<","$datadir/CT-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/IL-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/IN-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/KS-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/MI-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/MO-KansasCity-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/MO-Springfield-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/MO-StLouis-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/OH-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/OK-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/OK-Tulsa-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Abilene-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Amarillo-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Austin-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Beaumont-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Brownsville-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-CorpusChristi-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Dallas-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-ElPaso-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Fayetteville-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-FortSmith-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-FtWorth-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Houston-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Longview-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Lubbock-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Midland-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-PineBluf-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-SanAntonio-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Tyler-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-Waco-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/TX-WichitaFalls-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

open($fh,"<","$datadir/WI-EO.csv");
while (<$fh>) { chomp;
}
close($fh);

my $of = \*OUTFILE;
open($of,">","$datadir/eos.pm");
my $dumper = Data::Dumper->new([$db]);
print $of $dumper->Dump;
close($of);
