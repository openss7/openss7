#!/usr/bin/awk -f

function print_q(string) {
    if (ENVIRON["V"] == 0)
	print me ": Q: " string > stderr
}
function print_v(string) {
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ":  : " string > stdout
}
function print_t(string) {
    if (ENVIRON["MAINTAINER_MODE"] == "verbose" || ENVIRON["V"] == 1)
	print me ": T: " string > stdout
}
function print_d(string) {
    if (ENVIRON["MAINTAINER_MODE"] == "verbose" || ENVIRON["V"] == 1)
	print me ": D: " string > stderr
    print me ": D: " string > logfile
}
function get_sections(sections,    i, result) {
    i = 1
    if (i in sections)
	result = " in sections " sections[i]
    for (i = 2; i in sections; i++) {
	if ((i+1) in sections)
	    result = result ", " sections[i]
	else
	    result = result " or " sections[i]
    }
    return result
}
function print_s(string, sections) {
    string = string get_sections(sections)
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ": S: " string > stdout
    #print me ": S: " string > logfile
    success++
}
function print_f(string, sections) {
    string = string get_sections(sections)
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ": F: " string > stdout
    #print me ": F: " string > logfile
}
function print_w(string, sections) {
    string = string get_sections(sections)
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ": W: --- " string > stderr
    print me ": W: " string > logfile
    warnings++
}
function print_e(string, sections) {
    string = string get_sections(sections)
    print me ": E: --- " string > stderr
    print me ": E: " string > logfile
    errors++
}
function show_starting(doing, things) {
    print_q(doing " " things)
    count = 0
    progress = 0
}
function show_progress(doing, things, numb) {
    if (progress >= numb) {
	print_q(doing " " count " " things "...")
	progress = 0
    }
    count++
    progress++
}
function show_complete(doing, things) {
    print_q(doing " " count " " things)
}

function get_manpages(s,    numb, page, path, find) {
    find = "find " ENVIRON["top_builddir"] "/doc/man -follow -type f -name '*.[" s "]*' 2>/dev/null | sort -u"
    show_starting("indexing", "built manual pages")
    while ((find | getline) == 1) {
	page = $0
	sub(/.*\//, "", page)
	sub(/\.gz$/, "", page)
	sub(/\.bz2$/, "", page)
	sub(/\.xz$/, "", page)
	numb = page
	sub(/\.[^\.]*$/, "", page)
	sub(/.*\./, "", numb)
	numb = substr(numb, 1, 1)
	manpaths[$0] = numb SUBSEP page
	if ((numb,page) in manpages)
	    manpages[numb,page] = manpages[numb,page] " " $0
	else
	    manpages[numb,page] = $0
	show_progress("indexed", "built manual pages", 1000)
    }
    show_complete("indexed", "built manual pages")
}
function get_srcpages(s,    numb, page, path, find) {
    find = "find " ENVIRON["top_srcdir"] "/doc/man -follow -type f -name '*.[" s "]*' 2>/dev/null | sort -u"
    show_starting("indexing", "source manual pages")
    while ((find | getline) == 1) {
	page = $0
	sub(/.*\//, "", page)
	sub(/\.gz$/, "", page)
	sub(/\.bz2$/, "", page)
	sub(/\.xz$/, "", page)
	numb = page
	sub(/\.[^\.]*$/, "", page)
	sub(/.*\./, "", numb)
	numb = substr(numb, 1, 1)
	srcpaths[$0] = numb SUBSEP page
	if ((numb,page) in srcpages)
	    srcpages[numb,page] = srcpages[numb,page] " " $0
	else
	    srcpages[numb,page] = $0
	show_progress("indexed", "source manual pages", 1000)
    }
    show_complete("indexed", "source manual pages")
}
function search_pages(base, sections, pages,    found, n, s) {
    for (n = 1; n in sections; n++) {
	s = sections[n]
	#print_t("checking " base " in section " s)
	if ((s,base) in pages) {
	    found = "man" s "/" base "." s
	    #print_s("found " base " in section " s)
	}
    }
    return found
}
function search_symbol(symbol, sections, weak,    found, string) {
    if ((found = search_pages(symbol, sections, manpages))) {
	print_s("found " found)
	print found >> usedlog
    } else {
	if ((found = search_pages(symbol, sections, srcpages)))
	    print_w("found manual source page " found)
	string = "could not find manual page for symbol " symbol
	if (weak || symbol ~ /^_/)
	    print_w(string, sections)
	else
	    print_e(string, sections)
    }
}
function search_multi(myname, sections,    mcname, msname, nlname, ucname, usname, ulname, CHECKS, checks, c, gotone, j, found) {
    mcname = myname; gsub(/[^-_[:alnum:]]/, "_", mcname)
    msname = mcname; gsub(/-/, "_", msname)
    nlname = mcname; sub(/^lib/, "", nlname)
    ucname = toupper(mcname)
    usname = toupper(msname)
    ulname = toupper(nlname)
    CHECKS = mcname ", " ucname ", " msname ", " usname ", " nlname ", " ulname
    c = split(CHECKS, checks, ", ")
    gotone = 0
    for (j in checks) {
	check = checks[j]
	if ((found = search_pages(check, sections, manpages))) {
	    print_s("found " found)
	    print found >> usedlog
	    gotone = 1
	}
    }
    if (!gotone) {
	for (j in checks) {
	    check = checks[j]
	    if ((found = search_pages(check, sections, srcpages)))
		print_w("found manual source page " found)
	}
	print_e("could not find " CHECKS, sections)
    }
}
function get_symbols(modules,    i, mod, mods, nm, addr, type, symb) {
    for (i in modules) {
	mod = modules[i]
	if (system("test -f " mod) != 0) {
	    print_e("cannot find module " modules[i])
	    continue
	}
	mods = mods " " mod
    }
    show_starting("indexing", "symbols")
    nm = "nm -s " mods
    while (nm | getline) {
	if (sub(/^.*. _?__ksymtab_/, "")) {
	    sub(/_+$/, "", $1)
	    symbols[$1] = 1
	}
	show_progress("indexed", "symbols", 10000)
    }
    close(nm)
    show_complete("indexed", "symbols")
}
function get_ctags() {
    while ((getline < ctaglog) == 1) {
	if ($2 == "function")  functions[$1]  = $0
	else
	if ($2 == "prototype") prototypes[$1] = $0
	else
	if ($2 == "externvar") externvars[$1] = $0
    }
    close(ctaglog)
}

BEGIN {
    me = "check_modules"
    stdout = "/dev/stdout"
    stderr = "/dev/stderr"
    logfile = ENVIRON["top_builddir"] "/" me ".log"
    usedlog = ENVIRON["top_builddir"] "/check_used.log"
    symslog = ENVIRON["top_builddir"] "/check_symbols.log"
    ctaglog = ENVIRON["top_builddir"] "/check_ctags.log"
    modlist = ENVIRON["top_builddir"] "/" me ".lst"

    retval = 0

    if (!(KERNEL_SYSMAP = ENVIRON["KERNEL_SYSMAP"]))
	if (!(KERNEL_SYSMAP = ENVIRON["DESTDIR"] ENVIRON["ksysmap"]))
	    exit 77
    if (!(KERNEL_MODMAP = ENVIRON["KERNEL_MODMAP"]))
	KERNEL_MODMAP = "Modules.map"
    if (!ENVIRON["KERNEL_MODULES"])
	exit 77
    print_d("using system map " KERNEL_SYSMAP)
    while ((getline < KERNEL_SYSMAP) == 1) {
	if (sub(/^.*. _?__ksymtab_/, "")) {
	    sub(/_+$/, "", $1)
	    defined[$1] = 1
	}
    }
    close(KERNEL_SYSMAP)
    print_d("using module map " KERNEL_MODMAP)
    while ((getline < KERNEL_MODMAP) == 1) {
	if (sub(/^.*. _?__ksymtab_/, "")) {
	    sub(/_+$/, "", $1)
	    if (!($1 in defined))
		defined[$1] = 1
	    else
		print_e("conflicting symbol " $1)
	}
    }
    close(KERNEL_MODMAP)
    if ((n = split(ENVIRON["KERNEL_MODULES"], modules))) {
	for (i = 1; i <= n; i++)
	    print modules[i] > modlist
	close(modlist)
	undefine = "cat " modlist " | xargs nm -s | egrep '\\<U\\>'"
	while ((undefine | getline) == 1)
	    undefined[$2] = 1
	close(undefine)
	for (sym in undefined) {
	    if (sym in defined) {
		print_s("symbol " sym " is defined")
	    } else {
		if (sym ~ /^_?__this_module$/)
		    continue
		sub(/_R(smp_)?[[:xdigit:]]{8}$/, "", sym)
		if (sym in defined)
		    print_s("symbol " sym " is defined")
		else
		    print_e("cannot find symbol " sym)
	    }
	}
    }
    get_manpages("4579")
    get_srcpages("4579")
    if ((n = split(ENVIRON["KERNEL_MODULES"], modules))) {
	split("4 5 7 9", sections)
	show_starting("checking", "modules")
	for (i = 1; i <= n; i++) {
	    mod = modules[i]
	    myname = mod
	    sub(/^lib[^[:space:]]*_a-/, "", myname)
	    sub(/^streams[-_]/, "", myname)
	    sub(/\.(gz|bz2|xz)$/, "", myname)
	    sub(/\.(ko|o)$/, "", myname)
	    search_multi(myname, sections)
	    show_progress("checked", "modules", 1000)
	}
	show_complete("checked", "modules")
	get_symbols(modules)
	get_ctags()
	show_starting("checking", "symbols")
	split("9", sections)
	for (symb in symbols) {
	    if (symb in functions) {
		print_s("exported symbol " symb " has function")
		print symb >> symslog
	    } else
	    if (symb in prototypes) {
		print_s("exported symbol " symb " has prototype")
		print symb >> symslog
	    } else
	    if (symb in externvars) {
		print_s("exported symbol " symb " has externvar")
		print symb >> symslog
	    } else {
		string = "exported symbol " symb " has no function, prototoype or externvar"
		if (symb ~ /^_/)
		    print_w(string)
		else
		    print_e(string)
	    }
	    search_symbol(symb, sections)
	    show_progress("checked", "symbols", 1000)
	}
	show_complete("checked", "symbols")
    }

    close(usedlog)
    close(symslog)
    close(logfile)
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
    if (warnings) {
	print_v("+-----------------+")
	print_v("| Warning Summary |")
	print_v("+-----------------+")
	while ((getline < logfile) == 1)
	    if (/\<W:/) print > stderr
	close(logfile)
    }
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
    if (errors) {
	print_v("+---------------+")
	print_v("| Error Summary |")
	print_v("+---------------+")
	while ((getline < logfile) == 1)
	    if (/\<E:/) print > stderr
	close(logfile)
    }
    if (errors) {
	if (ENVIRON["MAINTAINER_MODE"] == "continue")
	    retval = 77
	else
	    retval = 1
    }
    print_v("successes " success+0)
    print_v("warnings  " warnings+0)
    print_v("errors    " errors+0)
    exit retval
}

# vim: ft=awk sw=4 nocin nosi fo+=tcqlorn
