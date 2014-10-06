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

my %npas_wcs = ();
my @npas_fields = ();
my $npas_count = 0;
$fn = "$progdir/wc.csv";
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
		my $wc = $tokens[0];
		$npas_wcs{$wc} = {} unless exists $npas_wcs{$wc};
		my $rec = $npas_wcs{$wc};
		for (my $i=0;$i<@npas_fields;$i++) {
			$rec->{$npas_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$npas_count++;
	}
	close($fh);
	print STDERR "I: RCs for npas: $npas_count\n";
}

my %nanp_wcs = ();
my @nanp_fields = ();
my $nanp_count = 0;
$fn = "$nanpdir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		$nanp_wcs{$wc} = {} unless exists $nanp_wcs{$wc};
		my $rec = $nanp_wcs{$wc};
		for (my $i=0;$i<@nanp_fields;$i++) {
			$rec->{$nanp_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$nanp_count++;
	}
	close($fh);
	print STDERR "I: RCs for nanp: $nanp_count\n";
}

my %neca_wcs = ();
my @neca_fields = ();
my $neca_count = 0;
$fn = "$necadir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		#next if exists $nanp_wcs{$wc};
		$neca_wcs{$wc} = {} unless exists $neca_wcs{$wc};
		my $rec = $neca_wcs{$wc};
		for (my $i=0;$i<@neca_fields;$i++) {
			$rec->{$neca_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$neca_count++;
	}
	close($fh);
	print STDERR "I: RCs for neca: $neca_count\n";
}

my %locl_wcs = ();
my @locl_fields = ();
my $locl_count = 0;
$fn = "$locldir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		#next if exists $nanp_wcs{$wc};
		#next if exists $neca_wcs{$wc};
		$locl_wcs{$wc} = {} unless exists $locl_wcs{$wc};
		my $rec = $locl_wcs{$wc};
		for (my $i=0;$i<@locl_fields;$i++) {
			$rec->{$locl_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$locl_count++;
	}
	close($fh);
	print STDERR "I: RCs for locl: $locl_count\n";
}

my %telc_wcs = ();
my @telc_fields = ();
my $telc_count = 0;
$fn = "$telcdir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		#next if exists $nanp_wcs{$wc};
		#next if exists $neca_wcs{$wc};
		#next if exists $locl_wcs{$wc};
		$telc_wcs{$wc} = {} unless exists $telc_wcs{$wc};
		my $rec = $telc_wcs{$wc};
		for (my $i=0;$i<@telc_fields;$i++) {
			$rec->{$telc_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$telc_count++;
	}
	close($fh);
	print STDERR "I: RCs for telc: $telc_count\n";
}

my %ping_wcs = ();
my @ping_fields = ();
my $ping_count = 0;
$fn = "$pingdir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		#next if exists $nanp_wcs{$wc};
		#next if exists $neca_wcs{$wc};
		#next if exists $locl_wcs{$wc};
		#next if exists $telc_wcs{$wc};
		$ping_wcs{$wc} = {} unless exists $ping_wcs{$wc};
		my $rec = $ping_wcs{$wc};
		for (my $i=0;$i<@ping_fields;$i++) {
			$rec->{$ping_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$ping_count++;
	}
	close($fh);
	print STDERR "I: RCs for ping: $ping_count\n";
}

my %area_wcs = ();
my @area_fields = ();
my $area_count = 0;
$fn = "$areadir/wc.csv";
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
		my $wc = $tokens[0];
		#next if exists $npas_wcs{$wc};
		#next if exists $nanp_wcs{$wc};
		#next if exists $neca_wcs{$wc};
		#next if exists $locl_wcs{$wc};
		#next if exists $telc_wcs{$wc};
		#next if exists $ping_wcs{$wc};
		$area_wcs{$wc} = {} unless exists $area_wcs{$wc};
		my $rec = $area_wcs{$wc};
		for (my $i=0;$i<@area_fields;$i++) {
			$rec->{$area_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$area_count++;
	}
	close($fh);
	print STDERR "I: RCs for area: $area_count\n";
}

print STDERR "I: fetching RCs for nanp...\n";
foreach (values %nanp_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [nanp] fetching $nxx for $_->{WC} ($nanp_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$nanp_count--;
}

print STDERR "I: fetching RCs for neca...\n";
foreach (values %neca_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [neca] fetching $nxx for $_->{WC} ($neca_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$neca_count--;
}

print STDERR "I: fetching RCs for locl...\n";
foreach (values %locl_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [locl] fetching $nxx for $_->{WC} ($locl_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$locl_count--;
}

print STDERR "I: fetching RCs for telc...\n";
foreach (values %telc_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [telc] fetching $nxx for $_->{WC} ($telc_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$telc_count--;
}

print STDERR "I: fetching RCs for ping...\n";
foreach (values %ping_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [ping] fetching $nxx for $_->{WC} ($ping_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$ping_count--;
}

print STDERR "I: fetching RCs for area...\n";
foreach (values %area_wcs) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [area] fetching $nxx for $_->{WC} ($area_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$area_count--;
}

exit;

1;

__END__
