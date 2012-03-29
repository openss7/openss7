#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

use Data::Dumper;

my $assignments;

unless ($assignments = do "$progdir/assigments.pl") {
	die "couldn't parse file: $@" if $@;
	die "couldn't do file: $!" unless defined $assignments;
	die "couldn't run file" unless $assignments;
}
die "no assignments" unless ref $assignments eq 'HASH';

print Dumper($assignments);
