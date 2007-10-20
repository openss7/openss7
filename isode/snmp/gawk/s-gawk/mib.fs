BEGIN {
	printf "%-20.20s  %6.6s  %6.6s  %6.6s %8.8s  %-20.20s\n", 
	    "Filesystem", "kbytes", "used", "avail", "capacity", "Mounted on";

	didone = 0;
	for (i in fsIdentifier) {
		didone = 1;

		kbyte = (fsBlockSize * fsBlockCount) / 1024;
		used  = ((fsBlockCount - fsBlocksFree) * fsBlockSize) / 1024;
		avail = (fsBlocksAvailable * fsBlockSize ) / 1024;
		cap   = (1 - (avail / kbyte) ) * 100;

		if( length(fsName) > 20 )
			printf "%s\n                    ", fsName;
		else
			printf "%-20.20s", fsName;

		printf "  %6.0d  %6.0d  %6.0d  %4.0d%%    %-s\n",
		    kbyte, used, avail, cap, fsMountPoint;		
	}

	if (!didone && DIAGNOSTIC)
		printf "fsTable: %s\n", DIAGNOSTIC;
    }
