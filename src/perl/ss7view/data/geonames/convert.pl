#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

# ITU-T Country name to ISO 3166-1 country codes.
my %ccodes = ();
{
	$fn = "$codedir/ccodes.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		last if /^$/;
		next if /^#/;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		if (my $nm = $rec->{'ITU-T Name'}) {
			$ccodes{$nm} = $rec;
		}
	}
	close($fh);
}

# geoname data
my %geonames = ();
{
	$fn = "$datadir/countries.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		last if /^$/;
		next if /^#/;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		my $gi = $tokens[0];
		$geonames{$gi} = $rec;
	}
	close($fh);
}

# ISO 3166-1 alpha-2 to geonameid mapping
my %geocodes = ();
{
	$fn = "$datadir/ccgeoid.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		last if /^$/;
		next if /^#/;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $rec = {};
		my $a2 = $rec->{'alpha-2'} = $tokens[0];
		my $gi = $rec->{geonameid} = $tokens[1];
		$geocodes{$a2} = $rec;
		if (exists $geonames{$gi}) {
			my $dat = $geonames{$gi};
			foreach my $k (keys %{$dat}) {
				$rec->{$k} = $dat->{$k} if $dat->{$k};
			}
		} else {
			print STDERR "E: no geoname entry for $tokens[1]\n";
		}
	}
}

# ITU-T Country name to E164/C country code
my %e164c = ();
{
	$fn = "$codedir/e164c.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		last if /^$/;
		next if /^#/;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $nm = $tokens[0];
		unless ($nm) {
			print STDERR "E: no country\n";
			next;
		}
		my $cc = $tokens[1];
		unless ($cc) {
			print STDERR "E: no country code for $nm\n";
			next;
		}
		$e164c{$cc} = {} unless exists $e164c{$cc};
		my $rec = $e164c{$cc};
		$rec->{'Country'}{$nm}++ if $nm;
		$rec->{'Country Code'}{$cc}++ if $cc;
		for (my $i=2;$i<@fields;$i++) {
			if ($tokens[$i]) {
				my $k = $fields[$i];
				my $t = $tokens[$i];
				if ($k eq 'Timezone') {
					foreach my $tz (split(/\//,$t)) {
						$rec->{Timezone}{$tz}++ if $tz;
					}
				} else {
					if ($k eq 'Digits') {
						$t =~ s/\([0-9]{3}[^\)]*\)\s*\+\s*7 digits/10 digits/;
					}
					if (exists $rec->{$k} and $rec->{$k} ne $t) {
						print STDERR "E: $k changing from '$t' to '$rec->{$k}'\n";
					}
					$rec->{$k} = $t;
				}
			}
		}
		if (exists $ccodes{$nm}) {
			my $dat = $ccodes{$nm};
			if (exists $dat->{'alpha-2'} and $dat->{'alpha-2'}) {
				my $a2 = $dat->{'alpha-2'};
				if (exists $geocodes{$a2}) {
					$geocodes{$a2}{'E164/C Name'} = $nm if $nm;
					$geocodes{$a2}{'E164/C Country Code'} = $cc if $cc;
					$geocodes{$a2}{Digits} = $rec->{Digits} if $rec->{Digits};
					$geocodes{$a2}{Timezone} = $rec->{Timezone} if $rec->{Timezone};
				} else {
					print STDERR "W: no geoname for $a2\n";
				}
			} else {
				print STDERR "E: no alpha-2 for $dat->{'ITU-T Name'}\n";
			}
			$rec->{'alpha-2'}{$dat->{'alpha-2'}}++ if $dat->{'alpha-2'};
			$rec->{'alpha-3'}{$dat->{'alpha-3'}}++ if $dat->{'alpha-3'};
			$rec->{'numeric'}{$dat->{'numeric'}}++ if $dat->{'numeric'};
		} else {
			print STDERR "E: no name mapping for $nm\n";
		}
	}
	close($fh);
}

{
	$fn = "$datadir/e164c.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	my @fields = ('Country Code','alpha-2','Country','Digits','Timezone');
	print $of '"',join('","',@fields),'"',"\n";
	foreach my $k (sort keys %e164c) {
		my $rec = $e164c{$k};
		my @values = ();
		foreach (@fields) {
			my $val;
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				if ($_ eq 'Timezone') {
					$val = join('/',sort {$a<=>$b} keys %{$rec->{$_}});
				} elsif ($_ eq 'Country') {
					$val = join('; ',sort keys %{$rec->{$_}});
				} else {
					$val = join(',',sort keys %{$rec->{$_}});
				}
			} else {
				$val = $rec->{$_};
			}
			push @values,$val;
		}
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}

# ITU-T Country name to E164/D country code
my %e164d = ();
{
	$fn = "$codedir/e164d.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		last if /^$/;
		next if /^#/;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $cc = $tokens[0];
		unless ($cc or length($cc)) {
			print STDERR "E: no country code\n";
			next;
		}
		my $nm = $tokens[1];
		next if $nm =~ /Spare|Reserved/;
		unless ($nm) {
			print STDERR "E: no country for $cc\n";
			next;
		}
		#if (exists $e164d{$cc}) {
		#	print STDERR "W: overwriting entry for $cc\n";
		#}
		$e164d{$cc} = {} unless exists $e164d{$cc};
		my $rec = $e164d{$cc};
		$rec->{'Country'}{$nm}++ if $nm;
		$rec->{'Country Code'}{$cc}++ if $cc;
		for (my $i=2;$i<@fields;$i++) {
			if ($tokens[$i]) {
				my $k = $fields[$i];
				my $t = $tokens[$i];
				if (exists $rec->{$k} and $rec->{$k} ne $t) {
					print STDERR "E: $k changing from '$t' to '$rec->{$k}'\n";
				}
				$rec->{$k} = $t;
			}
		}
		if (exists $ccodes{$nm}) {
			my $dat = $ccodes{$nm};
			if (exists $dat->{'alpha-2'} and $dat->{'alpha-2'}) {
				my $a2 = $dat->{'alpha-2'};
				if (exists $geocodes{$a2}) {
					$geocodes{$a2}{'E164/D Name'} = $nm if $nm;
					$geocodes{$a2}{'E164/D Country Code'} = $cc if $cc;
				} else {
					print STDERR "W: no geoname for $a2\n";
				}
			} else {
				print STDERR "E: no alpha-2 for $dat->{'ITU-T Name'}\n";
			}
			$rec->{'alpha-2'}{$dat->{'alpha-2'}}++ if $dat->{'alpha-2'};
			$rec->{'alpha-3'}{$dat->{'alpha-3'}}++ if $dat->{'alpha-3'};
			$rec->{'numeric'}{$dat->{'numeric'}}++ if $dat->{'numeric'};
		} else {
			unless ($nm =~ /Spare|Reserved/) {
				print STDERR "E: no name mapping for $nm\n";
			}
		}
	}
	close($fh);
}

{
	$fn = "$datadir/e164d.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	my @fields = ('Country Code','alpha-2','Country');
	print $of '"',join('","',@fields),'"',"\n";
	foreach my $k (sort keys %e164d) {
		my $rec = $e164d{$k};
		my @values = ();
		foreach (@fields) {
			my $val;
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				if ($_ eq 'Country') {
					$val = join('; ',sort keys %{$rec->{$_}});
				} else {
					$val = join(',',sort keys %{$rec->{$_}});
				}
			} else {
				$val = $rec->{$_};
			}
			push @values,$val;
		}
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}

{
	$fn = "$datadir/geocodes.csv";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
	my @fields = (
		'alpha-2',
		'E164/C Country Code',
		'E164/D Country Code',
		'E164/C Name',
		'E164/D Name',
		'Digits',
		'Timezone',
		'geonameid',
		'name',
		'asciiname',
		'alternatenames',
		'latitude',
		'longitude',
		'feature class',
		'feature code',
		'country code',
		'cc2',
		'admin1 code',
		'admin2 code',
		'admin3 code',
		'admin4 code',
		'population',
		'elevation',
		'dem',
		'timezone',
		'modification date',
	);
	print $of '"',join('","',@fields),'"',"\n";
	foreach my $k (sort keys %geocodes) {
		my $rec = $geocodes{$k};
		my @values = ();
		foreach (@fields) {
			my $val;
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				if ($_ eq 'Timezone') {
					$val = join('/',sort {$a<=>$b} keys %{$rec->{$_}});
				} elsif ($_ eq 'Country') {
					$val = join('; ',sort keys %{$rec->{$_}});
				} else {
					$val = join(',',sort keys %{$rec->{$_}});
				}
			} else {
				$val = $rec->{$_};
			}
			push @values,$val;
		}
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}
