//  ==========================================================================
//  
//  @(#) src/tcl/tcap/tcap.hh
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
//  ==========================================================================

#ifndef __LOCAL_TCAP_HH__
#define __LOCAL_TCAP_HH__

#pragma interface

//
// These are the definitions of a brief class library for interfacing with a
// Stream implementing the TCI interface.  Its purpose is to provide an
// object-oriented interface to SWIG primarily for generation of a JAVA API for
// use with JAIN TCAP.
//

extern "C" {
    typedef int np_ulong;
    typedef unsigned char uchar;

    typedef struct sccp_addr {
        np_ulong ni;			/* network indicator */
        np_ulong ri;			/* routing indicator */
        np_ulong pc;			/* point code (-1 not present) */
        np_ulong ssn;			/* subsystem number (0 not present) */
        np_ulong gtt;			/* type of global title */
        np_ulong tt;			/* translation type */
        np_ulong es;			/* encoding scheme */
        np_ulong nplan;			/* numbering plan */
        np_ulong nai;			/* nature of address indicator */
        np_ulong alen;			/* address length */
        uchar addr[0];			/* address digits */
        /* followed by address bytes */
    } sccp_addr_t;

    typedef unsigned int t_uscalar_t;
    typedef int t_scalar_t;

    struct t_opthdr {
        t_uscalar_t len;		/* Option length, incl. header. */
        t_uscalar_t level;		/* Option level. */
        t_uscalar_t name;		/* Option name. */
        t_uscalar_t status;		/* Negotiation result. */
        char value[0];			/* and onwards...  */
    };

    typedef struct TC_info_ack {
	t_scalar_t PRIM_type;		/* always TC_INFO_ACK */
	t_scalar_t TSDU_size;		/* maximum TSDU size */
	t_scalar_t ETSDU_size;		/* maximum ETSDU size */
	t_scalar_t CDATA_size;		/* connect data size */
	t_scalar_t DDATA_size;		/* disconnect data size */
	t_scalar_t ADDR_size;		/* maximum address size */
	t_scalar_t OPT_size;		/* maximum options size */
	t_scalar_t TIDU_size;		/* transaction interface data size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_flag;	/* provider flags */
	t_scalar_t TCI_version;		/* TCI version */
    } TC_info_ack_t;

}

namespace tci {
    using namespace std;

    class TciError {
        public:
            TciError();
            TciError(const char *msg);
            TciError(const char *function, const char *filename, const int lineno, int error);
    };

    class TcapData {
        private:
            int length;             // length of buffer
            unsigned char *buf;     // buffer start
            unsigned char *end;     // buffer end
            unsigned char *pos;     // position for read or write
        public:
            virtual ~TcapData(void);
            TcapData(int length);

            virtual void rewind(void);
            virtual void copyTo(char *destination, int length_of_destination);
            virtual int getLength(void);
            virtual void setLength(void);
    };

    class SccpAddress : public TcapData {
        private:
            struct sccp_addr addr;
        public:
            virtual ~SccpAddress(void);
            SccpAddress(int spc, int ssn);

            struct sccp_addr& getAddress(void);
            void setAddress(const struct sccp_addr& addr);
    };

    class TcapOption {
        private:
            struct t_opthdr hdr;
        public:
            virtual ~TcapOption(void);
            virtual void encodeTo(char *buffer, int length);
            static TcapOption& decodeFrom(char *buffer, int length);
    };

    class TcapOptionBuffer : public TcapData {
        public:
            virtual ~TcapOptionBuffer(void);
            TcapOptionBuffer(int length);

            void putOption(const TcapOption& option);
            TcapOption& getOption(void);
    };

    class TcapEvent {
    };

    class TcapDialogueEvent : public TcapEvent {
        private:
            int dialogue_id;
            ::std::list<TcapOption> options;
        public:
            virtual ~TcapDialogueEvent(void);
            TcapDialogueEvent(int id);
    };

    class TcapBegin : public TcapDialogueEvent {
        private:
            SccpAddress originating;
            SccpAddress terminating;
            bool permission;
        public:
            virtual ~TcapBegin(void);
            TcapBegin(int dialogue_id, const SccpAddress& originating, const SccpAddress& terminating, const ::std::list<TcapOption>& options, bool permission);
    };
    class TcapAccept : public TcapDialogueEvent {
        private:
            SccpAddress responding;
            bool permission;
        public:
            virtual ~TcapAccept(void);
            TcapAccept(int dialogue_id, const SccpAddress& responding, const ::std::list<TcapOption>& options, bool permission);
    };
    class TcapCont : public TcapDialogueEvent {
        private:
            bool permission;
        public:
            virtual ~TcapCont(void);
            TcapCont(int dialogue_id, const ::std::list<TcapOption>& options, bool permission);
    };
    class TcapEnd : public TcapDialogueEvent {
        private:
            int term_scenario;
        public:
            virtual ~TcapEnd(void);
            TcapEnd(int dialogue_id, const ::std::list<TcapOption>& options, int term_scenario);
    };
    class TcapAbort : public TcapDialogueEvent {
        private:
            int abort_reason;
        public:
            virtual ~TcapAbort(void);
            TcapAbort(int dialogue_id, const ::std::list<TcapOption>& options, int abort_reason);
    };
    class TcapUni : public TcapDialogueEvent {
        private:
            SccpAddress originating;
            SccpAddress terminating;
        public:
            virtual ~TcapUni(void);
            TcapUni(const SccpAddress& originating, const SccpAddress& terminating, const ::std::list<TcapOption>& options);
    };

    class TcapComponentEvent : public TcapEvent {
        private:
            int dialogue_id;
            int invoke_id;
        public:
            virtual ~TcapComponentEvent(void);
            TcapComponentEvent();
            TcapComponentEvent(int dialogue_id, int invoke_id);

            virtual int getDialogueId(void);
            virtual void setDialogueId(int dialogue_id);
            virtual int getInvokeId(void);
            virtual void setInvokeId(int invoke_id);
    };

    class TcapInvoke : public TcapComponentEvent {
        private:
            int operation;
            int pclass;
            int linked_id;
            bool more;
        public:
            virtual ~TcapInvoke(void);
            TcapInvoke(int dialogue_id, int invoke_id, int operation, int pclass, int linked_id, const TcapData& data, bool more, int timeout);

            virtual int getOperation(void);
            virtual void setOperation(int operation);
            virtual int getOperationClass(void);
            virtual void setOperationClass(int pclass);
            virtual int getLinkedId(void);
            virtual void setLinkedId(int linked_id);
            virtual bool getMore(void);
            virtual void setMore(bool more);
            virtual int getTimeout(void);
            virtual void setTimeout(int timeout);
    };
    class TcapResult : public TcapComponentEvent {
        private:
            int operation;
            bool more;
        public:
            virtual ~TcapResult(void);
            TcapResult(int dialogue_id, int invoke_id, int operation, bool more);

            virtual int getOperation(void);
            virtual void setOperation(int operation);
            virtual bool getMore(void);
            virtual void setMore(bool more);
    };

    class TcapError : public TcapComponentEvent {
        private:
            int error;
            bool more;
        public:
            virtual ~TcapError(void);
            TcapError(int dialogue_id, int invoke_id, int error, bool more);

            virtual int getError(void);
            virtual void setError(int error);
            virtual bool getMore(void);
            virtual void setMore(bool more);
    };

    class TcapCancel : public TcapComponentEvent {
        public:
            virtual ~TcapCancel(void);
            TcapCancel(int dialogue_id, int invoke_id);
    };

    class TcapReject : public TcapComponentEvent {
        private:
            int problem;
        public:
            virtual ~TcapReject(void);
            TcapReject(int dialogue_id, int invoke_id, int problem);

            virtual int getProblem(void);
            virtual void setProblem(int problem);
    };

    class TcapManagement : public TcapEvent {
        private:
            SccpAddress address;
        public:
            virtual ~TcapManagement(void);
            TcapManagement(const SccpAddress& address);

            virtual SccpAddress& getSccpAddress(void);
            virtual void setSccpAddress(const SccpAddress& address);
    };

    class TcapCoord : public TcapManagement {
        public:
            virtual ~TcapCoord(void);
            TcapCoord(const SccpAddress& address);
    };
    class TcapAllow : public TcapManagement {
        public:
            virtual ~TcapAllow(void);
            TcapAllow(const SccpAddress& address);
    };
    class TcapState : public TcapManagement {
        private:
            int status;
        public:
            virtual ~TcapState(void);
            TcapState(const SccpAddress& address, int status);

            virtual int getStatus(void);
            virtual void setStatus(int status);
    };

    class TcapStream {
        private:
            int fd;                                 // file descriptor
            struct TC_info_ack info;                // TC information
            ::std::list<TcapOption> dflt_options;         // Default options
            ::std::list<TcapOption> curr_options;         // Current options
        protected:
            virtual int bind(SccpAddress& addr, int xacts);
            virtual int unbind(void);
            virtual int optmgmt(::std::list<TcapOption>& options, int flags);
            virtual int begin(const SccpAddress& originating, const SccpAddress& terminating, int& dialogue_id, const ::std::list<TcapOption>& options, TcapData& data, bool permission);
            virtual int accept(const SccpAddress& responding, int dialogue_id, const ::std::list<TcapOption>& options, TcapData& data, bool permission);
            virtual int cont(int dialogue_id, const ::std::list<TcapOption>& options, TcapData& data, bool permission);
            virtual int end(int dialogue_id, const ::std::list<TcapOption>& options, TcapData& data, int term_scenario);
            virtual int abort(int dialogue_id, const ::std::list<TcapOption>& options, TcapData& data, int abort_reason);
            virtual int uni(SccpAddress& originating, SccpAddress& terminating, ::std::list<TcapOption>& options, TcapData& data);

            virtual int invoke(int dialogue_id, int pclass, int operation, int& invoke_id, int linked_id, TcapData& data, bool more, int timeout);
            virtual int result(int dialogue_id, int invoke_id, int operation, TcapData& data, bool more);
            virtual int error(int dialogue_id, int invoke_id, int error, TcapData& data, bool more);
            virtual int cancel(int dialogue_id, int invoke_id);
            virtual int reject(int dialogue_id, int invoke_id, TcapData& data, int problem);

            virtual int coord(const SccpAddress& address);
            virtual int allow(const SccpAddress& address);
            virtual int state(const SccpAddress& address, int status);
        public:
            virtual ~TcapStream(void);
            TcapStream(const char *devname);
    };

    class TcapEndPoint : public TcapStream {
        public:
            virtual ~TcapEndPoint(void);
            TcapEndPoint(const char *devname);

            void putEvent(TcapEvent& msg);
            TcapEvent& getEvent(int timeout=0);
    };
};

#endif				/* __LOCAL_TCAP_HH__ */

// vim: ft=cpp sw=4 et
