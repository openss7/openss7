#!/usr/bin/perl -w

eval 'exec perl -S $0 ${1+"@"}'
    if $running_under_some_shell;

$\ = "\n";

no strict;

my $filename = shift;

print "filename: $filename";

die unless -r $filename;

open(INFILE,"<$filename") || die;
print "opened";

my %syms;

while (<INFILE>) {
	chomp;
	my ($crc,$sym,$path,$export) = (split(/\b+/));
	$crc = $1;
	$sym = $2;
	$path = $3;
	$export = $4;
	my $module = $path;
	$module =~ s/[^\/]*\///;
	unless (defined $syms{$module}) {
		my %set = {};
		bless $set;
		$syms{$module} = \%set;
	}
	unless (defined $syms{$module}->{$sym}) {
		my %group = {
			sym => $sym,
			path => $path,
			crc => $crc,
			export => $export,
			module => $module
		};
		bless $group;
		$syms{$module}->{$sym} = \%group;
	} else {
		print "symbol conflict";
		print "sym = $sym";
		print "path = $path";
		print "crc = $crc";
		print "export = $export";
	}
}

close(INFILE);
print "closed";

foreach my $mod ( keys %syms ) {
	print "$mod";
	foreach my $sym ( keys %{$syms{$mod}} ) {
		print "$mod:$sym($syms{$mod}->{$sym}->{crc})";
	}
}

exit 0;
