#!/usr/bin/perl
#
# $Source: /xtel/isode/isode/others/ntp/RCS/extract.pl,v $ $Revision: 9.0 $ $Date: 1992/06/16 12:42:48 $
#
$HOST = '10.2.0.96';
if ($#ARGV != 0) {
	die "Must specify internet address of host.";
}
$HOST = $ARGV[1];
while(<stdin>) {
	if(/^host: $HOST/) {
		s/host: //;
		s/\(/ /g;
		s/\)/ /g;
		s/:/ /g;
		@A = split(' ');
		print $A[3],"\n";
	 }
}
