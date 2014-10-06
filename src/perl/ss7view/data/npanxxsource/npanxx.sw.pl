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

my %npas_sws = ();
my @npas_fields = ();
my $npas_count = 0;
$fn = "$progdir/sw.csv";
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
		my $sw = $tokens[0];
		$npas_sws{$sw} = {} unless exists $npas_sws{$sw};
		my $rec = $npas_sws{$sw};
		for (my $i=0;$i<@npas_fields;$i++) {
			$rec->{$npas_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$npas_count++;
	}
	close($fh);
	print STDERR "I: SWs for npas: $npas_count\n";
}

my %nanp_sws = ();
my @nanp_fields = ();
my $nanp_count = 0;
$fn = "$nanpdir/sw.csv";
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
		next unless $tokens[6]; # NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		$nanp_sws{$sw} = {} unless exists $nanp_sws{$sw};
		my $rec = $nanp_sws{$sw};
		for (my $i=0;$i<@nanp_fields;$i++) {
			$rec->{$nanp_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$nanp_count++;
	}
	close($fh);
	print STDERR "I: SWs for nanp: $nanp_count\n";
}

my %neca_sws = ();
my @neca_fields = ();
my $neca_count = 0;
$fn = "$necadir/sw.csv";
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
		next unless $tokens[6]; # NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		$neca_sws{$sw} = {} unless exists $neca_sws{$sw};
		my $rec = $neca_sws{$sw};
		for (my $i=0;$i<@neca_fields;$i++) {
			$rec->{$neca_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$neca_count++;
	}
	close($fh);
	print STDERR "I: SWs for neca: $neca_count\n";
}


my %locl_sws = ();
my @locl_fields = ();
my $locl_count = 0;
$fn = "$locldir/sw.csv";
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
		next unless $tokens[7]; # NPA-NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		#next if exists $neca_sws{$sw};
		$locl_sws{$sw} = {} unless exists $locl_sws{$sw};
		my $rec = $locl_sws{$sw};
		for (my $i=0;$i<@locl_fields;$i++) {
			$rec->{$locl_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$rec->{NXX} = delete $rec->{'NPA-NXX'} if exists $rec->{'NPA-NXX'};
		$locl_count++;
	}
	close($fh);
	print STDERR "I: SWs for locl: $locl_count\n";
}

my %telc_sws = ();
my @telc_fields = ();
my $telc_count = 0;
$fn = "$telcdir/sw.csv";
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
		next unless $tokens[7]; # NPA-NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		#next if exists $neca_sws{$sw};
		$telc_sws{$sw} = {} unless exists $telc_sws{$sw};
		my $rec = $telc_sws{$sw};
		for (my $i=0;$i<@telc_fields;$i++) {
			$rec->{$telc_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$rec->{NXX} = delete $rec->{'NPA-NXX'} if exists $rec->{'NPA-NXX'};
		$telc_count++;
	}
	close($fh);
	print STDERR "I: SWs for telc: $telc_count\n";
}

my %ping_sws = ();
my @ping_fields = ();
my $ping_count = 0;
$fn = "$pingdir/sw.csv";
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
		next unless $tokens[7]; # NPA-NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		#next if exists $neca_sws{$sw};
		$ping_sws{$sw} = {} unless exists $ping_sws{$sw};
		my $rec = $ping_sws{$sw};
		for (my $i=0;$i<@ping_fields;$i++) {
			$rec->{$ping_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$rec->{NXX} = delete $rec->{'NPA-NXX'} if exists $rec->{'NPA-NXX'};
		$ping_count++;
	}
	close($fh);
	print STDERR "I: SWs for ping: $ping_count\n";
}

my %area_sws = ();
my @area_fields = ();
my $area_count = 0;
$fn = "$areadir/sw.csv";
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
		next unless $tokens[7]; # NPA-NXX
		my $sw = $tokens[0];
		#next if exists $npas_sws{$sw};
		#next if exists $neca_sws{$sw};
		$area_sws{$sw} = {} unless exists $area_sws{$sw};
		my $rec = $area_sws{$sw};
		for (my $i=0;$i<@area_fields;$i++) {
			$rec->{$area_fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$rec->{NXX} = delete $rec->{'NPA-NXX'} if exists $rec->{'NPA-NXX'};
		$area_count++;
	}
	close($fh);
	print STDERR "I: SWs for area: $area_count\n";
}

print STDERR "I: fetching switches from nanp...\n";
foreach (values %nanp_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [nanp] fetching $nxx for $_->{CLLI} ($nanp_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$nanp_count--;
}

print STDERR "I: fetching switches from neca...\n";
foreach (values %neca_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		next unless length($nxx) == 6;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [neca] fetching $nxx for $_->{CLLI} ($neca_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$neca_count--;
}

print STDERR "I: fetching switches from locl...\n";
foreach (values %locl_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [locl] fetching $nxx for $_->{CLLI} ($locl_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$locl_count--;
}

print STDERR "I: fetching switches from telc...\n";
foreach (values %telc_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [telc] fetching $nxx for $_->{CLLI} ($telc_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$telc_count--;
}

print STDERR "I: fetching switches from ping...\n";
foreach (values %ping_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [ping] fetching $nxx for $_->{CLLI} ($ping_count remaining)\n";
		system("mkdir -v -p $dir");
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$nxx' | xz > $fil");
		#sleep(1);
		#last;
	}
	$ping_count--;
}

print STDERR "I: fetching switches from area...\n";
foreach (values %area_sws) {
	my $cnt = 0;
	foreach my $nxx (split(/,/,$_->{NXX})) {
		$nxx =~ s/-//g;
		my $dir = $progdir.'/'.substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		my $fil = "$dir/$nxx.html.xz";
		if (-f $fil) { $cnt++; last if 0 and $cnt >= 8; next; }
		print STDERR "I: [area] fetching $nxx for $_->{CLLI} ($area_count remaining)\n";
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
