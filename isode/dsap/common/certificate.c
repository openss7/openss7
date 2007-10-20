/* certificate.c - Attribute Syntax for certificates */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/certificate.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/certificate.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: certificate.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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
 */


#include <stdio.h>

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/name.h"
#include "quipu/authen.h"
#include "quipu/syntaxes.h"

PE cert_enc(parm)
struct certificate *parm;
{
PE pe;

  (void) encode_AF_Certificate(&pe, 0, 0, NULLCP, parm);
  return (pe);
}

struct certificate *cert_dec(pe)
PE pe;
{
struct certificate *result;

  if (decode_AF_Certificate(pe, 0, NULLIP, NULLVP, &result) == NOTOK)
	return ((struct certificate *)NULL);
  return (result);
}

alg_cpy(a, b)
struct alg_id *a, *b;
{
  a->algorithm = oid_cpy(b->algorithm);

  if (b->asn)
    a->asn = pe_cpy(b->asn);

  a->p_type = b->p_type;

  if (b->p_type == ALG_PARM_NUMERIC)
    a->un.numeric = b->un.numeric;
}

struct certificate *cert_cpy(parm)
struct certificate *parm;
{
struct certificate *result;

  result = (struct certificate *) calloc(1, sizeof(struct certificate));

  alg_cpy(&(result->sig.alg), &(parm->sig.alg));
  result->sig.n_bits = parm->sig.n_bits;
  result->sig.encrypted = malloc((unsigned)(parm->sig.n_bits+7)/8);
  bcopy(parm->sig.encrypted, result->sig.encrypted, (parm->sig.n_bits+7)/8);

  alg_cpy(&(result->alg), &(parm->alg));
  alg_cpy(&(result->key.alg), &(parm->key.alg));
  result->serial = parm->serial;
  result->version = parm->version;
  result->issuer = dn_cpy(parm->issuer);
  result->subject = dn_cpy(parm->subject);
  result->valid.not_before =
	strdup(parm->valid.not_before);
  result->valid.not_after = 
	strdup(parm->valid.not_after); 
  result->key.n_bits = parm->key.n_bits;
  result->key.value = malloc((unsigned)(parm->key.n_bits+7)/8);
  bcopy(parm->key.value, result->key.value, 
	(parm->key.n_bits+7)/8);

  return (result);
}

cert_free(parm)
struct certificate *parm;
{
  dn_free(parm->issuer);
  dn_free(parm->subject); 

  if (parm->sig.alg.algorithm)
      oid_free (parm->sig.alg.algorithm);
  parm->sig.alg.algorithm = NULLOID;

  if (parm->alg.algorithm)
      oid_free (parm->alg.algorithm);
  parm->alg.algorithm = NULLOID;

  if (parm->key.alg.algorithm)
      oid_free (parm->key.alg.algorithm);
  parm->key.alg.algorithm = NULLOID;

  if (parm->sig.encrypted)
      free (parm->sig.encrypted);
  parm->sig.encrypted = NULLCP;

  if (parm->key.value)
      free (parm->key.value);
  parm->key.value = NULLCP;

  if (parm->valid.not_before)
      free (parm->valid.not_before);
  parm->valid.not_before = NULLCP;

  if (parm->valid.not_after)
      free (parm->valid.not_after);
  parm->valid.not_after = NULLCP;

  if (parm->key.alg.p_type == ALG_PARM_UNKNOWN)
      pe_free (parm->key.alg.asn);
  parm->key.alg.asn = NULLPE;

  free((char *) parm);
}

str2alg(str, alg)
char *str;
struct alg_id *alg;
{
PE asn2pe();

  if ((str == NULLCP) || (*str == '\0'))
   {
     alg->asn = NULLPE;
     alg->p_type = ALG_PARM_ABSENT;
   }
  else if (strncmp(str,"{ASN}", 5) == 0)
    {
      alg->asn = asn2pe((char*)str+5);
      alg->p_type = ALG_PARM_UNKNOWN;
    }
  else 
    {
      alg->asn=NULLPE;
      alg->p_type = ALG_PARM_NUMERIC;
      alg->un.numeric = atoi(str);
    }
}

str2encrypted(str, cp, len)
char *str;
char **cp;
int *len;
{
int i;
int l;
int k = 0;
int tmp;

  l=strlen(str);
  if (str[l-1] == '#') l--;
  if ((l>2) && str[l-2] == '-')
  {
    k = atoi(&(str[l-1]));
    l = l-2;
  }
  *cp = malloc((unsigned)(l+1)/2);
  *len = 8*((l+1)/2) - k;
  for (i=0;i<(l+1)/2;i++)
  {
   (void) sscanf(str+2*i, "%02x", &tmp);
   (*cp)[i] = tmp & 255;
  }
}


struct certificate *str2cert(str)
char *str;
{
struct certificate *result;
char *ptr;
OID oid;

  result = (struct certificate *) calloc(1, sizeof(*result));

  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Algorithm not present (SIGNED Value)",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  oid = name2oid(SkipSpace(str));
  if (oid == NULLOID)
  {
    parse_error("Bad algorithm identifier (SIGNED Value)",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }

  result->sig.alg.algorithm = oid;

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Parameters not present (SIGNED Value)",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  str2alg(str, &(result->sig.alg));

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Signature not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  str2encrypted(str, &(result->sig.encrypted), &(result->sig.n_bits));

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Issuer not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->issuer = str2dn(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Subject not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->subject = str2dn(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Algorithm not present (\"signature\")",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  oid = name2oid(SkipSpace(str));
  if (oid == NULLOID)
  {
    parse_error("Bad algorithm identifier (\"signature\")",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }

  result->alg.algorithm = oid;

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Parameters not present (\"signature\")",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  str2alg(str, &(result->alg));

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Version Number not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->version = atoi(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Serial Number not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->serial = atoi(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("Start time not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->valid.not_before = strdup(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    parse_error("End time not present",NULLCP);
    cert_free(result); 
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  result->valid.not_after = strdup(str);

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    free((char*)result);
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  oid = name2oid(SkipSpace(str));
  if (oid == NULLOID)
  {
    free((char*)result);
    return (struct certificate *) 0;
  }

  result->key.alg.algorithm = oid;

  str = ptr;
  ptr = index(str, '#');
  if (ptr == NULLCP)
  {
    free((char*)result);
    return (struct certificate *) 0;
  }
  *ptr = '\0';
  ptr++;

  str2alg(str, &(result->key.alg));

  str = ptr;

  str2encrypted(str, &(result->key.value), &(result->key.n_bits));

  return (result);
}

print_algid(ps, parm, format)
PS ps;
struct alg_id *parm;
int format;
{
  ps_printf(ps, "%s#", oid2name (parm->algorithm, OIDPART));

  switch(parm->p_type) {
     case ALG_PARM_ABSENT:
       if(parm->asn != NULLPE)
             pe_print(ps, parm->asn, format);
       ps_printf(ps, "#\\\n");
       break;
     case ALG_PARM_NUMERIC:
       if (format == READOUT)
         ps_printf(ps, "%d#", parm->un.numeric);
       else
         ps_printf(ps, "%d#", parm->un.numeric);
       break;
      default:
       if (format == READOUT)
       {
         if ((parm->asn->pe_class == PE_CLASS_UNIV)
           &&(parm->asn->pe_form  == PE_FORM_PRIM)
           &&(parm->asn->pe_id    == PE_PRIM_INT))
           ps_printf(ps, "%d", prim2num(parm->asn));
         else
         {
           vpushquipu (ps);
           vunknown(parm->asn);
	   vpopquipu ();
         }
       }
       else
       {
	/* This routine will print a {ASN} prefix */
         pe_print(ps, parm->asn, format);
       }
       ps_printf(ps, "#\\\n");
   }
}

print_encrypted(ps, str, n_bits, format)
PS ps;
char *str;
int n_bits;
int format;
{
int i;

/* The end-user doesn't care what the signature is, so don't display it */

  if (format != READOUT)
  {
    for (i=0;i<(n_bits+7)/8;i++) {
      ps_printf(ps, "%02x", str[i] & 255);
      if ( ((i % EDB_LINEWRAP) == 0) && i)
	      ps_print(ps, "\\\n");
    }
    if ((i = (n_bits % 8)) != 0)
      ps_printf(ps, "-%d", 8-i);

    ps_printf(ps, "#");
  }

}


printcert(ps, parm, format)
PS ps;
struct certificate *parm;
int format;
{
  print_algid(ps, &(parm->sig.alg), format);
  print_encrypted(ps, parm->sig.encrypted, parm->sig.n_bits, format);

  dn_print(ps, parm->issuer, EDBOUT);
  ps_printf(ps, "#\\\n");
  dn_print(ps, parm->subject, EDBOUT);
  ps_printf(ps, "#\\\n");
  print_algid(ps, &(parm->alg), format);
  ps_printf(ps, "%d#", parm->version);
  ps_printf(ps, "%d#", parm->serial);

  utcprint(ps, parm->valid.not_before, format);
  ps_printf(ps, "#");
  utcprint(ps, parm->valid.not_after, format);
  ps_printf(ps, "#\\\n");

  print_algid(ps, &(parm->key.alg), format);
  print_encrypted(ps, parm->key.value, 
	parm->key.n_bits, format);
}

int cert_cmp(a, b)
struct certificate *a, *b;
{
int ret;

  ret = dn_cmp(a->issuer, b->issuer);
  if (ret != 0)
    return (ret);

  ret = dn_cmp(a->subject, b->subject);
  if (ret != 0)
    return (ret);

  if (a->version > b->version)
    return (1);
  if (a->version < b->version)
    return (-1);

  if (a->serial > b->serial)
    return (1);
  if (a->serial < b->serial)
    return (-1);

  /* issuer, subject, version and serial should uniquely identify the
   * certificate.
   */

  return (0);
}

certificate_syntax()
{
  (void) add_attribute_syntax(
	"Certificate",
	(IFP) cert_enc, (IFP) cert_dec,
	(IFP) str2cert, (IFP) printcert,
	(IFP) cert_cpy, (IFP) cert_cmp,
	cert_free,	NULLCP,
	NULLIFP,	TRUE);
}
