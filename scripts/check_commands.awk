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
function no_support(runnable, option,    line) {
    print_e(runnable " does not support " option)
    print_d("stdout was:")
    while ((getline line < outfile) == 1)
	print_d(line)
    close(outfile)
    print_d("stderr was:")
    while ((getline line < errfile) == 1) 
	print_d(line)
    close(errfile)
}
function check_supports(runnable, option,    result, line) {
    print_t("testing " runnable " " option)
    print_d(runnable " " option)
    if (system(runnable " " option " >" outfile " 2>" errfile) == 0) {
	# cannot have error output
	result = getline line < errfile
	close(errfile)
	if (result == 1)
	    no_support(runnable, option)
	else {
	    # must have normal output
	    result = getline line < outfile
	    close(outfile)
	    if (result != 1)
		no_support(runnable, option)
	    else
		print_s(runnable " supports " option)
	}
    } else
	no_support(runnable, option)
    system("rm -f " outfile)
    system("rm -f " errfile)
}
function check_runnable(runnable,    SHELL, result) {
    if ((runnable in exempt) || ((ENVIRON["srcdir"] "/" runnable) in exempt)) {
	print_s(runnable " is standard options exempt")
	return
    }
    if (system("test -e " runnable) == 0)
	runnable = "./" runnable
    else if (system("test -e " ENVIRON["srcdir"] "/" runnable) == 0)
	runnable = ENVIRON["srcdir"] "/" runnable
    else {
	print_e(runnable " does not exist")
	return
    }
    if (system("test -x " runnable) != 0) {
	result = getline SHELL < runnable;
	close(runnable)
	if (result == 1 && SHELL ~ /^#!/)  {
	    sub(/^#![[:space:]]*/, "", SHELL)
	    sub(/[[:space:]]*$/, " ", SHELL)
	} else {
	    print_w(runnable " is not executable, skipping")
	    return
	}
    }
    check_supports(SHELL runnable, "--help")
    check_supports(SHELL runnable, "--version")
    check_supports(SHELL runnable, "--copying")
}

BEGIN {
    me = "check_commands"
    stdout = "/dev/stdout"
    stderr = "/dev/stderr"
    logfile = ENVIRON["top_builddir"] "/" me ".log"
    usedlog = ENVIRON["top_builddir"] "/check_used.log"
    outfile = ENVIRON["top_builddir"] "/" me ".out"
    errfile = ENVIRON["top_builddir"] "/" me ".err"

    n = split(ENVIRON["AM_INSTALLCHECK_STD_OPTIONS_EXEMPT"], exempt)
    for (i = 1; i <= n; i++) { exempt[exempt[i]] = 1; delete exempt[i] }
    retval = 0
    if (ENVIRON["cross_compiling"] != "yes") {
	PROGRAMS = \
	    ENVIRON["bin_PROGRAMS"] " " \
	    ENVIRON["sbin_PROGRAMS"] " " \
	    ENVIRON["sysbin_PROGRAMS"] " " \
	    ENVIRON["syssbin_PROGRAMS"] " " \
	    ENVIRON["libexec_PROGRAMS"] " " \
	    ENVIRON["pkglibexec_PROGRAMS"]
	if ((n = split(PROGRAMS, programs))) {
	    show_starting("checking", "programs")
	    for (i = 1; i <= n; i++) {
		show_progress("checked", "programs", 10)
		check_runnable(programs[i])
	    }
	    show_complete("checked", "programs")
	} else
	    print_d("no programs to test")
    } else {
	print_d("cannot run programs when cross-compiling")
	retval = 77
    }
    SCRIPTS = \
	ENVIRON["dist_bin_SCRIPTS"] " " \
	ENVIRON["dist_sbin_SCRIPTS"] " " \
	ENVIRON["pkglibexec_SCRIPTS"]
    if ((n = split(SCRIPTS, scripts))) {
	show_starting("checking", "scripts")
	for (i = 1; i <= n; i++) {
	    show_progress("checked", "scripts", 10)
	    check_runnable(scripts[i])
	}
	show_complete("checked", "scripts")
    } else
	print_d("no scripts to test")
    if (ENVIRON["MAINTAINER_MODE"] != "no") {
	COMMANDS = \
	    ENVIRON["bin_PROGRAMS"] " " \
	    ENVIRON["sysbin_PROGRAMS"] " " \
	    ENVIRON["dist_bin_SCRIPTS"]
	n1 = split(COMMANDS, commands)
	COMMANDS = \
	    ENVIRON["bin_PROGRAMS"] " " \
	    ENVIRON["sysbin_PROGRAMS"] " " \
	    ENVIRON["dist_bin_SCRIPTS"] " " \
	    ENVIRON["sbin_PROGRAMS"] " " \
	    ENVIRON["syssbin_PROGRAMS"] " " \
	    ENVIRON["dist_sbin_SCRIPTS"] " " \
	    ENVIRON["libexec_PROGRAMS"] " " \
	    ENVIRON["pkglibexec_PROGRAMS"] " " \
	    ENVIRON["pkglibexec_SCRIPTS"]
	if ((n = split(COMMANDS, commands))) {
	    get_manpages("18")
	    get_srcpages("18")
	    show_starting("checking", "commands")
	    for (i = 1; i <= n; i++) {
		command = commands[i]
		sections[1] = (i <= n1) ? 1 : 8
		base = command; sub(/.*\//, "", base)
		if ((found = search_pages(base, sections, manpages))) {
		    print_s("found " found)
		    print found >> usedlog
		} else {
		    if ((found = search_pages(base, sections, srcpages)))
			print_w("found manual source page " found)
		    if ((command in exempt) || ((ENVIRON["srcdir"] "/" command) in exempt))
			print_s(command " is standard options exempt")
		    else
			print_e("could not find " base, sections)
		}
		show_progress("checked", "commands", 1000)
	    }
	    show_complete("checked", "commands")
	}
    } else
	retval = 77
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

# vim: ft=awk sw=4 nocin fo+=tcqlorn
