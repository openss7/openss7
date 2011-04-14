#!/usr/bin/awk -f
# =============================================================================
#
# @(#) $RCSfile: modpost.awk,v $ $Name:  $($Revision: 1.1.2.13 $) $Date: 2011-04-12 06:33:27 $
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
# Last Modified $Date: 2011-04-12 06:33:27 $ by $Author: brian $
#
# =============================================================================

#
# This is an awk program to generate the modpost.cache cache file.  The inputs
# consists of the sysmap file, the modules directory, specific modpost input
# files, and a sysver file.

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
  $Id: modpost.awk,v 1.1.2.13 2011-04-12 06:33:27 brian Exp $\n\
Usage:\n\
  [awk -f ]" me " -- [options] [MODULE ...]\n\
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
  " longargs["modules"] "\n\
      " descrips["modules"] "\
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
$Id: modpost.awk,v 1.1.2.13 2011-04-12 06:33:27 brian Exp $\n\
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
$Id: modpost.awk,v 1.1.2.13 2011-04-12 06:33:27 brian Exp $\n\
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
			if (longopts[option] && substr(longopts[option], 1, 1) == optval)
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
		    if (longopts[option] && substr(longopts[option], 1, 1) == optval)
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
function missing(fnlp, expected)
{
    print_error(fnlp " definition missing " (expected - NF) " fields")
}
function garbage(fnlp, maximum)
{
    print_warns(fnlp " definition with " (NF - maximum) " garbage fields")
}
function unrecog(fnlp, line)
{
    print_error(fnlp " unrecognized line: " line)
}
function show_junk(line, position,	number,fields,junk)
{
    number = split(line, fields)
    junk = ""
    for (i=position;i<=number;i++)
	junk = junk " " fields[i]
    print_warns("got junk: " junk)
}
function cat_command(files,    goodfiles,command,filenames,i,n,filename)
{
    print_debug(2,"p: cat files, files =\"" files "\"")
    n = split(files,filenames); goodfiles = ""; command = ""
    for (i=1;i<=n;i++) {
	filename = filenames[i]
	if (system("test -r \"" filename "\"") == 0)
	    goodfiles = goodfiles " " filename
	else
	    print_warns("p: cat files, failed file =\"" filename "\"")
    }
    if (goodfiles) {
	command = "cat" goodfiles
	if (goodfiles ~ /\.gz\>/)
	    command = "zcat" goodfiles
	if (goodfiles ~ /\.bz2\>/)
	    command = "bzcat" goodfiles
	if (goodfiles ~ /\.xz\>/)
	    command = "xzcat" goodfiles
    } else {
	print_warns("p: cat files, files not found = \"" files "\"")
    }
    return command
}
function set_symmap(sym, type, addr, src, from)
{
    if (!(sym in mapsyms)) {
	mapsyms[sym] = 1
	cache_dirty = 1
	count_maps++
    }
    if (type) {
	if (sym in maptypes) {
	    if (maptypes[sym] != type) {
		#print_warns("s: " from ", type changed for " sym)
		#if (sym in mapsrcs)
		#print_wmore("s: " from ", srcs = " mapsrcs[sym])
		#print_wmore("s: " from ", old = " maptypes[sym])
		#print_wmore("s: " from ", new = " type)
		type = maptypes[sym] "," type
	    }
	} else cache_dirty = 1
	maptypes[sym] = type
    } else if (sym in maptypes) { type = maptypes[sym] }
    if (addr) {
	if (sym in mapaddrs) {
	    if (mapaddrs[sym] != type) {
		#print_warns("s: " from ", addr changed for " sym)
		#if (sym in mapsrcs)
		#print_wmore("s: " from ", srcs = " mapsrcs[sym])
		#print_wmore("s: " from ", old = 0x" mapaddrs[sym])
		#print_wmore("s: " from ", new = 0x" addr)
		addr = mapaddrs[sym] "," addr
	    }
	} else cache_dirty = 1
	mapaddrs[sym] = addr
    } else if (sym in mapaddrs) { addr = mapaddrs[sym] }
    if (src) {
	if (sym in mapsrcs) {
	    if ("," mapsrcs[sym] "," !~ "," src ",") {
		src = mapsrcs[sym] "," src
		cache_dirty = 1
	    }
	} else cache_dirty = 1
    } else if (sym in mapsrcs) { src = mapsrcs[sym] }
}
function set_symbol(sym, mod, crc, expt, set, pos, own, src, from)
{
    if (!(sym in syms)) {
	syms[sym] = 1
	cache_dirty = 1
	count_syms++
    }
    if (mod) {
	if (sym in mods) {
	    if (mods[sym] != mod) {
		if (mod~/(vmlinux|built-in)/ && mods[sym]!~/(vmlinux|built-in)/) {
		    print_error("s: " from ", kernel conflict for sym " sym)
		    print_emore("s: " from ", kernel = " mod)
		    print_emore("s: " from ", module = " mods[sym])
		    cache_dirty = 1
		} else 
		if (mod!~/(vmlinux|built-in)/ && mods[sym]~/(vmlinux|built-in)/) {
		    print_error("s: " from ", kernel conflict for sym " sym)
		    print_emore("s: " from ", kernel = " mods[sym])
		    print_emore("s: " from ", module = " mod)
		    mod = mods[sym]
		} else
		if (mod!~/(vmlinux|built-in)/ && mods[sym]!~/(vmlinux|built-in)/) {
		    base1 = mod;       sub(/.*\//, "", base1)
		    base2 = mods[sym]; sub(/.*\//, "", base2)
		    if (base1 != base2) {
			print_warns("s: " from ", conflict for sym " sym)
			print_wmore("s: " from ", old = " mods[sym])
			print_wmore("s: " from ", new = " mod)
		    }
		    cache_dirty = 1
		} else
		    cache_dirty = 1
	    }
	} else cache_dirty = 1
	if (mod~/built-in/)
	    mod_syms["vmlinux",sym] = 1
	else
	    mod_syms[mod,sym] = 1
	mod_mods[mod] = 1
	mods[sym] = mod
    }
    if (crc) {
	sub(/0x00000000/, "0x", crc)
	if (sym in crcs) {
	    if (crcs[sym] != crc) {
		print_warns("s: " from ", crc changed for " mod ":" sym)
		print_wmore("s: " from ", old = " crcs[sym])
		print_wmore("s: " from ", new = " crc)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	crcs[sym] = crc
    } else if (sym in crcs) { crc = crcs[sym] }
    if (expt) {
	if (sym in exps) {
	    if (exps[sym] != expt) {
		print_warns("s: " from ", exp changed for " mod ":" sym)
		print_wmore("s: " from ", old = " exps[sym])
		print_wmore("s: " from ", new = " expt)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	exps[sym] = expt
    } else if (sym in exps) { expt = exps[sym] }
    if (set) {
	if (sym in sets) {
	    if ("," set "," !~ "," sets[sym] ",") {
		set = sets[sym] "," set
		pos = posn[sym] "," pos
		cache_dirty = 1
	    } else {
		# TODO: check for a position change
	    }
	} else cache_dirty = 1
	sets[sym] = set
	posn[sym] = pos
    } else if (sym in sets) { set = sets[sym]; pos = posn[sym] }
    if (!own && mod~/(vmlinux|built-in)/) { own = "kernel" }
    if (own) {
	if (sym in ownr) {
	    if (ownr[sym] != own) {
		if (ownr[sym] != "kernel" || own != "kabi") {
		    print_warns("s: " from ", own changed for " mod ":" sym)
		    print_wmore("s: " from ", old = " ownr[sym])
		    print_wmore("s: " from ", new = " own)
		}
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	ownr[sym] = own
    } else if (sym in ownr) { own = ownr[sym] }
    if (src) {
	if (sym in srcs) {
	    if ("," srcs[sym] "," !~ "," src ",") {
		src = srcs[sym] "," src
		cache_dirty = 1
	    }
	} else cache_dirty = 1
    } else if (sym in srcs) { src = srcs[sym] }
}
function set_symset(set, hash, numb, ssym, scrc, own, src, from,	n,i,symbols)
{
    if (set in setsets)
	print_warns("s: " from ", set " set " already exists!")
    else {
	setsets[set] = 1
	count_sets++
	cache_dirty = 1
    }
    if (own == "kabi" && ssym) {
	n = split(ssym,symbols,/,/)
	for(i=1;i<=n;i++) kabi[symbols[i]] = 1
    }
    if (hash) {
	if (set in sethash) {
	    if (sethash[set] != hash) {
		print_warns("s: " from ", hash changed for " set)
		print_wmore("s: " from ", old = " sethash[set])
		print_wmore("s: " from ", new = " hash)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	sethash[set] = hash
    }
    if (numb) {
	if (set in setnumb) {
	    if (setnumb[set] != numb) {
		print_warns("s: " from ", numb changed for " set)
		print_wmore("s: " from ", old = " setnumb[set])
		print_wmore("s: " from ", new = " numb)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	setnumb[set] = numb
    }
    if (ssym) {
	if (set in setsyms) {
	    if (setsyms[set] != ssym) {
		print_warns("s: " from ", syms changed for " set)
		print_wmore("s: " from ", old = " setsyms[set])
		print_wmore("s: " from ", new = " ssym)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	setsyms[set] = ssym
    }
    if (scrc) {
	if (set in setcrcs) {
	    if (setcrcs[set] != scrc) {
		print_warns("s: " from ", crcs changed for " set)
		print_wmore("s: " from ", old = " setcrcs[set])
		print_wmore("s: " from ", new = " scrc)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	setcrcs[set] = scrc
    }
    if (own) {
	if (set in setownr) {
	    if (setownr[set] != own) {
		print_warns("s: " from ", ownr changed for " set)
		print_wmore("s: " from ", old = " setownr[set])
		print_wmore("s: " from ", new = " own)
		cache_dirty = 1
	    }
	} else cache_dirty = 1
	setownr[set] = own
    }
    if (src) {
	if (set in setsrcs && "," setsrcs[set] "," !~ "," src ",")
	    src = setsrcs[set] "," src
	setsrcs[set] = src
	cache_dirty = 1
    }
}
#
# Read in an objdump -t listing.  This must be from objdump -t run on a kernel
# object.  This will add EXPORT_SYMBOL or EXPORT_SYMBOL_GPL for use in the
# Module.symvers file.
#
function read_modobject(command, dir, own, src,	    result,files,progress,HEADER,mod,flags,val,sec,ofs,sym,count_syms,count_unds,count_weak)
{
    print_vinfo(1,"r: ko object, subdirectory = \"" dir "\"")
    files = 0
    progress = 0
    HEADER = 1
    mod = ""
    count_syms = 0; count_unds = 0; count_weak = 0
    while ((result = (command | getline)) == 1) {
	if (HEADER) {
	    if (/^$/)
		continue
	    if (/:.*file format/) {
		sub(/:.*$/, "")
		print_debug(1,"r: ko object, module = " $0)
		sub(/(\.mod)?\.(k)?o(\.gz)?$/, "")
		if (own == values["pkgdirectory"]) {
		    sub(/.*\//, "")
		    $0 = values["pkgdirectory"] "/" $0
		} else {
		    if ("moddir" in values)
			sub(values["moddir"] "/", "")
		    else if ("moddir" in defaults)
			sub(defaults["moddir"] "/", "")
		    sub(/kernel\//, "")
		}
		mod = $0
		if (own == values["pkgdirectory"]) {
		    if (("vmlinux","struct_module") in mod_syms) {
			mod_unds[mod,"struct_module"] = 1
		    } else if (("vmlinux","module_layout") in mod_syms) {
			mod_unds[mod,"module_layout"] = 1
		    } else {
			mod_unds[mod,"struct_module"] = 1
		    }
		}
		continue
	    }
	    if (/SYMBOL TABLE:/) {
		count_syms = 0; count_unds = 0; count_weak = 0
		mod_mods[mod] = 1
		HEADER = 0
		if (progress >= 200) {
		    print_dmore(1,"r: ko object, " files " kernel modules...")
		    progress = 0
		}
		files++
		progress++
		continue
	    }
	}
	if (/^$/ && !HEADER) {
	    HEADER = 1
	    if (count_syms || count_unds || count_weak) {
		print_debug(1,"r: ko object, " mod)
		print_debug(1,"r: ko object, syms " count_syms ", unds " count_unds ", weak " count_weak)
	    }
	    continue
	}
	# ok we have a listing line
	# The flags are just after the first field and consist of 7 characters
	# The last three fields always contain the same information
	if (NF<4)
	    continue
	flags = substr($0, length($1)+2, 7); val = $1; sec = $(NF-2); ofs = $(NF-1); sym = $NF
	if (substr(flags,1,1) == "l") {
	    if (substr(flags,7,1) == "O") {
		if (sec == ".modinfo") {
		    if (sym ~ /^_?__mod_version[0-9]*$/)
			mod_vers[mod] = 1
		    continue
		}
		if (sub(/^_?__ksymtab/,"",sec)) {
		    if (sub(/^_?__ksymtab_/,"",sym)) {
			print_debug(3,"r: ko object, symbol: " sym)
			set_symbol(sym, mod, "", "EXPORT_SYMBOL" toupper(sec), "", 0, own, "", "ko object")
			count_syms++
		    }
		    continue
		}
	    }
	    continue
	}
	if (sec == "*ABS*") {
	    if (substr(flags,1,1) == "g") {
		if (sub(/^_?__crc_/,"",sym)) {
		    print_debug(3,"r: ko object, symbol: " sym)
		    set_symbol(sym, mod, "0x" val, "", "", 0, own, src, "ko object")
		    count_syms++
		    continue
		}
	    }
	    mod_abss[mod,sym] = 1
	}
	if (own != values["pkgdirectory"])
	    continue
	if (sec == "*UND*") {
	    if (sym ~ /^_?__this_module$/) {
		if (mod in mod_this)
		    delete mod_this[mod]
		continue
	    }
	    if (substr(flags,2,1) == " " && !((mod,sym) in mod_unds)) {
		print_debug(3,"r: ko object, undefined symbol: " sym)
		mod_unds[mod,sym] = 1
		count_unds++
	    }
	    if (substr(flags,2,1) == "w" && !((mod,sym) in mod_weak)) {
		print_debug(3,"r: ko object, weakundef symbol: " sym)
		mod_weak[mod,sym] = 1
		count_weak++
	    }
	    continue
	}
	if (sym ~ /^_?init_module$/) {
	    mod_init[mod] = 1
	    print_debug(2,"r: ko object, module " mod " has init_module")
	    continue
	}
	if (sym ~ /^_?cleanup_module$/) {
	    mod_exit[mod] = 1
	    print_debug(2,"r: ko object, module " mod " has cleanup_module")
	    continue
	}
	if (sym ~ /^_?__this_module$/) {
	    mod_this[mod] = 1
	    print_debug(2,"r: ko object, module " mod " has this_module")
	    continue
	}
	if (sym ~ /^_?__mod_version[0-9]*$/) {
	    mod_vers[mod] = 1
	    print_debug(2,"r: ko object, module " mod " has mod_version")
	    continue
	}
    }
    if (result != -1) close(command)
    print_vinfo(1,"r: ko object, " files " kernel modules   ")
}
#
# Read in any nm -Bs listing.  This can be a System.map file, a Modules.map
# file or nm -Bz run an a kernel object.
#
function read_systemmap(command, mod, own, src,		result,val,flag,sym,count_syms,count_unds,count_weak)
{
    print_vinfo(1,"r: systemmap, command = \"" command "\"")
    count_syms = 0; count_unds = 0; count_weak = 0
    sub(/^[[]/, "", mod)
    sub(/[]]$/, "", mod)
    mod_mods[mod] = 1
    while ((result = (command | getline)) == 1) {
	if (NF > 4) {
	    show_junk($0, 5)
	    continue
	}
	if (NF < 2) {
	    continue
	}
	sym = $NF; flag = $(NF-1)
	if (NF < 3) { val = "" } else { val = $1 }

	set_symmap(sym, flag, val, "systemmap", "systemmap")
	if (flag ~ /[wbdgrstv]/) {
	    if (sym ~ /^_?_mod_version[0-9]*$/) {
		mod_vers[mod] = 1
		continue
	    }
	    if (sub(/^_?__ksymtab_/,"",sym)) {
		print_debug(3,"r: systemmap, symbol: " sym)
		set_symbol(sym, mod, "", "", "", 0, own, "", "systemmap")
		count_syms++
		continue
	    }
	}
	if (flag ~ /[Aa]/) {
	    if (sub(/^_?__crc_/,"",sym)) {
		print_debug(3,"r: systemmap, symbol: " sym)
		set_symbol(sym, mod, "0x" val, "", "", 0, own, src, "systemmap")
		count_syms++
		continue
	    }
	    mod_abss[mod,sym] = 1
	}
	if (own != values["pkgdirectory"])
	    continue
	if (flag ~ /[Uwv]/) {
	    if (sym ~ /^_?__this_module$/) {
		if (mod in mod_this)
		    delete mod_this[mod]
		continue
	    }
	    if (flag == "U" && !((mod,$2) in mod_unds)) {
		print_debug(3,"r: systemmap, undefined symbol: " sym)
		mod_unds[mod,$2] = 1
		count_unds++
	    }
	    if (flag ~ /[vw]/ && !((mod,$2) in mod_weak)) {
		print_debug(3,"r: systemmap, weakundef symbol: " sym)
		mod_weak[mod,$2] = 1
		count_weak++
	    }
	    continue
	}
	if (sym ~ /^_?init_module$/) {
	    mod_init[mod] = 1
	    print_debug(2,"r: systemmap, module " mod " has init_module")
	    continue
	}
	if (sym ~ /^_?cleanup_module$/) {
	    mod_exit[mod] = 1
	    print_debug(2,"r: systemmap, module " mod " has cleanup_module")
	    continue
	}
	if (sym ~ /^_?__this_module$/) {
	    mod_this[mod] = 1
	    print_debug(2,"r: systemmap, module " mod " has this_module")
	    continue
	}
	if (sym ~ /^_?__mod_version[0-9]*$/) {
	    mod_vers[mod] = 1
	    print_debug(2,"r: systemmap, module " mod " has mod_version")
	    continue
	}
    }
    if (result != -1) close(command)
    print_vinfo(1,"r: systemmap, syms " count_syms " unds " count_unds ", weak " count_weak)
}
# read modpost cache file
function read_cachefile(file,    i,line,n,fields)
{
    if ("PWD" in ENVIRON) sub("^" ENVIRON["PWD"] "/", "", file)
    print_vinfo(1,"r: cachefile, file = \"" file "\"")
    count_syms = 0; count_sets = 0
    if (system("test -r " file) == 0) {
	while ((getline line < file) == 1) {
	    if (line~/:/) {
		# new style cache file format
		n = split(line,fields,/:/)
		if (fields[1] == "format") {
		    values["format"] = fields[2]
		} else
		if (fields[1] == "kabi") {
		    values["kabi"] = fields[2]
		} else
		if (fields[1] == "sym") {
		    if (n<3) { print_error("syntax error in cachefile: line = \"" line "\""); continue }
		    for (i=9;i>n;i--) { fields[i] = "" }; n = 9
		    set_symbol(fields[2],fields[3],fields[4],fields[5],fields[6],fields[7],fields[8],fields[9],"cachefile")
		} else
		if (fields[1] == "set") {
		    if (n<3) { print_error("syntax error in cachefile: line = \"" line "\""); continue }
		    for (i=8;i>n;i--) { fields[i] = "" }; n = 8
		    set_symset(fields[2],fields[3],fields[4],fields[5],fields[6],fields[7],fields[8],"cachefile")
		} else
		if (fields[1] == "map") {
		    if (n<4) { print_error("syntax error in cachefile: line = \"" line "\""); continue }
		    for (i=5;i>n;i--) { fields[i] = "" }; n = 5
		    set_symmap(fields[2],fields[3],fields[4],fields[5],"cachefile")
		}
	    } else
		print_error("r: cachefile, bad line = \"" line "\"")
	}
	close(file)
	print_vinfo(1,"r: cachefile, syms " count_syms ", sets " count_sets)
	return 0
    } else {
	print_vinfo(1,"r: cachefile, file not found")
	return -1
    }
}
function write_cachefile(file,	    sym,set,line,count_syms,count_sets,count_maps)
{
    count_syms = 0; count_sets = 0; count_maps = 0
    if ("PWD" in ENVIRON) sub("^" ENVIRON["PWD"] "/", "", file)
    print_vinfo(1,"w: cachefile, file = \"" file "\"")
    print "format:" values["format"] > file
    print "kabi:" values["kabi"] > file
    for (sym in syms) {
	line = "sym:" sym
	line = line ":"; if (sym in mods) { line = line mods[sym] }
	line = line ":"; if (sym in crcs) { line = line crcs[sym] }
	line = line ":"; if (sym in exps) { line = line exps[sym] }
	line = line ":"; if (sym in sets) { line = line sets[sym] }
	line = line ":"; if (sym in posn) { line = line posn[sym] }
	line = line ":"; if (sym in ownr) { line = line ownr[sym] }
	line = line ":"; if (sym in srcs) { line = line srcs[sym] }
	print line > file; count_syms++; written[file] = 1
    }
    for (set in setsets) {
	line = "set:" set
	line = line ":"; if (set in sethash) { line = line sethash[set] }
	line = line ":"; if (set in setnumb) { line = line setnumb[set] }
	line = line ":"; if (set in setsyms) { line = line setsyms[set] }
	line = line ":"; if (set in setcrcs) { line = line setcrcs[set] }
	line = line ":"; if (set in setownr) { line = line setownr[set] }
	line = line ":"; if (set in setsrcs) { line = line setsrcs[set] }
	print line > file; count_sets++; written[file] = 1
    }
    for (sym in mapsyms) {
	line = "map:" sym
	line = line ":"; if (sym in maptypes) { line = line maptypes[sym] }
	line = line ":"; if (sym in mapaddrs) { line = line mapaddrs[sym] }
	line = line ":"; if (sym in mapsrcs ) { line = line  mapsrcs[sym] }
	print line > file; count_maps++; written[file] = 1
    }
    if (file in written) {
	close(file)
	delete written[file]
    }
    print_vinfo(1,"w: cachefile, syms " count_syms ", sets " count_sets ", maps " count_maps)
    cache_dirty = 0
}
function read_dumpfiles_line(own, src)
{
    if (NF < 2) {
	print_error("oops NF = " NF)
	return
    }
    if (NF > 4 || (NF > 3 && $4 !~ /^EXPORT_.*SYMBOL/)) {
	show_junk($0, 4)
	return
    }
    if (NF < 3) $3 = "vmlinux"
    if (NF < 4) $4 = ""
    set_symbol($2, $3, $1, $4, "", 0, own, src, "dumpfiles")
}
function read_dumpfiles(command, own, src)
{
    print_vinfo(1,"r: dumpfiles, command = \"" command "\"")
    count_syms = 0; count_unds = 0
    while ((command | getline) == 1)
	read_dumpfiles_line(own, src)
    close(command)
    print_vinfo(1,"r: dumpfiles, syms " count_syms ", unds " count_unds)
}
function read_moduledir(directory, src,	    dir,command)
{
    print_vinfo(1,"r: moduledir, directory = \"" directory "\"")
    dir = "kernel"
    command = "find " directory "/" dir " -type f -name '*.ko' 2>/dev/null | xargs -r objdump -t"
    read_modobject(command, dir, "kernel", src)
    dir = values["pkgdirectory"]
    command = "find " directory "/" dir " -type f -name '*.ko' 2>/dev/null | xargs -r objdump -t"
    read_modobject(command, dir, values["pkgdirectory"], "pkgdirectory")
}
function read_mymodules(modules, src,    i,pair,ind,base,name,sym,fmt) {
    fmt = "r: mymodules, %-20s: %14s; %-30s"
    print_vinfo(1,"r: mymodules")
    for (i = 1; i in modules; i++)
	print modules[i] > "modvers.list"
    close("modvers.list")
    command = "cat modvers.list | xargs objdump -t "
    read_modobject(command, ".", values["pkgdirectory"], src)
    system("rm -f modvers.list")
    # double check
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]; base = name; gsub(/^.*\//, "", base)
	if (sym in mods) {
	    mod_deps[name,mods[sym]] = 1
	    print_debug(3,"r: mymodules, dependency: " base " depends on " mods[sym] " for symbol " sym)
	    if ((mods[sym],sym) in mod_used)
		mod_used[mods[sym],sym] = mod_used[mods[sym],sym] "," name
	    else
		mod_used[mods[sym],sym] = name
	}
	if (!(sym in syms)) { # symbol not exported
	    if (!(sym in mapsyms)) { # symbol not rippable
		print_error(sprintf(fmt, base, "unresolved", sym))
	    } else {
		if (values["rip-symbols"])
		    print_vinfo(1,sprintf(fmt, base, "unexported", sym))
		else
		    print_error(sprintf(fmt, base, "unexported", sym))
	    }
	    continue
	}
	if (!(sym in kabi) && values["kabi"]) {
	    if (ownr[sym] != values["pkgdirectory"])
		if (values["unsupported"])
		    print_vinfo(1,sprintf(fmt, base, "unsupportd", sym))
		else
		    print_error(sprintf(fmt, base, "unsupportd", sym))
	}
	if (!(sym in crcs)) {
	    print_error("r: mymodules, symbol " sym " defined in module " mods[sym] " has no version")
	    continue
	}
    }
    for (pair in mod_weak) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]; base = name; gsub(/^.*\//, "", base)
	if (sym in mods) {
	    mod_wdep[name,mods[sym]] = 1
	    if ((mods[sym],sym) in mod_used)
		mod_used[mods[sym],sym] = mod_used[mods[sym],sym] "," name
	    else
		mod_used[mods[sym],sym] = name
	}
	if (!(sym in syms)) { # symbol not exported
	    if (!(sym in mapsyms)) { # symbol not rippable
		print_warns(sprintf(fmt, base, "weak unres", sym))
	    } else {
		if (values["rip-weak"] || values["unres-weak"])
		    print_vinfo(1,sprintf(fmt, base, "weak unexp", sym))
		else
		    print_error(sprintf(fmt, base, "weak unexp", sym))
	    }
	    continue
	}
	if (!(sym in kabi) && values["kabi"]) {
	    if (ownr[sym] != values["pkgdirectory"]) {
		if (values["unres-weak"])
		    print_vinfo(1,sprintf(fmt, base, "weak unsup", sym))
		else
		    print_warns(sprintf(fmt, base, "weak unsup", sym))
	    }
	}
	if (!(sym in crcs)) {
	    print_warns(sprintf(fmt " in %-20s", base, "no version", sym, mods[sym]))
	    continue
	}
    }
    for (pair in mod_abss) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]; base = name; gsub(/^.*\//, "", base)
	if (!(sym in crcs))
	    print_warns(sprintf(fmt, base, "rip intern", sym))
	else
	    print_vinfo(1,sprintf(fmt, base, "rip export", sym))
    }
    for (pair in mod_weak) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]; base = name; gsub(/^.*\//, "", base)
	if (!(sym in crcs)) {
	    if (values["unres-weak"])
		print_vinfo(1,sprintf(fmt, base, "weak unexp", sym))
	    else
		print_warns(sprintf(fmt, base, "weak unexp", sym))
	} else {
	    print_vinfo(1,sprintf(fmt, base, "weak unsup", sym))
	}
    }
    for (pair in mod_syms) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]; base = name; gsub(/^.*\//, "", base)
	if (!(sym in ownr)) continue
	if (ownr[sym] != values["pkgdirectory"]) continue
	if (!((name,sym) in mod_used)) {
	    if ((name,sym) in mod_abss) {
		if (sym in crcs)
		    print_warns(sprintf(fmt, base, "ripe unuse", sym))
		else
		    print_warns(sprintf(fmt, base, "ripi unuse", sym))
	    } else
		    print_vinfo(1,sprintf(fmt, base, "norm unuse", sym))
	}
    }
    if (count_errs) {
	errors = ""
	exit 1
    }
}
# read and process symsets-kversion.tar.gz file
function read_symsets(file, own, src,	n,tar,fname,set,hash,pos,lineno,ssym,scrc,sep,list,fnl,fnlp)
{
    count_syms = 0; count_sets = 0
    print_vinfo(1,"r: syssymset, file = \"" file "\"")
    n = 0
    if (system("test -r " file) == 0) {
	tar = "tar -tzf " file
	while ((tar | getline fname)) {
	    print_debug(2,"r: syssymset, fname = \"" fname "\"")
	    if (fname~/\/$/) continue # skip directory
	    set  = fname; sub(/^.*\//,"",set)
	    hash = set;   sub(/^.*\./,"",hash)
	                  #sub(/\..*$/,"",set)
	    print_debug(3,"r: syssymset,     set = \"" set "\"")
	    print_debug(3,"r: syssymset,     hash = \"" hash "\"")
	    pos = 0; lineno = 0
	    ssym = ""; scrc = ""; sep = ""
	    list = "tar -xzOf " file " " fname
	    while ((list | getline)) {
		lineno++; fnl = file ":" lineno; fnlp = fnl ": "
		if (NF < 3) { missing(fnlp, 3); continue }
		if (NF > 4) { garbage(fnlp, 4); NF = 4 }
		if (NF < 4) { $4 = ""; NF = 4; values["format"] = "redhat" }
		if ($1~/^0x/) {
		    ssym = ssym sep $2
		    scrc = scrc sep $1
		    sep = ","
		    pos++
		    print_debug(3,"r: syssymset,     sym = \"" $2 "\"")
		    print_debug(3,"r: syssymset,         crc = \"" $1 "\"")
		    print_debug(3,"r: syssymset,         mod = \"" $3 "\"")
		    print_debug(4,"r: syssymset,         exp = \"" $4 "\"")
		    print_debug(4,"r: syssymset,         set = \"" set "\"")
		    print_debug(4,"r: syssymset,         pos = \"" pos "\"")
		    print_debug(4,"r: syssymset,         own = \"" own "\"")
		    print_debug(5,"r: syssymset,         src = \"" src "\"")
		    set_symbol($2, $3, $1, $4, set, pos, own, src, "syssymset")
		    n++
		} else { unrecog(fnlp, $0); continue }
	    }
	    close(list)
	    set_symset(set, hash, pos, ssym, scrc, own, src, "syssymset")
	}
	# TODO: revalidate the hash by regenerating it
    }
    print_vinfo(1,"r: syssymset, syms " count_syms ", sets " count_sets)
    close(tar)
    return n
}
function path_symvers(path,	n,files,file,i)
{
    if (!path) {
	# Build a search path.  Note that we should always prefer System.symvers
	# because our modpost.awk will get details from System.map or /proc/ksyms
	# and the kernel modules themselves when a kernel Module.symvers is not
	# available.  This will actually provide a symvers and symsets file for
	# systems not built for symbol versions or sets (such as Debian).
	path = "System.symvers /lib/modules/" kversion "/build/Module.symvers"
    }
    n = split(path,files)
    file = ""
    for (i=1;i<=n;i++) {
	if (system("test -r " files[i]) != 0) {
	    file = files[i]
	    break
	}
    }
    return file
}
function write_syssymver(file,    sym,line,count_syms,mod)
{
    if ("PWD" in ENVIRON) sub("^" ENVIRON["PWD"] "/", "", file)
    print_vinfo(1,"w: syssymver, file = \"" file "\"")
    count_syms = 0
    for (sym in syms) {
	if (sym in ownr && ownr[sym] == values["pkgdirectory"])
	    continue
	if (!(sym in crcs)) {
	    print_error("no crc for symbol: " sym)
	    continue
	}
	mod = mods[sym]
	sub(/^.*\/built-in(\.o)?$/,"vmlinux",mod)
	line = crcs[sym] "\t" sym "\t" mod
	if (values["exportsyms"]) {
	    if (!(sym in exps)) {
		print_error("w: syssymver, no export for symbol: " sym)
		continue
	    }
	    line = line "\t" exps[sym]
	}
	count_syms++
	print line > file
    }
    if (count_syms)
	close(file)
    print_vinfo(1,"w: syssymver, syms " count_syms)
}
function write_modsymver(file,    sym, line, count_syms)
{
    print_vinfo(1,"w: modsymver, file = \"" file "\"")
    count_syms = 0
    for (sym in ownr) {
	if (ownr[sym] != values["pkgdirectory"]) continue
	if (!(sym in crcs)) {
	    print_warns("no crc for symbol: " sym)
	    continue
	}
	line = crcs[sym] "\t" sym "\t" mods[sym]
	if (values["exportsyms"]) {
	    if (!(sym in exps)) {
		print_warns("w: modsymver, no export for symbol: " sym)
		continue
	    }
	    line = line "\t" exps[sym]
	}
	count_syms++
	print line > file
    }
    if (count_syms)
	close(file)
    print_vinfo(1,"w: modsymver, syms " count_syms)
}
function create_missing_symsets(	progress,count,count_syms,count_sets,sym,set,setmiss,n,symbols,hash,symfile,pos)
{
    progress = 0; count = 0; count_syms = 0; count_sets = 0
    print_vinfo(1,"p: missedset")
    for (sym in syms) {
	if (progress >= 100) {
	    print_vmore(2,"p: missedset, " count " symbols")
	    progress = 0
	}
	count++
	progress++
	if (sym in sets) {
	    print_debug(1,"p: missedset, sym " sym " already in sets " sets[sym])
	    continue
	}
	if (!(sym in mods)) {
	    print_debug(1,"p: missedset, sym " sym " has no mod")
	    continue
	}
	if (!(sym in crcs)) {
	    print_debug(1,"p: missedset, sym " sym " has no crc")
	    continue
	}
	if (!(sym in ownr)) {
	    print_debug(1,"p: missedset, sym " sym " has no owner")
	    continue
	}
	if (!values["missing"] && ownr[sym] != values["pkgdirectory"]) {
	    print_debug(1,"p: missedset, sym " sym " owned by " ownr[sym])
	    continue
	}
	print_debug(3,"p: missedset, sym " mods[sym] ":" sym)
	set = mods[sym]; if (set!~"^" ownr[sym] "/") set = ownr[sym] "/" set
	gsub(/\//,"_",set)
	print_debug(3,"p: missedset, set " set)
	setmiss[set] = 1
	if (!(set in setnumb)) {
	    setnumb[set] = 1
	    setsyms[set] = sym
	    setcrcs[set] = crcs[sym]
	} else {
	    setnumb[set]++
	    setsyms[set] = setsyms[set] "," sym
	    setcrcs[set] = setcrcs[set] "," crcs[sym]
	}
    }
    print_vinfo(2,"p: missedset, " count " symbols")
    for (set in setmiss) {
	n = split(setsyms[set],symbols,/,/)
	if (values["format"] == "redhat") {
	    hash = setcrcs[set]; gsub(/,/,"",hash)
	    hash = getline_command("printf \"%s\" '" hash "' | sha1sum")
	    gsub(/[[:space:]].*$/,"",hash)
	} else {

	    symfile = ".my.temp.sym.file"
	    for (pos=1;pos<=n;pos++) {
		sym = symbols[pos]
		print crcs[sym] "\t" sym "\t" mods[sym] "\t" exps[sym] > symfile
		written[symfile] = 1
	    }
	    if (written[symfile]) {
		close(symfile)
		delete written[symfile]
	    }
	    hash = getline_command("md5sum " symfile)
	    system_command("rm -f -- " symfile)
	    hash = substr(hash,1,16)
	}
	set_symset(set "." hash, hash, setnumb[set], setsyms[set], setcrcs[set],
		   values["pkgdirectory"], "modpost", "missedset")
	count_sets++

	for (pos=1;pos<=n;pos++) {
	    sym = symbols[pos]
	    set_symbol(sym, "", "", "", set "." hash, pos, "", "", "missedset")
	    count_syms++
	}
	delete setnumb[set]
	delete setsyms[set]
	delete setcrcs[set]
    }
    delete setmiss
    delete symbols
    print_vinfo(1,"p: missedset, sets " count_sets ", syms " count_syms)
}
function write_symsets(file,		progress,count_sets,count_syms,directory,tarball,dirname,basename,chngdir,set,setfile,nsyms,n,i,sym)
{
    progress = 0; count_sets = 0; count_syms = 0
    print_vinfo(1,"w: pkgsymset, file   = \"" file "\"")
    print_vinfo(1,"w: pkgsymset, format = \"" values["format"] "\"")
    directory = file
    sub(/\.tar\.gz/, "", directory)
    tarball = directory ".tar.gz"
    dirname  = getline_command("dirname "  directory)
    print_debug(2,"w: pkgsymset, dirname  = \"" dirname "\"")
    basename = getline_command("basename " directory)
    print_debug(2,"w: pkgsymset, basename = \"" basename "\"")
    if (dirname == ".") { chngdir = "" } else { chngdir = " -C " dirname }
    system_command("mkdir -p " directory)
    for (set in setsets) {
	if (progress >= 100) {
	    print_vmore(2,"w: pkgsymset, sets " count_sets ", syms " count_syms)
	    progress = 0
	}
	progress++; count_sets++
	#if (!(set in setownr) || setownr[set] != values["pkgdirectory"]) continue
	print_debug(1,"w: pkgsymset, set = " set)
	print_debug(3,"w: pkgsymset, removing " directory "/" set ".*")
	system_command("rm -f -- " directory "/" set ".*")
	setfile = directory "/" set
	print_debug(3,"w: pkgsymset, creating " setfile)
	if (set in setsyms) {
	    nsyms = count_syms
	    n = split(setsyms[set],symbols,/,/)
	    for (i=1;i<=n;i++) {
		if (progress >= 100) {
		    print_vmore(2,"w: pkgsymset, sets " count_sets ", syms " count_syms)
		    progress = 0
		}
		progress++; count_syms++
		sym = symbols[i]
		print_debug(3,"w: pkgsymset, adding sym " sym)
		if (values["format"] == "redhat")
		    print crcs[sym] "\t" sym "\t" mods[sym] > setfile
		else
		    print crcs[sym] "\t" sym "\t" mods[sym] "\t" exps[sym] > setfile
		written[setfile] = 1
	    }
	    print_vinfo(2,"w: pkgsymset, set " set ", syms " (count_syms - nsyms))
	    print_vmore(2,"w: pkgsymset, sets " count_sets ", syms " count_syms)
	    if (setfile in written) {
		close(setfile)
		delete written[setfile]
	    }
	}
    }
    print_debug(3,"w: pkgsymset, done processing symbols")
    print_debug(3,"w: pkgsymset, creating " tarball)
    system_command("tar -czf " tarball chngdir " " basename)
    print_debug(3,"w: pkgsymset, removing working directory " directory)
    system_command("rm -fr -- " directory)
    print_vinfo(1,"w: pkgsymset, sets " count_sets ", syms " count_syms)
}
function write_header(file, mod, base)
{
    print "\
#include <linux/module.h>\n\
#include <linux/vermagic.h>\n\
#include <linux/compiler.h>\n\
\n\
MODULE_INFO(vermagic, VERMAGIC_STRING);\n\
#ifdef CONFIG_SUSE_KERNEL\n\
MODULE_INFO(supported, \"yes\");\n\
#endif\n\
\n\
#undef unix\n\
\n\
#ifndef __attribute_used__\n\
#define __attribute_used__ __used\n\
#endif\n\
\n\
#undef KBUILD_MODNAME\n\
#define KBUILD_MODNAME " base "\n\
\n\
#ifdef KBUILD_MODNAME\n\
struct module __this_module\n\
__attribute__((section(\".gnu.linkonce.this_module\"))) = {\n\
	.name = __stringify(KBUILD_MODNAME),\
" > file
	if (mod in mod_init)
	    print "\t.init = init_module," > file
	if (mod in mod_exit)
	    print "#ifdef CONFIG_MODULE_UNLOAD\n\t.exit = cleanup_module,\n#endif" > file
	print "};\n#endif" > file
}
function write_modversions(file, mod, base,	count_unds,count_weak,pair,ind,name,sym,add,fmt)
{
    fmt = "w: mymodules, %-20s: %14s; %-30s"
    count_unds = 0; count_weak = 0
    text = ""
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	if (name != mod) continue
	if (!(sym in crcs)) continue
	print_debug(3,sprintf(fmt, base, crcs[sym], sym))
	text = text "\n\t{ " crcs[sym] ", \"" sym "\" },"
	count_unds++
    }
    if (values["weak-versions"]) {
	for (pair in mod_weak) {
	    split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	    if (name != mod) continue
	    if ((!(sym in ownr) || ownr[sym] != values["pkgdirectory"]) && values["weak-hidden"])
		continue
	    if (!(sym in crcs)) continue
	    print_debug(3,sprintf(fmt, base, crcs[sym], sym))
	    text = text "\n\t{ " crcs[sym] ", \"" sym "\" },"
	    count_weak++
	}
    }
    if (text) {
	print "\n\
#ifdef CONFIG_MODVERSIONS\n\
static const struct modversion_info ____versions[]\n\
__attribute_used__\n\
__attribute__((section(\"__versions\"))) = {\
" text "\n};\n#endif" > file
    }
    if (values["weak-versions"] && values["weak-hidden"]) {
	text = ""
	for (pair in mod_weak) {
	    split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	    if (name != mod) continue
	    if ((sym in ownr) && ownr[sym] == values["pkgdirectory"]) continue
	    if (!(sym in crcs)) continue
	    print_debug(3,sprintf(fmt, base, crcs[sym], sym))
	    text = text "\n\t{ " crcs[sym] ", \"" sym "\" },"
	    count_weak++
	}
	if (text) {
	    print "\n\
#ifdef CONFIG_MODVERSIONS\n\
static const struct modversion_info ____weak_versions[]\n\
__attribute_used__\n\
__attribute__((section(\"__weak_versions\"))) = {\
" text "\n};\n#endif" > file
	}
    }
    if (values["rip-symbols"]) {
	text = ""
	for (pair in mod_unds) {
	    split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	    if (name != mod) continue
	    if (sym in crcs) continue
	    if (sym in mapaddrs) { add = mapaddrs[sym] } else { add = "00000000" }
	    print_debug(3,sprintf(fmt, base, "0x" add, sym))
	    text = text "\n\t{ " "0x" add ", \"" sym "\" },"
	    count_unds++
	}
	if (text) {
	    print "\n\
static const struct modversion_info ____absolute[]\n\
__attribute_used__\n\
__attribute__((section(\"__absolute\"))) = {\
" text "\n};" > file
	}
    }
    if (values["rip-weak"]) {
	text = ""
	for (pair in mod_weak) {
	    split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	    if (name != mod) continue
	    if (sym in crcs) continue
	    if (sym in mapaddrs) { add = mapaddrs[sym] } else { add = "00000000" }
	    print_debug(3,sprintf(fmt, base, "0x" add, sym))
	    text = text "\n\t{ " "0x" add ", \"" sym "\" },"
	    count_weak++
	}
	if (text) {
	    print "\n\
static const struct modversion_info ____weak_absolute[]\n\
__attribute_used__\n\
__attribute__((section(\"__weak_absolute\"))) = {\
" text "\n};" > file
	}
    }
    print_debug(1, "w: mymodules, " base ": unds " count_unds ", weak " count_weak)
}
function write_dependencies(file, mod, base,	count, deps, sep, pair, ind, name, dep) {
    count = 0; deps = ""; sep = ""
    for (pair in mod_deps) {
	split(pair, ind, SUBSEP); name = ind[1]; dep = ind[2]
	if (name != mod) continue
	sub(/.*\//, "", dep)
	if (dep == "vmlinux" || dep == "built-in" || dep == "built-in.o" || dep == "") continue
	print_debug(3,"w: mymodules, " base ": depends on " dep)
	deps = deps sep dep; sep = ","; count++
    }
# dependencies are never based on weak symbols
#    if (values["weak-versions"] && !values["weak-hidden"]) {
#	for (pair in mod_wdep) {
#	    split(pair, ind, SUBSEP); name = ind[1]; dep = ind[2]
#	    if (name != mod) continue
#	    sub(/.*\//, "", dep)
#	    if (dep == "vmlinux" || dep == "built-in" || dep == "built-in.o" || dep == "") continue
#	    print_debug(3,"w: mymodules, " base ": depends on " dep)
#	    deps = deps sep dep; sep = ","; count++
#	}
#    }
    if (deps) {
	print_debug(1,"wrote " count " dependencies")
	print "\n\
static const char __module_depends[]\n\
__attribute_used__\n\
__attribute__((section(\".modinfo\"))) =\n\
\"depends=" deps "\";\
" > file
    }
}
function write_srcversion(file, mod, base,	srcversion)
{
    srcversion = "42e8e4e40dfa7590147e23a5b9a3105d"
    # first step, see if there are any __mod_version symbols
    reason = ""
    if (mod in mod_vers)
	reason = "version"
    else if (values["allsrcversion"])
	reason = "all"
    else return
    # the tricky way to find sources is to use the dependencies; however,
    # this means that we must enable dependency tracking when building rpms
    srcbase = base
    sub(/^streams[-_]/, "", srcbase)
    command = "find .deps -name 'lib*" srcbase "_a-*.Po' -o -name 'lib*_a-*" srcbase ".Po' | xargs egrep -h ':$' | sort -u | cut -f1 -d: | egrep -v '(/lib/modules|/usr/lib)' | xargs -r cat | md5sum" # openssl md4 | cut -f2 '-d '"
    result = (command | getline)
    if (result == 1)
	srcversion = $1
    else print_error("comand result was " result ", ERRNO = " ERRNO)
    close(command)
    #system("rm -f " srcbase ".sources.log")
    if (reason == "all")
	print "\n#ifdef CONFIG_MODULE_SRCVERSION_ALL\nMODULE_INFO(srcversion, \"" srcversion "\");\n#endif" > file
    else
	print "\nMODULE_INFO(srcversion, \"" srcversion "\");\n" > file
}
function write_rippedsyms(file, mod, base,	fname,count_unds,count_weak,pair,name,sym,grep,result,defs,fmt)
{
    fmt = "w: mymodules, %-20s: %14s; %-30s"
    fname = file; sub(/\.mod\.c$/,".lds",fname)
    count_unds = 0; count_weak = 0; defs = ""
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	if (name != mod) continue
	if (sym in crcs) continue
	if (!(sym in mapsyms)) {
	    print_error(sprintf(fmt, base, "norm no res", sym))
	    continue
	}
	if (!(sym in maptypes)) {
	    print_error(sprintf(fmt, base, "norm no typ", sym))
	    continue
	}
	if (!(sym in mapaddrs)) {
	    print_error(sprintf(fmt, base, "norm no add", sym))
	    continue
	}
	if (mapaddrs[sym]~/,/) {
	    print_error(sprintf(fmt, base, "norm addr??", sym))
	    continue
	}
	if (maptypes[sym]!~/[sSgGCbBdDtTwWvV]/) {
	    print_warns(sprintf(fmt " map = %s", base, "norm type??", sym, maptypes[sym]))
	    continue
	}
	if (values["rip-symbols"]) {
	    print_vinfo(1, sprintf(fmt, base, "norm newdef", sym))
	    if (count_unds || count_weak) defs = defs "\n"
	    defs = defs "        " sym " = 0x" mapaddrs[sym] ";"
	    count_unds++
	} else {
	    print_error(sprintf(fmt, base, "norm no def", sym))
	}
    }
    if (values["weak-symbols"]) {
	for (pair in mod_weak) {
	    split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	    if (name != mod) continue
	    if (sym in crcs && !values["weak-hidden"]) continue
	    if (!(sym in mapsyms)) {
		print_warns(sprintf(fmt, base, "weak no res", sym))
		continue
	    }
	    if (!(sym in maptypes)) {
		print_error(sprintf(fmt, base, "weak no typ", sym))
		continue
	    }
	    if (!(sym in mapaddrs)) {
		print_warns(sprintf(fmt, base, "weak no add", sym))
		continue
	    }
	    if (mapaddrs[sym]~/,/) {
		print_warns(sprintf(fmt, base, "weak addr??", sym))
		continue
	    }
	    if (maptypes[sym]!~/[sSgGCbBdDtTwWvV]/) {
		print_warns(sprintf(fmt " map = %s", base, "weak type??", sym, maptypes[sym]))
		continue
	    }
	    if (values["rip-weak"]) {
		print_vinfo(1,sprintf(fmt, base, "weak newdef", sym))
		if (count_unds || count_weak) defs = defs "\n"
		defs = defs "        " sym " = 0x" mapaddrs[sym] ";"
		count_weak++
	    } else {
		if (values["unres-weak"])
		    print_warns(sprintf(fmt, base, "weak no def", sym))
		else
		    print_error(sprintf(fmt, base, "weak no def", sym))
	    }
	}
    }
    if (count_unds || count_weak) {
	if (!("V" in ENVIRON) || (ENVIRON["V"] != 1))
	    print "  GEN    " base ".lds"
	else
	    print_vinfo(1,"w: mymodules, " base ": file = " fname)
	print "SECTIONS\n{" defs "\n}" > fname
	close(fname)
    }
    print_debug(1,"w: mymodules, " base ": unds " count_unds ", weak " count_weak)
}
function write_mymodules(modules,    i, base, file, mod)
{
    files = 0
    print_vinfo(1,"w: mymodules")
    for (i = 1; i in modules; i++) {
	base = modules[i]
	sub(/(\.mod)?\.(k)?o(\.gz)?$/, "", base)
	#sub(/^.*\//, "", base)
	file = base ".mod.c"
	sub(/^.*\//, "", base)
	mod = values["pkgdirectory"] "/" base
	files++
	if (!("V" in ENVIRON) || (ENVIRON["V"] != 1))
	    print "  GEN    " base ".mod.c"
	else
	    print_vinfo(1,"w: mymodules, " base ": file = " file)
	write_header(file, mod, base)
	if (values["modversions"])
	    write_modversions(file, mod, base)
	write_dependencies(file, mod, base)
	write_srcversion(file, mod, base)
	write_rippedsyms(file, mod, base)
	close(file)
    }
    print_vinfo(1,"w: mymodules, " files " files written")
}
BEGIN {
    LINT = "yes"
    me = "modpost.awk"
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
    if ("kversion" in ENVIRON) { kversion = ENVIRON["kversion"] }
    else                       { kversion = getline_command("uname -r") }
    # MODPOST_SYSVER  contains all the system  defined symbol versions
    # MODPOST_MODVER  contains all the package defined symbol versions
    # MODPOST_SYMSETS contains the new symbol sets
    longopts["modules"      ] = "M:" ;																		  descrips["modules"      ] = "modules for which to generate symbols (space separated list)"			; longargs["modules"] = "MODULE ..."
    longopts["cachefile"    ] = "c:" ; environs["cachefile"    ] = "MODPOST_CACHE"		 ; defaults["cachefile"    ] = "modpost.cache"					; descrips["cachefile"    ] = "where to cache system smybol versions"
    longopts["kversion"     ] = "k:" ; environs["kversion"     ] = "kversion"			 ; defaults["kversion"     ] = kversion						; descrips["kversion"     ] = "kernel version KVERSION"
    longopts["moddir"       ] = "d:" ; environs["moddir"       ] = "MODPOST_MODDIR"		 ; defaults["moddir"       ] = "/lib/modules/" kversion				; descrips["moddir"       ] = "directory containing modules for which to generate symbols"
    longopts["filename"     ] = "F:" ; environs["filename"     ] = "MODPOST_SYSMAP"		 ; defaults["filename"     ] = "/boot/System.map-" kversion			; descrips["filename"     ] = "system map file"
    longopts["infile"       ] = "i:" ; environs["infile"       ] = "MODPOST_INPUTS"		 ; defaults["infile"       ] = "/lib/modules/" kversion "/build/Module.symvers"	; descrips["infile"       ] = "input file (space separated)"
    longopts["outfile"      ] = "o:" ; environs["outfile"      ] = "MODPOST_MODVER"		 ; defaults["outfile"      ] = "Module.symvers"					; descrips["outfile"      ] = "output file for module symbols"
    longopts["sysfile"      ] = "s:" ; environs["sysfile"      ] = "MODPOST_SYSVER"		 ; defaults["sysfile"      ] = "System.symvers"					; descrips["sysfile"      ] = "output file for system symbols"
    longopts["ksysvers"     ] = ":"  ; environs["ksysvers"     ] = "MODPOST_KSYSVERS"		 ; defaults["ksysvers"     ] = "/boot/symvers-" kversion ".gz"			; descrips["ksysvers"     ] = "input file for system sysvers"
    longopts["sysvers"      ] = ":"  ; environs["sysvers"      ] = "MODPOST_SYSVERS"		 ; defaults["sysvers"      ] = "sysvers-" kversion ".gz"			; descrips["sysvers"      ] = "output file for combined sysvers"
    longopts["pkgabi"       ] = "P:" ; environs["pkgabi"       ] = "MODPOST_PKGABI"		 ; defaults["pkgabi"       ] = "package.abi"					; descrips["pkgabi"       ] = "input file containing package ABI definitions"
    longopts["symsets"      ] = "S:" ; environs["symsets"      ] = "MODPOST_SYMSETS"		 ; defaults["symsets"      ] = "symsets-openss7-" kversion ".tar.gz"		; descrips["symsets"      ] = "output file for symsets"
    longopts["ksymsets"     ] = "K:" ; environs["ksymsets"     ] = "MODPOST_KSYMSETS"		 ; defaults["ksymsets"     ] = "/boot/symsets-" kversion ".tar.gz"		; descrips["ksymsets"     ] = "input system symsets file"
    longopts["missing"      ] = "g"  ;								   defaults["missing"      ] = 0						; descrips["missing"      ] = "create missing system symsets (ksyms missing from kabi)"
    longopts["rip-symbols"  ] = "r"  ;								   defaults["rip-symbols"  ] = 1						; descrips["rip-symbols"  ] = "attempt to rip undefined symbols from system map (requires -F, implies -U)"
    longopts["rip-weak"     ] = "R"  ;								   defaults["rip-weak"     ] = 1						; descrips["rip-weak"     ] = "attempt to rip weak undefined symbols from system map (requires -F, implies -U)"
    longopts["unres-weak"   ] = "b"  ;								   defaults["unres-weak"   ] = 1						; descrips["unres-weak"   ] = "allow unresolved weak symbols"
    longopts["unsupported"  ] = "U"  ;								   defaults["unsupported"  ] = 1						; descrips["unsupported"  ] = "permit use of (strong) unsupported (non-ABI) kernel exports"
    longopts["weak-symbols" ] = "w"  ;								   defaults["weak-symbols" ] = 1						; descrips["weak-symbols" ] = "resolve weak symbols (useful with -r and -U)"
    longopts["weak-versions"] = "W"  ;								   defaults["weak-versions"] = 1						; descrips["weak-versions"] = "version weak symbols (useful with -w)"
    longopts["weak-hidden"  ] = "H"  ;								   defaults["weak-hidden"  ] = 0						; descrips["weak-hidden"  ] = "weak symbol versions are hidden (used with -W)"
    longopts["kabi"         ] = ""   ;								   defaults["kabi"         ] = 0						; descrips["kabi"         ] = "distribution has a kABI"
    longopts["format"       ] = "f:" ;								   defaults["format"       ] = "auto"						; descrips["format"       ] = "symsets file format (suse,redhat,auto)"
    longopts["unload"       ] = "u"  ; environs["unload"       ] = "CONFIG_MODULE_UNLOAD"	 ; defaults["unload"       ] = 0						; descrips["unload"       ] = "module unloading is supported"
    longopts["modversions"  ] = "m"  ; environs["modversions"  ] = "CONFIG_MODVERSIONS"		 ; defaults["modversions"  ] = 0						; descrips["modversions"  ] = "module versions are supported"
    longopts["allsrcversion"] = "a"  ; environs["allsrcversion"] = "CONFIG_MODULE_SRCVERSION_ALL"; defaults["allsrcversion"] = 0						; descrips["allsrcversion"] = "source version all modules"
    longopts["exportsyms"   ] = "x"  ;								   defaults["exportsyms"   ] = 0						; descrips["exportsyms"   ] = "place export symbols in versions files"
    longopts["pkgdirectory" ] = "p:" ; environs["pkgdirectory" ] = "PACKAGE_LCNAME"		 ; defaults["pkgdirectory" ] = "openss7"					; descrips["pkgdirectory" ] = "subdirectory for module"
    longopts["exit-on-error"] = "e"  ;								   defaults["exit-on-error"] = 0						; descrips["exit-on-error"] = "exit with error status on program errors"
    longopts["dryrun"       ] = "n"  ;								   defaults["dryrun"       ] = 0						; descrips["dryrun"       ] = "don't perform actions, just check them"
    longopts["dry-run"      ] = "n"  ;								   defaults["dry-run"      ] = 0						; descrips["dry-run"      ] = "don't perform actions, just check them"
    longopts["quiet"        ] = "q"  ;								   defaults["quiet"        ] = 0						; descrips["quiet"        ] = "suppress normal output"
    longopts["silent"       ] = "q"  ;								   defaults["silent"       ] = 0						; descrips["silent"       ] = "suppress normal output"
    longopts["debug"        ] = "D::"; environs["debug"        ] = "MODPOST_DEBUG"		 ; defaults["debug"        ] = 0						; descrips["debug"        ] = "increase or set debug level DEBUG"
    longopts["verbose"      ] = "v::"; environs["verbose"      ] = "MODPOST_VERBOSE"		 ; defaults["verbose"      ] = 0						; descrips["verbose"      ] = "increase or set verbosity level VERBOSITY"
    longopts["help"         ] = "h!" ;																		  descrips["help"         ] = "display this usage information and exit"
    longopts["version"      ] = "V!" ;																		  descrips["version"      ] = "display script version and exit"
    longopts["copying"      ] = "C!" ;																		  descrips["copying"      ] = "display coding permissions and exit"
    # mark options that must default to environment or default setting
    values["cachefile"    ] = ""
    # set mandatory defaults
    values["missing"      ] = defaults["missing"      ]
    values["rip-symbols"  ] = defaults["rip-symbols"  ]
    values["rip-weak"     ] = defaults["rip-weak"     ]
    values["unres-weak"   ] = defaults["unres-weak"   ]
    values["unsupported"  ] = defaults["unsupported"  ]
    values["weak-symbols" ] = defaults["weak-symbols" ]
    values["weak-versions"] = defaults["weak-versions"]
    values["weak-hidden"  ] = defaults["weak-hidden"  ]
    values["format"       ] = defaults["format"       ]
    values["unload"       ] = defaults["unload"       ]
    values["modversions"  ] = defaults["modversions"  ]
    values["allsrcversion"] = defaults["allsrcversion"]
    values["exportsyms"   ] = defaults["exportsyms"   ]
    values["pkgdirectory" ] = defaults["pkgdirectory" ]
    values["exit-on-error"] = defaults["exit-on-error"]
    values["dryrun"       ] = defaults["dryrun"       ]
    values["dry-run"      ] = defaults["dry-run"      ]
    values["quiet"        ] = defaults["quiet"        ]
    values["silent"       ] = defaults["silent"       ]
    values["debug"        ] = defaults["debug"        ]
    values["verbose"      ] = defaults["verbose"      ]
    optstring = "M:c:k:d:F:i:o:s:*P:K:S:grRbUwWHf:umaxp:enqD::v::hVC"
    optind = 0
    #opts = ""; for (i=1;i<ARGC;i++) { if (i == 1) { opts = ARGV[i] } else { opts = opts " " ARGV[i] } }
    #print me ": D: o: command line: " opts > stderr; written[stderr] = 1
    command = ""
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	#if (c != -1) { print me ": D: o: option -" c ", longopt --" option ", optset = " optset ", optarg = " optarg > stderr; written[stderr] = 1 }
	if (c == -1) break
	else if (c ~ /[MckdFiosPKSfp]/)			{ values[option] = optarg }
	else if (c ~ /[grRbUwWHumaxenq]/)		{ values[option] = optset }
	else if (c~/[Dv]/) { if (optarg != "")		{ values[option] = optarg } else { if (optset)  { values[option]++ } else { values[option] = optset } } }
	else if (c~/[hVC]/)	{ command = option }
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
	if ("modules" in values)
	{ values["modules"] = values["modules"] " " ARGV[optind] } else
	{ values["modules"] = ARGV[optind] }
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
    count_syms = 0; count_sets = 0; count_maps = 0
    cache_dirty = 1
    if (("cachefile" in values) && values["cachefile"] && (read_cachefile(values["cachefile"]) != -1))
	cache_dirty = 0
    else {
	if (("filename" in values) && values["filename"]) {
	    command = cat_command(values["filename"])
	    if (command) read_systemmap(command, "vmlinux", "kernel", "systemmap")
	} else {
	    print_vinfo(2,"r: systemmap, searching...")
	    command = cat_command("/boot/System.map-" kversion)
	    if (command) { read_systemmap(command, "vmlinux", "kernel", "systemmap") }
	    else {
	    command = cat_command("/boot/System.map-" kversion ".gz")
	    if (command) { read_systemmap(command, "vmlinux", "kernel", "systemmap") }
	    else {
	    command = cat_command("/boot/System.map-" kversion ".bz2")
	    if (command) { read_systemmap(command, "vmlinux", "kernel", "systemmap") }
	    else {
	    print_error("r: systemmap, file not found") } } }
	}
	if (("infile" in values) && values["infile"]) {
	    command = cat_command(values["infile"])
	    if (command) read_dumpfiles(command, "kernel", "infile")
	} else {
	    print_vinfo(2,"r: syssymver, searching...")
	    command = cat_command("/lib/modules/" kversion "/build/Module.symvers")
	    if (command) { read_dumpfiles(command, "kernel", "syssymver") }
	    else {
	    command = cat_command("/boot/symvers-" kversion ".gz")
	    if (command) { read_dumpfiles(command, "kernel", "syssymver") }
	    else {
	    print_error("r: syssymver, file not found") } }
	}
	if (("moddir" in values) && values["moddir"])
	    read_moduledir(values["moddir"], "moduledir")
	else {
	    if (system("test -d /lib/modules/" kversion) == 0)
		read_moduledir("/lib/modules/" kversion, "moduledir")
	    else
		print_error("r: moduledir, directory not found")
	}
	if (("ksymsets" in values) && values["ksymsets"])
	    values["kabi"] = read_symsets(values["ksymsets"], "kabi", "syssymset")
	else {
	    values["kabi"] = 0
	    file = "symsets-" kversion ".tar.gz"
	    if (system("test -r /boot/" file) == 0) {
		values["kabi"] = values["kabi"] + read_symsets("/boot/" file, "kabi", "syssymset")
	    }
	    if (system("test -r /lib/modules/" kversion "/build/" file) == 0) {
		values["kabi"] = values["kabi"] + read_symsets("/lib/modules/" kversion "/build/" file, "kabi", "syssymset")
	    }
	}
    }
    if (("modules" in values) && values["modules"]) {
	split(values["modules"], modules)
	read_mymodules(modules, "modules")
    }
    if (("symsets" in values) && values["symsets"])
	create_missing_symsets()
    # write out cache file
    if (("cachefile" in values) && values["cachefile"] && cache_dirty)
	write_cachefile(values["cachefile"])
    if (("sysfile" in values) && values["sysfile"])
	write_syssymver(values["sysfile"])
    if (("modules" in values) && values["modules"])
	write_mymodules(modules)
    if (("outfile" in values) && values["outfile"])
	write_modsymver(values["outfile"])
    if (("symsets" in values) && values["symsets"])
	write_symsets(values["symsets"])
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

# =============================================================================
#
# $Log: modpost.awk,v $
# Revision 1.1.2.13  2011-04-12 06:33:27  brian
# - passes distcheck
#
# Revision 1.1.2.12  2011-04-11 06:13:43  brian
# - working up weak updates
#
# Revision 1.1.2.11  2011-04-08 12:35:42  brian
# - documented openss7-modules script
#
# Revision 1.1.2.10  2011-04-06 21:33:05  brian
# - corrections
#
# Revision 1.1.2.9  2011-04-05 16:35:10  brian
# - weak module design
#
# Revision 1.1.2.8  2011-03-26 04:28:46  brian
# - updates to build process
#
# Revision 1.1.2.7  2011-03-17 07:01:29  brian
# - build and repo system improvements
#
# Revision 1.1.2.6  2011-01-12 03:44:13  brian
# - update awk scripts and work around gawk close bug
#
# Revision 1.1.2.5  2010-11-28 14:01:53  brian
# - awk script updates and corrections
#
# Revision 1.1.2.4  2009-09-08 10:46:51  brian
# - changes to avoid haldaemon problems
#
# Revision 1.1.2.3  2009-09-01 09:09:50  brian
# - added text image files
#
# Revision 1.1.2.2  2009-07-23 16:37:51  brian
# - updates for release
#
# Revision 1.1.2.1  2009-07-21 11:06:21  brian
# - new awk scripts for release check
#
# =============================================================================
# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
