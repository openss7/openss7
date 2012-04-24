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

my %switches = ();

$fn = "sw.csv";
my @fields = ();
my $header = 1;
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		$header = undef;
		next;
	}
	if (my $sw = $tokens[0]) {
		for (my $i=0;$i<@fields;$i++) {
			$switches{$sw}{$fields[$i]} = $tokens[$i] if $tokens[$i];
		}
	}
}
close($fh);

my @keys = (
	'CLLI',
	'LATA',
	'SWITCHNAME',
	'SWITCHTYPE',
	'RC',
	'OCN',
	'COMPANY',
);

sub closerecord {
	my $data = shift;
	if (my $sw = $data->{CLLI}) {
		$switches{$sw} = {} unless exists $switches{$sw};
		my $rec = $switches{$sw};
		for (my $i=0;$i<4;$i++) {
			my $k = $keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: $sw $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
		push @{$rec->{rcs}}, {
			RC=>$data->{RC},
			OCN=>$data->{OCN},
			COMPANY=>$data->{COMPANY},
		};
	}
}

my $data = {};

print STDERR "I: finding data...\n";
my @files = `find switches -name 'data.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn\n";
	open($fh,"xzcat $fn |") or die "can't process $fn";
	while (<$fh>) { chomp;
		if (/<tr class="rc([0-9]+)"/) {
			$data->{RC} = $1;
		} elsif (/<td headers="oswitch"/) {
			if (/>\s*([^<]+?)\s*</) {
				$data->{CLLI} = $1;
			} else {
				print STDERR "E: can't grok '$_'\n";
			}
		} elsif (/<td headers="oswitchname"/) {
			if (/>\s*([^<]*?)\s*</) {
				$data->{SWITCHNAME} = $1;
			} else {
				print STDERR "E: can't grok '$_'\n";
			}
		} elsif (/<td headers="ocompany"/) {
			if (/>\s*([^<]*?)\s*</) {
				my $ocn = '';
				my $company = $1;
				if ($company =~ /^([0-9]{3}[0-9A-Z])\s+(.*)$/) {
					$ocn = $1;
					$company = $2;
				}
				$data->{COMPANY} = $company;
				$data->{OCN} = $ocn;
			} else {
				print STDERR "E: can't grok '$_'\n";
			}
		} elsif (/<\/tr>/) {
			closerecord($data);
			$data = {};
		}
	}
	close($fh);
}

$fn = "switches.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
foreach my $sw (sort keys %switches) {
	foreach my $sub (@{$switches{$sw}{rcs}}) {
		my @values = ();
		for (my $i=0;$i<4;$i++) {
			my $k = $keys[$i];
			push @values,$switches{$sw}{$k};
		}
		for (my $i=4;$i<@keys;$i++) {
			my $k = $keys[$i];
			push @values,$sub->{$k};
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);


