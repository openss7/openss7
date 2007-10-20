/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/pod.h,v 9.0 1992/06/16 12:44:54 isode Rel $
 */

#include "general.h"
#include <stdio.h>
#include "sequence.h"
#include <ctype.h>

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>

#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/Shell.h>

#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/List.h>


#define FONTHEIGHT(f) ((f)->max_bounds.ascent + \
		       (f)->max_bounds.descent )


#define FONTWIDTH(f) ((f)->max_bounds.width)


