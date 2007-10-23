#include <ssap.h>

...

int	owned = 0;
int	required;		/* initialized from connection negotation */
int	settings;		/*   .. */

...

#define dotoken(requires,shift,bit,type) \
{ \
    if (requirements & requires) \
	switch (settings & (ST_MASK << shift)) { \
	    case ST_CALL_VALUE << shift: \
		settings &= ~(ST_MASK << shift); \
		settings |= ST_INIT_VALUE << shift; \
		break; \
 \
	    case ST_INIT_VALUE: \
		break; \
 \
	    case ST_RESP_VALUE: \
		owned |= bit; \
		break; \
 \
	    default: \
		error ("initial %s token setting", type); \
	} \
}

    dotokens ();

#undef  dotoken
