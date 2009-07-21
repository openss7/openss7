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
function print_s(string) {
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ": S: " string > stdout
    #print me ": S: " string > logfile
    success++
}
function print_w(string) {
    if (ENVIRON["MAINTAINER_MODE"] ~ /^(verbose|continue)$/ || ENVIRON["V"] == 1)
	print me ": W: --- " string > stderr
    print me ": W: " string > logfile
    warnings++
}
function print_e(string) {
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

BEGIN {
    me = "check_decls"
    stdout = "/dev/stdout"
    stderr = "/dev/stderr"
    logfile = ENVIRON["top_builddir"] "/" me ".log"
    symslog = ENVIRON["top_builddir"] "/check_symbols.log"
    ctaglog = ENVIRON["top_builddir"] "/check_ctags.log"

    n = split(ENVIRON["EXPOSED_SYMBOLS"], exposed)
    for (i = 1; i <= n; i++) { exposed[exposed[i]] = 1; delete exposed[i] }

    show_starting("indexing", "symbols")
    while ((getline < symslog) == 1) {
	symbols[$0] = 1
	show_progress("indexed", "symbols", 1000)
    }
    close(symslog)
    show_complete("indexed", "symbols")
    retval = 0
    show_starting("checking", "prototypes and externvars")
    while ((getline < ctaglog) == 1) {
	if ($2 ~ /^(prototype|externvar)/) {
	    print_t("testing " $2 " " $1 " for exported symbol")
	    if ($1 in symbols)
		print_s($2 " " $1 " has exported symbol")
	    else if (($1 ~ /^_/) || (ENVIRON["WARN_EXCESS"] == "yes") || ($1 in exposed))
		print_w("cannot find symbol for " $2 " " $1)
	    else
		print_e("cannot find symbol for " $2 " " $1)
	    show_progress("checked", "prototypes and externvars", 1000)
	}
    }
    close(ctaglog)
    show_complete("checked", "prototypes and externvars")

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
