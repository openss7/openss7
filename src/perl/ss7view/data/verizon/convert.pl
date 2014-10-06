#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'CLLI',
	'SWITCH',
	'OLD POINT CODE',
	'NEW POINT CODE',
	'STATE',
);

$fn = "ND10-0003.txt";
open($fh,"<",$fn) or die "can't open $fn";
$fn = "ND10-0003.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"CLLI","SWITCH","OLD POINT CODE","NEW POINT CODE","STATE"', "\n";
while (<$fh>) { chomp;
	if (/^([A-Z]{2})\s+(.*?)\s+([A-Z0-9]{11})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})/) {
		printf $of '"%s","%s","%s","%s"'."\n", $3,$2,$4,$5,$1;

	}
}
close($of);
close($fh);

$fn = "E911_ActGuide_March2007_X.txt";
open($fh,"<",$fn) or die "can't open $fn";
$fn = "E911_ActGuide_March2007_X.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"CLLI","POINT CODE"', "\n";
while (<$fh>) { chomp;
	while (/\b([A-Z0-9]{11})\s+([0-9]{3})\s+([0-9]{3})\s+([0-9]{3})\b/g) {
		printf $of '"%s","%s-%s-%s"'."\n", $1, $2, $3, $4;
	}
}
close($of);
close($fh);
