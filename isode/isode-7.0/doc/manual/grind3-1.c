#include <isode/psap.h>

...

register struct PE pe, p;

...

for (p = first_member (pe); p; p = next_member (pe, p))
    switch (PE_ID (p -> pe_class, p -> pe_id)) {
	case PE_ID (PE_CLASS_UNIV, PE_DEFN_IA5S):
	    ...

	case PE_ID (PE_CLASS_UNIV, PE_DEFN_T61S):
	    ...

	...
    }

...
