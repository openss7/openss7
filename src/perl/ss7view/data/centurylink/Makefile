
get:
	mkdir -p network-disclosures
	files=`wget -O - http://www.centurylink.com/wholesale/networkdisclosures.html | grep '<td><a href' | sed -r 's,^[\t ]*,,;s,">.*$$,,;s,<td><a href="([^/]),http://www.centurylink.com/wholesale/\1,;s,<td><a href="/,http://www.centurylink.com/,'` ; \
	for f in $$files; do \
		( \
			cd network-disclosures; \
			wget "$$f" \
		) ; \
	done
