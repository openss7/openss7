#!/usr/bin/perl -w

use strict;

use Geo::Coordinates::VandH;

my $fh = \*INFILE;
my $fn;

my %wcs = ();
my %bad = ();
my %of1 = ();
my %of2 = ();
my %of3 = ();
my %inc = ();

my $inc_count = 0;
my $of1_count = 0;
my $of2_count = 0;
my $of3_count = 0;
my $bad_count = 0;
my $wcs_count = 0;

$fn = 'db.csv';
print STDERR "I: reading $fn...\n";
my $heading = 1;
my @fields = ();
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//; my @tokens = split(/"\s*,\s*"/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{$fields[$i]} = $tokens[$i];
	}
	my $clli = $data->{clli};
	my $wc = substr($clli,0,8);
	unless (exists $wcs{$wc}) {
		$wcs{$wc}{recs} = [];
		$wcs_count++;
	}
	push @{$wcs{$wc}{recs}}, $data;
	if ($data->{wcvh}) {
		$wcs{$wc}{where}{$data->{wcvh}}++;
		unless (exists $wcs{$wc}{wcvh}) {
			$wcs{$wc}{wcvh} = $data->{wcvh};
		}
	}
	if (exists $wcs{$wc}{wcvh} and $wcs{$wc}{wcvh} and $wcs{$wc}{wcvh} ne $data->{wcvh}) {
		my ($v1,$h1) = split(/,/,$wcs{$wc}{wcvh});
		unless (defined $v1 and defined $h1) {
			print STDERR "E: $clli has V&H of '$wcs{$wc}{wcvh}'\n";
			next;
		}
		my ($v2,$h2) = split(/,/,$data->{wcvh});
		my $vd = abs($v2-$v1);
		my $hd = abs($h2-$h1);
		my $dd = $vd > $hd ? $vd : $hd;
		my $ad = sqrt((($vd**2)+($hd**2))/10)*5280;
		if ($ad > 20000) {
			unless (exists $bad{$wc}) {
				$bad{$wc}++;
				$bad_count++;
				if (exists $of3{$wc}) {
					$of3_count -= delete $of3{$wc};
				}
				if (exists $of2{$wc}) {
					$of2_count -= delete $of2{$wc};
				}
				if (exists $of1{$wc}) {
					$of1_count -= delete $of1{$wc};
				}
				if (exists $inc{$wc}) {
					$inc_count -= delete $inc{$wc};
				}
			}
			#} elsif ($dd > 2) {
		} elsif ($ad > 10000) {
			unless (exists $bad{$wc}) {
				unless (exists $of3{$wc}) {
					$of3{$wc}++;
					$of3_count++;
					if (exists $of2{$wc}) {
						$of2_count -= delete $of2{$wc};
					}
					if (exists $of1{$wc}) {
						$of1_count -= delete $of1{$wc};
					}
					if (exists $inc{$wc}) {
						$inc_count -= delete $inc{$wc};
					}
				}
			}
			#} elsif ($dd > 1) {
		} elsif ($ad > 5000) {
			unless (exists $bad{$wc} or exists $of3{$wc}) {
				unless (exists $of2{$wc}) {
					$of2{$wc}++;
					$of2_count++;
					if (exists $of1{$wc}) {
						$of1_count -= delete $of1{$wc};
					}
					if (exists $inc{$wc}) {
						$inc_count -= delete $inc{$wc};
					}
				}
			}
			#} elsif ($dd > 0) {
		} elsif ($ad > 2602.7831) {
			unless (exists $bad{$wc} or exists $of3{$wc} or exists $of2{$wc}) {
				unless (exists $of1{$wc}) {
					$of1{$wc}++;
					$of1_count++;
					if (exists $inc{$wc}) {
						$inc_count -= delete $inc{$wc};
					}
				}
			}
		} else {
			unless (exists $bad{$wc} or exists $of3{$wc} or exists $of2{$wc} or exists $of1{$wc}) {
				unless (exists $inc{$wc}) {
					$inc{$wc}++;
					$inc_count++;
				}
			}
		}
	}
}
close($fh);

print STDERR "I: there are $wcs_count wire centers\n";
print STDERR "I: there are $inc_count wire centers inconsistent\n";
print STDERR "I: there are $of1_count wire centers inconsistent (1)\n";
print STDERR "I: there are $of2_count wire centers inconsistent (2)\n";
print STDERR "I: there are $of3_count wire centers inconsistent (3)\n";
print STDERR "I: there are $bad_count wire centers in conflict\n";

my $geo = new Geo::Coordinates::VandH;

#push @fields, 'lat';
#push @fields, 'lon';

foreach my $wc (sort keys %inc) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent\n";
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x\n";
	}
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		#my ($v,$h) = split(/,/,$rec->{wcvh});
		#my ($y,$x) = $geo->vh2ll($v,$h);
		#$rec->{lat} = $y; $rec->{lon} = -$x;
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of1) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (1)\n";
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x\n";
	}
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		#my ($v,$h) = split(/,/,$rec->{wcvh});
		#my ($y,$x) = $geo->vh2ll($v,$h);
		#$rec->{lat} = $y; $rec->{lon} = -$x;
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of2) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (2)\n";
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x\n";
	}
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		#my ($v,$h) = split(/,/,$rec->{wcvh});
		#my ($y,$x) = $geo->vh2ll($v,$h);
		#$rec->{lat} = $y; $rec->{lon} = -$x;
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of3) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (3)\n";
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x\n";
	}
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		#my ($v,$h) = split(/,/,$rec->{wcvh});
		#my ($y,$x) = $geo->vh2ll($v,$h);
		#$rec->{lat} = $y; $rec->{lon} = -$x;
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %bad) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc in conflict\n";
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x\n";
	}
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		#my ($v,$h) = split(/,/,$rec->{wcvh});
		#my ($y,$x) = $geo->vh2ll($v,$h);
		#$rec->{lat} = $y; $rec->{lon} = -$x;
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}
