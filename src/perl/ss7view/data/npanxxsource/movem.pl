#!/usr/bin/perl

my @files = `ls *.html.xz`;

foreach (@files) { chomp;
	if (/^([0-9]{6}).html.xz/) {
		my $nxx = $1;
		my $dir = substr($nxx,0,3).'/'.substr($nxx,3,1).'/'.substr($nxx,4,1);
		system("mkdir -v -p $dir");
		system("mv -v $_ $dir");
	}
}

