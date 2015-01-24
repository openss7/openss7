#!/usr/bin/awk -f
# =============================================================================
#
# @(#) scripts/symsets.awk
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
    if ((values["quiet"] == 0) && (values["verbose"] >= level)) {
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
    if (values["debug"] >= level) {
	printf "%s", mag me ": D: " string std cr lf > stderr
	fflush(stderr)
	written[stderr] = 1
    }
}
function print_error(string)
{
    print red me ": E: " string std > stderr
    if (count_errs) errors = errors "\n"
    errors = errors red me ": E: " string std;
    written[stderr] = 1
    count_errs++
}
function print_emore(string)
{
    print red me ": E: " string std > stderr
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
  scripts/symsets.awk\n\
Usage:\n\
  awk -f " me " -- [options] [INPUT ...]\n\
  awk -f " me " -- -" longopts["help"] "\n\
  awk -f " me " -- -" longopts["version"] "\n\
  awk -f " me " -- -" longopts["copying"] "\
" > output
    written[output] = 1
}
function help_usage(name,  line,sep,dflt,env)
{
    line = ""; sep = ""; dflt = ""; env = ""
    if (name in defaults) {
	if (longopts[name]~/:/) {
	    dflt = defaults[name]
	} else {
	    if (defaults[name]) { dflt = "yes" } else { dflt = "no" }
	}
	if (dflt) {
	    line = line sep "[default: " dflt "]"; sep = " "
	}
    }
    if (name in environs) {
	if (longopts[name]!~/:/) env = "?"
	if (environs[name])
	    line = line sep "{" env environs[name] "}"; sep = " "
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
	    gsub(/:/,"",char)
	    line = "-" char ", --" name
	} else {
	    line = "--" name
	}
	if (opt~/::$/) { line = line " [" toupper(name) "]" } else
	if (opt~/:$/)  { line = line " " toupper(name) } else
	if (name !~ /^(help|version|copying)$/)
	               { line = line ", --no-" name }
	if (opt~/[[:alnum:]]/) {
	    for (oth in longopts) {
		if (opt != longopts[oth]) continue
		if (oth == name) continue
		if (opt~/::$/) { line = line ", --" oth " [" toupper(oth) "]" } else
		if (opt~/:$/)  { line = line ", --" oth " " toupper(oth) } else
			       { line = line ", --" oth ", --no-" oth }
	    }
	}
    }
    return line
}
function help_option(output, name)
{
    printf "  %s\n", help_opttags(name) > output
    if (name in descrips && descrips[name])
    printf "      %s\n", descrips[name] > output
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
  INPUT ...\n\
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
scripts/symsets.awk\n\
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
scripts/symsets.awk\n\
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
    option = ""; optval = ""; optarg = ""; optset = ""; pos = 0; needarg = 0; wantarg = 0
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
		if (longopts[option] ~ /:/ ) { needarg = 1 }
		if ((wantarg || needarg) && !optset) {
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
    print_debug(3,cmd); return system(cmd)
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
    print_vinfo(1,"r: systemmap, syms " count_syms ", unds " count_unds, ", weak " count_weak)
}
# read modpost cache file
function read_cachefile(file,    i,result,line,n,fields)
{
    if ("PWD" in ENVIRON) sub("^" ENVIRON["PWD"] "/", "", file)
    print_vinfo(1,"r: cachefile, file = \"" file "\"")
    count_syms = 0; count_sets = 0
    while ((result = (getline line < file)) == 1) {
	if (line~/:/) {
	    # new style cache file format
	    n = split(line,fields,/:/)
	    if (fields[1] == "format") {
		values["format"] = fields[2]
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
    print_debug(3,"result = " result)
    if (result == -1)
	print_vinfo(1,"r: cachefile, file not found")
    else
	print_vinfo(1,"r: cachefile, syms " count_syms ", sets " count_sets)
    return result
}
function write_cachefile(file,	    sym,set,line,count_syms,count_sets,count_maps)
{
    count_syms = 0; count_sets = 0; count_maps = 0
    if ("PWD" in ENVIRON) sub("^" ENVIRON["PWD"] "/", "", file)
    print_vinfo(1,"w: cachefile, file = \"" file "\"")
	print "format:" values["format"] > file
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
# read and process package Module.symvers file
function read_modsymver(file,	    lineno,fnl,fnlp,set,pos,count_syms,count_sets)
{
    print_vinfo(1,"r: modsymver, file = \"" file "\"")
    lineno = 0; count_syms = 0; count_sets = 0
    while ((result = (getline < file)) == 1) {
	lineno++; fnl = file ":" lineno; fnlp = fnl ": "
	if (NF < 4) { missing(fnlp, 4); continue }
	if (NF > 4) { garbage(fnlp, 4); NF = 4 }
	if (NF < 4) { $4 = ""; NF = 4 }
	if ($1~/^0x/) {
	    if ($2 in sets) {
		set_symbol($2, $3, $1, $4, sets[$2], posn[$2], ownr[$2], "modsymver")
		continue
	    }
	    # make a new set
	    set = $3; gsub(/\//, "_", set)
	    if (set in setsets || !(set in setnumb)) {
		pos = 1; count_sets++
		set_symset(set, "", pos, $2, $1, values["pkgdirectory"], "modsymver")
	    } else {
		setnumb[set]++; pos = setnumb[set]
		setsyms[set] = setsyms[set] "," $2
		setcrcs[set] = setcrcs[set] "," $1
	    }
	    count_syms++
	    set_symbol($2, $3, $1, $4, set, pos, values["pkgdirectory"], "modsymver")
	} else { unrecog(fnlp, $0); continue }
    }
    print_vinfo(1,"r: modsymver, syms " count_syms ", sets" count_sets)
    if (result == -1) return 0
    close(file)
    return 1
}
# read in system defined symsets
function path_symsets(path,	find,sep,file,files,n,i)
{
    if (!path) {
	# build a search path
	find = "find /boot /usr/src/kernels -type f -name 'symsets-" kversion ".tar.gz' 2>/dev/null"
	path = ""; sep = ""
	while ((find | getline file)) {
	    path = path sep file
	    sep = " "
	}
	close(find)
    }
    n = split(path,files)
    file = ""
    for (i=1;i<=n;i++) {
	if (system("test -r " files[i]) != 0) {
	    file = files[i]
	    break;
	}
    }
    return file
}
# read and process symsets-kversion.tar.gz file
function read_symsets(file, own, src,	tar,fname,set,hash,pos,lineno,ssym,scrc,sep,list,fnl,fnlp)
{
    count_syms = 0; count_sets = 0
    print_vinfo(1,"r: syssymset, file = \"" file "\"")
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
		} else { unrecog(fnlp, $0); continue }
	    }
	    close(list)
	    set_symset(set, hash, pos, ssym, scrc, own, src, "syssymset")
	}
	# TODO: revalidate the hash by regenerating it
    }
    print_vinfo(1,"r: syssymset, syms " count_syms ", sets " count_sets)
    close(tar)
    return 1
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
	if (!(set in setownr) || setownr[set] != values["pkgdirectory"]) continue
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
BEGIN {
    LINT = "yes"
    me = "symsets.awk"
    count_errs = 0; count_warn = 0; errors = ""
    if (!("TERM" in ENVIRON)) ENVIRON["TERM"] = "dumb"
    if (ENVIRON["TERM"] == "dumb" || system("test -t 1 -a -t 2") != 0) {
	stdout = "/dev/stderr"
	stderr = "/dev/stderr"
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
	std = "\033[m"
    }
    if ("kversion" in ENVIRON) { kversion = ENVIRON["kversion"] }
    else                       { kversion = getline_command("uname -r") }
    # MODPOST_SYSVER  contains all the system  defined symbol versions
    # MODPOST_MODVER  contains all the package defined symbol versions
    # MODPOST_SYMSETS contains the new symbol sets
    longopts["cachefile"    ] = "c:" ; environs["cachefile"     ] = "MODPOST_CACHE"   ; defaults["cachefile"    ] = "modpost.cache"			; descrips["cachefile"    ] = "where to cache system symbol versions"
    longopts["kversion"     ] = "k:" ; environs["kversion"      ] = "kversion"        ; defaults["kversion"     ] = kversion				; descrips["kversion"     ] = "kernel version KVERSION"
    longopts["inputs"       ] = "I:" ; environs["inputs"        ] = "MODPOST_MODVER"  ; defaults["inputs"       ] = "Module.symvers"			; descrips["inputs"       ] = "input Module.symvers files (space separated list)"
    longopts["sysver"       ] = "S:" ; environs["sysver"        ] = "MODPOST_SYSVER"  ; defaults["sysver"       ] = "System.symvers"			; descrips["sysver"       ] = "input System.symvers file"
    longopts["sysset"       ] = "M:" ; environs["sysset"        ] = "ksymsets"        ; defaults["sysset"       ] = "/boot/symsets-" kversion ".tar.gz"	; descrips["sysset"       ] = "input system symsets file"
    longopts["suse"         ] = "s"  ;							defaults["suse"         ] = 0					; descrips["suse"         ] = "output symsets in SuSE format"
    longopts["redhat"       ] = "r"  ;							defaults["redhat"       ] = 1					; descrips["redhat"       ] = "output symsets in RedHat format"
    longopts["missing"      ] = "m"  ;							defaults["missing"      ] = 0					; descrips["missing"      ] = "create missing system symsets"
    longopts["outfile"      ] = "o:" ; environs["outfile"       ] = "MODPOST_SYMSETS" ; defaults["outfile"      ] = "symsets-" kversion ".tar.gz"	; descrips["sysset"       ] = "output file name OUTFILE"
    longopts["pkgdirectory" ] = "p:" ; environs["pkgdirectory" ] = "PACKAGE_LCNAME"   ; defaults["pkgdirectory" ] = "openss7"				; descrips["pkgdirectory" ] = "subdirectory for module"
    longopts["exit-on-error"] = "e"  ;								   defaults["exit-on-error"] = 1						; descrips["exit-on-error"] = "exit with error status on program errors"
    longopts["dryrun"       ] = "n"  ;							defaults["dryrun"       ] = 0					; descrips["dryrun"       ] = "don't perform actions, just check them"
    longopts["dry-run"      ] = "n"  ;							defaults["dry-run"      ] = 0					; descrips["dry-run"      ] = "don't perform actions, just check them"
    longopts["quiet"        ] = "q"  ;							defaults["quiet"        ] = 0					; descrips["quiet"        ] = "suppress normal output"
    longopts["silent"       ] = "q"  ;							defaults["silent"       ] = 0					; descrips["silent"       ] = "suppress normal output"
    longopts["debug"        ] = "D::"; environs["debug"        ] = "MODPOST_DEBUG"		 ; defaults["debug"        ] = 0						; descrips["debug"        ] = "increase or set debug level DEBUG"
    longopts["verbose"      ] = "v::"; environs["verbose"      ] = "MODPOST_VERBOSE"		 ; defaults["verbose"      ] = 0						; descrips["verbose"      ] = "increase or set verbosity level VERBOSITY"
    longopts["help"         ] = "h"  ;															  descrips["help"         ] = "display this usage information and exit"
    longopts["version"      ] = "V"  ;															  descrips["version"      ] = "display script version and exit"
    longopts["copying"      ] = "C"  ;															  descrips["copying"      ] = "display coding permissions and exit"
    # mark options that must default to environment or default setting
    values["cachefile"    ] = ""
    # set mandatory defaults
    values["suse"         ] = defaults["suse"    ]
    values["redhat"       ] = defaults["redhat"  ]
    values["format"       ] = "auto"
    values["missing"      ] = defaults["missing" ]
    values["kversion"     ] = defaults["kversion"]
    values["inputs"       ] = defaults["inputs"  ]
    values["sysver"       ] = defaults["sysver"  ]
    values["outfile"      ] = defaults["outfile" ]
    values["pkgdirectory" ] = defaults["pkgdirectory" ]
    values["exit-on-error"] = defaults["exit-on-error"]
    values["debug"        ] = defaults["debug"   ]
    values["verbose"      ] = defaults["verbose" ]
    values["quiet"        ] = defaults["quiet"   ]
    optstring = "c:k:I:S:M:srmo:p:nqD::v::hVC"
    optind = 0
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	if (c == -1) break
	else if (c ~ /[ckISMop]/) { values[option] = optarg }
	else if (c ~ /[srmnq]/)  { values[option] = 1 }
	else if (c == "s") { values["format"] = "suse" }
	else if (c == "r") { values["format"] = "redhat" }
	else if (c~/[Dv]/) { if (optarg) { values[option] = optarg } else { values[option]++ } }
	else if (c == "h") { help(   stdout); exit 0 }
	else if (c == "V") { version(stdout); exit 0 }
	else if (c == "C") { copying(stdout); exit 0 }
	else               { usage(  stderr); exit 2 }
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
	    print_debug(1,"o: assigning value for " value " from environment " environs[value])
	    values[value] = ENVIRON[environs[value]]
	}
	if (!values[value] && (value in defaults) && defaults[value]) {
	    print_debug(1,"o: assigning value for " value " from default " defaults[value])
	    values[value] = defaults[value]
	}
    }
    kversion = values["kversion"]
    if (!values["inputs"] || values["inputs"] == "-") {
	print_debug(1,"o: no inputs, using " stdin)
	values["inputs"] = stdin
    }
    # read in cachefile
    cache_dirty = 1
    if (("cachefile" in values) && values["cachefile"] && (read_cachefile(values["cachefile"]) != -1))
	cache_dirty = 0
    split(values["inputs"], inputs)
    read_modsymver(inputs[1])
    write_symsets(values["outfile"])
    # write out cachefile when necessary
    if (("cachefile" in values) && values["cachefile"] && cache_dirty)
	write_cachefile(values["cachefile"])
    exit 0
}
END {
    if (count_errs) print_error("errs = " count_errs)
    if (count_warn)  print_warns("warn = " count_warn)
    for (file in written)
	close(file)
}

# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
