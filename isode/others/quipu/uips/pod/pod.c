#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/pod.c,v 9.0 1992/06/16 12:44:54 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/pod.c,v 9.0 1992/06/16 12:44:54 isode Rel $
 */

#include "bitmap"
#include "pod.h"
#include "defs.h"
#include "dir_entry.h"

dsEnqError srch_start(), read_config_types(), list_start(), read_all();
dsErrorStruct modify_entry();

void make_friendly(), make_friendly_rdn(), make_template();
void quit();

void rfc2greybook();
extern mailtype mailformat;

extern bool read_all_flag;

extern char base_path[];
extern char friendly_base_path[];
extern char mvalue[];
extern char dir_error_message[];

extern unsigned int filt_num, typeindx;
extern int *av_typeindx;

extern char *filtvalue[];
extern char *filttype[];

extern char dua_help_dir[];

extern int NUMLINES;
extern int px, py, maxx;
extern Pixmap photo_pixmap;
extern bool photo_on;

Widget PhotoWindow;
Pixmap icon_pixmap;

extern int sizelimit, histlimit;

char curr_help[STRINGLEN];
char curr_selection[STRINGLEN];
char help_string[RESBUF];

static Widget standby = 0, error_popup = 0,
              curr_read_popup = 0, curr_list_popup = 0, 
              curr_modify_popup = 0, version_popup = 0;

static Display *dpy;
static Screen *screen;
static int scr;

extern str_seq showseq, dnseq, backseq;
extern int entry_number, dn_number, back_buf_num;

int element_number = 0;
int rdn_number = 0;
int help_up = 0;

static void ShowVersion(), HideVersion();
static void CreateCurrPosWindow(), CreateSearchWindow(), CreateMessagePopup();
static void CreateErrorPopup(), CreateHelpPopup(), CreateHistoryPopup();
static void CreateCommandForm(), CreateVersionPopup();
static dirEntry createModifyTemplate();
static Widget createModifyPopup(), createReadPopup();
static Widget createTypeMenu();
static void Quit(), QuitFromHelp(), InsertHelp();
static void List(), ListDestroy();
static void AddNewList(), ListSelect(), destroyList(), keepList();
static void Move(), DnMoveRead(), DnMove();
static void SetType(), Read(), Help(), ReadAll();
static void TSearch(), ClearSearchArea(), StartSearch();
static void createList();
static void createHistoryPopup(), popupHistory(), popdownHistory();
static void displayReadPopup(), readDestroy(), keepRead();
static void submitModif(), closeModify(), modifyEntry(), addValField();
static void UndoValChanges(), UndoAttrChanges(), modUpdate(), keepModify();
static void cannotModify();
static void killError(), doError();
static void ChangeHelp();
static void buttonPress();
static void freeEntry(), freeSpace(), CutString();
static void ListSelectList(), DnList();
static void ListSelectMove();

static bool ConvSel();
static int GetTextWidth(), GetTextHeight();
static void PopupMessage();
static void entry_print();
static void kill_error();
void kill_message(), message();

static void CreateBackgroundPixmap(), FreeWidgetPixmap();
void displayError();
void print_photo(), kill_photo(), make_photo_widget();

Widget toplevel, outer;

static XtActionsRec verActionsTable[] = {
  {"HideVersion", (XtActionProc) HideVersion},
};

static XtActionsRec buttonActionsTable[] = {
  {"ChangeHelp", (XtActionProc) ChangeHelp},
  {"buttonPress", (XtActionProc) buttonPress},
};

static XtActionsRec listActionsTable[] = {
  {"ListSelectList", (XtActionProc) ListSelectList},
  {"ListSelectMove", (XtActionProc) ListSelectMove},
};

static XtActionsRec currPosActionsTable[] = {
  {"DnList", (XtActionProc) DnList},
  {"DnMove", (XtActionProc) DnMove},
};

void CreateWidgets()
{
  int count;
  Arg args[MAXARGS];

  count = 0;
  outer = XtCreateManagedWidget("outer", formWidgetClass, toplevel,
				args, count);

  dpy = XtDisplay(toplevel);
  scr = DefaultScreen(dpy);
  screen = XtScreen(toplevel);

  XtAddActions(buttonActionsTable, XtNumber(buttonActionsTable));
  XtAddActions(listActionsTable, XtNumber(listActionsTable));
  XtAddActions(currPosActionsTable, XtNumber(currPosActionsTable));
  XtAddActions(verActionsTable, XtNumber(verActionsTable));

  curr_help[0] = '\0';
  CreateCurrPosWindow(outer);
  CreateSearchWindow(outer);
  CreateCommandForm(outer);
  CreateHistoryPopup(backseq, "");
  CreateHelpPopup();
  CreateMessagePopup();
  CreateErrorPopup();
  CreateVersionPopup();
}

void PodLoop()
{
  Widget PosWindow;
  XSizeHints   hints;
  XWMHints wm_hints;
  XClassHint class_hint;
  XTextProperty window, icon;
  String window_name = "Directory",
  	 icon_name = "Directory";
  

  XtRealizeWidget(toplevel);

  icon_pixmap = XCreatePixmapFromBitmapData(dpy, XtWindow(toplevel),
					    icon_bits, icon_width, icon_height,
					    BlackPixelOfScreen(screen),
					    WhitePixelOfScreen(screen),
					    DefaultDepthOfScreen(screen));

  hints.flags = 0;

  wm_hints.input = TRUE;
  wm_hints.icon_pixmap = icon_pixmap;
  wm_hints.initial_state = NormalState;
  wm_hints.flags = InputHint | IconPixmapHint | StateHint;

  class_hint.res_name ="pod";
  class_hint.res_class = "Pod";

  XStringListToTextProperty(&window_name, 1, &window);
  XStringListToTextProperty(&icon_name, 1, &icon);

  XSetWMProperties(dpy, XtWindow(toplevel),
		   &window, &icon,
		   (char **) 0, (int) 0,
		   &hints, &wm_hints, &class_hint);

  PosWindow = XtNameToWidget(outer, 
		       "PosForm.PosScrolledWindow.PosWindow");
  print_search_area(PosWindow);

  if (DefaultDepthOfScreen(screen) == 1) {
    CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm"),
			   gray_bits, gray_width, gray_height);

    CreateBackgroundPixmap(outer, gray_bits, gray_width, gray_height);
    CreateBackgroundPixmap(XtNameToWidget(outer, "TypeForm"), 
			   gray_bits, gray_width, gray_height);
  }

  CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm.searchButton"),
                         Search_bits, Search_width, Search_height);

  CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm.listButton"),
                         List_bits, List_width, List_height);

  CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm.historyButton"),
                         History_bits, History_width, History_height);

  CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm.quitButton"),
                         Quit_bits, Quit_width, Quit_height);

  CreateBackgroundPixmap(XtNameToWidget(outer, "MainButtonForm.helpButton"),
                         Help_bits, Help_width, Help_height);

  goto_addr();
  SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);

  XtMainLoop();
}

void make_photo_widget()
{
  int count;
  Arg args[MAXARGS];
  Widget TextForm, TextWindow;

  TextForm = XtNameToWidget(curr_read_popup,
			    "ReadForm.TextScrolledWindow.TextForm");
  TextWindow = XtNameToWidget(curr_read_popup,
			    "ReadForm.TextScrolledWindow.TextForm.AttrWindow");
  
  count = 0;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNforeground, WhitePixelOfScreen(screen)); count++;
  XtSetArg(args[count], XtNbackground, BlackPixelOfScreen(screen)); count++;
  XtSetArg(args[count], XtNwidth, (Dimension) maxx); count++;
  XtSetArg(args[count], XtNheight, (Dimension) py); count++;
  XtSetArg(args[count], XtNlabel, ""); count++;
  XtSetArg(args[count], XtNfromVert, TextWindow); count++;
  PhotoWindow = XtCreateManagedWidget("PhotoWindow",
				       labelWidgetClass,
				       TextForm, args, count);
}

void kill_photo()
{
  if (PhotoWindow != NULL) {
    XtUnmanageChild(PhotoWindow);
    XtDestroyWidget(PhotoWindow);
    PhotoWindow = (Widget) 0;
  }
}

void print_photo()
{
  int count;
  Arg args[MAXARGS];

  count = 0;
  if (photo_pixmap) {
    XtSetArg(args[count], XtNbackgroundPixmap, photo_pixmap); count++;
  }
  XtSetArg(args[count], XtNresizable, FALSE); count++;
  XtSetValues(PhotoWindow, args, count);

  XtAddCallback(PhotoWindow, XtNdestroyCallback, FreeWidgetPixmap, 
		(XtPointer) photo_pixmap);
  
  photo_pixmap = (Pixmap) 0;
}

/*ARGSUSED*/
static void FreeWidgetPixmap(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  XFreePixmap(dpy, (Pixmap) closure);
}

static void createList(list_seq, top_mess, lower_mess)
     str_seq list_seq;
     char *top_mess, *lower_mess;
{
  int count = 0;
  Widget shell, swindow, ListForm, ListWindow, 
         closeButton, keepButton, ListTitleView;
  Arg args[MAXARGS];

  if (curr_list_popup) {
    if (!XtIsManaged(curr_list_popup)) XtManageChild(curr_list_popup);
    shell = curr_list_popup;

    ListForm = XtNameToWidget(shell, "ListForm");
    swindow = XtNameToWidget(shell,
			     "ListForm.ListScrolledWindow");
    ListWindow = XtNameToWidget(shell,
				"ListForm.ListScrolledWindow.ListWindow");
    
    XawFormDoLayout(ListForm, FALSE);

    XtUnmanageChild(ListWindow);
    XtDestroyWidget(ListWindow);
    
    count = 0;
    ListWindow = XtCreateWidget("ListWindow", formWidgetClass,
				       swindow, args, count);

    AddNewList(ListWindow, list_seq,  (unsigned) entry_number);
    XtRealizeWidget(ListWindow);
    XtManageChild(ListWindow);

    XawFormDoLayout(ListForm, TRUE);
    
    count = 0;
    XtSetArg(args[count], XtNlabel, top_mess); count++;
    XtSetValues(XtNameToWidget(shell, "ListForm.ListTitleView.listTitle"),
		args, count);
    
    count = 0;
    XtSetArg(args[count], XtNlabel, lower_mess); count++;
    XtSetValues(XtNameToWidget(shell, "ListForm.ListMessage"),
		args, count);
    
    XRaiseWindow(dpy, XtWindow(shell));
    return;
  }

  count = 0;
  shell = XtCreatePopupShell("ListOutput", topLevelShellWidgetClass,
                             toplevel, args, 0);
  count = 0;
  ListForm = XtCreateManagedWidget("ListForm", formWidgetClass,
				       shell, args, count);

  count = 0;
  XtSetArg(args[count], XtNheight, Close_height); count++;
  XtSetArg(args[count], XtNwidth, Close_width); count++;
  closeButton = XtCreateManagedWidget("closeButton",
				     commandWidgetClass,
                                     ListForm, args, count);
  
  XtAddCallback(closeButton, XtNcallback,
                (XtCallbackProc) destroyList, (XtPointer) shell);

  count = 0;
  XtSetArg(args[count], XtNheight, Keep_height); count++;
  XtSetArg(args[count], XtNwidth, Keep_width); count++;
  keepButton = XtCreateManagedWidget("keepButton",
                                     commandWidgetClass,
				     ListForm, args, count);

  XtAddCallback(keepButton, XtNcallback,
		(XtCallbackProc) keepList, (XtPointer) shell);

  count = 0;
  XtSetArg(args[count], XtNforceBars, FALSE); count++;
  XtSetArg(args[count], XtNallowVert, FALSE); count++;
  XtSetArg(args[count], XtNallowHoriz, TRUE); count++;
  ListTitleView = XtCreateManagedWidget("ListTitleView", viewportWidgetClass,
					ListForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, top_mess); count++;
  (void) XtCreateManagedWidget("listTitle", labelWidgetClass, 
				ListTitleView, args, count);

  count = 0;
  swindow = XtCreateManagedWidget("ListScrolledWindow", viewportWidgetClass,
				  ListForm, args, count);

  count = 0;
  ListWindow = XtCreateManagedWidget("ListWindow", formWidgetClass,
                                     swindow, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, lower_mess); count++;
  (void) XtCreateManagedWidget("ListMessage", labelWidgetClass,
                                ListForm, args, count);
  
  XtAddCallback(ListWindow, XtNdestroyCallback, 
		ListDestroy, (XtPointer) list_seq);

  AddNewList(ListWindow, list_seq,  (unsigned) entry_number);

  XtRealizeWidget(shell);
  XtPopup(shell, XtGrabNone);

  curr_list_popup = shell;

  CreateBackgroundPixmap(closeButton, Close_bits, Close_width, Close_height);
  CreateBackgroundPixmap(keepButton, Keep_bits, Keep_width, Keep_height);
  if (DefaultDepthOfScreen(screen) == 1)
    CreateBackgroundPixmap(ListForm, gray_bits, gray_width, gray_height);
}

static void CreateCurrPosWindow(parent)
     Widget parent;
{
  int count;
  Widget PosForm, swindow, title;
  Arg args[MAXARGS];

  count = 0;
  PosForm = XtCreateManagedWidget("PosForm", formWidgetClass,
				       parent, args, count);
  count = 0;
  title = XtCreateManagedWidget("PosTitle", commandWidgetClass, PosForm,
				args, count);

  XtAddCallback(title, XtNcallback, CutString, (XtPointer) base_path);

  count = 0;
  swindow = XtCreateManagedWidget("PosScrolledWindow", viewportWidgetClass,
				  PosForm, args, count);

  count = 0;
  (void) XtCreateManagedWidget("PosWindow", formWidgetClass,
				    swindow, args, count);
}

static XtActionsRec actionsTable[] = {
  {"TSearch", (XtActionProc) TSearch},
  {"ClearSearchArea", (XtActionProc) ClearSearchArea},
};

static char defaultTranslations[] =  
  "<Key>Return: TSearch() \n\
   Ctrl<Key>M:  TSearch() \n\
   Ctrl<Key>O:  TSearch() \n\
   Ctrl<Key>J:  TSearch() \n\
   Ctrl<Key>U:  ClearSearchArea()";

static void CreateSearchWindow(parent)
     Widget parent;
{
  int count;
  Arg args[MAXARGS];
  Widget TypeForm, SearchVal;
  XtTranslations trans_table;
  XFontStruct *font;
  Dimension height;

  count = 0;
  TypeForm = XtCreateManagedWidget("TypeForm", formWidgetClass,
				   parent, args, count);


  count = 0;
  XtSetArg(args[count], XtNlabel, "Searching For"); count++;
  (void) XtCreateManagedWidget("SearchValLabel", labelWidgetClass,
				TypeForm, args, count);
  
  count = 0;
  XtSetArg(args[count], XtNeditType, XawtextEdit); count++;
  SearchVal = XtCreateManagedWidget("SearchVal", asciiTextWidgetClass,
				    TypeForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(SearchVal, args, count);

  height = FONTHEIGHT(font);
  height += 15;
  
  count = 0;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetValues(SearchVal, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, XtNresizable); count++;
  (void) XtCreateManagedWidget("TypeButton", menuButtonWidgetClass,
				TypeForm, args, count);

  XtSetKeyboardFocus(parent, SearchVal);

  XtAddActions(actionsTable, XtNumber(actionsTable));
  trans_table = XtParseTranslationTable(defaultTranslations);
  XtOverrideTranslations(SearchVal, trans_table);
}

void add_to_history(seqnum)
     int seqnum;
{
  int count;
  char curr_base[STRINGLEN];
  Widget history_form, history_display, scrolwin;
  Arg args[MAXARGS];
  str_seq first;

  history_form = XtNameToWidget(toplevel, "Session History.HistoryForm");

  history_display = XtNameToWidget(history_form, 
				   "ListScrolledWindow.ListWindow");
  scrolwin = XtParent(history_display);
  
  XawFormDoLayout(history_form, FALSE);

  XtUnmanageChild(history_display);
  XtDestroyWidget(history_display);

  count = 0;
  history_display = XtCreateWidget("ListWindow", formWidgetClass,
				   scrolwin, args, count);

  if (seqnum > histlimit) {
    first = backseq;
    backseq = backseq->next;
    first->next = NULLDS;
    free_seq(first);
    back_buf_num--;
  }

  (void) strcpy(curr_base, base_path);
  base_path[0] = '\0';

  AddNewList(history_display, backseq,  (unsigned) back_buf_num);

  XtRealizeWidget(history_display);
  XtManageChild(history_display);

  (void) strcpy(base_path, curr_base);
  XawFormDoLayout(history_form, TRUE);

}

print_search_area(PosWindow)
     Widget PosWindow;
{
  char *str, *end, save, name_array[STRINGLEN];
  char *trans_start_btn2 = "None <Btn2Up>:  set() DnList(",
       *trans_start_btn3 = "None <Btn3Up>:  set() DnMove(",
        translations[STRINGLEN],
        trans_buf[SMALLSTRING];
  Dimension width;
  int count;
  Arg args[MAXARGS];
  Widget rdn_window, last_rdn = 0;
  XtTranslations trans_table;

  count = 0;
  XtSetArg(args[count], XtNwidth, &width); count++;
  XtGetValues(PosWindow, args, count);
  
  width -= 24;

  count = 0;
  XtSetArg(args[count], XtNlabel, ("The World")); count++;
  XtSetArg(args[count], XtNborderWidth, 0); count++;
  XtSetArg(args[count], XtNborderColor, WhitePixelOfScreen(screen)); count++;
  XtSetArg(args[count], XtNhighlightThickness, 1); count++;
  XtSetArg(args[count], XtNjustify, XtJustifyLeft); count++;
  XtSetArg(args[count], XtNwidth, width); count++;
  rdn_window = XtCreateManagedWidget("x", commandWidgetClass,
				     PosWindow, args, count);
  XtAddCallback(rdn_window, XtNcallback, DnMoveRead, (XtPointer) 0);
  
  (void) sprintf(trans_buf, "%d", 0);

  (void) strcpy(translations, trans_start_btn2);
  (void) strcat(translations, trans_buf);
  (void) strcat(translations, ") unset() \n ");
  (void) strcat(translations, trans_start_btn3);
  (void) strcat(translations, trans_buf);
  (void) strcat(translations, ") unset()");
    
  trans_table = XtParseTranslationTable(translations);
  XtOverrideTranslations(rdn_window, trans_table);
  
  last_rdn = rdn_window;

  rdn_number = 1;
  
  (void) strcpy (name_array, "xx");

  make_friendly(friendly_base_path, base_path);
  end = friendly_base_path;

  while (*end != '\0') end++;
  str = end;

  while (str > (char *) friendly_base_path) {
    while (*str != ',' && str > (char *) friendly_base_path)
      if (*str == '\"')	{
	end = str--;
	while (*str != '\"' && str > (char *) friendly_base_path)
	  str--;
      } else
	str--;
    
    while (!isalnum(*str)) str++;

    save = *end;
    *end = '\0';

    count = 0;
    XtSetArg(args[count], XtNlabel, (*str? str: "ahem")); count++;
    XtSetArg(args[count], XtNborderWidth, 0); count++;
    XtSetArg(args[count], XtNborderColor, WhitePixelOfScreen(screen)); count++;
    XtSetArg(args[count], XtNhighlightThickness, 1); count++;
    XtSetArg(args[count], XtNjustify, XtJustifyLeft); count++;
    XtSetArg(args[count], XtNwidth, width); count++;
    XtSetArg(args[count], XtNfromVert, last_rdn); count++;
    rdn_window = XtCreateManagedWidget((String) name_array, commandWidgetClass,
				       PosWindow, args, count);
    last_rdn = rdn_window;
    XtAddCallback(rdn_window, XtNcallback, DnMoveRead, (XtPointer) rdn_number);

    (void) sprintf(trans_buf, "%d", (strlen(name_array) - 1));

    (void) strcpy(translations, trans_start_btn2);
    (void) strcat(translations, trans_buf);
    (void) strcat(translations, ") unset() \n ");
    (void) strcat(translations, trans_start_btn3);
    (void) strcat(translations, trans_buf);
    (void) strcat(translations, ") unset()");
    
    trans_table = XtParseTranslationTable(translations);
    XtOverrideTranslations(rdn_window, trans_table);
  
    (void) strcat(name_array, "x");
    rdn_number++;

    *end = save;

    if (str > (char *) friendly_base_path) {
      while (*str != ',' && str > (char *) friendly_base_path) str--;
      end = str;
      if (*str == ',') str--;
    }
  }
}
  
/*ARGSUSED*/
static void StartSearch(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Arg args[MAXARGS];
  int count, indx;
  Widget text;
  char entry[STRINGLEN], string[STRINGLEN], mess[STRINGLEN];
  char *value;
  dsEnqError status;
  Cursor time_cur = XCreateFontCursor(dpy, XC_watch);
  Cursor normal_cur = XCreateFontCursor(dpy, XC_left_ptr);

  text = XtNameToWidget(outer, "TypeForm.SearchVal");

  count = 0;
  XtSetArg(args[count], XtNstring, &value); count++;
  XtGetValues(text, args, count);
  (void) strcpy(mvalue, value);

  if (mvalue[0] == '\0') {
    kill_message();
    displayError(outer,
		 "You have not specified a search value!\n\
Click on this window to continue");
    return;
  }

  if (index(mvalue, '=') == NULLCP)
    (void) sprintf(mess,
		   "Searching for entry of type `%s'.\nPlease stand by....",
		   filttype[typeindx]);
  else
    (void) sprintf(mess, 
		   "Searching with filter `%s'.\nPlease stand by....",
		   mvalue);

  message(outer, mess);

  XDefineCursor(dpy, XtWindow(outer), time_cur);
  XFlush(dpy);

  status = srch_start();

  XDefineCursor(dpy, XtWindow(outer), normal_cur);
  XFlush(dpy);

  kill_message();

  switch(status) {
  case Okay:
    if (entry_number == 1) {
      (void) strcpy(entry, (get_from_seq(1, dnseq)));
      if (!is_dit_leaf(entry)) {
	(void) strcpy(base_path, entry);
	make_friendly(friendly_base_path, base_path);
	set_search_area(XtNameToWidget
			(outer, "PosForm.PosScrolledWindow.PosWindow"));
	goto_addr();
	SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
	XFlush(dpy);

	if (curr_read_popup == 0) curr_read_popup = createReadPopup();

	status = read_config_types();

	if (status == Okay)
	  displayReadPopup();
	else
	  doError(status);
      } else {
	(void) strcpy(string, base_path);
	(void) strcpy(base_path, entry);
	make_friendly(friendly_base_path, base_path);
	XFlush(dpy);

	if (curr_read_popup == 0) curr_read_popup = createReadPopup();

	status = read_config_types();
	displayReadPopup();

	if (status == Okay) {
	  (void) strcpy(base_path, string);
	  make_friendly(friendly_base_path, base_path);
	} else 
	  doError(status);
      }
      clear_dnseq();
      entry_number = 0;
    } else if (entry_number > 0) {
      (void) strcpy(mess, "Results of search under ");

      if (strlen(base_path) > 3) {
	indx = 0;
	while (friendly_base_path[indx] != ',' && 
	       friendly_base_path[indx] != '\0') indx++;
	friendly_base_path[indx] = '\0';
	(void) strcat(mess, (char *) (friendly_base_path));
      } else {
	(void) strcat(mess, "The World");
	indx = 0;
      }
      
      createList(dnseq, mess, "");
      dnseq = 0;
      break;
    case listsizelimit:
    case adminlimit_w_partial:
    case timelimit_w_partial:
      (void) strcpy(mess, "Results of search under ");

      if (strlen(base_path) > 3) {
	indx = 0;
	while (friendly_base_path[indx] != ',' && 
	       friendly_base_path[indx] != '\0') indx++;
	friendly_base_path[indx] = '\0';
	(void) strcat(mess, (char *) friendly_base_path);
      } else {
	(void) strcat(mess, "The World");
	indx = 0;
      }
      
      switch (status) {
      case timelimit_w_partial:
	(void) sprintf(string,
		       "%s. %s. %d %s.",
		       "Time limit reached",
		       "Partial results only",
		       entry_number,
		       "items displayed");
	break;
      case adminlimit_w_partial:
	(void) sprintf(string,
		       "%s. %s. %d %s.",
		       "Administrative limit reached",
		       "Partial results only",
		       entry_number,
		       "items displayed");
	break;
      case listsizelimit:
	(void) sprintf(string, 
		       "List size limit exceeded. Only %d items displayed.", 
		       entry_number);
	break;
      }

      createList(dnseq, mess, string);
      dnseq = 0;
      break;
    default:
      doError(status);
    }
  }

  ClearSearchArea(XtNameToWidget(outer, "TypeForm.SearchVal"),
		  (XEvent *) NULL, (String *) NULL, (Cardinal) 0);
}

static Widget createTypeMenu(parent)
     Widget parent;
{
  Widget menu_mgr, button;
  Arg args[MAXARGS];
  int count = 0, n;

  menu_mgr = XtCreatePopupShell("menu", simpleMenuWidgetClass, 
				parent, args, 0 );

  n = 0;
  while (av_typeindx[n] != -1) {
    count = 0;
    XtSetArg(args[count], XtNlabel, (String) filttype[av_typeindx[n]]);count++;
    button = XtCreateManagedWidget((String) filttype[av_typeindx[n]],
                                   smeBSBObjectClass,
                                   menu_mgr, args, count);
    XtAddCallback(button, XtNcallback, SetType, (XtPointer) av_typeindx[n]);
    n++;
  }
  return menu_mgr;
}

/*ARGSUSED*/
static void SetType(w, indx, calldata)
     Widget w;
     XtPointer indx, calldata;
{
  Widget  menu, menuButton;
  int count = 0;
  Arg args[MAXARGS];
  char label[1024];

  menuButton = XtNameToWidget(outer, "TypeForm.TypeButton");
  
  if ((int) indx < 0 || (int) indx >= (int) filt_num) return;

  if ((menu = XtNameToWidget(outer, "TypeForm.TypeButton.menu")))
    XtDestroyWidget(menu);

  (void) createTypeMenu(menuButton);

  (void) strcpy(label, filttype[(int) indx]);

  count = 0;
  XtSetArg(args[count], XtNlabel, label); count++;
  XtSetValues(menuButton, args, count);

  typeindx = (int) indx;
}

static void CreateCommandForm(parent)
     Widget parent;
{
  Widget form, quitButton, helpButton,
         searchButton, listButton, historyButton;
  Arg args[MAXARGS];
  int count;
  
  count = 0;
  form = XtCreateManagedWidget("MainButtonForm", formWidgetClass, parent,
			       args, count);

  count = 0;
  XtSetArg(args[count], XtNheight, Quit_height); count++;
  XtSetArg(args[count], XtNwidth, Quit_width); count++;
  quitButton = XtCreateManagedWidget("quitButton",
				     commandWidgetClass,
				     form, args, count);

  XtAddCallback(quitButton, XtNcallback,
                (XtCallbackProc) Quit, (XtPointer) 0);
 
  count = 0;
  XtSetArg(args[count], XtNheight, Help_height); count++;
  XtSetArg(args[count], XtNwidth, Help_width); count++;
  helpButton = XtCreateManagedWidget("helpButton",
				     commandWidgetClass,
				     form, args, count);
  
  XtAddCallback(helpButton, XtNcallback,
                (XtCallbackProc) Help, (XtPointer) 0);

  count = 0;
  XtSetArg(args[count], XtNheight, Search_height); count++;
  XtSetArg(args[count], XtNwidth, Search_width); count++;
  searchButton = XtCreateManagedWidget("searchButton",
				       commandWidgetClass,
				       form, args, count);

  XtAddCallback(searchButton, XtNcallback, 
		(XtCallbackProc) StartSearch, (XtPointer) 0);

  count = 0;
  XtSetArg(args[count], XtNheight, List_height); count++;
  XtSetArg(args[count], XtNwidth, List_width); count++;
  listButton = XtCreateManagedWidget("listButton",
				     commandWidgetClass,
				     form, args, count);

  XtAddCallback(listButton, XtNcallback,
		  (XtCallbackProc) List, (XtPointer) 0);

  count = 0;
  XtSetArg(args[count], XtNheight, History_height); count++;
  XtSetArg(args[count], XtNwidth, History_width); count++;
  historyButton = XtCreateManagedWidget("historyButton",
					commandWidgetClass,
					form, args, count);

  XtAddCallback(historyButton, XtNcallback,
		(XtCallbackProc) popupHistory, (XtPointer) 0);

}

static void displayReadPopup()
{
  if (curr_read_popup) {
    XtPopup(curr_read_popup, XtGrabNone);
    XRaiseWindow(dpy, XtWindow(curr_read_popup));
  }
}

void setReadEntryName(entry_name)
     char *entry_name;
{
  Widget title, modifyButton, showAllButton;
  int count = 0;
  Arg args[MAXARGS];
  char *object, friendly_name[STRINGLEN];

  make_friendly(friendly_name, entry_name);

  if (curr_read_popup) {
    object = strdup(entry_name);

    title = XtNameToWidget(curr_read_popup, "ReadForm.titleView.readTitle");
    if (*friendly_name == '\0') {
      XtSetArg(args[count], XtNlabel, "The World"); count++;
    } else {
      XtSetArg(args[count], XtNlabel, friendly_name); count++;
    }
    XtSetValues(title, args, count);

    modifyButton = XtNameToWidget(curr_read_popup, 
				  "ReadForm.ReadButtonForm.modifyButton");
    showAllButton = XtNameToWidget(curr_read_popup, 
				   "ReadForm.ReadButtonForm.showAllButton");
				   
    if (XtHasCallbacks(title, XtNcallback) == XtCallbackHasSome)
      XtRemoveAllCallbacks(title, XtNcallback);

    XtAddCallback(title, XtNcallback, CutString, (XtPointer) object);

    if (XtHasCallbacks(showAllButton, XtNcallback) == XtCallbackHasSome)
      XtRemoveAllCallbacks(showAllButton, XtNcallback);

    XtAddCallback(showAllButton, XtNcallback, ReadAll, (XtPointer) object);

    if (XtHasCallbacks(modifyButton, XtNcallback) == XtCallbackHasSome) 
      XtRemoveAllCallbacks(modifyButton, XtNcallback);

    if (*object != '\0')
      XtAddCallback(modifyButton, XtNcallback, 
		    modifyEntry, (XtPointer) object);
    else 
      XtAddCallback(modifyButton, XtNcallback,
		    cannotModify, (XtPointer) NULL);

    XtAddCallback(modifyButton, XtNdestroyCallback,
		  freeSpace, (XtPointer) object);
  }
}

/*ARGSUSED*/
static void cannotModify(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  displayError(XtParent(XtParent(w)), 
	       "Cannot modify this entry!\nClick on this window to continue.");
}

static XawTextSelectType select_types[] = {XawselectLine, XawselectNull};

static Widget createReadPopup()
{
  Arg args[MAXARGS];
  int count = 0;
  Widget closeButton, keepButton, modifyButton, showAllButton,
         TextScrolledWindow, AttrWindow, titleView,
         TextForm, ReadPopup, ReadForm, ButtonForm;

  ReadPopup = XtCreatePopupShell("Directory Entry", topLevelShellWidgetClass,
				 toplevel, args , 0);

  /* Top of popup form. */
  ReadForm = XtCreateWidget("ReadForm", formWidgetClass,
			    ReadPopup, args, 0);
  
  /* Buttons */
  ButtonForm = XtCreateManagedWidget("ReadButtonForm", formWidgetClass,
				     ReadForm, args, 0);

  count = 0;
  XtSetArg(args[count], XtNheight, Close_height); count++;
  XtSetArg(args[count], XtNwidth, Close_width); count++;
  closeButton = XtCreateManagedWidget("closeButton", commandWidgetClass,
				      ButtonForm, args, count);

  XtAddCallback(closeButton, XtNcallback, readDestroy, (XtPointer) ReadPopup);

  count = 0;
  XtSetArg(args[count], XtNheight, Keep_height); count++;
  XtSetArg(args[count], XtNwidth, Keep_width); count++;
  keepButton = XtCreateManagedWidget("keepButton", commandWidgetClass,
                                      ButtonForm, args, count);

  XtAddCallback(keepButton, XtNcallback, keepRead, (XtPointer) ReadPopup);

  count = 0;
  XtSetArg(args[count], XtNheight, ShowAll_height); count++;
  XtSetArg(args[count], XtNwidth, ShowAll_width); count++;
  showAllButton = XtCreateManagedWidget("showAllButton", commandWidgetClass,
					ButtonForm, args, count);
  
  count = 0;
  XtSetArg(args[count], XtNheight, Modify_height); count++;
  XtSetArg(args[count], XtNwidth, Modify_width); count++;
  modifyButton = XtCreateManagedWidget("modifyButton", commandWidgetClass,
				       ButtonForm, args, count);

  /* End of buttons */
  count = 0;
  XtSetArg(args[count], XtNforceBars, FALSE); count++;
  XtSetArg(args[count], XtNallowVert, FALSE); count++;
  XtSetArg(args[count], XtNallowHoriz, TRUE); count++;
  titleView = XtCreateManagedWidget("titleView", viewportWidgetClass,
				    ReadForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, friendly_base_path); count++;
  (void) XtCreateManagedWidget("readTitle", commandWidgetClass,
			       titleView, args, count);

  count = 0;
  TextScrolledWindow = XtCreateManagedWidget("TextScrolledWindow",
					     viewportWidgetClass,
					     ReadForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNborderWidth, 0); count++;
  TextForm = XtCreateManagedWidget("TextForm", formWidgetClass,
				   TextScrolledWindow, args, count);

  count = 0;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNstring, ""); count++;
  AttrWindow = XtCreateManagedWidget("AttrWindow", asciiTextWidgetClass,
				     TextForm, args, count);

  XawTextSetSelectionArray(AttrWindow, select_types);
  
  count = 0;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNstring, ""); count++;
  (void) XtCreateManagedWidget("SepWindow", asciiTextWidgetClass,
			       TextForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNstring, ""); count++;
  (void) XtCreateManagedWidget("ValWindow", asciiTextWidgetClass,
                               TextForm, args, count);

  XawFormDoLayout(TextForm, FALSE);

  /* End of text layout */
  XFlush(dpy);
  XtManageChild(ReadForm);
  XtRealizeWidget(ReadPopup);

  CreateBackgroundPixmap(closeButton, Close_bits, Close_width, Close_height);
  
  CreateBackgroundPixmap(keepButton, Keep_bits, 
			 Keep_width, Keep_height);

  CreateBackgroundPixmap(showAllButton, ShowAll_bits,
			 ShowAll_width, ShowAll_height);
  
  CreateBackgroundPixmap(modifyButton, Modify_bits,
			 Modify_width, Modify_height);

  if (DefaultDepthOfScreen(screen) == 1) {
    CreateBackgroundPixmap(ReadForm, gray_bits, 
			   gray_width, gray_height);

    CreateBackgroundPixmap(ButtonForm, gray_bits,
                           gray_width, gray_height);
  }
    
  PhotoWindow = 0;
  return ReadPopup;
}


static void AddNewList(list_widget, list_seq, list_size)
     Widget list_widget;
     str_seq list_seq;
     unsigned int list_size;
{
  Arg args[MAXARGS];
  Widget element, scrolwin;
  int count = 0, n;
  char name[STRINGLEN], friendly_rdn[STRINGLEN];
  char *prev, *object;
  Dimension width, rwidth;
  char *trans_start_btn2 = "None <Btn2Up>:  set() ListSelectList(",
       *trans_start_btn3 = "None <Btn3Up>:  set() ListSelectMove(",
        translations[STRINGLEN];
  XtTranslations trans_table;

  scrolwin = (Widget) XtParent(list_widget);

  count = 0;
  XtSetArg(args[count], XtNwidth, &width); count++;
  XtGetValues(scrolwin, args, count);

  width -= 15;

  element_number = list_size;
  
  *name = '\0';
  for (n = 0; n < element_number; n++) {
    (void) strcat(name, "X");

    object = get_from_seq(n+1, list_seq);
    make_friendly_rdn(friendly_rdn, object, base_path);
    count = 0;
    XtSetArg(args[count], XtNlabel, (*friendly_rdn? 
				   friendly_rdn: "The World")); count++;
    XtSetArg(args[count], XtNborderWidth, 0);count++;
    XtSetArg(args[count], XtNborderColor, WhitePixelOfScreen(screen)); count++;
    XtSetArg(args[count], XtNhighlightThickness, 1); count++;
    XtSetArg(args[count], XtNjustify, XtJustifyLeft); count++;
    if (strlen(name) > 1) {
      prev = (char *) name + 1;
      XtSetArg(args[count], XtNfromVert, XtNameToWidget(list_widget, prev)); 
      count++;
    }
    element = XtCreateManagedWidget((String) name,
				    commandWidgetClass,
				    list_widget, args, count);

    XtAddCallback(element, XtNcallback, ListSelect, (XtPointer) object);

    (void) strcpy(translations, trans_start_btn2);
    (void) strcat(translations,"\"");
    (void) strcat(translations, object);
    (void) strcat(translations,"\"");
    (void) strcat(translations, ") unset()\n ");
    (void) strcat(translations, trans_start_btn3);
    (void) strcat(translations,"\"");
    (void) strcat(translations, object);
    (void) strcat(translations,"\"");
    (void) strcat(translations, ") unset()");
    
    trans_table = XtParseTranslationTable(translations);
    XtOverrideTranslations(element, trans_table);
  
    count = 0;
    XtSetArg(args[count], XtNwidth, &rwidth); count++;
    XtGetValues(element, args, count);

    if (rwidth < width) {
      count = 0;
      XtSetArg(args[count], XtNwidth, width); count++;
      XtSetValues(element, args, count);
    }
  }
}

void readEntryPrint(entry_ptr)
     char *entry_ptr;
{
  Widget entry_form = 0;
  
  if (curr_read_popup) {
    entry_form = XtNameToWidget(curr_read_popup,
				"ReadForm.TextScrolledWindow.TextForm");
    entry_print(entry_form, entry_ptr);
  }
}

static void entry_print(entry_form, entry_ptr)
     Widget entry_form; 
     char *entry_ptr;
{
  Widget attr_list, val_list, sep_list;
  Arg args[MAXARGS];
  int count = 0, attr_count = 0; 
  register char *str_start, *str_end;
  char save, vals[RESBUF], attrs[RESBUF], seps[RESBUF];
  Dimension attr_width = 0, max_attr_width = 0;
  Dimension val_width = 0, max_val_width = 0;
  Dimension sep_width;
  Dimension height;
  XFontStruct *font;
  bool do_mail_reverse = FALSE;

  XawFormDoLayout(entry_form, FALSE);

  (void) strcpy(vals, "");
  (void) strcpy(attrs, "");
  (void) strcpy(seps, "");

  attr_list = XtNameToWidget(entry_form, "AttrWindow");
  val_list = XtNameToWidget(entry_form, "ValWindow");
  sep_list = XtNameToWidget(entry_form, "SepWindow");

  XawTextDisableRedisplay(attr_list);
  XawTextDisableRedisplay(val_list);
  XawTextDisableRedisplay(sep_list);

  count = 0;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(attr_list, args, count);

  str_start = str_end = entry_ptr;
  while (str_end != NULLCP && *str_end != '\0') {
    attr_count++;
    if (*str_end == '\t') {
      while (isspace(*str_end)) str_end++;
      str_start = str_end;
      
      while (*str_end != '\n' && *str_end != '\0') str_end++;

      save = *str_end;
      *str_end = '\0';
      
      (void) strcat(attrs, "\n");
      (void) strcat(vals, "\n");

      if (do_mail_reverse == TRUE) rfc2greybook(str_start);
      (void) strcat(vals, str_start);

      (void) strcat(seps, "\n");

      val_width = XTextWidth(font, str_start, strlen(str_start))+
		   FONTWIDTH(font);

      if (val_width > max_val_width) max_val_width = val_width;

      if (save == '\0') str_end--;
    } else {
      while (!isspace(*str_end)) str_end++;

      save = *str_end;
      *str_end = '\0';

      (void) strcat(attrs, "\n");

      if (mailformat == greybook && !lexequ(str_start, "rfc822Mailbox")) {
	do_mail_reverse = TRUE;
	(void) strcat(attrs, "mailbox");
      } else {
	do_mail_reverse = FALSE;
	(void) strcat(attrs, str_start);
      }
      
      attr_width = XTextWidth(font, str_start, strlen(str_start)) +
		   FONTWIDTH(font);

      if (attr_width > max_attr_width) max_attr_width = attr_width;

      *str_end = save;
      while (*str_end != '-') str_end++;
      str_end++;

      while (isspace(*str_end)) str_end++;

      str_start = str_end;
      while (*str_end != '\n' && *str_end != '\0') str_end++;
      
      save = *str_end;
      *str_end = '\0';

      (void) strcat(seps, "-\n");
      (void) strcat(vals, "\n");

      if (do_mail_reverse == TRUE) rfc2greybook(str_start);
      (void) strcat(vals, str_start);

      val_width = 
	XTextWidth(font, str_start, strlen(str_start)) + FONTWIDTH(font);

      if (val_width > max_val_width) max_val_width = val_width;
      if (save == '\0') str_end--;
    }
    str_start = ++str_end;
  }

  max_attr_width += FONTWIDTH(font);
  max_val_width += FONTWIDTH(font);
  height = ++attr_count * FONTHEIGHT(font);
  sep_width = FONTWIDTH(font);

  if (entry_ptr == NULLCP) {
    (void) strcpy (attrs, "No Attributes found!");
    max_attr_width = 
      XTextWidth(font, attrs, strlen(attrs)) + FONTWIDTH(font);
  }

  count = 0;
  XtSetArg(args[count], XtNstring, (attrs+1)); count++;
  XtSetArg(args[count], XtNwidth, max_attr_width); count++;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetValues(attr_list, args, count);

  count = 0;
  XtSetArg(args[count], XtNstring, (vals+1)); count++;
  XtSetArg(args[count], XtNwidth, max_val_width); count++;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetValues(val_list, args, count);

  count = 0;
  XtSetArg(args[count], XtNstring, seps); count++;
  XtSetArg(args[count], XtNwidth, sep_width); count++;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetValues(sep_list, args, count);

  XawTextEnableRedisplay(attr_list);
  XawTextEnableRedisplay(val_list);
  XawTextEnableRedisplay(sep_list);

  if (PhotoWindow != NULL && photo_on == TRUE) {
    XtRealizeWidget(PhotoWindow);
    print_photo();
  }

  XawFormDoLayout(entry_form, TRUE);
}

/*ARGSUSED*/
static void Quit(widget, closure, callData)
     Widget widget;
     XtPointer closure, callData;
{
  XtUnmapWidget(toplevel);
  XtDestroyWidget(toplevel);
  quit(0);
}

/*ARGSUSED*/
static void TSearch(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  StartSearch((Widget) 0, (XtPointer) 0, (XtPointer) 0);
}

/*ARGSUSED*/
static void ClearSearchArea(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  Arg args[MAXARGS];
  int count= 0;
  XtSetArg(args[count], XtNstring, "\0"); count++;
  XtSetValues(w, args, count);
}

/*ARGSUSED*/
static void ReadAll(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  dsEnqError status;
  Widget temp_read_popup;
  char current_path[STRINGLEN];
  char *object = (char *) closure;

  (void) strcpy(current_path, base_path);
  (void) strcpy(base_path, object);

  temp_read_popup = curr_read_popup;

  curr_read_popup = XtParent(XtParent(XtParent(w)));

  status = read_all();

  if (status != Okay)
    doError(status);
  else
    displayReadPopup();

  (void) strcpy(base_path, current_path);
  curr_read_popup = temp_read_popup;
}

/*ARGSUSED*/
static void Read(w, data, calldata)
     Widget w;
     XtPointer data, calldata;
{
  dsEnqError status;

  if (curr_read_popup == 0) curr_read_popup = createReadPopup();

  XFlush(dpy);
  status = read_config_types();

  if (status != Okay)
    doError(status);
  else
    displayReadPopup();
}

set_search_area(search_area)
     Widget search_area;
{
  WidgetList wlist;
  char name[STRINGLEN];
  int count = 0, n;

  wlist = (WidgetList) XtMalloc((rdn_number + 1) * sizeof(Widget));

  name[0] = '\0';

  for (n = 0; n < rdn_number; n++) {
    (void) strcat(name, "x");
    wlist[n] = XtNameToWidget(search_area, name);
    count++;
  }
  XtUnmanageChildren(wlist, count);
  
  for (n = 0; n < count; n++)
    XtDestroyWidget(wlist[n]);

  XtFree(wlist);
  print_search_area(search_area);
}

/*ARGSUSED*/
static void List(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  int indx;
  char string[1024], mess[1024];
  dsEnqError status;
  Cursor time_cur = XCreateFontCursor(dpy, XC_watch);
  Cursor normal_cur = XCreateFontCursor(dpy, XC_left_ptr);

  message(outer, "Listing, Please Stand By......");
  XFlush(dpy);

  XDefineCursor(dpy, XtWindow(outer), time_cur);
  XFlush(dpy);

  status = list_start();

  XDefineCursor(dpy, XtWindow(outer), normal_cur);
  XFlush(dpy);

  kill_message();

  make_friendly(friendly_base_path, base_path);

  switch(status) {
  case listsizelimit:
  case adminlimit_w_partial:
  case timelimit_w_partial:
    (void) strcpy(mess, "Results of list under ");

    if (strlen(base_path) > 3) {
      indx = 0;
      while (friendly_base_path[indx] != ',' && 
	     friendly_base_path[indx] != '\0') indx++;
      friendly_base_path[indx] = '\0';
      (void) strcat(mess, (char *) friendly_base_path);
    } else
      (void) strcat(mess, "The World");

    switch (status) {
    case listsizelimit:
      (void) sprintf(string, 
		      "List size limit exceeded. Only %d items displayed.", 
		      entry_number); 
      break;
    case adminlimit_w_partial:
      (void) sprintf(string,
		     "%s. %s. %d %s.",
		     "Administrative limit reached",
		     "Partial results only",
		     entry_number,
		     "items displayed");
      break;
    case timelimit_w_partial:
            (void) sprintf (string,
			    "%s. %s. %d %s.",
			    "Time limit reached",
			    "Partial results only",
			    entry_number,
			    "items displayed");
      break;
    }
    createList(dnseq, mess, string);
    dnseq = 0;
    break;
  case Okay:
    (void) strcpy(mess, "Results of list under ");

    if (strlen(base_path) > 3) {
      indx = 0;
      while (friendly_base_path[indx] != ',' && 
	     friendly_base_path[indx] != '\0') indx++;
      friendly_base_path[indx] = '\0';
      (void) strcat(mess, (char *) friendly_base_path);
    } else
      (void) strcat(mess, "The World");

    createList(dnseq, mess, "");
    dnseq = 0;
    break;
  default:
    dnseq = 0;
    entry_number = 0;
    doError(status);
  }
}

/*ARGSUSED*/
static void ListSelect(w, object, calldata)
     Widget w;
     XtPointer object, calldata;
{
  char *entry_name, parent[STRINGLEN];

  entry_name = (char *) object;
  
  (void) strcpy(parent, base_path);
  (void) strcpy(base_path, entry_name);
  make_friendly(friendly_base_path, base_path);

  if (!is_dit_leaf(entry_name)) {
    if (read_all_flag == TRUE) {
      Read((Widget) 0, (XtPointer) 0, (XtPointer) 0);
    }   
    Move((Widget) 0, (XtPointer) entry_name, (XtPointer) 0);
  } else {
    Read((Widget) 0, (XtPointer) 0, (XtPointer) 0);
    (void) strcpy(base_path, parent);
  }
}

/*ARGSUSED*/
static void ListSelectMove(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  char temp_path[STRINGLEN];
  Widget search_area = XtNameToWidget(outer, 
				      "PosForm.PosScrolledWindow.PosWindow");
  (void) strcpy(temp_path, base_path);
  (void) strcpy(base_path, params[0]);

  if (!is_dit_leaf(base_path)) {
    goto_addr();
    set_search_area(search_area);
    SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
    XFlush(dpy);
  } else {
    (void) strcpy(base_path, temp_path);
    displayError(outer,
		 "Cannot move to a leaf entry!\n\
Click on this window to continue");
  }
}

/*ARGSUSED*/
static void ListSelectList(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  Widget search_area = XtNameToWidget(outer, 
				      "PosForm.PosScrolledWindow.PosWindow");
  char temp_path[STRINGLEN];

  (void) strcpy(temp_path, base_path);
  (void) strcpy(base_path, params[0]);

  if (!is_dit_leaf(base_path)) {
    List((Widget) 0, (XtPointer) 0, (XtPointer) 0);
    
    goto_addr();
    set_search_area(search_area);
    SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
    XFlush(dpy);
  } else {
    (void) strcpy(base_path, temp_path);
    displayError(outer, 
		 "Cannot list under this entry!\n\
Click on this window to continue");
  }
}

/*ARGSUSED*/
static void DnList(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  Widget search_area = XtNameToWidget(outer, 
				      "PosForm.PosScrolledWindow.PosWindow");
  register char *end = base_path;
  int rdnlevel = atoi(params[0]);

  if (rdnlevel == 0) *base_path = '\0';
  else if (rdn_number > (int) (rdnlevel+1)) {
    while (rdnlevel) {
      while (*end != '@') end++;
      if (*end == '@') end++;
      rdnlevel--;
    }
    *--end = '\0';
  } 

  List((Widget) 0, (XtPointer) 0, (XtPointer) 0);
  goto_addr();
  set_search_area(search_area);
  SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
  XFlush(dpy);
}

/*ARGSUSED*/
static void DnMove(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  register char *end;
  Widget search_area = XtNameToWidget(outer, 
				      "PosForm.PosScrolledWindow.PosWindow");
  int rdnlevel = atoi(params[0]);

  end = base_path;

  if (rdnlevel == 0) *base_path = '\0';
  else if (rdn_number > (int) (rdnlevel+1)) {
    while (rdnlevel) {
      while (*end != '@') end++;
      if (*end == '@') end++;
      rdnlevel--;
    }
    *--end = '\0';
  } 

  goto_addr();
  set_search_area(search_area);
  SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
  XFlush(dpy);
}
  
/*ARGSUSED*/
static void DnMoveRead(w, rdnlevel, calldata)
     Widget w;
     XtPointer rdnlevel, calldata;
{
  Widget search_area;
  char *end;
  dsEnqError status;

  search_area = XtNameToWidget(outer, "PosForm.PosScrolledWindow.PosWindow");

  end = base_path;
  if (rdnlevel == 0) *base_path = '\0';
  else if (rdn_number > (int) (rdnlevel+1)) {
    while (rdnlevel) {
      while (*end != '@') end++;
      if (*end == '@') end++;
      rdnlevel--;
    }
    *--end = '\0';
  } 

  goto_addr();
  set_search_area(search_area);
  SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
  XFlush(dpy);

  if (read_all_flag == TRUE) {
    if (curr_read_popup == 0) curr_read_popup = createReadPopup();
    status = read_config_types();
    
    if (status != Okay)
      doError(status);
    else {
      displayReadPopup();
    }
  }
}

/*ARGSUSED*/
static void Move(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget search_area;
  char *new_base;

  search_area = XtNameToWidget(outer, "PosForm.PosScrolledWindow.PosWindow");
  
  new_base = (char *) closure;
  (void) strcpy(base_path, new_base);

  make_friendly(friendly_base_path, new_base);
  set_search_area(search_area);
  goto_addr();
  SetType((Widget) 0, (XtPointer) typeindx, (XtPointer) 0);
}

static void CreateMessagePopup()
{
  int count;
  Arg args[MAXARGS];

  count = 0;
  standby = XtCreatePopupShell("POD: Working.",  
			       transientShellWidgetClass,
			       toplevel, args, count);

  count = 0;
  XtSetArg(args[count], XtNresize, FALSE); count++;
  (void) XtCreateManagedWidget("standby", labelWidgetClass,
			       standby, args, count);

  XtRealizeWidget(standby);
}

static void CreateVersionPopup()
{
  Widget title, piccy, version_label, text_form;
  Arg args[MAXARGS];
  int count;
  /* Have to hard to code the version number in! */
  char *version = "Version 2.4 29th May 1991",
       *title_string = "Pod - POpup Directory",
       *author_string = "by\n\
Andrew Findlay, Damanjit Mahl, Stefan Nahajski\n\
Brunel University";
  
  count = 0;
  version_popup = XtCreatePopupShell("About Pod", transientShellWidgetClass,
				     toplevel, args, count);
  
  count = 0;
  text_form = XtCreateManagedWidget("verForm", formWidgetClass,
				    version_popup, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, title_string); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  title = XtCreateManagedWidget("verTitle", labelWidgetClass,
				text_form, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, version); count++;
  XtSetArg(args[count], XtNfromVert, title); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  version_label = XtCreateManagedWidget("verLabel", labelWidgetClass,
					text_form, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, ""); count++;
  XtSetArg(args[count], XtNfromVert, version_label); count++;
  XtSetArg(args[count], XtNwidth, work_width); count++;
  XtSetArg(args[count], XtNheight, work_height); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  piccy = XtCreateManagedWidget("teamPic", labelWidgetClass,
				text_form, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, author_string); count++;
  XtSetArg(args[count], XtNfromVert, piccy); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  (void) XtCreateManagedWidget("authLabel", labelWidgetClass,
			       text_form, args, count);

  XtRealizeWidget(version_popup);

  CreateBackgroundPixmap(piccy, work_bits, work_width, work_height);
}

/* ARGSUSED */
static void ShowVersion(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget title, authors, version_label, text_form, piccy;
  Position x, y;
  Dimension width, label_width;
  Arg args[MAXARGS];
  int count;
  
  text_form = XtNameToWidget(version_popup, "verForm");
  title = XtNameToWidget(text_form, "verTitle");
  authors = XtNameToWidget(text_form, "authLabel");
  version_label = XtNameToWidget(text_form, "verLabel");
  piccy = XtNameToWidget(text_form, "teamPic");

  PopupMessage(version_popup, w, text_form, (String) NULL, XtGrabNone);

  count = 0;
  XtSetArg(args[count], XtNwidth, &width); count++;
  XtGetValues(text_form, args, count);

  count = 0;
  XtSetArg(args[count], XtNwidth, &label_width); count++;
  XtSetArg(args[count], XtNy, &y); count++;
  XtGetValues(authors, args, count);

  x = (Position) ((width - label_width) /2);
  XtMoveWidget(authors, x, y);

  count = 0;
  XtSetArg(args[count], XtNwidth, &label_width); count++;
  XtSetArg(args[count], XtNy, &y); count++;
  XtGetValues(title, args, count);  

  x = (Position) ((width - label_width) /2);
  XtMoveWidget(title, x, y);

  count = 0;
  XtSetArg(args[count], XtNwidth, &label_width); count++;
  XtSetArg(args[count], XtNy, &y); count++;
  XtGetValues(piccy, args, count);  

  x = (Position) ((width - label_width) /2);
  XtMoveWidget(piccy, x, y);

  count = 0;
  XtSetArg(args[count], XtNwidth, &label_width); count++;
  XtSetArg(args[count], XtNy, &y); count++;
  XtGetValues(version_label, args, count);  

  x = (Position) ((width - label_width) /2);
  XtMoveWidget(version_label, x, y);

  XRaiseWindow(dpy, XtWindow(version_popup));
}

/* ARGSUSED */
static void HideVersion(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  XtPopdown(version_popup);
}

static void CreateErrorPopup()
{
  Widget text;
  int count;
  Arg args[MAXARGS];

  count = 0;
  error_popup = XtCreatePopupShell("POD: <Directory Error>", 
				   transientShellWidgetClass,
				   toplevel, args, count);

  count = 0;
  XtSetArg(args[count], XtNresize, FALSE); count++;
  text = XtCreateManagedWidget("error_popup", commandWidgetClass,
			       error_popup, args, count);

  XtAddCallback(text, XtNcallback, killError, (XtPointer) 0);

  XtRealizeWidget(error_popup);
}

static void doError(status)
     dsEnqError status;
{
  char show_message[STRINGLEN];
  entry_number = 0;

  switch(status) {
  case timelimit:
    displayError
      (outer,
       "Time limit reached.\nClick on this window to continue");
    break;
  case adminlimit:
    displayError
      (outer, 
       "Administrative limit reached.\nClick on this window to continue");
    break;
  case nothingfound:
    dnseq = 0;
    displayError
      (outer, "Nothing found!\nClick on this window to continue");
    break;
  case localdsaerror:
    dnseq = 0;
    displayError
      (outer,
    "Problem with remote directory server.\nClick on this window to continue");
    break;
  case remotedsaerror:
    dnseq = 0;
    displayError
      (outer, 
   "Requested data unavailable at present.\nClick on this window to continue");
    break;
  case duaerror:
    dnseq = 0;
    displayError
      (outer, "Internal error. Sorry!\nClick on this window to continue");
    break;
  case security:
    dnseq = 0;
    displayError
      (outer,
       "You do not have the access privileges required\n\
to make this request!\nClick on this window to continue");
    break;
  case namerror:
    dnseq = 0;
    displayError
      (outer, "Invalid directory position!\nClick on this window to continue");
    break;
  case attributerror:
    dnseq = 0;
    displayError
      (outer, 
       "Faulty data found in database!\nClick on this window to continue");
    break;
  case serviceerror:
    dnseq = 0;
    displayError
      (outer, "Directory Service Error!\nClick on this window to continue");
    break;
  case updaterror:
    dnseq = 0;
    displayError
      (outer, "Update Error!\nClick on this window to continue");
    break;
  case error_string_given:
    dnseq = 0;
    (void) sprintf(show_message, "%sClick on this window to continue",
		   dir_error_message);
    displayError(outer, show_message);
    break;
  }
}

void displayError(refto, mess)
     Widget refto;
     String mess;
{
  Arg args[MAXARGS];
  int count;
  Widget text;

  text = XtNameToWidget(error_popup, "error_popup");

  count = 0;
  XtSetArg(args[count], XtNlabel, mess); count++;
  XtSetValues(text, args, count);

  PopupMessage(error_popup, refto, text, mess, XtGrabExclusive);

  XBell(dpy, 100);
}

void displayStartupError(mess)
     String mess;
{
  Arg args[MAXARGS];
  int count;
  Widget text;

  text = XtNameToWidget(error_popup, "error_popup");

  XtRemoveAllCallbacks(text, XtNcallback);
  XtAddCallback(text, XtNcallback, Quit, (XtPointer) 0);

  count = 0;
  XtSetArg(args[count], XtNlabel, mess); count++;
  XtSetValues(text, args, count);

  PopupMessage(error_popup, (Widget) 0, text, mess, XtGrabExclusive);
}

/*ARGSUSED*/
static void killError(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  kill_error();
}

static void kill_error()
{
  Arg args[MAXARGS];
  int count;
  Pixmap pixmap_resource;

  Widget text = XtNameToWidget(error_popup, "error_popup");

  XtPopdown(error_popup);

  count = 0;
  XtSetArg(args[count], XtNbackgroundPixmap, &pixmap_resource); count++;
  XtGetValues(text, args, count);

  count = 0;
  XtSetArg(args[count], XtNbackgroundPixmap, XtUnspecifiedPixmap); count++;
  XtSetValues(text, args, count);

  if (pixmap_resource != XtUnspecifiedPixmap) 
    XFreePixmap(dpy, pixmap_resource);
}

void message(refto, mess)
     Widget refto;
     char *mess;
{
  Arg args[MAXARGS];
  int count;
  Widget text;
  XEvent event;

  text = XtNameToWidget(standby, "standby");
  
  count = 0;
  XtSetArg(args[count], XtNlabel, mess); count++;
  XtSetValues(text, args, count);

  PopupMessage(standby, refto, text, mess, XtGrabNonexclusive);

  while (!XCheckTypedWindowEvent(dpy, XtWindow(text), Expose, &event));
  XtDispatchEvent(&event);

  XFlush(dpy);
}

void kill_message()
{
  Arg args[MAXARGS];
  int count;
  Pixmap pixmap_resource;

  Widget text = XtNameToWidget(standby, "standby");

  XtPopdown(standby);

  count = 0;
  XtSetArg(args[count], XtNbackgroundPixmap, &pixmap_resource); count++;
  XtGetValues(text, args, count);

  count = 0;
  XtSetArg(args[count], XtNbackgroundPixmap, XtUnspecifiedPixmap); count++;
  XtSetValues(text, args, count);

  if (pixmap_resource != XtUnspecifiedPixmap) 
    XFreePixmap(dpy, pixmap_resource);
}
 
static void CreateHelpPopup()
{
  Widget popup_help, popup_help_button, popup_quit_button, 
  popup_help_form, popup_help_scrolwin;

  Arg args[MAXARGS];
  int count;

  count = 0;
  popup_help = XtCreatePopupShell("Pod Help Screen", topLevelShellWidgetClass,
				  toplevel, args, count);
  
  count = 0;
  popup_help_form = XtCreateManagedWidget("popup_help_form", formWidgetClass,
					  popup_help, args, count);

  count = 0;
  XtSetArg(args[count], XtNheight, Close_height); count++;
  XtSetArg(args[count], XtNwidth, Close_width); count++;
  popup_quit_button = XtCreateManagedWidget("popup_help_quit", 
					    commandWidgetClass, 
					    popup_help_form, args, count);

  XtAddCallback(popup_quit_button, XtNcallback, QuitFromHelp, (XtPointer) 0);
  
  count = 0;
  XtSetArg(args[count], XtNheight, About_height); count++;
  XtSetArg(args[count], XtNwidth, About_width); count++;
  popup_help_button = XtCreateManagedWidget("popup_help_general",
					    commandWidgetClass,
                                            popup_help_form, args, count);

  XtAddCallback(popup_help_button, XtNcallback, ShowVersion, (XtPointer) NULL);

  count = 0;
  popup_help_scrolwin = XtCreateManagedWidget("popup_help_scrolwin",
					      viewportWidgetClass,
					      popup_help_form, args, count);

  count = 0;
  (void) XtCreateManagedWidget("popup_help_text",
					  asciiTextWidgetClass,
					  popup_help_scrolwin, args, count);
  XtRealizeWidget(popup_help);

  CreateBackgroundPixmap(popup_help_button, About_bits, 
			 About_width, About_height);

  CreateBackgroundPixmap(popup_quit_button, Close_bits, 
			 Close_width, Close_height);

  if (DefaultDepthOfScreen(screen) == 1) {
    CreateBackgroundPixmap(popup_help_form, gray_bits, 
			   gray_width, gray_height);
  }
}

/*ARGSUSED*/
static void Help(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget help_popup;

  help_popup = XtNameToWidget(toplevel, "Pod Help Screen");

  XtPopup(help_popup, XtGrabNone);
  InsertHelp((Widget) 0, (XtPointer) "help", (XtPointer) 0);
  XFlush(dpy);
  help_up = TRUE;
}

/*ARGSUSED*/
static void QuitFromHelp(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget help_popup;
 
  help_up = FALSE;
  help_popup = XtNameToWidget(toplevel, "Pod Help Screen");
  XtPopdown(help_popup);
  HideVersion((Widget) NULL, (XEvent *) NULL, (String *) NULL, 0);
}

/*ARGSUSED*/
static void InsertHelp(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget text, scrolwin;
  FILE *file;
  char filename[STRINGLEN], help_buf[RESBUF];
  int count;
  Arg args[MAXARGS];
  Dimension width, height;
  XFontStruct *font;

  scrolwin = XtNameToWidget(toplevel,
		      "Pod Help Screen.popup_help_form.popup_help_scrolwin");

  text = XtNameToWidget(scrolwin, "popup_help_text");

  count = 0;
  XtSetArg(args[count], XtNwidth, &width); count++;
  XtGetValues(scrolwin, args, count);
  
  (void) strcpy(filename, dua_help_dir);
  (void) strcat(filename, (String) closure);

  count = 0;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(text, args, count);

  height = FONTHEIGHT(font);
  count = 0;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetValues(text, args, count);

  height = 0;
  if (!(file = fopen(filename, "r"))) {
    (void) strcpy(filename, "./Xd/podHelpdir/");
    (void) strcat(filename, (String) closure);
    if (!(file = fopen(filename, "r"))) {
      (void) fprintf(stderr, "Helpfile '%s%s' not found\n", 
		     dua_help_dir, closure);
      return;
    }
  }

  if (file) {
    (void) strcpy(help_buf, "\0");
    while (fgets(help_string, STRINGLEN - 1, file)) {
      height += (FONTHEIGHT(font));
      (void) strcat(help_buf, help_string);
    }
    
    (void) fclose(file);
    count = 0;
    XtSetArg(args[count], XtNstring, ""); count++;
    XtSetValues(text, args, count);

    count = 0;
    XtSetArg(args[count], XtNwidth, width - 18); count++;
    XtSetArg(args[count], XtNheight, height); count++;
    XtSetArg(args[count], XtNstring, help_buf); count++;
    XtSetValues(text, args, count);
  }
}

static void CreateBackgroundPixmap(widget, bits, width, height)
     Widget widget;
     char bits[];
     Cardinal width, height;
{
  Arg args[MAXARGS];
  int count;
  Pixmap bitmap;

  bitmap = XCreatePixmapFromBitmapData(dpy,
				       XtWindow(widget), bits,
				       width, height,
				       BlackPixelOfScreen(screen),
				       WhitePixelOfScreen(screen),
				       DefaultDepthOfScreen(screen));

  count = 0;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetArg(args[count], XtNwidth, width); count++;
  XtSetArg(args[count], XtNbackgroundPixmap, bitmap); count++;
  XtSetArg(args[count], XtNresizable, FALSE); count++;
  XtSetValues(widget, args, count);

  XtAddCallback(widget, XtNdestroyCallback, 
		FreeWidgetPixmap, (XtPointer) bitmap);
}

/*ARGSUSED*/
static void ListDestroy(w, list_seq, calldata)
     Widget w;
     XtPointer calldata, list_seq;
{
  free_seq((str_seq) list_seq);
}

/*ARGSUSED*/
static void destroyList(w, shellwidget, calldata)
     Widget w;
     XtPointer shellwidget, calldata;
{
  if (curr_list_popup == (Widget) shellwidget) curr_list_popup = 0;
  
  XtPopdown((Widget) shellwidget);
  XtUnmanageChild((Widget) shellwidget);
  XtDestroyWidget((Widget) shellwidget);
  XFlush(dpy);
}

/*ARGSUSED*/
static void keepList(w, shellwidget, calldata)
     Widget w;
     XtPointer calldata, shellwidget;
{
  if (curr_list_popup == (Widget) shellwidget) curr_list_popup = 0;
}

static void CreateHistoryPopup(list_seq, mess)
     str_seq list_seq;
     char *mess;
{
  int count = 0;
  Widget shell, swindow, HistoryForm, ListWindow, closeButton;
  Arg args[MAXARGS];

  count = 0;
  shell = XtCreatePopupShell("Session History", topLevelShellWidgetClass,
                             toplevel, args, 0);
  count = 0;
  HistoryForm = XtCreateManagedWidget("HistoryForm", formWidgetClass,
				       shell, args, count);

  count = 0;
  XtSetArg(args[count], XtNheight, Close_height); count++;
  XtSetArg(args[count], XtNwidth, Close_width); count++;
  closeButton = XtCreateManagedWidget("closeButton",
				     commandWidgetClass,
                                     HistoryForm, args, count);
  
  XtAddCallback(closeButton, XtNcallback,
                (XtCallbackProc) popdownHistory, (XtPointer) shell);

  count = 0;
  XtSetArg(args[count], XtNlabel, "History Window"); count++;
  (void) XtCreateManagedWidget("HistoryTitle", labelWidgetClass, 
				HistoryForm, args, count);

  count = 0;
  swindow = XtCreateManagedWidget("ListScrolledWindow", viewportWidgetClass,
				  HistoryForm, args, count);

  count = 0;
  ListWindow = XtCreateManagedWidget("ListWindow", formWidgetClass,
				     swindow, args, count);

  count = 0;
  XtSetArg(args[count], XtNlabel, mess); count++;
  (void) XtCreateManagedWidget("ListMessage", labelWidgetClass,
                                HistoryForm, args, count);
  
  XtAddCallback(ListWindow, XtNdestroyCallback, 
		ListDestroy, (XtPointer) list_seq);

  AddNewList(ListWindow, list_seq,  (unsigned) entry_number);
  XtRealizeWidget(shell);

  CreateBackgroundPixmap(closeButton, Close_bits, Close_width, Close_height);
  if (DefaultDepthOfScreen(screen) == 1)
    CreateBackgroundPixmap(HistoryForm, gray_bits, gray_width, gray_height);
}

/*ARGSUSED*/
static void popupHistory(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget history_popup;

  history_popup = XtNameToWidget(toplevel, "Session History");
  XtPopup(history_popup, XtGrabNone);
  XRaiseWindow(dpy, XtWindow(history_popup));
}

/*ARGSUSED*/
static void popdownHistory(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget history_popup;

  history_popup = XtNameToWidget(toplevel, "Session History");
  XtPopdown(history_popup);
}


/*ARGSUSED*/
static void ChangeHelp(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  if ((help_up) && strcmp(params[0], curr_help)) {
    InsertHelp((Widget) 0, (XtPointer) params[0], (XtPointer) 0);
    (void) strcpy(curr_help, params[0]);
  }
}

/*ARGSUSED*/
static void buttonPress(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal num_params;
{
  char *buttonName = params[0];

  if (!strcmp(buttonName, "search")) 
    CreateBackgroundPixmap(w, SearchPress_bits, 
			   SearchPress_width, SearchPress_height);
  else if (!strcmp(buttonName, "help")) 
    CreateBackgroundPixmap(w, HelpPress_bits,
			   HelpPress_width, HelpPress_height);
  else if (!strcmp(buttonName, "quit"))
    CreateBackgroundPixmap(w, QuitPress_bits,
                           QuitPress_width, QuitPress_height);
  else if (!strcmp(buttonName, "list"))
    CreateBackgroundPixmap(w, ListPress_bits,
                           ListPress_width, ListPress_height);
  else if (!strcmp(buttonName, "history"))
    CreateBackgroundPixmap(w, HistoryPress_bits,
                           HistoryPress_width, HistoryPress_height);
  else if (!strcmp(buttonName, "close"))
    CreateBackgroundPixmap(w, ClosePress_bits,
                           ClosePress_width, ClosePress_height);
  else if (!strcmp(buttonName, "about"))
    CreateBackgroundPixmap(w, AboutPress_bits,
                           AboutPress_width, AboutPress_height);
  else if (!strcmp(buttonName, "keep"))
    CreateBackgroundPixmap(w, KeepPress_bits,
                           KeepPress_width, KeepPress_height);
  else if (!strcmp(buttonName, "modify"))
    CreateBackgroundPixmap(w, ModifyPress_bits,
			   ModifyPress_width, ModifyPress_height);
  else if (!strcmp(buttonName, "show_all"))
    CreateBackgroundPixmap(w, ShowAllPress_bits,
			   ShowAllPress_width, ShowAllPress_height);
  else if (!strcmp(buttonName, "submit_modify"))
    CreateBackgroundPixmap(w, ModifyPress_bits,
			   ModifyPress_width, ModifyPress_height);

  XFlush(dpy);
  XtCallCallbacks(w, XtNcallback, NULL);
  XFlush(dpy);

  if (!strcmp(buttonName, "search")) 
    CreateBackgroundPixmap(w, Search_bits, 
			   Search_width, Search_height);
  else if (!strcmp(buttonName, "help")) 
    CreateBackgroundPixmap(w, Help_bits,
			   Help_width, Help_height);
  else if (!strcmp(buttonName, "quit"))
        CreateBackgroundPixmap(w, Quit_bits,
                           Quit_width, Quit_height);
  else if (!strcmp(buttonName, "list"))
    CreateBackgroundPixmap(w, List_bits,
                           List_width, List_height);
  else if (!strcmp(buttonName, "history"))
    CreateBackgroundPixmap(w, History_bits,
                           History_width, History_height);
  else if (!strcmp(buttonName, "close"))
    CreateBackgroundPixmap(w, Close_bits,
                           Close_width, Close_height);
  else if (!strcmp(buttonName, "about"))
    CreateBackgroundPixmap(w, About_bits,
                           About_width, About_height);
  else if (!strcmp(buttonName, "keep"))
    CreateBackgroundPixmap(w, Kept_bits,
                           Kept_width, Kept_height);
  else if (!strcmp(buttonName, "modify"))
    CreateBackgroundPixmap(w, Modify_bits,
                           Modify_width, Modify_height);
  else if (!strcmp(buttonName, "show_all"))
    CreateBackgroundPixmap(w, ShowAll_bits,
			   ShowAll_width, ShowAll_height);
  else if (!strcmp(buttonName, "submit_modify"))
    CreateBackgroundPixmap(w, Modify_bits,
			   Modify_width, Modify_height);

  XFlush(dpy);
}

/*ARGSUSED*/
static void readDestroy(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Widget readForm = XtNameToWidget((Widget) closure, "ReadForm");
  PhotoWindow = XtNameToWidget(readForm, 
			       "TextScrolledWindow.TextForm.PhotoWindow");

  if (PhotoWindow != (Widget) 0) kill_photo();

  if (curr_read_popup == (Widget) closure) {
    XtPopdown(curr_read_popup);
    return;
  }

  if (curr_read_popup == (Widget) 0) {
    Widget keepButton = XtNameToWidget((Widget) closure, 
				       "ReadForm.ReadButtonForm.keepButton");

    CreateBackgroundPixmap(keepButton, Keep_bits, Keep_width, Keep_height);
    XtPopdown((Widget) closure);

    curr_read_popup = (Widget) closure;
    return;
  }
  
  XtPopdown((Widget) closure);

  XtUnmanageChild(readForm);
  XtDestroyWidget(readForm);

  XtUnmanageChild((Widget) closure);
  XtDestroyWidget((Widget) closure);
}

/*ARGSUSED*/
static void keepRead(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  if (curr_read_popup == (Widget) closure) {
    PhotoWindow = 0;
    curr_read_popup = 0;
  }
}

/*ARGSUSED*/
static void modifyEntry(w, object, calldata)
     Widget w;
     XtPointer object, calldata;
{
char *tmp;
  tmp = (char *)object;
  if (*tmp == '\0') return;
  
  if (!curr_modify_popup)
    curr_modify_popup = createModifyPopup((char *) object);
  else {
    Widget modify_form = XtNameToWidget(curr_modify_popup, "modifyForm");
    XawFormDoLayout(modify_form, FALSE);
    (void) createModifyTemplate(curr_modify_popup, (char *) object);
    XawFormDoLayout(modify_form, TRUE);
    XtPopup(curr_modify_popup, XtGrabNone);
    XRaiseWindow(dpy, XtWindow(curr_modify_popup));
  }
}


/*ARGSUSED*/
static void closeModify(w, clientdata, calldata)
     Widget w;
     XtPointer clientdata, calldata;
{
  Widget modifyPopup = (Widget) clientdata;
  Widget modifyForm = XtNameToWidget(modifyPopup, "modifyForm");
  Widget modifyButtonForm = XtNameToWidget(modifyForm, "ModifyButtonForm");
  Widget modifyScrolledWindow = XtNameToWidget(modifyForm, 
					       "modifyScrolledWindow");
  Widget modMessage = XtNameToWidget(modifyForm, "modMessage");

  XtPopdown(modifyPopup);

  if (modifyPopup == curr_modify_popup) return;
    
  if (curr_modify_popup == (Widget) 0) {
    Widget keepButton = XtNameToWidget(modifyButtonForm, "keepButton");
    CreateBackgroundPixmap(keepButton, Keep_bits, Keep_width, Keep_height);

    return;
  }

  XtUnmanageChild (modifyButtonForm);
  XtDestroyWidget(modifyButtonForm);

  XtUnmanageChild(modifyScrolledWindow);
  XtDestroyWidget(modifyScrolledWindow);

  XtUnmanageChild(modMessage);
  XtDestroyWidget(modMessage);

  XtUnmanageChild(modifyForm);
  XtDestroyWidget(modifyForm);

  XtUnmanageChild(modifyPopup);
  XtDestroyWidget(modifyPopup);

  if (modifyPopup == curr_modify_popup) curr_modify_popup = (Widget) 0;
}

/*ARGSUSED*/
static void keepModify(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  if (curr_modify_popup == (Widget) closure) {
    curr_modify_popup = 0;
  }
}

/*ARGSUSED*/
static void modUpdate(w, attrValue, calldata)
     Widget w;
     XtPointer attrValue, calldata;
{
  modVals val = (modVals) attrValue;
  val->mod_flag = TRUE;
  val->attr->mod_flag = TRUE;
}


/*ARGSUSED*/
static void submitModif(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  dirEntry mods = (dirEntry) closure;
  dirAttrs attrs;
  modVals vals;
  int curr_mod_num = 0;
  int count;
  Arg args[MAXARGS];
  char *string, error_message[STRINGLEN];
  dsErrorStruct mod_error;

  for (attrs = mods->attrs; attrs; attrs = attrs->next)
    if (attrs->mod_flag) {
      for (vals = attrs->val_seq; vals; vals = vals->next)
	if (vals->mod_flag) {
	  if (vals->text_widg) {
	    count = 0;
	    XtSetArg(args[count], XtNstring, &string); count++;
	    XtGetValues(vals->text_widg, args, count);
	    
	    if (!(*string == '\0' && (!vals->value || *vals->value == '\0'))) {
	      vals->new_value = (*string == '\0'? NULLCP: strdup(string));
	      curr_mod_num++;
	    } else {
	      vals->mod_flag = FALSE;
	    }
	  } else {
	    if (vals->value && *vals->value != '\0') {
	      vals->new_value = 0;
	      curr_mod_num++;
	    } else {
	      vals->mod_flag = FALSE;
	    }
	  }
	}
      if (curr_mod_num == 0) attrs->mod_flag = 0;
      curr_mod_num = 0;
    }

  message(XtParent(XtParent(XtParent(w))), "Modifying, please stand by......");
  mod_error = modify_entry(mods);
  kill_message();
  
  switch (mod_error.error) {
  case Okay:
    break;
  default:
    XBell(dpy, 100);
    (void) strcpy(error_message, "Cannot modify!\n");
    (void) strcat(error_message, mod_error.err_mess);
    (void) strcat(error_message, "\n");
    (void) strcat(error_message, "Click on this window to continue.");
    displayError(XtParent(XtParent(XtParent(w))), error_message);
    free(mod_error.err_mess);
  }
}


XtCallbackRec modCallbacks[] = {
  {modUpdate, (XtPointer) NULL},
  {(XtCallbackProc) NULL, (XtPointer) NULL},
};

/*ARGSUSED*/
static void deleteVal(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  modVals this_val = (modVals) closure, curr_val;
  dirAttrs this_attr = (dirAttrs) this_val->attr, curr_attr;
  Widget valWidg, menuWidg, nextValWidg, nextValMenuWidg, 
         nextAttrWidg = 0, fromVertWidg, form;
  int count, vertDistance;
  Arg args[MAXARGS];
  
  valWidg = this_val->text_widg;
  if (!valWidg) return;

  form = XtParent(valWidg);
  XawFormDoLayout(form, FALSE);

  count = 0;
  XtSetArg(args[count], XtNfromVert, &fromVertWidg); count++;
  XtSetArg(args[count], XtNfromHoriz, &menuWidg); count++;
  XtGetValues(valWidg, args, count);

  curr_val = this_attr->val_seq;
  while (curr_val && curr_val != this_val && !(curr_val->text_widg))
    curr_val = curr_val->next;

  if (curr_val == this_val) vertDistance = 10;
  else vertDistance = 2;

  curr_val = this_val->next;
  while (curr_val && !(nextValWidg = curr_val->text_widg)) 
    curr_val = curr_val->next;
    
  if (curr_val && nextValWidg) {
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &nextValMenuWidg); count++;
    XtGetValues(nextValWidg, args, count);
    
    this_val->text_widg = 0;
    nextAttrWidg = 0;

    XtUnmanageChild(valWidg);
    XtDestroyWidget(valWidg);
    XtUnmanageChild(menuWidg);
    XtDestroyWidget(menuWidg);
  } else {
    curr_attr = this_attr->next;

    if (curr_attr) {
      vertDistance = 10;

      curr_val = curr_attr->val_seq;
      while (!(nextValWidg = curr_val->text_widg)) curr_val = curr_val->next;

      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &nextValMenuWidg); count++;
      XtGetValues(nextValWidg, args, count);

      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &nextAttrWidg); count++;
      XtGetValues(nextValMenuWidg, args, count);
    } else 
      nextValWidg = nextValMenuWidg = nextAttrWidg = 0;

    curr_val = this_attr->val_seq;
    while (curr_val && !curr_val->text_widg) curr_val = curr_val->next;
 
    if (this_val == curr_val) {
      count = 0;
      XtSetArg(args[count], XtNstring, ""); count++;
      XtSetValues(valWidg, args, count);
      
      nextValWidg = nextValMenuWidg = nextAttrWidg = 0;
    } else {
      this_val->text_widg = 0;

      XtUnmanageChild(valWidg);
      XtDestroyWidget(valWidg);
      XtUnmanageChild(menuWidg);
      XtDestroyWidget(menuWidg);
    }
  }

  if (nextValWidg) {
    count = 0;
    XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
    XtSetArg(args[count], XtNfromVert, fromVertWidg); count++;

    XtSetValues(nextValMenuWidg, args, count);
    XtSetValues(nextValWidg, args, count);

    if (nextAttrWidg) XtSetValues(nextAttrWidg, args, count);
  }

  this_val->mod_flag = TRUE;
  this_attr->mod_flag = TRUE;
  if (this_val->new_value) {
    free(this_val->new_value);
    this_val->new_value = 0;
  }

  XawFormDoLayout(form, TRUE);
}
  
/*ARGSUSED*/
static void addValField(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  dirAttrs attr = (dirAttrs) closure;
  modVals first_val = attr->val_seq, new_val;
  Widget newValWidg, firstValWidg = first_val->text_widg, fromVertWidg, form,
         modValButton, modValMenu, modValMenuButton, firstValButton;
  int count;
  Arg args[MAXARGS];
  XFontStruct *font = 0;
  Dimension height;

  new_val = (modVals) malloc(sizeof(struct mod_vals));
  new_val->next = first_val;
  new_val->mod_flag = FALSE;
  new_val->new_value = 0;
  new_val->value = 0;
  new_val->attr = attr;

  attr->val_seq = new_val;

  form = XtParent(XtParent(XtParent(w)));
  XawFormDoLayout(form, FALSE);

  while (first_val && !(firstValWidg = first_val->text_widg)) 
    first_val = first_val->next;

  firstValWidg = first_val->text_widg;

  count = 0;
  XtSetArg(args[count], XtNfromVert, &fromVertWidg); count++;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(firstValWidg, args, count);

  count = 0;
  XtSetArg(args[count], XtNfromHoriz, XtParent(XtParent(w))); count++;
  XtSetArg(args[count], XtNfromVert, fromVertWidg); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNwidth, menuicon_width); count++;
  XtSetArg(args[count], XtNheight, menuicon_height); count++;
  XtSetArg(args[count], XtNvertDistance, 10); count++;
  modValButton = XtCreateWidget("modValMenu", 
				menuButtonWidgetClass,
				form, args, count);

  count = 0;
  modValMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass, 
				  modValButton, args, count);
			 
  count = 0;
  XtSetArg(args[count], XtNlabel, "Delete this value"); count++;
  modValMenuButton = XtCreateManagedWidget("delValMenuButton",
					   smeBSBObjectClass,
					   modValMenu, args, count);

  XtAddCallback(modValMenuButton, XtNcallback,
		deleteVal, (XtPointer) new_val);

  count = 0;
  XtSetArg(args[count], XtNlabel, "Undo changes to this value"); count++;
  modValMenuButton = XtCreateManagedWidget("undoValMenuButton",
					   smeBSBObjectClass,
					   modValMenu, args, count);

  XtAddCallback(modValMenuButton, XtNcallback,
		UndoValChanges, (XtPointer) new_val);
  
  height = FONTHEIGHT(font) + font->ascent;

  modCallbacks[0].closure = (XtPointer) new_val;
  count = 0;
  XtSetArg(args[count], XtNfromHoriz, modValButton); count++;
  XtSetArg(args[count], XtNeditType, XawtextEdit); count++;
  XtSetArg(args[count], XtNresizable, TRUE); count++;
  XtSetArg(args[count], XtNresize, XawtextResizeBoth); count++;
  XtSetArg(args[count], XtNstring, ""); count++;
  XtSetArg(args[count], XtNcallback, modCallbacks); count++;
  XtSetArg(args[count], XtNheight, height); count++;
  XtSetArg(args[count], XtNfromVert, fromVertWidg); count++;
  XtSetArg(args[count], XtNvertDistance, 10); count++;
  newValWidg = XtCreateWidget("valText", asciiTextWidgetClass,
			      form, args, count);
  new_val->text_widg = newValWidg;

  count = 0;
  XtSetArg(args[count], XtNfromHoriz, &firstValButton); count++;
  XtGetValues(firstValWidg, args, count);

  count = 0;
  XtSetArg(args[count], XtNfromVert, newValWidg); count++;
  XtSetArg(args[count], XtNvertDistance, 2); count++;
  XtSetValues(firstValWidg, args, count);
  XtSetValues(firstValButton, args, count);

  XFlush(dpy);

  XtManageChild(modValButton);
  XtManageChild(newValWidg);

  CreateBackgroundPixmap(modValButton, menuicon_bits, 
			 menuicon_width, menuicon_height);

  XawFormDoLayout(form, TRUE);
}

/*ARGSUSED*/
static void UndoAttrChanges(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  dirAttrs this_attr = (dirAttrs) closure;
  modVals this_val = this_attr->val_seq;
  Widget form = XtParent(XtParent(XtParent(w)));

  this_attr->mod_flag = FALSE;

  XawFormDoLayout(form, FALSE);
  while (this_val) {
    UndoValChanges(w, (XtPointer) this_val, (XtPointer) w);
    this_val = this_val->next;
  }
  XawFormDoLayout(form, TRUE); 
  XFlush(dpy);
}

/*ARGSUSED*/
static void UndoValChanges(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  int count;
  Arg args[MAXARGS];
  Widget thisValWidg, thisValMenuWidg, thisAttrWidg,
         prevValWidg, prevValMenuWidg,
         nextValWidg, nextValMenuWidg, nextAttrWidg,
         modValMenu, modValMenuButton, form;
  int vertDistance;
  modVals this_val = (modVals) closure, next_val, prev_val;
  dirAttrs this_attr = this_val->attr, next_attr;
  
  if (!this_val->mod_flag && this_val->value) return;
  
  if (w != (Widget) calldata) {
    form = XtParent(XtParent(XtParent(w)));
    XawFormDoLayout(form, FALSE);
  }

  this_val->mod_flag = FALSE;
  thisValWidg = this_val->text_widg;
  
  if (this_val->new_value) {
    free(this_val->new_value);
    this_val->new_value = NULLCP;
  }

  if (this_val->value && *this_val->value != '\0') {
    if (!thisValWidg) {
      prevValWidg = 0;
      prev_val = this_attr->val_seq;
      while (prev_val && prev_val != this_val) {
	if (prev_val->text_widg) prevValWidg = prev_val->text_widg;
	prev_val = prev_val->next;
      }
      
      nextValWidg = 0;
      if (!prevValWidg) {
	next_val = this_val->next;
	while (next_val && !(next_val->text_widg)) next_val = next_val->next;
	
	if (!next_val) {
	  if (w != (Widget) calldata) XawFormDoLayout(form, TRUE);
	  return;
	}

	nextValWidg = next_val->text_widg;
	
	count = 0;
	XtSetArg(args[count], XtNfromVert, &prevValWidg); count++;
	XtGetValues(nextValWidg, args, count);

	count = 0;
        XtSetArg(args[count], XtNfromHoriz, &prevValMenuWidg); count++;
	XtGetValues(nextValWidg, args, count);

	count = 0;
	XtSetArg(args[count], XtNfromHoriz, &thisAttrWidg); count++;
	XtGetValues(prevValMenuWidg, args, count);
	
	vertDistance = 10;
      } else {
	count = 0;
	XtSetArg(args[count], XtNfromHoriz, &prevValMenuWidg); count++;
	XtGetValues(prevValWidg, args, count);
	
	count = 0;
	XtSetArg(args[count], XtNfromHoriz, &thisAttrWidg); count++;
	XtGetValues(prevValMenuWidg, args, count);
	
	vertDistance = 2;
      }
      
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, thisAttrWidg); count++;
      XtSetArg(args[count], XtNfromVert, prevValWidg); count++;
      XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
      XtSetArg(args[count], XtNwidth, menuicon_width); count++;
      XtSetArg(args[count], XtNheight, menuicon_height); count++;
      thisValMenuWidg = XtCreateWidget("modValMenu",
				       menuButtonWidgetClass,
				       XtParent(thisAttrWidg), args, count);
      
      count = 0;
      modValMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,
                                      thisValMenuWidg, args, count);
      
      count = 0;
      XtSetArg(args[count], XtNlabel, "Delete this value"); count++;
      modValMenuButton = XtCreateManagedWidget("delValMenuButton",
					       smeBSBObjectClass,
					       modValMenu, args, count);

      XtAddCallback(modValMenuButton, XtNcallback,
                    deleteVal, (XtPointer) this_val);
      
      count = 0;
      XtSetArg(args[count], XtNlabel, "Undo changes to this value"); count++;
      modValMenuButton = XtCreateManagedWidget("undoValMenuButton",
                                               smeBSBObjectClass,
                                               modValMenu, args, count);
      
      XtAddCallback(modValMenuButton, XtNcallback,
                    UndoValChanges, (XtPointer) this_val);
      
      modCallbacks[0].closure = (XtPointer) this_val;
      
      count = 0;
      XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
      XtSetArg(args[count], XtNfromVert, prevValWidg); count++;
      XtSetArg(args[count], XtNfromHoriz, thisValMenuWidg); count++;
      XtSetArg(args[count], XtNstring, this_val->value); count++;
      XtSetArg(args[count], XtNeditType, XawtextEdit); count++;
      XtSetArg(args[count], XtNresize, XawtextResizeBoth); count++;
      XtSetArg(args[count], XtNcallback, modCallbacks); count++;
      XtSetArg(args[count], XtNresizable, TRUE); count++;
      thisValWidg = XtCreateWidget("valText", asciiTextWidgetClass,
				   XtParent(thisAttrWidg), args, count);

      this_val->text_widg = thisValWidg;

      next_attr = 0;
      next_val = this_val->next;
      while (next_val && !(next_val->text_widg)) next_val = next_val->next;
      
      if (next_val) nextValWidg = next_val->text_widg;
      else {
	next_attr = this_attr->next;
	if (!next_attr) nextValWidg = 0;
	else {
	  next_val = next_attr->val_seq;
	  while (!(next_val->text_widg)) next_val = next_val->next;

	  nextValWidg = next_val->text_widg;
	}
      }

      if (nextValWidg) {
	count = 0;
	XtSetArg(args[count], XtNfromHoriz, &nextValMenuWidg); count++;
	XtGetValues(nextValWidg, args, count);

	if (next_attr) {
	  vertDistance = 10;

	  count = 0;
	  XtSetArg(args[count], XtNfromHoriz, &nextAttrWidg); count++;
	  XtGetValues(nextValMenuWidg, args, count);
	} else {
	  vertDistance = 2;
	  nextAttrWidg = 0;
	}

	count = 0;
	XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
	XtSetArg(args[count], XtNfromVert, thisValWidg); count++;
	
	XtSetValues(nextValWidg, args, count);
	XtSetValues(nextValMenuWidg, args, count);
	
	if (nextAttrWidg) XtSetValues(nextAttrWidg, args, count);
      }
      
      XtManageChild(thisValMenuWidg);
      XtManageChild(thisValWidg);

      count = 0;
      XtSetArg(args[count], 
	       XtNwidth, 
	       (Dimension) GetTextWidth(thisValWidg,this_val->value)); count++;
      XtSetArg(args[count], 
	       XtNheight, 
	       (Dimension) GetTextHeight(thisValWidg,this_val->value));count++;
      XtSetValues(thisValWidg, args, count);

      CreateBackgroundPixmap(thisValMenuWidg, menuicon_bits, 
			     menuicon_width, menuicon_height);
    } else {
      count = 0;
      XtSetArg(args[count], 
	       XtNwidth, 
	       GetTextWidth(thisValWidg, this_val->value)); count++;
      XtSetArg(args[count], 
	       XtNheight, 
	       GetTextHeight(thisValWidg, this_val->value)); count++;
      XtSetArg(args[count], XtNstring, this_val->value); count++;
      XtSetValues(thisValWidg, args, count);
    }
  } else {
    deleteVal(thisValWidg, (XtPointer) this_val, (XtPointer) 0);
  }
  if (w != (Widget) calldata) XawFormDoLayout(form, TRUE);
}

static dirEntry createModifyTemplate(modify_form, entry_name)
     Widget modify_form;
     char *entry_name;
{
  Widget modifyViewForm, modAttrMenu, attrName, lastVal,
         modAttrMenuButton, modValButton, modValMenu, modifyScrolledWindow,
         valName, modValMenuButton, modifyButton;
  dirEntry entry;
  dirAttrs attrs, curr_attrs;
  modVals curr_val;
  Dimension text_width, text_height, max_width;
  Arg args[MAXARGS];
  int count, vertDistance;

  make_template(entry_name, &attrs);

  entry = (dirEntry) malloc(sizeof(dir_entry));
  entry->entry_name = malloc((unsigned) strlen(entry_name) + 1);
  (void) strcpy(entry->entry_name, entry_name);
  entry->attrs = attrs;

  modifyViewForm = XtNameToWidget(modify_form,
		   "modifyForm.modifyScrolledWindow.modifyViewForm");
  modifyScrolledWindow = XtNameToWidget(modify_form, 
					"modifyForm.modifyScrolledWindow");

  if (modifyViewForm != 0) {
    XtUnmanageChild(modifyViewForm);
    XtDestroyWidget(modifyViewForm);
  }

  count = 0;
  modifyViewForm = XtCreateWidget("modifyViewForm", formWidgetClass,
				  modifyScrolledWindow, args, count);

  lastVal = (Widget) 0;
  for (curr_attrs = entry->attrs; curr_attrs; curr_attrs = curr_attrs->next) {
    vertDistance = 10;

    count = 0;
    if (lastVal) {XtSetArg(args[count], XtNfromVert, lastVal); count++;}
    XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
    XtSetArg(args[count], XtNlabel, ""); count++;
    XtSetArg(args[count], XtNjustify, XtJustifyLeft); count++;
    XtSetArg(args[count], XtNresizable, TRUE); count++;
    attrName = XtCreateWidget("attrLabel", menuButtonWidgetClass,
			      modifyViewForm, args, count);

    text_width = GetTextWidth(attrName, curr_attrs->attr_name);

    count = 0;
    modAttrMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,
				     attrName, args, count);

    count = 0;
    XtSetArg(args[count], XtNlabel, "Add value field"); count++;
    modAttrMenuButton = XtCreateManagedWidget("addValMenuButton", 
					      smeBSBObjectClass, 
					      modAttrMenu, args, count);

    XtAddCallback(modAttrMenuButton, XtNcallback, 
		  addValField, (XtPointer) curr_attrs);

    count = 0;
    XtSetArg(args[count], XtNlabel, "Undo ALL changes"); count++;
    modAttrMenuButton = XtCreateManagedWidget("undoMenuButton",
                                              smeBSBObjectClass,
                                              modAttrMenu, args, count);

    XtAddCallback(modAttrMenuButton, XtNcallback,
		  UndoAttrChanges, (XtPointer) curr_attrs);

    count = 0;
    XtSetArg(args[count], XtNlabel, curr_attrs->attr_name); count++;
    XtSetArg(args[count], XtNwidth, text_width); count++;
    XtSetValues(attrName, args, count);

    for (curr_val = curr_attrs->val_seq; 
	 curr_val; 
	 curr_val = curr_val->next) {
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, attrName); count++;
      if (lastVal) {XtSetArg(args[count], XtNfromVert, lastVal); count++;}
      XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
      XtSetArg(args[count], XtNresizable, TRUE); count++;
      XtSetArg(args[count], XtNwidth, menuicon_width); count++;
      XtSetArg(args[count], XtNheight, menuicon_height); count++;
      modValButton = XtCreateWidget("modValMenu", 
				    menuButtonWidgetClass,
				    modifyViewForm, args, count);

      count = 0;
      modValMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass, 
				      modValButton, args, count);
			 
      count = 0;
      XtSetArg(args[count], XtNlabel, "Delete this value"); count++;
      modValMenuButton = XtCreateManagedWidget("delValMenuButton",
						smeBSBObjectClass,
						modValMenu, args, count);

      XtAddCallback(modValMenuButton, XtNcallback,
		    deleteVal, (XtPointer) curr_val);

      count = 0;
      XtSetArg(args[count], XtNlabel, "Undo changes to this value"); count++;
      modValMenuButton = XtCreateManagedWidget("undoValMenuButton",
					       smeBSBObjectClass,
					       modValMenu, args, count);
      
      XtAddCallback(modValMenuButton, XtNcallback,
		    UndoValChanges, (XtPointer) curr_val);
      
      modCallbacks[0].closure = (XtPointer) curr_val;
      count = 0;
      if (curr_val->value)
	if (!curr_attrs->hidden_flag) {
	  XtSetArg(args[count], XtNstring, curr_val->value); count++;
	} else {
	  XtSetArg(args[count], XtNstring, "HIDDEN"); count++;
	}

      if (lastVal) {
	XtSetArg(args[count], XtNfromVert, lastVal); count++;
      }

      XtSetArg(args[count], XtNfromHoriz, modValButton); count++;
      XtSetArg(args[count], XtNvertDistance, vertDistance); count++;
      XtSetArg(args[count], XtNeditType, XawtextEdit); count++;
      XtSetArg(args[count], XtNresize, XawtextResizeBoth); count++;
      XtSetArg(args[count], XtNresizable, TRUE); count++;
      XtSetArg(args[count], XtNcallback, modCallbacks); count++;
      valName = XtCreateWidget("valText", asciiTextWidgetClass,
			       modifyViewForm, args, count);

      text_height = text_width = 0;
      if (!attrs->hidden_flag) {
	text_height = GetTextHeight(valName, curr_val->value);
	text_width = GetTextWidth(valName, curr_val->value);
      } else {
	text_height = GetTextHeight(valName, "HIDDEN");
	text_width = GetTextWidth(valName, "HIDDEN");
      }
      
      count = 0;
      if (text_height && text_width) {
	XtSetArg(args[count], XtNwidth, text_width); count++;
	XtSetArg(args[count], XtNheight, text_height); count++;
	XtSetValues(valName, args, count);
      }

      curr_val->text_widg = valName;

      lastVal = valName;
      vertDistance = 2;
    }
  }

  if (curr_modify_popup) {
    for (max_width = 0, curr_attrs = entry->attrs; 
	 curr_attrs; 
	 curr_attrs = curr_attrs->next) {
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
      XtGetValues(curr_attrs->val_seq->text_widg, args, count);
      
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &attrName); count++;
      XtGetValues(modValButton, args, count);
      
      count = 0;
      XtSetArg(args[count], XtNwidth, &text_width); count++;
      XtGetValues(attrName, args, count);
      
      max_width = (text_width > max_width? text_width: max_width);
    }
    
    for(curr_attrs = entry->attrs; curr_attrs; curr_attrs = curr_attrs->next) {
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
      XtGetValues(curr_attrs->val_seq->text_widg, args, count);
      
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &attrName); count++;
      XtGetValues(modValButton, args, count);
      
      count = 0;
      XtSetArg(args[count], XtNwidth, max_width); count++;
      XtSetValues(attrName, args, count);
      
      XtManageChild(attrName);
      
      count = 0;
      XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
      
      for (curr_val = curr_attrs->val_seq;
	   curr_val; 
	   curr_val = curr_val->next) {
	valName = curr_val->text_widg;
	XtGetValues(valName, args, count);
	XtManageChild(modValButton);
	XtManageChild(valName);
      }
    }

    XtManageChild(modifyViewForm);

    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
    
    for(curr_attrs = entry->attrs; curr_attrs; curr_attrs = curr_attrs->next) {
      for (curr_val = curr_attrs->val_seq;
	   curr_val; 
	   curr_val = curr_val->next) {
	valName = curr_val->text_widg;
        XtGetValues(valName, args, count);
	CreateBackgroundPixmap(modValButton, menuicon_bits,
                               menuicon_width, menuicon_height);
      }
    }

    modifyButton = XtNameToWidget(modify_form, 
				  "modifyForm.ModifyButtonForm.modifyButton");

    if (XtHasCallbacks(modifyButton, XtNcallback) == XtCallbackHasSome)
      XtRemoveAllCallbacks(modifyButton, XtNcallback);

    XtAddCallback(modifyButton, XtNcallback, submitModif, (XtPointer) entry);
    XtAddCallback(modifyButton, XtNdestroyCallback,
		  freeEntry, (XtPointer) entry);

  } else XtManageChild(modifyViewForm);
  return entry;
}

static Widget createModifyPopup(entry_name)
     char *entry_name;
{
  Widget modifyPopup, modifyForm, modifyButton, closeButton, keepButton, 
         attrName, valName, ButtonForm, modValButton;
  dirEntry entry;
  dirAttrs curr_attrs;
  modVals curr_val;
  Dimension text_width, max_width;
  Arg args[MAXARGS];
  int count;

  modifyPopup = XtCreatePopupShell("Modify Entry", topLevelShellWidgetClass,
				   toplevel, args, 0);

  count = 0;
  modifyForm = XtCreateManagedWidget("modifyForm", formWidgetClass,
				     modifyPopup, args, count);

  count = 0;
  ButtonForm = XtCreateManagedWidget("ModifyButtonForm", formWidgetClass,
				     modifyForm, args, count);

  count = 0;
  XtSetArg(args[count], XtNheight, Close_height); count++;
  XtSetArg(args[count], XtNwidth, Close_width); count++;
  closeButton = XtCreateManagedWidget("closeButton", commandWidgetClass,
				      ButtonForm, args, count);

  XtAddCallback(closeButton, XtNcallback, closeModify,
		(XtPointer) modifyPopup);

  count = 0;
  XtSetArg(args[count], XtNheight, Keep_height); count++;
  XtSetArg(args[count], XtNwidth, Keep_width); count++;
  keepButton = XtCreateManagedWidget("keepButton", commandWidgetClass,
                                      ButtonForm, args, count);

  XtAddCallback(keepButton, XtNcallback, keepModify, (XtPointer) modifyPopup);

  count = 0;
  XtSetArg(args[count], XtNheight, Modify_height); count++;
  XtSetArg(args[count], XtNwidth, Modify_width); count++;
  modifyButton = XtCreateManagedWidget("modifyButton", commandWidgetClass,
				       ButtonForm, args, count);
  
  count = 0;
  (void) XtCreateManagedWidget("modifyScrolledWindow",
			       viewportWidgetClass, modifyForm,
			       args, count);

  entry = createModifyTemplate(modifyPopup, entry_name);

  XtAddCallback(modifyButton, XtNcallback, submitModif, (XtPointer) entry);
  XtAddCallback(modifyButton, XtNdestroyCallback, 
		freeEntry, (XtPointer) entry);

  XtRealizeWidget(modifyPopup);

  for (max_width = 0, curr_attrs = entry->attrs; 
       curr_attrs; 
       curr_attrs = curr_attrs->next) {
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
    XtGetValues(curr_attrs->val_seq->text_widg, args, count);
    
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &attrName); count++;
    XtGetValues(modValButton, args, count);
    
    count = 0;
    XtSetArg(args[count], XtNwidth, &text_width); count++;
    XtGetValues(attrName, args, count);
    
    max_width = (text_width > max_width? text_width: max_width);
  }
  
  for(curr_attrs = entry->attrs; curr_attrs; curr_attrs = curr_attrs->next) {
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
    XtGetValues(curr_attrs->val_seq->text_widg, args, count);
    
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &attrName); count++;
    XtGetValues(modValButton, args, count);
      
    count = 0;
    XtSetArg(args[count], XtNwidth, max_width); count++;
    XtSetValues(attrName, args, count);
    
    XtManageChild(attrName);
    
    count = 0;
    XtSetArg(args[count], XtNfromHoriz, &modValButton); count++;
    
    for (curr_val = curr_attrs->val_seq;
	 curr_val; 
	 curr_val = curr_val->next) {
      valName = curr_val->text_widg;
      XtGetValues(valName, args, count);
      XtManageChild(modValButton);
      XtManageChild(valName);
      CreateBackgroundPixmap(modValButton, menuicon_bits, 
			     menuicon_width, menuicon_height);
    }
  }
  
  CreateBackgroundPixmap(modifyButton, Modify_bits, 
			 Modify_width, Modify_height);
  CreateBackgroundPixmap(closeButton, Close_bits,
			 Close_width, Close_height);
  CreateBackgroundPixmap(keepButton, Keep_bits,
			 Keep_width, Keep_height);

  if (DefaultDepthOfScreen(screen) == 1) {
    CreateBackgroundPixmap(ButtonForm, gray_bits,
			   gray_width, gray_height);
    CreateBackgroundPixmap(modifyForm, gray_bits,
			   gray_width, gray_height);
  }

  XtPopup(modifyPopup, XtGrabNone);
  XRaiseWindow(dpy, XtWindow(modifyPopup));
  XFlush(dpy);

  return(modifyPopup);
}


/*ARGSUSED*/
static void freeEntry(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  free_dir_entry((dirEntry) closure);
}

/*ARGSUSED*/
static void freeSpace(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  free(closure);
}

static int GetTextWidth(widget, istring)
     Widget widget;
     char *istring;
{
  int count;
  Arg args[MAXARGS];
  XFontStruct *font;
  int max_width, text_width;
  register char *sptr, *str;
  char * string;

  if (!widget) return 0;

  count = 0;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(widget, args, count);

  if (!istring || *istring == '\0') return (FONTWIDTH(font));

  string = strdup (istring);

  max_width = 0;
  sptr = str = string;
  while (str && *str != '\0') {
    if (*str == '\n') {
      *str = '\0';
      text_width = XTextWidth(font, sptr, strlen(sptr));
      if (text_width > max_width) max_width = text_width;
      *str = '\n';
      sptr = ++str;
    } else str++;
  }

  if (sptr) {
    text_width = XTextWidth(font, sptr, strlen(sptr));
    if (text_width > max_width) max_width = text_width;
  }
  free(string);
  return (max_width + FONTWIDTH(font));
}
      

static int GetTextHeight(widget, string)
     Widget widget;
     char *string;
{
  int count;
  Arg args[MAXARGS];
  XFontStruct *font;
  int lines;
  register char *str;
  
  if (!widget) return 0;

  count = 0;
  XtSetArg(args[count], XtNfont, &font); count++;
  XtGetValues(widget, args, count);

  if (!string || *string == '\0') return (FONTHEIGHT(font) + font->ascent);

  lines = 1;
  str = string;
  while (str && *str != '\0') {
    if (*str == '\n') lines++;
    str++;
  }

  return ((FONTHEIGHT(font) * lines) + font->ascent);
}

/*ARGSUSED*/
static bool ConvSel(w, selection, target, type, value, length, format)
     Widget w;
     Atom *selection, *target, *type;
     XtPointer *value;
     unsigned long *length;
     int *format;
{
  if (XmuConvertStandardSelection(w, selection, target, 
				  type, value, length, format))
    return TRUE;

  if (*target == XA_STRING) {
    *type = XA_STRING;
    *value = strdup(curr_selection);
    *length = strlen(*value);
    *format = 8;
    return TRUE;
  } else
    return FALSE;
}

/*ARGSUSED*/
static void CutString(w, closure, calldata)
     Widget w;
     XtPointer closure, calldata;
{
  Time xtime = XtLastTimestampProcessed(dpy);
  char *string = (char *) closure;
  
  (void) strcpy(curr_selection, "@");
  (void) strcat(curr_selection, string);
  XtOwnSelection(w, XA_PRIMARY, xtime, ConvSel, NULL, NULL);
}
		 
static void PopupMessage(shell, refto, label_widget, label, grab_kind)
     Widget refto, shell, label_widget;
     char *label;
     XtGrabKind grab_kind;
{
  int count;
  Arg args[MAXARGS];
  Dimension width, height;
  Pixmap border_pixmap;
  GC gc;
  char dash_list[2];
  Position x, y;

  dash_list[0] = dash_list[1] = 1;

  if (label != (String) NULL) {
    width = GetTextWidth(label_widget, label) + 8;
    height = GetTextHeight(label_widget, label) + 8;

    count = 0;
    XtSetArg(args[count], XtNlabel, label); count++;
    XtSetValues(label_widget, args, count);

    XtMakeResizeRequest(shell, width, height, NULL, NULL);
    XtResizeWidget(label_widget, width, height, (Dimension) 1);
  } else {
    count = 0;
    XtSetArg(args[count], XtNwidth, &width); count++;
    XtSetArg(args[count], XtNheight, &height); count++;
    XtGetValues(label_widget, args, count);
  }

  if (refto == (Widget) 0 || !XtIsRealized(refto)) XtMoveWidget(shell, 0, 0);
  else {
    XtTranslateCoords(refto, -20, 20, &x, &y);
    
    if (x < 0) x = 0;

    XtMoveWidget(shell, x, y);
  }

  XtPopup(shell, grab_kind);
  XRaiseWindow(dpy, XtWindow(shell));

  border_pixmap = XCreatePixmap(dpy, XtWindow(label_widget),
				width, height, DefaultDepthOfScreen(screen));

  gc = XCreateGC(dpy, XtWindow(label_widget), 0, NULL);

  XSetFunction(dpy, gc, GXcopy);
  
  XSetBackground(dpy, gc, WhitePixelOfScreen(screen));
  XSetForeground(dpy, gc, WhitePixelOfScreen(screen));
  
  XFillRectangle(dpy, border_pixmap, gc, 0, 0, 
		 (unsigned int) width, (unsigned int) height);
  
  XSetForeground(dpy, gc, BlackPixelOfScreen(screen));

  XSetDashes(dpy, gc, 0, dash_list, 2);
  XSetLineAttributes(dpy, gc, 0, LineOnOffDash, CapButt, JoinBevel);

  XDrawLine(dpy, border_pixmap, gc, 0, 0, width - 1, 0);
  XDrawLine(dpy, border_pixmap, gc, 1, 1, width - 2, 1);
  XDrawLine(dpy, border_pixmap, gc, 2, 2, width - 3, 2);
  XDrawLine(dpy, border_pixmap, gc, 3, 3, width - 4, 3);

  XDrawLine(dpy, border_pixmap, gc, 0, 0, 0, height - 1);
  XDrawLine(dpy, border_pixmap, gc, 1, 1, 1, height - 2);
  XDrawLine(dpy, border_pixmap, gc, 2, 2, 2, height - 3);
  XDrawLine(dpy, border_pixmap, gc, 3, 3, 3, height - 4);

  XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinBevel);

  XDrawLine(dpy, border_pixmap, gc, 0, height - 1, width - 1, height - 1);
  XDrawLine(dpy, border_pixmap, gc, 1, height - 2, width - 2, height - 2);
  XDrawLine(dpy, border_pixmap, gc, 2, height - 3, width - 3, height - 3);
  XDrawLine(dpy, border_pixmap, gc, 3, height - 4, width - 4, height - 4);

  XDrawLine(dpy, border_pixmap, gc, width - 1, 0, width - 1, height - 1);
  XDrawLine(dpy, border_pixmap, gc, width - 2, 1, width - 2, height - 2);
  XDrawLine(dpy, border_pixmap, gc, width - 3, 2, width - 3, height - 3);
  XDrawLine(dpy, border_pixmap, gc, width - 4, 3, width - 4, height - 4);

  count = 0;
  XtSetArg(args[count], XtNbackgroundPixmap, border_pixmap); count++;
  XtSetValues(label_widget, args, count);
}


