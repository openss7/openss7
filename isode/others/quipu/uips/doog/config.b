%{
/* $Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/config.b,v 9.0 1992/06/16 12:45:18 isode Rel $ */

/*
 * $Log: config.b,v $
 * Revision 9.0  1992/06/16  12:45:18  isode
 * Release 8.0
 *
 * Revision 8.4  1992/04/03  08:20:14  isode
 * sync
 *
 * Revision 8.3  1992/02/26  13:41:43  isode
 * rel7_2
 *
 * Revision 8.2  1992/02/05  10:05:00  isode
 * "Release
 *
 * Revision 8.1  1991/12/11  09:09:08  isode
 * AIX
 *
 * Revision 8.0  91/07/17  13:21:28  isode
 * Release 7.0
 * 
 *
 */

#include "types.h"
#include "sequence.h"
#include "util.h"
#include "initialize.h"
#include "query.h"

#include "quipu/util.h"

#ifdef _AIX
#pragma alloca
#endif

#define yyparse parseConfig
static int yylex();

void add_ufn_path_element();

extern FILE *config_file;
extern QCardinal config_line_num;
extern char config_file_name[];

static char lexeme[255];

void exit();

%}
%start configure

%union
{
  int number;
  QCardinal symbol;
  char strval[255];
  entryList dnlist;
}

%pure_parser

%token SEARCHPATH
%token <symbol> SEARCHPATH
%token <number> NUMBER
%token <strval> STRING
%token <symbol> ':' '(' ')' ','
%type <dnlist> entry_list
%type <number> lower_bound upper_bound

%%
configure:	option_list
  		;

option_list:	option option_list
		|
  		;

option:	SEARCHPATH ':' lower_bound ':' '(' entry_list ')'
	  {
	    add_ufn_path_element($3, UfnNobound, $6);
	  }

     	| SEARCHPATH ':' lower_bound ',' upper_bound ':' '(' entry_list ')'
	  {
	    add_ufn_path_element($3, $5, $8);
	  }

	| SEARCHPATH ':' lower_bound ',' '+' ':' '(' entry_list ')'
          {
	    add_ufn_path_element($3, UfnPlus, $8);
	  }
  	;

lower_bound:	NUMBER 	{$$ = $1;}
		;

upper_bound:	NUMBER 	{$$ = $1;}
		;

entry_list:	STRING ',' entry_list
		  {
		    $$ = $3;
		    dn_list_insert($1, &$$, NULLAttrT);
		  }
                | STRING
		  {
		    $$ = NULLEntryList;
		    dn_list_insert($1, &$$, NULLAttrT);
		  }
  		;
	
%%

static int yylex(lvalp, llocp)
     YYSTYPE *lvalp;
     YYLTYPE *llocp;
{
  register int c;
  QCardinal count = 0;
  FILE *lexfile = config_file;

  lexeme[0] = '\0';

  while (isspace(c = getc(lexfile)))
    {
      if (c == EOF) return 0;
      if (c == '\n') config_line_num++;
    }
  
  lexeme[count++] = c;

  switch (c)
    {
    case '#':
      while ((c = getc(lexfile)) != '\n' && c != EOF)
	;
      
      config_line_num++;
	
      return yylex(lvalp, llocp);
      
    case ':':
    case '(':
    case ')':
    case ',':
    case '+':
      lexeme[count] = '\0';
      return c;
      
    case '\"':
      count = 0;
      
      c = getc(lexfile);

      while (c != EOF && c != '\"')
	{
	  lexeme[count++] = (char) c;
	  c = getc(lexfile);
	}
      
      if (c == '\"' || c == '\0')
	{
	  lexeme[count] = '\0';
	  (void) strcpy(lvalp->strval, lexeme);

	  return STRING;
	}
      
      break;
    }

  while ((c = getc(lexfile)) != EOF &&
	 !(c == '('
	   || c == ')'
	   || c == ':'
	   || c == ','
	   || c == '\"'
	   || isspace((char) c)))
    lexeme[count++] = (char) c;

  if (c == EOF) return 0;

  (void) fseek(lexfile, (long) -1, 1);

  lexeme[count] = '\0';

  switch (lexeme[0])
    {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      lvalp->number = (int) atoi(lexeme);
      return NUMBER;
      
    case 's':
    case 'S':
      if (!lexequ(lexeme, "searchpath"))
	{
	  lvalp->symbol = SEARCHPATH;
	  return SEARCHPATH;
	}

      break;
    }
  
  return 0;
}

/* ARGSUSED */
int yyerror(err)
     char *err;
{
  (void) fprintf(stderr, "Line %d. ", config_line_num);
  (void) fprintf(stderr,
		 "Parse error in file `%s' at or near `%s'.\n",
		 config_file_name,
		 lexeme); 
  exit(1);
}
  
