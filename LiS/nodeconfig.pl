#!/usr/bin/perl -an
# $Id: nodeconfig.pl,v 9.3 2001/04/16 20:27:11 vassilii Exp $
#
# Written by Vassilii Khachaturov <Vassilii.Khachaturov@comverse.com>
# This file converts a Node(4dsp) file to 
# a subset of LiS config file syntax
# (see the LiS config.html document page).
#
# $Log: nodeconfig.pl,v $
# Revision 9.3  2001/04/16 20:27:11  vassilii
# Added loadable/objname lines for each driver
# so that the output by strconf entry for modules.conf is correct.
#
# Revision 9.2  2001/04/13 21:53:52  vassilii
# Treating 'clone' module_name specially for SVR4 compatibility.
#
# Revision 9.1  2001/04/13 18:12:28  vassilii
# Convert a Node file to a corresponding LiS Config snippet
#
BEGIN { 
	require 5.005; # or the following 2 lines don't work...
	my $driver_name = shift
		or die "Usage: $0 <driver name> [<Source node file>]";
	my $src = $ARGV[0] || 'STDIN';
	print "# Auto-built from\n#\t$src\n# by $0\n", 
		'# $Header: config/nodeconfig.pl,v 9.3 2001/04/16 20:27:11 vassilii Exp $', "\n", <<"EOF";
driver $driver_name ${driver_name}_ *
objname driver $driver_name $driver_name
loadable $driver_name
EOF
} 

# See Node(4dsp) for the SVR4 fields descr.
# In our product, there is no type:offset syntax used or channels
my ($module_name, $node_name, $type, $minor, # required fields
	$user, $group, $permissions, $level # optional fields
) = @F;

$permissions = '*' 
	unless defined $permissions;
if ($module_name eq 'clone') {
	# LiS uses 'clone-drvr' as the stream cloneable device major symbolic name.
	# Everywhere else I looked they use 'clone' instead.
	$module_name = 'clone-drvr';
}

my $major = $module_name;

if ($module_name =~ m/^[#\*\$]/) {
	# Comment or unsupported directive,
	# or configuration for another driver in the same source present.
	print "#$_";
}
else {
	print <<"EOF";
node	/dev/$node_name	$type $permissions $major $minor
EOF
}
