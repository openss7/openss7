/*
 *  In accordance with draft-ietf-sigtran-sctp-mib-07
 */
struct sctp_mib
{
	unsigned long	SctpRtoAlgorithm;
	unsigned long	SctpRtoMin;
	unsigned long	SctpRtoMax;
	unsigned long	SctpRtoInitial;
	unsigned long	SctpMaxAssoc;
	unsigned long	SctpValCookieLife;
	unsigned long	SctpMaxInitRetr;
	unsigned long	SctpCurrEstab;
	unsigned long	SctpActiveEstabs;
	unsigned long	SctpPassiveEstabs;
	unsigned long	SctpAborteds;
	unsigned long	SctpShutdowns;
	unsigned long	SctpOutOfBlues;
	unsigned long	SctpChecksumErrors;
	unsigned long	SctpOutCtrlChunks;
	unsigned long	SctpOutOrderChunks;
	unsigned long	SctpOutUnorderChunks;
	unsigned long	SctpInCtrlChunks;
	unsigned long	SctpInOrderChunks;
	unsigned long	SctpInUnorderChunks;
	unsigned long	SctpFragUsrMsgs;
	unsigned long	SctpReasmUsrMsgs;
	unsigned long	SctpOutSCTPPacks;
	unsigned long	SctpInSCTPPacks;
	unsigned long	SctpDiscontinuityTime;
	unsigned long   __pad[0];
} ____cacheline_aligned;

