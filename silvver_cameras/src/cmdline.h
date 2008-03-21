/* cmdline.h */

/* File autogenerated by gengetopt version 2.19.1  */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE PACKAGE
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION VERSION
#endif

struct gengetopt_args_info
{
  const char *help_help; /* Print help and exit help description.  */
  const char *version_help; /* Print version and exit help description.  */
  char * ip_servidor_arg;	/* O endereço ip do silvver-servidor (default='127.0.0.1').  */
  char * ip_servidor_orig;	/* O endereço ip do silvver-servidor original value given at command line.  */
  const char *ip_servidor_help; /* O endereço ip do silvver-servidor help description.  */
  int porta_recepcionista_arg;	/* A porta onde o recepcionista do servidor está ligado (default='12000').  */
  char * porta_recepcionista_orig;	/* A porta onde o recepcionista do servidor está ligado original value given at command line.  */
  const char *porta_recepcionista_help; /* A porta onde o recepcionista do servidor está ligado help description.  */
  char * cameras_config_arg;	/* Arquivo XML com a configuração das câmeras. (default='cameras.xml').  */
  char * cameras_config_orig;	/* Arquivo XML com a configuração das câmeras. original value given at command line.  */
  const char *cameras_config_help; /* Arquivo XML com a configuração das câmeras. help description.  */
  
  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int ip_servidor_given ;	/* Whether ip-servidor was given.  */
  int porta_recepcionista_given ;	/* Whether porta-recepcionista was given.  */
  int cameras_config_given ;	/* Whether cameras-config was given.  */

} ;

extern const char *gengetopt_args_info_purpose;
extern const char *gengetopt_args_info_usage;
extern const char *gengetopt_args_info_help[];

int cmdline_parser (int argc, char * const *argv,
  struct gengetopt_args_info *args_info);
int cmdline_parser2 (int argc, char * const *argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

void cmdline_parser_init (struct gengetopt_args_info *args_info);
void cmdline_parser_free (struct gengetopt_args_info *args_info);

int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
