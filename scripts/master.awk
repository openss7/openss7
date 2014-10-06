#!/usr/bin/awk -f

# combine strconf inputs into master file on standard output

function date(format) {
    if (format) {
	"date +\"" format "\"" | getline
	close("date +\"" format "\"")
    } else {
	"date" | getline
	close("date")
    }
    return $0
}

BEGIN { print "# Created " date("%F %T") " by configure -- DO NOT EDIT" }
(FNR == 1) { print "file " FILENAME; SKIPPING = 0 }
/^# / { print; next }
/^$/ || /^#/ { SKIPPING = 1; next }
(SKIPPING == 1) { SKIPPING = 0; print "line " FNR }
{ print }
END { print "# Created " date("%F %T") " by configure -- DO NOT EDIT" }

# =============================================================================
#
# $Log: master.awk,v $
# Revision 1.1.2.1  2009-07-21 11:06:21  brian
# - new awk scripts for release check
#
# =============================================================================
# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
