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
use Time::gmtime;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

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

my %vhcache = ();

sub vh2llfast {
	my ($v,$h,$inc) = @_;
	my ($la,$lo);
	if (exists $vhcache{$v}{$h}) {
		($la,$lo) = @{$vhcache{$v}{$h}};
	} else {
		($la,$lo) = vh2ll($v,$h);
		$vhcache{$v}{$h} = [$la,$lo] if $inc and $inc >= 100;
	}
	return ($la,$lo);
}

sub dist {
	my ($la1,$lo1,$la2,$lo2) = @_;
	my $d = sqrt((($la1-$la2)**2)+(($lo1-$lo2)**2));
	return $d;
}

sub adjv {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	while ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h,$inc)))) { $d = $dt; $v += $inc; }
	while ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h,$inc)))) { $d = $dt; $v -= $inc; }
	return ($v,$d);
}

sub adjh {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	while ($d > ($dt = dist($la,$lo,vh2llfast($v,$h+$inc,$inc)))) { $d = $dt; $h += $inc; }
	while ($d > ($dt = dist($la,$lo,vh2llfast($v,$h-$inc,$inc)))) { $d = $dt; $h -= $inc; }
	return ($h,$d);
}

sub adjvh {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h+$inc,$inc)))) { $v+=$inc; $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h,     $inc)))) { $v+=$inc;           return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h-$inc,$inc)))) { $v+=$inc; $h-=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v,     $h+$inc,$inc)))) {           $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h+$inc,$inc)))) { $v-=$inc; $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h,     $inc)))) { $v-=$inc;           return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v,     $h-$inc,$inc)))) {           $h-=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h-$inc,$inc)))) { $v-=$inc; $h-=$inc; return ($v,$h,$dt); }
	return ($v,$h,$d);
}

sub adj {
	my ($la,$lo,$v,$h,$inc,$d) = @_;
	my $changed = 1;
	while ($changed) {
		my ($vsav,$hsav) = ($v,$h);
		($v,$h,$d) = adjvh($la,$lo,$v,$h,$inc,$d);
		$changed = ($v != $vsav or $h != $hsav);
	}
	return ($v,$h,$d);
}

sub ll2vhold {
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
	for (my $v=$gv-8000;$v<=$gv+8000;$v+=2000) {
		for (my $h=$gh-8000;$h<=$gh+8000;$h+=2000) {
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
	for (my $v=$gv-4000;$v<=$gv+4000;$v+=1000) {
		for (my $h=$gh-4000;$h<=$gh+4000;$h+=1000) {
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
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=500) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=500) {
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
	for (my $v=$gv-800;$v<=$gv+800;$v+=200) {
		for (my $h=$gh-800;$h<=$gh+800;$h+=200) {
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
	for (my $v=$gv-400;$v<=$gv+400;$v+=100) {
		for (my $h=$gh-400;$h<=$gh+400;$h+=100) {
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
	for (my $v=$gv-200;$v<=$gv+200;$v+=50) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=50) {
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
	for (my $v=$gv-80;$v<=$gv+80;$v+=20) {
		for (my $h=$gh-80;$h<=$gh+80;$h+=20) {
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
	for (my $v=$gv-40;$v<=$gv+40;$v+=10) {
		for (my $h=$gh-40;$h<=$gh+40;$h+=10) {
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
	for (my $v=$gv-20;$v<=$gv+20;$v+=5) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=5) {
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
	for (my $v=$gv-8;$v<=$gv+8;$v+=2) {
		for (my $h=$gh-8;$h<=$gh+8;$h+=2) {
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
	for (my $v=$gv-4;$v<=$gv+4;$v+=1) {
		for (my $h=$gh-4;$h<=$gh+4;$h+=1) {
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

sub ll2vh {
	my ($la,$lo) = @_;
	my ($v,$h) = (-5000,-5000);
	my $d = dist($la,$lo,vh2llfast($v,$h,5000));
	($v,$h,$d) = adj($la,$lo,$v,$h,5000,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,1250,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,400,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,100,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,25,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,5,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,1,$d);
	if (0) {
	my ($vo,$ho) = ll2vhold($la,$lo);
	if ($v != $vo or $h != $ho) {
		my ($la1,$lo1) = vh2ll($v,$h);
		my $d1 = dist($la,$lo,$la1,$lo1);
		my ($la2,$lo2) = vh2ll($vo,$ho);
		my $d2 = dist($la,$lo,$la2,$lo2);
		print STDERR "E: VH mismatch $la1,$lo1($v,$h)[$d1] instead of $la2,$lo2($vo,$ho)[$d2] targetting $la,$lo";
		if ($d1 < $d2) {
			print STDERR " new is closer!\n";
		} elsif ($d2 < $d1) {
			print STDERR " old is closer!\n";
		} else {
			print STDERR " equidistant!\n";
		}
	}
	}
	return ($v,$h);
}

my %npaccst = ();
our %nanpst = ();

$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	my ($npa,$cc,$st,$name) = split(/\t/,$_);
	$nanpst{$npa} = $st;
	$npaccst{$npa} = [ $cc, $st ];
}
close($fh);

my %nxxccst = ();

$fn = "$codedir/nxxst.txt";
print STDER "I: reading $fn\n";
open ($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next unless /^[0-9]+/;
	my ($npa,$nxx,$cc,$st,$name) = split(/\t/,$_);
	$nxxccst{$npa}{$nxx} = [ $cc, $st ];
}
close($fh);

our %lergcc = ();
our %lergst = ();
our %lergps = ();
our %lergcs = ();
our %cllist = ();
our %cllicc = ();
our %cllirg = ();
our %statrg = ();
our %ccstrg = ();
our %pcpsda = ();
our %countries = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^\s*$/;
	my @tokens = split(/\t/,$_);
	$pcpsda{$tokens[4]}{$tokens[5]} = [ $tokens[0], $tokens[1], $tokens[2], $tokens[3] ];
	$ccstrg{$tokens[0]}{$tokens[1]} = $tokens[2];
	$statrg{$tokens[1]} = $tokens[2];
	$lergcc{$tokens[2]} = $tokens[0];
	$lergst{$tokens[2]} = $tokens[1];
	$lergps{$tokens[2]} = [ $tokens[4], $tokens[5] ];
	$lergcs{$tokens[2]} = $tokens[3];
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
	$countries{$tokens[0]}++;
}
close($fh);

my $corrected = 0;
my %corrections = ();
my @cordbs = qw/RC RN SW WC PL PC EX DB/;

for my $db (@cordbs) {
	$fn = "$datadir/\L$db\E.corx.csv";
	$corrections{fixx}{$db} = {};
	$corrections{corx}{$db} = {};
	if (-f $fn) {
		print STDERR "I: reading $fn\n";
		open($fh,"<:utf8",$fn) or die "can't read $fn";
		my $heading = 1;
		my @cfields = ();
		while (<$fh>) { chomp;
			s/^"//; s/"$//; my @tokens = split(/","/,$_);
			if ($heading) {
				@cfields = @tokens;
				$heading = undef;
				next;
			}
			my $rec = $corrections{corx}{$db};
			for (my $i=0;$i<@cfields-6;$i++) {
				$rec->{$tokens[$i]} = {} unless exists $rec->{$tokens[$i]};
				$rec = $rec->{$tokens[$i]};
			}
			$rec->{$tokens[@cfields-6]} = [] unless exists $rec->{$tokens[@cfields-6]};
			$rec = $rec->{$tokens[@cfields-6]};
			push @{$rec}, [$tokens[-5],$tokens[-4],$tokens[-3],$tokens[-2],$tokens[-1]];
		}
		close($fh);
	}
}

sub getpfx {
	my $dat = shift;
	my $pfx;
	if (length($dat->{NPA})) {
		$pfx = "$dat->{NPA}-$dat->{NXX}";
		$pfx .= "-$dat->{X}" if length($dat->{X});
		$pfx .= "($dat->{XXXX}-$dat->{YYYY})" if length($dat->{XXXX}) or length($dat->{YYYY});
	} else {
		$pfx = $dat->{SWCLLI};
	}
	return $pfx;
}

my %fixrecs = (
	'RC'=>sub{
		my ($dat,$db,@args) = @_;
		my ($rg,$rc) = @args;
		($rg,$rc) = (
			$dat->{REGION},
			$dat->{RCSHORT},
		) unless $rg or $rc;
		$corrections{fixx}{$db}{$rg}{$rc} = [] unless exists
		$corrections{fixx}{$db}{$rg}{$rc};
		return $corrections{fixx}{$db}{$rg}{$rc};
	},
	'RN'=>sub{
		my ($dat,$db,@args) = @_;
		my ($cc,$st,$rn) = @args;
		($cc,$st,$rn) = (
			$dat->{RCCC},
			$dat->{RCST},
			$dat->{RCNAME},
		) unless $cc or $st or $rn;
		$corrections{fixx}{$db}{$cc}{$st}{$rn} = [] unless exists
		$corrections{fixx}{$db}{$cc}{$st}{$rn};
		return $corrections{fixx}{$db}{$cc}{$st}{$rn};
	},
	'SW'=>sub{
		my ($dat,$db,@args) = @_;
		my ($cc,$st,$sw) = @args;
		($cc,$st,$sw) = (
			$dat->{WCCC},
			$dat->{WCST},
			$dat->{SWCLLI},
		) unless $cc or $st or $sw;
		$corrections{fixx}{$db}{$cc}{$st}{$sw} = [] unless exists
		$corrections{fixx}{$db}{$cc}{$st}{$sw};
		return $corrections{fixx}{$db}{$cc}{$st}{$sw};
	},
	'WC'=>sub{
		my ($dat,$db,@args) = @_;
		my ($cc,$st,$wc) = @args;
		($cc,$st,$wc) = (
			$dat->{WCCC},
			$dat->{WCST},
			$dat->{WCCLLI},
		) unless $cc or $st or $wc;
		$corrections{fixx}{$db}{$cc}{$st}{$wc} = [] unless exists
		$corrections{fixx}{$db}{$cc}{$st}{$wc};
		return $corrections{fixx}{$db}{$cc}{$st}{$wc};
	},
	'PL'=>sub{
		my ($dat,$db,@args) = @_;
		my ($cc,$st,$pl) = @args;
		($cc,$st,$pl) = (
			$dat->{WCCC},
			$dat->{WCST},
			$dat->{PLCLLI},
		) unless $cc or $st or $pl;
		$corrections{fixx}{$db}{$cc}{$st}{$pl} = [] unless exists
		$corrections{fixx}{$db}{$cc}{$st}{$pl};
		return $corrections{fixx}{$db}{$cc}{$st}{$pl};
	},
	'PC'=>sub{
		my ($dat,$db,@args) = @_;
		my ($pc) = @args;
		($pc) = (
			$dat->{SPC},
		) unless $pc;
		$corrections{fixx}{$db}{$pc} = [] unless exists
		$corrections{fixx}{$db}{$pc};
		return $corrections{fixx}{$db}{$pc};
	},
	'EX'=>sub{
		my ($dat,$db,@args) = @_;
		my ($ex) = @args;
		($ex) = (
			$dat->{EXCH},
		) unless $ex;
		$corrections{fixx}{$db}{$ex} = [] unless exists
		$corrections{fixx}{$db}{$ex};
		return $corrections{fixx}{$db}{$ex};
	},
	'DB'=>sub{
		my ($dat,$db,@args) = @_;
		my ($npa,$nxx,$x,$xxxx,$yyyy) = (
			$dat->{NPA},
			$dat->{NXX},
			$dat->{X},
			$dat->{XXXX},
			$dat->{YYYY},
		);
		$corrections{fixx}{$db}{$npa}{$nxx}{$x}{$xxxx}{$yyyy} = [] unless exists
		$corrections{fixx}{$db}{$npa}{$nxx}{$x}{$xxxx}{$yyyy};
		return $corrections{fixx}{$db}{$npa}{$nxx}{$x}{$xxxx}{$yyyy};
	},
);

my %corkeys = (
	'RC'=>[qw/REGION RCSHORT/],
	'RN'=>[qw/RCCC RCST RCNAME/],
	'SW'=>[qw/WCCC WCST SWCLLI/],
	'WC'=>[qw/WCCC WCST WCCLLI/],
	'PL'=>[qw/WCCC WCST PLCLLI/],
	'PC'=>[qw/SPC/],
	'EX'=>[qw/EXCH/],
	'DB'=>[qw/NPA NXX X XXXX YYYY/],
);

sub getfixrecs {
	my ($dat,$db,@args) = @_;
	my $recs = &{$fixrecs{$db}}($dat,$db,@args) if exists $fixrecs{$db};
	return $recs;
}

sub correct {
	my ($rec,$dat,$fld,$old,$new,$com,$db,@args) = @_;
	my $recs = getfixrecs($dat,$db,@args);
	return unless $recs;
	$fld = $mapping::fieldsource{$fld} if $db eq 'DB' and exists $mapping::fieldsource{$fld};
	my %sects = ();
	%sects = %{$rec->{SECT}} if $rec and exists $rec->{SECT} and ref $rec->{SECT} eq 'HASH';
	$sects{$rec->{SECT}}++ if $rec and exists $rec->{SECT} and ref $rec->{SECT} eq '';
	$sects{$dat->{SECT}}++ if $dat->{SECT};
	my @secs = sort keys %sects;
	my $ref = join(',',@secs) if @secs;
	foreach (@{$recs}) {
		if ($_->[0] eq $fld and $_->[2] eq $new) { $_->[4] = $ref if $ref; return; }
	}
	my $com2 = " [$db ".join('-',@args)."]" if $db;
	print STDERR "C: ",getpfx($dat),": $fld: $old \-> $new ($com$com2) $ref\n";
	push @{$recs}, [$fld,$old,$new,$com,$ref];
	$corrected++;
}

sub rptchange {
	my ($rec,$dat,$k,$db,@args) = @_;
	print STDERR "E: ",getpfx($dat),": $db ",join('-',@args)," $k changing from '$rec->{$k}'(".gmctime($rec->{FDATE}).") to '$dat->{$k}'(".gmctime($dat->{FDATE}).")\n";
}

sub updchange {
	my ($rec,$dat,$k,$db,@args) = @_;
	#print STDERR "W: ",getpfx($dat),": $db ",join('-',@args)," $k updated from '$rec->{$k}'(".gmctime($rec->{FDATE}).") to '$dat->{$k}'(".gmctime($dat->{FDATE}).")\n";
}

sub oldchange {
	my ($rec,$dat,$k,$db,@args) = @_;
	#print STDERR "W: ",getpfx($dat),": $db ",join('-',@args)," $k not updated from '$rec->{$k}'(".gmctime($rec->{FDATE}).") to '$dat->{$k}'(",gmctime($dat->{FDATE}).")\n";
}

sub badchange {
	my ($rec,$dat,$k,$db,@args) = @_;
	if (length($dat->{$k})) {
		if (length($rec->{$k}) and $dat->{$k} ne $rec->{$k}) {
			if ($rec->{FDATE} < $dat->{FDATE}) {
				updchange(@_) unless $k eq 'FDATE';
				$rec->{$k} = $dat->{$k};
			} elsif ($rec->{FDATE} > $dat->{FDATE}) {
				oldchange(@_) unless $k eq 'FDATE';
			} else {
				rptchange(@_);
				correct($rec,$dat,$k,$rec->{$k},$dat->{$k},'FIXME: pick one!',$db,@args);
			}
		} else {
			$rec->{$k} = $dat->{$k};
		}
	}
}

sub extchange {
	my ($rec,$dat,$k,$db,@args) = @_;
	rptchange(@_);
	correct($rec,$dat,$k,$rec->{$k},$dat->{$k},'FIXME: choose one!',$db,@args);
}

sub getnxxccst {
	my $dat = shift;
	my ($npa,$nxx) = ($dat->{NPA},$dat->{NXX});
	my ($cc,$st,$rg,$pc,$ps);
	if (exists $npaccst{$npa}) {
		($cc,$st) = @{$npaccst{$npa}};
		($cc,$st) = @{$nxxccst{$npa}{$nxx}} if exists $nxxccst{$npa}{$nxx};
		if ($cc and $st) {
			if (exists $ccstrg{$cc}{$st}) {
				$rg = $ccstrg{$cc}{$st};
				if (exists $lergps{$rg}) {
					($pc,$ps) = @{$lergps{$rg}};
				} else {
					print STDERR "E: $npa-$nxx: no PC/PS for REGION $rg\n";
				}
			} else {
				print STDERR "E: $npa-$nxx: no REGION for CC/ST $cc-$st\n";
			}
		}
	} else {
		print STDERR "E: $npa-$nxx: NPA $npa not assigned by NANPA\n";
		correct(undef,$dat,'**DELETE**','','','not assigned by NANPA','DB',$npa,$nxx);
	}
	return ($cc,$st,$rg,$pc,$ps);
}

my $pattern = '^........(';

{
$fn = "$codedir/matching.txt";
print STDERR "I: reading $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
my $sep = '';
while (<$fh>) { chomp;
	my ($numb,$pat,$regex) = split(/\t/,$_);
	next unless $numb >= 50 or $pat =~ /[x]/ or ($pat =~ /[an]/ and $numb >= 5);
	next if $pat =~ /[a]/;
	$pattern .= "$sep$regex";
	$sep = '|';
}
close($fh);
$pattern .= ')$';
}

my %pcs = ();

my @pc_keys = qw/SPC CLLI SWCLLI HOST CLS45SW STP1 STP2 FDATE/;

my %lts = ();
my %dblt = ();

my @lt_keys = qw/LATA NPA NXX X CC ST RG RCCC RCST REGION OCN FDATE/;

my %sws = ();
my %wcs = ();
my %pls = ();
my @sw_keys = qw/SWCLLI NPA NXX X LATA OCN SWNAME SWTYPE SWFUNC WCVH WCLL RCVH RCLL CLS45SW SPC FEAT STP1 STP2 ACTUAL AGENT HOST TDM TDMILT TDMFGC TDMFGD TDMLCL TDMFGB TDMOPS SECT FDATE/;
my @wc_keys = qw/WCCLLI NPA NXX X LATA OCN WCNAME SWCLLI WCVH WCLL RCVH RCLL WCADDR WCZIP WCCITY WCCOUNTY WCST WCCC SECT FDATE/;
my @pl_keys = qw/PLCLLI NPA NXX X LATA OCN WCCLLI WCCITY WCCOUNTY WCST WCCC SECT FDATE/;

my %rcs = ();
my %rns = ();
my @rc_keys = qw/RCSHORT REGION RCCC RCST RCNAME RCVH RCLL WCVH WCLL NPA NXX X LATA OCN FDATE/;
my @rn_keys = qw/RCCC RCST RCNAME REGION RCSHORT RCVH RCLL WCVH WCLL NPA NXX X LATA OCN FDATE/;

my %exs = ();
my @ex_keys = qw/EXCH REGION RCSHORT RCNAME RCVH RCLL NPA NXX X LATA OCN FDATE/;

my %db = ();
my @db_keys = qw/NPA NXX X XXXX YYYY CC ST RG OCN LATA LOCNAME WCCC WCST SWCLLI CLS45SW EXCH REGION RCSHORT RCCC RCST RCNAME STP1 STP2 ACTUAL AGENT HOST TDM TDMILT TDMFGC TDMFGD TDMLCL TDMFGB TDMOPS SECT FDATE/;

sub checkclli {
	my ($dat,$fld,$clli) = @_;
	my %skipcllis = (
		'XXXXXXXXXXX'=>1,
		'NOCLLIKNOWN'=>1,
		'__VARIOUS__'=>1,
		'99999999999'=>1,
		'ALL SWITCHE'=>1,
		'ALLSWITCHES'=>1,
		'N/A'=>1,
		'NONE'=>1,
	);
	return 0 if exists $skipcllis{$clli};
	my $sw = $clli; $sw = $dat->{SWCLLI} if $dat->{SWCLLI} or $fld ne 'SWCLLI';
	my $cs = substr($sw,4,2);
	my $cc = $cllicc{$cs} if exists $cllicc{$cs};
	my $st = $cllist{$cs} if exists $cllist{$cs};
	if (length($clli) != 11) {
		print STDERR "E: ",getpfx($dat),": CLLI $clli is not 11 characters long\n";
		correct($sws{$cc}{$st}{$sw},$dat,$fld,$clli,$clli,'FIXME: wrong length','SW',$cc,$st,$sw);
		$dat->{BADCLLI} = 1;
		return 0;
	} elsif (!exists $cllicc{substr($clli,4,2)}) {
		print STDERR "E: ",getpfx($dat),": CLLI $clli has bad region ",substr($clli,4,2),"\n";
		correct($sws{$cc}{$st}{$sw},$dat,$fld,$clli,$clli,'FIXME: bad region','SW',$cc,$st,$sw);
		$dat->{BADCLLI} = 1;
		return 0;
	} elsif ($clli !~ /$pattern/) {
		print STDERR "W: ",getpfx($dat),": CLLI $clli has equip ",substr($clli,8,3)," that is poorly formatted\n";
		correct($sws{$cc}{$st}{$sw},$dat,$fld,$clli,$clli,'FIXME: poor equip','SW',$cc,$st,$sw);
		$dat->{BADCLLI} = 1;
		return 1;
	} else {
		return 1;
	}
}

sub addclli {
	my ($dat,$fld,$val,$func) = @_;
	while ($val) {
		my $sw = substr($val,0,11);
		$val = substr($val,11);
		$val =~ s/^\s+//;
		my $cs = substr($sw,4,2);
		my $cc = $cllicc{$cs} if exists $cllicc{$cs};
		my $st = $cllist{$cs} if exists $cllist{$cs};
		next unless not $fld or checkclli($dat,$fld,$sw);
		my $wc = substr($sw,0,8);
		my $pl = substr($sw,0,6);
		if ($cc and $st and $sw) {
			$sws{$cc}{$st}{$sw} = {} unless exists $sws{$cc}{$st}{$sw};
			my $rec = $sws{$cc}{$st}{$sw};
			$rec->{SWCLLI} = $sw;
			if ($func) {
				$dat->{$func} = $sw;
				if ($func =~ /STP/) {
					$rec->{SWFUNC}{STP}++;
				} else {
					$rec->{SWFUNC}{$func}++;
				}
			}
		}
		if ($cc and $st and $wc) {
			$wcs{$cc}{$st}{$wc} = {} unless exists $wcs{$cc}{$st}{$wc};
			my $rec = $wcs{$cc}{$st}{$wc};
			$rec->{WCCLLI} = $wc;
			$rec->{SWCLLI}{$sw}++;
		}
		if ($cc and $st and $pl) {
			$pls{$cc}{$st}{$pl} = {} unless exists $pls{$cc}{$st}{$pl};
			my $rec = $pls{$cc}{$st}{$pl};
			$rec->{PLCLLI} = $pl;
			$rec->{WCCLLI}{$wc}++;
		}
	}
}

sub addpl {
	my ($data,$cc,$st,$pl) = @_;
	return unless $cc and $st and $pl;
	$pls{$cc}{$st}{$pl} = {} unless exists $pls{$cc}{$st}{$pl};
	my $rec = $pls{$cc}{$st}{$pl};
	foreach my $k (@pl_keys) {
		if ($k =~ /^(NPA|LATA|OCN|SECT|WCCLLI)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'PL',$pl);
		}
	}
}
sub addwc {
	my ($dat,$cc,$st,$wc) = @_;
	return unless $cc and $st and $wc;
	$wcs{$cc}{$st}{$wc} = {} unless exists $wcs{$cc}{$st}{$wc};
	my $rec = $wcs{$cc}{$st}{$wc};
	foreach my $k (@wc_keys) {
		if ($k =~ /^(NPA|LATA|OCN|SECT|RCVH|RCLL|SWCLLI)$/) {
			$rec->{$k}{$dat->{$k}}++ if length($dat->{$k});
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$dat->{NPA}-$dat->{NXX}"}++ if $dat->{NPA} and $dat->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$dat->{NPA}-$dat->{NXX}-$dat->{X}"}++ if $dat->{NPA} and $dat->{NXX} and length($dat->{X});
		} else {
			badchange($rec,$dat,$k,'WC',$wc);
		}
	}
}
sub addsw {
	my ($data,$cc,$st,$sw) = @_;
	return unless $cc and $st and $sw;
	$sws{$cc}{$st}{$sw} = {} unless exists $sws{$cc}{$st}{$sw};
	my $rec = $sws{$cc}{$st}{$sw};
	foreach my $k (@sw_keys) {
		if ($k =~ /^(NPA|LATA|OCN|SECT|RCVH|RCLL|SWNAME|TDM.*)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'SWFUNC') {
			if ($data->{$k}) { foreach my $f (keys %{$data->{$k}}) { $rec->{$k}{$f}++ } }
		} elsif ($k eq 'FEAT') {
			foreach my $f (split(/\s+/,$data->{$k})) {
				$rec->{$k}{$f}++ if $f;
			}
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'SW',$sw);
		}
	}
}
sub addpc {
	my ($data,$pc) = @_;
	my $sw = $data->{SWCLLI};
	$sw = $data->{CLS45SW} if $data->{CLS45SW};
	$sw = $data->{HOST} if $data->{HOST};
	$data->{CLLI} = $sw;
	return unless $pc and $sw;
	$pcs{$pc} = {} unless exists $pcs{$pc};
	my $rec = $pcs{$pc};
	foreach my $k (@pc_keys) {
		if ($k =~ /^(NPA|LATA|OCN|SECT|RCVH|RCLL|SWCLLI|SWNAME|TDM.*)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'SWFUNC') {
			if ($data->{$k}) { foreach my $f (keys %{$data->{$k}}) { $rec->{$k}{$f}++ } }
		} elsif ($k eq 'FEAT') {
			foreach my $f (split(/\s+/,$data->{$k})) {
				$rec->{$k}{$f}++ if $f;
			}
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'PC',$pc);
		}
	}
}
sub addrc {
	my ($data,$rg,$rc) = @_;
	return unless $rg and $rc;
	$rcs{$rg}{$rc} = {} unless exists $rcs{$rg}{$rc};
	my $rec = $rcs{$rg}{$rc};
	foreach my $k (@rc_keys) {
		if ($k =~ /^(NPA|LATA|OCN|WCVH|WCLL|SWCLLI|WCCLLI)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'RC',$rg,$rc);
		}
	}
}
sub addrn {
	my ($data,$cc,$st,$rn) = @_;
	return unless $cc and $st and $rn;
	$rns{$cc}{$st}{$rn} = {} unless exists $rns{$cc}{$st}{$rn};
	my $rec = $rns{$cc}{$st}{$rn};
	foreach my $k (@rn_keys) {
		if ($k =~ /^(NPA|LATA|OCN|WCVH|WCLL|SWCLLI|WCCLLI)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'RN',$cc,$st,$rn);
		}
	}
}
sub addex {
	my ($data,$ex) = @_;
	return unless $ex;
	$exs{$ex} = {} unless exists $exs{$ex};
	my $rec = $exs{$ex};
	foreach my $k (@ex_keys) {
		if ($k =~ /^(NPA|LATA|OCN|WCVH|WCLL|SWCLLI|WCCLLI)$/) {
			$rec->{$k}{$data->{$k}}++ if length($data->{$k});
		} elsif ($k eq 'NXX') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		} elsif ($k eq 'X') {
			$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
		} else {
			badchange($rec,$data,$k,'EX',$ex);
		}
	}
}

$fn = "$progdir/swx.csv";
if (-f $fn) {
	print STDERR "I: processing $fn\n";
	open($fh,"<:utf8",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	my %fieldnos = ();
	while (<$fh>) { chomp;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			for (my $i=0;$i<@fields;$i++) {
				$fieldnos{$fields[$i]} = $i;
			}
			$heading = undef;
			next;
		}
		my $sw = $tokens[0];
		my $wc = substr($sw,0,8);
		my $pl = substr($sw,0,6);
		my $cs = substr($sw,4,2);
		my $cc = $cllicc{$cs} if exists $cllicc{$cs};
		my $st = $cllist{$cs} if exists $cllist{$cs};
		if (my $recs = $corrections{corx}{SW}{$cc}{$st}{$sw}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $sw = undef; last }
				if (exists $fieldnos{$cor->[0]}) {
					$tokens[$fieldnos{$cor->[0]}] = $cor->[2];
				}
			}
		}
		if (my $recs = $corrections{corx}{WC}{$cc}{$st}{$wc}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $sw = undef; last }
				if (exists $fieldnos{$cor->[0]}) {
					$tokens[$fieldnos{$cor->[0]}] = $cor->[2];
				}
			}
		}
		if (my $recs = $corrections{corx}{PL}{$cc}{$st}{$pl}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $sw = undef; last }
				if (exists $fieldnos{$cor->[0]}) {
					$tokens[$fieldnos{$cor->[0]}] = $cor->[2];
				}
			}
		}
		next unless defined $sw;
		my %data = ();
		for (my $i=0;$i<@fields;$i++) {
			$data{$fields[$i]} = $tokens[$i] if length($tokens[$i]);
		}
		$sw = $data{SWCLLI};
		my $wc = $data{WCCLLI} = substr($sw,0,8);
		my $pl = $data{PLCLLI} = substr($sw,0,6);
		my $cs = $data{STCLLI} = substr($sw,4,2);
		my $cc = $data{WCCC} = $cllicc{$cs} if exists $cllicc{$cs};
		my $st = $data{WCST} = $cllist{$cs} if exists $cllist{$cs};
		next unless checkclli(\%data,'SWCLLI',$sw);
		addclli(\%data,'HOST',$data{HOST},'HOST') if length($data{HOST});
		addsw(\%data,$cc,$st,$sw);
		addwc(\%data,$cc,$st,$wc);
		addpl(\%data,$cc,$st,$pl);
		addpc(\%data,$data{SPC}) if $data{SPC} and not $data{HOST};
	}
	close($fh);
}

require "$progdir/mapping.pm";

my ($oldnpa,$oldrg);

$fn = "$progdir/db.csv";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
my %fieldnos = ();
while (<$fh>) { chomp;
	next unless /^"/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		for (my $i=0;$i<@fields;$i++) {
			$fieldnos{$fields[$i]} = $i;
		}
		$heading = undef;
		next;
	}
	my ($npa,$nxx,$x,$xxxx,$yyyy) = @tokens;
	if (my $recs = $corrections{corx}{DB}{$npa}{$nxx}{$x}{$xxxx}{$yyyy}) {
		foreach my $cor (@{$recs}) {
			if ($cor->[0] eq '**DELETE**') { $npa = undef; last; }
			if (exists $fieldnos{$cor->[0]}) {
				$tokens[$fieldnos{$cor->[0]}] = $cor->[2];
			}
		}
	}
	next unless defined $npa;
	my $data = {};
	if (exists $fieldnos{sect}) {
		$data->{SECT} = $tokens[$fieldnos{sect}] if $tokens[$fieldnos{sect}];
	} else {
		$data->{SECT} = "$tokens[0]-$tokens[1]";
		$data->{SECT} .= "-$tokens[2]" if length($tokens[2]);
	}
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping::mapping{$fields[$i]}) {
			&{$mapping::mapping{$fields[$i]}}($data,$fields[$i],$tokens[$i]);
		} else {
			print STDERR "E: no mapping for '$fields[$i]'\n";
		}
	}
	#print STDERR "I: processing $data->{NPA} $data->{NXX} $data->{X}\n";
	my $region = $data->{RG}; $region = $data->{REGION} unless $region; $region = $data->{WCRG} unless $region;
	if ($npa ne $oldnpa and $region ne $oldrg) {
		print STDERR "I: processing $npa $region...\n";
	}
	($oldnpa,$oldrg) = ($npa,$region);
	{
		$db{$data->{NPA}}{$data->{NXX}}{$data->{X}}{$data->{XXXX}}{$data->{YYYY}} = {} unless exists
		$db{$data->{NPA}}{$data->{NXX}}{$data->{X}}{$data->{XXXX}}{$data->{YYYY}};
		my $rec = $db{$data->{NPA}}{$data->{NXX}}{$data->{X}}{$data->{XXXX}}{$data->{YYYY}};
		foreach my $k (@db_keys) {
			badchange($rec,$data,$k,'DB',$data->{NPA},$data->{NXX});
		}
	}
	{
		if (my $lt = $data->{LATA}) {
			$lts{$lt} = {} unless exists $lts{$lt};
			my $rec = $lts{$lt};
			foreach my $k (@lt_keys) {
				if ($k =~ /^(NPA|OCN|CC|ST|RG|RCCC|RCST|REGION)$/) {
					$rec->{$k}{$data->{$k}}++ if length($data->{$k});
				} elsif ($k eq 'NXX') {
					$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
				} elsif ($k eq 'X') {
					$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and length($data->{X});
				} else {
					badchange($rec,$data,$k,'LATA',$lt);
				}
			}
		}
	}
	{
		my ($cc,$st,$sw,$wc,$pl) = (
			$data->{WCCC},
			$data->{WCST},
			$data->{SWCLLI},
			$data->{WCCLLI},
			$data->{PLCLLI},
		);
		if (my $recs = $corrections{corx}{SW}{$cc}{$st}{$sw}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $data->{SWCLLI} = undef; last }
				$data->{$cor->[0]} = $cor->[2];
			}
		}
		if (my $recs = $corrections{corx}{WC}{$cc}{$st}{$wc}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $data->{WCCLLI} = undef; last }
				$data->{$cor->[0]} = $cor->[2];
			}
		}
		if (my $recs = $corrections{corx}{PL}{$cc}{$st}{$pl}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $data->{PLCLLI} = undef; last }
				$data->{$cor->[0]} = $cor->[2];
			}
		}
		my $pc = $data->{SPC};
		if (my $recs = $corrections{corx}{PC}{$pc}) {
			foreach my $cor (@{$recs}) {
				if ($cor->[0] eq '**DELETE**') { $data->{SPC} = undef; last }
				$data->{$cor->[0]} = $cor->[2];
			}
		}
		addsw($data,$data->{WCCC},$data->{WCST},$data->{SWCLLI});
		addwc($data,$data->{WCCC},$data->{WCST},$data->{WCCLLI});
		addpl($data,$data->{WCCC},$data->{WCST},$data->{PLCLLI});
		addpc($data,$data->{SPC});
	}
	{
		if (my $ex = $data->{EXCH}) {
			addex($data,$ex);
		} else {
			my ($rg,$rc) = ($data->{REGION},$data->{RCSHORT});
			if ($rg and $rc) {
				addrc($data,$rg,$rc);
			} else {
				my ($cc,$st,$rn) = ($data->{RCCC},$data->{RCST},"\U$data->{RCNAME}\E");
				if ($cc and $st and $rn) {
					addrn($data,$cc,$st,$rn);
				}
			}
		}
	}
}
close($fh);

printf STDERR "I: --------------------------\n";

sub dumpline {
	my ($of,$keys,$rec) = @_;
	my @values = ();
	foreach (@{$keys}) {
		if (exists $rec->{$_}) {
			if (ref $rec->{$_} eq 'HASH') {
				my $sep = ',';
				if ($_ eq 'FEAT') {
					$sep = ' ';
				} elsif (/(VH|LL|NAME)$/) {
					$sep = ';';
				} else {
					foreach my $k (keys %{$rec->{$_}}) {
						if ($k =~ /,/) { $sep = ';'; last; }
					}
				}
				push @values, join($sep,sort keys %{$rec->{$_}});
			} elsif (ref $rec->{$_} eq 'ARRAY') {
				my $sep = ',';
				if ($_ eq 'FEAT') {
					$sep = ' ';
				} elsif (/(VH|LL|NAME)$/) {
					$sep = ';';
				} else {
					foreach my $k (@{$rec->{$_}}) {
						if ($k =~ /,/) { $sep = ';'; last; }
					}
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

$fn = "$datadir/db.all.nogeo.csv";
if (scalar keys %db) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@db_keys),'"',"\n";
foreach my $npa (sort keys %db) {
	foreach my $nxx (sort keys %{$db{$npa}}) {
		foreach my $x (sort keys %{$db{$npa}{$nxx}}) {
			foreach my $xxxx (sort keys %{$db{$npa}{$nxx}{$x}}) {
				foreach my $yyyy (sort keys %{$db{$npa}{$nxx}{$x}{$xxxx}}) {
					dumpline($of,\@db_keys,$db{$npa}{$nxx}{$x}{$xxxx}{$yyyy});
				}
			}
		}
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/lt.nogeo.csv";
if (scalar keys %lts) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@lt_keys),'"',"\n";
foreach my $lt (sort keys %lts) {
	dumpline($of,\@lt_keys,$lts{$lt});
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/ex.nogeo.csv";
if (scalar keys %exs) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@ex_keys),'"',"\n";
foreach my $ex (sort keys %exs) {
	dumpline($of,\@ex_keys,$exs{$ex});
}
close($of);
} else {
	system("rm -fv $fn");
}


$fn = "$datadir/pc.nogeo.csv";
if (scalar keys %pcs) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@pc_keys),'"',"\n";
foreach my $pc (sort keys %pcs) {
	dumpline($of,\@pc_keys,$pcs{$pc});
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/sw.nogeo.csv";
if (scalar keys %sws) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@sw_keys),'"',"\n";
foreach my $cc (sort keys %sws) {
	foreach my $st (sort keys %{$sws{$cc}}) {
		foreach my $sw (sort keys %{$sws{$cc}{$st}}) {
			dumpline($of,\@sw_keys,$sws{$cc}{$st}{$sw});
		}
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/wc.nogeo.csv";
if (scalar keys %wcs) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@wc_keys),'"',"\n";
foreach my $cc (sort keys %wcs) {
	foreach my $st (sort keys %{$wcs{$cc}}) {
		foreach my $wc (sort keys %{$wcs{$cc}{$st}}) {
			dumpline($of,\@wc_keys,$wcs{$cc}{$st}{$wc});
		}
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/pl.nogeo.csv";
if (scalar keys %pls) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@pl_keys),'"',"\n";
foreach my $cc (sort keys %pls) {
	foreach my $st (sort keys %{$pls{$cc}}) {
		foreach my $pl (sort keys %{$pls{$cc}{$st}}) {
			dumpline($of,\@pl_keys,$pls{$cc}{$st}{$pl});
		}
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/rc.nogeo.csv";
if (scalar keys %rcs) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@rc_keys),'"',"\n";
foreach my $rg (sort keys %rcs) {
	foreach my $rc (sort keys %{$rcs{$rg}}) {
		dumpline($of,\@rc_keys,$rcs{$rg}{$rc});
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

$fn = "$datadir/rn.nogeo.csv";
if (scalar keys %rns) {
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@rn_keys),'"',"\n";
foreach my $cc (sort keys %rns) {
	foreach my $st (sort keys %{$rns{$cc}}) {
		foreach my $rn (sort keys %{$rns{$cc}{$st}}) {
			dumpline($of,\@rn_keys,$rns{$cc}{$st}{$rn});
		}
	}
}
close($of);
} else {
	system("rm -fv $fn");
}

my %cordump = (
	'RC'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $rg (sort keys %{$corrections{$t}{$db}}) {
				foreach my $rc (sort keys %{$corrections{$t}{$db}{$rg}}) {
					foreach my $rec (@{$corrections{$t}{$db}{$rg}{$rc}}) {
						print $of '"',join('","',$rg,$rc,@{$rec}),'"',"\n";
					}
				}
			}
		}
	},
	'RN'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $cc (sort keys %{$corrections{$t}{$db}}) {
				foreach my $st (sort keys %{$corrections{$t}{$db}{$cc}}) {
					foreach my $rn (sort keys %{$corrections{$t}{$db}{$cc}{$st}}) {
						foreach my $rec (@{$corrections{$t}{$db}{$cc}{$st}{$rn}}) {
							print $of '"',join('","',$cc,$st,$rn,@{$rec}),'"',"\n";
						}
					}
				}
			}
		}
	},
	'SW'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $cc (sort keys %{$corrections{$t}{$db}}) {
				foreach my $st (sort keys %{$corrections{$t}{$db}{$cc}}) {
					foreach my $sw (sort keys %{$corrections{$t}{$db}{$cc}{$st}}) {
						foreach my $rec (@{$corrections{$t}{$db}{$cc}{$st}{$sw}}) {
							print $of '"',join('","',$cc,$st,$sw,@{$rec}),'"',"\n";
						}
					}
				}
			}
		}
	},
	'WC'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $cc (sort keys %{$corrections{$t}{$db}}) {
				foreach my $st (sort keys %{$corrections{$t}{$db}{$cc}}) {
					foreach my $wc (sort keys %{$corrections{$t}{$db}{$cc}{$st}}) {
						foreach my $rec (@{$corrections{$t}{$db}{$cc}{$st}{$wc}}) {
							print $of '"',join('","',$cc,$st,$wc,@{$rec}),'"',"\n";
						}
					}
				}
			}
		}
	},
	'PL'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $cc (sort keys %{$corrections{$t}{$db}}) {
				foreach my $st (sort keys %{$corrections{$t}{$db}{$cc}}) {
					foreach my $pl (sort keys %{$corrections{$t}{$db}{$cc}{$st}}) {
						foreach my $rec (@{$corrections{$t}{$db}{$cc}{$st}{$pl}}) {
							print $of '"',join('","',$cc,$st,$pl,@{$rec}),'"',"\n";
						}
					}
				}
			}
		}
	},
	'PC'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $pc (sort keys %{$corrections{$t}{$db}}) {
				foreach my $rec (@{$corrections{$t}{$db}{$pc}}) {
					print $of '"',join('","',$pc,@{$rec}),'"',"\n";
				}
			}
		}
	},
	'EX'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $ex (sort keys %{$corrections{$t}{$db}}) {
				foreach my $rec (@{$corrections{$t}{$db}{$ex}}) {
					print $of '"',join('","',$ex,@{$rec}),'"',"\n";
				}
			}
		}
	},
	'DB'=>sub{
		my $db = shift;
		foreach my $t (qw/corx fixx/) {
			foreach my $npasw (sort keys %{$corrections{$t}{$db}}) {
				foreach my $nxx (sort keys %{$corrections{$t}{$db}{$npasw}}) {
					foreach my $x (sort keys %{$corrections{$t}{$db}{$npasw}{$nxx}}) {
						foreach my $xxxx (sort keys %{$corrections{$t}{$db}{$npasw}{$nxx}{$x}}) {
							for my $yyyy (sort keys %{$corrections{$t}{$db}{$npasw}{$nxx}{$x}{$xxxx}}) {
								foreach my $rec (@{$corrections{$t}{$db}{$npasw}{$nxx}{$x}{$xxxx}{$yyyy}}) {
									print $of '"',join('","',$npasw,$nxx,$x,$xxxx,$yyyy,@{$rec}),'"',"\n";
								}
							}
						}
					}
				}
			}
		}
	},
);

if ($corrected > 0) {
	foreach my $db (@cordbs) {
		$fn = "$datadir/\L$db\E.fixx.csv";
		if ((exists $corrections{corx}{$db} and scalar keys %{$corrections{corx}{$db}}) or
		    (exists $corrections{fixx}{$db} and scalar keys %{$corrections{fixx}{$db}})) {
			print STDERR "I: writing $fn\n";
			open($of,">:utf8",$fn) or die "can't write $fn";
			print $of '"',join('","',@{$corkeys{$db}},'FIELD','OLD','NEW','COMMENT','REFERENCE'),'"',"\n";
			&{$cordump{$db}}($db);
			close($of);
		} else {
			system("rm -fv $fn");
		}
	}
}

exit;

1;

__END__

