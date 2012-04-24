#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'CLLI',
	'wcvh',
	'ocn',
	'lata',
	'feat',
	'NPA',
	'NXX',
);

my %recs = ();

$fn = "$datadir/db.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) { $data->{$fields[$i]} = $tokens[$i] if $tokens[$i] }
	my $sw = $data->{clli};
	next unless $sw;
	$recs{$sw} = {} unless exists $recs{$sw};
	my $rec = $recs{$sw};
	$data->{CLLI} = $sw;
	foreach my $k (@keys) {
		next unless $data->{$k};
		if ($k eq 'CLLI') { # or $k eq 'wcvh') {
			if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
				printf STDERR "E: %-8.8s %-8.8s changing from %-12.12s to %-12.12s\n",
					$sw, $k, $rec->{$k}, $data->{$k};
			}
			$rec->{$k} = $data->{$k};
		} elsif ($k eq 'feat') {
			foreach my $f (split(/\s+/,$data->{$k})) {
				$rec->{$k}{$f}++;
			}
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} else {
			$rec->{$k}{$data->{$k}}++ if $data->{$k};
		}
	}
}
close($fh);

my %bads = ();

$fn = "$datadir/sw.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@keys),'"',"\n";
foreach my $k (sort keys %recs) {
	my $rec = $recs{$k};
	my @values = ();
	foreach (@keys) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

$fn = "$datadir/swfix.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',qw/CLLI wcvh/),'"',"\n";
foreach my $k (sort keys %bads) {
	my $rec = $recs{$k};
	my @values = ();
	foreach (qw/CLLI wcvh/) {
		if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
			if ($_ eq 'feat') {
				push @values,join(' ',sort keys %{$rec->{$_}});
			} elsif ($_ eq 'wcvh') {
				push @values,join(';',sort keys %{$rec->{$_}});
				$bads{$k}++ if $values[-1] =~ /;/;
			} else {
				push @values,join(',',sort keys %{$rec->{$_}});
			}
		} else {
			push @values, $rec->{$_};
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);


exit;

1;

__END__

