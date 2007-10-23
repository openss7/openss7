/* perform T-CONNECT.REQUEST */

if (TConnRequest (NULLTA, ta, 1, NULLCP, 0, NULLQOS, tc, td) == NOTOK) {
    /* handle T-DISCONNECT.INDICATION in "td" */
}
else {
    /* handle T-CONNECT.CONFIRMATION in "tc" */
}
