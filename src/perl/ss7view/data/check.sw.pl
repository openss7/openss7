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

my %db = ();

my @dirs = (
	'neca4',
	'npanxxsource',
#	'telcodata',
#	'localcallingguide',
#	'pinglo',
#	'areacodes',
);

my %fields = ();

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/db.sw.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<",$fn) or die "can't read $fn";
	my $heading = 1;
	my $npa = 100;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			$fields{$dir} = \@tokens;
			$heading = undef;
			next;
		}
		my %data = ();
		for (my $i=0;$i<@{$fields{$dir}};$i++) {
			$data{$fields{$dir}[$i]} = $tokens[$i] if $tokens[$i];
		}
		push @{$db{$dir}{$data{NPA}}{$data{NXX}}{$data{X}}}, \%data;
		$db{mark}{$data{NPA}}{$data{NXX}}{$data{X}}++;
		if ($npa != int($data{NPA}/100)*100) {
			$npa = int($data{NPA}/100)*100;
			print STDERR "I: reading NPA $npa for $dir\n";
		}
	}
	close($fh);
}

my ($checks,$matches,$mismatches);

my %compares = (
	'npanxxsource'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
	'telcodata'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
	'localcallingguide'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
	'pinglo'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
	'neca4'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
	'areacodes'=>{
		SWCLLI=>sub{
			my ($val,$tst,$npa,$nxx,$x) = @_;
			if (defined $$val) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } else { $$val = $tst }
			return 1;
		},
	},
);

foreach my $field (qw/SWCLLI/) {
	print STDERR "I: checking $field...\n";
	foreach my $npa (sort keys %{$db{mark}}) {
		foreach my $nxx (sort keys %{$db{mark}{$npa}}) {
			next if $nxx eq '555';
			next if $nxx =~ /[2-9]11/;
			next if $nxx eq '950';
			next if $nxx eq '955';
			next if $nxx eq '958';
			next if $nxx eq '959';
			next if $nxx eq '976';
			foreach my $x (sort keys %{$db{mark}{$npa}{$nxx}}) {
				my $all = 1;
				my $good = 1;
				my $val;
				my @vals;
				foreach my $dir (@dirs) {
					my $goodone = undef;
					my @items = ();
					foreach my $rec (@{$db{$dir}{$npa}{$nxx}{$x}}) {
						if (exists $compares{$dir}{$field}) {
							$checks++;
							$goodone = 1 if &{$compares{$dir}{$field}}(\$val,$rec->{$field},$npa,$nxx,$x);
						}
						push @items,$rec->{$field};
					}
					if (scalar @items) {
						$good = undef unless $goodone;
						push @vals,join(';',@items);
					} else {
						$all = undef;
						push @vals,undef;
					}
				}
				next if $good;
				print STDERR "E: $npa-$nxx($x) $field mismatch: ",'"',join('","',@vals),'"',"\n";
			}
		}
	}
}

print STDERR "I: key ",'"',join('","',@dirs),'"',"\n";

print STDERR "I: ---------------------\n";
printf STDERR "I: there were %10d checks\n", $checks;
printf STDERR "I: there were %10d matches\n", $matches;
printf STDERR "I: there were %10d mismatches\n", $mismatches;
printf STDERR "I: accuracy = %5.2f percent\n", ($matches/($matches+$mismatches))*100;
print STDERR "I: ---------------------\n";

exit;

1;

__END__


