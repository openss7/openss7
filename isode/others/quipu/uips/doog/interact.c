#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/interact.c,v 9.0 1992/06/16 12:45:18 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/interact.c,v 9.0 1992/06/16 12:45:18 isode Rel $ */

/*
 * $Log: interact.c,v $
 * Revision 9.0  1992/06/16  12:45:18  isode
 * Release 8.0
 *
 */

#include "general.h"
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "quipu/util.h"

#include "query.h"
#include "util.h"
#include "interact.h"
#include "initialize.h"

void ufnsearch();
void quitfn();
void readentry();
void looklist();

void ufnresolve();
void printcommands(), callcommand();
QBool query_matches();

void print_entry_list();
void print_read_results();
void new_current_list();

void nullfn(), abort_query(), intquit(), abort_command();

static void uprint();
static char message[LINESIZE];

void exit();

entryList currentlist = NULLEntryList;
QCardinal currlistsize;
QCardinal request_id = 0;
QCardinal requests_made = 0, requests_failed = 0;

jmp_buf env;

void interact()
{
  char commandline[LINESIZE];
  
  (void) setjmp(env);
  (void) signal(SIGINT, intquit);

  for (;;)
    {
      uprint(":- ");
      
      if (gets(commandline) == NULL) quitfn("");
      callcommand(commandline);
      
      (void) signal(SIGINT, intquit);
    }
}

void intquit()
{
  char input[LINESIZE];

  while (1)
    {
      uprint("\nReally exit? [y/n] - ");
      if (gets(input) == NULL) quitfn("");
      
      if ((int)strlen(input) > 1)
	{
	  uprint("y or n only! - ");
	  continue;
	}
      else if (*input == 'y')
	quitfn(NULLCP);
      else if (*input == 'n')
	longjmp(env, 0); 
    }
}

/* ARGSUSED */
void quitfn(params)
     char *params;
{
  uprint("\nOK, exiting.\n");
  exit(0);
}

void ufnsearch(params)
     char *params;
{
  char *str;

  (void) signal(SIGINT, abort_command);

  /* Zero current list */
  dn_list_free(&currentlist);
  currlistsize = 0;

  /* Zero task and error counts */
  requests_made = requests_failed = 0;

  if (*params == '\0')
    {
      uprint("You haven't entered a name to search on!\n");
      return;
    }
  
  /* Count number of components in user friendly name */
  str = params;
  for (;;)
    {
      while (*str != ',' && !isnull(*str))
	str++;

      if (isnull(*str))
	break;

      str++;
    }
  
  ufnresolve(params, NULLEntryList, unknown);
  
  if (requests_failed > 0)
    {
      (void) sprintf(message,
		     "%d requests were sent of which %d failed.\n",
		     requests_made,
		     requests_failed);
      uprint(message);
    }
  
  switch (currlistsize)
    {
    case 0:
      uprint("\nFailed to find anything!\n");
      break;
      
    case 1:
    readentry("1");
      break;
      
      default:
      (void) sprintf(message, "\nFound %d good matches.\n", currlistsize);
      uprint(message);

      print_entry_list(currentlist);
      break;
    }
}

/*
 * - ufnresolve() -
 *
 * Recursive procedure that calls do_ufn_resolve.
 *
 * If partial match is made with poor matches, then query user and
 * continue search if good matches identified.
 *
 */
void ufnresolve(name, baseobjects, is_leaf)
     char *name;
     entryList baseobjects;
     known is_leaf;
{
  ufnMatchState match_state;
  namePart ufname;
  ufnResults results;
  fd_set association, writefds, exceptfds;
  int assoc_des;
  entryList baselist = NULLEntryList, matches;
  QCardinal *request_list;

  ufname = str2ufname(name);

  baselist = baseobjects;

  if (do_ufn_resolve(baselist, ufname, is_leaf, &request_id) == QERR_ok)
    {
      request_state request_status = RQ_processing;
      (void) signal(SIGINT, abort_query);

      do
	{
	  do
	    {
	      do
		{
		  assoc_des = get_association_descriptor(request_id);
		  FD_ZERO(&association);
		  FD_ZERO(&writefds);
		  FD_ZERO(&exceptfds);
		  FD_SET(assoc_des, &association);
		}
	      while (select(getdtablesize(),
			    &association,
			    &writefds,
			    &exceptfds,
			    (struct timeval *) NULL) <= 0);
	    }
	  while (directory_wait(&request_list) == 0);
	  
	  (void) signal(SIGINT, abort_command);
	  
	  results = get_ufn_results(request_id);
	  free((char *) request_list);
	  
	  requests_made += results->tasks_sent;
	  requests_failed += results->tasks_failed;
	  
	  match_state = results->match_status;
	  
	  if (baseobjects == NULLEntryList)
	    dn_list_free(&baselist);
	  
	  request_status = RQ_results_returned;

	  switch (match_state)
	    {
	    case Failed:
	      break;
	      
	    case GoodMatches:
	      new_current_list(results);
	      results->matches = NULLEntryList;
	      
	      break;
      
	    case PoorComplete:
	      (void) sprintf(message,
			     "Made poor matches for `%s'.\n",
			     results->resolved_part);
	      
	      uprint(message);
	      uprint("Please indicate which ones you wish to follow up.\n");
	  
	      if (query_matches(results->matches, &matches) == FALSE)
		;
	      else
		{
		  if (matches != NULLEntryList)
		    {
		      dn_list_free(&results->matches);
		      
		      results->matches = matches;
		      results->match_num = 0;
		      
		      while (matches != NULLEntryList)
			{
			  results->match_num++;
			  matches = matches->next;
			}
		      
		      new_current_list(results);
		      results->matches = NULLEntryList;
		    }
		  else
		    {
		      ufname_result_free(&results);
		      request_status = continue_ufn_search(matches,request_id);
		    }
		}
	      
	      break;
	      
	    case PoorPartial:
	      (void) sprintf(message,
			     "Made poor matches for `%s'.\n",
			     results->resolved_part);
	      
	      uprint(message);
	      uprint("Please indicate which ones you wish to follow up.\n");
	      
	      if (query_matches(results->matches, &matches) == FALSE)
		break;
	      else
		{
		  ufname_result_free(&results);
		  request_status = continue_ufn_search(matches, request_id);
		}
	      
	      break;
	    }
	}
      while (request_status == RQ_processing);
    }
  else
    {
      match_state = Failed;
      results = NULLUfnResults;
    }

  ufname_result_free(&results);
  _request_complete(request_id);
}

void readentry(params)
     char *params;
{
  fd_set association, writefds, exceptfds;
  int assoc_des;
  int entry_num = 0;
  stringCell attrs_to_read = NULLStrCell;
  entryList entries;
  QCardinal count;
  readResults results;
  QCardinal *request_list;

  (void) signal(SIGINT, abort_command);

  if (currentlist == NULLEntryList)
    {
      uprint("No current entry list!\n");
      return;
    }
  
  if (!isdigit(*params))
    {
      uprint("Invalid command syntax! Need `read <number>'.\n");
      return;
    }
  
  entry_num = atoi(params);
  
  if (entry_num < 1 || entry_num > currlistsize)
    {
      uprint("Entry number out of range!\n");
      return;
    }

  add_string_to_seq("2.5.4.3", &attrs_to_read);
  add_string_to_seq("2.5.4.4", &attrs_to_read);
  add_string_to_seq("2.5.4.6", &attrs_to_read);
  add_string_to_seq("2.5.4.10", &attrs_to_read);
  add_string_to_seq("2.5.4.11", &attrs_to_read);
  add_string_to_seq("0.9.2342.19200300.100.1.3", &attrs_to_read);
  add_string_to_seq("0.9.2342.19200300.99.1.8", &attrs_to_read);
  add_string_to_seq("0.9.2342.19200300.100.1.22", &attrs_to_read);
  add_string_to_seq("2.5.4.16", &attrs_to_read);
  add_string_to_seq("2.5.4.17", &attrs_to_read);
  add_string_to_seq("2.5.4.20", &attrs_to_read);

  for (count = 1, entries = currentlist; 
       count < entry_num && entries != NULLEntryList; 
       count++, entries = entries->next) 
    ;
    
  if (entries != NULLEntryList)
    {
      if (do_read(entries->string_dn, &request_id, attrs_to_read) != QERR_ok) {
	uprint("Read failed!\n");
	return;
      }
    }
  else
    {
      uprint("Invalid entry number!\n");
      return;
    }
  
  (void) signal(SIGINT, abort_query);
  
  do
    {
      do
	{
	  assoc_des = get_association_descriptor(request_id);
	  FD_ZERO(&association);
	  FD_ZERO(&writefds);
	  FD_ZERO(&exceptfds);
	  FD_SET(assoc_des, &association);
	}
      while (select(getdtablesize(),
		    &association,
		    &writefds,
		    &exceptfds,
		    (struct timeval *) NULL) <= 0);
    }
  while (directory_wait(&request_list) == 0);

  (void) signal(SIGINT, abort_command);
  results = get_read_results(request_id);
  
  free((char *) request_list);
  
  print_read_results(results, entries->string_dn);
  read_result_free(&results);
}

void print_read_results(results, baseobject)
     readResults results;
     char *baseobject;
{
  stringCell val_list;
  attrValList entry_attrs;
  int error_count = 0;
  errorList errors;
  char fname[LINESIZE];

  errors = results->errors;

  for (errors = results->errors; errors != NULLError; errors = errors->next)
    error_count++;

  if (results->entry == NULLAVList && error_count > 0)
    {
      uprint("Cannot perform read!\n");
      return;
    }
 
  friendlify(baseobject, fname);

  (void) sprintf(message, "Read `%s'.\n", fname);
  uprint(message);

  if (error_count > 0)
    {
      (void) sprintf(message, "Errors encountered: %d.\n\n", error_count);
      uprint(message);
    }

  for (entry_attrs = results->entry;
       entry_attrs != NULLAVList;
       entry_attrs = entry_attrs->next)
    {
      for (val_list = entry_attrs->val_list;
	   val_list != NULLStrCell;
	   val_list = val_list->next)
	{
	  (void) sprintf(message,
			 " %-21s- %s\n",
			 entry_attrs->attr_name->string,
			 val_list->string); 
	  uprint(message);
	}
    }
  
  uprint("\n");
}

void print_entry_list(entries)
     entryList entries;
{
  int line_count = 0, entry_count = 0;
  char fname[LINESIZE];

  if (entries != NULLEntryList)
    {
      for (; entries != NULLEntryList; entries = entries->next)
	{
	  entry_count++;
	  
	  if (line_count >= 15)
	    {
	      uprint("< ** Press return to continue ** >");
	      (void) fgetc(stdin);
	      line_count = 0;
	    }

	  friendlify(entries->string_dn, fname);
	  
	  (void) sprintf(message, " %d %s\n", entry_count, fname);

	  if ((int)strlen(message) > 80)
	    line_count += 2;
	  else
	    line_count++;

	  uprint(message);
	}
      
      uprint("\n");
    }
}

/* ARGSUSED */
void printcommands(params)
     char *params;
{
  char buffer[LINESIZE];

  (void) signal(SIGINT, abort_command);

  uprint("* Commands *\n\n");

  (void) strcpy(buffer, "quit, q\t\t\tQuit this application.\n\n");
  uprint(buffer);

  (void) strcpy(buffer, "<name>, find <name>\tSearch for the named object,\n");
  (void) strcat(buffer,"\t\t\tfor example ");
  (void) strcat(buffer, "`damanjit, manufacturing, brunel, gb'.\n");
  uprint(buffer);

  (void) strcpy(buffer,	"curr\t\t\tLook at the list of entries returned");
  (void) strcat(buffer, "by the last search.\n");
  (void) strcat(buffer, "This is referred to as the current list\n\n");
  uprint(buffer);

  uprint("<number>\t\tView numbered entry in current list.\n\n");

  uprint("help, ?\t\t\tView commands.\n");
}

void callcommand(commandline)
     char *commandline;
{
  char *params;

  if (*commandline == '\0') return;

  for (; isspace(*commandline); commandline++)
    ;
  for (params = commandline; isspace(*params) && !isnull(*params); params++)
    ;
  for (; !isspace(*params) && !isnull(*params); params++)
    ;
  for (; isspace(*params) && !isnull(*params); params++)
    ;

  if ((strncmp(commandline, "quit", 4) == 0 && !isalnum(commandline[4])) ||
      (*commandline == 'q' && strlen(commandline) == 1))
    quitfn(params);
  else if (strncmp(commandline, "find", 4) == 0 && isspace(commandline[4]))
    ufnsearch(params);
  else if (strncmp(commandline, "curr", 4) == 0 && !isalnum(commandline[4]))
    looklist(params);
  else if (isdigit(*commandline))
    readentry(commandline);
  else if (*commandline == '?' ||
	   (strncmp(commandline, "help", 4) == 0 && !isalnum(commandline[4])))
    printcommands(params);
  else
    ufnsearch(commandline);
}

void new_current_list(results)
     ufnResults results;
{
  if (currentlist != NULLEntryList) dn_list_free(&currentlist);

  currlistsize = results->match_num;
  currentlist = results->matches;
  
  results->matches = NULLEntryList;
}

/* ARGSUSED */
void looklist(params)
     char *params;
{
  (void) signal(SIGINT, abort_command);

  if (currentlist == NULLEntryList)
    {
      uprint("No current list at present!\n");
      return;
    }
  
  uprint("\n");
  print_entry_list(currentlist);
}

QBool query_matches(matches, returnlist)
     entryList matches;
     entryList *returnlist;
{
  entryList good_matches = NULLEntryList, curr_poor_match;
  char fname[LINESIZE], input[LINESIZE];

  uprint("\n");

  for (curr_poor_match = matches;
       curr_poor_match != NULLEntryList;
       curr_poor_match = curr_poor_match->next)
    {
      friendlify(curr_poor_match->string_dn, fname);
      
      (void) sprintf(message, "`%s'? [y/n/q]\n:- ", fname); 
      uprint(message);
      
      while (1)
	{
	  if (gets(input) == NULL) quitfn("");
	  if ((int)strlen(input) > 1)
	    {
	      uprint("y, n or q only!\n:- ");
	      continue;
	    }
	  else if (*input == 'y' || *input == 'Y')
	    {
	      (void) dn_list_add(curr_poor_match->string_dn,
				 &good_matches,
				 NULLAttrT);
	      break;
	    }
	  else if (*input == 'q' || *input == 'Q')
	    {
	      if (good_matches != NULLEntryList) dn_list_free(&good_matches);
	      return FALSE;
	    }
	  else if (*input == 'n' || *input == 'N')
	    {
	      break;
	    }
	  else
	    uprint("y, n or q only!\n:- ");
	}
      
      *input = '\0';
    }
  
  *returnlist = good_matches;
  return TRUE;
}

void nullfn()
{
}

void abort_query()
{
  uprint("\nQuery interrupted.\n");
  abort_request(request_id);
  longjmp(env, 0);
}

void abort_command()
{
  uprint("\nCommand interrupted.\n");
  longjmp(env, 0);
}
  

static void uprint(printstring)
     char *printstring;
{
  (void) fprintf(stderr, "%s", printstring);
}
