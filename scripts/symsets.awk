#!/usr/bin/awk -f
# =============================================================================
#
# @(#) $RCSfile: symsets.awk,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2011-03-17 07:01:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2011-03-17 07:01:22 $ by $Author: brian $
#
# =============================================================================

function getline_command(cmd)
{
    cmd | getline; close(cmd); return $0
}
function date(format) {
    if (format) {
	return getline_command("date +\"" format "\"")
    } else {
	return getline_command("date -uIseconds")
    }
}
function year()
{
    return date("%Y")
}
function allyears(    this, last, sep, result)
{
    last = year(); sep = ""; result = ""
    for (this = 2009; this <= last; this++) {
	result = result sep this
	sep = ", "
    }
    return result
}
function print_info(string)
{
    if (values["quiet"] == 0 && values["verbose"] > 0) {
	print blu me ": I: " string std > stderr
	written[stderr] = 1
    }
}
function print_debug(string)
{
    if (values["debug"] > 0) {
	print mag me ": D: " string std > stderr
	written[stderr] = 1
    }
}
function print_error(string)
{
    print red me ": E: " string std > stderr
    written[stderr] = 1
}
function print_warn(string)
{
    if (values["quiet"] == 0 || values["verbose"] > 0 || values["debug"] > 0) {
	print org me ": W: " string std > stderr
	written[stderr] = 1
    }
}
function usage(output)
{
    if (values["quiet"])
	return
    print "\
symsets:\n\
  $Id: symsets.awk,v 1.1.2.1 2011-03-17 07:01:22 brian Exp $\n\
Usage:\n\
  symsets [options] [INPUT ...]\n\
  symsets -h\n\
  symsets -V\n\
  symsets -C\
" > output
    written[output] = 1
}
function help(output)
{
    if (values["quiet"])
	return
    usage(output)
    print "\
Arguments:\n\
  INPUT ...\n\
      Module.symvers files for which to generate symsets\n\
      [default: " defaults["inputs"] "] {" environs["inputs"] "}\n\
Options:\n\
  -s, --suse\n\
      output symsets in SuSE format\n\
  -r, --redhat\n\
      output symsets in RedHat format (default)\n\
  -k, --kversion KVERSION\n\
      kernel version KVERSION\n\
      [default: " defaults["kversion"] "] {" environs["kversion"] "}\n\
  -I, --inputs INPUTS\n\
      input Module.symvers files (space separated)\n\
      [default: " defaults["inputs"] "] {" environs["inputs"] "}\n\
  -o, --outfile OUTFILE\n\
      output file name OUTFILE\n\
      [default: " defaults["outfile"] "] {" environs["outfile"] "}\n\
  -n, --dryrun, --dry-run\n\
      don't perform the actions, just check them\n\
  -q, --quiet, --silent\n\
      suppress normal output\n\
  -D, --debug[=LEVEL]\n\
      increase or set debug level\n\
      [default: " defaults["debug"] "]\n\
  -v, --verbose[=LEVEL]\n\
      increase or set verbosity level\n\
      [default: " defaults["verbose"] "]\n\
  -h, --help\n\
      display this usage information and exit\n\
  -V, --version\n\
      display script version and exit\n\
  -C, --copying\n\
      display copying permissions and exit\n\
" > output
    written[output] = 1
}
function version(output)
{
    if (values["quiet"])
	return
    print "\
Version 2.1\n\
$Id: symsets.awk,v 1.1.2.1 2011-03-17 07:01:22 brian Exp $\n\
Copyright (c) 2008, " allyears() "  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
\n\
All Rights Reserved.\n\
\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See " me " -- --copying' for copying permissions.\
" > output
    written[output] = 1
}
function copying(output)
{
    if (values["quiet"])
	return
    print "\
--------------------------------------------------------------------------------\n\
$Id: symsets.awk,v 1.1.2.1 2011-03-17 07:01:22 brian Exp $\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008, " allyears() "  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the  GNU  Affero General Public  License as published  by the Free\n\
Software Foundation; version 3 of the License.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\
" > output
    written[output] = 1
}
function getopt_long(argc, argv, optstring, longopts, longindex,    pos, needarg, wantarg)
{
    option = ""; optval = ""; optarg = ""; pos = 0; needarg = 0; wantarg = 0
    if (optind == 0) { optind = 1; more = "" }
    while ((optind < argc) || (more != "")) {
	if (more) { arg = "-" more; more = "" }
	else { arg = argv[optind]; optind++ }
	if (arg ~ /^--?[a-zA-Z0-9]/) {
	    if (needarg) {
		print_error("option -" optval " requires an argument")
		usage(stderr)
		exit 2
	    }
	    if (wantarg) {
		more = substr(arg, 2)
		return optval
	    }
	    if (arg ~ /^--[a-zA-Z0-9][-_a-zA-Z0-9]*=.*$/) {
		option = arg; sub(/^--/, "", option); sub(/=.*$/, "", option)
		optarg = arg; sub(/^--([a-zA-Z0-9][-_a-zA-Z0-9]*)=/, "", optarg)
		if (!(option in longopts)) {
		    print_error("option --" option " not recognized")
		    usage(stderr)
		    exit 2
		}
		if (longopts[option] !~ /:/) {
		    print_error("option --" option " does not take an argument")
		    usage(stderr)
		    exit 2
		}
		optval = substr(longopts[option], 1, 1)
		return optval
	    }
	    if (arg ~ /^--[a-zA-Z0-9][-_a-zA-Z0-9]*$/) {
		option = arg; sub(/^--/, "", option)
		if (!(option in longopts)) {
		    print_error("option --" option " not recognized")
		    usage(stderr)
		    exit 2
		}
		if (longopts[option] ~ /::/) { wantarg = 1 } else
		if (longopts[option] ~ /:/ ) { needarg = 1 }
		optval = substr(longopts[option], 1, 1)
		for (option in longopts)
		    if (substr(longopts[option], 1, 1) == optval)
			break
		if (!needarg && !wantarg)
		    return optval
		continue
	    }
	    if (arg ~ /^-[a-zA-Z0-9]/) {
		optval = substr(arg, 2, 1)
		pos = index(optstring, optval)
		if (pos == 0) {
		    print_error("option -" optval " not recognized")
		    usage(stderr)
		    exit 2
		}
		if (substr(optstring, pos+1, 1) == ":") {
		    if (length(arg) > 2) {
			if (substr(optstring, pos+2, 1) == ":") {
			    more = substr(arg, 3)
			    if (more && more !~ /^[a-zA-Z0-9]/) {
				print_error("bad option sequence " arg)
				usage(stderr)
				exit 2
			    }
			} else
			    optarg = substr(arg, 3)
			for (option in longopts)
			    if (substr(longopts[option], 1, 1) == optval)
				break
			return optval
		    }
		    if (substr(optstring, pos+2, 1) == ":") { wantarg = 1 } else
		    if (substr(optstring, pos+1, 1) == ":") { needarg = 1 }
		    for (option in longopts)
			if (substr(longopts[option], 1, 1) == optval)
			    break
		    continue
		}
		if (length(arg) > 2) { more = substr(arg, 3) } else { more = "" }
		if (more && more !~ /^[a-zA-Z0-9]/) {
		    print_error("bad option sequence " arg)
		    usage(stderr)
		    exit 2
		}
		for (option in longopts)
		    if (substr(longopts[option], 1, 1) == optval)
			break
		return optval
	    }
	}
	if (arg == "--")
	    return -1
	if (needarg || wantarg) {
	    optarg = arg
	    return optval
	}
	optind--
	return -1
    }
    return -1
}
function system_command(cmd)
{
    print_debug(cmd); return system(cmd)
}
function missing(fnlp, expected) {
    print_error(fnlp " definition missing " (expected - NF) " fields")
}
function garbage(fnlp, maximum) {
    print_warn(fnlp " definition with " (NF - maximum) " garbage fields")
}
function unrecog(fnlp, line) {
    print_error(fnlp " unrecognized line: " line)
}
function read_inputs(inputs) {
    print_debug("reading inputs...")
    for (k in inputs) {
	input = inputs[k]
	print_debug("reading input " input)
	filename = input
	lineno = 0
	while ((getline < input) == 1) {
	    lineno++; fnl = filename ":" lineno; fnlp = fnl ": "
	    if (NF < 4) { missing(fnlp, 4); continue }
	    if (NF > 4) { garbage(fnlp, 4); NF = 4 }
	    if ($1~/^0x/) {
		gsub(/\//, "_")
		if ($3 in symsets) {
		    symsets[$3] = symsets[$3] " " $2
		    crcsets[$3] = crcsets[$3] $1
		} else {
		    symsets[$3] = $2
		    crcsets[$3] = $1
		}
		# Note that SuSE includes EXPORT type in the
		# kABI, whereas RedHat does not.
		if (values["suse"] == 1) {
		    syms[$2] = $1 "\t" $2 "\t" $3 "\t" $4
		} else {
		    syms[$2] = $1 "\t" $2 "\t" $3
		}
		#print $1 "\t" $2 "\t" $3
	    } else { unrecog(fnlp, $0); continue }
	}
	close(input)
    }
}
BEGIN {
    LINT = "yes"
    me = "symsets.awk"
    if (!("TERM" in ENVIRON)) ENVIRON["TERM"] = "dumb"
    if (ENVIRON["TERM"] == "dumb" || system("test -t 1 -a -t 2") != 0) {
	stdout = "/dev/stderr"; written[stdout] = 0
	stderr = "/dev/stderr"; written[stderr] = 0
	cr = ""; lf = "\n"
	blk = ""; hblk = ""
	red = ""; hred = ""
	grn = ""; hgrn = ""
	org = ""; horg = ""
	blu = ""; hblu = ""
	mag = ""; hmag = ""
	cyn = ""; hcyn = ""
	std = ""
    } else {
	stdout = "/dev/stdout"; written[stdout] = 0
	stderr = "/dev/stderr"; written[stderr] = 0
	cr = "\r"; lf = ""
	blk = "\033[0;30m"; hblk = "\033[1;30m"
	red = "\033[0;31m"; hred = "\033[1;31m"
	grn = "\033[0;32m"; hgrn = "\033[1;32m"
	org = "\033[0;33m"; horg = "\033[1;33m"
	blu = "\033[0;34m"; hblu = "\033[1;34m"
	mag = "\033[0;35m"; hmag = "\033[1;35m"
	cyn = "\033[0;36m"; hcyn = "\033[1;36m"
	std = "\033[m"
    }
    if ("kversion" in ENVIRON) { uname = ENVIRON["kversion"] }
    else                       { uname = getline_command("uname -r") }
    longopts["inputs"       ] = "I:" ; environs["inputs"        ] = "MODPOST_MODVER"  ; defaults["inputs"       ] = "Module.symvers"
    longopts["suse"         ] = "s"  ;                                                  defaults["suse"         ] = 0
    longopts["redhat"       ] = "r"  ;                                                  defaults["redhat"       ] = 1
    longopts["kversion"     ] = "k:" ; environs["kversion"      ] = "kversion"        ; defaults["kversion"     ] = uname
    longopts["outfile"      ] = "o:" ; environs["outfile"       ] = "MODPOST_SYMSETS" ; defaults["outfile"      ] = "symsets-" uname ".tar.gz"
    longopts["dryrun"       ] = "n"  ;                                                  defaults["dryrun"       ] = 0
    longopts["dry-run"      ] = "n"  ;                                                  defaults["dry-run"      ] = 0
    longopts["quiet"        ] = "q"  ;                                                  defaults["quiet"        ] = 0
    longopts["silent"       ] = "q"  ;                                                  defaults["silent"       ] = 0
    longopts["debug"        ] = "D::";                                                  defaults["debug"        ] = 0
    longopts["verbose"      ] = "v::";                                                  defaults["verbose"      ] = 0
    longopts["help"         ] = "h"  ;
    longopts["version"      ] = "V"  ;
    longopts["copying"      ] = "C"  ;
    # set mandatory defaults
    values["suse"         ] = defaults["suse"    ]
    values["redhat"       ] = defaults["redhat"  ]
    values["kversion"     ] = defaults["kversion"]
    values["inputs"       ] = defaults["inputs"  ]
    values["outfile"      ] = defaults["outfile" ]
    values["debug"        ] = defaults["debug"   ]
    values["verbose"      ] = defaults["verbose" ]
    values["quiet"        ] = defaults["quiet"   ]
    if ("V" in ENVIRON && ENVIRON["V"] == 1) {
	values["quiet"  ] = 0
	values["verbose"] = 2
    }
    optstring = "I:srk:o:nqD::v::hVC"
    optind = 0
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	if (c == -1) break
	else if (c ~ /[Iko]/)  { values[option] = optarg }
	else if (c ~ /[srnq]/) { values[option] = 1 }
	else if (c == "D")     { if (optarg) { values["debug"  ] = optarg } else { values["debug"  ]++ } }
	else if (c == "v")     { if (optarg) { values["verbose"] = optarg } else { values["verbose"]++ } }
	else if (c == "h")     { help(   stdout); exit 0 }
	else if (c == "V")     { version(stdout); exit 0 }
	else if (c == "C")     { copying(stdout); exit 0 }
	else                   { usage(  stderr); exit 2 }
    }
    if (optind < ARGC) {
	values["inputs"] = ARGV[optind]; optind++
	while (optind < ARGC) {
	    values["inputs"] = values["inputs"] " " ARGV[optind]
	    optind++
	}
    }
    for (i=1;ARGC>i;i++) { delete ARGV[i] }
    for (value in values) {
	if (!values[value] && (value in environs) && (environs[value] in ENVIRON) && ENVIRON[environs[value]]) {
	    print_debug("assigning value for " value " from environment " environs[value])
	    values[value] = ENVIRON[environs[value]]
	}
	if (!values[value] && (value in defaults) && defaults[value]) {
	    print_debug("assigning value for " value " from default " defaults[value])
	    values[value] = defaults[value]
	}
    }
    if (values["suse"] == 1) { values["redhat"] = 0 }
    if (!values["inputs"] || values["inputs"] == "-") {
	print_debug("no inputs, using " stdin)
	values["inputs"] = stdin
    }
    split(values["inputs"], inputs)
    read_inputs(inputs)
    directory = values["outfile"]
    sub(/\.tar\.gz/, "", directory)
    tarball = directory ".tar.gz"
    dirname  = getline_command("dirname "  directory)
    basename = getline_command("basename " directory)
    if (dirname == ".") { chngdir = "" } else { chngdir = " -C " dirname }
    system_command("mkdir -p " directory)
    if (system("test -f " tarball) == 0) {
	print_debug("unpacking tarball " tarball)
	system_command("tar -xzf " tarball chngdir)
    }

    print_debug("processing symbols")
    for (symset in symsets) {
	print_debug("processing symbol set " symset)
	if (values["redhat"] == 1) {
	    # note that RedHat does not protect the symbol name,
	    # the location of the symbol within the kernel, nor
	    # the export type in the kABI, whereas SuSE does.
	    $0 = getline_command("echo '" crcsets[symset] "' | sha1sum")
	    hash = $1
	    print_debug("hash is: '" hash "'")
	} else {
	    hash = "tmp"
	}
	print_debug("removing " directory "/" symset ".*")
	system_command("rm -f -- " directory "/" symset ".*")
	file = directory "/" symset "." hash
	print_debug("creating " file)
	n = split(symsets[symset],symbols)
	n = asort(symbols)
	for (i=1;i<=n;i++) {
	    sym = symbols[i]
	    print syms[sym] > file
	}
	close(file)
	if (hash == "tmp") {
	    hash = getline_command("md5sum " file)
	    hash = substr(hash, 1, 16)
	    print_debug("hash is: '" hash "'")
	    print_debug("moving " file " to " directory "/" symset "." hash)
	    system_command("mv -f -- " file " " directory "/" symset "." hash)
	}
    }
    print_debug("done processing symbols")
    print_debug("creating " tarball)
    system_command("tar -czf " tarball chngdir " " basename)
    print_debug("removing working directory " directory)
    system_command("rm -fr -- " directory)
    exit 0
}
END {
    if (written[stdout]) {
	close(stdout)
	written[stdout] = 0
    }
    if (written[stderr]) {
	close(stderr)
	written[stderr] = 0
    }
}

# =============================================================================
#
# $Log: symsets.awk,v $
# Revision 1.1.2.1  2011-03-17 07:01:22  brian
# - added new symsets script
#
#
# =============================================================================
# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
