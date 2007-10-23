# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
#ifndef	lint
static char *RCSid = "$Header: /xtel/isode/isode/pepy/RCS/lex.l.gnrc,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/pepy/RCS/lex.l.gnrc,v 9.0 1992/06/16 12:25:01 isode Rel $
 *
 *
 * $Log: lex.l.gnrc,v $
 * Revision 9.0  1992/06/16  12:25:01  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 *
 */


struct table {
    char   *t_keyword;
    int	    t_value;
    int	    t_porting;
};

static struct table reserved[] = {
    "ABSENT", ABSENT, 0,
    "ANY", ANY, 0,
    "APPLICATION", APPLICATION, 0,
    "BEGIN", BGIN, 0,
    "BIT", BIT, 0,
    "BITSTRING", BITSTRING, 0,
    "BOOLEAN", BOOLEAN, 0,
    "BY", BY, 0,
    "CHOICE", CHOICE, 0,
    "COMPONENT", COMPONENT, 0,
    "COMPONENTS", COMPONENTS, 0,
    "COMPONENTSOF", COMPONENTSOF, 0,
    "DECODER", DECODER, 0,
    "DEFAULT", DEFAULT, 0,
    "DEFINED", DEFINED, 0,
    "DEFINITIONS", DEFINITIONS, 0,
    "ENCODER", ENCODER, 0,
    "ENCRYPTED", ENCRYPTED, 0,
    "END", END, 0,
    "ENUMERATED", ENUMERATED, 0,
    "EXPLICIT", EXPLICIT, 0,
    "EXPORTS", EXPORTS, 0,
    "FALSE", L_FALSE, 0,
    "FROM", FROM, 0,
    "IDENTIFIER", IDENTIFIER, 0,
    "IMPLICIT", IMPLICIT, 0,
    "IMPORTS", IMPORTS, 0,
    "INCLUDE", INCLUDES, 0,
    "INTEGER", INTEGER, 0,
    "MIN", MIN, 0,
    "MAX", MAX, 0,
    "NULL", NIL, 0,
    "OBJECT", OBJECT, 0,
    "OCTET", OCTET, 0,
    "OCTETSTRING", OCTETSTRING, 0,
    "OF", OF, 0,
    "OPTIONAL", OPTIONAL, 0,
    "PREFIXES", PREFIXES, 0,
    "PRESENT", PRESENT, 0,
    "PRINTER", PRINTER, 0,
    "PRIVATE", PRIVATE, 0,
    "REAL", REAL, 0,
    "SECTIONS", SECTIONS, 0,
    "SEQUENCE", SEQUENCE, 0,
    "SEQUENCEOF", SEQUENCEOF, 0,
    "SET", SET, 0,
    "SETOF", SETOF, 0,
    "SIZE", SIZE, 0,
    "STRING", STRING, 0,
    "TAGS", TAGS, 0,
    "TRUE", L_TRUE, 0,
    "UNIVERSAL", UNIVERSAL, 0,
    "WITH", WITH, 0,
    "PLUS-INFINITY", PLUSINFINITY, 0,
    "MINUS-INFINITY", MINUSINFINITY, 0,
    NULL, 0
};
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
		{   register int c, d;

			    for (d = 0; c = input (); d = c == '-')
				if (c == '\n' || (d && c == '-'))
				    break;
			}
break;
case 2:
		{
			    if (yydebug)
				fprintf (stderr, "WT\n");
			}
break;
case 3:
		{
			    if (yydebug)
				fprintf (stderr, "NL\n");
			}
break;
case 4:
		{
			    if (yydebug)
				fprintf (stderr, "SY: CCE\n");
			    return CCE;
			}
break;
case 5:
		{
			    if (yydebug)
			    	fprintf (stderr, "SY: DOTDOTDOT\n");
			    return DOTDOTDOT;
			}
break;
case 6:
		{
    			    if (yydebug)
				fprintf (stderr, "SY: DOTDOT\n");
			    return DOTDOT;
			}
break;
case 7:
		{
			    if (yydebug)
				fprintf (stderr, "SY: DOT\n");
			    return DOT;
			}
break;
case 8:
		{
			    if (yydebug)
				fprintf (stderr, "SY: SEMICOLON");
			    return SEMICOLON;
			}
break;
case 9:
		{
			    if (yydebug)
				fprintf (stderr, "SY: COMMA\n");
			    return COMMA;
			}
break;
case 10:
		{
			    if (yydebug)
				fprintf (stderr, "SY: LBRACE\n");
			    return LBRACE;
			}
break;
case 11:
		{
			    if (yydebug)
				fprintf (stderr, "SY: RBRACE\n");
			    return RBRACE;
			}
break;
case 12:
		{
			    if (yydebug)
				fprintf (stderr, "SY: BAR\n");
			    return BAR;
			}
break;
case 13:
	{   register int tok, c, d, len;
			    register char *cp, *ep, *pp;

			    if (*yytext == '$')
				tok = VLENGTH;
			    else
				if (*yytext == '<')
				    tok = CONTROL;
				else {
				    while((c = input()) == ' ' || c =='\t')
				        continue;
				    switch (c) {
					case 'a': tok = VALA;
						  break;
					case 'b': tok = VALB;
						  break;
					case 'i': tok = VALI;
						  break;
					case 's': tok = VALS;
						  break;
					case 'o': tok = VALO;
						  break;
					case 'x': tok = VALX;
						  break;
					case 'p': tok = VALP;
						  break;
					case 'q': tok = VALQ;
					    	  break;
					case 'r': tok = VALR;
						  break;
					case 'O': tok = VALOID;
						  break;
					case 'P': tok = PARAMETERTYPE;
					    	  break;
					default : myyerror ("unknown token: \"%s\"", yytext);
						  break;
				    }
				    if ((c = input()) != ' ' && c != '\t'
					&& c != '\n')
					yyerror ("syntax error in [[ ... ]]");
				}
 
			    if ((pp = malloc ((unsigned) (len = BUFSIZ)))
				    == NULL)
				yyerror ("out of memory");

			    for (ep = (cp = pp) + len - 1, d = NULL;; d = c) {
				if ((c = input ()) == NULL)
				    yyerror ("end-of-file while reading value");
				if ((d == ']' && c == ']' && tok !=CONTROL) ||
				    (c == '$' && (tok ==VALX || tok ==VALO)) ||
				    (d == '>' && c == '>' && tok ==CONTROL)) {
				    if ((tok == VALX || tok == VALO) &&
				       (c != '$'))
				       yyerror("Missing '$' in [[ - ]]");
				    if (c == '$') {unput(c); *cp = NULL;}
				    else *--cp = NULL;
				    yylval.yy_string = pp;
				    if (yydebug)
					fprintf (stderr, "VAL: \"%s\"\n",
						yylval.yy_string);
				    return tok;
				}
				if (cp >= ep) {
				    register int curlen = cp - pp;
				    register char *dp;

				    if ((dp = realloc (pp,
						(unsigned) (len += BUFSIZ)))
					    == NULL)
					yyerror ("out of memory");
				    cp = dp + curlen;
				    ep = (pp = dp) + len - 1;
				}
				*cp++ = c;
			    }
			}
break;
case 14:
		{
			    if (yydebug)
				fprintf (stderr, "SY: LBRACKET\n");
			    return LBRACKET;
			}
break;
case 15:
		{
			    if (yydebug)
				fprintf (stderr, "SY: RBRACKET\n");
			    return RBRACKET;
			}
break;
case 16:
		{
			    if (yydebug)
				fprintf (stderr, "SY: LANGLE\n");
			    return LANGLE;
			}
break;
case 17:
		{
			    if (yydebug)
				fprintf (stderr, "SY: LPAREN\n");
			    return LPAREN;
			}
break;
case 18:
		{
			    if (yydebug)
				fprintf (stderr, "SY: RPAREN\n");
			    return RPAREN;
			}
break;
case 19:
		{
			    (void) sscanf (yytext, "%d", &yylval.yy_number);
			    if (yydebug)
				fprintf (stderr, "LIT: 0x%x\n", yylval.yy_number);
			    return LITNUMBER;
			}
break;
case 20:
		{
			    (void) sscanf (yytext, "%d", &yylval.yy_number);
			    if (yydebug)
				fprintf (stderr, "LIT: 0x%x\n", yylval.yy_number);
			    return LITNUMBER;
			}
break;
case 21:
	{   register char *cp; register int i;

			    switch (*(cp = yytext + strlen (yytext) - 1)) {
				case 'H':
				case 'h':
				    *cp = NULL;
				    (void) sscanf (yytext + 1, "%x",
						&yylval.yy_number);
				    break;

				case 'B':
				case 'b':
				    *cp-- = NULL, *cp = NULL;
				    for (i = 0, cp = yytext + 1; *cp; ) {
					i <<= 1;
					i += *cp++ - '0';
				    }
				    yylval.yy_number = i;
				    break; 
			    }
			    if (yydebug)
				fprintf (stderr, "LIT: 0x%x\n", yylval.yy_number);
			    return LITNUMBER;
			}
break;
case 22:
		{
			    int	    c, len;
			    register char *cp, *ep, *pp;

			    if ((pp = malloc ((unsigned) (len = BUFSIZ)))
				    == NULL)
				yyerror ("out of memory");

			    for (ep = (cp = pp) + len - 1;;) {
				if ((c = input ()) == NULL)
				    yyerror ("end-of-file while reading string");
				if (c == '"')
				    break;

				if (cp >= ep) {
				    register int curlen = cp - pp;
				    register char *dp;

				    if ((dp = realloc (pp,
						       (unsigned) (len += BUFSIZ)))
					    == NULL)
					yyerror ("out of memory");
				    cp = dp + curlen;
				    ep = (pp = dp) + len - 1;
				}
				*cp++ = c;
			    }
			    *cp = NULL;
			    yylval.yy_string = pp;
			    if (yydebug)
				fprintf (stderr, "LIT: \"%s\"\n",
					 yylval.yy_string);
			    return LITSTRING;
			}
break;
case 23:
{   register struct table *t;

			    for (t = reserved; t -> t_keyword; t++)
				if (strcmp (t -> t_keyword, yytext) == 0) {
				    if (yyporting && t -> t_porting)
					break;
				    if (yydebug)
					fprintf (stderr,
						  "KE: \"%s\"\n", yytext);
				    return t -> t_value;
				}
			    yylval.yy_string = new_string (yytext);
			    if (yydebug)
				fprintf (stderr, "ID: \"%s\"\n", yylval.yy_string);
			    return ID;
			}
break;
case 24:
{   yylval.yy_string = new_string (yytext);
			    if (yydebug)
				fprintf (stderr, "NAME: \"%s\"\n", yylval.yy_string);
			    return NAME;
			}
break;
case 25:
		{   register int c, d, len;
			    register char *cp, *ep, *pp;

			    if ((pp = malloc ((unsigned) (len = BUFSIZ)))
				    == NULL)
				yyerror ("out of memory");

			    for (ep = (cp = pp) + len - 1, d = NULL;; d = c) {
				if ((c = input ()) == NULL)
				    yyerror ("end-of-file while reading value");
				if (d == '%' && c == ']' ) {
				    *--cp = NULL;
				    yylval.yy_string = pp;
				    if (yydebug)
					fprintf (stderr, "VAL: \"%s\"\n",
						 yylval.yy_string);
				    return SCTRL;
				}
				if (d == '\n')
				    yyerror ("newline in %[ %] construct");
				if (cp >= ep) {
				    register int curlen = cp - pp;
				    register char *dp;

				    if ((dp = realloc (pp,
						(unsigned) (len += BUFSIZ)))
					    == NULL)
					yyerror ("out of memory");
				    cp = dp + curlen;
				    ep = (pp = dp) + len - 1;
				}
				*cp++ = c;
			    }
			}
break;
case 26:
		{   register int c, d, len;
			    int	    mylineno;
			    register char *cp, *ep, *pp;

			    mylineno = yylineno;
			    if ((pp = malloc ((unsigned) (len = BUFSIZ)))
				    == NULL)
				yyerror ("out of memory");

			    for (ep = (cp = pp) + len - 1, d = NULL;; d = c) {
				if ((c = input ()) == NULL)
				    yyerror ("end-of-file while reading action");
				if (d == '%' && c == '}') {
				    *--cp = NULL;
				    yylval.yy_action = new_action (pp, mylineno);;
				    if (yydebug)
					fprintf (stderr, "ACTION: \"%s\", %d\n",
						yylval.yy_action -> ya_text,
						yylval.yy_action -> ya_lineno);
				    return ACTION;
				}
				if (cp >= ep) {
				    register int curlen = cp - pp;
				    register char *dp;

				    if ((dp = realloc (pp,
						(unsigned) (len += BUFSIZ)))
					    == NULL)
					yyerror ("out of memory");
				    cp = dp + curlen;
				    ep = (pp = dp) + len - 1;
				}
				*cp++ = c;
			    }
			}
break;
case 27:
		{   
			    myyerror ("unknown token: \"%s\"", yytext);
			}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] = {
0,

2,
0,

2,
0,

27,
0,

2,
27,
0,

3,
0,

22,
27,
0,

13,
27,
0,

27,
0,

27,
0,

17,
27,
0,

18,
27,
0,

9,
27,
0,

27,
0,

7,
27,
0,

19,
27,
0,

27,
0,

8,
27,
0,

16,
27,
0,

23,
27,
0,

14,
27,
0,

15,
27,
0,

24,
27,
0,

10,
27,
0,

12,
27,
0,

11,
27,
0,

2,
0,

25,
0,

26,
0,

1,
0,

20,
0,

6,
0,

19,
0,

13,
0,

23,
0,

24,
0,

21,
0,

5,
0,

4,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	4,26,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,6,	
4,26,	1,7,	1,8,	0,0,	
1,9,	1,10,	1,11,	0,0,	
9,29,	1,12,	1,13,	1,14,	
14,33,	1,15,	33,40,	29,0,	
9,29,	9,29,	29,30,	0,0,	
0,0,	0,0,	0,0,	1,16,	
1,17,	1,18,	16,35,	18,36,	
35,41,	0,0,	1,19,	1,19,	
0,0,	0,0,	0,0,	2,6,	
0,0,	0,0,	2,8,	0,0,	
2,9,	2,10,	2,11,	9,0,	
0,0,	2,12,	9,30,	2,14,	
0,0,	0,0,	0,0,	0,0,	
9,29,	0,0,	0,0,	9,29,	
1,20,	20,36,	1,21,	2,16,	
2,17,	2,18,	1,22,	1,22,	
8,27,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	13,31,	
9,29,	9,29,	13,32,	13,32,	
13,32,	13,32,	13,32,	13,32,	
13,32,	13,32,	13,32,	13,32,	
0,0,	30,39,	0,0,	0,0,	
1,23,	1,24,	1,25,	30,39,	
2,20,	0,0,	2,21,	0,0,	
8,28,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
9,29,	9,29,	15,34,	15,34,	
15,34,	15,34,	15,34,	15,34,	
15,34,	15,34,	15,34,	15,34,	
0,0,	30,39,	0,0,	0,0,	
0,0,	0,0,	0,0,	30,39,	
2,23,	2,24,	2,25,	19,37,	
0,0,	0,0,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	19,37,	19,37,	19,37,	
19,37,	22,38,	0,0,	0,0,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	22,38,	
22,38,	22,38,	22,38,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	32,32,	32,32,	32,32,	
32,32,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		yyvstop+1,
yycrank+-37,	yysvec+1,	yyvstop+3,
yycrank+0,	0,		yyvstop+5,
yycrank+4,	0,		yyvstop+7,
yycrank+0,	0,		yyvstop+10,
yycrank+0,	0,		yyvstop+12,
yycrank+0,	0,		yyvstop+15,
yycrank+9,	0,		yyvstop+18,
yycrank+-43,	0,		yyvstop+20,
yycrank+0,	0,		yyvstop+22,
yycrank+0,	0,		yyvstop+25,
yycrank+0,	0,		yyvstop+28,
yycrank+62,	0,		yyvstop+31,
yycrank+2,	0,		yyvstop+33,
yycrank+94,	0,		yyvstop+36,
yycrank+4,	0,		yyvstop+39,
yycrank+0,	0,		yyvstop+41,
yycrank+3,	0,		yyvstop+44,
yycrank+118,	0,		yyvstop+47,
yycrank+2,	0,		yyvstop+50,
yycrank+0,	0,		yyvstop+53,
yycrank+196,	0,		yyvstop+56,
yycrank+0,	0,		yyvstop+59,
yycrank+0,	0,		yyvstop+62,
yycrank+0,	0,		yyvstop+65,
yycrank+0,	yysvec+4,	yyvstop+68,
yycrank+0,	0,		yyvstop+70,
yycrank+0,	0,		yyvstop+72,
yycrank+-15,	yysvec+9,	0,	
yycrank+55,	0,		0,	
yycrank+0,	0,		yyvstop+74,
yycrank+271,	0,		yyvstop+76,
yycrank+4,	0,		yyvstop+78,
yycrank+0,	yysvec+15,	yyvstop+80,
yycrank+3,	0,		0,	
yycrank+0,	0,		yyvstop+82,
yycrank+0,	yysvec+19,	yyvstop+84,
yycrank+0,	yysvec+22,	yyvstop+86,
yycrank+0,	0,		yyvstop+88,
yycrank+0,	0,		yyvstop+90,
yycrank+0,	0,		yyvstop+92,
0,	0,	0};
struct yywork *yytop = yycrank+328;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,'$' ,01  ,01  ,'$' ,
01  ,01  ,01  ,01  ,01  ,'-' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'B' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'B' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
01  ,'a' ,'b' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'b' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
