#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = $progdir;

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(STDERR,':utf8');

my %db = ();

my @dirs = (
	'areacodes',
	'localcallingguide',
	'nanpa',
	'neca4',
	'npanxxsource',
	'pinglo',
	'telcodata',
);

my %fields = ();
my %fieldnos = ();

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $heading = 1;
	my $oldnpa = 100;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			$fields{$dir} = \@tokens;
			for (my $i=0;$i<@tokens;$i++) {
				$fieldnos{$dir}{$tokens[$i]} = $i;
			}
			$heading = undef;
			next;
		}
		next if $dir eq 'nanpa' and $tokens[$fieldnos{$dir}{Use}] eq 'UA';
		my ($npa,$nxx,$x) = @tokens;
		#push @{$db{$dir}{$npa}{$nxx}{$x}}, \@tokens;
		$db{cnts}{$npa}{$nxx}{$dir}++;
		if ($oldnpa != int($npa/100)*100) {
			$oldnpa = int($npa/100)*100;
			print STDERR "I: reading NPA $oldnpa for $dir\n";
		}
	}
	close($fh);
}

foreach my $npa (keys %{$db{cnts}}) {
	foreach my $nxx (keys %{$db{cnts}{$npa}}) {
		my @ds = (keys %{$db{cnts}{$npa}{$nxx}});
		if (scalar @ds < 2) {
			$db{numb}{$npa}{$nxx} = $ds[0];
		}
		delete $db{cnts}{$npa}{$nxx};
	}
}

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.csv";
	print STDERR "I: re-reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $heading = 1;
	my $oldnpa = 100;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) { $heading = undef; next }
		my ($npa,$nxx,$x) = @tokens;
		next if $dir eq 'nanpa' and $tokens[$fieldnos{$dir}{Use}] eq 'UA';
		if ($oldnpa != int($npa/100)*100) {
			$oldnpa = int($npa/100)*100;
			print STDERR "I: re-reading NPA $oldnpa for $dir\n";
		}
		if (exists $db{numb}{$npa}{$nxx}) {
			push @{$db{$dir}{$npa}{$nxx}{$x}}, \@tokens;
		}
	}
	close($fh);
}

my %isolated = ();
print STDERR "I: checking isolated records...\n";
foreach my $npa (sort keys %{$db{numb}}) {
	foreach my $nxx (sort keys %{$db{numb}{$npa}}) {
		#next if $nxx eq '555';
		#next if $nxx =~ /[2-9]11/;
		#next if $nxx eq '950';
		#next if $nxx eq '955';
		#next if $nxx eq '958';
		#next if $nxx eq '959';
		#next if $nxx eq '976';
		if (exists $db{numb}{$npa}{$nxx}) {
			my $dir = $db{numb}{$npa}{$nxx};
			print STDERR "E: $npa-$nxx only has records for $dir\n";
			$isolated{$dir}++;
			print STDERR "E: $npa-$nxx: ",'"',join('","',@{$fields{$dir}}),'"',"\n";
			foreach my $x (keys %{$db{$dir}{$npa}{$nxx}}) {
				foreach my $rec (@{$db{$dir}{$npa}{$nxx}{$x}}) {
					print STDERR "E: $npa-$nxx($x): ",'"',join('","',@{$rec}),'"',"\n";
				}
			}
		}
	}
}
print STDERR "I: ---------------------\n";
foreach my $d (@dirs) {
	my $cnt = 0;
	$cnt = $isolated{$d} if exists $isolated{$d};
	printf STDERR "I: there are %10d isolated records for $d\n", $cnt;
}
print STDERR "I: ---------------------\n";

exit;

__END__
