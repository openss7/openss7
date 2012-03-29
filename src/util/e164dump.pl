#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

use Data::Dumper;
use XML::Dumper;
use XML::Simple;

local $fh = \*INFILE;
local $of = \*OUTFILE;

#binmode(INFILE,':utf8');
#binmode(STDOUT,':utf8');
#binmode(STDERR,':utf8');

my $var = {};

$var->{ansi} = {};
$var->{q708} = {};
$var->{e164} = {};
$var->{e212} = {};
$var->{iso3166} = {};

my %cmap = ();

open($fh,"<","$progdir/ccodes.txt");
while (<$fh>) { chomp;
	my ($a2,$a3,$n3,$loc) = split(/\t/,$_,4);
	$cmap{$loc} = { a2=>$a2, a3=>$a3, n3=>$n3 };
	if (exists $var->{iso3166}->{$a2}) {
		push @{$var->{iso3166}->{$a2}->{names}}, $loc;
	} else {
		$var->{iso3166}->{$a2}->{a2} = $a2;
		$var->{iso3166}->{$a2}->{a3} = $a3;
		$var->{iso3166}->{$a2}->{n3} = $n3;
		$var->{iso3166}->{$a2}->{names} = [ $loc ];
	}
}
close($fh);

my %smap = ();

open($fh,"<","$progdir/states.txt");
while (<$fh>) { chomp;
	my ($a2,$st,$loc,$tz) = split(/\t/,$_);
	$smap{$loc} = { a2=>$a2, st=>$st };
	$smap{$loc}->{tz} = $tz if defined $tz;
	unless (exists $var->{iso3166}->{$a2}) {
		warn "cannot find country code '$a2'";
		next;
	}
	if (!$st or $st eq '--') {
		push @{$var->{iso3166}->{$a2}->{names}}, $loc;
		next;
	}
	if ($st =~ /,/) {
		$st = [ split(/,/,$st) ];
	} else {
		$st = [ $st ];
	}
	foreach my $s (@$st) {
		my $val;
		if (exists $var->{iso3166}->{$a2}->{states}->{$s}) {
			$val = $var->{iso3166}->{$a2}->{states}->{$s};
		} else {
			$val = {};
			$var->{iso3166}->{$a2}->{states}->{$s} = $val;
		}
		push @{$val->{names}}, $loc;
	}
}
close($fh);

my %abbv = {};
open($fh,"<","$progdir/owns.txt");
while (<$fh>) { chomp;
	my ($sn,$op) = split(/\t/,$_);
	$abbv{$op} = $sn;
}
close($fh);

my %notes;
%notes = ();

open($fh,"<","$progdir/e164d.txt");
while (<$fh>) { chomp;
	if (/^[0-9]+[0-9-]*\t/) {
		my ($code,$loc,$note) = split(/\t/,$_);
		next if $loc eq 'Spare code';
		next if $loc =~ /reserved/i;
		my ($a2,$a3,$n3) = ('ZZ','-','-');
		unless ($loc =~ /shared code/i or $loc =~ /reserved/i) {
			my $cc = $cmap{$loc};
			unless (defined $cc) {
				warn "cannot find country code for '$loc'";
			} else {
				$a2 = $cc->{a2};
				$a3 = $cc->{a3};
				$n3 = $cc->{n3};
				unless ($a2) {
					warn "no country code for '$loc'";
				}
			}
		}
		my $sub = {a2=>$a2,loc=>$loc}; $sub->{note} = $note if $note;
		if ($code =~ /-/) {
			my ($pcode,$scode) = split(/-/,$code,2);
			$var->{e164}->{$pcode}->{area}->{$scode} = $sub;
		} else {
			$var->{e164}->{$code}->{a2} = $a2;
			$var->{e164}->{$code}->{loc} = $loc;
			$var->{e164}->{$code}->{note} = $note if $note;
		}
		next;
	}
	if (/^[a-z]\t/) {
		my ($letter,$text) = split(/\t/,$_,2);
		$notes{$letter} = $text;
		next;
	}
}
close($fh);

sub notem {
	my ($var) = @_;
	foreach (keys %$var) {
		my $val = $var->{$_};
		$val->{note} = $notes{$val->{note}} if $val->{note};
		notem($val->{area}) if $val->{area};
	}
}
notem($var->{e164});

my %dials = ();

open($fh,"<","$progdir/e164c.txt");
while (<$fh>) { chomp;
	next if /^$/ or /^#/ or /^\t[[:upper:]]/;
	if (/^[^\t]+\t/) {
		my ($loc,$code,$idp,$ndp,$digs,$tz,$dial) = split(/\t/,$_);
		my ($a2,$a3,$n3) = ('ZZ','-','-');
		unless ($loc =~ /shared code/i or $loc =~ /reserved/i) {
			my $cc = $cmap{$loc};
			unless (defined $cc) {
				if (exists $var->{e164}->{$code}->{a2}) {
					$a2 = $var->{e164}->{$code}->{a2};
				} else {
					warn "cannot find country code for '$loc'";
					warn "cannot find country code $code";
				}
			} else {
				$a2 = $cc->{a2};
				$a3 = $cc->{a3};
				$n3 = $cc->{n3};
				unless ($a2) {
					warn "no country code for '$loc'";
				}
			}
		}
		if ($digs) {
			$digs =~ s/up  *to  */0-/g;
			$digs =~ s/  *to  */-/g;
			$digs =~ s/  *digits//g;
			$digs =~ s/,  */,/g;
			$digs =~ s/ *\+ */+/g;
			$digs =~ s/  *//g;
		}
		my $dst;
		if ($tz =~ /\//) {
			($tz,$dst) = split(/\//,$tz,2);
		}
		my $scode;
		if ($digs =~ /\(([0-9]+(\/[0-9]+)*)\)\+?/) {
			$scode = $1;
			#$digs =~ s/\([0-9]+(\/[0-9]+)*\)\+?//;
			if ($scode =~ /\//) {
				$scode = [ split(/\//,$scode) ];
			} else {
				$scode = [ $scode ];
			}
		}
		if ($a2 eq 'CA' and !defined $scode) {
			$scode = [ 204, 226, 236, 249, 250, 289, 306, 343, 365, 403, 416, 418, 431, 437, 438, 450, 506, 514, 519, 579, 581, 587, 604, 613, 639, 647, 672, 705, 709, 778, 780, 807, 819, 825, 867, 873, 902, 905 ];
		} elsif ($a2 eq 'KZ' and !defined $scode) {
			$scode = [ 6, 7 ];
		}
		if (defined $scode) {
			foreach my $sc (@$scode) {
				my $val;
				if (exists $var->{e164}->{$code}->{area}->{$sc}) {
					$val = $var->{e164}->{$code}->{area}->{$sc};
				} else {
					$val = {};
					$var->{e164}->{$code}->{area}->{$sc} = $val;
				}
				$val->{a2} = $a2 unless $val->{a2};
				$val->{loc} = $loc unless $val->{loc};
				$val->{idp} = $idp if defined $idp and $idp ne '...' and $idp ne '';
				$val->{ndp} = $ndp if defined $ndp and $ndp ne '...' and $ndp ne '';
				$val->{digs} = $digs if defined $digs and $digs ne '';
				$val->{tz} = $tz if defined $tz and $tz ne '';
				$val->{dst} = $dst if defined $dst and $dst ne '';
				$val->{dial} = $dial if defined $dial and !$val->{dial};
			}
		} else {
			my $val;
			if (exists $var->{e164}->{$code}) {
				$val = $var->{e164}->{$code};
			} else {
				$val = {};
				$var->{e164}->{$code} = $val;
			}
			$val->{a2} = $a2 unless $val->{a2};
			$val->{loc} = $loc unless $val->{loc};
			$val->{idp} = $idp if defined $idp and $idp ne '...' and $idp ne '';
			$val->{ndp} = $ndp if defined $ndp and $ndp ne '...' and $ndp ne '';
			$val->{digs} = $digs if defined $digs and $digs ne '';
			$val->{tz} = $tz if defined $tz and $tz ne '';
			$val->{dst} = $dst if defined $dst and $dst ne '';
			$val->{dial} = $dial if defined $dial and !$val->{dial};
		}
		next;
	}
	if (/^\t[0-9]+\t/) {
		my ($dummy,$num,$dial) = split(/\t/,$_);
		$dials{$num} = $dial;
		next;
	}
	warn "cannot process line: '$_'";
}
close($fh);

sub descem {
	my ($var) = @_;
	foreach (keys %$var) {
		my $val = $var->{$_};
		if (exists $val->{dial}) {
			warn "no description for note $val->{dial} for code $_" unless exists $dials{$val->{dial}};
			$val->{dial} = $dials{$val->{dial}};
		}
		descem($val->{area}) if $val->{area};
	}
}
descem($var->{e164});

open($fh,"<","$progdir/nanp.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($npa,$loc) = split(/\t/,$_);
	my $state = $smap{$loc};
	unless ($state) {
		warn "cannot find state code for '$loc'";
		next;
	}
	my $cc = 1;
	my $a2 = $state->{a2};
	my $st = $state->{st};
	my $tz = $state->{tz} if exists $state->{tz};
	my $dst;
	if ($tz =~ /\//) {
		($tz,$dst) = split(/\//,$tz,2);
	}
	my $val;
	if (exists $var->{e164}->{$cc}->{area}->{$npa}) {
		$val = $var->{e164}->{$cc}->{area}->{$npa};
	} else {
		if ($a2 ne 'US' and $a2 ne 'CA' and $a2 ne 'ZZ') {
			warn "no ITU entry for '$a2' '$npa'";
		}
		$val = {};
		$var->{e164}->{$cc}->{area}->{$npa} = $val;
	}
	$val->{a2} = $a2 unless $val->{a2};
	$val->{st} = $st unless $val->{st};
	$val->{loc} = $loc unless $val->{loc};
	$val->{idp} = '011' unless defined $val->{idp};
	$val->{ndp} = '1'   unless defined $val->{ndp};
	$val->{digs} = 10   unless defined $val->{digs};
	$val->{tz} = $tz if !defined $val->{tz} and defined $tz and $tz ne '';
	$val->{dst} = $dst if !defined $val->{dst} and defined $dst and $dst ne '';
}
close($fh);

%notes = ();

open($fh,"<","$progdir/e212a.txt");
while (<$fh>) { chomp;
	if (/^[0-9]+\t/) {
		my ($code,$loc,$note) = split(/\t/,$_);
		next if $loc eq 'Spare code';
		next if $loc =~ /reserved/i;
		my ($a2,$a3,$n3) = ('ZZ','-','-');
		unless ($loc =~ /shared code/i or $loc =~ /reserved/i) {
			my $cc = $cmap{$loc};
			unless (defined $cc) {
				warn "cannot find country code for '$loc'";
			} else {
				$a2 = $cc->{a2};
				$a3 = $cc->{a3};
				$n3 = $cc->{n3};
				unless ($a2) {
					warn "no country code for '$loc'";
				}
			}
		}
		my $sub = {a2=>$a2,loc=>$loc}; $sub->{note} = $note if $note;
		if ($code =~ /-/) {
			my ($pcode,$scode) = split(/-/,$code,2);
			$var->{e212}->{$pcode}->{area}->{$scode} = $sub;
		} else {
			$var->{e212}->{$code}->{a2} = $a2;
			$var->{e212}->{$code}->{loc} = $loc;
			$var->{e212}->{$code}->{note} = $note if $note;
		}
		next;
	}
	if (/^[a-z]\t/) {
		my ($letter,$text) = split(/\t/,$_,2);
		$notes{$letter} = $text;
		next;
	}
}
close($fh);

notem($var->{e212});

open($fh,"<","$progdir/e212b.txt");
while (<$fh>) { chomp;
	if (/^[0-9]+\t/) {
		my ($code,$area,$op) = split(/\t/,$_);
		my $val = {op=>$op};
		my $sn = $abbv{$op};
		unless ($sn) {
			warn "no abbreviation for '$op'";
		}
		$val->{sn} = $sn if $sn;
		$var->{e212}->{$code}->{area}->{$area} = $val;
	}
}
close($fh);

open($fh,"<","$progdir/q708a.txt");
while (<$fh>) { chomp;
	if (/^[0-9]+[0-9-]*\t/) {
		my ($sanc,$loc,$note) = split(/\t/,$_);
		my ($zone,$region) = split(/-/,$sanc);
		my $cc = $cmap{$loc};
		unless (defined $cc) {
			warn "cannot find country code for '$loc'";
		} else {
			$a2 = $cc->{a2};
			$a3 = $cc->{a3};
			$n3 = $cc->{n3};
			unless ($a2) {
				warn "no country code for '$loc'";
			}
		}
		my $val = {a2=>$a2,loc=>$loc};
		$val->{note} = $node if $note;
		$var->{q708}->{$zone}->{$region} = $val;
	}
}
close($fh);

open($fh,"<","$progdir/q708b.txt");
while (<$fh>) { chomp;
	if (/^[0-9]+[0-9-]*\t/) {
		my ($pc,$num,$name,$op) = split(/\t/,$_);
		my ($zone,$region,$node) = split(/-/,$pc);
		my $val = {name=>$name,op=>$op};
		my $sn = $abbv{$op};
		unless ($sn) {
			warn "no abbreviation for '$op'";
		}
		$val->{sn} = $sn if $sn;
		$var->{q708}->{$zone}->{$region}->{spid}->{$node} = $val;
	}
}
close($fh);

open($fh,"<","$progdir/t1.111.8a.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($net,$op,$note) = split(/\t/,$_);
	my $val = {op=>$op};
	my $sn = $abbv{$op};
	unless ($sn) {
		warn "no abbreviation for '$op'";
	}
	$val->{sn} = $sn if $sn;
	$val->{note} = $note if $note;
	$var->{ansi}->{$net} = $val;
}
close($fh);

if (0) {
	for (my $net=1;$net<5;$net++) {
		next if exists $var->{ansi}->{$net};
		$var->{ansi}->{$net} = {sn=>'SNC',op=>'Small network code'};
	}
	$var->{ansi}->{5} = {sn=>'PCB',op=>'Point code blocks'} unless exists $var->{ansi}->{5};
	for (my $net=6;$net<255;$net++) {
		next if exists $var->{ansi}->{$net};
		$var->{ansi}->{$net} = {sn=>'LNC',op=>'Large network code',note=>'Unassigned'};
	}
}

open($fh,"<","$progdir/t1.111.8b.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($net,$clus,$op,$note) = split(/\t/,$_);
	my $val = {op=>$op};
	my $sn = $abbv{$op};
	unless ($sn) {
		warn "no abbreviation for '$op'";
	}
	$val->{sn} = $sn if $sn;
	$val->{note} = $note if $note;
	$var->{ansi}->{$net}->{cluster}->{$clus} = $val;
}
close($fh);

open($fh,"<","$progdir/t1.111.8c.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($clus,$cn,$an,$desc) = split(/\t/,$_);
	my $net = 5;
	my $val = {a2=>$cn,area=>$an};
	$val->{desc} = $desc;
	$var->{ansi}->{$net}->{cluster}->{$clus} = $val;
}
close($fh);

open($fh,"<","$progdir/t1.111.8d.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($clus,$block,$op,$note) = split(/\t/,$_);
	my $net = 5;
	my $val = {op=>$op};
	my $sn = $abbv{$op};
	unless ($sn) {
		warn "no abbreviation for '$op'";
	}
	$val->{sn} = $sn if $sn;
	$val->{note} = $note if $note;
	$var->{ansi}->{$net}->{cluster}->{$clus}->{block}->{$block} = $val;
}
close($fh);


sub listem {
	my ($var,$tab) = @_;
	foreach my $code (sort keys %$var) {
		my $join = '';
		my $val = $var->{$code};
		if ($code) {
			printf $of $tab."%3d=>{", $code;
		} else {
			printf $of $tab." ''=>{";
		}
		my $a2 = $val->{a2};
		if ($a2) {
			$a2 =~ s/\\/\\\\/g; $a2 =~ s/'/\\'/g;
			printf $of $join."a2=>'%s'", $a2;
			$join = ',';
		}
		my $st = $val->{st};
		if ($st) {
			$st =~ s/\\/\\\\/g; $st =~ s/'/\\'/g;
			printf $of $join."st=>'%s'", $st;
			$join = ',';
		}
		my $cn = $val->{loc};
		if ($cn and (!$a2 or $a2 eq '-' or $a2 eq 'ZZ' or $st eq '--')) {
			$cn =~ s/\\/\\\\/g; $cn =~ s/'/\\'/g;
			printf $of $join."loc=>'%s'", $cn;
			$join = ',';
		}
		if (defined (my $idp = $val->{idp})) {
			$idp =~ s/\\/\\\\/g; $idp =~ s/'/\\'/g;
			printf $of $join."idp=>'%s'", $idp;
			$join = ',';
		}
		if (defined (my $ndp = $val->{ndp})) {
			$ndp =~ s/\\/\\\\/g; $ndp =~ s/'/\\'/g;
			printf $of $join."ndp=>'%s'", $ndp;
			$join = ',';
		}
		if (my $digs = $val->{digs}) {
			$digs =~ s/\\/\\\\/g; $digs =~ s/'/\\'/g;
			printf $of $join."digs=>'%s'", $digs;
			$join = ',';
		}
		if (my $tz = $val->{tz}) {
			$tz =~ s/\\/\\\\/g; $tz =~ s/'/\\'/g;
			printf $of $join."tz=>'%s'", $tz;
			$join = ',';
		}
		if (my $dst = $val->{dst}) {
			$dst =~ s/\\/\\\\/g; $dst =~ s/'/\\'/g;
			printf $of $join."dst=>'%s'", $dst;
			$join = ',';
		}
		if (my $sn = $val->{sn}) {
			$sn =~ s/\\/\\\\/g; $sn =~ s/'/\\'/g;
			printf $of $join."sn=>'%s'", $sn;
			$join = ',';
		}
		if (my $op = $val->{op}) {
			$op =~ s/\\/\\\\/g; $op =~ s/'/\\'/g;
			printf $of $join."op=>'%s'", $op;
			$join = ',';
		}
		if (my $no = $val->{note}) {
			$no =~ s/\\/\\\\/g; $no =~ s/'/\\'/g;
			printf $of $join."note=>'%s'", $no;
			$join = ',';
		}
		if (my $de = $val->{dial}) {
			$de =~ s/\\/\\\\/g; $de =~ s/'/\\'/g;
			printf $of $join."dial=>'%s'", $de;
			$join = ',';
		}
		if ($val->{area}) {
			print $of $join."area=>{\n";
			listem($val->{area},$tab."\t");
			print $of $tab."}";
		}
		print $of "},\n";
	}
}

open($of,">","$progdir/assigments.pl");
print $of <<'EOF';
$VAR1 = {
EOF

	print $of "\tiso3166=>{\n";
	foreach (sort keys %{$var->{iso3166}}) {
		my $val = $var->{iso3166}->{$_};
		printf $of "\t\t$_=>{a2=>'%s',a3=>'%s',n3=>'%03d',names=>[\n",
			$val->{a2},$val->{a3},$val->{n3};
		foreach my $name (@{$val->{names}}) {
			$name =~ s/\\/\\\\/g; $name =~ s/'/\\'/g;
			printf $of "\t\t\t'%s',\n", $name;
		}
		print $of "\t\t]";
		if ($val->{states}) {
			print $of ",states=>[\n";
			foreach my $state (sort keys %{$val->{states}}) {
				my $sta = $val->{states}->{$state};
				printf $of "\t\t\t%s=>{names=>[\n", $state;
				foreach my $name (@{$sta->{names}}) {
					$name =~ s/\\/\\\\/g; $name =~ s/'/\\'/g;
					printf $of "\t\t\t\t'%s',\n", $name;
				}
				print $of "\t\t\t]},\n";
			}
			print $of "\t\t]";
		}
		print $of "},\n";
	}
	print $of "\t},\n";

foreach (qw(e164 e212)) {
	print $of "\t$_=>{\n";
	listem($var->{$_},"\t\t");
	print $of "\t},\n";
}

	print $of "\tq708=>{\n";
	foreach (sort {$a<=>$b} keys %{$var->{q708}}) {
		my $val = $var->{q708}->{$_};
		printf $of "\t\t%1d=>{\n", $_;
		foreach my $region (sort {$a<=>$b} keys %$val) {
			my $reg = $val->{$region};
			printf $of "\t\t\t%3d=>{", $region;
			my $a2 = $reg->{a2};   $a2 =~ s/\\/\\\\/g; $a2 =~ s/'/\\'/g;
			my $cn = $reg->{loc};  $cn =~ s/\\/\\\\/g; $cn =~ s/'/\\'/g;
			my $no = $reg->{note}; $no =~ s/\\/\\\\/g; $no =~ s/'/\\'/g;
			my $join = '';
			if ($a2) {
				printf $of $join."a2=>'%s'", $a2;
				$join = ',';
			}
			if ($cn and (!$a2 or $a2 eq 'ZZ')) {
				printf $of $join."loc=>'%s'", $cn;
				$join = ',';
			}
			if ($no) {
				printf $of $join."note=>'%s'", $no;
				$join = ',';
			}
			if ($reg->{spid}) {
				print $of $join."spid=>{\n";
				foreach my $node (sort {$a<=>$b} keys %{$reg->{spid}}) {
					my $nod = $reg->{spid}->{$node};
					printf $of "\t\t\t\t%d=>{", $node;
					$join = '';
					if (my $na = $nod->{name}) {
						$na =~ s/\\/\\\\/g; $na =~ s/'/\\'/g;
						$na = '' if $na eq '...'; $na = '' if $na eq '-';
						printf $of $join."name=>'%s'", $na;
						$join = ',';
					}
					if (my $sn = $nod->{sn}) {
						$sn =~ s/\\/\\\\/g; $sn =~ s/'/\\'/g;
						printf $of $join."sn=>'%s'", $sn;
						$join = ',';
					}
					if (my $op = $nod->{op}) {
						$op =~ s/\\/\\\\/g; $op =~ s/'/\\'/g;
						$op = '' if $op eq '...'; $op = '' if $op eq '-';
						printf $of $join."op=>'%s'", $op;
						$join = ',';
					}
					printf $of "},\n";
				}
				print $of "\t\t\t}";
			}
			print $of "},\n";
		}
		print $of "\t\t},\n";
	}
	print $of "\t},\n";


	print $of "\tansi=>{\n";
	foreach my $net (sort {$a<=>$b} keys %{$var->{ansi}}) {
		my $val = $var->{ansi}->{$net};
		printf $of "\t\t%3d=>{", $net;
		my $join = '';
		if (my $sn = $val->{sn}) {
			$sn =~ s/\\/\\\\/g; $sn =~ s/'/\\'/g;
			printf $of $join."sn=>'%s'", $sn;
			$join = ',';
		}
		if (my $op = $val->{op}) {
			$op =~ s/\\/\\\\/g; $op =~ s/'/\\'/g;
			printf $of $join."op=>'%s'", $op;
			$join = ',';
		}
		if (my $no = $val->{note}) {
			$no =~ s/\\/\\\\/g; $no =~ s/'/\\'/g;
			printf $of $join."note=>'%s'", $no;
			$join = ',';
		}
		if ($val->{cluster}) {
			print $of $join."cluster=>{\n";
			foreach my $clus (sort {$a<=>$b} keys %{$val->{cluster}}) {
				my $clu = $val->{cluster}->{$clus};
				printf $of "\t\t\t%3d=>{", $clus;
				$join = '';
				my $c = $clu->{a2};
				my $p = $clu->{area};
				my $d = $clu->{desc};
				if ($c) {
					$c =~ s/\\/\\\\/g; $c =~ s/'/\\'/g;
					printf $of $join."a2=>'%s'", $c;
					$join = ',';
				}
				if ($p) {
					$p =~ s/\\/\\\\/g; $p =~ s/'/\\'/g;
					printf $of $join."area=>'%s'", $p;
					$join = ',';
				}
				if ($d and (!$c or $c eq 'ZZ')) {
					$d =~ s/\\/\\\\/g; $d =~ s/'/\\'/g;
					printf $of $join."desc=>'%s'", $d;
					$join = ',';
				}
				if (my $s = $clu->{sn}) {
					$s =~ s/\\/\\\\/g; $s =~ s/'/\\'/g;
					printf $of $join."sn=>'%s'", $s;
					$join = ',';
				}
				if (my $l = $clu->{op}) {
					$l =~ s/\\/\\\\/g; $l =~ s/'/\\'/g;
					printf $of $join."op=>'%s'", $l;
					$join = ',';
				}
				if (my $n = $clu->{note}) {
					$n =~ s/\\/\\\\/g; $n =~ s/'/\\'/g;
					printf $of $join."note=>'%s'", $n;
					$join = ',';
				}
				if ($clu->{block}) {
					print $of $join."block=>{\n";
					foreach my $block (sort {$a<=>$b} keys %{$clu->{block}}) {
						my $blk = $clu->{block}->{$block};
						printf $of "\t\t\t\t%3d=>{", $block;
						$join = '';
						if (my $s = $blk->{sn}) {
							$s =~ s/\\/\\\\/g; $s =~ s/'/\\'/g;
							printf $of $join."sn=>'%s'", $s;
							$join = ',';
						}
						if (my $l = $blk->{op}) {
							$l =~ s/\\/\\\\/g; $l =~ s/'/\\'/g;
							printf $of $join."op=>'%s'", $l;
							$join = ',';
						}
						if (my $n = $blk->{note}) {
							$n =~ s/\\/\\\\/g; $n =~ s/'/\\'/g;
							printf $of $join."note=>'%s'", $n;
							$join = ',';
						}
						print $of "},\n";
					}
					print $of "\t\t\t}";
				}
				print $of "},\n";
			}
			print $of "\t\t}";
		}
		print $of "},\n";
	}
	print $of "\t},\n";

print $of <<'EOF';
};
EOF
close($of);

#my $dumper = new XML::Dumper;
#$dumper->pl2xml($var, "$progdir/assigments.xml.gz");
