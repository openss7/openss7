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

function get_headers(    find, files, i, n, bdir, sdir, file) {
    if ((n = split(ENVIRON["PKG_HEADERS"], headers))) {
	bdir = "."
	sdir = ENVIRON["srcdir"]
	find = "find " bdir " " sdir " -type f | sort -u 2>/dev/null"
	while ((find | getline) == 1)
	    files[$0] = 1
	close(find)
	# find all the headers
	show_starting("checking", "headers")
	for (i = 1; i <= n; i++) {
	    header = headers[i]
	    show_progress("checked", "headers", 100)
	    file = bdir "/" header; if (file in files) { headers[i] = file; continue }
	    file = sdir "/" header; if (file in files) { headers[i] = file; continue }
	    print_e("could not find header " header)
	    headers[i] = ""
	}
	show_complete("checked", "headers")
    }
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
function get_srcpages(    numb, page, path, find) {
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

BEGIN {
    me = "check_headers"
    stdout = "/dev/stdout"
    stderr = "/dev/stderr"
    logfile = ENVIRON["top_builddir"] "/" me ".log"
    usedlog = ENVIRON["top_builddir"] "/check_used.log"
    symslog = ENVIRON["top_builddir"] "/check_symbols.log"
    ctaglog = ENVIRON["top_builddir"] "/check_ctags.log"
    hdrlist = ENVIRON["top_builddir"] "/" me ".lst"

    get_headers()
    hcount = count
    show_starting("listing", "headers")
    for (i = 1; i in headers; i++)
	if ((header = headers[i]))
	    print header > hdrlist
    close(hdrlist)
    count = hcount
    show_complete("listed", "headers")
    # cannot do this during a distcheck
    if (ENVIRON["MAINTAINER_MODE"] != "no") {
	get_manpages("235679")
	get_srcpages("235679")
	split("3 5 6", sections)
	show_starting("checking", "headers")
	for (i = 1; i in headers; i++) {
	    base = headers[i]; sub(/.*\//, "", base); sub(/\.h.*$/, "", base)
	    if ((found = search_pages(base, sections, manpages)) ||
		(found = search_pages(base ".h", sections, manpages))) {
		print_s("found " found)
		print found >> usedlog
	    } else {
		print_w("could not find " base, sections)
		if ((found = search_pages(base, sections, srcpages)) ||
		    (found = search_pages(base ".h", sections, srcpages)))
		    print_w("found manual source page " found)
	    }
	    show_progress("checked", "headers", 100)
	}
	show_complete("checked", "headers")
	types["function" ] = "2 3 9" SUBSEP 1 SUBSEP "E"
	types["prototype"] = "2 3 9" SUBSEP 0 SUBSEP "E"
	types["variable" ] = "2 3 9" SUBSEP 1 SUBSEP "S"
	types["externvar"] = "2 3 9" SUBSEP 0 SUBSEP "S"
	types["macro"    ] = "3 9 7" SUBSEP 0 SUBSEP "S"
	types["enumerate"] = "3 9 7" SUBSEP 0 SUBSEP "S"
	types["enum"     ] = "3 9"   SUBSEP 0 SUBSEP "W"
	types["struct"   ] = "3 9 7" SUBSEP 0 SUBSEP "W"
	types["union"    ] = "3 9 7" SUBSEP 0 SUBSEP "W"
	types["typedef"  ] = "3 9 7" SUBSEP 0 SUBSEP "W"
	types["member"   ] = ""      SUBSEP 0 SUBSEP "E"
	types["OTHER"    ] = "3 9 7" SUBSEP 0 SUBSEP "W"
	types["_"        ] = "3 9 7" SUBSEP 0 SUBSEP "W"
	show_starting("checking", "symbols")
	ctags ="cat " hdrlist " | xargs ctags -x --c-kinds=defgpstuvx " ENVIRON["CTAGS"] " | sed -e 's,\\$,\\ ,' | tee " ctaglog
	while ((ctags | getline) == 1) {
	    if ($1 ~ /^_/) { i = "_" } else { if ($2 in types) { i = $2 } else { i = "OTHER" } }
	    split(types[i], vals, SUBSEP)
	    if (vals[2])
		print $1 >> symslog
	    if (split(vals[1], sections)) {
		if ((found = search_pages($1, sections, manpages))) {
		    print_s("found " found)
		    print found >> usedlog
		} else {
		    if ((found = search_pages($1, sections, srcpages)))
			print_w("found manual source page " found)
		    string = "could not find " $2 " " $1
		    if (vals[3] == "S") { print_f(string, sections) } else
		    if (vals[3] == "W") { print_w(string, sections) } else
		                        { print_e(string, sections) }
		}
	    }
	    show_progress("checked", "symbols", 1000)
	}
	show_complete("checked", "symbols")
	close(ctags)
	system("rm -f " hdrlist)
    }
    retval = 0
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
