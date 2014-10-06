#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/data";

use Data::Dumper;

my $var = {};

$var->{numplan} = {};
$var->{city} = {};
$var->{town} = {};
$var->{county} = {};
$var->{other} = {};

local $fh = \*INFILE;
local $of = \*OUTFILE;

foreach my $type (qw/city town county other/) {
	open($fh,"<","$datadir/$type.tmp");
	while (<$fh>) { chomp;
		$var->{$type}->{$_} = 1;
	}
	close($fh);
}
foreach (keys %{$var->{city}}) {
	delete $var->{town}->{$_};
}

open($fh,"<","$datadir/numplan.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9]+\t/;
	#print STDERR "processing '$_'\n";
	my ($num,$locs,$plan) = split(/\t/,$_,3);
	if ($locs =~ /;/) {
		$locs = [ split(/;/,$locs) ];
	} else {
		$locs = [ $locs ];
	}
	foreach my $loc (@$locs) {
		#print STDERR "processing loc '$loc'\n";
		my $digs;
		if ($loc =~ /^(.*)\(([0-9X,]+)\)$/) {
			$digs = [ split(/,/,$2) ];
			$loc = $1; $loc =~ s/ +$//;
		}
		my $where = $loc;
		$where =~ s/-/ /g unless $where =~ / - /;
		$where =~ s/ \([^\)]*\)//g;
		my $type;
		foreach (qw/city town county other/) {
			if (exists $var->{$_}->{$where}) {
				$type = $_;
				last;
			}
		}
		if ($digs) {
			foreach my $dig (@$digs) {
				#print STDERR "processing digits '$dig'\n";
				$dig =~ s/X+$//;
				my $pref = $num.$dig;
				my $val;
				if (exists $var->{numplan}->{$pref}) {
					warn "record exists for '$pref'";
					$val = $var->{numplan}->{$pref};
				} else {
					$val = {};
					$var->{numplan}->{$pref} = $val;

				}
				$val->{loc} = $loc;
				$val->{plan} = $plan;
				if (defined $type) {
					$val->{type} = $type;
				} else {
					warn "no type for '$loc'";
				}
			}
		} else {
			if (exists $var->{numplan}->{$num}) {
				warn "record exists for '$num'";
				$val = $var->{numplan}->{$num};
			} else {
				$val = {};
				$var->{numplan}->{$num} = $val;
			}
			$val->{loc} = $loc;
			$val->{plan} = $plan;
			$val->{type} = $type if defined $type;
			if (defined $type) {
				$val->{type} = $type;
			} else {
				warn "no type for '$loc'";
			}
		}
	}
}
close($fh);

open($of,">","$datadir/numplan.tst");
print $of <<'EOF';
$var = {
EOF
print $of <<'EOF';
	city=>{
EOF
foreach (sort keys %{$var->{city}}) {
	s/\\/\\\\/g; s/'/\\'/g;
	printf $of "\t\t'%s'=>1,\n",$_;
}
print $of <<'EOF';
	},
EOF
print $of <<'EOF';
	town=>{
EOF
foreach (sort keys %{$var->{town}}) {
	s/\\/\\\\/g; s/'/\\'/g;
	printf $of "\t\t'%s'=>1,\n",$_;
}
print $of <<'EOF';
	},
EOF
print $of <<'EOF';
	county=>{
EOF
foreach (sort keys %{$var->{county}}) {
	s/\\/\\\\/g; s/'/\\'/g;
	printf $of "\t\t'%s'=>1,\n",$_;
}
print $of <<'EOF';
	},
EOF
print $of <<'EOF';
	other=>{
EOF
foreach (sort keys %{$var->{other}}) {
	s/\\/\\\\/g; s/'/\\'/g;
	printf $of "\t\t'%s'=>1,\n",$_;
}
print $of <<'EOF';
	},
EOF
print $of <<'EOF';
	numplan=>{
EOF
foreach (sort keys %{$var->{numplan}}) {
	my $loc  = $var->{numplan}->{$_}->{loc};
	my $plan = $var->{numplan}->{$_}->{plan};
	my $type = $var->{numplan}->{$_}->{type};
	$loc =~ s/\\/\\\\/g; $loc =~ s/'/\\'/g;
	printf $of "\t'%s'=>{loc=>'%s',plan=>'%s'", $_, $loc, $plan;
	printf $of ",type=>'%s'", $type if $type;
	print $of "},\n";
}
print $of <<'EOF';
	},
};
EOF
close($of);
