
%: %.c
	$(CC) -I../include -O3 -o $@ $<

ETSI = esti8 etsi1 etsi10 etsi2 etsi3 etsi4 etsi5 etsi6 etsi7 etsi8 etsi9
TEST = test1 test10 test11 test12 test13 test2 test3 test4 test5 test6 test7 test8 test9
PERF = test-tcps test-tcpc test-udps test-udpc test-sctps test-sctpc
DELY = test-tcp-ds test-tcp-dc test-sctp-ds test-sctp-dc
THPT = test-tcp-ts test-tcp-tc test-sctp-ts test-sctp-tc

OBJS = ${ETSI} ${TEST} ${PERF} ${DELY} ${THPT}

all: ${OBJS}

clean:
	rm -f ${OBJS}
