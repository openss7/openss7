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

BEGIN {
    me = "check_streams"
    stdout = "/dev/stdout"
    stderr = "/dev/stderr"
    logfile = ENVIRON["top_builddir"] "/" me ".log"
    usedlog = ENVIRON["top_builddir"] "/check_used.log"

    if (ENVIRON["STRCONF_MAKEDEV"])
	    makedev = ENVIRON["top_builddir"] "/" ENVIRON["STRCONF_MAKEDEV"]
    if (ENVIRON["STRCONF_STRLOAD"])
	    strload = ENVIRON["top_builddir"] "/" ENVIRON["STRCONF_STRLOAD"]

    if (!makedev && !strload) {
	    print_d("nothing to check, skipping test")
	    exit 77
    }

    retval = 0

    print_d(makedev " file contents:")
    while ((getline < makedev) == 1) {
	print_d($0)
	if (sub(/%dev.*\)[[:space:]]+/, "")) {
	    sub(/^.*\//, "")
	    sub(/^[0-9]+/, "")
	    sub(/\.[0-9].*/, "")
	    devices[$0] = 1
	}
    }
    close(makedev)
    print_d(strload " file contents:")
    while ((getline < strload) == 1) {
	print_d($0)
	if (sub(/^d[[:space:]]+/, "")) {
	    drivers[$1] = 1
	} else
	if (sub(/^m[[:space:]]+/, "")) {
	    modules[$1] = 1
	}
    }
    close(strload)
    get_manpages("47")
    get_srcpages("47")
    show_starting("checking", "drivers")
    split("4", sections)
    for (driver in drivers) {
	search_multi(driver, sections)
	show_progress("checked", "drivers", 1000)
    }
    show_complete("checked", "drivers")
    show_starting("checking", "devices")
    split("4", sections)
    for (device in devices) {
	search_multi(device, sections)
	show_progress("checked", "devices", 1000)
    }
    show_complete("checked", "drivers")
    show_starting("checking", "modules")
    split("4 7", sections)
    for (module in modules) {
	search_multi(module, sections)
	show_progress("checked", "modules", 1000)
    }
    show_complete("checked", "modules")

    close(usedlog)
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
