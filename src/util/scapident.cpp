//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

static char const ident[] = "$Id$";

// The purpose of this program is to identify the characteristics of a number of
// files.  It is the basis for the monitoring program.

extern "C" {
#       include <stropts.h>
#       include <stdlib.h>
#       include <unistd.h>
#       include <stdio.h>
#	include <string.h>
#	include <errno.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/poll.h>
#	include <sys/time.h>
#	include <sys/ioctl.h>
#	include <sys/stat.h>
#	include <fcntl.h>

#ifdef _GNU_SOURCE
#	include <getopt.h>
#endif

#	include <time.h>
#	include <signal.h>
#	include <syslog.h>
#	include <sys/utsname.h>

#	include <pcap/pcap.h>
}

#include <list>
#include <vector>
#include <algorithm>
#include <hash_map>

using std::list;
using std::vector;
using __gnu_cxx::hash_map;

int output = -1;
int debug = 0;

class Error;
class MsgStat;
class File;
class FileText;
class FilePcap;
class Path;
class Message;

class Error {
      private:
	int error;
      public:
	Error(int val) : error(val) { };
	virtual ~Error(void) { };
};

struct pseudohdr {
	unsigned char dir;
	unsigned char xsn;
	unsigned char lkno[2];
};

//  -------------------------------------------------------------------------
//
//  MsgStat class
//
//  -------------------------------------------------------------------------
//  This is a base class for things that count messages.
class MsgStat {
      protected:
	ulong msgs;
	ulong bytes;
	ulong bads;
	ulong fisus;
	ulong lssus;
	ulong lssu2s;
	ulong msus;
	ulong sis[16];
	ulong undec;
	struct timeval beg, end;

      public:
	MsgStat(void) : msgs(0), bytes(0), bads(0), fisus(0), lssus(0), lssu2s(0), msus(0), sis(), undec(0) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		beg.tv_sec = 0x7fffffff;
		beg.tv_usec = 0x7fffffff;
		end.tv_sec = 0;
		end.tv_usec = 0;
		for (int i = 0; i < 16; i++)
			sis[i] = 0;
	};
	virtual ~MsgStat(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual void identify(void);
	virtual void add(Message& msg);
};

//  -------------------------------------------------------------------------
//
//  File class
//
//  -------------------------------------------------------------------------
class File : public MsgStat {
      protected:
	File() { };
	FILE *fp;
	char fname[256];
	list<Path *> fpaths;
	void delall(void);

      public:
	File(char *path) : MsgStat(), fp(NULL), fpaths() {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		if ((fp = fopen(path, "r")) == NULL) {
			perror(__PRETTY_FUNCTION__);
			throw Error(1);
		}
		strncpy(fname, path, 255);
	};
	virtual ~File(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		delall();
	};
	void add(Path *path) {
		fpaths.push_back(path);
	};
	void del(Path *path) {
		for (list<Path *>::iterator it = fpaths.begin(); it != fpaths.end(); ++it) {
			if (*it == path) {
				if (debug > 1)
					fprintf(stderr, "I: %s: erasing path %p\n", __PRETTY_FUNCTION__, path);
				fpaths.erase(it);
				break;
			}
		}
	};
	virtual void add(Message& msg);
	virtual Message *readmsg(void) = 0;
	virtual void readmsgs(void);
	virtual void identify(void);
};

//  -------------------------------------------------------------------------
//
//  FileText class
//
//  -------------------------------------------------------------------------
//  Represents a text file.  When text files are used, each file can potentially
//  represent a different data link path.  However, we have no idea which data
//  link path that might be (no identifier) until we read the header.
//
class FileText : public File {
      private:
	FileText(void) { };
	uint coms;
	char buf[1024];
	int ppa;
	struct pseudohdr phdr;
	int line;

      public:
	FileText(char *path) : File(path), coms(0), ppa(0), line(1) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		int ret, i1, i2;
		char c1, c2;
		if ((ret = fscanf(fp, "%u.%u %c%c", &i1, &i2, &c1, &c2)) < 4) {
			if (debug)
				fprintf(stderr, "W: %s: cannot parse file %s\n", __PRETTY_FUNCTION__, path);
			throw Error(1);
		}
		if (c1 != '#' || c2 != ' ') {
			fprintf(stderr, "E: %s: first characters are '%c%c' not '# '\n", __PRETTY_FUNCTION__, c1, c2);
			throw Error(1);
		}
		rewind(fp);
	};
	virtual ~FileText(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		fclose(fp);
	};
	virtual Message *readmsg(void);
	virtual void identify(void);
};

//  -------------------------------------------------------------------------
//
//  FilePcap class
//
//  -------------------------------------------------------------------------
//  This class represents a pcap formatted file.
//
class FilePcap : public File {
      private:
	pcap_t *pp;
	char errbuf[PCAP_ERRBUF_SIZE];

      public:
	FilePcap(char *path) : File(path), pp(NULL) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		errbuf[0] = '\0';
		if ((pp = pcap_fopen_offline(fp, errbuf)) == NULL) {
			fprintf(stderr, "error: %s: %s\n", __PRETTY_FUNCTION__, errbuf);
			throw Error(1);
		}
	};
	virtual ~FilePcap(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		pcap_close(pp);
	};
	int getDLT(void) const {
		return pcap_datalink(pp);
	};
	const char *getDLTName(void) const {
		return pcap_datalink_val_to_name(pcap_datalink(pp));
	};
	const char *getDLTDesc(void) const {
		return pcap_datalink_val_to_description(pcap_datalink(pp));
	};
	virtual Message *readmsg(void);
	virtual void identify(void);
};

hash_map<int,Path *> paths;

//  -------------------------------------------------------------------------
//
//  Path class
//
//  -------------------------------------------------------------------------
//  Each path object represents a channel of communication.
//
enum HeaderType {
	HT_UNKNOWN,
	HT_BASIC,
	HT_EXTENDED
};
enum RlType {
	RL_UNKNOWN,
	RL_14BIT_PC,
	RL_24BIT_PC
};
enum MsgPriority {
	MP_UNKNOWN,
	MP_JAPAN,
	MP_NATIONAL,
	MP_INTERNATIONAL
};
class Path : public MsgStat {
      private:
	// static hash_map<int,Path *> paths;
	File& file;
	int ppa;
	enum HeaderType ht;
	enum MsgPriority pr;
	enum RlType rt;

      public:
	Path(File& f, int a) : file(f), ppa(a), ht(HT_UNKNOWN),
		pr(MP_UNKNOWN), rt(RL_UNKNOWN)
	{
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		paths[ppa] = this;
		file.add(this);
		if (debug > 1)
			fprintf(stderr, "I: %s: created (%d:%d:%d) %p\n", __PRETTY_FUNCTION__,
					cardNum(),spanNum(),slotNum(),this);
		if (slotNum() == 0)
			setHeaderType(HT_EXTENDED);
		else
			setHeaderType(HT_BASIC);
	};
	virtual ~Path(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		file.del(this);
		paths.erase(ppa);
	};
	static Path& get(File& f, int a) {
		Path *p;
		hash_map<int,Path *>::iterator it;
		if ((it = paths.find(a)) == paths.end()) {
			if ((p = new Path(f, a)) == NULL)
				throw Error(1);
		} else
			p = paths[a];
		return (*p);
	};
	int cardNum(void) { return ((ppa >> 7) & 0x01); };
	int spanNum(void) { return ((ppa >> 5) & 0x03); };
	int slotNum(void) { return ((ppa >> 0) & 0x1f); };
	virtual void identify(void);
	virtual void add(Message& msg);
	enum HeaderType headerType(void) { return (ht); }
	void setHeaderType(enum HeaderType t) {
		if (ht != t && t != HT_UNKNOWN) {
			switch (ht) {
			case HT_UNKNOWN:
				if (debug) {
					if (t == HT_BASIC)
						fprintf(stderr, "I: path header type set to basic\n");
					else if (t == HT_EXTENDED)
						fprintf(stderr, "I: path header type set to extended\n");
				}
				break;
			case HT_BASIC:
				if (debug && t == HT_EXTENDED)
					fprintf(stderr, "W: path header type change from basic to extended\n");
				break;
			case HT_EXTENDED:
				if (debug && t == HT_BASIC)
					fprintf(stderr, "W: path header type change from extended to basic\n");
				break;
			}
			ht = t;
		}
	};
	enum MsgPriority msgPriority(void) { return (pr); };
	void setMsgPriority(enum MsgPriority p) {
		if (pr != p && p != MP_UNKNOWN) {
			switch (pr) {
			case MP_UNKNOWN:
				if (debug) {
					if (p == MP_JAPAN)
						fprintf(stderr, "I: msg priority set to japan\n");
					else if (p == MP_NATIONAL)
						fprintf(stderr, "I: msg priority set to national\n");
					else if (p == MP_INTERNATIONAL)
						fprintf(stderr, "I: msg priority set to international\n");
				}
				break;
			case MP_JAPAN:
				if (debug) {
					if (p == MP_NATIONAL)
						fprintf(stderr, "W: msg priority change from japan to national\n");
					if (p == MP_INTERNATIONAL)
						fprintf(stderr, "W: msg priority change from japan to international\n");
				}
				break;
			case MP_NATIONAL:
				if (debug) {
					if (p == MP_JAPAN)
						fprintf(stderr, "W: msg priority change from national to japan\n");
					if (p == MP_INTERNATIONAL)
						fprintf(stderr, "W: msg priority change from national to international\n");
				}
				break;
			case MP_INTERNATIONAL:
				if (debug) {
					if (p == MP_NATIONAL)
						fprintf(stderr, "W: msg priority change from international to national\n");
					if (p == MP_JAPAN)
						fprintf(stderr, "W: msg priority change from international to japan\n");
				}
				break;
			}
			pr = p;
		}
	};
	enum RlType routingLabel(void) { return (rt); };
	void setRoutingLabel(enum RlType r) {
		if (rt != r && r != RL_UNKNOWN) {
			switch (rt) {
			case RL_UNKNOWN:
				if (debug) {
					if (r == RL_24BIT_PC)
						fprintf(stderr, "W: routing label set to 24-bit\n");
					if (r == RL_14BIT_PC)
						fprintf(stderr, "W: routing label set to 14-bit\n");
				}
				break;
			case RL_14BIT_PC:
				if (debug && r == RL_24BIT_PC)
					fprintf(stderr, "W: routing label change from 14-bit to 24-bit\n");
				break;
			case RL_24BIT_PC:
				if (debug && r == RL_14BIT_PC)
					fprintf(stderr, "W: routing label change from 24-bit to 14-bit\n");
				break;
			}
		}
	};
};

//  -------------------------------------------------------------------------
//
//  Message class
//
//  -------------------------------------------------------------------------
//  Each message object represents an SS7 message.
//
enum MessageType {
	MT_UNKNOWN,
	MT_FISU,
	MT_LSSU,
	MT_LSSU2,
	MT_MSU
};
class Message {
      private:
	Path& path;

      public:
	struct pcap_pkthdr hdr;
	unsigned char *dat;
	struct pseudohdr phdr;
	bool mtp2decode;
	bool mtp3decode;
	unsigned char *beg;
	unsigned char *mid;
	unsigned char *end;
	unsigned int bib, bsn, fib, fsn, li, li0, ni, mp, si, dpc, opc, sls, cic, mt;

	Message(Path& p, struct pcap_pkthdr *h, unsigned char *d, struct pseudohdr *ph) :
		path(p), hdr(*h), dat(d), phdr(*ph), mtp2decode(!!0), mtp3decode(!!0),
		beg(d), mid(d), end(d+h->caplen)
	{
		if (debug > 3)
			fprintf(stderr, "I: %s\n", __PRETTY_FUNCTION__);
		if (3 <= hdr.len && hdr.len <= 5)
			path.setHeaderType(HT_BASIC);
		if (6 <= hdr.len && hdr.len <= 8)
			path.setHeaderType(HT_EXTENDED);
		switch (path.headerType()) {
		case HT_UNKNOWN:
			fprintf(stderr, "unknown header type\n");
			return;
		case HT_BASIC:
			// basic link header
			bsn = *mid & 0x7f;
			bib = (*mid++ & 0x80) >> 7;
			fsn = *mid & 0x7f;
			bib = (*mid++ & 0x80) >> 7;
			li = *mid & 0x3f;
			if ((li != hdr.len - 3) && (hdr.len - 3 <= 63 || li != 63))
				fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 3);
			mp = (*mid++ & 0xc0) >> 6;
			if (mp != 0) {
				path.setRoutingLabel(RL_24BIT_PC);
				path.setMsgPriority(MP_JAPAN);
			}
			break;
		case HT_EXTENDED:
			// annex a link header
			bsn = (*mid++ & 0xff);
			bsn |= ((*mid & 0x0f) << 8);
			bib = (*mid++ & 0x80) >> 7;
			fsn = (*mid++ & 0xff);
			fsn |= ((*mid & 0x0f) << 8);
			fib = (*mid++ & 0x80) >> 7;
			li = (*mid++ & 0xff);
			li |= ((*mid & 0x01) << 8);
			if ((li != hdr.len - 6) && (hdr.len - 6 <= 63 || li != 63))
				fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 6);
			mp = (*mid++ & 0xc0) >> 6;
			if (mp != 0) {
				path.setRoutingLabel(RL_24BIT_PC);
				path.setMsgPriority(MP_JAPAN);
			}
			break;
		}
		mtp2decode = !0;
		switch (li) {
		case 0:
			return;
		case 1:
			si = (*mid++ & 0xff);
			return;
		case 2:
			si = (*mid++ & 0xff);
			mid++;
			return;
		}
		si = *mid & 0x0f;
		ni = (*mid & 0xc0) >> 6;
		if (ni == 0) {
			path.setRoutingLabel(RL_14BIT_PC);
			path.setMsgPriority(MP_INTERNATIONAL);
		}
		switch (path.msgPriority()) {
		case MP_UNKNOWN:
			mp = (*mid & 0x30) >> 4;
			break;
		case MP_JAPAN:
			break;
		case MP_NATIONAL:
			mp = (*mid & 0x30) >> 4;
			break;
		case MP_INTERNATIONAL:
			mp = 0;
			break;
		}
		mid++;
		if (li < 6) {
			fprintf(stderr, "too short for RL, li = %d", li);
			return;
		}
		if (li < 9)
			path.setRoutingLabel(RL_14BIT_PC);
		else if (si == 0x05 && li < 11)
			path.setRoutingLabel(RL_14BIT_PC);
		if (path.getRoutingLabel() == RL_UNKNOWN) {
		}
	};
	virtual ~Message(void) {
		if (debug > 3)
			fprintf(stderr, "I: %s\n", __PRETTY_FUNCTION__);
		free(dat);
	};
	bool isFisu(void) { return ((hdr.len == 3) || (hdr.len == 6)); };
	bool isLssu(void) { return ((hdr.len == 4) || (hdr.len == 7)); };
	bool isLssu2(void) { return ((hdr.len == 5) || (hdr.len == 6)); };
	bool isMsu(void) { return (hdr.len > 6); };
	enum MessageType messageType(void) {
		switch (hdr.len) {
		case 0: case 1: case 2: return (MT_UNKNOWN);
		case 3: case 6: return (MT_FISU);
		case 4: case 7: return (MT_LSSU);
		case 5: case 8: return (MT_LSSU2);
		default: return (MT_MSU);
		}
	};
	bool mtp2Decoded(void) { return mtp2decode; };
	bool mtp3Decoded(void) { return mtp3decode; };
	int siValue(void) {
		if (!mtp2decode)
			return (-1);
		return (si);
	};
};

void
File::delall(void)
{
	list<Path *>::iterator it;
	while ((it = fpaths.begin()) != fpaths.end()) {
		Path *p = (*it);
		if (debug > 1)
			fprintf(stderr, "I: %s: deleting path %p\n", __PRETTY_FUNCTION__, p);
		delete p;
	}
}

void
MsgStat::add(Message& msg)
{
	msgs++;
	switch (msg.messageType()) {
	case MT_UNKNOWN:
		bads++;
		break;
	case MT_FISU:
		fisus++;
		break;
	case MT_LSSU:
		lssus++;
		break;
	case MT_LSSU2:
		lssu2s++;
		break;
	case MT_MSU:
		msus++;
		if (msg.siValue() != -1)
			sis[msg.siValue()]++;
		else
			undec++;
		break;
	}
	bytes += msg.hdr.len;
	if ((msg.hdr.ts.tv_sec < beg.tv_sec) ||
	   ((msg.hdr.ts.tv_sec == beg.tv_sec) && (msg.hdr.ts.tv_usec < beg.tv_usec)))
		beg = msg.hdr.ts;
	if ((msg.hdr.ts.tv_sec > end.tv_sec) ||
	   ((msg.hdr.ts.tv_sec == end.tv_sec) && (msg.hdr.ts.tv_usec > end.tv_usec)))
		end = msg.hdr.ts;
}
void
File::add(Message& msg)
{
	MsgStat::add(msg);
}
void
Path::add(Message& msg)
{
	MsgStat::add(msg);
	file.add(msg);
}

void
MsgStat::identify(void)
{
	fprintf(stdout, "Message count %lu\n", msgs);
	fprintf(stdout, "Message bytes %lu\n", bytes);
	if (msgs > 0) {
		double avglen = (double) bytes/ (double) msgs;
		fprintf(stdout, "Message length %g bytes (avg)\n", avglen);
	}
	fprintf(stdout, "Message bad   %lu\n", bads);
	fprintf(stdout, "Message fisu  %lu\n", fisus);
	fprintf(stdout, "Message lssu  %lu\n", lssus);
	fprintf(stdout, "Message lssu2 %lu\n", lssu2s);
	fprintf(stdout, "Message msu   %lu\n", msus);
	if (sis[0]) fprintf(stdout, "        snmm  %lu\n", sis[0]);
	if (sis[1]) fprintf(stdout, "        sntm  %lu\n", sis[1]);
	if (sis[2]) fprintf(stdout, "        snsm  %lu\n", sis[2]);
	if (sis[3]) fprintf(stdout, "        sccp  %lu\n", sis[3]);
	if (sis[4]) fprintf(stdout, "        tup   %lu\n", sis[4]);
	if (sis[5]) fprintf(stdout, "        isup  %lu\n", sis[5]);
	if (sis[6]) fprintf(stdout, "        dup1  %lu\n", sis[6]);
	if (sis[7]) fprintf(stdout, "        dup2  %lu\n", sis[7]);
	if (sis[8]) fprintf(stdout, "        mtup  %lu\n", sis[8]);
	if (sis[9]) fprintf(stdout, "        bisup %lu\n", sis[9]);
	if (sis[10]) fprintf(stdout, "        siup  %lu\n", sis[10]);
	if (sis[11]) fprintf(stdout, "        spnep %lu\n", sis[11]);
	if (sis[12]) fprintf(stdout, "        stc   %lu\n", sis[12]);
	if (sis[13]) fprintf(stdout, "        si13  %lu\n", sis[13]);
	if (sis[14]) fprintf(stdout, "        si14  %lu\n", sis[14]);
	if (sis[15]) fprintf(stdout, "        si15  %lu\n", sis[15]);
	if (undec) fprintf(stdout, "        undec %lu\n", undec);
	if (beg.tv_sec != 0x7fffffff || beg.tv_usec != 0x7fffffff) {
		fprintf(stdout, "Timestamp beg %012ld.%06ld\n", beg.tv_sec, beg.tv_usec);
	}
	if (end.tv_sec != 0 || end.tv_usec != 0) {
		fprintf(stdout, "Timestamp end %012ld.%06ld\n", end.tv_sec, end.tv_usec);
	}
	if ((beg.tv_sec != 0x7fffffff || beg.tv_usec != 0x7fffffff) &&
	    (end.tv_sec != 0 || end.tv_usec != 0)) {
		struct timeval dur;
		dur.tv_sec = end.tv_sec - beg.tv_sec;
		dur.tv_usec = end.tv_usec - beg.tv_usec;
		if (dur.tv_usec < 0) {
			dur.tv_usec += 1000000;
			dur.tv_sec -= 1;
		}
		fprintf(stdout, "Timestamp dur %012ld.%06ld\n", dur.tv_sec, dur.tv_usec);
	}
}
void
Path::identify(void)
{
	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Path (%d:%d:%d):\n", cardNum(), spanNum(), slotNum());
	MsgStat::identify();
}
void
File::identify(void)
{
	this->readmsgs();
	MsgStat::identify();
	for (list<Path *>::iterator it = fpaths.begin(); it != fpaths.end(); ++it) {
		(*it)->identify();
	}
}
void
FileText::identify(void)
{
	fprintf(stdout, "===========================================\n");
	fprintf(stdout, "File %s is a text file\n", fname);
	fprintf(stdout, "Comment lines %u\n", coms);
	File::identify();
	fprintf(stdout, "===========================================\n");
}
void
FilePcap::identify(void)
{
	fprintf(stdout, "===========================================\n");
	fprintf(stdout, "File %s is a pcap file\n", fname);
	fprintf(stdout, "Data link type %d\n", getDLT());
	fprintf(stdout, "Data link name %s\n", getDLTName());
	fprintf(stdout, "Data link desc %s\n", getDLTDesc());
	File::identify();
	fprintf(stdout, "===========================================\n");
}

Message *
FileText::readmsg(void)
{
	Message *m = NULL;
	size_t len, len1, len2;
	char *s;

	for (; (s = fgets(buf, 1024, fp)) != NULL; line++) {
		len = strnlen(buf, 1024);
		if (len < 22) {
			// aborted files can have short lines
			if (debug > 0) {
				fprintf(stderr, "W: syntax error line %d: %s\n", line, buf);
				fprintf(stderr, "W: line too short: len = %zu\n", len);
			}
			continue;
		}
		// note buf includes the \n (unless file was aborted)
		if (buf[len-1] != '\n') {
			// aborted files can have lines with no termination
			if (debug > 0) {
				fprintf(stderr, "W: syntax error line %d: %s\n", line, buf);
				fprintf(stderr, "W: no line termination\n");
			}
			continue;
		}
		buf[len-1] = '\0';
		if (buf[19] != ' ') {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: character position 19 is '%c' not ' '\n", buf[19]);
			continue;
		}
		if (buf[20] == '#') {
			// comment line
			coms++;
			if (strncmp("device:", &buf[22], 7) == 0) {
				int card, span, slot;
				if (sscanf(&buf[44], "(%d:%d:%d)", &card, &span, &slot) == 3) {
					ppa = ((card & 0x0f) << 12) |
						((span & 0x0f) << 8) |
						((slot & 0xff) << 0);
					phdr.lkno[0] = (ppa >> 8) & 0xff;
					phdr.lkno[1] = (ppa >> 0) & 0xff;
				}
			}
			continue;
		}
		len2 = strnlen(&buf[20], 1024-20);
		if (len2 & 0x01) {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: message length %zu is odd\n", len2);
			continue;
		}
		if ((len1 = strspn(&buf[20], "0123456789abcdefABCDEF")) != len2) {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: len1 = %zu, len2 = %zu\n", len1, len2);
			continue;
		}
		struct pcap_pkthdr hdr;
		unsigned char *dat;
		sscanf(buf, "%ld.%ld", &hdr.ts.tv_sec, &hdr.ts.tv_usec);
		hdr.caplen = hdr.len = strnlen(&buf[20], 1024-20) / 2;
		if ((dat = (unsigned char *)malloc(hdr.len)) == NULL)
			throw Error(1);
		for (uint i = 0; i < hdr.len * 2; i+=2) {
			unsigned char val = 0;
			for (int j = 0; j < 2; j++, val<<=4) {
				unsigned char c = buf[20+i+j];
				if ('0' <= c && c <= '9')
					val += c - '0';
				else if ('a' <= c && c <= 'f')
					val += c - 'a' + 10;
				else if ('A' <= c && c <= 'F')
					val += c - 'A' + 10;
			}
			dat[i/2] = val;
		}
		Path &p = Path::get(*this, ppa);
		if ((m = new Message(p, &hdr, dat, &phdr)) == NULL)
			throw Error(1);
		p.add(*m);
		break;
	}
	return (m);
}

Message *
FilePcap::readmsg(void)
{
	Message *m = NULL;
	int ret;
	struct pcap_pkthdr *pcap_hdr, hdr;
	struct pseudohdr *phdr;
	const u_char *dat;
        unsigned char *buf;
	int ppa;

	if ((ret = pcap_next_ex(pp, &pcap_hdr, &dat)) == 1) {
		phdr = (struct pseudohdr *)dat;
		dat += sizeof(*phdr);
		hdr = *pcap_hdr;
		hdr.caplen -= sizeof(*phdr);
		hdr.len -= sizeof(*phdr);
		ppa = (phdr->lkno[1] << 8) | (phdr->lkno[1] << 0);
		Path &p = Path::get(*this, ppa);
		if ((buf = (unsigned char *)malloc(hdr.len)) == NULL)
			throw Error(1);
		memcpy(buf, dat, hdr.len);
		if ((m = new Message(p, &hdr, buf, phdr)) == NULL)
			throw Error(1);
		p.add(*m);
	}
	return (m);
}

void
File::readmsgs(void)
{
	Message *m;

	while ((m = this->readmsg()) != NULL)
		delete m;
}

static void
scapident(int num, char *fnames[])
{
	for (int i = 0; i < num; i++) {
		File *f;

		try {
			f = new FileText(fnames[i]);
		}
		catch(Error e1) {
			try {
				f = new FilePcap(fnames[i]);
			}
			catch(Error e2) {
				f = NULL;
			}
		}
		if (f == NULL) {
			fprintf(stderr, "E: error opening file %s\n", fnames[i]);
			continue;
		}
		f->identify();
		delete f;
		f = NULL;
	}
}

//  =========================================================================
//
//  MAIN and OPTIONS HANDLING
//
//  =========================================================================

void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision$ $Date$");
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-o|--outfile} outfile] [{-p|--parfile} parfile] files [...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] files [...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    files [...]\n\
        one or more files to use for input\n\
Options:\n\
  General Options:\n\
    -q, --quiet                       (default: off)\n\
        suppress normal output\n\
    -D, --debug [LEVEL]               (default: %2$d)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]             (default: %3$d)\n\
        increase or set verbosity LEVEL\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints version and exits\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], debug, output);
}

int
main(int argc, char **argv)
{
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int
		 option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
		    {"quiet",   no_argument,        NULL,   'q'},
		    {"debug",   optional_argument,  NULL,   'D'},
		    {"verbose", optional_argument,  NULL,   'v'},
		    {"help",    no_argument,        NULL,   'h'},
		    {"version", no_argument,        NULL,   'V'},
		    {"copying", no_argument,        NULL,   'C'},
		    {"?",       no_argument,        NULL,   'H'},
		    {NULL,      0,                  NULL,    0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'q':	// -q, --quiet
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'D':	// -D, --debug [LEVEL]
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	// -v, --verbose [LEVEL]
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	// -h, --help
		case 'H':	// -H, --?
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	// -V, --version
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	// -C, --copying
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
				}
				fprintf(stderr, "\n");
				fflush(stderr);
				goto bad_usage;
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	scapident(argc - optind, &argv[optind]);
	exit(4);
}
