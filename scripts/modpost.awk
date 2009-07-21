#!/usr/bin/awk -f
# =============================================================================
#
# @(#) $RCSfile: modpost.awk,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-07-21 11:06:21 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2009-07-21 11:06:21 $ by $Author: brian $
#
# =============================================================================

#
# This is an awk program to generate the modpost.cache cache file.  The inputs
# consists of the sysmap file, the modules directory, specific modpost input
# files, and a sysver file.

function date(format) {
    if (format) {
	("date +\"" format "\"") | getline
	close("date +\"" format "\"")
    } else {
	"date" | getline
	close("date")
    }
    return $0
}
function year()
{
    return date("%Y")
}
function allyears(    year, this, last, sep, result)
{
    last = year()
    for (this = 2009; this <= last; this++) {
	result = result sep this
	sep = ", "
    }
    return result
}
function print_debug(string)
{
    if (values["debug"] > 0)
	print "modpost.awk: D: " string > "/dev/stderr"
}
function print_error(string)
{
    print "modpost.awk: E: " string > "/dev/stderr"
}
function print_warn(string)
{
    if (values["verbose"] > 0 || values["debug"] > 0)
	print "modpost.awk: W: " string > "/dev/stderr"
}
function usage(output)
{
    if (values["quiet"])
	return
    print "\
modpost:\n\
  $Id: modpost.awk,v 1.1.2.1 2009-07-21 11:06:21 brian Exp $\n\
Usage:\n\
  modpost [options] [MODULE ...]\n\
  modpost -h\n\
  modpost -V\n\
  modpost -C\
" >> output
}
function help(output)
{
    if (values["quiet"])
	return
    usage(output)
    print "\
Arguments:\n\
  MODULE ...\n\
      modules for which to generate symbols\n\
      [default: " defaults["modules"] "]\n\
Options:\n\
  -M, --modules MODULE ...\n\
      modules for which to generate symbols\n\
      [default: " defaults["modules"] "]\n\
  -c, --cachefile CACHEFILE\n\
      where to cache system symbol versions\n\
      [default: " defaults["cachefile"] "]\n\
  -d, --moddir MODDIR\n\
      module directory\n\
      [default: " defaults["moddir"] "]\n\
  -F, --filename SYSMAP\n\
      system map file\n\
      [default: " defaults["filename"] "]\n\
  -i, --infile INFILE\n\
      input file (space separated list)\n\
      [default: " defaults["infile"] "]\n\
  -o, --outfile OUTFILE\n\
      output file for module symbols\n\
      [default: " defaults["outfile"] "]\n\
  -s, --sysfile SYSFILE\n\
      output file for system symbols\n\
      [default: " defaults["sysfile"] "]\n\
  -u, --unload\n\
      module unloading is supported\n\
      [default: " defaults["unload"] "]\n\
  -m, --modversions\n\
      module versions are supported\n\
      [default: " defaults["modversions"] "]\n\
  -a, --allsrcversion\n\
      source version all modules\n\
      [default: " defaults["allsrcversion"] "]\n\
  -x, --exportsyms\n\
      place export symbols in versions files\n\
      [default: " defaults["exportsyms"] "]\n\
  -p, --pkgdirectory SUBDIRECTORY\n\
      subdirectory for module\n\
      [default: " defaults["pkgdirector"] "]\n\
  -n, --dryrun, --dry-run\n\
      don't perform the actions, just check them\n\
  -q, --quiet, --silent\n\
      suppress normal output\n\
  -D, --debug [LEVEL]\n\
      increase or set debugging verbsosity\n\
      [default: " defaults["debug"] "]\n\
  -v, --verbose [LEVEL]\n\
      increase or set output verbosity\n\
      [default: " defaults["verbose"] "]\n\
  -h, --help\n\
      prints this usage information and exist\n\
  -V, --version\n\
      prints the version and exit\n\
  -C, --copying\n\
      prints copying permissions and exist\
" >> output
}
function version(output)
{
    if (values["quiet"])
	return
    print "\
Version 2.1\n\
$Id: modpost.awk,v 1.1.2.1 2009-07-21 11:06:21 brian Exp $\n\
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
See modpost.awk -- --copying' for copying permissions.\
" >> output
}
function copying(output)
{
    if (values["quiet"])
	return
    print "\
--------------------------------------------------------------------------------\n\
$Id: modpost.awk,v 1.1.2.1 2009-07-21 11:06:21 brian Exp $\n\
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
" >> output
}
function getopt_long(argc, argv, optstring, longopts, longindex,    pos, needarg, wantarg)
{
    option = ""; optval = ""; optarg = ""
    if (optind == 0) { optind = 1; more = "" }
    while ((optind < argc) || (more)) {
	if (more) { arg = "-" more; more = "" }
	else { arg = argv[optind]; optind++ }
	if (arg ~ /^--?[a-zA-Z0-9]/) {
	    if (needarg) {
		print_error("option -" optval " requires an argument")
		usage("/dev/stderr")
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
		    usage("/dev/stderr")
		    exit 2
		}
		if (longopts[option] !~ /:/) {
		    print_error("option --" option " does not take an argument")
		    usage("/dev/stderr")
		    exit 2
		}
		optval = substr(longopts[option], 1, 1)
		return optval
	    }
	    if (arg ~ /^--[a-zA-Z0-9][-_a-zA-Z0-9]*$/) {
		option = arg; sub(/^--/, "", option)
		if (!(option in longopts)) {
		    print_error("option --" option " not recognized")
		    usage("/dev/stderr")
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
		if ((pos = index(optstring, optval)) == 0) {
		    print_error("option -" optval " not recognized")
		    usage("/dev/stderr")
		    exit 2
		}
		if (substr(optstring, pos+1, 1) == ":") {
		    if (length(arg) > 2) {
			if (substr(optstring, pos+2, 1) == ":") {
			    if ((more = substr(arg, 3)) && more !~ /^[a-zA-Z0-9]/) {
				print_error("bad option sequence " arg)
				usage("/dev/stderr")
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
		if ((more = substr(arg, 3)) && more !~ /^[a-zA-Z0-9]/) {
		    print_error("bad option sequence " arg)
		    usage("/dev/stderr")
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
}
function show_junk(line, position)
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
function set_sym(name, sym, mymodule)
{
    if (!(sym in syms))
	count_syms++
    mod_mods[name] = 1
    mod_syms[name,sym] = 1
    if (sym in mods)
	if (mods[sym] != name)
	    print_warn("module for " sym " change from " mods[sym] " to " name)
    mods[sym] = name
    syms[sym] = 1
    if (mymodule)
	mine[sym] = 1
}
function set_crc(name, sym, crc)
{
    if (!(sym in crcs))
	count_crcs++
    if (crc ~ /0x00000000[[:xdigit:]]+/)
	sub(/00000000/, "", crc)
    if ((name,sym) in mod_crcs)
	if (mod_crcs[name,sym] != crc)
	    print_warn("crc for " name ":" sym " changed from " mod_crcs[name,sym] " to " crc)
    mod_crcs[name,sym] = crc
    crcs[sym] = crc
    cache_dirty = 1
}
function set_exp(name, sym, expt)
{
    if (!(sym in exps))
	count_exps++
    if ((name,sym) in mod_exps)
	if (mod_exps[name,sym] != expt)
	    print_warn("export for " name ":" sym " changed from " mod_exps[name,sym] " to " expt)
    mod_exps[name,sym] = expt
    exps[sym] = expt
    cache_dirty = 1
}
#
# Read in an objdump -t listing.  This must be from objdump -t run on a kernel
# object.  This will add EXPORT_SYMBOL or EXPORT_SYMBOL_GPL for use in the
# Module.symvers file.
#
function read_modobject(command, mymodule)
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
		if (mymodule) {
		    sub(/.*\//, "")
		    $0 = values["pkgdirectory"] "/" $0
		} else {
		    sub(values["moddir"] "/", "")
		    sub(/[^\/]*\//, "")
		}
		modname = $0
		continue
	    }
	    if (/SYMBOL TABLE:/) {
		count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
		mod_mods[modname] = 1
		HEADER = 0
		if (progress >= 200) {
		    print_debug("r: ko object, " files " kernel modules...")
		    progress = 0
		}
		files++
		progress++
		continue
	    }
	}
	if (/^$/ && !HEADER) {
	    HEADER = 1
	    if (count_syms || count_crcs || count_exps || count_unds) {
		print_debug("r: ko object, " modname)
		print_debug("r: ko object, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
	    }
	    continue
	}
	if ($2 == "l") {
	    if ((NF >= 6) && ($3 == "O")) {
		if ($4 = ".modinfo") {
		    if ($6 ~ /^_?__mod_version[0-9]*$/)
			mod_vers[modname] = 1
		    continue
		}
		if (sub(/^_?__ksymtab/, "", $4)) {
		    if (sub(/^_?__ksymtab_/, "", $6)) {
			set_sym(modname,$6, mymodule)
			set_exp(modname,$6, "EXPORT_SYMBOL" toupper($4))
		    }
		}
	    }
	    continue
	}
	if ($2 == "g") {
	    if ($3 == "*ABS*") {
		if ((NF >= 5) && ($5 ~ /^_?__crc_/)) {
		    if (sub(/^_?__crc_/, "", $5)) {
			set_sym(modname,$5,mymodule)
			set_crc(modname,$5,"0x" $1)
		    }
		}
	    }
	    continue
	}
	if (!mymodule)
	    continue
	undef = ""
	if (($2 == "w") && ($3 == "*UND*") && (NF == 5)) undef = $5
	if (($2 == "*UND*") && (NF == 4)) undef = $4
	if (undef) {
	    if (undef ~ /^_?__this_module$/) {
		delete mod_this[modname]
		continue
	    }
	    if (!((modname,undef) in mod_unds)) {
		mod_unds[modname,undef] = 1
		count_unds++
	    }
	    continue
	}
	if ($0 ~ /\<_?init_module$/) mod_init[modname] = 1
	if ($0 ~ /\<_?cleanup_module$/) mod_exit[modname] = 1
	if ($0 ~ /\<_?__this_module$/) mod_this[modname] = 1
	if ($0 ~ /\<_?__mod_version[0-9]*$/) mod_vers[modname] = 1
    }
    close(command)
    print_debug("r: ko object, " files " kernel modules")
}
#
# Read in any nm -Bs listing.  This can be a System.map file, a Modules.map
# file or nm -Bz run an a kernel object.
#
function read_systemmap(command, modname, mymodule)
{
    #print_debug("r: systemmap, command = \"" command "\"")
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    sub(/^[[]/, "", modname)
    sub(/[]]$/, "", modname)
    mod_mods[modname] = 1
    while ((command | getline) == 1) {
	if (NF > 4) {
	    show_junk($0, 5)
	    continue
	}
	if ($2 ~ /^[Awat]$/) {
	    if (sub(/^_?__crc_/, "", $3)) {
		#print_debug("r: systemmap, symbol: " $3)
		set_sym(modname,$3, mymodule)
		set_crc(modname,$3,"0x" $1)
	    } else {
		#print_warn("wrong symbol " $3)
	    }
	    continue
	}
	if (!mymodule)
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
	    continue
	}
	if ($3 ~ /^_?cleanup_module$/) {
	    mod_exit[modname] = 1
	    continue
	}
	if ($3 ~ /^_?__this_module$/) {
	    mod_this[modname] = 1
	    continue
	}
	if ($3 ~ /^_?__mod_version[0-9]*$/) {
	    mod_vers[modname] = 1
	    continue
	}
    }
    close(command)
    if (count_syms || count_crcs || count_exps || count_unds)
	print_debug("r: systemmap, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
}
function read_cachefile(file,    result) {
    print_debug("r: cachefile, file = \"" file "\"")
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    while ((result = (getline < file)) == 1) {
	sub(/=/, " ")
	sub(/^sym_/, "", $1)
	if (sub(/_name$/, "", $1)) {
	    set_sym($2,$1)
	    if ($1 in crcs)
		set_crc($2,$1,crcs[$1])
	    if ($1 in exps)
		set_exp($2,$1,exps[$1])
	    continue
	}
	if (sub(/_crc$/, "", $1)) {
	    if ($1 in mods) {
		set_sym(mods[$1],$1)
		set_crc(mods[$1],$1,$2)
	    }
	    else {
		syms[$1] = 1
		crcs[$1] = $2
	    }
	    continue
	}
	if (sub(/_exp$/, "", $1)) {
	    if ($1 in mods) {
		set_sym(mods[$1],$1)
		set_exp(mods[$1],$1,$2)
	    } else {
		syms[$1] = 1
		exps[$1] = $2
	    }
	    continue
	}
    }
    close(file)
    #print_debug("result = " result)
    print_debug("r: cachefile, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
    return result
}
function write_cachefile(file)
{
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    print_debug("w: cachefile, file = \"" file "\"")
    print "" > file
    for (sym in mods) {
	count_syms++
	print "sym_" sym "_name=" mods[sym] >> file
    }
    for (sym in crcs) {
	count_crcs++
	print "sym_" sym "_crc="  crcs[sym] >> file
    }
    for (sym in exps) {
	count_exps++
	print "sym_" sym "_exp="  exps[sym] >> file
    }
    cache_dirty = 0
    print_debug("w: cachefile, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
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
    if (NF < 3)
	$3 = "vmlinux"
    set_sym($3,$2)
    set_crc($3,$2,$1)
    if (NF > 3)
	set_exp($3,$2,$4)
}
function read_dumpfiles(command)
{
    print_debug("r: dumpfiles, command = \"" command "\"")
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    while ((command | getline) == 1)
	read_dumpfiles_line()
    close(command)
    print_debug("r: dumpfiles, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
}
function read_moduledir(directory)
{
    print_debug("r: moduledir, directory = \"" directory "\"")
    command = "find " values["moddir"] " -type f -name '*.ko' | xargs objdump -t"
    read_modobject(command, 0)
}
function read_mymodules(modules,    i, pair, ind, name, sym) {
    print_debug("r: mymodules")
    for (i = 1; i in modules; i++)
	print modules[i] > "modvers.list"
    close("modvers.list")
    command = "cat modvers.list | xargs objdump -t "
    read_modobject(command, 1)
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
function write_syssymver(file)
{
    print_debug("w: syssymver, file = \"" file "\"")
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    for (sym in syms) {
	if (sym in mine)
	    continue
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
	    count_exps++
	}
	count_syms++
	count_crcs++
	print line > file
    }
    print_debug("w: syssymver, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
}
function write_modsymver(file)
{
    print_debug("w: modsymver, file = \"" file "\"")
    count_syms = 0; count_crcs = 0; count_exps = 0; count_unds = 0
    for (sym in mine) {
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
	    count_exps++
	}
	count_syms++
	count_crcs++
	print line > file
    }
    print_debug("w: modsymver, syms " count_syms ", crcs " count_crcs ", exps " count_exps ", unds " count_unds)
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
#define KBUILD_MODNAME " basename "\n\
\n\
#ifdef KBUILD_MODNAME\n\
struct module __this_module\n\
__attribute__((section(\".gnu.linkonce.this_module\"))) = {\n\
	.name = __stringify(KBUILD_MODNAME),\
" > file
	if (modname in mod_init)
	    print "\t.init = init_module," >> file
	if (modname in mod_exit)
	    print "#ifdef CONFIG_MODULE_UNLOAD\n\t.exit = cleanup_module,\n#endif" >> file
	print "};\n#endif" >> file
}
function write_modversions(file, modname, basename,	count, pair, ind, name, sym)
{
    count = 0
    print "\n\
#ifdef CONFIG_MODVERSIONS\n\
static const struct modversion_info ____versions[]\n\
__attribute_used__\n\
__attribute__((section(\"__versions\"))) = {\
" >> file
    for (pair in mod_unds) {
	split(pair, ind, SUBSEP); name = ind[1]; sym = ind[2]
	if (name != modname) continue
	if (!(sym in crcs)) continue
	print "\t{ " crcs[sym] ", \"" sym "\" }," >> file
	count ++
    }
    print "};\n#endif" >> file
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
" >> file
    }
}
function write_srcversion(file, modname, basename,	srcversion)
{
    srcversion = "42e8e4e40dfa7590147e23a5b9a3105d"
    # first step, see if there are any __mod_version symbols
    reason = ""
    if (modname in mod_vers) reason = "version"
    else if (values["allsrcversion"]) reason = "all"
    else return
    # the tricky way to find sources is to use the dependencies; however,
    # this means that we must enable dependency tracking when building rpms
    srcbase = basename
    sub(/^streams[-_]/, "", srcbase)
    command = "find .deps -name 'lib*" srcbase "_a-*.Po' -o -name 'lib*_a-*" srcbase ".Po' | xargs egrep -h ':$' | sort -u | cut -f1 -d: | egrep -v '(/lib/modules|/usr/lib)' | xargs -r cat | md5sum" # openssl md4 | cut -f2 '-d '"
    if ((result = (command | getline)) == 1) srcversion = $1
    else print_error("comand result was " result ", ERRNO = " ERRNO)
    close(command)
    #system("rm -f " srcbase ".sources.log")
    if (reason == "all")
	print "\n#ifdef CONFIG_MODULE_SRCVERSION_ALL\nMODULE_INFO(srcversion, \"" srcversion "\");\n#endif" >> file
    else
	print "\nMODULE_INFO(srcversion, \"" srcversion "\");\n" >> file
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
	 if (values["verbose"] == 0)
	    print "  GEN   " basename ".mod.c"
	print_debug("writing " file)
	write_header(file, modname, basename)
	if (values["modversion"])
	    write_modversions(file, modname, basename)
	write_dependencies(file, modname, basename)
	write_srcversion(file, modname, basename)
	close(file)
    }
    print_debug("w: mymodules, " files " files written")
}
BEGIN {
    print_debug("excuting " ARGV[0])
    "uname -r" | getline uname
    "pwd"      | getline pwd
    longopts["modules"      ] = "M:"                                                             ; defaults["modules"      ] = ""
    longopts["cachefile"    ] = "c:" ; environs["cachefile"    ] = "MODPOST_CACHE"               ; defaults["cachefile"    ] = "modpost.cache"
    longopts["moddir"       ] = "d:" ; environs["moddir"       ] = "MODPOST_MODDIR"              ; defaults["moddir"       ] = "/lib/modules/" uname
    longopts["filename"     ] = "F:" ; environs["filename"     ] = "MODPOST_SYSMAP"              ; defaults["filename"     ] = "/boot/System.map-" uname
    longopts["infile"       ] = "i:" ; environs["infile"       ] = "MODPOST_INPUTS"              ; defaults["infile"       ] = "/lib/modules/" uname "/build/Module.symvers"
    longopts["outfile"      ] = "o:" ; environs["outfile"      ] = "MODPOST_MODVER"              ; defaults["outfile"      ] = "Module.symvers"
    longopts["sysfile"      ] = "s:" ; environs["sysfile"      ] = "MODPOST_SYSVER"              ; defaults["sysfile"      ] = "System.symvers"
    longopts["unload"       ] = "u"  ; environs["unload"       ] = "CONFIG_MODULE_UNLOAD"        ; defaults["unload"       ] = 0
    longopts["modversions"  ] = "m"  ; environs["modversions"  ] = "CONFIG_MODVERSIONS"          ; defaults["modversions"  ] = 0
    longopts["allsrcversion"] = "a"  ; environs["allsrcversion"] = "CONFIG_MODULE_SRCVERSION_ALL"; defaults["allsrcversion"] = 0
    longopts["exportsyms"   ] = "x"                                                              ; defaults["exportsyms"   ] = 0
    longopts["pkgdirectory" ] = "p:" ; environs["pkgdirectory" ] = "PACKAGE_LCNAME"              ; defaults["pkgdirectory" ] = "openss7"
    longopts["dryrun"       ] = "n"                                                              ; defaults["dryrun"       ] = 0
    longopts["dry-run"      ] = "n"                                                              ; defaults["dry-run"      ] = 0
    longopts["quiet"        ] = "q"                                                              ; defaults["quiet"        ] = 0
    longopts["silent"       ] = "q"                                                              ; defaults["silent"       ] = 0
    longopts["debug"        ] = "D::"                                                            ; defaults["debug"        ] = 0
    longopts["verbose"      ] = "v::"                                                            ; defaults["verbose"      ] = 1
    longopts["help"         ] = "h"  ;
    longopts["version"      ] = "V"  ;
    longopts["copying"      ] = "C"  ;
    # set mandatory defaults
    values["debug"     ] = defaults["debug"     ]
    values["verbose"   ] = defaults["verbose"   ]
    values["quiet"     ] = defaults["quiet"     ]
    optstring = "M:c:d:F:i:o:s:umaxp:nqD::v::hVC"
    while ((c = getopt_long(ARGC, ARGV, optstring, longopts))) {
	if (c == -1) break
	else if (c ~ /[cdFiosp]/) { values[option] = optarg }
	else if (c ~ /[umaxnq]/)  { values[option] = 1 }
	else if (c == "D") { if (optarg) { values["debug"  ] = optarg } else { values["debug"  ]++ } }
	else if (c == "v") { if (optarg) { values["verbose"] = optarg } else { values["verbose"]++ } }
	else if (c == "h") { help("/dev/stdout"); exit 0 }
	else if (c == "V") { version("/dev/stdout"); exit 0 }
	else if (c == "C") { copying("/dev/stdout"); exit 0 }
	else { usage("/dev/stderr"); exit 2 }
    }
    if (optind < ARGC) {
	values["modules"] = ARGV[optind]; optind++
	while (optind < ARGC) {
	    values["modules"] = values["modules"] " " ARGV[optind]; optind++
	}
    }
    for (value in values) {
	if (!values[value] && (value in environs) && ENVIRON[environs[value]])
	    values[value] = ENVIRON[environs[value]]
	if (!values[value] && (value in defaults) && defaults[value])
	    values[value] = defaults[value]
    }
    split(values["modules"], modules)
    #if (values["cachefile"])	print_debug("cachefile is " values["cachefile"])
    #if (values["moddir"])		print_debug("moddir is " values["moddir"])
    #if (values["filename"])	print_debug("filename is " values["filename"])
    #if (values["infile"])	print_debug("infile is " values["infile"])
    #if (values["outfile"])	print_debug("outfile is " values["outfile"])
    #if (values["sysfile"])	print_debug("sysfile is " values["sysfile"])
    #if (values["pkgdirectory"])	print_debug("pkgdirectory is " values["pkgdirectory"])
    #if (values["unload"])	print_debug("module unloading is supported")
    #if (values["modversion"])	print_debug("module versions are supported")
    #if (values["allsrcversion"])	print_debug("source version all modules")
    #if (exportsym)	print_debug("mark export of symbols")
    if (values["modules"])	print_debug("modules are \"" values["modules"] "\"")
    cache_dirty = 1
    if (values["cachefile"] && (read_cachefile(values["cachefile"]) != -1))
	cache_dirty = 0
    else {
	if (values["filename"])
	    read_systemmap(cat_command(values["filename"]), "vmlinux", 0)
	if (values["infile"])
	    read_dumpfiles(cat_command(values["infile"]))
	if (values["moddir"])
	    read_moduledir(values["moddir"])
    }
    if (values["modules"])
	read_mymodules(modules)
    # write out cache file
    if (values["cachefile"] && cache_dirty)
	write_cachefile(values["cachefile"])
    if (values["sysfile"])
	write_syssymver(values["sysfile"])
    if (values["modules"]) {
	if (values["outfile"])
	    write_modsymver(values["outfile"])
	write_mymodules(modules)
    }
}

# =============================================================================
#
# $Log: modpost.awk,v $
# Revision 1.1.2.1  2009-07-21 11:06:21  brian
# - new awk scripts for release check
#
# =============================================================================
# vim: sw=4 fo+=tcqlorn
