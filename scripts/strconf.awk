#!/usr/bin/awk -f
# =============================================================================
#
# @(#) scripts/strconf.awk
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
# =============================================================================

# Streams configuration ala awk

function getline_command(cmd)
{
    cmd | getline; close(cmd); return $0
}
function date(format)
{
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
function print_vinfo(level, string)
{
    if ((values["quiet"] == 0) && (values["verbose"] >= level)) {
	print blu me ": I: " string std > stdout
	written[stdout] = 1
    }
}
function print_vmore(level, string)
{
    if (prog == "yes" && (values["quiet"] == 0) && (values["verbose"] >= level)) {
	printf "%s", blu me ": I: " string std cr lf > stdout
	fflush(stdout)
	written[stdout] = 1
    }
}
function print_debug(level, string)
{
    if (values["debug"] >= level) {
	print mag me ": D: " string std > stderr
	written[stderr] = 1
    }
}
function print_dmore(level, string)
{
    if (prog == "yes" && values["debug"] >= level) {
	printf "%s", mag me ": D: " string std cr lf > stderr
	fflush(stderr)
	written[stderr] = 1
    }
}
function print_error(string)
{
    print red me ": E: " string std > stderr
    if (count_errs) errors = errors "\n"
    errors = errors red me ": E: " string std
    written[stderr] = 1
    count_errs++
}
function print_emore(string)
{
    print red me ": E: " string std > stderr
    if (count_errs) errors = errors "\n"
    errors = errors red me ": E: " string std
    written[stderr] = 1
}
function print_warns(string)
{
    if ((values["quiet"] == 0) || (values["verbose"] > 0) || (values["debug"] > 0)) {
	print org me ": W: " string std > stderr
	written[stderr] = 1
    }
    count_warn++
}
function print_wmore(string)
{
    if ((values["quiet"] == 0) || (values["verbose"] > 0) || (values["debug"] > 0)) {
	print org me ": W: " string std > stderr
	written[stderr] = 1
    }
}
function usage(output)
{
    if (values["quiet"])
	return
    print "\
" me ":\n\
  scripts/strconf.awk\n\
Usage:\n\
  [awk -f ]" me " -- [options] [INPUT ...]\n\
  [awk -f ]" me " -- -" gensub(/!/, "", 1, longopts["help"]) ", --help\n\
  [awk -f ]" me " -- -" gensub(/!/, "", 1, longopts["version"]) ", --version\n\
  [awk -f ]" me " -- -" gensub(/!/, "", 1, longopts["copying"]) ", --copying\
" > output
    written[output] = 1
}
function help_usage(name,  line,sep,dflt,env,valu)
{
    line = ""; sep = ""; dflt = ""; env = ""
    if (name in defaults && longopts[name] !~ /!/) {
	if (longopts[name]~/:/) {
	    dflt = defaults[name]
	} else {
	    if (defaults[name]) { dflt = "yes" } else { dflt = "no" }
	}
	#if (dflt) {
	    line = line sep "[default: '" dflt "']"; sep = " "
	#}
    }
    if (name in environs && longopts[name] !~ /!/) {
	if (longopts[name]!~/:/) env = "?"
	if (environs[name])
	    line = line sep "{" env environs[name] "}"; sep = " "
    }
    if (name in values && longopts[name] !~ /!/) {
	if (longopts[name]~/:/) {
	    valu = values[name]
	} else {
	    if (values[name]) { valu = "yes" } else { valu = "no" }
	}
	if (line) sep = "\n      "
	line = line sep "(current: '" valu "')"; sep = " "
    }
    return line
}
function help_opttags(name,  line,char,opt,oth)
{
    line = ""
    if (name in longopts) {
	opt = longopts[name]
	if (opt~/[[:alnum:]]/) {
	    char = opt
	    gsub(/[:!]/,"",char)
	    line = "-" char ", --" name
	} else {
	    line = "--" name
	}
	if (name in longargs) { line = line " " longargs[name] } else
	if (opt~/::$/) { line = line " [" toupper(name) "]" } else
	if (opt~/:$/)  { line = line " " toupper(name) } else
	if (opt!~/!$/) { line = line ", --no-" name }
	if (opt~/[[:alnum:]]/) {
	    for (oth in longopts) {
		if (opt != longopts[oth]) continue
		if (oth == name) continue
		line = line ", --" oth
		if (oth in longargs) { line = line " " longargs[oth] } else
		if (opt~/::$/) { line = line " [" toupper(oth) "]" } else
		if (opt~/:$/)  { line = line " " toupper(oth) } else
		if (opt!~/!$/) { line = line ", --no-" oth }
	    }
	}
    }
    return line
}
function help_option(output, name,
		     desc)
{
    printf "  %s\n", help_opttags(name) > output
    if (name in descrips && descrips[name])
	printf "      %s\n", gensub(/\n/, "\n      ", "g", descrips[name]) > output
    if ((name in defaults || name in environs) && help_usage(name))
    printf "      %s\n", help_usage(name) > output
}
function help_options(output,		opt,char,pos,long,n,sorted,i)
{
    if (!optstring) return
    print "Options:" > output; written[output] = 1
    # index all of the long options
    for (opt in longopts) {
	char = substr(longopts[opt],1,1)
	if (char && char != ":") {
	    if (char in optchars) {
		if (char == substr(opt,1,1))
		    optchars[char] = opt
	    } else
		optchars[char] = opt
	}
    }
    for (pos=1;pos<=length(optstring);pos++) {
	char = substr(optstring,pos,1)
	if (char == "*") {
	    # document any long-only options
	    n = asorti(longopts,sorted)
	    for (i=1;i<=n;i++) {
		long = sorted[i]
		if (longopts[long]~/^[[:alnum:]]/) continue
		help_option(output, long)
	    }
	}
	if (char!~/[[:alnum:]]/) continue
	if (!(char in optchars)) continue
	help_option(output, optchars[char])
    }
}
function help(output)
{
    if (values["quiet"])
	return
    usage(output)
    print "\
Arguments:\n\
  " longargs["inputs"] "\n\
      " descrips["inputs"] "\n\
      [default: " defaults["inputs"] "] {" environs["inputs"] "}\
" > output
    help_options(output)
    written[output] = 1
}
function version(output)
{
    if (values["quiet"])
	return
    print "\
Version 2.1\n\
scripts/strconf.awk\n\
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
scripts/strconf.awk\n\
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
function getopt_long(argc, argv, optstring, longopts, longindex,
		     pos, needarg, wantarg, modearg)
{
    option = ""; optval = ""; optarg = ""; optset = ""; pos = 0; needarg = 0; wantarg = 0
    if (optind == 0) { optind = 1; more = "" }
    while ((optind < argc) || (more != "")) {
	if (more) { arg = "-" more; more = "" }
	else { arg = argv[optind]; optind++ }
	if (arg ~ /^--?[a-zA-Z0-9]/) {
	    if (wantarg) {
		more = substr(arg, 2)
		if (option in defaults)
		    optarg = defaults[option]
		return optval
	    }
	    if (needarg) {
		print_error("option -" optval " requires an argument")
		usage(stderr)
		exit 2
	    }
	    if (arg ~ /^--[a-zA-Z0-9][-_a-zA-Z0-9]*=.*$/) {
		option = arg; sub(/^--/, "", option); sub(/=.*$/, "", option)
		optarg = arg; sub(/^--([a-zA-Z0-9][-_a-zA-Z0-9]*)=/, "", optarg)
		optset = 1
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
		optset = 1
		if (!(option in longopts)) {
		    optset = 0
		    sub(/^no-/,"",option)
		if (!(option in longopts)) {
		    print_error("option --" option " not recognized")
		    usage(stderr)
		    exit 2
		}
		}
		if (longopts[option] ~ /::/) { wantarg = 1 } else
		if (longopts[option] ~ /:/ ) { needarg = 1 } else
		if (longopts[option] ~ /!/ ) { modearg = 1 }
		if ((wantarg || needarg || modearg) && !optset) {
		    print_error("option --no-" option " not recognized")
		    usage(stderr)
		    exit 2
		}
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
		optset = 1
		if (pos == 0 || substr(optstring, pos, 1) == "*") {
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
		} else optset = 1
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
	if (arg == "--") {
	    if (wantarg) {
		if (option in defaults)
		    optarg = defaults[option]
		optind--
		return optval
	    }
	    return -1
	}
	if (needarg || wantarg) {
	    optarg = arg
	    return optval
	}
	optind--
	return -1
    }
    if (wantarg) {
	if (option in defaults)
	    optarg = defaults[option]
	return optval
    }
    if (needarg) {
	print_error("option -" optval " requires an argument")
	usage(stderr)
	exit 2
    }
    return -1
}
function system_command(cmd)
{
    print_debug(3, "x: " cmd); return system(cmd)
}
function doit(cmd)
{
    if (values["dry-run"] == 0)
	return system_command(cmd)
    print_vinfo(2, "x: execute,   " cmd)
    return 0
}
function file_compare(tmpfile,    file) {
    file = tmpfile
    sub(/\.tmp$/, "", file)
    if (system("diff -I 'GENERATED BY' " tmpfile " " file " >/dev/null 2>&1")) {
	system("rm -f " file)
	system("mv -f " tmpfile " " file)
    } else {
	system("rm -f " tmpfile)
	print_vinfo(1,file " is unchanged")
    }
}
function already(what, object, where) {
    print_error(fnlp what " `" object "' already defined at " where)
}
function missing(what, expected) {
    print_error(fnlp what " definition missing " (expected - NF) " fields")
}
function garbage(what, maximum) {
    print_warns(fnlp what " definition with " (NF - maximum) " garbage fields")
}
function nmajors(nminors,    exponents, size, result) {
    exponents[0] = 1
    exponents[8] = 256
    exponents[16] = 65536
    exponents[20] = 1048576
    size = exponents[values["minorbits"]]
    result = (nminors + size - 1) / size
    sub(/\..*/, "", result)
    return result
}

function read_inputs(inputs) {
    print_debug(1,"r: inputs")
    imajor = values["basemajor"]
    idnumber = values["basemodid"]
    # process all of the input files
    indices["driver_names"] = 0
    indices["module_names"] = 0
    indices["drv_objnames"] = 0
    indices["mod_objnames"] = 0
    indices["node_names"  ] = 0
    for (k in inputs) {
	input = inputs[k]
	print_debug(1,"r: inputs, input " input)
	filename = input
	lineno = 0
	while ((getline < input) == 1) {
	    lineno++; fnl = filename ":" lineno; fnlp = fnl ": "
	    for (n = 1; n <= NF; n++) if ($n ~ /^#/) { NF = n - 1; break }
	    if (NF < 1) continue
	    if ($1 == "file") {
		if (NF < 2) { missing("file", 2); continue }
		if (NF > 2) { garbage("file", 2); NF = 2 }
		filename = $2; fnl = filename ":" lineno; fnlp = fnl ": "
	    } else
	    if ($1 == "line") {
		if (NF < 2) { missing("line", 2); continue }
		if (NF > 2) { garbage("line", 2); NF = 2 }
		lineno = $2; fnl = filename ":" lineno; fnlp = fnl ": "
	    } else
	    if ($1 == "driver") {
		if (NF < 3) { missing("driver", 3); continue }
		if (NF > 6) { garbage("driver", 6); NF = 6 }
		assigned = 0
		if ($2 in drivers) { already("driver", $2, drivers[$2]); continue }
		if (NF < 4 || $4 ~ /^[-*]?$/) { $4 = imajor; assigned = 1 }
		if (NF < 5 || $5 ~ /^[-*]?$/) { $5 = 1  } else
		if (NF < 5 || $5 !~ /^[0-9]+$/) { print_error("r: inputs, " fnlp "bad nminors field `" $5 "'"); continue }
		if (NF < 6 || $6 ~ /^[-*]?$/) { $6 = nmajors($5) } else
		if (NF < 6 || $6 !~ /^[0-9]+$/) { print_error("r: inputs, " fnlp "bad nmajors field `" $6 "'"); continue }
		drivers[$2] = fnl
		driver_prefix[$2] = $3
		driver_major[$2] = $4
		driver_nminors[$2] = $5
		driver_nmajors[$2] = $6
		driver_id[$2] = $4
		indices["driver_names"]++
		driver_names[indices["driver_names"]] = $2
		if (assigned)
		    imajor += $6
	    } else
	    if ($1 == "module") {
		if (NF < 3) { missing("module", 3); continue }
		if (NF > 3) { garbage("module", 3); NF = 3 }
		if ($2 in modules) { already("module", $2, modules[$2]); continue }
		modules[$2] = fnl
		module_prefix[$2] = $3
		module_id[$2] = idnumber++
		indices["module_names"]++
		module_names[indices["module_names"]] = $2
	    } else
	    if ($1 == "objname") {
		if (NF < 4) { missing("object", 4); continue }
		if (NF > 4) { garbage("object", 4); NF = 4 }
		if ($2 == "driver") {
		    if ($3 in objname_drivers) { already("objname", $3, objname_drivers[$2]); continue }
		    objname_drivers[$3] = fnl
		    driver_objname[$3] = $4
		    driver_objects[$4] = $3
		    indices["drv_objnames"]++
		    drv_objnames[indices["drv_objnames"]] = $4
		} else
		if ($2 == "module") {
		    if ($3 in objname_modules) { already("objname", $3, objname_modules[$2]); continue }
		    objname_modules[$3] = fnl
		    module_objname[$3] = $4
		    module_objects[$4] = $3
		    indices["mod_objnames"]++
		    mod_objnames[indices["mod_objnames"]] = $4
		} else {
		    print_error("r: inputs, " fnlp "unknown object name type `" $2 "'")
		    continue }
	    } else
	    if ($1 == "autopush") {
		if (NF < 5) { missing("autopush", 5); continue }
		if ($2 in autopush) { already("autopush", $2, autopush[$2]); continue }
		autopush[$2] = fnl
		autopush_minor[$2] = $3
		autopush_lastminor[$2] = $4
		autopush_modules[$2] = $5
		for (n = 6; n < NF; n++)
		    autopush_modules[$2] = autopush_modules[$2] " " $n
	    } else
	    if ($1 == "loadable") {
		if (NF < 2) { missing("loadable", 2); continue }
		if (NF > 2) { garbage("loadable", 2); NF = 2 }
		if ($2 in loadables) { already("loadable", $2, loadables[$2]); continue }
		loadables[$2] = fnl
	    } else
	    if ($1 == "initialize") {
		if (NF < 2) { missing("initialize", 2); continue }
		if (NF > 2) { garbage("initialize", 2); NF = 2 }
		if ($2 in initialized) { already("initialize", $2, initialized[$2]); continue }
		initialized[$2] = fnl
	    } else
	    if ($1 == "terminate") {
		if (NF < 2) { missing("terminate", 2); continue }
		if (NF > 2) { garbage("terminate", 2); NF = 2 }
		if ($2 in terminated) { already("terminate", $2, terminated[$2]); continue }
		terminated[$2] = fnl
	    } else
	    if ($1 == "node") {
		if (NF < 6) { missing("node", 6); continue }
		if (NF > 6) { garbage("node", 6); NF = 6 }
		if ($2 in nodes) { already("node", $2, nodes[$2]); continue }
		nodes[$2] = fnl
		node_devtype[$2] = $3
		if ($4 ~ /^[-*]$/) { $4 = values["permission"] }
		node_devperm[$2] = $4
		node_majname[$2] = $5
		node_minname[$2] = $6
		indices["node_names"]++
		node_names[indices["node_names"]] = $2
	    } else
	    if ($1 == "device") {
		if (NF < 11) { missing("device", 11); continue }
		if (NF > 11) { garbage("device", 11); NF = 11 }
		if ($2 in devices) { already("device", $2, devices[$2]); continue }
		devices[$2] = fnl
		device_unit[$2] = $3
		device_port[$2] = $4
		device_nports[$2] = $5
		device_share[$2] = $6
		device_irq[$2] = $7
		device_mem[$2] = $8
		device_size[$2] = $9
		device_dma1[$2] = $10
		device_dma2[$2] = $11
	    } else
	    if ($1 == "qlock") {
		if (NF < 4) { missing("qlock", 4); continue }
		if (NF > 4) { garbage("qlock", 4); NF = 4 }
		if ($2 == "driver") {
		    if ($3 in qlocks_driver) { already("qlock", $3, qlocks_driver[$3]); continue }
		    qlocks_driver[$3] = fnl
		    qlock_type[$3] = $4
		} else
		if ($2 == "module") {
		    if ($3 in qlocks_module) { already("qlock", $3, qlocks_module[$3]); continue }
		    qlocks_module[$3] = fnl
		    qlock_type[$3] = $4
		} else {
		    print_error("r: inputs, " fnlp "unrecognized qlock type `" $2 "'")
		    continue
		}
	    } else {
		print_warns("r: inputs, " fnlp "unknown command name `" $1 "'")
		continue }
	}
	close(input)
	print_debug(1,"r: inputs, input " input " done.")
    }
    print_debug(1,"r: inputs, done.")
}

function write_driverconf(file,    i, object, loads, links) {
    print_debug(1,"w: driverconf, file = `" file "'"); loads = ""; links = ""
    for (i = 1; i <= indices["drv_objnames"]; i++) {
	object = drv_objnames[i]
	if (object in loadables)
	     loads = loads " " object
	else links = links " " object
    }
    for (i = 1; i <= indices["mod_objnames"]; i++) {
	object = mod_objnames[i]
	if (object in loadables)
	     loads = loads " " object
	else links = links " " object
    }
    gsub(/[[:space:]]+/, " ", links)
    gsub(/[[:space:]]+/, " ", loads)
    print "\
# vim: ft=make\n\
# =============================================================================\n\
#\n\
# Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
#\n\
# All Rights Reserved.\n\
#\n\
# This program is free software; you can redistribute it and/or modify it under\n\
# the terms of the GNU Affero General Public License as published by the Free\n\
# Software Foundation; version 3 of the License.\n\
#\n\
# This program is distributed in the hope that it will be useful, but WITHOUT\n\
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
# details.\n\
#\n\
# You should have received a copy of the GNU Affero General Public License along\n\
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to\n\
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
# behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
# to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
# is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
# of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
# successor regulations) and the Government is acquiring only the license rights\n\
# granted herein (the license rights customarily provided to non-Government\n\
# users).  If the Software is supplied to any unit or agency of the Government\n\
# other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
# Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
# Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
# (or any successor regulations).\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# Commercial licensing and support of this software is available from OpenSS7\n\
# Corporation at a fee.  See http://www.openss7.com/\n\
#\n\
# =============================================================================\n\
\n\
#\n\
# GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
# Edits to this file will be lost: edit strconf.awk instead.\n\
#\n\
\n\
# --------------------------------\n\
\n\
MODCONF_LINKS =" links "\n\
MODCONF_LOADS =" loads "\n\
\n\
# --------------------------------\n\
# vim: ft=make\
" > file
    close(file)
}
function write_hconfig(file,    name, prefix) {
    print_debug(1,"w: hconfig, file = `" file "'")
    file = file ".tmp"
    print "\
/******************************************************************* vim: ft=c\n\
\n\
 @(#) " file "\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software; you can redistribute it and/or modify it under\n\
 the terms of the GNU Affero General Public License as published by the Free\n\
 Software Foundation; version 3 of the License.\n\
\n\
 This program is distributed in the hope that it will be useful, but WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU Affero General Public License\n\
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or\n\
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA\n\
 02139, USA.\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
 behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
 to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
 of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the Government is acquiring only the license rights\n\
 granted herein (the license rights customarily provided to non-Government\n\
 users).  If the Software is supplied to any unit or agency of the Government\n\
 other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
 Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
 Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Commercial licensing and support of this software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
 *****************************************************************************/\n\
\n\
#ifndef __SYS_" toupper(values["package"]) "_CONFIG_H__\n\
#define __SYS_" toupper(values["package"]) "_CONFIG_H__\n\
\n\
/*\n\
 * GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
 * EDITS TO THIS FILE WILL BE LOST: EDIT strconf.awk INSTEAD.\n\
 */\n\
\n\
/* -------------------------------------------------- */\
" > file
    # do some work in here
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	prefix = toupper(driver_prefix[name])
	gsub(/[^A-Z0-9]/, "_", prefix)
	print "\
#define CONFIG_STREAMS_" prefix "_NAME    \t\"" name "\"\n\
#define CONFIG_STREAMS_" prefix "_MODID   \t" driver_id[name] "\n\
#define CONFIG_STREAMS_" prefix "_NMINORS \t" driver_nminors[name] "\n\
#define CONFIG_STREAMS_" prefix "_NMAJORS \t" driver_nmajors[name] "\
" > file
	first = 0
	last = driver_nmajors[name]
	print "#define CONFIG_STREAMS_" prefix "_MAJOR   \t" driver_major[name] > file
	for (count = first; count < last; count++)
	    print "#define CONFIG_STREAMS_" prefix "_MAJOR_" count " \t" (driver_major[name] + count) > file
	if (driver_objname[name] in loadables)
	    print "#define CONFIG_STREAMS_" prefix "_MODULE  \t1" > file
    }
    for (i = 1; i <= indices["module_names"]; i++) {
	name = module_names[i]
	prefix = toupper(module_prefix[name])
	gsub(/[^A-Z0-9]/, "_", prefix)
	print "\
#define CONFIG_STREAMS_" prefix "_NAME    \t\"" name "\"\n\
#define CONFIG_STREAMS_" prefix "_MODID   \t" module_id[name] "\
" > file
	if (module_objname[name] in loadables)
	    print "#define CONFIG_STREAMS_" prefix "_MODULE  \t1" > file
    }
    print "\
/* -------------------------------------------------- */\n\
\n\
#endif				/* __SYS_" toupper(values["package"]) "_CONFIG_H__ */\n\
\n\
/*\n\
 * vim: ft=c\n\
 */\
" > file
    close(file)
    file_compare(file)
}
function write_modconf(file) {
    print_debug(1,"w: modconf, file = `" file "'")
    file = file ".tmp"
    print "\
/******************************************************************* vim: ft=c\n\
\n\
 @(#) " file "\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software; you can redistribute it and/or modify it under\n\
 the terms of the GNU Affero General Public License as published by the Free\n\
 Software Foundation; version 3 of the License.\n\
\n\
 This program is distributed in the hope that it will be useful, but WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU Affero General Public License\n\
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or\n\
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA\n\
 02139, USA.\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
 behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
 to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
 of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the Government is acquiring only the license rights\n\
 granted herein (the license rights customarily provided to non-Government\n\
 users).  If the Software is supplied to any unit or agency of the Government\n\
 other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
 Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
 Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Commercial licensing and support of this software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
 *****************************************************************************/\n\
\n\
#ifndef __SYS_" toupper(values["package"]) "__MODCONF_INC__\n\
#define __SYS_" toupper(values["package"]) "__MODCONF_INC__\n\
\n\
/*\n\
 * GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
 * EDITS TO THIS FILE WILL BE LOST: EDIT strconf.awk INSTEAD.\n\
 */\n\
\n\
/* -------------------------------------------------- */\
" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	if (!(driver_objname[name] in loadables)) {
	    prefix = driver_prefix[name]
	    print "\nextern struct streamtab " prefix "info;" > file
	    if (name in initialized)
		print "extern int  " prefix "init(void);" > file
	    if (name in terminated)
		print "extern void " prefix "exit(void);" > file
	    count = driver_major[name]; majors = count
	    for (count = count + 1; count < driver_major[name] + driver_nmajors[name]; count++)
		majors = majors ", " count
	    print "int " prefix "_0_majors[] = { " majors " };" > file
	}
    }
    for (i = 1; i <= indices["module_names"]; i++) {
	name = module_names[i]
	if (!(module_objname[name] in loadables)) {
	    prefix = module_prefix[name]
	    print "\nextern struct streamtab " prefix "info;" > file
	    if (name in initialized)
		print "extern int  " prefix "init(void);" > file
	    if (name in terminated)
		print "extern void " prefix "exit(void);" > file
	}
    }
    print "\ndriver_config_t lfs_driver_config[] = {" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	if (!(driver_objname[name] in loadables)) {
	    prefix = driver_prefix[name]
	    if (name in terminated ) { exitfunc = prefix "exit" } else { exitfunc = "NULL" }
	    if (name in initialized) { initfunc = prefix "init" } else { initfunc = "NULL" }
	    print "\t{ \"" name "\", &" prefix "info, " prefix "_0_majors, " \
		driver_nmajors[name] ", " driver_nminors[name] ", " initfunc ", " \
		exitfunc ", -1 }," > file
	}
    }
    print "};" > file
    print "\ndevice_config_t lfs_device_config[] = {" > file
    print "};" > file
    print "\nmodule_config_t lfs_module_config[] = {" > file
    print "\t{ \"\", NULL, \"\" },\t/* empty 0th entry */" > file
    for (i = 1; i <= indices["module_names"]; i++) {
	name = module_names[i]
	if (!(module_objname[name] in loadables)) {
	    prefix = module_prefix[name]
	    if (name in terminated ) { exitfunc = prefix "exit" } else { exitfunc = "NULL" }
	    if (name in initialized) { initfunc = prefix "init" } else { initfunc = "NULL" }
	    print "\t{ \"" name "\", &" prefix "info, \"\", " initfunc ", " exitfunc ", -1 }," > file
	}
    }
    print "};" > file
    print "\n#ifdef LFS_LOADABLE_SUPPORT" > file
    print "driver_obj_name_t lfs_drv_objnames[] = {" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	if (!(driver_objname[name] in loadables)) {
	    prefix = driver_prefix[name]
	    if (name in initialized) { initfunc = prefix "init" } else { initfunc = "NULL" }
	    print "\t{ " prefix "_0_majors, " driver_nmajors[name] ", " initfunc ", \"" \
		driver_objname[name] "\" }," > file
	}
    }
    print "};" > file
    print "#endif\t\t/* LFS_LOADABLE_SUPPORT */" > file
    print "\nautopush_init_t lfs_apush_init[] = {" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	if (!(driver_objname[name] in loadables) && autopush_modules[name]) {
	    prefix = driver_prefix[name]
	    pushes = "\"" autopush_modules[name] "\", "
	    count = gsub(/[[:space:]]+/, ",", pushes) + 1
	    print "\t{ " driver_major[name] ", " autopush_minor[name] ", " \
		autopush_lastminor[name] ", " count ", " " { " pushes " } }," > file
	}
    }
    print "};" > file
    print "\n\
/* -------------------------------------------------- */\n\
\n\
#endif				/* __SYS_" toupper(values["package"]) "__MODCONF_INC__ */\n\
\n\
/*\n\
 * vim: ft=c\n\
 */\
" > file
    close(file)
    file_compare(file)
}
function write_makenodes(file) {
    print_debug(1,"w: makenodes, file = `" file "'")
    file = file ".tmp"
    print "\
/******************************************************************* vim: ft=c\n\
\n\
 Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software: you can redistribute it and/or modify it under\n\
 the terms of the GNU Affero General Public License as published by the Free\n\
 Software Foundation, version 3 of the license.\n\
\n\
 This program is distributed in the hope that it will be useful, but WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU Affero General Public License\n\
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or\n\
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA\n\
 02139, USA.\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
 behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
 to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
 of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the Government is acquiring only the license rights\n\
 granted herein (the license rights customarily provided to non-Government\n\
 users).  If the Software is supplied to any unit or agency of the Government\n\
 other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
 Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
 Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Commercial licensing and support of this software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
 *****************************************************************************/\n\
\n\
/*\n\
 * GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
 * EDITS TO THIS FILE WILL BE LOST: EDIT strconf.awk INSTEAD.\n\
 */\n\
\n\
static char const ident[] = \"makenodes (\" PACKAGE_ENVR \" \" PACKAGE_DATE \")\";\n\
\n\
#if defined(LINUX)\n\
#	include <sys/types.h>\n\
#	include <sys/stat.h>\n\
#	include <sys/sysmacros.h>\n\
#	define makedevice(maj,min) makedev(maj,min)\n\
#elif defined(QNX)\n\
#	include <sys/types.h>\n\
#	include <sys/stat.h>\n\
#	define makedevice(maj,min) makedev(1,maj,min)\n\
#elif defined(USER)\n\
#	include <sys/stropts.h>\n\
#	include <sys/LiS/usrio.h>\n\
#else\n\
#	include <sys/types.h>\n\
#	include <sys/stat.h>\n\
#	define makedevice(maj,min) makedev(maj,min)\n\
#endif\n\
#ifdef _GNU_SOURCE\n\
#       include <getopt.h>\n\
#endif\n\
\n\
#include <stdio.h>\n\
#include <unistd.h>\n\
#include <stdlib.h>\n\
\n\
#if !defined(LINUX)\n\
#	include <fcntl.h>\n\
#endif\n\
\n\
#if !defined(USER)\n\
void copying(int argc, char *argv[])\n\
{\n\
	fprintf(stdout, \"\\\n\
\\n\\\n\
%1$s %2$s:\\n\\\n\
\\n\\\n\
Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\\n\\\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\\n\\\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\\n\\\n\
\\n\\\n\
All Rights Reserved.\\n\\\n\
\\n\\\n\
This program is free software: you can  redistribute it  and/or modify  it under\\n\\\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\\n\\\n\
Software Foundation, version 3 of the license.\\n\\\n\
\\n\\\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\\n\\\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\\n\\\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\\n\\\n\
\\n\\\n\
You should have received a copy of the  GNU Affero General Public License  along\\n\\\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\\n\\\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\\n\\\n\
\\n\\\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\\n\\\n\
of the U.S. Government (\\\"Government\\\"), the following provisions apply to you. If\\n\\\n\
the Software is supplied by the Department of Defense (\\\"DoD\\\"), it is classified\\n\\\n\
as \\\"Commercial  Computer  Software\\\"  under  paragraph  252.227-7014  of the  DoD\\n\\\n\
Supplement  to the  Federal Acquisition Regulations  (\\\"DFARS\\\") (or any successor\\n\\\n\
regulations) and the  Government  is acquiring  only the  license rights granted\\n\\\n\
herein (the license rights customarily provided to non-Government users). If the\\n\\\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\\n\\\n\
is  classified as  \\\"Restricted Computer Software\\\" and the Government's rights in\\n\\\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\\n\\\n\
Regulations (\\\"FAR\\\")  (or any successor regulations) or, in the cases of NASA, in\\n\\\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\\n\\\n\
regulations).\\n\\\n\
\\n\\\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\\n\\\n\
Corporation at a fee.  See http://www.openss7.com/\\n\\\n\
\\n\\\n\
\", argv[0], ident);\n\
}\n\
\n\
void version(int argc, char *argv[])\n\
{\n\
	fprintf(stdout, \"\\\n\
\\n\\\n\
%1$s (OpenSS7 %2$s) %3$s:\\n\\\n\
Written by Brian Bidulock.\\n\\\n\
\\n\\\n\
Copyright (c) 2008, " allyears() "  Monavacon Limited.\\n\\\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\\n\\\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\\n\\\n\
This is free software; see the source for copying conditions.  There is NO\\n\\\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\\n\\\n\
\\n\\\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\\n\\\n\
with conditions, incorporated herein by reference.\\n\\\n\
\\n\\\n\
See `%1$s --copying' for copying permissions.\\n\\\n\
\\n\\\n\
\", NAME, PACKAGE, VERSION);\n\
}\n\
\n\
void usage(int argc, char *argv[])\n\
{\n\
	fprintf(stderr, \"\\\n\
Usage:\\n\\\n\
    %1$s [options]\\n\\\n\
    %1$s {-h,--help}\\n\\\n\
    %1$s {-V,--version}\\n\\\n\
    %1$s {-C,--copying}\\n\\\n\
\", argv[0]);\n\
}\n\
\n\
void help(int argc, char *argv[])\n\
{\n\
	fprintf(stdout, \"\\\n\
\\n\\\n\
Usage:\\n\\\n\
    %1$s [options]\\n\\\n\
    %1$s {-h,--help}\\n\\\n\
    %1$s {-V,--version}\\n\\\n\
    %1$s {-C,--copying}\\n\\\n\
Options:\\n\\\n\
    -r, --remove\\n\\\n\
        remove devices before making them\\n\\\n\
    -h, --help, -?\\n\\\n\
        print this usage information and exit\\n\\\n\
    -V, --version\\n\\\n\
        print version and exit\\n\\\n\
    -C, --copying\\n\\\n\
        print copying permission and exit\\n\\\n\
\\n\\\n\
\", argv[0]);\n\
}\n\
#endif				/* !defined(USER) */\n\
\n\
int main(int argc, char *argv[])\n\
{\n\
	int rslt;\n\
	int rmopt = 0;\n\
	char *strerror(int);\n\
\n\
        (void) rslt;\n\
#if !defined(USER)\n\
	(void) umask(0);\n\
	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'r')\n\
		rmopt = 1;\n\
\n\
	while (1) {\n\
		int c;\n\
#ifdef _GNU_SOURCE\n\
		int option_index = 0;\n\
		/* *INDENT-OFF* */\n\
		static struct option long_options[] = {\n\
			{ \"help\",    no_argument, NULL, 'h'},\n\
			{ \"version\", no_argument, NULL, 'V'},\n\
			{ \"copying\", no_argument, NULL, 'C'},\n\
			{ \"remove\",  no_argument, NULL, 'r'},\n\
			{ \"?\",       no_argument, NULL, 'h'},\n\
			{ 0, }\n\
		};\n\
		/* *INDENT-ON* */\n\
		c = getopt_long_only(argc, argv, \"rhVC?\", long_options, &option_index);\n\
#else				/* _GNU_SOURCE */\n\
		c = getopt(argc, argv, \"rhVC?\");\n\
#endif				/* _GNU_SOURCE */\n\
		if (c == -1)\n\
			break;\n\
		switch (c) {\n\
		case 'r':	/* -r, --remove */\n\
			rmopt = 1;\n\
                        break;\n\
		case 'h':	/* -h, --help */\n\
			help(argc, argv);\n\
			exit(0);\n\
		case 'V':	/* -V, --version */\n\
			version(argc, argv);\n\
			exit(0);\n\
		case 'C':	/* -C, --copying */\n\
			copying(argc, argv);\n\
			exit(0);\n\
		case '?':\n\
			optind--;\n\
		      bad_nonoption:\n\
			if (optind < argc) {\n\
				fprintf(stderr, \"%s: illegal syntax -- \", argv[0]);\n\
				for (; optind < argc; optind++)\n\
					fprintf(stderr, \"%s \", argv[optind]);\n\
				fprintf(stderr, \"\\n\");\n\
			}\n\
			usage(argc, argv);\n\
			exit(2);\n\
		}\n\
	}\n\
	if (optind < argc)\n\
		goto bad_nonoption;\n\
\n\
#endif\n\
	(void) ident;\n\
\n\
/* -------------------------------------------------- */\n\
" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: makenodes, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = 0
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: makenodes, invalid minor name `" minname "'"); continue }
	print "\
#if !defined(USER)\n\
	(void) unlink(\"" name "\");\n\
#endif\n\
	if (!rmopt) {\n\
		rslt = mknod(\"" name "\", 02" node_devperm[name] ", makedevice(" majnumb ", " minnumb "));\n\
		if (rslt < 0)\n\
			printf(\"" name ": %s\\n\", strerror(-rslt));\n\
	}\
" > file
    }
    print "\n\
/* -------------------------------------------------- */\n\
\n\
#if !defined(USER)\n\
	exit(0);\n\
#else\n\
	return (0);\n\
#endif\n\
}\n\
\n\
/* \n\
 * vim: ft=c\n\
 */\
" > file
    close(file)
    file_compare(file)
}
function write_mkdevices(file,    i, name, majname, majnumb, minname, minnumb, dir, dirs, create, remove) {
    print_debug(1,"w: mkdevices, file = `" file "'"); create = ""; remove = ""
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: mkdevices, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = 0
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: mkdevices, invalid minor name `" minname "'"); continue }
	if (majnumb == 230) majname = "clone"
	dir = name; sub(/\/[^\/]*$/, "", dir); dirs[dir] = 1
	remove = remove "\n\trm -f -- $DESTDIR" name
	if (values["minorbits"] + 0 > 8) {
	    create = create "\n\tmknod -m " node_devperm[name] " $DESTDIR" name " c " majnumb " " minnumb
	} else {
	    create = create "\n\tln -s /dev/streams/" majname "/" minname " $DESTDIR" name
	}
    }
    for (dir in dirs) {
	if (dir && dir != "/dev") {
	    remove = remove "\n\ttest -d " dir " && rmdir --ignore-fail-on-non-empty -- $DESTDIR" dir
	    create = "\n\ttest -d " dir " || mkdir -p -- $DESTDIR" dir create
	}
    }
    remove = remove "\n"
    create = create "\n"
    print "#!/bin/sh" > file
    print "remove() {" remove "}" > file
    print "create() {" create "}" > file
    print "\n\
if test \":$V\" = :1 ; then\n\
    set -x\n\
fi\n\
if test $# -ne 1 ; then\n\
    set $0 --create\n\
fi\n\
case :$1 in\n\
    :-c|:--create)\n\
	remove\n\
	create\n\
	exit 0\n\
	;;\n\
    :-r|:--remove)\n\
	remove\n\
	exit 0\n\
	;;\n\
    :-h|:--help)\n\
	cat<<EOF\n\
usage:\n\
    `basename $0` {-c,--create}\n\
    `basename $0` {-r,--remove}\n\
    `basename $0` {-h,--help}\n\
    `basename $0` {-V,--version}\n\
    `basename $0` {-C,--copying}\n\
EOF\n\
	exit 0\n\
	;;\n\
    :-V|:--version)\n\
	echo \"Version 1.1.2\"\n\
	exit 0\n\
	;;\n\
    :-C|:--copying)\n\
	cat<<EOF\n\
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
EOF\n\
	exit 0\n\
	;;\n\
    *)\n\
	cat 1>&2 <<EOF\n\
usage:\n\
    `basename $0` {-c,--create}\n\
    `basename $0` {-r,--remove}\n\
    `basename $0` {-h,--help}\n\
    `basename $0` {-V,--version}\n\
    `basename $0` {-C,--copying}\n\
EOF\n\
	exit 2\n\
	;;\n\
esac\n\
" > file
    close(file)
    system("chmod 0755 " file)
}
function write_strmknods(file) {
    print_debug(1,"w: strmknods, file = `" file "'")
    print "%defattr(-,root,root)" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: strmknods, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = 0
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: strmknods, invalid minor name `" minname "'"); continue }
	print "%attr(" node_devperm[name] ", root, root) %dev(c, " majnumb ", " minnumb ") " name > file
    }
    close(file)
}
function write_strsetup(file) {
    print_debug(1,"w: strsetup, file = `" file "'")
    print "\
# vim: ft=conf\n\
# =============================================================================\n\
#\n\
# Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
#\n\
# All Rights Reserved.\n\
#\n\
# This program is free software; you can redistribute it and/or modify it under\n\
# the terms of the GNU Affero General Public License as published by the Free\n\
# Software Foundation; version 3 of the License.\n\
#\n\
# This program is distributed in the hope that it will be useful, but WITHOUT\n\
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
# details.\n\
#\n\
# You should have received a copy of the GNU Affero General Public License along\n\
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to\n\
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
# behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
# to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
# is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
# of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
# successor regulations) and the Government is acquiring only the license rights\n\
# granted herein (the license rights customarily provided to non-Government\n\
# users).  If the Software is supplied to any unit or agency of the Government\n\
# other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
# Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
# Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
# (or any successor regulations).\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# Commercial licensing and support of this software is available from OpenSS7\n\
# Corporation at a fee.  See http://www.openss7.com/\n\
#\n\
# =============================================================================\n\
\n\
#\n\
# GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
# Edits to this file will be lost: edit strconf.awk instead.\n\
#\n\
# --------------------------------\n\
\n\
#\n\
#   strsetup configuration file\n\
#\n\
" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: strsetup, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = 0
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: strsetup, invalid minor name `" minname "'"); continue }
	minor = minnumb
	if (majname == "clone" && minnumb != 0) {
	    majname = minname
	    minor = "clone"
	}
	print majname "\t" name "\t" node_devperm[name] "\t" minor "\t# major " majnumb "\tminor " minnumb > file
    }
    print "\n\
# --------------------------------\n\
# vim: ft=conf\
" > file
    close(file)
}
function write_strload(file) {
    print_debug(1,"w: strload, file = `" file "'")
    print "\
# vim: ft=conf\n\
# =============================================================================\n\
#\n\
# Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
#\n\
# All Rights Reserved.\n\
#\n\
# This program is free software; you can redistribute it and/or modify it under\n\
# the terms of the GNU Affero General Public License as published by the Free\n\
# Software Foundation; version 3 of the License.\n\
#\n\
# This program is distributed in the hope that it will be useful, but WITHOUT\n\
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
# details.\n\
#\n\
# You should have received a copy of the GNU Affero General Public License along\n\
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to\n\
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
# behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
# to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
# is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
# of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
# successor regulations) and the Government is acquiring only the license rights\n\
# granted herein (the license rights customarily provided to non-Government\n\
# users).  If the Software is supplied to any unit or agency of the Government\n\
# other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
# Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
# Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
# (or any successor regulations).\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# Commercial licensing and support of this software is available from OpenSS7\n\
# Corporation at a fee.  See http://www.openss7.com/\n\
#\n\
# =============================================================================\n\
\n\
#\n\
# GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
# Edits to this file will be lost: edit strconf.awk instead.\n\
#\n\
# --------------------------------\n\
\n\
#\n\
#   strload configuration file\n\
#\n\
\n\
# Attr	Filename	Argument	Node		Minors\n\
" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: strload, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = 0
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: strload, invalid minor name `" minname "'"); continue }
	cloneflag = "s"
	minor = minnumb
	if (majname == "clone" && minnumb != 0) {
	    majname = minname
	    minor = ""
	    cloneflag = ""
	}
	devname = name
	if (name == "/dev/" majname) { devname = "-" }
	print "d" cloneflag "\t" majname "\t\t-\t\t" devname "\t" minor > file
	seen[majname] = 1
    }
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	if (name in seen) continue
	print "d\t" name > file
    }
    for (i = 1; i <= indices["module_names"]; i++) {
	name = module_names[i]
	print "m\t" name > file
    }
    print "\n\
# --------------------------------\n\
# vim: ft=conf\
" > file
    close(file)
}
function write_confmodules(file) {
    print_debug(1,"w: confmodules, file = `" file "'")
    print "\
# vim: ft=conf\n\
# =============================================================================\n\
#\n\
# Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
#\n\
# All Rights Reserved.\n\
#\n\
# This program is free software; you can redistribute it and/or modify it under\n\
# the terms of the GNU Affero General Public License as published by the Free\n\
# Software Foundation; version 3 of the License.\n\
#\n\
# This program is distributed in the hope that it will be useful, but WITHOUT\n\
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
# details.\n\
#\n\
# You should have received a copy of the GNU Affero General Public License along\n\
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to\n\
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
# behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
# to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
# is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
# of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
# successor regulations) and the Government is acquiring only the license rights\n\
# granted herein (the license rights customarily provided to non-Government\n\
# users).  If the Software is supplied to any unit or agency of the Government\n\
# other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
# Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
# Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
# (or any successor regulations).\n\
#\n\
# -----------------------------------------------------------------------------\n\
#\n\
# Commercial licensing and support of this software is available from OpenSS7\n\
# Corporation at a fee.  See http://www.openss7.com/\n\
#\n\
# =============================================================================\n\
\n\
#\n\
# GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
# Edits to this file will be lost: edit strconf.awk instead.\n\
#\n\
# --------------------------------\n\
" > file
    print "# older style character device demand loading entries" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	first = driver_major[name]
	last = first + driver_nmajors[name]
	for (count = first; count < last; count++) {
	    object = driver_objname[name]
	    if (object in loadables) {
		print "alias\tchar-major-" count "\t\tstreams-" object "\t# driver " name > file
	    } else {
		print "alias\tchar-major-" count "\t\tstreams\t\t# driver " name > file
	    }
	}
    }
    print "\n# alias the driver name to the object name where they do not match" > file
    for (i = 1; i <= indices["driver_names"]; i++) {
	name = driver_names[i]
	object = driver_objname[name]
	if (object in loadables) {
	    if (object != name)
		print "alias\tstreams-" name "\t\tstreams-" object "\t# driver " name " in " object > file
	} else {
	    print "alias\tstreams-" name "\t\tstreams\t\t# driver " name " in streams" > file
	}
    }
    print "\n# alias the module name to the object name where they do not match" > file
    for (i = 1; i < indices["module_names"]; i++) {
	name = module_names[i]
	object = module_objname[name]
	if (object in loadables) {
	    if (object ! name)
		print "alias\tstreams-" name "\t\tstreams-" object "\t# module " name " in " object > file
	} else {
	    print "alias\tstreams-" name "\t\tstreams\t\t# driver " name " in streams" > file
	}
    }
    print "\n# new style devfs character device demand loading entries" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: confmodules, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = "*"
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: confmodules, invalid minor name `" minname "'"); continue }
	if (minname) { drvname = minname } else { drvname = majname }
	#object = driver_objname[drvname]
	if (drvname in driver_objname && driver_objname[drvname] in loadables) {
	    driver = driver_objects[driver_objname[drvname]]
	    print "alias\tchar-major-" majnumb "-" minnumb "\tstreams-" driver "\t# driver " name > file
	} else {
	    print "alias\tchar-major-" majnumb "-" minnumb "\tstreams\t\t# driver " name > file
	}
    }
    print "\n# newer style devfsd device demand loading entires" > file
    for (i = 1; i <= indices["node_names"]; i++) {
	name = node_names[i]
	majname = node_majname[name]
	majnumb = 230
	if (majname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { majnumb = driver_major[majname] }
	else if (majname ~ /^[0-9]+$/) { majnumb = majname }
	else { print_error("w: confmodules, invalid major name `" majname "'"); continue }
	minname = node_minname[name]
	minnumb = "*"
	if (minname ~ /^[a-zA-Z][-_a-zA-Z0-9]*$/) { minnumb = driver_major[minname] }
	else if (minname ~ /^[0-9]+$/) { minnumb = minname }
	else { print_error("w: confmodules, invalid minor name `" minname "'"); continue }
	if (minname) { drvname = minname } else { drvname = majname }
	#object = driver_objname[drvname]
	if (drvname in driver_objname && driver_objname[drvname] in loadables) {
	    driver = driver_objects[driver_objname[drvname]]
	    print "alias\tchar-major-" name "\tstreams-" driver "\t# driver " name > file
	} else {
	    print "alias\tchar-major-" name "\tstreams\t\t# driver " name > file
	}
    }
    print "\n\
# --------------------------------\n\
# vim: ft=conf\
" > file
    close(file)
}
function write_pkgobject(pkgobject,    file, object, name, prefix, count, first, last, initfunc, exitfunc) {
    object = pkgobject
    sub(/.*\//, "", object)
    sub(/\.o$/, "", object)
    file = object "_wrapper.c"
    file = file ".tmp"
    print_debug(1,"w: pkgobject, for `" pkgobject "' to `" file "'")
    if (!(object in driver_objects) && !(object in module_objects)) {
	print_error("w: pkgobject, cannot find object " object)
	return
    }
    print "\
/******************************************************************* vim: ft=c\n\
\n\
 Copyright (c) 2008-" year() "  Monavacon Limited <http://www.monavacon.com/>\n\
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software: you can redistribute it and/or modify it under\n\
 the terms of the GNU Affero General Public License as published by the Free\n\
 Software Foundation, version 3 of the license.\n\
\n\
 This program is distributed in the hope that it will be useful, but WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU Affero General Public License\n\
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or\n\
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA\n\
 02139, USA.\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
 behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
 to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
 of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the Government is acquiring only the license rights\n\
 granted herein (the license rights customarily provided to non-Government\n\
 users).  If the Software is supplied to any unit or agency of the Government\n\
 other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
 Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
 Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).\n\
\n\
 -----------------------------------------------------------------------------\n\
\n\
 Commercial licensing and support of this software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
 *****************************************************************************/\n\
\n\
/*\n\
 * GENERATED BY strconf.awk (" date("") ") FROM " values["inputs"] "\n\
 * EDITS TO THIS FILE WILL BE LOST: EDIT strconf.awk INSTEAD.\n\
 */\n\
\n\
static char const ident[] = \"pkgobject (\" PACKAGE_ENVR \" \" PACKAGE_DATE \")\";\n\
\n\
#include <linux/config.h>\n\
#include <linux/version.h>\n\
#include <linux/module.h>\n\
#include <linux/init.h>\n\
\n\
#include <sys/kmem.h>\n\
#include <sys/stream.h>\n\
#include <sys/strconf.h>\n\
#include <sys/ddi.h>\n\
\n\
#define _XX_DESCRIP	\"UNIX/SYSTEM V RELEASE 4 STREAMS FOR LINUX\"\n\
#define _XX_COPYRIGHT	\"Copyright (c) 2008-" year() " Monavacon Limited.  All Rights Reserved.\"\n\
#define _XX_DEVICE	\"LiS Binary Compatibility Wrapper for " pkgobject "\"\n\
#define _XX_CONTACT	\"Brian Bidulock <bidulock@openss7.org>\"\n\
#define _XX_LICENSE	\"Proprietary\"\n\
#define _XX_BANNER	_XX_DESCRIP	\"\\n\" \\\n\
			_XX_COPYRIGHT	\"\\n\" \\\n\
			_XX_DEVICE	\"\\n\" \\\n\
			_XX_CONTACT\n\
\n\
MODULE_AUTHOR(_XX_CONTACT);\n\
MODULE_DESCRIPTION(_XX_DESCRIP);\n\
MODULE_SUPPORTED_DEVICE(_XX_DEVICE);\n\
#ifdef MODULE_LICENSE\n\
MODULE_LICENSE(_XX_LICENSE);\n\
#endif				/* MODULE_LICENSE */\n\
#if defined MODULE_ALIAS\n\
MODULE_ALIAS(\"streams-" object "\");\
" > file
    if (object in driver_objects) {
	name = driver_objects[object]
	prefix = driver_prefix[name]
	first = driver_major[name]
	last = first + driver_nmajors[name]
	for (count = first; count < last; count++)
	    print "MODULE_ALIAS(\"char-major-" count "\");" > file
    } else {
	name = module_objects[object]
	prefix = module_prefix[name]
    }
    print "\
#endif\n\
\n\
extern struct streamtab " prefix "info;\n\
" > file
    if (name in initialized) {
	initfunc = "\n\t" prefix "init(); /* only if we have an initialize function */"
	print "extern int  " prefix "init(void); /* only if we have the function */" > file
    }
    if (name in terminated) {
	exitfunc = "\n\t" prefix "exit(); /* only if we have a terminate function */"
	print "extern void " prefix "exit(void); /* only if we have the function */" > file
    }
    # do some work
    if (object in driver_objects) {
	print "static int _xx_majors[" driver_nmajors[name] "] = {" > file
	for (count = first; count < last; count++)
	    print "\t" count "," > file
	print "};" > file
	print "\n\
static void\n\
_xx_del(void)\n\
{\n\
	int err, mindex;\n\
\n\
	for (mindex = " driver_nmajors[name] " - 1; mindex >= 0; mindex--) {\n\
		if (_xx_majors[mindex]) {\n\
			if ((err = lis_unregister_strdev(_xx_majors[mindex])))\n\
				cmn_err(CE_PANIC, \"" name ": cannot unregister major %d\", _xx_majors[mindex]);\n\
			_xx_majors[mindex] = 0;\n\
		}\n\
	}\n\
}\n\
\n\
static int\n\
_xx_add(void)\n\
{\n\
	int err, mindex = 0;\n\
\n\
	cmn_err(CE_NOTE, _XX_BANNER);	/* console splash */\n\
	for (mindex = 0; mindex < " driver_nmajors[name] "; mindex++) {\n\
		if ((err = lis_register_strdev(_xx_majors[mindex], &" prefix "info, " driver_nminors[name] ", \"" name "\")) < 0) {\n\
			if (mindex) {\n\
				cmn_err(CE_WARN, \"" name ": could not register major %d\", _xx_majors[mindex]);\n\
				continue;\n\
			} else {\n\
				cmn_err(CE_WARN, \"" name ": could not register driver, err = %d\", err);\n\
				return (err);\n\
			}\n\
		}\n\
		if (_xx_majors[mindex] == 0)\n\
			_xx_majors[mindex] = err;\n\
		if ((err = lis_register_driver_qlock_option(_xx_majors[mindex], " qlock ")) < 0)\n\
			cmn_err(CE_WARN, \"" name ": could not register qlock option, err = %d\", err);\n\
	}\n\
	return (0);\n\
}\
" > file
    } else {
	print "\n\
static void\n\
_xx_del(void)\n\
{\n\
	int err;\n\
\n\
	if ((err = lis_unregister_strmod(&" prefix "info)) < 0)\n\
		cmn_err(CE_PANIC, \"" name ": cannot unregister module, err = %d\", err);\n\
	return;\n\
}\n\
\n\
#ifndef LIS_NULL_MID\n\
#define LIS_NULL_MID 0\n\
#endif\n\
\n\
static int\n\
_xx_add(void)\n\
{\n\
	int err, modid;\n\
\n\
	cmn_err(CE_NOTE, _XX_BANNER);	/* console splash */\n\
	if ((modid = lis_register_strmod(&" prefix "info, \"" name "\")) == LIS_NULL_MID) {\n\
		cmn_err(CE_WARN, \"" name ": could not register module\");\n\
		return (-ENXIO);\n\
	}\n\
	if ((err = lis_register_module_qlock_option(modid, " qlock ")) < 0)\n\
		cmn_err(CE_WARN, \"" name ": could not register qlock option, err = %d\", err);\n\
	return (0);\n\
}\n\
" > file
    }
    # do some more work
    print "\n\
static void __exit\n\
_xx_exit(void)\n\
{" exitfunc "\n\
	_xx_del();\n\
}\n\
\n\
static int __init\n\
_xx_init(void)\n\
{\n\
	int err;\n\
\n\
	if ((err = _xx_add())) {\n\
		_xx_del();\n\
		return (err);\n\
	}" initfunc "\n\
	return (0);\n\
}\n\
\n\
module_init(_xx_init);\n\
module_exit(_xx_exit);\
" > file
    close(file)
    file_compare(file)
}
function write_packagedir(directory) {
    print_debug(1,"w: packagedir, directory = `" directory "'")
}
BEGIN {
    LINT = "yes"
    me = "strconf.awk"
    count_errs = 0; count_warn = 0; errors = ""
    if (!("TERM" in ENVIRON)) ENVIRON["TERM"] = "dumb"
    if (ENVIRON["TERM"] == "dumb" || system("test -t 1 -a -t 2") != 0) {
	stdout = "/dev/stdout"
	stderr = "/dev/stderr"
	cr = ""; lf = "\n"
	blk = ""; hblk = ""
	red = ""; hred = ""
	grn = ""; hgrn = ""
	org = ""; horg = ""
	blu = ""; hblu = ""
	mag = ""; hmag = ""
	cyn = ""; hcyn = ""
	std = ""; prog = "no"
    } else {
	stdout = "/dev/stdout"
	stderr = "/dev/stderr"
	cr = "\r"; lf = ""
	blk = "\033[0;30m"; hblk = "\033[1;30m"
	red = "\033[0;31m"; hred = "\033[1;31m"
	grn = "\033[0;32m"; hgrn = "\033[1;32m"
	org = "\033[0;33m"; horg = "\033[1;33m"
	blu = "\033[0;34m"; hblu = "\033[1;34m"
	mag = "\033[0;35m"; hmag = "\033[1;35m"
	cyn = "\033[0;36m"; hcyn = "\033[1;36m"
	std = "\033[m"; prog = "yes"
    }
    longopts["inputs"       ] = "I:" ; environs["inputs"       ] = "STRCONF_INPUT"	; defaults["inputs"       ] = "Config.master"	; descrips["inputs"       ] = "input configuration files (space separated list)"		; longargs["inputs"      ] = "INPUT ..."
    longopts["basemajor"    ] = "b:" ; environs["basemajor"    ] = "STRCONF_MAJBASE"	; defaults["basemajor"    ] = 231		; descrips["basemajor"    ] = "major number to act as base for STREAMS drivers and devices"
    longopts["basemodid"    ] = "i:" ; environs["basemodid"    ] = "STRCONF_MIDBASE"	; defaults["basemodid"    ] = 1			; descrips["basemodid"    ] = "module id number to act as base for STREAMS modules"
    longopts["minorbits"    ] = "B:" ; environs["minorbits"    ] = "STRCONF_MINORSZ"	; defaults["minorbits"    ] = 8			; descrips["minorbits"    ] = "number of bits in a minor device number"
    longopts["hconfig"      ] = "h::"; environs["hconfig"      ] = "STRCONF_CONFIG"	; defaults["hconfig"      ] = "config.h"	; descrips["hconfig"      ] = "full path and filename of the STREAMS configuration header file"
    longopts["modconf"      ] = "o::"; environs["modconf"      ] = "STRCONF_MODCONF"	; defaults["modconf"      ] = "modconf.inc"	; descrips["modconf"      ] = "full path and filename of the module configuration include file"
    longopts["makenodes"    ] = "m::"; environs["makenodes"    ] = "STRCONF_MKNODES"	; defaults["makenodes"    ] = "makenodes.c"	; descrips["makenodes"    ] = "full path and filename of the makenodes file"
    longopts["mkdevices"    ] = "M::"; environs["mkdevices"    ] = "STRCONF_DEVICES"	; defaults["mkdevices"    ] = "mkdevices"	; descrips["mkdevices"    ] = "full path and filename of the mkdevices file"
    longopts["permission"   ] = "p:" ;							  defaults["permission"   ] = "0666"		; descrips["permission"   ] = "permissions to assign to created files"
    longopts["driverconf"   ] = "l::"; environs["driverconf"   ] = "STRCONF_DRVCONF"	; defaults["driverconf"   ] = "drvrconf.mk"	; descrips["driverconf"   ] = "full path and filename of the driver configuration makefile"
    longopts["confmodules"  ] = "L::"; environs["confmodules"  ] = "STRCONF_CONFMOD"	; defaults["confmodules"  ] = "conf.modules"	; descrips["confmodules"  ] = "full path and filename of the modules configuration file"
    longopts["functionname" ] = "r:" ;							  defaults["functionname" ] = "main"		; descrips["functionname" ] = "function name of the function in makenodes"
    longopts["strmknods"    ] = "s::"; environs["strmknods"    ] = "STRCONF_MAKEDEV"	; defaults["strmknods"    ] = "makedev.lst"	; descrips["strmknods"    ] = "full path and filename of the makedevices script"
    longopts["strsetup"     ] = "S::"; environs["strsetup"     ] = "STRCONF_STSETUP"	; defaults["strsetup"     ] = "strsetup.conf"	; descrips["strsetup"     ] = "full path and filename of the strsetup configuration file"
    longopts["strload"      ] = "O::"; environs["strload"      ] = "STRCONF_STRLOAD"	; defaults["strload"      ] = "strload.conf"	; descrips["strload"      ] = "full path and filename of the strload configuration file"
    longopts["package"      ] = "k::"; environs["package"      ] = "STRCONF_PACKAGE"	; defaults["package"      ] = "LfS"		; descrips["package"      ] = "name of STREAMS package: LiS of LfS"
    longopts["pkgobject"    ] = "g::";							  defaults["pkgobject"    ] = "clone.o"		; descrips["pkgobject"    ] = "full path and filename of object file to package"
    longopts["packagedir"   ] = "d::"; environs["packagedir"   ] = "STRCONF_BPKGDIR"	;						  descrips["packagedir"   ] = "full path or vpath to binary package directory"
    longopts["pkgrules"     ] = "R::"; environs["pkgrules"     ] = "STRCONF_PKGRULE"	;						  descrips["pkgrules"     ] = "full path and filename of the pkgrules make rules file"
    longopts["exit-on-error"] = "e"  ;							  defaults["exit-on-error"] = 1			; descrips["exit-on-error"] = "exit with error status on program errors"
    longopts["dryrun"       ] = "n"  ;							  defaults["dryrun"       ] = 0			; descrips["dryrun"       ] = "don't perform actions, just check them"
    longopts["dry-run"      ] = "n"  ;							  defaults["dry-run"      ] = 0			; descrips["dry-run"      ] = "don't perform actions, just check them"
    longopts["quiet"        ] = "q"  ;							  defaults["quiet"        ] = 0			; descrips["quiet"        ] = "suppress normal output"
    longopts["silent"       ] = "q"  ;							  defaults["silent"       ] = 0			; descrips["silent"       ] = "suppress normal output"
    longopts["debug"        ] = "D::"; environs["debug"        ] = "STRCONF_DEBUG"	; defaults["debug"        ] = 0			; descrips["debug"        ] = "increase or set debug level DEBUG"
    longopts["verbose"      ] = "v::"; environs["verbose"      ] = "STRCONF_VERBOSE"	; defaults["verbose"      ] = 0			; descrips["verbose"      ] = "increase or set verbosity level VERBOSITY"
    longopts["help"         ] = "H!"  ;													  descrips["help"         ] = "display this usage information and exit"
    longopts["version"      ] = "V!"  ;													  descrips["version"      ] = "display script version and exit"
    longopts["copying"      ] = "C!"  ;													  descrips["copying"      ] = "display copying permissions and exit"
    # set mandatory defaults
    values["basemajor"    ] = defaults["basemajor"    ]
    values["basemodid"    ] = defaults["basemodid"    ]
    values["minorbits"    ] = defaults["minorbits"    ]
    values["permission"   ] = defaults["permission"   ]
    values["package"      ] = defaults["package"      ]
    values["exit-on-error"] = defaults["exit-on-error"]
    values["dryrun"       ] = defaults["dryrun"       ]
    values["dry-run"      ] = defaults["dry-run"      ]
    values["quiet"        ] = defaults["quiet"        ]
    values["silent"       ] = defaults["silent"       ]
    values["debug"        ] = defaults["debug"        ]
    values["verbose"      ] = defaults["verbose"      ]
    optstring = "I:b:i:B:h::o::m::M::p:l::L::r:s::S::O::k::g::d::R::enqD::v::HVC"
    optind = 0
    #opts = ""; for (i=1;i<ARGC;i++) { if (i == 1) { opts = ARGV[i] } else { opts = opts " " ARGV[i] } }
    #print me ": D: o: command line: " opts > stderr; written[stderr] = 1
    command = ""
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	#if (c != -1) { print me ": D: o: option -" c ", longopt --" option ", optset = " optset ", optarg = " optarg > stderr; written[stderr] = 1 }
	if (c == -1) break
	else if (c ~ /[IbiBhomMplLrsSOkgdR]/)		{ values[option] = optarg }
	else if (c ~ /[enq]/)				{ values[option] = optset }
	else if (c~/[Dv]/) { if (optarg != "")		{ values[option] = optarg } else { if (optset) { values[option]++ } else { values[option] = optset } } }
	else if (c~/[HVC]/)	{ command = option }
	else			{ usage(  stderr); exit 2 }
    }
    if (values["quiet"  ] == defaults["quiet"  ] &&
	values["debug"  ] == defaults["debug"  ] &&
	values["verbose"] == defaults["verbose"]) {
	if ("V" in ENVIRON) {
	    if (ENVIRON["V"] == "0") {
		values["quiet"  ] = 1
		values["debug"  ] = 0
		values["verbose"] = 0
	    } else
	    if (ENVIRON["V"] == "1") {
		values["quiet"  ] = 0
		values["debug"  ] = 0
		values["verbose"] = 2
	    }
	}
    }
    if (values["verbose"] >=3 && values["debug"] == defaults["debug"])
	values["debug"] = values["verbose"] - 2
    while (optind < ARGC) {
	if ("inputs" in values)
	{ values["inputs"] = values["inputs"] " " ARGV[optind] } else
	{ values["inputs"] = ARGV[optind] }
	    optind++
    }
    for (i=1;ARGC>i;i++) { delete ARGV[i] }
    for (value in values) {
	if (!values[value] && (value in environs) && (environs[value] in ENVIRON) && ENVIRON[environs[value]]) {
	    print_debug(1,"o: assigning value for " value " from environment " environs[value])
	    values[value] = ENVIRON[environs[value]]
	}
	if (!values[value] && (value in defaults) && defaults[value]) {
	    print_debug(1,"o: assigning value for " value " from default " defaults[value])
	    values[value] = defaults[value]
	}
    }
    for (value in values) {
	print_debug(1, "o: \"" value "\" = " values[value])
    }
    if (command == "help"          ) { help(   stdout); exit 0 }
    if (command == "version"       ) { version(stdout); exit 0 }
    if (command == "copying"       ) { copying(stdout); exit 0 }
    if (!values["inputs"] || values["inputs"] == "-") {
	print_debug(1,"o: no inputs, using " stdin)
	values["inputs"] = stdin
    }
    split(values["inputs"], inputs)
    read_inputs(inputs)
    if ("driverconf" in values)
	write_driverconf(values["driverconf"])
    if ("hconfig" in values)
	write_hconfig(values["hconfig"])
    if ("modconf" in values)
	write_modconf(values["modconf"])
    if ("makenodes" in values)
	write_makenodes(values["makenodes"])
    if ("strmknods" in values)
	write_strmknods(values["strmknods"])
    if ("strsetup" in values)
	write_strsetup(values["strsetup"])
    if ("strload" in values)
	write_strload(values["strload"])
    if ("mkdevices" in values)
	write_mkdevices(values["mkdevices"])
    if ("confmodules" in values)
	write_confmodules(values["confmodules"])
    if ("pkgobject" in values)
	write_pkgobject(values["pkgobject"])
    if ("packagedir" in values)
	write_packagedir(values["packagedir"])
    if (count_errs && ("exit-on-error" in values) && (values["exit-on-error"] != 0))
	exit 1
    exit 0
}
END {
    if (errors) { print errors > stderr; written[stderr] = 1 }
    if (count_errs) print_emore("errs = " count_errs)
    if (count_warn) print_wmore("warn = " count_warn)
    for (file in written)
	close(file)
}

# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
