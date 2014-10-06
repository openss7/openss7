#!/usr/bin/awk -f
# =============================================================================
# 
# @(#) $RCSfile: modshow.awk,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2011-04-07 15:24:01 $
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
# Last Modified $Date: 2011-04-07 15:24:01 $ by $Author: brian $
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
  $Id: modshow.awk,v 1.1.2.1 2011-04-07 15:24:01 brian Exp $\n\
Usage:\n\
  [awk -f ]" me " -- [options] [mode] [MODULE ...]\n\
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
$Id: modshow.awk,v 1.1.2.1 2011-04-07 15:24:01 brian Exp $\n\
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
$Id: modshow.awk,v 1.1.2.1 2011-04-07 15:24:01 brian Exp $\n\
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
    option = ""; optval = ""; optarg = ""; optset = ""; pos = 0; needarg = 0; wantarg = 0; modearg = 0
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
function show_module(module,
		     SECTION,string,command,char,sym,crc,line,label,modinfos,crcs,link,have,weak,need,i,n)
{
    if (system("test -r " module) == 0) {
	command = "objdump -t -j '*ABS*' -j '*UND*' -j __versions -j __weak_versions -j __absolute -j __weak_absolute -j .modinfo -s " module
	SECTION = ""; string = ""
	while ((command | getline) == 1) {
	    if (/SYMBOL TABLE:/) {
		SECTION = "SYMBOL TABLE"
	    } else
	    if (sub(/^Contents of section /,"")) {
		sub(/:.*$/, "")
		SECTION = $0
		string = ""
	    } else
	    if (SECTION == "SYMBOL TABLE") {
		if (NF < 4) continue
		if ($(NF-2) == "*ABS*") {
		    if (substr($0,length($1)+2,7)~/^g     [FO ]$/) {
			if (sub(/^_?__crc_/,"",$NF)) {
			    #sub(/^00000000/,"",$1)
			    have[$NF] = $1
			} else {
			    link[$NF] = $1
			}
		    }
		} else
		if ($(NF-2) == "*UND*") {
		    if (substr($0,length($1)+3,1) == "w") {
			weak[$NF] = ""
		    } else {
			need[$NF] = ""
		    }
		}
	    } else
	    if (SECTION == "__versions" || SECTION == "__weak_versions" || SECTION == "__absolute" || SECTION == "__weak_absolute")
	    {
		sub(/^ .... /,""); sub(/  .*$/,""); gsub(/ /,"")
		string = string $0
		if (length(string) < 128)
		    continue
		sub(/(00)+$/,"",string)
		crc = substr(string,7,2) substr(string,5,2) substr(string,3,2) substr(string,1,2)
		sub(/^......../, "", string)
		if (SECTION == "__versions" || SECTION == "__weak_versions") {
		    if (crc == "00000000")
		    { crc = crc substr(string,1,8); sub(/^......../,"",string) }
		} else {
		    if (crc == "ffffffff")
		    { crc = crc substr(string,1,8); sub(/^......../,"",string) }
		}
		if (sub(/^00000000/, "", string)) crc = "00000000" crc
		if (sub(/^ffffffff/, "", string)) crc = "ffffffff" crc
		sym = ""
		while (string) {
		    char = "0x" substr(string,1,2)
		    sub(/^../, "", string)
		    char = strtonum(char)
		    if (32 <= char && char <= 126)
			sym = sym sprintf("%c", char)
		}
		crcs[SECTION,sym] = crc
	    } else
	    if (SECTION == ".modinfo") {
		sub(/^ .... /, ""); sub(/  .*$/, ""); gsub(/ /, "")
		string = string $0
		gsub(/^00(00)*/, "", string)
		while (string~/^(0[^0]|[^0]0|[^0][^0])*00/) {
		    line = ""
		    while (string) {
			char = "0x" substr(string,1,2)
			sub(/^../, "", string)
			char = strtonum(char)
			if (32 <= char && char <= 126)
			    line = line sprintf("%c", char)
			else
			if (char == 0) {
			    gsub(/^00(00)*/, "", string)
			    break
			}
		    }
		    if (match(line,/^[^=]*=/)) {
			label = substr(line,RSTART,RLENGTH-1);
			sub(/^[^=]*=/, "", line)
			if (label in modinfos)
			{ modinfos[label] = modinfos[label] "," line } else
			{ modinfos[label] = line }
		    }
		}
	    }
	}
	close(command)
	text = sprintf("%36s = %s\n", "file", module)
	n = asorti(modinfos,reqs)
	for (i=1;i<=n;i++) {
	    label = reqs[i]
	    line = sprintf("%36s = %s\n", label, modinfos[label])
	    if (text) { text = text line } else { text = line }
	}
	if (text) print "Module:\n" text
	text = ""
	n = asorti(have,prov)
	for (i=1;i<=n;i++) {
	    sym = prov[i]
	    line = sprintf("%36s = 0x%08x\n", sym, strtonum("0x" have[sym]))
	    if (text) { text = text line } else { text = line }
	}
	if (text) print "Provides:\n" text
	text = ""
	n = asorti(need,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__versions",sym) in crcs) {
		line = sprintf("%36s = 0x%08x\n", sym, strtonum("0x" crcs["__versions",sym]))
		if (text) { text = text line } else { text = line }
	    } else
	    if (("__absolute",sym) in crcs)
		continue
	    else
		print_error("undefined symbol " sym)
	}
	if (text) print "Requires:\n" text
	text = ""
	n = asorti(weak,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__weak_versions",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, strtonum("0x" crcs["__weak_versions",sym]), 0)
		if (text) { text = text line } else { text = line }
		print_error("unresolved weak symbol " sym)
	    } else
	    if (("__versions",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, strtonum("0x" crcs["__versions",sym]), 0)
		if (text) { text = text line } else { text = line }
	    } else
	    if (("__weak_absolute",sym) in crcs)
		continue
	    else
		print_error("undefined weak symbol " sym)
	}
	n = asorti(link,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__weak_versions",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, strtonum("0x" crcs["__weak_versions",sym]), strtonum("0x" link[sym]))
		if (text) { text = text line } else { text = line }
	    } else
	    if (("__absolute",sym) in crcs)
		continue
	    else
	    if (("__weak_absolute",sym) in crcs)
		continue
	    else
		print_error("over-resolved symbol " sym)
	}
	if (text) print "Requires (weak):\n" text
	text = ""
	n = asorti(link,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__absolute",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, 0, strtonum("0x" link[sym]))
		if (text) { text = text line } else { text = line }
	    } else
	    if (("__weak_versions",sym) in crcs)
		continue
	    else
	    if (("__weak_absolute",sym) in crcs)
		continue
	    else
		print_error("unknown absolute symbol " sym)
	}
	n = asorti(need,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__absolute",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, strtonum("0x" crcs["__absolute",sym]), 0)
		if (text) { text = text line } else { text = line }
		print_error("unresolved hard symbol " sym)
	    } else
	    if (("__versions",sym) in crcs)
		continue
	    else
		print_error("undefined symbol " sym)
	}
	if (text) print "Absolute:\n" text
	text = ""
	n = asorti(link,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__weak_absolute",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, 0, strtonum("0x" link[sym]))
		if (text) { text = text line } else { text = line }
	    } else
	    if (("__weak_versions",sym) in crcs)
		continue
	    else
	    if (("__absolute",sym) in crcs)
		continue
	    else
		print_error("over-resolved symbol " sym)
	}
	n = asorti(need,reqs)
	for (i=1;i<=n;i++) {
	    sym = reqs[i]
	    if (("__weak_absolute",sym) in crcs) {
		line = sprintf("%36s = 0x%08x [0x%016x]\n", sym, strtonum("0x" crcs["__weak_absolute",sym]), 0)
		if (text) { text = text line } else { text = line }
		print_error("unresolved weak symbol " sym)
	    } else
	    if (("__versions",sym) in crcs)
		continue
	    else
		print_error("undefined symbol " sym)
	}
	if (text) print "Absolute (weak):\n" text
    } else {
	print_error("r: module, cannot read " module)
    }
}
function show_modules(modules,	n,mods,i)
{
    n = split(modules, mods)
    for (i=1;i<=n;i++) {
	show_module(mods[i])
    }
}
BEGIN {
    LINT = "yes"
    me = "modshow.awk"
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
    if ("kversion" in ENVIRON) { d["kversion"] = ENVIRON["kversion"] }
    else                       { d["kversion"] = getline_command("uname -r") }
    d["package" ] = "openss7"
    d["style"   ] = "debian"
    d["basedir" ] = ""
    d["weakdir" ] = "weak-updates"
    d["sysmap"  ] = "/boot/System.map-$KVERSION"
    d["sysver"  ] = "/lib/modules/$KVERSION/build/Module.symvers"
    d["symver"  ] = "/boot/symvers-$KVERSION.gz"
    longopts["kversion"      ] = "k:" ; defaults["kversion"      ] = d["kversion"]      ; descrips["kversion"      ] = "default kernel version"
    longopts["add-kernel"    ] = "A!" ;							  descrips["add-kernel"    ] = "mode: add kernels with versions"
    longopts["link-kernel"   ] = "L!" ;							  descrips["link-kernel"   ] = "mode: link kernels with versions"
    longopts["remove-kernel" ] = "R!" ;							  descrips["remove-kernel" ] = "mode: remove kernels with versions"
    longopts["add-modules"   ] = "a!" ;							  descrips["add-modules"   ] = "mode: add kernel modules to kernel versions"
    longopts["link-modules"  ] = "l!" ;							  descrips["link-modules"  ] = "mode: link modules for kernel with versions"
    longopts["remove-modules"] = "r!" ; defaults["remove-modules"] = d["kversion"]	; descrips["remove-modules"] = "mode: remove kernel modules from kernel versions"
    longopts["modules"       ] = "M:" ; defaults["modules"       ] = ""			; descrips["modules"       ] = "modules for which to generate symbols (search when unspec.)"				; longargs["modules"       ] = "MODULE ..."
    longopts["rootdir"       ] = "c:" ; defaults["rootdir"       ] = ""			; descrips["rootdir"       ] = "root of the kernels being installed"
    longopts["basedir"       ] = "b:" ; defaults["basedir"       ] = d["basedir" ]	; descrips["basedir"       ] = "use a base directory other than root, BASEDIR"
    longopts["weakdir"       ] = "w:" ; defaults["weakdir"       ] = d["weakdir" ]	; descrips["weakdir"       ] = "where to place weak update symbolic links\nabsolute or rel. to /lib/modules/$KVERSION)"	; longargs["weakdir"       ] = "PATH"
    longopts["sysmap"        ] = "F:" ; defaults["sysmap"        ] = d["sysmap"  ]	; descrips["sysmap"        ] = "system map file (same as depmod) (search when unspec.)"
    longopts["sysver"        ] = "E:" ; defaults["sysver"        ] = d["sysver"  ]	; descrips["sysver"        ] = "system version file (same as depmod) (search when unspec.)"
    longopts["symver"        ] = "S:" ; defaults["symver"        ] = d["symver"  ]	; descrips["symver"        ] = "system version file (compressed) (search when unspec.)"
    longopts["package"       ] = "P:" ; defaults["package"       ] = d["package" ]	; descrips["package"       ] = "package name (used to find subdirectories)"
    longopts["style"         ] = "s:" ; defaults["style"         ] = d["style"   ]	; descrips["style"         ] = "style of weak-updates (detect when unspec.)"						; longargs["style"         ] = "{debian|redhat|suse10|suse11}"
    longopts["depmodx"       ] = "x"  ; defaults["depmodx"       ] = 0			; descrips["depmodx"       ] = "depmod supports -E option (detect when unspec.)"
    longopts["exit-on-error" ] = "e"  ; defaults["exit-on-error" ] = 0			; descrips["exit-on-error" ] = "exit with error status on program errors"
    longopts["dryrun"        ] = "n"  ; defaults["dryrun"        ] = 0			; descrips["dryrun"        ] = "don't perform actions, just check them"
    longopts["dry-run"       ] = "n"  ; defaults["dry-run"       ] = 0			; descrips["dry-run"       ] = "don't perform actions, just check them"
    longopts["quiet"         ] = "q"  ; defaults["quiet"         ] = 0			; descrips["quiet"         ] = "suppress normal output"
    longopts["silent"        ] = "q"  ; defaults["silent"        ] = 0			; descrips["silent"        ] = "suppress normal output"
    longopts["debug"         ] = "D::"; defaults["debug"         ] = 0			; descrips["debug"         ] = "increase or set debug level DEBUG"							; environs["debug"        ] = "OS7UPDT_DEBUG"
    longopts["verbose"       ] = "v::"; defaults["verbose"       ] = 0			; descrips["verbose"       ] = "increase or set verbosity level VERBOSITY"						; environs["verbose"      ] = "OS7UPDT_VERBOSE"
    longopts["help"          ] = "h!" ;							  descrips["help"          ] = "display this usage information and exit"
    longopts["version"       ] = "V!" ;							  descrips["version"       ] = "display script version and exit"
    longopts["copying"       ] = "C!" ;							  descrips["copying"       ] = "display coding permissions and exit"
    delete d
    values["kversion"     ] = defaults["kversion"     ]
    values["sysmap"       ] = defaults["sysmap"       ]
    values["sysver"       ] = defaults["sysver"       ]
    values["symver"       ] = defaults["symver"       ]
    values["package"      ] = defaults["package"      ]
    values["rootdir"      ] = defaults["rootdir"      ]
    values["basedir"      ] = defaults["basedir"      ]
    values["exit-on-error"] = defaults["exit-on-error"]
    values["dryrun"       ] = defaults["dryrun"       ]
    values["dry-run"      ] = defaults["dry-run"      ]
    values["quiet"        ] = defaults["quiet"        ]
    values["silent"       ] = defaults["silent"       ]
    values["debug"        ] = defaults["debug"        ]
    values["verbose"      ] = defaults["verbose"      ]
    optstring = "k:ALRalrM:c:b:w:F:E:S:P:s:xenqD::v::hVC"
    optind = 0
    #opts = ""; for (i=1;i<ARGC;i++) { if (i == 1) { opts = ARGV[i] } else { opts = opts " " ARGV[i] } }
    #print me ": D: o: command line: " opts > stderr; written[stderr] = 1
    command = ""
    while (1) {
	c = getopt_long(ARGC, ARGV, optstring, longopts)
	#if (c != -1) { print me ": D: o: option -" c ", longopt --" option ", optset = " optset ", optarg = " optarg > stderr; written[stderr] = 1 }
	if (c == -1) break
	else if (c~/[kMcbwFESPs]/)			{ values[option] = optarg }
	else if (c~/[xenq]/)				{ values[option] = optset }
	else if (c~/[Dv]/)	{ if (optarg  != "")	{ values[option] = optarg } else { if (optset)  { values[option]++ } else { values[option] = optset } } }
	else if (c~/[ALRalr]/)				{ values[option] = optset; if (!optset && command == option)	{ command = "" } else
										   if (optset && !command)		{ command = option } else
										   if (optset) {  print_error("option --" option " is incompatible with --" command); exit 2 } }
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

    show_modules(values["modules"])

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
# $Log: modshow.awk,v $
# Revision 1.1.2.1  2011-04-07 15:24:01  brian
# - weak reference corrections
#
# =============================================================================
# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
