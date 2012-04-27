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
#	'npanxxsource',
	'telcodata',
	'localcallingguide',
	'pinglo',
#	'areacodes',
);

my %fields = ();

foreach my $dir (@dirs) {
	$fn = "$codedir/$dir/wc.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<",$fn) or die "can't read $fn";
	my $heading = 1;
	my $place = '';
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
		push @{$db{$dir}{$data{WCCLLI}}}, \%data;
		$db{mark}{$data{WCCLLI}}++;
		if ($place ne $data{STCLLI}) {
			$place = $data{STCLLI};
			print STDERR "I: reading STCLLI $place for $dir\n";
		}
	}
	close($fh);
}

my ($checks,$matches,$mismatches);

my %compares = (
	'npanxxsource'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
	'telcodata'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
	'localcallingguide'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
	'pinglo'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
	'neca4'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
	'areacodes'=>{
		LATA=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/,/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/,/,$tst)) {
				next if $_ eq '99999';
				$_ = substr($_,0,3);
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(',',sort keys %tsts);
			}
			return 1;
		},
		OCN=>sub{
			my ($val,$tst,$clli) = @_;
			if ($$val) { if ($tst) { if ($$val ne $tst) { $mismatches++; return undef } else { $matches++ } } } else { $$val = $tst }
			return 1;
		},
		WCVH=>sub{
			my ($val,$tst,$clli) = @_;
			my %vals = ();
			foreach (split(/;/,$$val)) {
				$vals{$_}++ if $_;
			}
			my %tsts = ();
			foreach (split(/;/,$tst)) {
				$tsts{$_}++ if $_;
			}
			if (keys %vals) {
				if (keys %tsts) {
					foreach my $t (keys %tsts) {
						unless (exists $vals{$t}) {
							$mismatches++;
							return undef;
						}
					}
					foreach my $v (keys %vals) {
						unless (exists $tsts{$v}) {
							$mismatches++;
							return undef;
						}
					}
					$matches++;
				}
			} else {
				$$val = join(';',sort keys %tsts);
			}
			return 1;
		},
	},
);

foreach my $field (qw/LATA OCN WCVH/) {
	($checks,$matches,$mismatches) = (0,0,0);
	print STDERR "I: checking $field...\n";
	foreach my $clli (sort keys %{$db{mark}}) {
		my $all = 1;
		my $good = 1;
		my $val;
		my @vals;
		foreach my $dir (@dirs) {
			my $goodone = undef;
			my @items = ();
			foreach my $rec (@{$db{$dir}{$clli}}) {
				if (exists $compares{$dir}{$field}) {
					$checks++;
					$goodone = 1 if &{$compares{$dir}{$field}}(\$val,$rec->{$field},$clli);
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
		my $sect = " ($db{neca4}{$clli}[0]{SECT})" if $db{neca4}{$clli};
		print STDERR "E: $clli $field mismatch: ",'"',join('","',@vals),'"',"$sect\n";
		
	}
	print STDERR "I: ---------------------\n";
	printf STDERR "I: there were %10d checks\n", $checks;
	printf STDERR "I: there were %10d matches\n", $matches;
	printf STDERR "I: there were %10d mismatches\n", $mismatches;
	printf STDERR "I: accuracy = %5.2f percent\n", ($matches/($matches+$mismatches))*100;
	print STDERR "I: ---------------------\n";
}

print STDERR "I: key ",'"',join('","',@dirs),'"',"\n";

exit;

1;

__END__

     1	"WCCLLI"
     2	"NPA"
     3	"NXX"
     4	"X"
     5	"LATA"
     6	"OCN"
     7	"SWCLLI"
     8	"WCVH"
     9	"WCLL"
    10	"WCADDR"
    11	"WCCITY"
    12	"WCCOUNTY"
    13	"SECT"
