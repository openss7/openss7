#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/calls.c,v 9.0 1992/06/16 12:44:54 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/calls.c,v 9.0 1992/06/16 12:44:54 isode Rel $
 *
 *
 * $Log: calls.c,v $
 * Revision 9.0  1992/06/16  12:44:54  isode
 * Release 8.0
 *
 */

#include "quipu/util.h"
#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/name.h"
#include "quipu/list.h"

#include "usr.dirent.h"
#include "tailor.h"
#include "sequence.h"
#include "filt.h"
#include "defs.h"
#include "util.h"

#include <X11/Intrinsic.h>

extern mailtype mailformat;
extern Attr_Sequence read_types, read_types2, oclass;
extern bool photo_on;

int typetoggled = 0;

int rdn_print(), as_print();
char *TidyString();
Attr_Sequence get_sorted_attrs();

void kill_message(), message();

void requestPassword();
void parse_duarc();

void set_default_type(), make_friendly();
void quit();

void put_dn_and_password();

str_seq dnseq = NULLDS, backseq = NULLDS, showseq = NULLDS;
str_seq alias_seq = NULLDS, aliased_dn_seq = NULLDS, password_seq = NULLDS;
int entry_number, back_buf_num, dn_number, histlimit = 50;
bool read_all_flag = TRUE;
bool testing = FALSE;

static void make_friendly_aux();

char            goto_path[STRINGLEN];         /* Used by the 'G:goto' command*/
char            base_path[STRINGLEN];         /* Used by all DS operations   */
char 		friendly_base_path[STRINGLEN];
char            namestr[STRINGLEN];
char            bindpass [STRINGLEN];
char            mvalue [STRINGLEN];
char   		passwd[STRINGLEN];

unsigned int curr_filt = 0;
unsigned int filt_num = 0;
unsigned int typeindx = 0;
filt_struct *filt_arr[MAXTYPES];
char	*filtvalue[MAXTYPES];
char    *filttype[MAXTYPES];

int default_num;
int *av_typeindx;
int *available_types[MAXTYPES];
char *levels[MAXTYPES];
int defaults[MAXTYPES];

typedef struct friendlyName {
  str_seq names;
  char fname[256];
} *fName;

unsigned int fname_num = 0;
fName name_map[MAXTYPES];

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

extern int sizelimit;
extern char *local_dit;

DN user_name;

char * addobj = NULLCP;
Filter search_filter;
FILE *config_file;
char *file_names[MAXTYPES];
char dua_help_dir[SMALLSTRING];

char *get_strioid(ptr)
     register char *ptr;
{
  register char *end_ptr;

  while(*ptr == '"') ptr++;
  while(*ptr != '"') ptr++;

  while(*ptr > '9' || *ptr < '0') ptr++;

  end_ptr = ptr;
  while(*end_ptr != '\n') end_ptr++;

  *end_ptr = '\0';
  return ptr;
}

void put_dn_and_password(dn, password, name)
     char *dn, *password, *name;
{
  str_seq alias_list = alias_seq;
  int seq_num;

  for (seq_num = 1; alias_list != NULLDS; 
       seq_num++, alias_list = alias_list->next)
    if (!lexequ(name, alias_list->dname)) break;
    
  if (alias_list == NULLDS) {
    (void) strcpy(dn, name);
    *password = '\0';
  } else {
    (void) strcpy(dn, get_from_seq(seq_num, aliased_dn_seq));

    if (get_from_seq(seq_num, password_seq) != NULLCP)
      (void) strcpy(password, get_from_seq(seq_num, password_seq));
    else
      *password = '\0';
  }
}

void user_tailor()
{
  char *part1, *part2;
  char *getenv(), *TidyString(), *SkipSpace();
  char *config_dir = "/.duaconfig",
       *type_dir = "/filterTypes",
       *readTypes = "/readTypes",
       *friendlyNames = "/friendlyNames",
       *options = "/.duarc",
       *typeDefaults = "/typeDefaults";

  char user_path[BUFSIZ], read_path[BUFSIZ], 
       type_path[BUFSIZ], options_path[BUFSIZ], 
       friendly_path[BUFSIZ], type_defaults_path[BUFSIZ],
       dua_config_dir[BUFSIZ];

  char stroid_buf[BUFSIZ];
  char Read_in_Stuff[STRINGLEN];

  extern  char * dsa_address,
  	       * myname;
  extern  char * tailfile;

  register char *str, *sptr, *p, *end;
  char save;

  DIR *config_directory;
  struct dirent  *dir_ent;

  int count, n, num;
  int tempints[MAXTYPES];

#ifndef NO_STATS
  ll_hdinit (log_stat,"pod");
#endif

   /* read dsaptailor file to get address */
 
   dsa_address = NULLCP;
   if((config_file = fopen(isodefile(tailfile,0), "r")) == (FILE *) NULL) {
     kill_message();
     message((Widget) NULL,
 	    "Cannot open `dsaptailor' file. Attempting to continue.");
   } else {
     while(fgets(Read_in_Stuff, sizeof(Read_in_Stuff), config_file) != NULLCP)
       if ((*Read_in_Stuff != '#') && (*Read_in_Stuff != '\n') )
 	(void) tai_string (Read_in_Stuff);
     
     (void) fclose(config_file);
   }
   
  (void) strcpy(dua_config_dir, isodefile("xd", 0));
  (void) strcpy(dua_help_dir, dua_config_dir);

  (void) strcat(dua_config_dir, "/duaconfig/");
  (void) strcat(dua_help_dir, "/podHelpdir/");

  (void) strcpy (user_path, getenv("HOME"));
  (void) strcpy(read_path, user_path);
  (void) strcpy(type_path, user_path);
  (void) strcpy(friendly_path, user_path);
  (void) strcpy(type_defaults_path, user_path);

  (void) strcat(read_path, config_dir);
  (void) strcat(read_path, readTypes);
  (void) strcat(type_path, config_dir);
  (void) strcat(type_path, type_dir);
  (void) strcat(friendly_path, config_dir);
  (void) strcat(friendly_path, friendlyNames);
  (void) strcat(type_defaults_path, config_dir);
  (void) strcat(type_defaults_path, typeDefaults);
	
  if (testing) {
    (void) strcpy(type_path, "./Xd/duaconfig/");
    (void) strcat(type_path, type_dir);
    if (!(config_directory = opendir(type_path))) {
      (void) fprintf(stderr, 
		"File error! Test pod should be run from source directory!\n");
      quit(1);
    }
  } else {
    if (!(config_directory = opendir(type_path))) {
      (void) strcpy(type_path, dua_config_dir);
      (void) strcat(type_path, type_dir);
      if(!(config_directory = opendir(type_path))) {
	(void) fprintf(stderr, "Can't find directory %s!\n", type_dir);
	quit(1);
      }
    }
  }

  rewinddir(config_directory);
  filt_num = 0;
  while(dir_ent = readdir(config_directory)) {
    if (!(strncmp(dir_ent->d_name, "Type_", 5))) {
      file_names[filt_num] = 
	(char *) malloc((unsigned int) 
			(strlen(dir_ent->d_name) + 
			strlen(type_path) + 2));
      (void) strcpy(file_names[filt_num], type_path);
      (void) strcat(file_names[filt_num], "/");
      (void) strcat(file_names[filt_num], dir_ent->d_name);
      filt_num++;
    }
  }
  (void) closedir(config_directory);

  if (testing) {
    (void) strcpy(options_path, "./Xd/duaconfig/duarc");
    parse_duarc(options_path);
  } else {
    (void) strcpy(options_path, dua_config_dir);
    (void) strcpy(options_path, "duarc");
    parse_duarc(options_path);
  }
 
  (void) strcpy(options_path, user_path);
  (void) strcat(options_path, options);
  parse_duarc(options_path);
 

  if (testing) {
    (void) strcpy(read_path, "./Xd/duaconfig/");
    (void) strcat(read_path, readTypes);
    if (!(config_file = fopen(read_path, "r"))) {
      (void) fprintf(stderr, "File error! Test pod must be run from source directory!\n");
      quit(1);
    }
  } else {
    if (!(config_file = fopen(read_path, "r"))) {
      (void) strcpy(read_path, dua_config_dir);
      (void) strcat(read_path, readTypes);
      if (!(config_file = fopen(read_path, "r"))) {
	(void) fprintf(stderr, "Can't find read file (%s)!\n", read_path);
	quit(1);
      }
    }
  }
  
  while(fgets(Read_in_Stuff, STRINGLEN, config_file) != 0) {
    (void) strcpy(stroid_buf, get_strioid(Read_in_Stuff));
    if (*stroid_buf) {
      if (!read_types)
	read_types = as_comp_new(AttrT_new(stroid_buf), NULLAV, NULLACL_INFO);
      else {
	read_types2 = as_comp_new(AttrT_new(stroid_buf), NULLAV, NULLACL_INFO);
	read_types = as_merge(read_types, read_types2);
      }
    }
  }
  (void) fclose(config_file);

#ifdef USE_PP

  /* If they want PP - they want OR names don't they ! */

      if (!read_types)
	read_types = as_comp_new(AttrT_new("2.6.5.2.6"), 
				 NULLAV, NULLACL_INFO);
      else {
	read_types2 = as_comp_new(AttrT_new("2.6.5.2.6"), 
				  NULLAV, NULLACL_INFO);
	read_types = as_merge(read_types, read_types2);
      }

#endif  

  if (testing) {
    (void) strcpy(friendly_path, "./Xd/duaconfig/");
    (void) strcat(friendly_path, friendlyNames);
    if (!(config_file = fopen(friendly_path, "r"))) {
      (void) fprintf(stderr, "File error! Test pod must be run from source directory!\n");
      quit(1);
    }
  } else {
    if (!(config_file = fopen(friendly_path, "r"))) {
      (void) strcpy(friendly_path, dua_config_dir);
      (void) strcat(friendly_path, friendlyNames);
      if (!(config_file = fopen(friendly_path, "r"))) {
	(void) fprintf(stderr, "Can't find read file (%s)!\n", friendly_path);
	quit(1);
      }
    }
  }

  name_map[fname_num] = 0;
  while(fgets(Read_in_Stuff, STRINGLEN, config_file) != 0) {
    if (*Read_in_Stuff != '#') {
      sptr = str = Read_in_Stuff;
      while (*sptr != ':' && !isspace(*sptr) && sptr != '\0') {
	while (!isalnum(*sptr)) sptr++;
	str = sptr;
	while (*str != ' ' && *str != ',' && *str != ':') str++;
	save = *str;
	*str = '\0';

	if (!name_map[fname_num]) {
	  name_map[fname_num] = (fName) malloc(sizeof(struct friendlyName));
	  name_map[fname_num]->names = 0;
	}

	add_seq(&name_map[fname_num]->names, sptr);
	*str = save;
	sptr = str;
	while (*sptr != ',' && *sptr != ':') sptr++;
      }

      while (!isalpha(*sptr) && *sptr != '\0') sptr++;

      str = sptr;
      while (*str != '\0' && *str != '\n') str++;
      *str = '\0';

      if (name_map[fname_num]) {
	(void) strcpy(name_map[fname_num]->fname, sptr);
	fname_num++;
	name_map[fname_num] = 0;
      }
    }
  }

  for (curr_filt = 0; curr_filt < filt_num; curr_filt++) {
    if (!(config_file = fopen(file_names[curr_filt], "r"))) {
      (void) fprintf(stderr, "Can't find file %s!\n", file_names[curr_filt]);
      quit(1);
    }
    filt_arr[curr_filt] = (filt_struct  *) NULL;
    filtvalue[curr_filt] = (char *) malloc(STRINGLEN);
    *filtvalue[curr_filt] = '\0';
    
    (void) yyparse();
    (void) fclose(config_file);
  }

  filttype[curr_filt] = NULLCP;
  for (count = 0; count < filt_num; count++)
    free(file_names[count]);

  if (testing) {
    (void) strcpy(type_defaults_path, "./Xd/duaconfig/");
    (void) strcat(type_defaults_path, typeDefaults);
    if (!(config_file = fopen(type_defaults_path, "r"))) {
      (void) fprintf(stderr, "File error! Test pod must be run from source directory.\n");
      quit(1);
    }
  } else {
    if (!(config_file = fopen(type_defaults_path, "r"))) {
      (void) strcpy(type_defaults_path, dua_config_dir);
      (void) strcat(type_defaults_path, typeDefaults);
      if (!(config_file = fopen(type_defaults_path, "r"))) {
	(void) fprintf(stderr, "File error! Can't find typeDefaults file.\n");
	quit(1);
      }
    }
  }

  default_num = 0;
  while (fgets (Read_in_Stuff, STRINGLEN, config_file) != 0) {
    p = SkipSpace(Read_in_Stuff);
    if (( *p == '#') || (*p == '\0'))
      continue;
    
    part1 = p;
    if ((part2 = index (p,':')) == NULLCP)
      continue;
    
    end = part2 - 1;
    while (isspace(*end)) end--;
    *++end = '\0';

    *part2++ = '\0';

    while (isspace(*part2)) part2++;
    end = part2;

    while (!isspace(*end) && *end != ',' && *end != ':') end++;

    count = 0;
    while (*part2 != ':') {
      n = 0;
      while (n < filt_num && strncmp(filttype[n], 
				     part2, (int) (end - part2))) n++;
      
      if (n == filt_num) {
	(void) fprintf(stderr, "Parsing error in typeDefaults file!");
	quit(1);
      } else {
	tempints[count] = n;
	count++;
	part2 = end;
	while (!isalpha(*part2) && *part2 != ':' && part2 != '\0') part2++;

	if (*part2 == '\0') {
	  (void) fprintf(stderr, "Parsing error in typeDefaults file!");
	  quit(1);
	} 

	if (*part2 != ':') {
	  while (!isalpha(*part2)) part2++;
	  end = part2;
	  while (!isspace(*end) && *end != ',' && 
		 *end != ':' && *end != '\0') end++;
	  if (*end == '\0') {
	    (void) fprintf(stderr, "Parsing error in typeDefaults file!");
	    quit(1);
	  }
	} else end = part2;
      }
    }

    if (*end == ':') {
      while(isspace(*++end));
      p = end;
      while(!isspace(*++end));
      *end = '\0';

      n = 0;
      while (n < filt_num && strcmp(filttype[n], p)) n++;
      
      if (n == filt_num) {
	(void) fprintf(stderr, "Parsing error in typeDefaults file!");
	quit(1);
      } else {
	num = 0;
	while (num < count && n != tempints[num]) num++;
	if (num == count) {
	  (void) fprintf(stderr, "Parsing error in typeDefaults file!");
	  quit(1);
	}
      }

      defaults[default_num] = n;

      levels[default_num] = malloc((unsigned int) (strlen(part1) + 1));
      (void) strcpy(levels[default_num], part1);
      available_types[default_num] = 
	(int *) malloc((unsigned int) (sizeof(int) * (count+1)));

      for (n = 0; n < count; n++)
	available_types[default_num][n] = tempints[n];
      
      available_types[default_num][n] = -1;
      default_num++;
    }
  }

  (void) fclose(config_file);
}

void parse_duarc(options_path)
     char *options_path;
{
  char *SkipSpace(), *TidyString();
  char *p, *part1, *part2, *part3;
  char Read_in_Stuff[BUFSIZ];
  extern char *myname;

  if ((config_file = fopen (options_path, "r")) == 0);
  else {
    while (fgets (Read_in_Stuff, STRINGLEN, config_file) != 0) {
      p = SkipSpace (Read_in_Stuff);
      if (( *p == '#') || (*p == '\0'))
	continue;  /* ignore comments and blanks */
      
      part1 = p;
      if ((part2 = index (p,':')) == NULLCP) 
	continue; /* ignore it */
      
      *part2++ = '\0';
      part2 = TidyString(part2);
      
      if ((lexequ(part1, "username") == 0) && *namestr == '\0')
	(void) strcpy (namestr, part2);
      else if ((lexequ(part1, "password") == 0) && *passwd == '\0') 
	(void) strcpy (passwd, part2);
      else if (lexequ(part1, "prefergreybook") == 0) {
	if (*part2 != '\0' && lexequ(part2, "TRUE") != 0)
	  mailformat = rfc822;
	else 
	  mailformat = greybook;
      }
      else if (lexequ (part1, "dsap") == 0)
	(void) tai_string (part2);
      else if (lexequ (part1, "isode") == 0) {
	char *split;
	if ((split = index (part2,' ')) != NULLCP) {
	  *split++ = 0;
	  (void) isodesetvar (part2, split, 0);
	}
      } else if (lexequ(part1, "service") == 0) 
	new_service (part2);
      else if (lexequ(part1, "history") == 0) {
	(void) sscanf(part2, "%d", &histlimit);
	if (histlimit < 1) histlimit = 1;
      } else if (lexequ(part1, "readnonleaf") == 0) {
	if (lexequ(part2, "TRUE") == 0) read_all_flag = TRUE;
	else read_all_flag = FALSE;
      } else {
	add_seq(&alias_seq, part1);
	if ((part3 = index (part2 ,':')) != NULLCP) {
	  *part3++ = '\0';
	  part3 = TidyString(part3);
	  add_seq(&password_seq, part3);
	} else {
	  add_seq(&password_seq, "");
	}
	add_seq(&aliased_dn_seq, part2);
      }
    }
    isodexport(myname);
    (void) fclose(config_file);
  }
}

char *cnnct_bind()
{
  struct ds_bind_arg bindarg;
  struct ds_bind_arg bindresult;
  struct ds_bind_error binderr;
  extern char *dsa_address,
              *myname;

  if (namestr[0] != '\0' && passwd[0] == '\0') {
    kill_message();
    message((Widget) NULL,
	    "Enter password at command line!");
    (void) strcpy(passwd, getpassword("Enter user password: "));
    kill_message();
    message((Widget) NULL,
	    "Connecting to Directory. Please wait.");
  }

  /* set dsa_address */
  if (dsa_address == NULLCP)
    dsa_address = myname;

  if (dsa_address == NULLCP || *dsa_address == '\0') {
    return
      ("Cannot find name of DSA to bind to! Click on this window to exit.");
  }
  
  /* now bind */
  bindarg.dba_version = DBA_VERSION_V1988;
  if (passwd[0] == 0) {
    bindarg.dba_passwd_len = 0;
    bindarg.dba_passwd [0] = '\0';
  } else {
    bindarg.dba_passwd_len = strlen(passwd);
    (void) strcpy (bindarg.dba_passwd, passwd);
  }
  
  bindarg.dba_dn = (*namestr == 0? NULLDN: str2dn(namestr));

  if (ds_bind (&bindarg, &binderr, &bindresult) != DS_OK) {
    return(binderr.dbe_type == DBE_TYPE_SECURITY?
	   "Security error - Check name and pasword.\n\
Click on this window to exit.":
	   "Service error - Can't connect to directory!\n\
Click on this window to exit.");
  } else {
    user_name = bindarg.dba_dn;

    if(local_dit && *local_dit)
      (void) strcpy(base_path, local_dit);

#ifndef NO_STATS
    LLOG (log_stat,LLOG_NOTICE,("bound ('%s' to '%s')",namestr,dsa_address));
#endif
#ifndef NO_STATS
    LLOG (log_stat,LLOG_NOTICE,("pod bound to directory"));
#endif
    
    make_friendly(friendly_base_path, base_path);

    set_default_type();
    check_known_oids();
    oclass = as_comp_new(AttrT_new("2.5.4.0"), NULLAV, NULLACL_INFO);
  }
  dn_number = 0;
  back_buf_num = 0;
  backseq = dnseq = NULLDS;
  return NULLCP;
}

void set_default_type()
{
  int count;
  register DN base_name;
  DN d_name;

  if (*base_path != '\0') {
    base_name = d_name = str2dn(base_path);

    while (base_name && base_name->dn_parent)
      base_name = base_name->dn_parent;

    for (count = 0
	 ;
	 count < default_num && base_name && 
	 strcmp(levels[count], base_name->dn_rdn->rdn_at->oa_ot.ot_stroid)
	 ;
	 count++)
      ;

    dn_free(d_name);

    if (count < default_num) {
      av_typeindx = available_types[count];
      typeindx = defaults[count];
    } else {
      av_typeindx = available_types[0];
      typeindx = defaults[0];
    }
  } else {
    for(count = 0; count < default_num && strcmp(levels[count], "@"); count++);
    if (count < default_num) {
      av_typeindx = available_types[count];
      typeindx = defaults[count];
    } else {
      av_typeindx = available_types[0];
      typeindx = defaults[0];
    }
  }
  typetoggled = 0;
}

void dn2buf (ptr,cptr)
     caddr_t ptr;
     char * cptr;
{
  PS ps;
  char buffer [RESBUF];
  
  if((ps = ps_alloc(str_open)) == NULLPS) return ;

  if(str_setup(ps, buffer, RESBUF, 1) == NOTOK) return ;
  
  dn_print(ps, (DN) ptr, EDBOUT);
  *ps->ps_ptr = 0;
  
  ps_free (ps);

  (void) strcpy(cptr, buffer);
}

void make_friendly(fstr, str)
     char *fstr, *str;
{
  char *end, *start;
  char save;
  char buffer[LINESIZE];
  int count, rdn_count = 0;
  str_seq rdns = NULLDS;


  if (str == NULLCP || *str == '\0') {
    *fstr = '\0';
    return;
  }

  end = str;
  while (*end != '\0') end++;

  start = end;
  while (start >= str) {
    if (*start == '@' || start == str) {
      save = *end;
      *end = '\0';

      if (start != str) start++;
      add_seq(&rdns, start);

      *end = save;
      
      end = --start;
      rdn_count++;
    }

    --start;
  }

  *fstr = '\0';
  for (count = 1; count <= rdn_count; count++) {
    start = get_from_seq(count, rdns);

    buffer[0] = '\0';
    make_friendly_aux(buffer, start);

    if (*fstr == '\0')
      (void) strcpy(fstr, buffer);
    else {
      (void) strcat(fstr, ", ");
      (void) strcat(fstr, buffer);
    }
  }

  free_seq(rdns);
}


static void make_friendly_aux(fstr, rdn)
     char *fstr, *rdn;
{
  char *start, *end, *string;
  char save;
  short int mapped;
  int count, seqnum;

  extern char *SkipSpace();


  /* First check the attribute type alias */
  start = end = SkipSpace(rdn);
  while (!isspace(*end) && *end != '=') end++;
  
  save = *end;
  *end = '\0';
    
  mapped = FALSE;
  for (count = 0; count < fname_num && !mapped; count++) {
    seqnum = 0;
    while (!mapped &&
	   (string = get_from_seq(seqnum++, name_map[count]->names)))
      if (!strcmp(string, start)) {
	(void) strcat(fstr, name_map[count]->fname);
	mapped = TRUE;
      }
  }
    
  if (!mapped) {
    (void) strcat(fstr, start);
    (void) strcat(fstr, " = ");
  } else {
    if (*name_map[--count]->fname != '\0') (void) strcat(fstr, " = ");
  }
  
  *end = save;
    
  /* Now get the attribute value*/
  start = end;
  while (isspace(*start) || *start == '=') start++;
    
  end = start;
  while (*end != '@' && *end != '\0' && *end != '%') end++;

  save = *end;
  *end = '\0';

  if (index(start, ',') == NULLCP)
    (void) strcat(fstr, start);
  else {
    (void) strcat(fstr, "\"");
    (void) strcat(fstr, start);
    (void) strcat(fstr, "\"");
  }

  *end = save;

  if (save == '\0' || save == '@') return;
  else if (save == '%')
    {
      (void) strcat(fstr, " (");

      for (start = fstr; *start != '\0'; start++)
	;

      make_friendly_aux(start, ++end);

      (void) strcat(fstr, ")");
    }
}


void make_friendly_rdn(friendly, object, base)
     char *friendly;
     char *object, *base;
{
  register char *front;
  char save;
  int count;

  count = strlen(base);
  front = (char *) (object + count);

  save = *front;
  *front = '\0';

  if (lexequ(object, base)) {
    *front = save;
    make_friendly(friendly, object);
    return;
  }

  *front = save;
  *friendly = '\0';

  front = base;
  count = 0;

  if (showseq != backseq) {
    while (*front != '\0') if (*front++ == '@') count++;
    count++;
  } else
    count = 0;

  front = object;

  while (count && *front != '\0') if (*front++ == '@') count--;

  if (*front != '\0') {
    if (front != object) while (isspace(*front) && *front != '\0') front++;
    make_friendly(friendly, front);
  } else
    make_friendly(friendly, object);
}

goto_addr()
{
  char *str;
  int count = 0;
  void add_to_history();

  set_default_type();

  make_friendly(friendly_base_path, base_path);

  str = get_from_seq(count+1, backseq);
  while (count < back_buf_num && strcmp(str, base_path)){
    count++;
    str = get_from_seq(count+1, backseq);
  }	

  if (count == back_buf_num) {
    add_seq(&backseq, base_path);
    back_buf_num++;
    add_to_history(back_buf_num);
  }
}

clear_dnseq()
{
  free_seq(dnseq);
  dnseq = NULLDS;
  dn_number = 0;
}

int is_dit_leaf(name)
     char *name;
{
  struct ds_list_arg      list_arg;
  struct ds_list_result   list_result;
  struct list_cache       *cached_list;

  struct ds_read_arg      read_arg;
  struct ds_read_result   read_result;
  Entry                   read_entry;
  Attr_Sequence           object_class;

  struct DSError          list_error, read_error;

  char entry_str[STRINGLEN];

  if (get_default_service (&read_arg.rda_common) != 0) return(1);
  read_arg.rda_common.ca_servicecontrol.svc_options = SVC_OPT_PREFERCHAIN;
  read_arg.rda_eis.eis_allattributes = FALSE;
  read_arg.rda_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
  read_arg.rda_eis.eis_select = oclass;

  read_arg.rda_object = (*name? str2dn(name): NULLDN);

  if ((read_entry = local_find_entry(read_arg.rda_object, FALSE))
      != NULLENTRY &&
      read_entry->e_data != E_TYPE_CONSTRUCTOR) {
    object_class = get_sorted_attrs(read_entry->e_attributes,
				    oclass);
    entry2str((caddr_t) object_class, entry_str, STRINGLEN);
    as_free(object_class);
    if (issubstr(entry_str, "NonLeaf")) return(0);
  } else if (ds_read(&read_arg, &read_error, &read_result) == DS_OK) {
    if (read_result.rdr_entry.ent_attr == NULLATTR) return(0);
    entry2str((caddr_t) read_result.rdr_entry.ent_attr, entry_str, STRINGLEN);
    entryinfo_comp_free(&read_result.rdr_entry, 0);
    if (issubstr(entry_str, "NonLeaf")) return(0);
  } else {
    dn_free(read_arg.rda_object);
    ds_error_free(&read_error);
  }

  if (get_default_service (&list_arg.lsa_common) != 0) return(1);
  list_arg.lsa_common.ca_servicecontrol.svc_sizelimit = 1;
  list_arg.lsa_common.ca_servicecontrol.svc_options = SVC_OPT_PREFERCHAIN;
  list_arg.lsa_object = (*name? str2dn(name): NULLDN);

  if ((cached_list = find_list_cache(list_arg.lsa_object, 1)) != NULL) {
    if (cached_list->list_sub_top) return(0);
    else return(1);
  } else if (ds_list (&list_arg, &list_error, &list_result) == DS_OK) {
    cache_list(list_result.lsr_subordinates, 0, list_arg.lsa_object, 1);
    dn_free(list_arg.lsa_object);
    if (list_result.lsr_subordinates) return(0);
    else return(1);
  } else {
    dn_free(list_arg.lsa_object);
    ds_error_free(&list_error);
    return (1);
  }
}

char *GetSurname(name)
     register char *name;
{
  while (*name != '\0') name++;
  while (*name != ' ' && *name != '=') name--;
  return ++name;
}

char *GetWholeRelName(name)
     register char *name;
{
  while (*name!= '\0') name++;
  while (*name != '=') name--;
  while (!isalpha(*name)) name++;
  return name;
}

str_seq SortList(list)
     str_seq list;
{
  register str_seq currEntry, lastSortedEntry , currSortedEntry; 
  str_seq sortedList;
  char *sortedName, *currName;
  register DN curr_dn;
  DN dn;

  if (!list) return 0;

  currEntry = list;
  sortedList = 0;

  while (currEntry) {
    dn = curr_dn = str2dn(currEntry->dname);

    while (curr_dn && curr_dn->dn_parent != NULLDN) 
      curr_dn = curr_dn->dn_parent;
    
    if (!strcmp(curr_dn->dn_rdn->rdn_at->oa_ot.ot_stroid, "2.5.4.3"))
      currName = GetSurname(currEntry->dname);
    else 
      currName = GetWholeRelName(currEntry->dname);

    dn_free(dn);

    if (!sortedList) {
      sortedList = currEntry;
      currEntry = currEntry->next;
      sortedList->next = 0;
    } else {
      lastSortedEntry = 0;
      currSortedEntry = sortedList;

      while (currSortedEntry != 0) {
	dn = curr_dn = str2dn(currSortedEntry->dname);
	
	while (curr_dn && curr_dn->dn_parent != NULLDN) 
	  curr_dn = curr_dn->dn_parent;
	
	if (!strcmp(curr_dn->dn_rdn->rdn_at->oa_ot.ot_stroid, 
		    "2.5.4.3"))
	  sortedName = GetSurname(currSortedEntry->dname);
	else 
	  sortedName = GetWholeRelName(currSortedEntry->dname);
	
	dn_free(dn);

	if (strcmp(currName, sortedName) <= 0) {
	  if (lastSortedEntry) {
	    lastSortedEntry->next = currEntry;
	    currEntry = currEntry->next;
	    lastSortedEntry->next->next = currSortedEntry;
	  } else {
	    sortedList = currEntry;
	    currEntry = currEntry->next;
	    sortedList->next = currSortedEntry;
	  }
	  currSortedEntry = 0;
	} else {
	  lastSortedEntry = currSortedEntry;
	  currSortedEntry = currSortedEntry->next;
	  if (!currSortedEntry) {
	    lastSortedEntry->next = currEntry;
	    currEntry = currEntry->next;
	    lastSortedEntry->next->next = 0;
	    currSortedEntry = 0;
	  }
	}
      }
    }
  }
  return sortedList;
}
