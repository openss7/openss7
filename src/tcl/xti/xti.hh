
%module Xti
%{
%}

%include "typemaps.i"

extern "C" {
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int u_int32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned short u_int16_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef unsigned char u_int8_t;

#define _XOPEN_SOURCE 600

%include "sys/xti.h"
%include "sys/xti_inet.h"
%include "sys/xti_atm.h"
%include "sys/xti_osi.h"
%include "sys/xti_mosi.h"
}

class xti {
    public:
	int accept(int, struct t_call const *INPUT);
	int addleaf(int, struct netbuf *);
	void *alloc(int, int);
	int bind(struct t_bind const *INPUT, struct t_bind *OUTPUT);
	int close(void);
	int connect(struct t_call const *INPUT, struct t_call *OUTPUT);
	static int error(char const*);
	static int free(void *, int);
	int getinfo(struct t_info *OUTPUT);
	int getprotaddr(struct t_bind *OUTPUT, struct t_bind *OUTPUT);
	int getstate(void);
	int listen(struct t_call *OUTPUT);
	int look(void);
	static xti* open(char const *INPUT, int INPUT, struct t_info *OUTPUT);
	int optmgmt(struct t_optmgmt const *INPUT, struct t_optmgmt *OUTPUT);
	int rcv(char *OUTPUT, unsigned int INPUT, int *OUTPUT);
	int rcvconnect(struct t_call *OUTPUT);
	int rcvdis(struct t_discon *OUTPUT);
	int rcvleafchange(struct t_leaf_status *OUTPUT);
	int rcvopt(struct t_unitdata *OUTPUT, int *OUTPUT);
	int rcvrel(void);
	int rcvreldata(struct t_discon *OUTPUT);
	int rcvudata(struct t_unitdata *OUTPUT);
	int rcvuderr(struct t_uderr *OUTPUT);
	int rcvv(struct t_iovec *INOUT, unsigned int INPUT, int *OUTPUT);
	int rcvvopt(struct t_unitdata *OUTPUT, struct t_iovec const *INPUT, unsigned int INPUT, int INPUT);
	int rcvvudata(struct t_unitdata *OUTPUT, struct t_iovec const *INPUT, unsigned int INPUT, int *OUTPUT);
	int removeleaf(int INPUT, int INPUT);
	int snd(char const *INPUT, unsigned int INPUT, int INPUT);
	int snddis(struct t_call const *INPUT);
	int sndopt(struct t_unitdata const *INPUT, int INPUT);
	int sndrel(void);
	int sndreldata(struct t_discon const *INPUT);
	int sndudata(struct t_unitdata const *INPUT);
	int sndv(struct t_iovec const *INPUT, unsigned int INPUT, int INPUT);
	int sndvopt(struct t_unitdata const *INPUT, struct t_iovec const *INPUT, unsigned int INPUT, int INPUT);
	int sndvudata(struct t_unitdata const *INPUT, struct t_iovec const *INPUT, unsigned int INPUT);
	static char const *strerror(int INPUT);
	int sync(void);
	int sysconf(void);
	int unbind(void);
}
