#!/usr/bin/perl -w

use strict;

use Geo::Coordinates::VandH;

my $fh = \*INFILE;
my $fn;

sub closest {
	my ($lat,$lon) = @_;
	my $geo = new Geo::Coordinates::VandH;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=0;$v<=10000;$v+=1000) {
		for (my $h=-5000;$h<=27000;$h+=1000) {
			my ($y,$x) = $geo->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=100) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=100) {
			my ($y,$x) = $geo->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-200;$v<=$gv+200;$v+=10) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=10) {
			my ($y,$x) = $geo->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-20;$v<=$gv+20;$v+=1) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=1) {
			my ($y,$x) = $geo->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	return ($cv,$ch);
}
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

my %wc_fields = (
	'NPA'=>1,
	'NXX'=>1,
	'Wire Center CLLI'=>1,
	'Wire Center LATA'=>1,
	'Wire Center Address'=>1,
	'Wire Center Country'=>1,
	'Wire Center City'=>1,
	'Wire Center State'=>1,
	'Wire Center Zip'=>1,
	'Wire Center Latitude/Longitude'=>1,
	'Wire Center V&H'=>1,
	'Wire Center CLLI'=>1,
	'Switch'=>1,
	'Wire Center Country'=>1,
	'Wire Center Name'=>1,
	'Rate Center LATA'=>1,
	'Rate Center Name'=>1,
	'Rate Center State'=>1,
	'Rate Center Country'=>1,
	'LERG Abbreviation'=>1,
	'Zone'=>1,
	'Zone Type'=>1,
	'Rate Step'=>1,
	'Major V&H'=>1,
	'Rate Center Latitude/Longitude'=>1,
);

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
		$data->{$fields[$i]} = $tokens[$i] if exists $wc_fields{$fields[$i]};
	}
	my $sw = $data->{Switch};
	my $wc = $data->{'Wire Center CLLI'};
	if (substr($sw,0,8) ne $wc) {
		printf STDERR "E: Switch %-11.11s and Wire Center CLLI %-8.8s do not match!\n", $sw, $wc;
	}
	unless (exists $wcs{$wc}) {
		$wcs{$wc}{recs} = [];
		$wcs_count++;
	}
	push @{$wcs{$wc}{recs}}, $data;
	my $wcvh = sprintf('%05d,%05d', split(/,/,$data->{'Wire Center V&H'}));
	if ($data->{'Wire Center V&H'}) {
		unless (exists $wcs{$wc}{wcvh}) {
			if ($data->{'Wire Center Latitude/Longitude'}) {
				my ($lat,$lon) = split(/,/,$data->{'Wire Center Latitude/Longitude'});
				my ($v2,$h2) = closest($lat,$lon);
				my ($v1,$h1) = split(/,/,$wcvh);
				my $ad = sqrt(((($v2-$v1)**2)+(($h2-$h1)**2))/10);
				if ($ad > 5) {
					print STDERR "W: $sw correcting ($wcvh)";
					$wcvh = sprintf('%05d,%05d',$v2,$h2);
					print STDERR " to $lat,$lon ($wcvh)";
					printf STDERR " %s, %s, %s, %s, %s, %s, %s\n",
						$data->{'Wire Center Address'},
						$data->{'Wire Center City'},
						$data->{'Wire Center State'},
						$data->{'Wire Center Zip'},
						$data->{'Wire Center Country'},
						$data->{'Wire Center V&H'},
						$data->{'Wire Center Latitude/Longitude'};
					$data->{'Wire Center V&H'} = $wcvh;
				}
			}
			$wcs{$wc}{wcvh} = $wcvh;
		}
		$wcs{$wc}{where}{$wcvh}++;
	}
	if ($data->{'Major V&H'} and $data->{'Major V&H'} ne '00000,00000') {
		my ($v2,$h2) = split(/,/,$data->{'Major V&H'});
		my $rcvh = sprintf('%05d,%05d', $v2, $h2);
		$wcs{$wc}{rcvh} = $rcvh;
		$wcs{$wc}{rated}{$rcvh}++;
		my ($v1,$h1) = split(/,/,$wcvh);
		my $ad = sqrt(((($v2-$v1)**2)+(($h2-$h1)**2))/10);
		$wcs{$wc}{delta}{$rcvh} = $ad;
		if ($ad > 500) {
			unless (exists $bad{$wc}) {
				$bad{$wc}++;
				$bad_count++;
				if (exists $of3{$wc}) { $of3_count -= delete $of3{$wc} }
				if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
				if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
				if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
			}
		} elsif ($ad > 200) {
			unless (exists $bad{$wc}) {
				unless (exists $of3{$wc}) {
					$of3{$wc}++;
					$of3_count++;
					if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 100) {
			unless (exists $bad{$wc} or exists $of3{$wc}) {
				unless (exists $of2{$wc}) {
					$of2{$wc}++;
					$of2_count++;
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 50) {
			unless (exists $bad{$wc} or exists $of3{$wc} or exists $of2{$wc}) {
				unless (exists $of1{$wc}) {
					$of1{$wc}++;
					$of1_count++;
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		}
	}
if (0) {
	if ($data->{'Wire Center Latitude/Longitude'}) {
		my ($lat,$lon) = split(/,/,$data->{'Wire Center Latitude/Longitude'});
		my ($v2,$h2) = closest($lat,$lon);
		my ($v1,$h1) = split(/,/,$wcvh);
		my $ad = sqrt(((($v2-$v1)**2)+(($h2-$h1)**2))/10)*5820;
		if ($ad > 20000) {
			unless (exists $bad{$wc}) {
				$bad{$wc}++;
				$bad_count++;
				if (exists $of3{$wc}) { $of3_count -= delete $of3{$wc} }
				if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
				if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
				if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
			}
		} elsif ($ad > 10000) {
			unless (exists $bad{$wc}) {
				unless (exists $of3{$wc}) {
					$of3{$wc}++;
					$of3_count++;
					if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 5000) {
			unless (exists $bad{$wc} or exists $of3{$wc}) {
				unless (exists $of2{$wc}) {
					$of2{$wc}++;
					$of2_count++;
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 2602.7831) {
			unless (exists $bad{$wc} or exists $of3{$wc} or exists $of2{$wc}) {
				unless (exists $of1{$wc}) {
					$of1{$wc}++;
					$of1_count++;
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
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
if (0) {
	if (exists $wcs{$wc}{wcvh} and $wcs{$wc}{wcvh} and $wcs{$wc}{wcvh} ne $wcvh) {
		my ($v1,$h1) = split(/,/,$wcs{$wc}{wcvh});
		unless (defined $v1 and defined $h1) {
			print STDERR "E: $sw has V&H of '$wcs{$wc}{wcvh}'\n";
			next;
		}
		my ($v2,$h2) = split(/,/,$wcvh);
		my $ad = sqrt(((($v2-$v1)**2)+(($h2-$h1)**2))/10)*5820;
		if ($ad > 20000) {
			unless (exists $bad{$wc}) {
				$bad{$wc}++;
				$bad_count++;
				if (exists $of3{$wc}) { $of3_count -= delete $of3{$wc} }
				if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
				if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
				if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
			}
		} elsif ($ad > 10000) {
			unless (exists $bad{$wc}) {
				unless (exists $of3{$wc}) {
					$of3{$wc}++;
					$of3_count++;
					if (exists $of2{$wc}) { $of2_count -= delete $of2{$wc} }
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 5000) {
			unless (exists $bad{$wc} or exists $of3{$wc}) {
				unless (exists $of2{$wc}) {
					$of2{$wc}++;
					$of2_count++;
					if (exists $of1{$wc}) { $of1_count -= delete $of1{$wc} }
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
				}
			}
		} elsif ($ad > 2602.7831) {
			unless (exists $bad{$wc} or exists $of3{$wc} or exists $of2{$wc}) {
				unless (exists $of1{$wc}) {
					$of1{$wc}++;
					$of1_count++;
					if (exists $inc{$wc}) { $inc_count -= delete $inc{$wc} }
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
}
close($fh);

print STDERR "I: there are $wcs_count wire centers\n";
print STDERR "I: there are $inc_count wire centers inconsistent\n";
print STDERR "I: there are $of1_count wire centers inconsistent (1)\n";
print STDERR "I: there are $of2_count wire centers inconsistent (2)\n";
print STDERR "I: there are $of3_count wire centers inconsistent (3)\n";
print STDERR "I: there are $bad_count wire centers in conflict\n";

my $geo = new Geo::Coordinates::VandH;

foreach my $wc (sort keys %inc) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		printf STDERR "V: %s, %s, %s, %s, %s, %s, %s\n",
			$rec->{'Wire Center Address'},
			$rec->{'Wire Center City'},
			$rec->{'Wire Center State'},
			$rec->{'Wire Center Zip'},
			$rec->{'Wire Center Country'},
			$rec->{'Wire Center V&H'},
			$rec->{'Wire Center Latitude/Longitude'};
	}
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x ($key)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $key (sort keys %{$wcs{$wc}{rated}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{rated}{$key} hits $y,$x ($key $wcs{$wc}{delta}{$key} miles)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k} if exists $wc_fields{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of1) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (1)\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		printf STDERR "V: %s, %s, %s, %s, %s, %s, %s\n",
			$rec->{'Wire Center Address'},
			$rec->{'Wire Center City'},
			$rec->{'Wire Center State'},
			$rec->{'Wire Center Zip'},
			$rec->{'Wire Center Country'},
			$rec->{'Wire Center V&H'},
			$rec->{'Wire Center Latitude/Longitude'};
	}
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x ($key)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $key (sort keys %{$wcs{$wc}{rated}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{rated}{$key} hits $y,$x ($key $wcs{$wc}{delta}{$key} miles)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k} if exists $wc_fields{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of2) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (2)\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		printf STDERR "V: %s, %s, %s, %s, %s, %s, %s\n",
			$rec->{'Wire Center Address'},
			$rec->{'Wire Center City'},
			$rec->{'Wire Center State'},
			$rec->{'Wire Center Zip'},
			$rec->{'Wire Center Country'},
			$rec->{'Wire Center V&H'},
			$rec->{'Wire Center Latitude/Longitude'};
	}
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x ($key)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $key (sort keys %{$wcs{$wc}{rated}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{rated}{$key} hits $y,$x ($key $wcs{$wc}{delta}{$key} miles)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k} if exists $wc_fields{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %of3) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc inconsistent (3)\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		printf STDERR "V: %s, %s, %s, %s, %s, %s, %s\n",
			$rec->{'Wire Center Address'},
			$rec->{'Wire Center City'},
			$rec->{'Wire Center State'},
			$rec->{'Wire Center Zip'},
			$rec->{'Wire Center Country'},
			$rec->{'Wire Center V&H'},
			$rec->{'Wire Center Latitude/Longitude'};
	}
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x ($key)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $key (sort keys %{$wcs{$wc}{rated}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{rated}{$key} hits $y,$x ($key $wcs{$wc}{delta}{$key} miles)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k} if exists $wc_fields{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}

foreach my $wc (sort keys %bad) {
	print STDERR "V: -------------------\n";
	print STDERR "V: $wc in conflict\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		printf STDERR "V: %s, %s, %s, %s, %s, %s, %s\n",
			$rec->{'Wire Center Address'},
			$rec->{'Wire Center City'},
			$rec->{'Wire Center State'},
			$rec->{'Wire Center Zip'},
			$rec->{'Wire Center Country'},
			$rec->{'Wire Center V&H'},
			$rec->{'Wire Center Latitude/Longitude'};
	}
	foreach my $key (sort keys %{$wcs{$wc}{where}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{where}{$key} hits $y,$x ($key)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $key (sort keys %{$wcs{$wc}{rated}}) {
		my ($v,$h) = split(/,/,$key);
		my ($y,$x) = $geo->vh2ll($v,$h); $x = -$x;
		print STDERR "V: $key has $wcs{$wc}{rated}{$key} hits $y,$x ($key $wcs{$wc}{delta}{$key} miles)\n";
	}
	print STDERR "V: -------------------\n";
	foreach my $rec (@{$wcs{$wc}{recs}}) {
		my @tokens = ();
		foreach my $k (@fields) {
			push @tokens, $rec->{$k} if exists $wc_fields{$k};
		}
		print STDERR "V: ", '"', join('","',@tokens), '"', "\n";
	}
}
