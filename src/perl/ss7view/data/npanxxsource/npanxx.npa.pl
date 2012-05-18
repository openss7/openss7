#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

use strict;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir = '.' unless $progdir =~ /\//; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/..";
my $rsltdir = "$progdir/results";

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %db = ();

my $tot = 0;

my @files = `ls $datadir/*/db.nxx.nogeo.csv`;
foreach my $file (@files) { chomp $file;
	print STDERR "I: reading $file...\n";
	my $oldnpa = 100; my $oldrg = '';
	open($fh,"<:utf8",$file) or die "can't open $file";
	my $heading = 1;
	while (<$fh>) { chomp;
		if ($heading) {
			$heading = undef;
			next;
		}
		s/^"//; s/"$//;
		my ($npa,$nxx,$x,$cc,$st,$rg,$lt) = split(/","/,$_);
		next unless $npa and $nxx;
		$tot++ unless exists $db{$npa}{$nxx};
		$db{$npa}{$nxx}{$x}++;
		if ($oldnpa ne $npa and $oldrg ne $rg) {
			print STDERR "I: processing $npa $rg\n";
		}
		($oldnpa,$oldrg) = ($npa,$rg);
	}
	close($fh);
}

foreach my $npa (keys %db) {
	foreach my $nxx (keys %{$db{$npa}}) {
		my $dir = $rsltdir.'/'.$npa.'/'.substr($nxx,0,1).'/'.substr($nxx,1,1);
		my $fil = "$dir/$npa$nxx.html.xz";
		if (-f $fil) { $tot--; next }
		print STDERR "I: fetching $npa-$nxx ($tot remaining)\n";
		system("mkdir -v -p $dir") unless -d $dir;
		system("wget -O - 'http://npanxxsource.com/nalennd.php?q=$npa$nxx' | xz > $fil");
		$tot--;
	}
}

exit;

1;

__END__
