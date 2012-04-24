#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

use strict;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir = '.' unless $progdir =~ /\//; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/..";
my $nanpdir = "$progdir/../nanpa";
my $necadir = "$progdir/../neca4";
my $locldir = "$progdir/../localcallingguide";
my $telcdir = "$progdir/../telcodata";
my $pingdir = "$progdir/../pinglo";
my $areadir = "$progdir/../areacodes";

my $fh = \*INFILE;
my $fn;

my %npas_rcs = ();
my @npas_fields = ();
my $npas_count = 0;
$fn = "$progdir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or "die can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@npas_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		$npas_rcs{$region}{$rcshort} = {} unless exists $npas_rcs{$region}{$rcshort};
		my $rec = $npas_rcs{$region}{$rcshort};
		for (my $i=0;$i<@npas_fields;$i++) {
			$rec->{$npas_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$npas_count++;
	}
	close($fh);
	print STDERR "I: RCs for npas: $npas_count\n";
}

my %nanp_rcs = ();
my @nanp_fields = ();
my $nanp_count = 0;
$fn = "$nanpdir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@nanp_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		$nanp_rcs{$region}{$rcshort} = {} unless exists $nanp_rcs{$region}{$rcshort};
		my $rec = $nanp_rcs{$region}{$rcshort};
		for (my $i=0;$i<@nanp_fields;$i++) {
			$rec->{$nanp_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$nanp_count++;
	}
	close($fh);
	print STDERR "I: RCs for nanp: $nanp_count\n";
}

my %neca_rcs = ();
my @neca_fields = ();
my $neca_count = 0;
$fn = "$necadir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@neca_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		#next if exists $nanp_rcs{$region}{$rcshort};
		$neca_rcs{$region}{$rcshort} = {} unless exists $neca_rcs{$region}{$rcshort};
		my $rec = $neca_rcs{$region}{$rcshort};
		for (my $i=0;$i<@neca_fields;$i++) {
			$rec->{$neca_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$neca_count++;
	}
	close($fh);
	print STDERR "I: RCs for neca: $neca_count\n";
}

my %locl_rcs = ();
my @locl_fields = ();
my $locl_count = 0;
$fn = "$locldir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@locl_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		#next if exists $nanp_rcs{$region}{$rcshort};
		#next if exists $neca_rcs{$region}{$rcshort};
		$locl_rcs{$region}{$rcshort} = {} unless exists $locl_rcs{$region}{$rcshort};
		my $rec = $locl_rcs{$region}{$rcshort};
		for (my $i=0;$i<@locl_fields;$i++) {
			$rec->{$locl_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$locl_count++;
	}
	close($fh);
	print STDERR "I: RCs for locl: $locl_count\n";
}

my %telc_rcs = ();
my @telc_fields = ();
my $telc_count = 0;
$fn = "$telcdir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@telc_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		#next if exists $nanp_rcs{$region}{$rcshort};
		#next if exists $neca_rcs{$region}{$rcshort};
		#next if exists $locl_rcs{$region}{$rcshort};
		$telc_rcs{$region}{$rcshort} = {} unless exists $telc_rcs{$region}{$rcshort};
		my $rec = $telc_rcs{$region}{$rcshort};
		for (my $i=0;$i<@telc_fields;$i++) {
			$rec->{$telc_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$telc_count++;
	}
	close($fh);
	print STDERR "I: RCs for telc: $telc_count\n";
}

my %ping_rcs = ();
my @ping_fields = ();
my $ping_count = 0;
$fn = "$pingdir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@ping_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		#next if exists $nanp_rcs{$region}{$rcshort};
		#next if exists $neca_rcs{$region}{$rcshort};
		#next if exists $locl_rcs{$region}{$rcshort};
		#next if exists $telc_rcs{$region}{$rcshort};
		$ping_rcs{$region}{$rcshort} = {} unless exists $ping_rcs{$region}{$rcshort};
		my $rec = $ping_rcs{$region}{$rcshort};
		for (my $i=0;$i<@ping_fields;$i++) {
			$rec->{$ping_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$ping_count++;
	}
	close($fh);
	print STDERR "I: RCs for ping: $ping_count\n";
}

my %area_rcs = ();
my @area_fields = ();
my $area_count = 0;
$fn = "$areadir/rc.csv";
if (-f $fn) {
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@area_fields = @tokens;
			$heading = undef;
			next;
		}
		my $rcshort = substr("\U$tokens[0]\E",0,10);
		my $region = $tokens[1];
		#next if exists $npas_rcs{$region}{$rcshort};
		#next if exists $nanp_rcs{$region}{$rcshort};
		#next if exists $neca_rcs{$region}{$rcshort};
		#next if exists $locl_rcs{$region}{$rcshort};
		#next if exists $telc_rcs{$region}{$rcshort};
		#next if exists $ping_rcs{$region}{$rcshort};
		$area_rcs{$region}{$rcshort} = {} unless exists $area_rcs{$region}{$rcshort};
		my $rec = $area_rcs{$region}{$rcshort};
		for (my $i=0;$i<@area_fields;$i++) {
			$rec->{$area_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$area_count++;
	}
	close($fh);
	print STDERR "I: RCs for area: $area_count\n";
}

print STDERR "I: fetching RCs for nanp...\n";
foreach my $region (keys %nanp_rcs) {
	foreach (values %{$nanp_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [nanp] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($nanp_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$nanp_count--;
	}
}

print STDERR "I: fetching RCs for neca...\n";
foreach my $region (keys %neca_rcs) {
	foreach (values %{$neca_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [neac] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($neca_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$neca_count--;
	}
}

print STDERR "I: fetching RCs for locl...\n";
foreach my $region (keys %locl_rcs) {
	foreach (values %{$locl_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [locl] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($locl_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$locl_count--;
	}
}

print STDERR "I: fetching RCs for telc...\n";
foreach my $region (keys %telc_rcs) {
	foreach (values %{$telc_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [telc] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($telc_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$telc_count--;
	}
}

print STDERR "I: fetching RCs for ping...\n";
foreach my $region (keys %ping_rcs) {
	foreach (values %{$ping_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [ping] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($ping_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$ping_count--;
	}
}

print STDERR "I: fetching RCs for area...\n";
foreach my $region (keys %area_rcs) {
	foreach (values %{$area_rcs{$region}}) {
		my $cnt = 0;
		foreach my $nxx (split(/,/,$_->{NXX})) {
			$nxx =~ s/-//g;
			next unless length($nxx) == 6;
			my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
			my $fil = "$dir/$nxx.html.xz";
			if (-f $fil) { $cnt++; last if 0 and $cnt >= 9; next; }
			print STDERR "I: [area] fetching $nxx for $_->{REGION} $_->{RCSHORT} ($area_count remaining)\n";
			system("mkdir -v -p $dir");
			system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
			#sleep(1);
			#last;
		}
		$area_count--;
	}
}

exit;

1;

__END__
