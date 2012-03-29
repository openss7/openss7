#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/data/npanxx";

use Data::Dumper;

local $fh = \*INFILE;

my @files = `find $datadir -name '*.html.xz'`;

my $db = {};

$db->{'NPA NXX'} = {};
$db->{'Wire Center'} = {};
$db->{'Switch'} = {};
$db->{'Rate Center - Locality'} = {};
$db->{'LATA'} = {};
$db->{'Carrier'} = {};

foreach my $file (@files) {

	my @lines = `xzcat $file`;

#	my @lines = ();

#	open($fh,"<","$datadir/npanxx.tst");
#	while (<$fh>) { chomp;
#		push @lines, $_;
#	}
#	close($fh);

	my $entry = join('',@lines);

	my $rec = {};

	my $end = '';
	my $key;
	while ($entry =~ /(<h3>(.*?)<\/h3>|<div class="n30">(.*?)<\/div>[[:space:]]*<div class="n70">(.*?)<\/div>)/g) {
		#print "found '$1','$2','$3','$4'\n";
		if ($2) {
			print $end if $end;
			my $tit = $2;
			my $npa;
			if ($tit =~ /^NPA NXX ([0-9]+-[0-9]+)$/) {
				$tit = 'NPA NXX';
				$npa = $1;
			}
			print "'$tit'=>{\n";
			$end = "},\n";
			$key = $tit;
			if ($npa) {
				printf "\t'%s'=>'%s',\n", $tit, $npa;
				$rec->{$key}{$tit} = $npa;
			}
		} else {
			my $fld = $3;
			my $val = $4;
			$fld =~ s/\&amp;/\&/g;
			$val =~ s/\&amp;/\&/g;
			if ($fld eq '&nbsp;') {
				printf "\t# %s\n", $val;
			} else {
				printf "\t'%s'=>'%s',\n", $fld, $val;
				$rec->{$key}{$fld} = $val;
			}
		}
	}

	if (my $npa = $rec->{'NPA NXX'}{'NPA NXX'}) {
		$db->{'NPA NXX'}{$npa} = $rec->{'NPA NXX'};
	} else {
		if ($entry =~ />([^<]*) is not an active telephone exchange\.</) {
			$db->{'NPA NXX'}{$1} = { Vacant => 'Yes' };
		}
	}
	if (my $nam = $rec->{'Rate Center - Locality'}{'Rate Center Name'}) {
		$db->{'Rate Center - Locality'}{$nam} = $rec->{'Rate Center - Locality'};
		$rec->{'NPA NXX'}{'Rate Center - Locality'} = $nam;
		$rec->{'Wire Center'}{'Rate Center - Locality'} = $nam;
		$rec->{'Switch'}{'Rate Center - Locality'} = $nam;
	}
	if (my $lata = $rec->{'LATA'}{'LATA'}) {
		$db->{'LATA'}{$lata} = $rec->{'LATA'};
		$rec->{'NPA NXX'}{'LATA'} = $lata;
		$rec->{'Switch'}{'LATA'} = $lata;
		$rec->{'Wire Center'}{'LATA'} = $lata;
		$rec->{'Rate Center - Locality'}{'LATA'} = $lata;
	}
	if (my $ocn = $rec->{'Carrier'}{'OCN'}) {
		$db->{'Carrier'}{$ocn} = $rec->{'Carrier'};
		$rec->{'NPA NXX'}{'Carrier'} = $ocn;
		$rec->{'Switch'}{'Carrier'} = $ocn;
		$rec->{'Wire Center'}{'Carrier'}{$ocn} = 1;
	}
	if (my $clli = $rec->{'Wire Center'}{'Wire Center CLLI'}) {
		$db->{'Wire Center'}{$clli} = $rec->{'Wire Center'};
		if (my $swi = $rec->{'Wire Center'}{'Switch'}) {
			foreach (keys %{$rec->{'Wire Center'}}) {
				$db->{'Switch'}{$swi}{$_} = $rec->{'Wire Center'}{$_};
			}
			#$db->{'Switch'}{$swi} = $rec->{'Wire Center'};
			$rec->{'NPA NXX'}{'Switch'} = $swi;
		}
		$rec->{'NPA NXX'}{'Wire Center'} = $clli;
		$rec->{'Switch'}{'Wire Center'} = $clli;
		$rec->{'Rate Center - Locality'}{'Wire Center'}{$clli} = 1;
	}
}

my $dumper = Data::Dumper->new([$db]);
print $dumper->Dump;
