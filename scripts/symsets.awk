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
  $Id: symsets.awk,v 1.1.2.1 2011-03-17 07:01:22 brian Exp $\n\
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
  INPUT ...\n\
      Module.symvers file for which to generate symsets\
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
# read modpost cache file
function read_cachefile(file,	i,result)
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
# read and process package Module.symvers file
function read_inputs(inputs,	    k, input, filename, lineno, fnl, fnlp, nothing)
{
    print_debug("reading inputs...")
    nothing = 1
    for (k in inputs) {
	input = inputs[k]
	if (system("test -r " input) != 0) {
	    print_error("cannot read file " input);
	    continue
	}
	nothing = 0
	print_debug("r: inputs, file = \"" input "\"")
	filename = input
	lineno = 0
	while ((getline < input) == 1) {
	    lineno++; fnl = filename ":" lineno; fnlp = fnl ": "
	    if (NF < 4) { missing(fnlp, 4); continue }
	    if (NF > 4) { garbage(fnlp, 4); NF = 4 }
	    if ($1~/^0x/) {
		set = $3; gsub(/\//, "_", set)
		if (set in symsets) {
		    symsets[set] = symsets[set] " " $2
		    crcsets[set] = crcsets[set] $1
		} else {
		    symsets[set] = $2
		    crcsets[set] = $1
		}
		# Note that SuSE includes EXPORT type in the
		# kABI, whereas RedHat does not.
		if (values["suse"] == 1) {
		    syms[$2] = $1 "\t" $2 "\t" $3 "\t" $4
		} else {
		    syms[$2] = $1 "\t" $2 "\t" $3
		}
		sets[$2] = set
	    } else { unrecog(fnlp, $0); continue }
	}
	close(input)
    }
    if (nothing) {
	print_error("no readable input files")
	exit 1
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
# output symsets file
function write_outputs(outfile,		directory,tarball,direname,basename,chngdir,symset,hash,file,n,sym)
{
    directory = outfile
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
	    if ((symset in sethash) && (hash != sethash[symset])) {
		print_error("symset " symset " hash NOT CHANGED from " sethash[symset] " to " hash)
		hash = sethash[symset]
	    }
	    print_debug("hash is: '" hash "'")
	} else {
	    hash = "tmp"
	}
	print_debug("removing " directory "/" symset ".*")
	system_command("rm -f -- " directory "/" symset ".*")
	file = directory "/" symset "." hash
	print_debug("creating " file)
	delete set_temp
	for (sym in set_syms) { set_temp[set_syms[sym]] = sym }
	for (sym in set_temp) {
	    if (values["suse"] == 1)
		print crcs[sym] "\t" sym "\t" mods[sym] "\t" exps[sym] > file
	    else
		print crcs[sym] "\t" sym "\t" mods[sym] > file
	}
	close(file)
	if (hash == "tmp") {
	    hash = getline_command("md5sum " file)
	    hash = substr(hash, 1, 16)
	    if ((symset in sethash) && (hash != sethash[symset])) {
		print_warn("symset " symset " hash NOT CHANGED from " sethash[symset] " to " hash)
		hash = sethash[symset]
	    }
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
}
BEGIN {
    LINT = "yes"
    me = "symsets.awk"
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
    longopts["inputs"       ] = "I:" ; environs["inputs"        ] = "MODPOST_MODVER"  ; defaults["inputs"       ] = "Module.symvers"			; descrips["inputs"       ] = "input Module.symvers file"
    longopts["sysver"       ] = "S:" ; environs["sysver"        ] = "MODPOST_SYSVER"  ; defaults["sysver"       ] = "System.symvers"			; descrips["sysver"       ] = "input System.symvers file"
    longopts["sysset"       ] = "M:" ; environs["sysset"        ] = "ksymsets"        ; defaults["sysset"       ] = "/boot/symsets-" kversion ".tar.gz"	; descrips["sysset"       ] = "input system symsets file"
    longopts["suse"         ] = "s"  ;							defaults["suse"         ] = 0					; descrips["suse"         ] = "output symsets in SuSE format"
    longopts["redhat"       ] = "r"  ;							defaults["redhat"       ] = 1					; descrips["redhat"       ] = "output symsets in RedHat format"
    longopts["missing"      ] = "m"  ;							defaults["missing"      ] = 0					; descrips["missing"      ] = "create missing system symsets"
    longopts["outfile"      ] = "o:" ; environs["outfile"       ] = "MODPOST_SYMSETS" ; defaults["outfile"      ] = "symsets-" kversion ".tar.gz"	; descrips["sysset"       ] = "output file name OUTFILE"
    longopts["dryrun"       ] = "n"  ;							defaults["dryrun"       ] = 0					; descrips["dryrun"       ] = "don't perform actions, just check them"
    longopts["dry-run"      ] = "n"  ;							defaults["dry-run"      ] = 0					; descrips["dry-run"      ] = "don't perform actions, just check them"
    longopts["quiet"        ] = "q"  ;							defaults["quiet"        ] = 0					; descrips["quiet"        ] = "suppress normal output"
    longopts["silent"       ] = "q"  ;							defaults["silent"       ] = 0					; descrips["silent"       ] = "suppress normal output"
    longopts["debug"        ] = "D::";							defaults["debug"        ] = 1					; descrips["debug"        ] = "increase or set debug level DEBUG"
    longopts["verbose"      ] = "v::";							defaults["verbose"      ] = 1					; descrips["verbose"      ] = "increase or set verbosity level VERBOSITY"
    longopts["help"         ] = "h"  ;															  descrips["help"         ] = "display this usage information and exit"
    longopts["version"      ] = "V"  ;															  descrips["version"      ] = "display script version and exit"
    longopts["copying"      ] = "C"  ;															  descrips["copying"      ] = "display coding permissions and exit"
    # mark options that must default to environment or default setting
    values["cachefile"    ] = ""
    # set mandatory defaults
    values["suse"         ] = defaults["suse"    ]
    values["redhat"       ] = defaults["redhat"  ]
    values["missing"      ] = defaults["missing" ]
    values["kversion"     ] = defaults["kversion"]
    values["inputs"       ] = defaults["inputs"  ]
    values["sysver"       ] = defaults["sysver"  ]
    values["outfile"      ] = defaults["outfile" ]
    values["debug"        ] = defaults["debug"   ]
    values["verbose"      ] = defaults["verbose" ]
    values["quiet"        ] = defaults["quiet"   ]
    if ("V" in ENVIRON && ENVIRON["V"] == 1) {
	values["quiet"  ] = 0
	values["verbose"] = 2
    }
    optstring = "c:k:I:S:M:srmo:nqD::v::hVC"
    optind = 0
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	if (c == -1) break
	else if (c ~ /[ckISMo]/) { values[option] = optarg }
	else if (c ~ /[srmnq]/)  { values[option] = 1 }
	else if (c == "D") { if (optarg) { values["debug"  ] = optarg } else { values["debug"  ]++ } }
	else if (c == "v") { if (optarg) { values["verbose"] = optarg } else { values["verbose"]++ } }
	else if (c == "h") { help(   stdout); exit 0 }
	else if (c == "V") { version(stdout); exit 0 }
	else if (c == "C") { copying(stdout); exit 0 }
	else               { usage(  stderr); exit 2 }
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
	    print_debug("o: assigning value for " value " from environment " environs[value])
	    values[value] = ENVIRON[environs[value]]
	}
	if (!values[value] && (value in defaults) && defaults[value]) {
	    print_debug("o: assigning value for " value " from default " defaults[value])
	    values[value] = defaults[value]
	}
    }
    if (values["suse"] == 1) { values["redhat"] = 0 }
    kversion = values["kversion"]
    if (!values["inputs"] || values["inputs"] == "-") {
	print_debug("no inputs, using " stdin)
	values["inputs"] = stdin
    }
    # read in cachefile
    cache_dirty = 1
    if (("cachefile" in values) && values["cachefile"] && (read_cachefile(values["cachefile"]) != -1))
	cache_dirty = 0
    split(values["inputs"], inputs)
    read_inputs(inputs)
    write_outputs(values["outfile"])
    # write out cachefile when necessary
    if (("cachefile" in values) && values["cachefile"] && cache_dirty)
	write_cachefile(values["cachefile"])
    exit 0
}
END {
    for (file in written)
	close(file)
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
