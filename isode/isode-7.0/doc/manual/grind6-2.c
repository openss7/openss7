#include <stdio.h>
#include <isode/tsap.h>
#include <isode/isoservent.h>


/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int     result,
	    sd;
    struct TSAPstart    tss;
    register struct TSAPstart  *ts = &tss;
    struct TSAPdata txs;
    register struct TSAPdata  *tx = &txs;
    struct TSAPdisconnect   tds;
    register struct TSAPdisconnect *td = &tds;

    if (TInit (argc, argv, ts, td) == NOTOK)
	error ("T-CONNECT.INDICATION: %s", TErrString (td -> td_reason));
    sd = ts -> ts_sd;

/* examine argv here, if need be */

    if (TConnResponse (sd, &ts -> ts_called, ts -> ts_expedited,
		ts -> ts_data, ts -> ts_len, NULLQOS, td) == NOTOK)
	error ("T-CONNECT.RESPONSE: %s", TErrString (td -> td_reason));

...
