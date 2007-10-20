: run this script through /bin/sh

PERSON="$1"

MHL="/usr/local/lib/mh/mhl -nobell -noclear -nomoreproc"
GAWK="/usr/local/bin/gawk"
MOSY="/usr/local/bin/mosy"
SNMPI="/usr/local/bin/snmpi"
SMI="/usr/include/isode/snmp/smi.defs"
MHMAIL="/usr/local/bin/mhmail"
SCAN="/usr/local/bin/scan -noclear -noheader"

F=/tmp/mib$$ 	   M=1                R=/tmp/mib$$.reply       L=/tmp/mib$$.log
I=$F/$M
Y=/tmp/mib$$.my    D=/tmp/mib$$.defs  Z=/tmp/mib$$a.defs
P=/tmp/mib$$.prf   C=/tmp/mib$$.ctx
trap "rm -rf $F $R $L $Y $D $Z $P $C" 0 1 2 3 13 15

cd /tmp

echo "Path: /tmp" > $P
MH="$P" export MH

echo "Current-Folder: $F" > $C
MHCONTEXT="$C" export MHCONTEXT

mkdir $F
cat > $I

FROM=`$SCAN -format "%<{reply-to}%{reply-to}%|%<{from}%{from}%|%<{sender}%{sender}%|%<{return-path}%{return-path}%>%>%>%>" $M`

SUBJECT=`$SCAN -format "%<{subject}Re: %{subject}%>" $M`

$GAWK '
BEGIN	{
	    prefix = "";
	    started = 0;
	}
/MIB [ \t]*DEFINITIONS[ \t]*::=[ \t]*BEGIN/ {
	    prefix = "";
	}
/DEFINITIONS[ \t]*::=[ \t]*BEGIN/ {
	    if (length(prefix) > 0) {
		print prefix;
		prefix = "";
	    }
	    started = 1;
	}
/MIB/ && !/--.*MIB/ {
	    if (!started) {
		prefix = $0;
		next;
	    }
	}
/\[Page [1-9][0-9]*\]$/ {
	    eating = 1;
	    next;
	}
//	{
	    if (eating)
		next;
	}
/^[ \t]*$/ {
	    if (eating)
		next;
	}
	{
	    if (eating)
		eating = 0;
	    else
		if (started)
		    print $0;
		else
		    if (length(prefix) > 0)
			prefix = prefix "\n" $0;
	}
/END$/	{
	    exit(0);
	}
' < $I > $Y

echo "% mosy $Y" >> $R
if $MOSY $Y >> $R 2>&1; then
    cat $SMI $D > $Z

    echo "" >> $R
    echo "% snmpi -d -f $Z quit" >> $R
    if $SNMPI -d -f $Z quit >> $R 2>&1; then
	echo "" >> $R
	if [ -z "$PERSON" ]; then
	    echo "# No errors were detected in your MIB" >> $R
	else
	    echo "# Your MIB is being sent to $PERSON for further checking" >> $R
	    $MHL -form mhl.body < $I \
		| $MHMAIL "$PERSON" -subject "MIB for checking" -from "$FROM"
	fi
    else
	echo "" >> $R
	echo "# Pass two (snmpi) failed" >> $R
    fi
else
    echo "" >> $R
    echo "# Pass one (mosy) failed" >> $R
fi

if [ -z "$PERSON" ]; then
    $MHMAIL "$FROM" -subject "$SUBJECT" < $R
else
    $MHMAIL "$FROM" -subject "$SUBJECT" -from "$PERSON" -cc "$PERSON" < $R
fi

date | $GAWK '
BEGIN	{
	    months["Jan"] =  1;
	    months["Feb"] =  2;
	    months["Mar"] =  3;
	    months["Apr"] =  4;
	    months["May"] =  5;
	    months["Jun"] =  6;
	    months["Jul"] =  7;
	    months["Aug"] =  8;
	    months["Sep"] =  9;
	    months["Oct"] = 10;
	    months["Nov"] = 11;
	    months["Dec"] = 12;
	}
	{ printf "%2d/%2d %s mib-chec ", months[$2], $3, $4; }
'						 > $L
echo $$	| $GAWK '{ printf "%05d ", $1; }'	>> $L
whoami	| $GAWK '{ printf "(%-8s)  ", $1; }'	>> $L
echo "$FROM" "$SUBJECT"				>> $L
cat $L >> /var/tmp/mosy.log

exit 0
