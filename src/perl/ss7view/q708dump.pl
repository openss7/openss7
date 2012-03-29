#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

use Data::Dumper;
use XML::Dumper;
use XML::Simple;

local $fh = \*INFILE;

binmode(STDOUT,':utf8');
binmode(INFILE,':utf8');

my $var = {};

my %cmap = ();
my %ccod = ();

open($fh,"<","$progdir/ccodes.txt");
while (<$fh>) { chomp;
	my ($a2,$a3,$n3,$name) = split(/\t/,$_,4);
	$cmap{$name} = [ $a2, $a3, $n3 ];
	$ccod{$a2} = $name;
	#printf "%s=>'%s',\n", $a2, $name;
}
close($fh);

my %owns = ();
my %ownr = ();

open($fh,"<","$progdir/owns.txt");
while (<$fh>) { chomp;
	my ($snam,$lnam) = split(/\t/,$_,2);
	$owns{$lnam} = $snam;
	$ownr{$snam} = $lnam;
	#printf "'%s'=>'%s',\n", $snam, $lnam;
}
close($fh);

open($fh,"<","$progdir/q708a.txt");
while (<$fh>) { chomp;
	unless (/^[2-7]-/) {
		#print $_, "\n";
		next;
	}
	my ($sanc,$name) = (split(/\t/,$_,2));
	my $cc = $cmap{$name};
	unless (defined $cc) {
		warn "cannot find country code for '$name'";
	}
	my ($a2,$a3,$n3) = @$cc;
	unless ($a2 and $a2 ne '-') {
		warn "no 2-digit country code for '$name'";
	}
	#printf "%s => ['%s','%s','%s','%s'],\n", $sanc,$a2,$a3,$n3,$name;
	my ($zone,$region) = (split(/-/,$sanc,2));
	$var->{$zone}->{$region} = [ $a2,$a3,$n3,$name,{}, ];
}
close($fh);

open($fh,"<","$progdir/q708b.txt");
while (<$fh>) { chomp;
	unless (/^[2-7]-/) {
		#print $_, "\n";
		next;
	}
	my ($pc,$num,$name,$owner) = (split(/\t/,$_,4));
	#printf "%s => ['%s',\t\t\t'%s'],\n", $pc,$name,$owner;
	my ($zone,$region,$point) = (split(/-/,$pc,3));
	my $snam = $owns{$owner};
	$var->{$zone}->{$region}->[2]->{$point} = {nam=>$name,own=>$owner,lab=>$snam};
}
close($fh);

open($fh,">","$progdir/sancs.pl");
print $fh <<'EOF';
$VAR1 = {
EOF

foreach my $zone (sort {$a<=>$b} keys %$var) {
	print $fh "\t$zone=>{\n";
	foreach my $region (sort {$a<=>$b} keys %{$var->{$zone}}) {
		my $a2 = $var->{$zone}->{$region}->[0]; $a2 =~ s/\\/\\\\/g; $a2 =~ s/'/\\'/g;
		my $a3 = $var->{$zone}->{$region}->[1]; $a3 =~ s/\\/\\\\/g; $a3 =~ s/'/\\'/g;
		my $n3 = $var->{$zone}->{$region}->[2]; $n3 =~ s/\\/\\\\/g; $n3 =~ s/'/\\'/g;
		my $cn = $var->{$zone}->{$region}->[3]; $cn =~ s/\\/\\\\/g; $cn =~ s/'/\\'/g;
		printf $fh "\t\t%3d=>['%s','%s','%s','%s',{\n", $region, $a2, $a3, $n3, $cn;
		foreach my $point (sort {$a<=>$b} keys %{$var->{$zone}->{$region}->[2]}) {
			my $nam = $var->{$zone}->{$region}->[2]->{$point}->{nam}; $nam =~ s/\\/\\\\/g; $nam =~ s/'/\\'/g;
			$nam = '' if $nam eq '...'; $nam = '' if $nam eq '-'; $nam = '' if $nam eq '\\';
			my $own = $var->{$zone}->{$region}->[2]->{$point}->{own}; $own =~ s/\\/\\\\/g; $own =~ s/'/\\'/g;
			$own = '' if $own eq '...'; $own = '' if $own eq '-'; $own = '' if $own eq '\\';
			my $lab = $var->{$zone}->{$region}->[2]->{$point}->{lab}; $lab =~ s/\\/\\\\/g; $lab =~ s/'/\\'/g;
			$lab = '' if $lab eq '...'; $lab = '' if $lab eq '-'; $lab = '' if $lab eq '\\';
			printf $fh "\t\t\t%d=>{nam=>'%s',\town=>'%s',\tlab=>'%s'},\n", $point, $nam, $own, $lab;
		}
		print $fh "\t\t}],\n";
	}
	print $fh "\t},\n";
}

print $fh <<'EOF';
};
EOF
close($fh);

