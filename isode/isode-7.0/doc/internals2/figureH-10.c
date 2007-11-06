if (TReadRequest (sd, tx, secs, td) == NOTOK) {
    /* handle T-DISCONNECT.INDICATION in "td" */
}
else {
    /* handle T-DATA (or T-EXPEDITED-DATA) .INDICATION in "tx" */
}
