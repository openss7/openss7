#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $geondir = "$progdir/../geonames";
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

sub vh2ll {
	my ($v,$h) = @_;
	my ($la,$lo) = Geo::Coordinates::VandH->vh2ll($v,$h);
	$lo = -$lo;
	$lo += 360 if $lo < -180;
	$lo -= 360 if $lo >  180;
	return ($la,$lo);
}

sub ll2vh {
	my ($lat,$lon) = @_;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=-5000;$v<=10000;$v+=5000) {
		for (my $h=-5000;$h<=30000;$h+=5000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-6000;$v<=$gv+6000;$v+=2000) {
		for (my $h=$gh-6000;$h<=$gh+6000;$h+=2000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-3000;$v<=$gv+3000;$v+=1000) {
		for (my $h=$gh-3000;$h<=$gh+3000;$h+=1000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-1500;$v<=$gv+1500;$v+=500) {
		for (my $h=$gh-1500;$h<=$gh+1500;$h+=500) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-600;$v<=$gv+600;$v+=200) {
		for (my $h=$gh-600;$h<=$gh+600;$h+=200) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-300;$v<=$gv+300;$v+=100) {
		for (my $h=$gh-300;$h<=$gh+300;$h+=100) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-150;$v<=$gv+150;$v+=50) {
		for (my $h=$gh-150;$h<=$gh+150;$h+=50) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-60;$v<=$gv+60;$v+=20) {
		for (my $h=$gh-60;$h<=$gh+60;$h+=20) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-30;$v<=$gv+30;$v+=10) {
		for (my $h=$gh-30;$h<=$gh+30;$h+=10) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-15;$v<=$gv+15;$v+=5) {
		for (my $h=$gh-15;$h<=$gh+15;$h+=5) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-6;$v<=$gv+6;$v+=2) {
		for (my $h=$gh-6;$h<=$gh+6;$h+=2) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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
	for (my $v=$gv-3;$v<=$gv+3;$v+=1) {
		for (my $h=$gh-3;$h<=$gh+3;$h+=1) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
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

my %lergcc = ();
my %lergst = ();
my %cllist = ();
my %cllicc = ();
my %cllirg = ();
my %statrg = ();
my %countries = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^\s*$/;
	my @tokens = split(/\t/,$_);
	$statrg{$tokens[1]} = $tokens[2];
	$lergcc{$tokens[2]} = $tokens[0];
	$lergst{$tokens[2]} = $tokens[1];
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
	$countries{$tokens[0]}++;
}
close($fh);

sub getll {
	my $data = shift;
	my $ll;
	my ($la,$lo) = ($data->{LATITUDE},$data->{LONGITUDE});
	if ($la and $lo) {
		$ll = "$la,$lo";
	}
	return $ll;
}

sub getvh {
	my $data = shift;
	my ($la,$lo) = ($data->{LATITUDE},$data->{LONGITUDE});
	my $vh = sprintf('%05d,%05d',ll2vh($la,$lo)) if $la and $lo;
	return $vh;
}

my %sws = ();
my %wcs = ();
my %pls = ();
my %dbsw = ();

my @sw_keys = qw/SWCLLI NPA NXX X LATA OCN NAME TYPE WCVH WCLL SECT/;
my @wc_keys = qw/WCCLLI NPA NXX X LATA OCN SWCLLI WCVH WCLL SECT/;
my @pl_keys = qw/PLCLLI NPA NXX X LATA OCN WCCLLI/;

my @dbsw_keys = qw/NPA NXX X SWCLLI LATA OCN/;

my %skipcllis = (
	'XXXXXXXXXXX'=>1,
	'__VARIOUS__'=>1,
);

my $oldnpa;

$fn = "$progdir/db.csv";
print STDERR "I: processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	next unless /^"/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{"\U$fields[$i]\E"} = $tokens[$i] if length($tokens[$i]);
	}
	if ($oldnpa ne $data->{NPA}) {
		$oldnpa = $data->{NPA};
		print STDERR "I: processing $oldnpa\n";
	}
	#print STDERR "I: processing $data->{NPA} $data->{NXX} $data->{X}\n";
	my $sw = $data->{SWITCH_CLLI};
	next unless $sw;
	next if exists $skipcllis{$sw};
	my $wc = substr($sw,0,8);
	my $pl = substr($sw,0,6);
	my $cs = substr($sw,4,2);
	my $rg = $cllirg{$cs} if exists $cllirg{$cs};
	my $cc = $cllicc{$cs} if exists $cllicc{$cs};
	my $st = $cllist{$cs} if exists $cllist{$cs};
	$data->{SWCLLI} = $sw;
	$data->{WCCLLI} = $wc;
	$data->{PLCLLI} = $pl;
	$data->{WCVH} = getvh($data);
	$data->{WCLL} = getll($data);
	$data->{CC} = $cc;
	$data->{ST} = $st;
	$data->{RG} = $rg;
	{
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}} = {} unless exists
		$dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		my $rec = $dbsw{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		foreach my $k (@dbsw_keys) {
			if ($k eq 'SWCLLI') {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) DBRC $data->{NPA}-$data->{NXX}-$data->{X} $k changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $sw) {
		$sws{$cc}{$st}{$sw} = {} unless exists $sws{$cc}{$st}{$sw};
		my $rec = $sws{$cc}{$st}{$sw};
		foreach my $k (@sw_keys) {
			if ($k =~ /^(NPA|LATA|OCN|SECT|WCLL|NAME)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'WCVH') {
				if ($data->{$k}) {
					if (exists $rec->{$k}) {
						if (ref $rec->{$k} eq 'HASH') {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$data->{$k}' ($data->{SECT})\n"
							unless exists $rec->{$k}{$data->{$k}};
							$rec->{$k}{$data->{$k}}++;
						} elsif ($rec->{$k} ne $data->{$k}) {
							my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$rec->{$k}' ($sects)\n";
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k extra value '$data->{$k}' ($data->{SECT})\n";
							$rec->{$k} = { $rec->{$k}=>1, $data->{$k}=>1 };
						}
					} else {
						$rec->{$k} = $data->{$k};
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) SW $sw $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $wc) {
		$wcs{$cc}{$st}{$wc} = {} unless exists $wcs{$cc}{$st}{$wc};
		my $rec = $wcs{$cc}{$st}{$wc};
		foreach my $k (@wc_keys) {
			if ($k =~ /^(NPA|LATA|OCN|SECT|SWCLLI|WCLL)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'WCVH') {
				if ($data->{$k}) {
					if (exists $rec->{$k}) {
						if (ref $rec->{$k} eq 'HASH') {
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$data->{$k}' ($data->{SECT})\n"
							unless exists $rec->{$k}{$data->{$k}};
							$rec->{$k}{$data->{$k}}++;
						} elsif ($rec->{$k} ne $data->{$k}) {
							my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$rec->{$k}' ($sects)\n";
							print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k extra value '$data->{$k}' ($data->{SECT})\n";
							$rec->{$k} = { $rec->{$k}=>1, $data->{$k}=>1 };
						}
					} else {
						$rec->{$k} = $data->{$k};
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) WC $wc $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
	if ($cc and $st and $pl) {
		$pls{$cc}{$st}{$pl} = {} unless exists $pls{$cc}{$st}{$pl};
		my $rec = $pls{$cc}{$st}{$pl};
		foreach my $k (@pl_keys) {
			if ($k =~ /^(NPA|LATA|OCN|WCCLLI)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) PL $pl $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
}
close($fh);

$fn = "$datadir/db.sw.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
foreach my $npa (sort keys %dbsw) {
	foreach my $nxx (sort keys %{$dbsw{$npa}}) {
		foreach my $x (sort keys %{$dbsw{$npa}{$nxx}}) {
			my $rec = $dbsw{$npa}{$nxx}{$x};
			my @values = ();
			foreach (@dbsw_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/sw.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@sw_keys),'"',"\n";
foreach my $cc (sort keys %sws) {
	foreach my $st (sort keys %{$sws{$cc}}) {
		foreach my $sw (sort keys %{$sws{$cc}{$st}}) {
			my $rec = $sws{$cc}{$st}{$sw};
			my @values = ();
			foreach (@sw_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/wc.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@wc_keys),'"',"\n";
foreach my $cc (sort keys %wcs) {
	foreach my $st (sort keys %{$wcs{$cc}}) {
		foreach my $wc (sort keys %{$wcs{$cc}{$st}}) {
			my $rec = $wcs{$cc}{$st}{$wc};
			my @values = ();
			foreach (@wc_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/pl.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@pl_keys),'"',"\n";
foreach my $cc (sort keys %pls) {
	foreach my $st (sort keys %{$pls{$cc}}) {
		foreach my $pl (sort keys %{$pls{$cc}{$st}}) {
			my $rec = $pls{$cc}{$st}{$pl};
			my @values = ();
			foreach (@pl_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						my $sep = ',';
						foreach my $k (keys %{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,sort keys %{$rec->{$_}});
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						my $sep = ',';
						foreach my $k (@{$rec->{$_}}) {
							if ($k =~ /,/) { $sep = ';'; last; }
						}
						push @values, join($sep,@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

exit;

1;

__END__

     1	"NPA"
     2	"NXX"
     3	"X"
     4	"COUNTRY"
     5	"STATE"
     6	"CITY"
     7	"COUNTY"
     8	"LATITUDE"
     9	"LONGITUDE"
    10	"LATA"
    11	"TIMEZONE"
    12	"OBSERVES_DST"
    13	"COUNTY_POPULATION"
    14	"FIPS_COUNTY_CODE"
    15	"MSA_COUNTY_CODE"
    16	"PMSA_COUNTY_CODE"
    17	"CBSA_COUNTY_CODE"
    18	"ZIPCODE_POSTALCODE"
    19	"ZIPCODE_COUNT"
    20	"ZIPCODE_FREQUENCY"
    21	"NEW_NPA"
    22	"NXX_USE_TYPE"
    23	"NXX_INTRO_VERSION"
    24	"OVERLAY"
    25	"OCN"
    26	"COMPANY"
    27	"RATE_CENTER"
    28	"SWITCH_CLLI"
    29	"RC_VERTICAL"
    30	"RC_HORIZONTAL"
