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

my @rc_keys = (
	'RCSHORT',
	'REGION',
	'RCVH',
	'NPA',
	'RC',
	'EXCH',
	'SEE-REGION',
	'SEE-RC',
	'SEE-EXCH',
	'ILEC-OCN',
	'ILEC-NAME',
	'LATA',
	'OCN',
	'COMPANY-NAME',
	'RC-V',
	'RC-H',
	'RC-LAT',
	'RC-LON',
	'UDATE',
);

my %rcs = ();

sub do_rc {
	my $fn;
	$fn = "$datadir/rc.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	print $of '"',join('","',@rc_keys),'"',"\n";
	my @files = `find ratecenters -name '*.xml.xz' | sort`;
	my $data = {};
	my $inrecord = 0;
	foreach $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn...\n";
		open($fh,"xzcat $fn |") or die "can't process $fn";
		while (<$fh>) { chomp;
			if (/^<\/rcdata>/) {
				if ($data->{'RC-V'} and $data->{'RC-H'}) {
					$data->{RCVH} = sprintf "%05d,%05d", $data->{'RC-V'}, $data->{'RC-H'};
				}
				if (my $exch = $data->{EXCH}) {
					$rcs{$exch} = {} unless exists $rcs{$exch};
					my $rec = $rcs{$exch};
					my @values = ();
					for (my $i=0;$i<@rc_keys;$i++) {
						my $k = $rc_keys[$i];
						if ($data->{$k}) {
							if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
								print STDERR "E: $exch $k changing from $rec->{$k} to $data->{$k}\n";
							}
							$rec->{$k} = $data->{$k};
						}
						push @values,$rec->{$k};
					}
					print $of '"',join('","',@values),'"',"\n";

				}
				$inrecord = 0;
			} elsif (/^<rcdata>/) {
				$data = {};
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				$data->{"\U$1\E"} = $2 if $2;
			}
		}
		close($fh);
	}
	close($of);
}

my %wcs = ();

sub do_wc {
	my $fn;
	my @keys = (
		'CLLI',
		'EXCH',
		'LATA',
		'ILEC-OCN',
		'ILEC-NAME',
		'RC',
		'RCSHORT',
		'REGION',
		'SEE-EXCH',
		'SEE-RC',
		'SEE-REGION',
		'RC-V',
		'RC-H',
	);
	$fn = "$datadir/wc.csv";
}

my %cllis = ();

sub do_clli {
	my $fn;
	my @keys = (
		'CLLI',
		'SWITCHNAME',
		'SWITCHTYPE',
		'LATA',
		'REGION',
		'STATE',
		'NPA',
		'NPA-NXX',
		'HOST',
		'REMOTES',
		'UPDATED',
	);
	my %fieldmap = (
		'Switch'=>'CLLI',
		'Switch name'=>'SWITCHNAME',
		'Switch type'=>'SWITCHTYPE',
		'Host'=>'HOST',
		'Remotes'=>'REMOTES',
		'Last updated'=>'UPDATED',
	);
	$fn = "$datadir/host.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $header = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $clli = $tokens[0];
		$cllis{$clli} = {} unless exists $cllis{$clli};
		my $rec = $cllis{$clli};
		$rec->{CLLI} = $clli;
		for (my $i=1;$i<@fields;$i++) {
			$rec->{$fieldmap{$fields[$i]}}{$tokens[$i]}++;
		}
	}
	close($fh);
	my @files = `find results -name '*.A.xml.xz' | sort`;
	my $data = {};
	my $inrecord = 0;
	foreach $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn...\n";
		open($fh,"xzcat $fn |") or die "can't process $fn";
		while (<$fh>) { chomp; s/\r//g;
			if (/^<\/prefixdata>/) {
				if (my $exch = $data->{EXCH}) {
					if (exists $rcs{$exch}) {
						my $rec = $rcs{$exch};
						foreach my $k (keys %{$rec}) {
							if ($rec->{$k} and not $data->{$k}) {
								$data->{$k} = $rec->{$k};
							}
						}
						if ($data->{NPA}) {
							$rec->{NPA}{$data->{NPA}}++;
						}
					}
				}
				my $clli = $data->{SWITCH};
				if ($clli and $clli ne '99999999999' and $clli ne 'XXXXXXXXXXX') {
					$clli =~ s/-/ /g;
					$cllis{$clli} = {} unless exists $cllis{$clli};
					my $rec = $cllis{$clli};
					$rec->{CLLI} = $clli;
					$data->{STATE} = substr($clli,4,2);
					if ($data->{STATE} eq 'AM') { $data->{STATE} = 'AS' }
					if ($data->{NPA} and $data->{NXX}) {
						$data->{'NPA-NXX'} = "$data->{NPA}-$data->{NXX}";
					}
					for (my $i=1;$i<@keys;$i++) {
						my $k = $keys[$i];
						if ($data->{$k}) {
						#	if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						#		printf STDERR "E: CLLI %-11.11s %-10.10s changing from %-24.24s to %-24.24s\n",
						#		$clli, $k, $rec->{$k}, $data->{$k};
						#	}
						#	$rec->{$k} = $data->{$k};
							$rec->{$k}{$data->{$k}}++;
						}
					}
				}
				$inrecord = 0;
			} elsif (/^<prefixdata>/) {
				$data = {};
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				$data->{"\U$1\E"} = $2 if length($2);
			}
		}
		close($fh);
	}
	$fn = "$datadir/clli.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	print $of '"',join('","',@keys),'"',"\n";
	foreach my $k (sort keys %cllis) {
		my $rec = $cllis{$k};
		my @values = ();
		push @values, $rec->{CLLI};
		for (my $i=1;$i<@keys;$i++) {
			my $k = $keys[$i];
			if (exists $rec->{$k} and ref $rec->{$k} eq 'HASH') {
				push @values,join(',',sort keys %{$rec->{$k}});
			} else {
				push @values,$rec->{$k};
			}
		}
		#foreach (@keys) { push @values, $rec->{$_} }
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
	$fn = "$datadir/rc.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	print $of '"',join('","',@rc_keys),'"',"\n";
	foreach my $exch (sort keys %rcs) {
		my $rec = $rcs{$exch};
		my @values = ();
		for (my $i=0;$i<@rc_keys;$i++) {
			my $k = $rc_keys[$i];
			if (exists $rec->{$k} and ref $rec->{$k} eq 'HASH') {
				push @values,join(',',sort keys %{$rec->{$k}});
			} else {
				push @values,$rec->{$k};
			}
		}
		#foreach (@rc_keys) { push @values, $rec->{$_} }
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}

my %ocns = ();

sub do_ocn {
	my $fn;
	my @keys = (
		'OCN',
		'COMPANY-NAME',
		'COMPANY-TYPE',
	);
	my @files = `find results -name '*.A.xml.xz' | sort`;
	my $data = {};
	my $inrecord = 0;
	foreach $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn...\n";
		open($fh,"xzcat $fn |") or die "can't process $fn";
		while (<$fh>) { chomp; s/\r//g;
			if (/^<\/prefixdata>/) {
				if (my $ocn = $data->{OCN}) {
					$ocns{$ocn} = {} unless exists $ocns{$ocn};
					my $rec = $ocns{$ocn};
					$rec->{OCN} = $ocn;
					for (my $i=1;$i<@keys;$i++) {
						my $k = $keys[$i];
						if ($data->{$k}) {
							if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
								printf STDERR "E: OCN %-4.4s %-10.10s changing from %-24.24s to %-24.24s\n",
								$ocn, $k, $rec->{$k}, $data->{$k};
							}
							$rec->{$k} = $data->{$k};
						}
					}
				}
				if (my $ocn = $data->{'ILEC-OCN'}) {
					$ocns{$ocn} = {} unless exists $ocns{$ocn};
					my $rec = $ocns{$ocn};
					$rec->{OCN} = $ocn;
					$rec->{'COMPANY-NAME'} = $data->{'ILEC-NAME'} unless $rec->{'COMPANY-NAME'};
					$rec->{'COMPANY-TYPE'} = 'I' unless $rec->{'COMPANY-TYPE'};
				}
				$inrecord = 0;
			} elsif (/^<prefixdata>/) {
				$data = {};
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				$data->{"\U$1\E"} = $2 if length($2);
			}
		}
		close($fh);
	}
	$fn = "$datadir/ocn.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	print $of '"',join('","',@keys),'"',"\n";
	foreach my $k (sort keys %ocns) {
		my $ocn = $ocns{$k};
		my @values = ();
		foreach (@keys) { push @values, $ocn->{$_} }
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}

sub do_nxx {
	my $fn;
	my @keys = (
		'NPA',
		'NXX',
		'X',
		'EXCH',
		'RC',
		'RCSHORT',
		'REGION',
		'SEE-EXCH',
		'SEE-RC',
		'SEE-REGION',
		'SWITCH',
		'SWITCHNAME',
		'SWITCHTYPE',
		'LATA',
		'OCN',
		'COMPANY-NAME',
		'COMPANY-TYPE',
		'ILEC-OCN',
		'ILEC-NAME',
		'RC-V',
		'RC-H',
		'RC-LAT',
		'RC-LON',
		'EFFDATE',
		'DISCDATE',
		'UDATE',
	);
	$fn = "$datadir/db.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	print $of '"',join('","',@keys),'"',"\n";
	my @files = `find results -name '*.A.xml.xz' | sort`;
	my $data = {};
	my $inrecord = 0;
	foreach $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn...\n";
		open($fh,"xzcat $fn |") or die "can't process $fn";
		while (<$fh>) { chomp; s/\r//g;
			if (/^<\/prefixdata>/) {
				if (my $exch = $data->{EXCH}) {
					if (exists $rcs{$exch}) {
						my $rec = $rcs{$exch};
						foreach my $k (keys %{$rec}) {
							if ($rec->{$k} and not $data->{$k}) {
								$data->{$k} = $rec->{$k};
							}
						}
					}
				}
				if (exists $data->{NPA} and exists $data->{NXX}) {
					my @values = ();
					foreach my $k (@keys) { push @values, $data->{$k} }
					print $of '"',join('","',@values),'"',"\n";
				}
				$inrecord = 0;
			} elsif (/^<prefixdata>/) {
				$data = {};
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				$data->{"\U$1\E"} = $2 if length($2);
			}
		}
		close($fh);
	}
	close($of);
}

do_rc();
do_clli();
#do_ocn();
#do_nxx();

exit;
