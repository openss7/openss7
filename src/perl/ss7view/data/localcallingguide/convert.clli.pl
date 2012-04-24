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

my @keys = (
	'Switch',
	'Switch name',
	'Switch type',
	'Host',
	'Remotes',
	'Last updated',
);

my %hosts = ();

print STDERR "I: finding data...\n";
my @files = `find switches -name 'data.html.xz' | sort`;

foreach $fn (@files) { chomp $fn;
	next unless $fn =~ /switches\/([^\/]+)\/([^\/]+)\/([^\/]+)\/([^\/]+)\/data.html.xz/;
	next unless $4;
	my $sw = "$2$1$3$4";
	$sw =~ s/-/ /g;
	my $data = {};
	my $inrecord = 0;
	$fn =~ s/ /\\ /g;
	print STDERR "I: processing $fn...\n";
	open($fh,"xzcat $fn |") or die "can't process $fn";
	while (<$fh>) { chomp;
		if (/<p>Last updated: <strong>([^<]+)<\/strong><\/p>/) {
			$data->{'Last updated'} = $1;
			$inrecord = 1;
		} elsif (/<p>Switch: <span class="srch">([^<]+)<\/span><\/p>/) {
			$data->{'Switch'} = $1;
			$data->{'Switch'} =~ s/-/ /g;
			$inrecord = 1;
		} elsif ($inrecord == 1 and /<ul>/) {
			$inrecord = 2;
		} elsif ($inrecord == 2 and /<li>([^<:]+):\s+(.*?)<\/li>/) {
			my ($k,$v) = ($1,$2);
			$v =~ s/<[^>]+>//g;
			if ($k and $v) {
				if (exists $data->{$k}) {
					if (ref $data->{$k}) {
						$data->{$k}{$v}++;
					} else {
						$data->{$k} = {$data->{$k}=>1,$v=>1};
					}
				} else {
					$data->{$k} = $v;
				}
			}
		} elsif ($inrecord == 2 and /<\/ul>/) {
			$inrecord = 0;
			$hosts{$sw} = {} unless exists $hosts{$sw};
			my $rec = $hosts{$sw};
			foreach my $k (sort keys %{$data}) {
				if (exists $rec->{$k}) {
					if (ref $rec->{$k}) {
						if (ref $data->{$k}) {
							foreach my $v (sort keys %{$data->{$k}}) {
								$rec->{$k}{$v} += $data->{$k}{$v};
							}
						} else {
							$rec->{$k}{$data->{$k}}++;
						}
					} else {
						if ($rec->{$k} ne $data->{$k}) {
							$rec->{$k} = {$rec->{$k}=>1};
							$rec->{$k}{$data->{$k}}++;
						}
					}
				} else {
					$rec->{$k} = $data->{$k};
				}
			}
			if (my $host = $data->{Host}) {
				$host =~ s/-/ /g;
				$hosts{$host}{Switch} = $host;
				$hosts{$host}{Remotes}{$sw}++;
			}
			foreach my $k (@keys) { delete $data->{$k} }
			foreach my $k (sort keys %{$data}) { push @keys, $k }
			last;
		}
	}
	close($fh);
}

my %h = ();
my %r = ();
my %o = ();

$fn = "$datadir/host.csv";
print STDERR "I: writing $fn\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@keys),'"',"\n";
foreach my $host (sort keys %hosts) {
	my $rec = $hosts{$host};
	my $st = substr($host,4,2);
	if ($rec->{Remotes}) {
		$h{count}++;
		$h{$st}++;
	}
	if ($rec->{Host}) {
		$r{count}++;
		$r{$st}++;
	}
	unless ($rec->{Remotes} or $rec->{Host}) {
		$o{count}++;
		$o{$st}++;
	}
	my @values = ();
	foreach my $k (@keys) {
		if (exists $rec->{$k}) {
			if (ref $rec->{$k}) {
				push @values, join(',',sort keys %{$rec->{$k}});
			} else {
				push @values, $rec->{$k};
			}
		} else {
			push @values, undef;
		}
	}
	print $of '"',join('","',@values),'"',"\n";
}
close($of);

printf STDERR "I: there are %6d hosts\n",   delete $h{count};
printf STDERR "I: there are %6d remotes\n", delete $r{count};
printf STDERR "I: there are %6d others\n",  delete $o{count};

foreach (sort keys %r) {
	printf STDERR "I: %-2.2s %6d hosts\n",   $_, $h{$_};
	printf STDERR "I: %-2.2s %6d remotes\n", $_, $r{$_};
	printf STDERR "I: %-2.2s %6d others\n",  $_, $o{$_};
}

exit;
