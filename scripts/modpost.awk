#!/usr/bin/awk -f
# =============================================================================
#
# @(#) $RCSfile: modpost.awk,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-03-17 07:01:29 $
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
# Last Modified $Date: 2011-03-17 07:01:29 $ by $Author: brian $
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
function print_info(string)
{
    if (values["quiet"] == 0 && values["verbose"] > 0) {
	print blu me ": I: " string std > stderr
	written[stderr] = 1
    }
}
function print_more(string)
{
    if (values["quiet"] == 0 && values["verbose"] > 0) {
	printf "%s", blu me ": I: " string std cr lf > stderr
	fflush(stderr)
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
function print_dmore(string)
{
    if (values["debug"] > 0) {
	printf "%s", mag me ": D: " string std cr lf > stderr
	fflush(stderr)
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
" me ":\n\
  $Id: modpost.awk,v 1.1.2.7 2011-03-17 07:01:29 brian Exp $\n\
Usage:\n\
  awk -f " me " -- [options] [MODULE ...]\n\
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
	if (longopts[name]~/:/)
	    dflt = defaults[name]
	else
	    if (defaults[name]) { dflt = "yes" } else { dflt = "no" }
	if (dflt)
	    line = line sep "[default: " dflt "]"; sep = " "
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
	    if (opt~/::$/) { line = line " [" toupper(name) "]" } else
	    if (opt~/:$/)  { line = line " " toupper(name) }
	} else {
	    line = "--" name
	}
	for (oth in longopts) {
	    if (opt != longopts[oth]) continue
	    if (oth == name) continue
	    if (opt~/::$/) { line = line ", --" oth " [" toupper(oth) "]" } else
	    if (opt~/:$/)  { line = line ", --" oth " " toupper(oth) } else
			   { line = line ", --" oth }
	}
    }
    return line
}
function help_options(output)
{
    if (!optstring) return
    print "Options:" > output; written[output] = 1
    # index all of the long options
    for (opt in longopts) {
	char = substr(longopts[opt],1,1)
	if (char) {
	    if (char in optchars) {
		if (char == substr(opt,1,1))
		    optchars[char] = opt
	    } else
		optchars[char] = opt
	}
    }
    for (pos=1;pos<=length(optstring);pos++) {
	char = substr(optstring,pos,1)
	if (char!~/[[:alnum:]]/) continue
	if (!(char in optchars)) continue
	name = optchars[char]
	printf "  %s\n", help_opttags(name) > output
	if (name in descrips)
	printf "      %s\n", descrips[name] > output
	if (name in defaults || name in environs)
	printf "      %s\n", help_usage(name) > output
    }
}
function help(output)
{
    if (values["quiet"])
	return
    usage(output)
    print "\
Arguments:\n\
  MODULE ...\n\
      modules for which to generate symbols\
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
$Id: modpost.awk,v 1.1.2.7 2011-03-17 07:01:29 brian Exp $\n\
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
$Id: modpost.awk,v 1.1.2.7 2011-03-17 07:01:29 brian Exp $\n\
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
function missing(fnlp, expected)
{
    print_error(fnlp " definition missing " (expected - NF) " fields")
}
function garbage(fnlp, maximum)
{
    print_warn(fnlp " definition with " (NF - maximum) " garbage fields")
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
    print_warn("got junk: " junk)
}
function cat_command(files,    command)
{
    command = "cat " files
    if (files ~ /\.gz\>/)
	command = "zcat " files
    if (files ~ /\.bz2\>/)
	command = "bzcat " files
    if (files ~ /\.xz\>/)
	command = "xzcat " files
    return command
}
function set_symbol(sym, mod, crc, expt, set, pos, own)
{
    if (!(sym in syms)) count_syms++
    if (sym in mods && mods[sym] != mod) {
	if (mod == "vmlinux") {
	    print_error("symbol " sym " in module " mods[sym] " conflicts with kernel")
	} else if (mods[sym] == "vmlinux") {
	    print_error("symbol " sym " in module " mod       " conflicts with kernel")
	    mod = mods[sym]
	} else {
	    base1 = mod;       sub(/.*\//, "", base1)
	    base2 = mods[sym]; sub(/.*\//, "", base2)
	    if (base1 != base2)
		print_warn("symbol " sym " conflict between " mods[sym] " and " mod)
	}
    }
    mod_syms[mod,sym] = 1
    mod_mods[mod] = 1
    mods[sym] = mod
    syms[sym] = 1
    if (crc) {
	sub(/0x00000000/, "0x", crc)
	if (sym in crcs && crcs[sym] != crc)
	    print_warn("crc for " mod ":" sym " changed from " crcs[sym] " to " crc)
	mod_crcs[mod,sym] = crc
	crcs[sym] = crc
    } else if (sym in crcs) { crc = crcs[sym] }
    if (expt) {
	if (sym in exps && exps[sym] != expt)
	    print_warn("exp for " mod ":" sym " changed from " exps[sym] " to " expt)
	mod_exps[mod,sym] = expt
	exps[sym] = expt
    } else if (sym in exps) { expt = exps[sym] }
    if (set) {
	if (sym in sets && sets[sym] != set) {
	    print_warn("set for " mod ":" sym " changed from " sets[sym] " to " set)
	    if ((sets[sym],sym) in set_syms) delete set_syms[sets[sym],sym]
	    if (sym in posn && (sets[sym],posn[sym]) in set_posn)
		delete set_posn[sets[sym],posn[sym]]
	}
	if (sym in posn) { set_syms[set,sym] = posn[sym]; set_posn[set,posn[sym]] = sym }
	sets[sym] = set
    } else if (sym in sets) { set = sets[sym] }
    if (pos) {
	if (sym in posn && posn[sym] != pos) {
	    print_warn("pos for " mod ":" sym " changed from " posn[sym] " to " pos)
	    if (sym in sets && (sets[sym],posn[sym]) in set_posn)
		delete set_posn[sets[sym],posn[sym]]
	}
	if (sym in sets) { set_syms[sets[sym],sym] = pos; set_posn[sets[sym],pos] = sym }
	posn[sym] = pos
    } else if (sym in posn) { pos = posn[sym] }
    if (own) {
	if (sym in ownr && ownr[sym] != own)
	    print_warn("own for " mod ":" sym " changed from " ownr[sym] " to " own)
	ownr[sym] = own
    } else if (sym in ownr) { own = ownr[sym] }
}
function set_symset(set, hash, numb, own)
{
    if (set in setsets)
	print_warn("set " set " already exists!")
    else {
	setsets[set] = 1
	count_sets++
    }
    if (hash) {
	if (set in sethash && sethash[set] != hash)
	    print_warn("hash for " set " changed from " sethash[set] " to " hash)
	sethash[set] = hash
    }
    if (numb) {
	if (set in setnumb && setnumb[set] != numb)
	    print_warn("numb for " set " changed from " setnumb[set] " to " numb)
	setnumb[set] = numb
    }
    if (own) {
	if (set in setownr && setownr[set] != own)
	    print_warn("own for " set " changed from " setownr[set] " to " own)
	setownr[set] = own
    }
}
function set_mod(sym, mod)
{
    if (sym in mods) {
	if (mod == mods[sym]) return
	if (mod == "vmlinux") {
	    print_error("sym " sym " in module " mods[sym] " conflicts with kernel")
	} else if (mods[sym] == "vmlinux") {
	    print_error("sym " sym " in module " mod       " conflicts with kernel")
	    mod = mods[sym]
	} else {
	    base1 = mod;       sub(/.*\//, "", base1)
	    base2 = mods[sym]; sub(/.*\//, "", base2)
	    if (base1 != base2)
		print_warn("sym " sym "conflict between " mods[sym] " and " mod)
	}
	if (mod = mods[sym]) return
	print_warn("sym " sym " mod changed from " mods[sym] " to " mod)
    } else count_mods++
    mod_syms[mod,sym] = 1
    mod_mods[mod] = 1
    mods[sym] = mod
    syms[sym] = 1
    cache_dirty = 1
}
function set_crc(sym, crc)
{
    sub(/0x00000000/, "0x", crc)
    if (sym in crcs) {
	if (crc == crcs[sym]) return
	print_warn("sym " sym " crc changed from " crcs[sym] " to " crc)
    }
    if (sym in mods) { mod_crcs[mods[sym],sym] = crc }
    crcs[sym] = crc
    syms[sym] = 1
    cache_dirty = 1
}
function set_exp(sym, expt)
{
    if (sym in exps) {
	if (expt != exps[sym]) return
	print_warn("sym " sym " exp changed from " exps[sym] " to " expt)
    }
    if (sym in mods) { mod_exps[mods[sym],sym] = expt }
    exps[sym] = expt
    syms[sym] = 1
    cache_dirty = 1
}
function set_set(sym, set)
{
    if (sym in sets) {
	if (set == sets[sym]) return
	print_warn("sym " sym " set changed from " sets[sym] " to " set)
	if ((sets[sym],sym) in set_syms) delete set_syms[sets[sym],sym]
	if (sym in posn && (sets[sym],posn[sym]) in set_posn)
	    delete set_posn[sets[sym],posn[sym]]
    }
    if (sym in posn) { set_syms[set,sym] = posn[sym]; set_posn[set,posn[sym]] = sym }
    if (sym in mods) { mod_sets[mods[sym],sym] = set }
    sets[sym] = set
    syms[sym] = 1
    cache_dirty = 1
}
function set_pos(sym, pos)
{
    if (sym in posn) {
	if (pos == posn[sym]) return
	print_warn("sym " sym " pos changed from " posn[sym] " to " pos)
	if (sym in sets && (sets[sym],posn[sym]) in set_posn)
	    delete set_posn[sets[sym],posn[sym]]
    }
    if (sym in sets) { set_syms[sets[sym],sym] = pos; set_posn[sets[sym],pos] = sym }
    if (sym in mods) { mod_posn[mods[sym],sym] = pos }
    posn[sym] = pos
    syms[sym] = 1
    cache_dirty = 1
}
function set_own(sym, own)
{
    if (sym in ownr) {
	if (own == ownr[sym]) return
	print_warn("sym " sym " own changed from " ownr[sym] " to " own)
    }
    ownr[sym] = own
    if (!(sym in syms)) count_syms++
    syms[sym] = 1
    cache_dirty = 1
}
function set_hash(set, hash)
{
    if (set in sethash) {
	if (hash == sethash[set]) return
	print_warn("set " set " hash changed from " sethash[set] " to " hash)
    }
    sethash[set] = hash
    if (!(set in setsets)) count_sets++
    setsets[set] = 1
    cache_dirty = 1
}
function set_numb(set, numb)
{
    if (set in setnumb) {
	if (numb == setnumb[set]) return
	print_warn("set " set " numb changed from " setnumb[set] " to " numb)
    }
    setnumb[set] = numb
    if (!(set in setsets)) count_sets++
    setsets[set] = 1
    cache_dirty = 1
}
function set_ownr(set, own)
{
    if (set in setownr) {
	if (own == setownr[set]) return
	print_warn("set " set " own changed from " setownr[set] " to " own)
    }
    setownr[set] = own
    if (!(set in setsets)) count_sets++
    setsets[set] = 1
    cache_dirty = 1
}
#
# Read in an objdump -t listing.  This must be from objdump -t run on a kernel
# object.  This will add EXPORT_SYMBOL or EXPORT_SYMBOL_GPL for use in the
# Module.symvers file.
#
function read_modobject(command, own)
{
    files = 0
    progress = 0
    HEADER = 1
    modname = ""
    while ((command | getline) == 1) {
	if (HEADER) {
	    if (/^$/)
		continue
	    if (/:.*file format/) {
		sub(/:.*$/, "")
		sub(/(\.mod)?\.(k)?o(\.gz)?$/, "")
		if (own) {
		    sub(/.*\//, "")
		    $0 = values["pkgdirectory"] "/" $0
		} else {
		    sub(values["moddir"] "/", "")
		    sub(/kernel\//, "")
		}
		modname = $0
		if (own) {
		    if (("vmlinux","struct_module") in mod_syms) {
			mod_unds[modname,"struct_module"] = 1
		    } else if (("vmlinux","module_layout") in mod_syms) {
			mod_unds[modname,"module_layout"] = 1
		    } else {
			mod_unds[modname,"struct_module"] = 1
		    }
		}
		continue
	    }
	    if (/SYMBOL TABLE:/) {
		count_syms = 0; count_unds = 0
		mod_mods[modname] = 1
		HEADER = 0
		if (progress >= 200) {
		    print_dmore("r: ko object, " files " kernel modules...")
		    progress = 0
		}
		files++
		progress++
		continue
	    }
	}
	if (/^$/ && !HEADER) {
	    HEADER = 1
	    if (count_syms || count_unds) {
		print_debug("r: ko object, " modname)
		print_debug("r: ko object, syms " count_syms ", unds " count_unds)
	    }
	    continue
	}
	if ($2 == "l") {
	    if ((NF >= 6) && ($3 == "O")) {
		if ($4 == ".modinfo") {
		    if ($6 ~ /^_?__mod_version[0-9]*$/)
			mod_vers[modname] = 1
		    continue
		}
		if ($4 ~ /^_?__ksymtab/) {
		    sub(/^_?__ksymtab/, "", $4)
		    if ($6 ~ /^_?__ksymtab_/) {
			sub(/^_?__ksymtab_/, "", $6)
			set_symbol($6, modname, "", "EXPORT_SYMBOL" toupper($4), "", 0, own)
		    }
		}
	    }
	    continue
	}
	if ($2 == "g") {
	    if ($3 == "*ABS*") {
		if ((NF >= 5) && ($5 ~ /^_?__crc_/)) {
		    if ($5 ~ /^_?__crc_/) {
			sub(/^_?__crc_/, "", $5)
			set_symbol($5, modname, "0x" $1, "", "", 0, own)
		    }
		}
		continue
	    }
	}
	if (!own)
	    continue
	undef = ""
	if (($2 == "w") && ($3 == "*UND*") && (NF == 5))
	    undef = $5
	if (($2 == "*UND*") && (NF == 4))
	    undef = $4
	if (undef != "") {
	    if (undef ~ /^_?__this_module$/) {
		if (modname in mod_this)
		    delete mod_this[modname]
		continue
	    }
	    if (!((modname,undef) in mod_unds)) {
		mod_unds[modname,undef] = 1
		count_unds++
	    }
	    continue
	}
	if ($NF ~ /^_?init_module$/) {
	    mod_init[modname] = 1
	    print_debug("r: module " modname " has init_module")
	    continue
	}
	if ($NF ~ /^_?cleanup_module$/) {
	    mod_exit[modname] = 1
	    print_debug("r: module " modname " has cleanup_module")
	    continue
	}
	if ($NF ~ /^_?__this_module$/) {
	    mod_this[modname] = 1
	    print_debug("r: module " modname " has this_module")
	    continue
	}
	if ($NF ~ /^_?__mod_version[0-9]*$/) {
	    mod_vers[modname] = 1
	    print_debug("r: module " modname " has mod_version")
	    continue
	}
    }
    close(command)
    print_debug("r: ko object, " files " kernel modules   ")
}
#
# Read in any nm -Bs listing.  This can be a System.map file, a Modules.map
# file or nm -Bz run an a kernel object.
#
function read_systemmap(command, modname, own)
{
    #print_debug("r: systemmap, command = \"" command "\"")
    count_syms = 0; count_unds = 0
    sub(/^[[]/, "", modname)
    sub(/[]]$/, "", modname)
    mod_mods[modname] = 1
    while ((command | getline) == 1) {
	if (NF > 4) {
	    show_junk($0, 5)
	    continue
	}
	if ($2 ~ /^[Awat]$/) {
	    if ($3 ~ /^_?__crc_/) {
		sub(/^_?__crc_/, "", $3)
		#print_debug("r: systemmap, symbol: " $3)
		set_symbol($3, modname, "0x" $1, "", "", 0, own)
	    } else {
		#print_warn("wrong symbol " $3)
	    }
	    continue
	}
	if (!own)
	    continue
	if ($1 ~ /^[Uw]$/) {
	    if ($2 ~ /^_?__this_module$/) {
		delete mod_this[modname]
		continue
	    }
	    if (!((modname,$2) in mod_unds)) {
		mod_unds[modname,$2] = 1
		count_unds++
	    }
	    continue
	}
	if ($3 ~ /^_?init_module$/) {
	    mod_init[modname] = 1
	    print_debug("r: module " modname " has init_module")
	    continue
	}
	if ($3 ~ /^_?cleanup_module$/) {
	    mod_exit[modname] = 1
	    print_debug("r: module " modname " has cleanup_module")
	    continue
	}
	if ($3 ~ /^_?__this_module$/) {
	    mod_this[modname] = 1
	    print_debug("r: module " modname " has this_module")
	    continue
	}
	if ($3 ~ /^_?__mod_version[0-9]*$/) {
	    mod_vers[modname] = 1
	    print_debug("r: module " modname " has mod_version")
	    continue
	}
    }
    close(command)
    if (count_syms || count_unds)
	print_debug("r: systemmap, syms " count_syms ", unds " count_unds)
}
# read modpost cache file
function read_cachefile(file,    i,result)
{
    print_debug("r: cachefile, file = \"" file "\"")
    count_syms = 0; count_sets = 0
    while ((result = (getline < file)) == 1) {
	if (/:/) {
	    # new style cache file format
	    gsub(/:/, " ")
	    if ($1 == "sym") {
		if (NF<3) { print_error("syntax error in cachefile: line = \"" $0 "\""); continue }
		for (i=8;i>NF;i--) { $i = "" }; NF = 8
		set_symbol($2,$3,$4,$5,$6,$7,$8)
	    } else
	    if ($1 == "set") {
		if (NF<2) { print_error("syntax error in cachefile: line = \"" $0 "\""); continue }
		for (i=5;i>NF;i--) { $i = "" }; NF = 5
		set_symset($2,$3,$4,$5)
	    }
	} else
	if (/=/) {
	    # old style cache file format
	    sub(/=/, " ")
	    if ($1 ~ /^sym_/) {
		sub(/^sym_/, "", $1)
		if ($1 ~ /_name$/ ) { sub(/_name$/, "",$1); set_mod($1,$2) } else
		if ($1 ~ /_crc$/  ) { sub(/_crc$/,  "",$1); set_crc($1,$2) } else
		if ($1 ~ /_exp$/  ) { sub(/_exp$/,  "",$1); set_exp($1,$2) } else
		if ($1 ~ /_set$/  ) { sub(/_set$/,  "",$1); set_set($1,$2) } else
		if ($1 ~ /_pos$/  ) { sub(/_pos$/,  "",$1); set_pos($1,$2) } else
		if ($1 ~ /_owner$/) { sub(/_owner$/,"",$1); set_own($1,$2) }
	    } else
	    if ($1 ~ /^set_/) {
		sub(/^set_/, "", $1)
		if ($1 ~ /_hash$/ ) { sub(/_hash$/, "",$1); set_hash($1,$2) } else
		if ($1 ~ /_numb$/ ) { sub(/_numb$/, "",$1); set_numb($1,$2) } else
		if ($1 ~ /_owner$/) { sub(/_owner$/,"",$1); set_ownr($1,$2) }
	    }
	}
    }
    if (result != -1)
	close(file)
    #print_debug("result = " result)
    print_debug("r: cachefile, syms " count_syms ", sets " count_sets)
    return result
}
function write_cachefile(file,	    sym,set,line,count_syms,count_sets,written)
{
    count_syms = 0; count_sets = 0; written = 0
    print_debug("w: cachefile, file = \"" file "\"")
    if (1) {
	# new style cache file format
	for (sym in syms) {
	    line = "sym:" sym
	    line = line ":"; if (sym in mods) { line = line mods[sym] }
	    line = line ":"; if (sym in crcs) { line = line crcs[sym] }
	    line = line ":"; if (sym in exps) { line = line exps[sym] }
	    line = line ":"; if (sym in sets) { line = line sets[sym] }
	    line = line ":"; if (sym in posn) { line = line posn[sym] }
	    line = line ":"; if (sym in ownr) { line = line ownr[sym] }
	    print line > file; count_syms++; written++
	}
	for (set in setsets) {
	    line = "set:" set
	    line = line ":"; if (set in sethash) { line = line sethash[set] }
	    line = line ":"; if (set in setnumb) { line = line setnumb[set] }
	    line = line ":"; if (set in setownr) { line = line setownr[set] }
	    print line > file; count_sets++; written++
	}
    } else {
	# old style cache file format
	for (sym in mods   ) { count_syms++; written++; print "sym_" sym "_name="  mods[sym]    > file }
	for (sym in crcs   ) {               written++; print "sym_" sym "_crc="   crcs[sym]    > file }
	for (sym in exps   ) {               written++; print "sym_" sym "_exp="   exps[sym]    > file }
	for (sym in sets   ) {               written++; print "sym_" sym "_set="   sets[sym]    > file }
	for (sym in posn   ) {               written++; print "sym_" sym "_pos="   posn[sym]    > file }
	for (sym in ownr   ) {               written++; print "sym_" sym "_owner=" ownr[sym]    > file }
	for (set in sethash) { count_sets++; written++; print "set_" set "_hash="  sethash[set] > file }
	for (set in setnumb) {               written++; print "set_" set "_numb="  setnumb[set] > file }
	for (set in setownr) {               written++; print "set_" set "_owner=" setownr[set] > file }
    }
    if (written) close(file)
    print_debug("w: cachefile, syms " count_syms ", sets " count_sets)
    cache_dirty = 0
}
function read_dumpfiles_line()
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
    set_symbol($2, $3, $1, $4, "", 0, "")
}
function read_dumpfiles(command)
{
    print_debug("r: dumpfiles, command = \"" command "\"")
    count_syms = 0; count_unds = 0
    while ((command | getline) == 1)
	read_dumpfiles_line()
    close(command)
    print_debug("r: dumpfiles, syms " count_syms ", unds " count_unds)
}
function read_moduledir(directory)
{
    print_debug("r: moduledir, directory = \"" directory "\"")
    command = "find " directory " -type f -name '*.ko' | xargs objdump -t"
    read_modobject(command, "")
}
function read_mymodules(modules,    i, pair, ind, name, sym) {
    print_debug("r: mymodules")
    for (i = 1; i in modules; i++)
	print modules[i] > "modvers.list"
    close("modvers.list")
    command = "cat modvers.list | xargs objdump -t "
    read_modobject(command, "own")
    system("rm -f modvers.list")
    # double check
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	if (sym in mods)
	    mod_deps[name,mods[sym]] = 1
	if (!(sym in syms)) {
	    print_error("unresolved symbol " sym " in module " name);
	    continue
	}
	if (!(sym in crcs)) {
	    print_warn("symbol " sym " defined in module " mods[sym] " has no version")
	    continue
	}
    }
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
function read_symsets(path,	file,tar,fname,name,hash,list,n,symbols)
{
    file = path_symsets(path)
    if (!file) {
	print_warn("no symsets found in path")
	return 0
    }
    print_debug("unpacking " file)
    tar = "tar -tzf " file
    while ((tar | getline fname)) {
	if (fname~/\/$/) continue # skip directory
	set  = fname; sub(/\..*$/,"",set )
	hash = fname; sub(/^.*\./,"",hash);
	pos = 0; lineno = 0
	list = "tar -xzOf " file " " fname
	while ((list | getline)) {
	    lineno++; fnl = file ":" lineno; fnlp = fnl ": "
	    if (NF < 4) { missing(fnlp, 4); continue }
	    if (NF > 4) { garbage(fnlp, 4); NF = 4 }
	    if (NF < 4) { $4 = ""; NF = 4 }
	    if ($1~/^0x/) {
		pos++
		set_symbol($2, $3, $1, $4, set, pos, 0)
	    } else { unrecog(fnlp, $0); continue }
	}
	set_symset(set, hash, pos, "kabi")
    }
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
# read and process system Module.symvers file
function read_symvers(path,	result,file,fnl,lineno,fnlp)
{
    file = path_symvers(path)
    if (!file) {
	print_warn("no symvers found in path")
	return 0
    }
    count_syms = 0
    print_debug("r: syssymver, file = \"" file "\"")
    while ((result = (getline < file)) == 1) {
	lineno++; fnl = file ":" lineno; fnlp = fnl ": "
	if (NF < 4) { missing(fnlp, 4); continue }
	if (NF > 4) { garbage(fnlp, 4); NF = 4 }
	if (NF < 4) { $4 = ""; NF = 4 }
	# try to find a symbol set that contains the symbol
	if ($1~/^0x/) {
	    if ($2 in sets) {
		set_symbol($2, $3, $1, $4, sets[$2], posn[$2], ownr[$2])
		continue
	    }
	    # make a new set
	    set = "kernel/" $3; gsub(/\//, "_", set)
	    pos = 0; if (set in setnumb) { pos = setnumb[set] }; pos++
	    set_symbol($2, $3, $1, $4, set, pos, "kernel")
	    setnumb[set] = pos
	} else { unrecog(fnlp, $0); continue }
    }
    print_debug("w: syssymver, syms " count_syms)
    if (result == -1) return 0
    close(file)
    return 1
}
function write_syssymver(file,    sym,line,count_syms)
{
    print_debug("w: syssymver, file = \"" file "\"")
    count_syms = 0
    for (sym in syms) {
	if (sym in ownr && ownr[sym])
	    continue
	if (!(sym in crcs)) {
	    print_error("no crc for symbol: " sym)
	    continue
	}
	line = crcs[sym] "\t" sym "\t" mods[sym]
	if (values["exportsyms"]) {
	    if (!(sym in exps)) {
		print_error("no export for symbol: " sym)
		continue
	    }
	    line = line "\t" exps[sym]
	}
	count_syms++
	print line > file
    }
    if (count_syms)
	close(file)
    print_debug("w: syssymver, syms " count_syms)
}
function write_modsymver(file,    sym, line, count_syms)
{
    print_debug("w: modsymver, file = \"" file "\"")
    count_syms = 0
    for (sym in ownr) {
	if (!ownr[sym]) continue
	if (!(sym in crcs)) {
	    print_warn("no crc for symbol: " sym)
	    continue
	}
	line = crcs[sym] "\t" sym "\t" mods[sym]
	if (values["exportsyms"]) {
	    if (!(sym in exps)) {
		print_warn("no export for symbol: " sym)
		continue
	    }
	    line = line "\t" exps[sym]
	}
	count_syms++
	print line > file
    }
    if (count_syms)
	close(file)
    print_debug("w: modsymver, syms " count_syms)
}
function write_header(file, modname, basename)
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
#define KBUILD_MODNAME " basename "\n\
\n\
#ifdef KBUILD_MODNAME\n\
struct module __this_module\n\
__attribute__((section(\".gnu.linkonce.this_module\"))) = {\n\
	.name = __stringify(KBUILD_MODNAME),\
" > file
	if (modname in mod_init)
	    print "\t.init = init_module," > file
	if (modname in mod_exit)
	    print "#ifdef CONFIG_MODULE_UNLOAD\n\t.exit = cleanup_module,\n#endif" > file
	print "};\n#endif" > file
}
function write_modversions(file, modname, basename,	count, pair, ind, name, sym)
{
    count = 0
    print "\n\
#ifdef CONFIG_MODVERSIONS\n\
static const struct modversion_info ____versions[]\n\
__attribute_used__\n\
__attribute__((section(\"__versions\"))) = {\
" > file
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	if (name != modname) continue
	if (!(sym in crcs)) continue
	print "\t{ " crcs[sym] ", \"" sym "\" }," > file
	count ++
    }
    print "};\n#endif" > file
    if (count) print_debug("wrote " count " symbol versions")
}
function write_dependencies(file, modname, basename,	count, deps, sep, pair, ind, name, dep) {
    count = 0; deps = ""; sep = ""
    for (pair in mod_deps) {
	split(pair, ind, SUBSEP); name = ind[1]; dep = ind[2]
	if (name != modname) continue
	sub(/.*\//, "", dep)
	if (dep == "vmlinux" || dep == "") continue
	deps = deps sep dep; sep = ","; count++
    }
    if (deps) {
	print_debug("wrote " count " dependencies")
	print "\n\
static const char __module_depends[]\n\
__attribute_used__\n\
__attribute__((section(\".modinfo\"))) =\n\
\"depends=" deps "\";\
" > file
    }
}
function write_srcversion(file, modname, basename,	srcversion)
{
    srcversion = "42e8e4e40dfa7590147e23a5b9a3105d"
    # first step, see if there are any __mod_version symbols
    reason = ""
    if (modname in mod_vers)
	reason = "version"
    else if (values["allsrcversion"])
	reason = "all"
    else return
    # the tricky way to find sources is to use the dependencies; however,
    # this means that we must enable dependency tracking when building rpms
    srcbase = basename
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
function write_mymodules(modules,    i, basename, file, modname)
{
    files = 0
    print_debug("w: mymodules")
    for (i = 1; i in modules; i++) {
	basename = modules[i]
	sub(/(\.mod)?\.(k)?o(\.gz)?$/, "", basename)
	#sub(/^.*\//, "", basename)
	file = basename ".mod.c"
	sub(/^.*\//, "", basename)
	modname = values["pkgdirectory"] "/" basename
	files++
	if (values["verbose"] == 1 && values["debug"] == 0)
	    print "  GEN    " basename ".mod.c"
	print_debug("writing " file)
	write_header(file, modname, basename)
	if (values["modversions"])
	    write_modversions(file, modname, basename)
	write_dependencies(file, modname, basename)
	write_srcversion(file, modname, basename)
	close(file)
    }
    print_debug("w: mymodules, " files " files written")
}
BEGIN {
    LINT = "yes"
    me = "modpost.awk"
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
    longopts["modules"      ] = "M:" ;																		  descrips["modules"      ] = "modules for which to generate symbols"
    longopts["cachefile"    ] = "c:" ; environs["cachefile"    ] = "MODPOST_CACHE"		 ; defaults["cachefile"    ] = "modpost.cache"					; descrips["cachefile"    ] = "where to cache system smybol versions"
    longopts["kversion"     ] = "k:" ; environs["kversion"     ] = "kversion"			 ; defaults["kversion"     ] = kversion						; descrips["kversion"     ] = "kernel version KVERSION"
    longopts["moddir"       ] = "d:" ; environs["moddir"       ] = "MODPOST_MODDIR"		 ; defaults["moddir"       ] = "/lib/modules/" kversion				; descrips["moddir"       ] = "directory containing modules for which to generate symbols"
    longopts["filename"     ] = "F:" ; environs["filename"     ] = "MODPOST_SYSMAP"		 ; defaults["filename"     ] = "/boot/System.map-" kversion			; descrips["filename"     ] = "system map file"
    longopts["infile"       ] = "i:" ; environs["infile"       ] = "MODPOST_INPUTS"		 ; defaults["infile"       ] = "/lib/modules/" kversion "/build/Module.symvers"	; descrips["infile"       ] = "input file (space separated)"
    longopts["outfile"      ] = "o:" ; environs["outfile"      ] = "MODPOST_MODVER"		 ; defaults["outfile"      ] = "Module.symvers"					; descrips["outfile"      ] = "output file for module symbols"
    longopts["sysfile"      ] = "s:" ; environs["sysfile"      ] = "MODPOST_SYSVER"		 ; defaults["sysfile"      ] = "System.symvers"					; descrips["sysfile"      ] = "output file for system symbols"
    longopts["unload"       ] = "u"  ; environs["unload"       ] = "CONFIG_MODULE_UNLOAD"	 ; defaults["unload"       ] = 0						; descrips["unload"       ] = "module unloading is supported"
    longopts["modversions"  ] = "m"  ; environs["modversions"  ] = "CONFIG_MODVERSIONS"		 ; defaults["modversions"  ] = 0						; descrips["modversions"  ] = "module versions are supported"
    longopts["allsrcversion"] = "a"  ; environs["allsrcversion"] = "CONFIG_MODULE_SRCVERSION_ALL"; defaults["allsrcversion"] = 0						; descrips["allsrcversion"] = "source version all modules"
    longopts["exportsyms"   ] = "x"  ;								   defaults["exportsyms"   ] = 0						; descrips["exportsyms"   ] = "place export symbols in versions files"
    longopts["pkgdirectory" ] = "p:" ; environs["pkgdirectory" ] = "PACKAGE_LCNAME"		 ; defaults["pkgdirectory" ] = "openss7"					; descrips["pkgdirectory" ] = "subdirectory for module"
    longopts["dryrun"       ] = "n"  ;								   defaults["dryrun"       ] = 0						; descrips["dryrun"       ] = "don't perform actions, just check them"
    longopts["dry-run"      ] = "n"  ;								   defaults["dry-run"      ] = 0						; descrips["dry-run"      ] = "don't perform actions, just check them"
    longopts["quiet"        ] = "q"  ;								   defaults["quiet"        ] = 0						; descrips["quiet"        ] = "suppress normal output"
    longopts["silent"       ] = "q"  ;								   defaults["silent"       ] = 0						; descrips["silent"       ] = "suppress normal output"
    longopts["debug"        ] = "D::";								   defaults["debug"        ] = 1						; descrips["debug"        ] = "increase or set debug level DEBUG"
    longopts["verbose"      ] = "v::";								   defaults["verbose"      ] = 1						; descrips["verbose"      ] = "increase or set verbosity level VERBOSITY"
    longopts["help"         ] = "h"  ;																		  descrips["help"         ] = "display this usage information and exit"
    longopts["version"      ] = "V"  ;																		  descrips["version"      ] = "display script version and exit"
    longopts["copying"      ] = "C"  ;																		  descrips["copying"      ] = "display coding permissions and exit"
    # mark options that must default to environment or default setting
    values["cachefile"    ] = ""
    # set mandatory defaults
    values["pkgdirectory" ] = defaults["pkgdirectory" ]
    values["unload"       ] = defaults["unload"       ]
    values["modversions"  ] = defaults["modversions"  ]
    values["allsrcversion"] = defaults["allsrcversion"]
    values["exportsyms"   ] = defaults["exportsyms"   ]
    values["debug"        ] = defaults["debug"        ]
    values["verbose"      ] = defaults["verbose"      ]
    values["quiet"        ] = defaults["quiet"        ]
    if ("V" in ENVIRON && ENVIRON["V"] == 1) {
	values["quiet"  ] = 0
	values["verbose"] = 2
    }
    optstring = "M:c:k:d:F:i:o:s:umaxp:nqD::v::hVC"
    optind = 0
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	if (c == -1) break
	else if (c ~ /[MckdFiosp]/) { values[option] = optarg }
	else if (c ~ /[umaxnq]/)    { values[option] = 1 }
	else if (c == "D") { if (optarg) { values["debug"  ] = optarg } else { values["debug"  ]++ } }
	else if (c == "v") { if (optarg) { values["verbose"] = optarg } else { values["verbose"]++ } }
	else if (c == "h") { help(   stdout); exit 0 }
	else if (c == "V") { version(stdout); exit 0 }
	else if (c == "C") { copying(stdout); exit 0 }
	else               { usage(  stderr); exit 2 }
    }
    if (optind < ARGC) {
	values["modules"] = ARGV[optind]; optind++
	while (optind < ARGC) {
	    values["modules"] = values["modules"] " " ARGV[optind]
	    optind++
	}
    }
    for (i=1;ARGC>i;i++) { delete ARGV[i] }
    for (value in values) {
	if (!values[value] && (value in environs) && (environs[value] in ENVIRON) && ENVIRON[environs[value]]) {
	    print_debug("o: assigning value for " value " from environment " environs[value])
	    values[value] = ENVIRON[environs[value]]
	}
	if (!values[value] && (value in defaults) && defaults[value]) {
	    print_debug("o: assigning value for " value " from default " defaults[value])
	    values[value] = defaults[value]
	}
    }
    cache_dirty = 1
    if (("cachefile" in values) && values["cachefile"] && (read_cachefile(values["cachefile"]) != -1))
	cache_dirty = 0
    else {
	if (("filename" in values) && values["filename"])
	    read_systemmap(cat_command(values["filename"]), "vmlinux", "")
	if (("infile" in values) && values["infile"])
	    read_dumpfiles(cat_command(values["infile"]))
	if (("moddir" in values) && values["moddir"])
	    read_moduledir(values["moddir"])
    }
    if (("modules" in values) && values["modules"]) {
	split(values["modules"], modules)
	read_mymodules(modules)
    }
    # write out cache file
    if (("cachefile" in values) && values["cachefile"] && cache_dirty)
	write_cachefile(values["cachefile"])
    if (("sysfile" in values) && values["sysfile"])
	write_syssymver(values["sysfile"])
    if (("modules" in values) && values["modules"]) {
	if (("outfile" in values) && values["outfile"])
	    write_modsymver(values["outfile"])
	write_mymodules(modules)
    }
    exit 0
}
END {
    for (file in written)
	close(file)
}

# =============================================================================
#
# $Log: modpost.awk,v $
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
