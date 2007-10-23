#include <isode/psap.h>

...

register struct PE pe, p;

...

for (p = first_member (pe); p; p = next_member (pe, p))
    switch (p -> pe_offset) {
	case 0:
	    ...

	case 1:
	    ...

	...
    }

...
