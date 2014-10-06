#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

use strict;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir = '.' unless $progdir =~ /\//; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/..";

use Data::Dumper;

my $fh = \*INFILE;

my $db = {};

my $line;

$line = 0;
open($fh,"<","$progdir/CIC_List.csv");
while (<$fh>) { chomp;
	$line++;
	next if $line == 1; # header line
	my ($carrier,$cic,$date,$fgrp,$remarks,$dummy) = split(/,/,$_);
	$carrier =~ s/  /, /g;
	$remarks =~ s/  /, /g;
	my $grp;
	if ($fgrp =~ /FGB/) {
		$grp = 'B';
	} elsif ($fgrp =~ /FGD/) {
		$grp = 'D';
	}
	if ($fgrp =~ /3D/) {
		$cic = sprintf("%03d",$cic);
	} elsif ($fgrp =~ /4D/) {
		$cic = sprintf("%04d",$cic);
	}
	my $rec;
	if (exists $db->{$grp}{$cic}) {
		$rec = $db->{$grp}{$cic};
	} else {
		$rec = {};
		$db->{$grp}{$cic} = $rec;
	}
	$rec->{carrier} = $carrier if $carrier;
	$rec->{date} = $date if $date;
	$rec->{fgrp} = $fgrp if $fgrp;
	$rec->{remarks} = $remarks if $remarks;
	$rec->{dummy} = $dummy if $dummy;
}
close($fh);

foreach my $grp (qw/B D/) {
	$line = 0;
	open($fh,"<","$progdir/CIC".$grp."MasterReport.csv");
	while (<$fh>) { chomp;
		$line++;
		next if $line == 1; # header line
		s/\t"--*"/\t""/g;
		my ($cic,$acna,$entity,$date,$contact,$phone,$street,$city,$state,$zip) = split(/\t/,$_);
		$cic	 =~ s/^" *(.*?) *"$/\1/;
		$acna    =~ s/^" *(.*?) *"$/\1/;
		$entity  =~ s/^" *(.*?) *"$/\1/;
		$date    =~ s/^" *(.*?) *"$/\1/;
		$contact =~ s/^" *(.*?) *"$/\1/;
		$phone   =~ s/^" *(.*?) *"$/\1/;
		$street  =~ s/^" *(.*?) *"$/\1/;
		$city    =~ s/^" *(.*?) *"$/\1/;
		$state   =~ s/^" *(.*?) *"$/\1/;
		$zip     =~ s/^" *(.*?) *"$/\1/;
		my $rec;
		if (exists $db->{$grp}{$cic}) {
			$rec = $db->{$grp}{$cic};
		} else {
			$rec = {};
			$db->{$grp}{$cic} = $rec;
		}
		$rec->{entity} = $entity if $entity;
		$rec->{date} = $date if $date;
		# ACNA = Access Customer Name Abbreviation
		$rec->{acna} = $acna if $acna;
		$rec->{contact} = $contact if $contact;
		$rec->{phone} = $phone if $phone;
		$rec->{street} = $street if $street;
		$rec->{city} = $city if $city;
		$rec->{state} = $state if $state;
		$rec->{zip} = $zip if $zip;
	}
	close($fh);
}

my $dumper = Data::Dumper->new([$db]);
print $dumper->Dump;
